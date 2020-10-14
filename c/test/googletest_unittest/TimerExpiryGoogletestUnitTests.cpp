#include <gtest/gtest.h>
#include <climits>

extern "C"
{
#include "timers.h"
}

class TimerExpiryTest : public ::testing::Test {
protected:
    void SetUp() override {
        timerConfig = new timer_config();
        timerConfig->timers = new timers();
        timerConfig->timers->duration = new duration();
    }

    void TearDown() override {
        delete timerConfig;
    }

    // the timer configuration for this test case
    timer_config *timerConfig;
    // seconds since the epoch
    unsigned int now_sec = 100;
    // in-parameter for when the next timer will expire, in milliseconds after now_sec
    unsigned long min_value_ms = LONG_MAX;

};

TEST_F(TimerExpiryTest, NoTimersSet) {
    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(LONG_MAX, min_value_ms);
}

TEST_F(TimerExpiryTest, IDT_Timer) {
    set_duration_meas_active(timerConfig, true);
    set_duration_meas(timerConfig, 1);
    set_duration_meas_start(timerConfig, now_sec - 1);
    set_last_pkt_time(timerConfig, now_sec);
    set_idt_alarm_time(timerConfig, 3);

    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(3000, min_value_ms);
}

TEST_F(TimerExpiryTest, P88N_Timer) {
    set_duration_meas_active(timerConfig, true);
    set_duration_meas(timerConfig, 1);
    set_duration_meas_start(timerConfig, now_sec - 1);
    set_last_pkt_time(timerConfig, now_sec);
    add_reporting_flag(timerConfig, REPORTING_TRIGGERS_P88N);
    set_time_threshold(timerConfig, 10);

    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(8000, min_value_ms);
}

TEST_F(TimerExpiryTest, TimeQuota_Timer) {
    set_duration_meas_active(timerConfig, true);
    set_duration_meas(timerConfig, 1);
    set_duration_meas_start(timerConfig, now_sec - 1);
    set_last_pkt_time(timerConfig, now_sec);
    add_operational_flag(timerConfig, OPERATIONAL_FLAG_TIME_QUOTA_PRESENT);
    set_time_quota(timerConfig, 5);

    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(3000, min_value_ms);
}

TEST_F(TimerExpiryTest, ZB12_Timer_Not_Modified) {
    add_reporting_flag(timerConfig, REPORTING_TRIGGERS_ZB12);
    set_quota_holding_time(timerConfig, 6);
    clear_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
    set_last_pkt_time(timerConfig, now_sec);

    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(6000, min_value_ms);
    EXPECT_FALSE(get_operational_flag_state(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));
}

TEST_F(TimerExpiryTest, ZB12_Timer_Modified) {
    add_reporting_flag(timerConfig, REPORTING_TRIGGERS_ZB12);
    set_quota_holding_time(timerConfig, 6);
    clear_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_STOPPED); // this should be the default anyway
    set_last_pkt_time(timerConfig, now_sec);
    add_operational_flag(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);

    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(6000, min_value_ms);
    EXPECT_FALSE(get_operational_flag_state(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED));
}

TEST_F(TimerExpiryTest, DY9X_Timer) {
    add_reporting_flag(timerConfig, REPORTING_TRIGGERS_DY9X);
    set_meas_dy9xd(timerConfig, 5);
    set_periodic_meas_start(timerConfig, now_sec - 2);

    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(3000, min_value_ms);
}

TEST_F(TimerExpiryTest, Monitoring_time_Timer) {
    set_monitoring_time_ts(timerConfig, 7);
    set_monitoring_time_start(timerConfig, now_sec - 4);

    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);
    EXPECT_EQ(3000, min_value_ms);
}

TEST_F(TimerExpiryTest, All_Timers) {
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
    EXPECT_EQ(3000, min_value_ms);
}