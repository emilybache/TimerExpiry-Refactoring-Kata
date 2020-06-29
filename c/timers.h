#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

#include <stdbool.h>

// operational flags
const unsigned int OPERATIONAL_FLAG_TIME_QUOTA_PRESENT = 1;
const unsigned int OPERATIONAL_FLAG_ZB12_STOPPED = 2;
const unsigned int OPERATIONAL_FLAG_ZB12_MODIFIED = 4;
const unsigned int OPERATIONAL_FLAG_BTI_PRESENT = 8;

// reporting flags
const unsigned int REPORTING_TRIGGERS_ZB12 = 1;
const unsigned int REPORTING_TRIGGERS_P88N = 2;
const unsigned int REPORTING_TRIGGERS_DY9X = 4;

struct duration {
    unsigned int meas;
    unsigned int meas_threshold_used;
    unsigned int meas_start;
    bool meas_active;
    unsigned int bti_time_interval;
};

struct timers {
    struct duration *duration;
    unsigned int quota_holding_time;
    unsigned int monitoring_time_ts;
    unsigned int periodic_meas_start;
    unsigned int monitoring_time_start;
    unsigned int meas_dy9xd;
};

struct timer_config {
    struct timers *timers;
    unsigned int operational_flags; // bit field
    unsigned int reporting_flags; // bit field
    unsigned int idt_alarm_time;
    unsigned int time_threshold;
    unsigned int time_quota;
    unsigned int last_pkt;
};

// This is the function you need to change - see README
// This function updates the input parameter 'min_value_ms' to the number of milliseconds until the next alarm.
// The parameter 'now_sec' is the current time (in seconds since the epoch)
// The parameter 'timerConfig' is the alarm clock configuration
void how_long_until_next_timer_expiry(struct timer_config *timerConfig,
                                      unsigned int now_sec,
                                      unsigned long *min_value_ms);

unsigned int get_duration_meas_start(const struct timer_config *timerConfig);

unsigned int get_duration_meas(const struct timer_config *timerConfig);

unsigned int get_duration_meas_threshold_used(const struct timer_config *timerConfig);

unsigned int get_quota_holding_time(const struct timer_config *timerConfig);

unsigned int get_meas_DY9Xd(const struct timer_config *timerConfig);

unsigned int get_periodic_meas_start(const struct timer_config *timerConfig);

unsigned int get_monitoring_time_ts(const struct timer_config *timerConfig);

unsigned int get_monitoring_time_start(const struct timer_config *timerConfig);

unsigned int get_time_of_last_pkt(const struct timer_config *timerConfig);

unsigned int get_reporting_flags(const struct timer_config *timerConfig);

unsigned int get_idt_alarm_time(const struct timer_config *timerConfig);

unsigned int get_time_threshold(const struct timer_config *timerConfig);

unsigned int get_time_quota(const struct timer_config *timerConfig);

unsigned int get_operational_flags(const struct timer_config *timerConfig);

bool duration_measurement_active(const struct timer_config *timerConfig);

bool get_operational_flag_state(const struct timer_config *timerConfig, unsigned int flag);

bool test_and_clear_op_flag(struct timer_config *timerConfig, unsigned int flag);

unsigned int get_bti_time_interval(const struct timer_config *timerConfig);

void set_duration_meas_active(struct timer_config *config, bool value);

void set_duration_meas_start(struct timer_config *config, unsigned int value);

void set_idt_alarm_time(struct timer_config *config, unsigned int value);

void set_last_pkt_time(struct timer_config *config, unsigned int value);

void add_reporting_flag(struct timer_config *config, unsigned int flag);

void set_time_threshold(struct timer_config *config, unsigned int value);

void set_duration_meas_threshold(struct timer_config *config, unsigned int value);

void add_operational_flag(struct timer_config *config, unsigned int flag);

void set_time_quota(struct timer_config *config, unsigned int value);

void set_duration_meas(struct timer_config *config, unsigned int value);

void set_quota_holding_time(struct timer_config *config, unsigned int value);

void set_meas_dy9xd(struct timer_config *config, unsigned int value);

void set_periodic_meas_start(struct timer_config *config, unsigned int value);

void set_monitoring_time_ts(struct timer_config *config, unsigned int value);

void set_monitoring_time_start(struct timer_config *config, unsigned int value);

void set_bti_time_interval(struct timer_config *config, unsigned int value);

void clear_operational_flag(struct timer_config *timerConfig, unsigned int flag);


#endif //ALARMCLOCK_H
