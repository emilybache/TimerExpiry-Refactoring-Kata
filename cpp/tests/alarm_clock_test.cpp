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

    SECTION("duration measurement active")
    {
        set_duration_meas_active(alarmConfig, true);
        set_duration_meas(alarmConfig, 1);
        set_duration_meas_start(alarmConfig, now_sec - 1);
        set_last_pkt_time(alarmConfig, now_sec);
        SECTION("idt alarm") {
            set_idt_alarm_time(alarmConfig, 3);
            how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 3000);
        }
        SECTION("P88N alarm") {
            add_reporting_flag(alarmConfig, ZJ77_REPORTING_TRIGGERS_P88N);
            set_time_threshold(alarmConfig, 10);
            how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 8000);
        }
        SECTION("Time Quota alarm") {
            add_operational_flag(alarmConfig, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
            set_time_quota(alarmConfig, 5);
            how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 3000);
        }
    }

    SECTION("ZB12") {
        add_reporting_flag(alarmConfig, ZJ77_REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(alarmConfig, 6);
        clear_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
        set_last_pkt_time(alarmConfig, now_sec);
        SECTION("ZB12 not modified"){
            how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 6000);
            REQUIRE(!get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));
        }
        SECTION("ZB12 modified") {
            add_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
            how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == 6000);
            REQUIRE(!get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));

        }

    }
    SECTION("DY9X alarm") {
        add_reporting_flag(alarmConfig, ZJ77_REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(alarmConfig, 5);
        set_periodig_meas_start(alarmConfig, now_sec-2);
        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 3000);
    }
    SECTION("Monitoring time TS") {
        set_monitoring_time_ts(alarmConfig, 7);
        set_monitoring_time_start(alarmConfig, now_sec - 4);
        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 3000);
    }

    SECTION("All alarms set") {
        set_duration_meas_active(alarmConfig, true);
        set_duration_meas_start(alarmConfig, now_sec - 1);
        set_last_pkt_time(alarmConfig, now_sec);
        set_idt_alarm_time(alarmConfig, 30);

        add_reporting_flag(alarmConfig, ZJ77_REPORTING_TRIGGERS_P88N);
        set_time_threshold(alarmConfig, 26);

        add_operational_flag(alarmConfig, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
        set_time_quota(alarmConfig, 21);

        add_reporting_flag(alarmConfig, ZJ77_REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(alarmConfig, 17);
        clear_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
        add_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);

        add_reporting_flag(alarmConfig, ZJ77_REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(alarmConfig, 15);
        set_periodig_meas_start(alarmConfig, now_sec-2);

        set_monitoring_time_ts(alarmConfig, 7);
        set_monitoring_time_start(alarmConfig, now_sec - 4);

        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 3000);
    }
}

