#include <AlarmClock.h>
#include <climits>

#include "doctest.h"
#include "ApprovalTests.hpp"

TEST_CASE ("test_alarm_clock") {

    unsigned int now_sec = 100; // 100 seconds since the epoch
    unsigned int* min_value_ms = INT_MAX;

    auto config = new alarm_config();
    config->last_pkt = now_sec;
    how_long_until_the_next_alarm(config, now_sec, min_value_ms);

    Approvals::verify(min_value_ms);
}
