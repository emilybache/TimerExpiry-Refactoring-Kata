#include <climits>

#include "catch2/catch.hpp"
#include "ApprovalTests.hpp"
#include <string>

extern "C"
{
#include "../alarm_clock.h"
}


TEST_CASE ("how_long_until_the_next_alarm") {

    unsigned int now_sec = 100; // 100 seconds since the epoch
    unsigned long min_value_ms = LONG_MAX;

    auto *config = new alarm_config();
    config->timers = new timers();
    config->timers->duration = new duration();

    SECTION("no alarms set")
    {
        how_long_until_the_next_alarm(config, now_sec, &min_value_ms);
        REQUIRE(min_value_ms == LONG_MAX);
    }
    SECTION("duration measurement active") {
        set_duration_meas_active(config, true);
        set_duration_meas_start(config, now_sec);
        set_duration_meas(config, 1);
        SECTION("idt_alarm")
        {
            set_idt_alarm_time(config, now_sec + 90);
            set_last_pkt_time(config, now_sec - 25);

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 165000);
        }

        SECTION("P88N")
        {
            add_reporting_flag(config, ZJ77_REPORTING_TRIGGERS_P88N);
            set_time_threshold(config, 10);
            set_duration_meas_threshold(config, 3);

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 6000);
        }

        SECTION("TIME_QUOTA")
        {
            add_operational_flag(config, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
            set_time_quota(config, 10);

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 9000);
        }

    }
    SECTION("ZB12")
    {
        add_reporting_flag(config, ZJ77_REPORTING_TRIGGERS_ZB12);
        add_operational_flag(config, OPERATIONAL_FLAG_ZB12_MODIFIED);
        set_last_pkt_time(config, now_sec - 3);
        set_quota_holding_time(config, 10);

        how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

        REQUIRE(min_value_ms == 10000);
    }
    SECTION("ZB12 unmodified")
    {
        add_reporting_flag(config, ZJ77_REPORTING_TRIGGERS_ZB12);
        set_last_pkt_time(config, now_sec - 3);
        set_quota_holding_time(config, 10);

        how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

        REQUIRE(min_value_ms == 7000);
    }
    SECTION("DY9Xd")
    {
        add_reporting_flag(config, ZJ77_REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(config, 5);
        set_periodig_meas_start(config, now_sec);

        how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

        REQUIRE(min_value_ms == 5000);
    }


    SECTION("monitoring_time")
    {
        set_last_pkt_time(config, 75);
        set_monitoring_time_ts(config, now_sec + 30);
        set_monitoring_time_start(config, now_sec - 1);

        how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

        REQUIRE(min_value_ms == 129000);
    }

}


