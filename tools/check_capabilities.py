#!/usr/bin/env python3
"""
Check that docs/CAPABILITY_MODEL.json and the C++ API have not drifted apart.

Verifies:
  * capability ids are unique and every field Melo Blocks needs is present
  * every codegen template calls a real MeloClass method with the right number
    of arguments, and its {placeholders} match the capability's inputs
  * every public MeloClass method is either a capability or in "systemApi"
    (so nothing can be added to the SDK without Blocks knowing)
  * inputs have sane ranges and defaults inside those ranges
  * error codes match the MeloStatus enum in src/MeloTypes.h, and every error
    named by a capability exists

Needs only the Python standard library. Exit code 0 = OK, 1 = problems found.
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
errors = []


def fail(msg):
    errors.append(msg)


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def public_methods(header):
    """Names of the public methods of MeloClass, with their parameter counts."""
    src = strip_comments(header.read_text(encoding="utf8"))
    m = re.search(r"class\s+MeloClass\s*\{(.*?)\n\};", src, flags=re.S)
    if not m:
        fail("could not find class MeloClass in src/Melo.h")
        return {}
    body = m.group(1).split("private:")[0]
    # an inline body { ... } ends its declaration just like ';' does
    body = re.sub(r"\{[^{}]*\}", ";", body)
    methods = {}
    for stmt in body.split(";"):
        stmt = " ".join(stmt.split())
        decl = re.match(r"^public:\s*", stmt)
        if decl:
            stmt = stmt[decl.end():]
        sig = re.match(r"^(?:[\w:<>&*]+\s+)+(\w+)\s*\((.*)\)\s*(?:const)?$", stmt)
        if not sig:
            continue  # constructor, blank, etc.
        name, params = sig.group(1), sig.group(2).strip()
        total = 0 if params in ("", "void") else params.count(",") + 1
        required = 0 if total == 0 else sum(1 for p in params.split(",") if "=" not in p)
        methods[name] = (required, total)
    return methods


def status_enum(header):
    src = strip_comments(header.read_text(encoding="utf8"))
    m = re.search(r"enum\s+MeloStatus\s*\{(.*?)\};", src, flags=re.S)
    if not m:
        fail("could not find enum MeloStatus in src/MeloTypes.h")
        return {}
    return {n: int(v) for n, v in re.findall(r"(\w+)\s*=\s*(\d+)", m.group(1))}


def main():
    model = json.loads((ROOT / "docs" / "CAPABILITY_MODEL.json").read_text(encoding="utf8"))["meloCapabilityModel"]
    methods = public_methods(ROOT / "src" / "Melo.h")
    enum = status_enum(ROOT / "src" / "MeloTypes.h")

    # error codes
    model_codes = {e["id"]: e["value"] for e in model["errorCodes"]}
    if model_codes != enum:
        fail(f"errorCodes differ from MeloStatus: json={model_codes} header={enum}")

    seen_ids, used_methods = set(), set()
    for cap in model["capabilities"]:
        cid = cap.get("id", "<missing id>")
        if cid in seen_ids:
            fail(f"{cid}: duplicate id")
        seen_ids.add(cid)

        for field in ("category", "description", "arduinoApi", "codegen", "blockLabel", "inputs", "outputs", "blocking", "errors"):
            if field not in cap:
                fail(f"{cid}: missing field '{field}'")
        if not cap.get("blockLabel", "").strip():
            fail(f"{cid}: empty blockLabel")

        inputs = cap.get("inputs", [])
        names = [i["name"] for i in inputs]
        if len(set(names)) != len(names):
            fail(f"{cid}: duplicate input names")

        placeholders = re.findall(r"\{(\w+)\}", cap.get("codegen", ""))
        if sorted(placeholders) != sorted(names):
            fail(f"{cid}: codegen placeholders {placeholders} do not match inputs {names}")

        for i in inputs:
            if i["type"] in ("uint8", "int8", "uint16", "uint32"):
                if not (isinstance(i.get("min"), int) and isinstance(i.get("max"), int) and i["min"] <= i["max"]):
                    fail(f"{cid}.{i['name']}: needs integer min <= max")
                elif "default" in i and not (i["min"] <= i["default"] <= i["max"]):
                    fail(f"{cid}.{i['name']}: default {i['default']} outside {i['min']}..{i['max']}")
            elif i["type"] == "enum":
                if not i.get("values"):
                    fail(f"{cid}.{i['name']}: enum without values")
            elif i["type"] != "bool":
                fail(f"{cid}.{i['name']}: unknown type '{i['type']}'")

        for e in cap.get("errors", []):
            if e not in enum:
                fail(f"{cid}: unknown error '{e}'")

        if cap.get("external"):
            continue  # plain Arduino call such as delay(), not part of MeloClass
        call = re.search(r"Melo\.(\w+)\s*\(", cap.get("codegen", ""))
        if not call:
            fail(f"{cid}: codegen does not call a Melo method")
            continue
        name = call.group(1)
        used_methods.add(name)
        if name not in methods:
            fail(f"{cid}: Melo.{name}() does not exist in src/Melo.h")
            continue
        required, total = methods[name]
        n = len(inputs)
        if not (required <= n <= total):
            fail(f"{cid}: Melo.{name}() takes {required}..{total} arguments but the capability has {n} inputs")

    system = set(model.get("systemApi", []))
    for name in sorted(set(methods) - used_methods - system):
        fail(f"Melo.{name}() is public but is neither a capability nor in systemApi")
    for name in sorted(system - set(methods)):
        fail(f"systemApi lists {name}() which does not exist in src/Melo.h")
    for name in sorted(system & used_methods):
        fail(f"{name}() is both a capability and in systemApi")

    if errors:
        print("Capability model check FAILED:")
        for e in errors:
            print("  -", e)
        return 1
    print(f"Capability model OK: {len(seen_ids)} capabilities, {len(system)} system methods, {len(enum)} status codes.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
