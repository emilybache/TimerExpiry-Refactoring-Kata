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

    SECTION("no alarms set") {
        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);

        // no timers are set, so min_value_ms is not updated and remains at the value we set earlier
        REQUIRE(min_value_ms == LONG_MAX);
    }
    SECTION("duration measurments set")
    {
        set_duration_meas_active(alarmConfig, true);
        set_duration_meas_start(alarmConfig, now_sec-2);
        set_duration_meas(alarmConfig, 3);
        set_last_pkt_time(alarmConfig, now_sec);

        SECTION("idt alarm set") {
            set_idt_alarm_time(alarmConfig, 3);

            how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 3000);
        }
        SECTION("P88N alarm set") {
            add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_P88N);
            set_time_threshold(alarmConfig, 10);
            set_duration_meas_threshold(alarmConfig, 1);

            how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 4000);
        }
    }

    SECTION("zb12 alarm set OPERATIONAL_FLAG_ZB12_MODIFIED false") {
        add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(alarmConfig, 2);

        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 2000);
    }

    SECTION("zb12 alarm set OPERATIONAL_FLAG_ZB12_MODIFIED true") {
        add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(alarmConfig, 2);
        add_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
        set_last_pkt_time(alarmConfig, now_sec - 1);

        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == LONG_MAX);
    }
}

