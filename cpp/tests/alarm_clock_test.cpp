#include <AlarmClock.h>
#include <climits>

#include "doctest.h"
#include "ApprovalTests.hpp"

TEST_CASE ("no alarms are set") {

    unsigned int now_sec = 100; // 100 seconds since the epoch
    unsigned long min_value_ms = INT_MAX;

    auto *config = new alarm_config();
    config->timers = new timers();
    config->timers->duration = new duration();

    how_long_until_the_next_alarm(config, now_sec, &min_value_ms);
    
    std::ostringstream result;
    result << "no alarms set: " << min_value_ms;
    std::string to_verify = result.str();

    Approvals::verify(to_verify);
}
