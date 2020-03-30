#include <climits>

#include "catch2/catch.hpp"
#include "ApprovalTests.hpp"
#include <string>

extern "C"
{
#include "../alarm_clock.h"
}

TEST_CASE("ZB12 alarm") {
    unsigned int now_sec = 100; // seconds since the epoch
    unsigned long min_value_ms = LONG_MAX; // when the next timer should go off, in milliseconds after now_sec

    auto *alarmConfig = new alarm_config();
    alarmConfig->timers = new timers();
    alarmConfig->timers->duration = new duration();

    add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_ZB12);
    set_quota_holding_time(alarmConfig, 6);
    clear_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
    set_last_pkt_time(alarmConfig, now_sec);
    SECTION("ZB12 not modified") {
        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 6000);
        REQUIRE(!get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));
    }SECTION("ZB12 modified") {
        add_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
        how_long_until_the_next_alarm(alarmConfig, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == 6000);
        REQUIRE(!get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));

    }


}

TEST_CASE ("calculate_earliest_alarm") {

    unsigned int now_sec = 100; // seconds since the epoch
    unsigned long min_value_ms = LONG_MAX; // when the next timer should go off, in milliseconds after now_sec

    auto *alarmConfig = new alarm_config();
    alarmConfig->timers = new timers();
    alarmConfig->timers->duration = new duration();

    SECTION("no alarms set") {

        REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == INT_MAX);
    }

    SECTION("duration measurement active") {
        set_duration_meas_active(alarmConfig, true);
        set_duration_meas(alarmConfig, 1);
        set_duration_meas_start(alarmConfig, now_sec - 1);
        set_last_pkt_time(alarmConfig, now_sec);
        SECTION("idt alarm") {
            set_idt_alarm_time(alarmConfig, 3);
            REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 3);
        }SECTION("P88N alarm") {
            add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_P88N);
            set_time_threshold(alarmConfig, 10);
            REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 8);
        }SECTION("Time Quota alarm") {
            add_operational_flag(alarmConfig, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
            set_time_quota(alarmConfig, 5);
            REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 3);
        }
    }

    SECTION("ZB12") {
        add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(alarmConfig, 6);
        clear_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
        set_last_pkt_time(alarmConfig, now_sec);
        SECTION("ZB12 not modified") {
            REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 6);
        }SECTION("ZB12 modified") {
            add_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
            REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 6);
        }

    }SECTION("DY9X alarm") {
        add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(alarmConfig, 5);
        set_periodic_meas_start(alarmConfig, now_sec - 2);
        REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 3);
    }SECTION("Monitoring time TS") {
        set_monitoring_time_ts(alarmConfig, 7);
        set_monitoring_time_start(alarmConfig, now_sec - 4);
        REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 3);
    }SECTION("BTI alarm") {
        add_operational_flag(alarmConfig, OPERATIONAL_FLAG_BTI_PRESENT);
        set_bti_time_interval(alarmConfig, 5);
        set_last_pkt_time(alarmConfig, now_sec - 2);
        REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 3);
    }

    SECTION("All alarms set") {
        set_duration_meas_active(alarmConfig, true);
        set_duration_meas_start(alarmConfig, now_sec - 1);
        set_last_pkt_time(alarmConfig, now_sec);
        set_idt_alarm_time(alarmConfig, 30);

        add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_P88N);
        set_time_threshold(alarmConfig, 26);

        add_operational_flag(alarmConfig, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
        set_time_quota(alarmConfig, 21);

        add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(alarmConfig, 17);
        clear_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
        add_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);

        add_reporting_flag(alarmConfig, REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(alarmConfig, 15);
        set_periodic_meas_start(alarmConfig, now_sec - 2);

        set_monitoring_time_ts(alarmConfig, 7);
        set_monitoring_time_start(alarmConfig, now_sec - 4);

        REQUIRE(calculate_earliest_alarm(alarmConfig, now_sec) == 3);
    }
}

