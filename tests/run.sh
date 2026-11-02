#!/bin/sh
# Build and run the host unit tests. Needs only a C++17 compiler.
set -e
cd "$(dirname "$0")"
${CXX:-c++} -std=c++17 -Wall -Wextra -Werror test_melo_core.cpp -o runner
./runner
