#define APPROVALS_CATCH

#include "ApprovalTests.hpp"

#include <climits>
#include <string>

#include "printers.h"

extern "C"
{
#include "timers.h"
}


std::string do_next_timer_expiry(struct timer_config *timerConfig, unsigned int now_sec, unsigned long min_value_ms) {
    std::stringstream to_approve;
    to_approve << "\nBEFORE:\n" << *timerConfig;
    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    to_approve << "\nAFTER:\n" << *timerConfig;
    to_approve << "\nNEXT_TIMER: ";
    if (min_value_ms != LONG_MAX) {
        to_approve << min_value_ms;
    } else {
        to_approve << "LONG_MAX";
    }
    return to_approve.str();
}

TEST_CASE ("TimerExpiry", "how_long_until_next_timer_expiry") {
    // seconds since the epoch
    unsigned int now_sec = 100;
    // in-parameter for when the next timer will expire, in milliseconds after now_sec
    unsigned long min_value_ms = LONG_MAX;

    // the configuration of all the various timers
    auto *timerConfig = new timer_config();
    timerConfig->timers = new timers();
    timerConfig->timers->duration = new duration();

    SECTION("no timers set") {
        auto to_approve = do_next_timer_expiry(timerConfig, now_sec, min_value_ms);
        ApprovalTests::Approvals::verify(to_approve);
    }

    SECTION("ZB12 modified") {
        add_reporting_flag(timerConfig, REPORTING_TRIGGERS_ZB12);
        set_quota_holding_time(timerConfig, 7);
        clear_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_STOPPED);
        add_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
        auto to_approve = do_next_timer_expiry(timerConfig, now_sec, min_value_ms);
        ApprovalTests::Approvals::verify(to_approve);
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

        add_reporting_flag(timerConfig, REPORTING_TRIGGERS_DY9X);
        set_meas_dy9xd(timerConfig, 15);
        set_periodic_meas_start(timerConfig, now_sec - 2);

        set_monitoring_time_ts(timerConfig, 7);
        set_monitoring_time_start(timerConfig, now_sec - 4);

        auto to_approve = do_next_timer_expiry(timerConfig, now_sec, min_value_ms);
        ApprovalTests::Approvals::verify(to_approve);
    }


}


