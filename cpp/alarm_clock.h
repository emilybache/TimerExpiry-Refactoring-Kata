#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

#include <stdbool.h>

// operational flags
const unsigned int OPERATIONAL_FLAG_TIME_QUOTA_PRESENT = 1;
const unsigned int OPERATIONAL_FLAG_ZB12_STOPPED = 2;
const unsigned int OPERATIONAL_FLAG_ZB12_MODIFIED = 4;
const unsigned int OPERATIONAL_FLAG_BTI_PRESENT = 8;

// reporting flags
const unsigned int ZJ77_REPORTING_TRIGGERS_ZB12 = 1;
const unsigned int ZJ77_REPORTING_TRIGGERS_P88N = 2;
const unsigned int ZJ77_REPORTING_TRIGGERS_DY9X = 4;

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

struct alarm_config {
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
// The parameter 'alarmConfig' is the alarm clock configuration
void how_long_until_the_next_alarm(struct alarm_config *alarmConfig,
                                   unsigned int now_sec,
                                   unsigned long *min_value_ms);

unsigned int get_duration_meas_start(const struct alarm_config *pAlarmConfig);

unsigned int get_duration_meas(const struct alarm_config *pAlarmConfig);

unsigned int get_duration_meas_threshold_used(const struct alarm_config *pAlarmConfig);

unsigned int get_quota_holding_time(const struct alarm_config *pAlarmConfig);

unsigned int get_meas_DY9Xd(const struct alarm_config *pAlarmConfig);

unsigned int get_periodic_meas_start(const struct alarm_config *pAlarmConfig);

unsigned int get_monitoring_time_ts(const struct alarm_config *pAlarmConfig);

unsigned int get_monitoring_time_start(const struct alarm_config *pAlarmConfig);

unsigned int get_time_of_last_pkt(const struct alarm_config *pAlarmConfig);

unsigned int get_reporting_flags(const struct alarm_config *pAlarmConfig);

unsigned int get_idt_alarm_time(const struct alarm_config *pAlarmConfig);

unsigned int get_time_threshold(const struct alarm_config *pAlarmConfig);

unsigned int get_time_quota(const struct alarm_config *pAlarmConfig);

unsigned int get_operational_flags(const struct alarm_config *pAlarmConfig);

bool duration_measurement_active(const struct alarm_config *pAlarmConfig);

bool get_operational_flag_state(const struct alarm_config *pAlarmConfig, unsigned int flag);

bool test_and_clear_op_flag(struct alarm_config *pAlarmConfig, unsigned int flag);

unsigned int get_bti_time_interval(const struct alarm_config *pAlarmConfig);

void set_duration_meas_active(struct alarm_config *config, bool value);

void set_duration_meas_start(struct alarm_config *config, unsigned int value);

void set_idt_alarm_time(struct alarm_config *config, unsigned int value);

void set_last_pkt_time(struct alarm_config *config, unsigned int value);

void add_reporting_flag(struct alarm_config *config, unsigned int flag);

void set_time_threshold(struct alarm_config *config, unsigned int value);

void set_duration_meas_threshold(struct alarm_config *config, unsigned int value);

void add_operational_flag(struct alarm_config *config, unsigned int flag);

void set_time_quota(struct alarm_config *config, unsigned int value);

void set_duration_meas(struct alarm_config *config, unsigned int value);

void set_quota_holding_time(struct alarm_config *config, unsigned int value);

void set_meas_dy9xd(struct alarm_config *config, unsigned int value);

void set_periodig_meas_start(struct alarm_config *config, unsigned int value);

void set_monitoring_time_ts(struct alarm_config *config, unsigned int value);

void set_monitoring_time_start(struct alarm_config *config, unsigned int value);

void set_bti_time_interval(struct alarm_config *config, unsigned int value);


#endif //ALARMCLOCK_H
