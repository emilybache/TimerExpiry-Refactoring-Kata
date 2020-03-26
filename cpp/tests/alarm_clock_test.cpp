#include <climits>

#include "catch2/catch.hpp"
#include "ApprovalTests.hpp"
#include <string>

extern "C"
{
#include "../alarm_clock.h"
}

TEST_CASE ("how_long_until_the_next_alarm") {
    
    unsigned int now_sec = 100; // seconds since the epoch
    unsigned long min_value_ms = LONG_MAX; // when the next timer should go off, in milliseconds after now_sec

    auto *alarmConfig = new alarm_config();
    alarmConfig->timers = new timers();
    alarmConfig->timers->duration = new duration();

    how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);

    // no timers are set, so min_value_ms is not updated and remains at the value we set earlier
    REQUIRE(min_value_ms == LONG_MAX);
}

