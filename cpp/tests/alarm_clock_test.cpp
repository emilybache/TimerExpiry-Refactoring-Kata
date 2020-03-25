#include <AlarmClock.h>
#include <climits>

#include "catch2/catch.hpp"
#include "ApprovalTests.hpp"
#include <string>

namespace std {

    string describeResult(unsigned long min_value_ms, const string &description);

    TEST_CASE ("how_long_until_the_next_alarm") {

        unsigned int now_sec = 100; // 100 seconds since the epoch
        unsigned long min_value_ms = INT_MAX;

        auto *config = new alarm_config();
        config->timers = new timers();
        config->timers->duration = new duration();

        SECTION("no alarms set")
        {
            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);
            REQUIRE(min_value_ms == INT_MAX);
        }
        SECTION("idt_alarm")
        {
            config->timers->duration->meas_active = true;
            config->timers->duration->meas_start = 50;
            config->idt_alarm_time = 190;
            config->last_pkt = 75;

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 165000);
        }

        SECTION("P88N")
        {
            config->timers->duration->meas_active = true;
            config->reporting_flags += ZJ77_REPORTING_TRIGGERS_P88N;
            config->time_threshold = 10;
            config->timers->duration->meas = 1;
            config->timers->duration->meas_start = now_sec;
            config->timers->duration->meas_threshold_used = 3;

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 6000);
        }

        SECTION("TIME_QUOTA")
        {
            config->timers->duration->meas_active = true;
            config->operational_flags += OPERATIONAL_FLAG_TIME_QUOTA_PRESENT;
            config->time_quota = 10;
            config->timers->duration->meas_start = now_sec;
            config->timers->duration->meas = 1;

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 9000);
        }
        SECTION("ZB12")
        {
            config->reporting_flags += ZJ77_REPORTING_TRIGGERS_ZB12;
            config->operational_flags += OPERATIONAL_FLAG_ZB12_MODIFIED;
            config->last_pkt = now_sec - 3;
            config->timers->quota_holding_time = 10;

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 10000);
        }
        SECTION("ZB12 unmodified")
        {
            config->reporting_flags += ZJ77_REPORTING_TRIGGERS_ZB12;
            config->last_pkt = now_sec - 3;
            config->timers->quota_holding_time = 10;

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 7000);
        }
        SECTION("DY9Xd")
        {
            config->reporting_flags = ZJ77_REPORTING_TRIGGERS_DY9X;
            config->timers->meas_dy9xd = 5;
            config->timers->periodic_meas_start = now_sec;

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 5000);
        }


        SECTION("monitoring_time")
        {
            config->last_pkt = 75;
            config->timers->monitoring_time_ts = 130;
            config->timers->monitoring_time_start = 99;

            how_long_until_the_next_alarm(config, now_sec, &min_value_ms);

            REQUIRE(min_value_ms == 129000);
        }

    }

}