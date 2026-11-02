/**
 * LineFollowLogic.h
 *
 * Single-IR-sensor line following as a pure state machine (no Arduino
 * dependencies, unit tested on the host).
 *
 * With one on/off sensor the robot can only know "on the line" or "off the
 * line", so it follows the EDGE of the line: arc one way while on the line,
 * the other way while off it. If it sees no line for MELO_LINE_LOST_MS it
 * sweeps left and right with growing sweeps, and gives up after
 * MELO_LINE_SEARCH_MAX_MS.
 *
 * All timings are untuned guesses; tune them on the real track.
 *
 * SDK Version: 0.2.0
 */

#ifndef MELO_LINE_FOLLOW_LOGIC_H
#define MELO_LINE_FOLLOW_LOGIC_H

#include <stdint.h>
#include "../MeloConfig.h"

struct MeloDriveCmd {
    int8_t left;    // -100..100, positive = forward
    int8_t right;
};

enum MeloLineState : uint8_t {
    LINE_FOLLOWING = 0,
    LINE_SEARCHING = 1,
    LINE_GAVE_UP   = 2
};

class MeloLineFollowLogic {
public:
    MeloLineFollowLogic() { reset(); }

    void reset() {
        _state = LINE_FOLLOWING;
        _started = false;
        _lastLineMs = _searchStartMs = _sweepStartMs = 0;
        _sweep = 0;
    }

    MeloLineState state() const { return _state; }

    /**
     * Decide the wheel speeds for one control step.
     *
     * @param onLine  the sensor currently sees the line
     * @param nowMs   current time in ms (millis(); wrap-around safe)
     * @param speed   cruising speed 0..100
     */
    MeloDriveCmd step(bool onLine, uint32_t nowMs, uint8_t speed) {
        if (speed > 100) speed = 100;

        if (!_started) {
            _started = true;
            _lastLineMs = nowMs;
        }
        if (_state == LINE_GAVE_UP) return MeloDriveCmd{0, 0};

        if (onLine) {
            _lastLineMs = nowMs;
            _state = LINE_FOLLOWING;
        }

        if (_state == LINE_FOLLOWING) {
            if (!onLine && (uint32_t)(nowMs - _lastLineMs) >= MELO_LINE_LOST_MS) {
                _state = LINE_SEARCHING;
                _searchStartMs = _sweepStartMs = nowMs;
                _sweep = 0;
            } else {
                int8_t inner = (int8_t)((int)speed * MELO_LINE_INNER_PCT / 100);
                // On the line: arc left. Off the line: arc right.
                return onLine ? MeloDriveCmd{inner, (int8_t)speed}
                              : MeloDriveCmd{(int8_t)speed, inner};
            }
        }

        // LINE_SEARCHING: pivot on the spot, each sweep longer than the last,
        // starting left and alternating.
        if ((uint32_t)(nowMs - _searchStartMs) >= MELO_LINE_SEARCH_MAX_MS) {
            _state = LINE_GAVE_UP;
            return MeloDriveCmd{0, 0};
        }
        uint32_t sweepLen = (uint32_t)MELO_LINE_SEARCH_STEP_MS * (_sweep + 1);
        if ((uint32_t)(nowMs - _sweepStartMs) >= sweepLen) {
            _sweep++;
            _sweepStartMs = nowMs;
        }
        uint8_t s = speed < MELO_LINE_SEARCH_SPEED_PCT ? speed : MELO_LINE_SEARCH_SPEED_PCT;
        bool pivotLeft = (_sweep % 2) == 0;
        return pivotLeft ? MeloDriveCmd{(int8_t)-s, (int8_t)s}
                         : MeloDriveCmd{(int8_t)s, (int8_t)-s};
    }

private:
    MeloLineState _state;
    bool          _started;
    uint32_t      _lastLineMs;
    uint32_t      _searchStartMs;
    uint32_t      _sweepStartMs;
    uint16_t      _sweep;
};

#endif // MELO_LINE_FOLLOW_LOGIC_H
