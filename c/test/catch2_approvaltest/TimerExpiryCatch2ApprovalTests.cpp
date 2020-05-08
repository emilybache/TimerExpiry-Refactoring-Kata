#define APPROVALS_CATCH

#include "ApprovalTests.hpp"

#include <climits>
#include <string>

extern "C"
{
#include "timers.h"
}

/*
 * This is a printer for a timer_config instance.
 * It generally prints fields that are not zero.
 */
std::ostream &operator<<(std::ostream &os, const struct timer_config &obj) {
    auto timerConfig = &obj;
    os << "operational flags: " << get_operational_flags(timerConfig) << "\n"
       << "reporting flags: " << get_reporting_flags(timerConfig) << "\n";
    if (get_time_of_last_pkt(timerConfig) > 0)
        os << "last packet: " << get_time_of_last_pkt(timerConfig) << "\n";
    if (get_idt_alarm_time(timerConfig) != 0)
        os << "idt alarm time: " << get_idt_alarm_time(timerConfig) << "\n";
    if (get_time_threshold(timerConfig) != 0) {
        os << "time threshold: " << get_time_threshold(timerConfig) << "\n";
        os << "duration meas: " << get_duration_meas(timerConfig) << "\n";
        os << "duration meas threshold: " << get_duration_meas_threshold_used(timerConfig) << "\n";
    }
    if (get_time_quota(timerConfig) != 0) {
        os << "time quota: " << get_time_quota(timerConfig) << "\n";
        os << "duration meas: " << get_duration_meas(timerConfig) << "\n";
    }
    if (get_quota_holding_time(timerConfig) != 0) {
        os << "time quota holding time: " << get_quota_holding_time(timerConfig) << "\n";
    }
    if (get_meas_DY9Xd(timerConfig) != 0) {
        os << "DY9Xd: " << get_meas_DY9Xd(timerConfig) << "\n";
        os << "periodic meas start: " << get_periodic_meas_start(timerConfig) << "\n";
    }
    if (get_monitoring_time_ts(timerConfig) != 0) {
        os << "monitoring time start: " << get_monitoring_time_start(timerConfig) << "\n";
        os << "monitoring time ts: " << get_monitoring_time_ts(timerConfig) << "\n";
    }
    return os;
}

TEST_CASE ("TimerExpiry", "how_long_until_next_timer_expiry")
{
    // seconds since the epoch
    unsigned int now_sec = 100;
    // in-parameter for when the next timer will expire, in milliseconds after now_sec
    unsigned long min_value_ms = LONG_MAX;

    // the configuration of all the various timers
    auto *timerConfig = new timer_config();
    timerConfig->timers = new timers();
    timerConfig->timers->duration = new duration();

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

    SECTION("All timers set") {
        how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
        std::stringstream to_approve;
        to_approve << *timerConfig;
        to_approve << "NEXT_TIMER: ";

        if (min_value_ms != LONG_MAX) {
            to_approve << min_value_ms;
        } else {
            to_approve << "LONG_MAX";
        }

        auto t = to_approve.str();
        ApprovalTests::Approvals::verify(t);

    }
}


