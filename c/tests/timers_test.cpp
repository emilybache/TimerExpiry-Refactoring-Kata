#include <climits>

#include "catch2/catch.hpp"
#include "ApprovalTests.hpp"
#include <string>

extern "C"
{
#include "timers.h"
}

TEST_CASE ("how_long_until_next_timer_expiry") {
    // seconds since the epoch
    unsigned int now_sec = 100;
    // in-parameter for when the next timer will expire, in milliseconds after now_sec
    unsigned long min_value_ms = LONG_MAX;

    // the configuration of all the various timers
    auto *timerConfig = new timer_config();
    timerConfig->timers = new timers();
    timerConfig->timers->duration = new duration();

    SECTION("no timers set") {
        how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);

        // no timers are set, so min_value_ms is not updated and remains at the value we set earlier
        REQUIRE(min_value_ms == LONG_MAX);
    }

    SECTION("duration measurement active")
    {
        set_duration_meas_active(timerConfig, true);
        set_duration_meas(timerConfig, 1);
        set_duration_meas_start(timerConfig, now_sec - 1);
        set_last_pkt_time(timerConfig, now_sec);
        SECTION("idt alarm") {
            set_idt_alarm_time(timerConfig, 3);
            how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 3000);
        }
        SECTION("P88N timer") {
            add_reporting_flag(timerConfig, REPORTING_TRIGGERS_P88N);
            set_time_threshold(timerConfig, 10);
            how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 8000);
        }
        SECTION("Time Quota timer") {
            add_operational_flag(timerConfig, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
            set_time_quota(timerConfig, 5);
            how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 3000);
        }
    }

    SECTION("ZB12 timer") {
        add_reporting_flag(timerConfig, REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(timerConfig, 6);
        clear_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
        set_last_pkt_time(timerConfig, now_sec);
        SECTION("ZB12 not modified"){
            how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 6000);
            REQUIRE(!get_operational_flag_state(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));
        }
        SECTION("ZB12 modified") {
            add_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
            how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 6000);
            REQUIRE(!get_operational_flag_state(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));

        }

    }
    SECTION("DY9X timer") {
        add_reporting_flag(timerConfig, REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(timerConfig, 5);
        set_periodic_meas_start(timerConfig, now_sec - 2);
        how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 3000);
    }
    SECTION("Monitoring time timer") {
        set_monitoring_time_ts(timerConfig, 7);
        set_monitoring_time_start(timerConfig, now_sec - 4);
        how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 3000);
    }

    SECTION("All timers set") {
        set_duration_meas_active(timerConfig, true);
        set_duration_meas_start(timerConfig, now_sec - 1);
        set_last_pkt_time(timerConfig, now_sec);
        set_idt_alarm_time(timerConfig, 30);

        add_reporting_flag(timerConfig, REPORTING_TRIGGERS_P88N);
        set_time_threshold(timerConfig, 26);

        add_operational_flag(timerConfig, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
        set_time_quota(timerConfig, 21);

        add_reporting_flag(timerConfig, REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(timerConfig, 17);
        clear_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
        add_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);

        add_reporting_flag(timerConfig, REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(timerConfig, 15);
        set_periodic_meas_start(timerConfig, now_sec - 2);

        set_monitoring_time_ts(timerConfig, 7);
        set_monitoring_time_start(timerConfig, now_sec - 4);

        how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 3000);
    }
}

