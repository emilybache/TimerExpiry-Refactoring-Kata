#include <limits.h>
#include "alarm_clock.h"


void clear_zb12_flag_if_alarm_was_active(struct alarm_config *alarmConfig);

unsigned int calculate_idt_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int calculate_p88n_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int calculate_time_quota_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int calculate_zb12_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int calculate_dy9z_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int calculate_monitoring_time_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int calculate_bti_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

typedef unsigned int (*AlarmFunction)(const struct alarm_config *, const unsigned int);

void
how_long_until_the_next_alarm(struct alarm_config *alarmConfig, const unsigned int now_sec,
                              unsigned long *min_value_ms) {
    unsigned int min_value_sec;
    min_value_sec = calculate_earliest_alarm(alarmConfig, now_sec);
    clear_zb12_flag_if_alarm_was_active(alarmConfig);

    if (min_value_sec != INT_MAX) {
        unsigned int in_ms = min_value_sec * 1000;
        if (in_ms < *min_value_ms) {
            *min_value_ms = in_ms;
        }
    }
}

unsigned int calculate_earliest_alarm(const struct alarm_config *alarmConfig, unsigned int now_sec) {
    unsigned int min_value_sec = INT_MAX;
    unsigned int alarm_value;

    AlarmFunction alarm_functions[] = {calculate_idt_alarm,
                                       calculate_p88n_alarm,
                                       calculate_time_quota_alarm,
                                       calculate_zb12_alarm,
                                       calculate_dy9z_alarm,
                                       calculate_monitoring_time_alarm,
                                       calculate_bti_alarm};

    int ALARM_FUNCTIONS_SIZE = sizeof(alarm_functions) / sizeof(alarm_functions[0]);
    for (int i = 0; i < ALARM_FUNCTIONS_SIZE; ++i) {
        alarm_value = alarm_functions[i](alarmConfig, now_sec);
        if (alarm_value < min_value_sec) {
            min_value_sec = alarm_value;
        }
    }
    return min_value_sec;
}

void clear_zb12_flag_if_alarm_was_active(struct alarm_config *alarmConfig) {
    if (get_reporting_flags(alarmConfig) & REPORTING_TRIGGERS_ZB12 && (get_quota_holding_time(alarmConfig) != 0) &&
        !get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED)) {
        // If ZB12 is modified, reset ZB12 modified flag
        if (get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED)) {
            clear_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
        }
    }
}

unsigned int calculate_bti_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    unsigned int diff_sec;
    if (get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_BTI_PRESENT)) {
        diff_sec = now_sec - get_time_of_last_pkt(alarmConfig);
        time_sec = get_bti_time_interval(alarmConfig) - diff_sec;
    }
    return time_sec;
}


unsigned int calculate_monitoring_time_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    unsigned int diff_sec;
    // Handle timer for Monitoring Time
    if (get_monitoring_time_ts(alarmConfig) != 0) {
        diff_sec = now_sec - get_monitoring_time_start(alarmConfig);
        time_sec = get_monitoring_time_ts(alarmConfig) - diff_sec;
    }
    return time_sec;
}

unsigned int calculate_dy9z_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int dy9z_alarm = INT_MAX;
    unsigned int diff_sec2;
    if ((get_reporting_flags(alarmConfig) & REPORTING_TRIGGERS_DY9X) && (get_meas_DY9Xd(alarmConfig) != 0)) {
        diff_sec2 = now_sec - get_periodic_meas_start(alarmConfig);
        dy9z_alarm = get_meas_DY9Xd(alarmConfig) - diff_sec2;
    }
    return dy9z_alarm;
}

unsigned int calculate_zb12_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int zb12_alarm = INT_MAX;
    if (get_reporting_flags(alarmConfig) & REPORTING_TRIGGERS_ZB12 && (get_quota_holding_time(alarmConfig) != 0) &&
        !get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED)) {
        /* If ZB12 is just provisioned, start timer with provisioned value
         * If ZB12 is modified, start timer with modified value
         * If ZB12 is not modified (and not just provisioned), restart the session timer for remainder of the provisioned value
         */
        zb12_alarm = get_quota_holding_time(alarmConfig);
        if (!get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED) &&
            (get_time_of_last_pkt(alarmConfig) != 0)) {
            zb12_alarm -= now_sec - get_time_of_last_pkt(alarmConfig);
        }

    }
    return zb12_alarm;
}

unsigned int calculate_time_quota_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_quota_alarm = INT_MAX;
    unsigned int pkt_rx_diff;

    if (duration_measurement_active(alarmConfig) &&
        (get_operational_flags(alarmConfig) & OPERATIONAL_FLAG_TIME_QUOTA_PRESENT) &&
        (get_time_quota(alarmConfig) != 0)) {
        pkt_rx_diff = now_sec - get_duration_meas_start(alarmConfig);
        time_quota_alarm = get_time_quota(alarmConfig) - (get_duration_meas(alarmConfig) + pkt_rx_diff);
    }
    return time_quota_alarm;
}

unsigned int calculate_p88n_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int p88n_alarm_time = INT_MAX;
    if (duration_measurement_active(alarmConfig) && (get_reporting_flags(alarmConfig) & REPORTING_TRIGGERS_P88N) &&
        (get_time_threshold(alarmConfig) != 0)) {
        unsigned int pkt_rx_diff = now_sec - get_duration_meas_start(alarmConfig);
        p88n_alarm_time = get_time_threshold(alarmConfig) -
                          ((get_duration_meas(alarmConfig) + get_duration_meas_threshold_used(alarmConfig))
                           + pkt_rx_diff);
    }
    return p88n_alarm_time;
}

unsigned int calculate_idt_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int idt_alarm_time = INT_MAX;
    unsigned int time_sec;
    unsigned int last_pkt_diff;

    if (duration_measurement_active(alarmConfig) && get_idt_alarm_time(alarmConfig) != 0) {
        time_sec = get_idt_alarm_time(alarmConfig);
        last_pkt_diff = now_sec - get_time_of_last_pkt(alarmConfig);
        idt_alarm_time = time_sec - last_pkt_diff;
    }
    return idt_alarm_time;
}

void clear_operational_flag(struct alarm_config *pAlarmConfig,
                            unsigned int flag) {
    pAlarmConfig->operational_flags &= ~flag;
}

bool get_operational_flag_state(const struct alarm_config *pAlarmConfig, unsigned int flag) {
    return pAlarmConfig->operational_flags & flag;
}

unsigned int get_monitoring_time_start(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->monitoring_time_start;
}

unsigned int get_monitoring_time_ts(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->monitoring_time_ts;
}

unsigned int get_periodic_meas_start(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->periodic_meas_start;
}

unsigned int get_meas_DY9Xd(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->meas_dy9xd;
}

unsigned int get_quota_holding_time(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->quota_holding_time;
}

unsigned int get_time_quota(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->time_quota;
}

unsigned int get_duration_meas_threshold_used(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_threshold_used;
}

unsigned int get_duration_meas(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas;
}

unsigned int get_time_threshold(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->time_threshold;
}

unsigned int get_idt_alarm_time(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->idt_alarm_time;
}

unsigned int get_duration_meas_start(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_start;
}

unsigned int get_reporting_flags(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->reporting_flags;
}

unsigned int get_time_of_last_pkt(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->last_pkt;
}

bool duration_measurement_active(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_active;
}

unsigned int get_operational_flags(const struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->operational_flags;
}

unsigned int get_bti_time_interval(const struct alarm_config *config) {
    return config->timers->duration->bti_time_interval;
}

void set_duration_meas_active(struct alarm_config *config, bool value) {
    config->timers->duration->meas_active = true;
}

void set_duration_meas_start(struct alarm_config *config, unsigned int value) {
    config->timers->duration->meas_start = value;
}

void set_idt_alarm_time(struct alarm_config *config, unsigned int value) {
    config->idt_alarm_time = value;
}

void set_last_pkt_time(struct alarm_config *config, unsigned int value) {
    config->last_pkt = value;
}

void add_reporting_flag(struct alarm_config *config, unsigned int flag) {
    config->reporting_flags += flag;
}

void set_time_threshold(struct alarm_config *config, unsigned int value) {
    config->time_threshold = value;
}

void set_duration_meas_threshold(struct alarm_config *config, unsigned int value) {
    config->timers->duration->meas_threshold_used = value;
}

void add_operational_flag(struct alarm_config *config, unsigned int flag) {
    config->operational_flags += flag;
}

void set_time_quota(struct alarm_config *config, unsigned int value) {
    config->time_quota = value;
}

void set_duration_meas(struct alarm_config *config, unsigned int value) {
    config->timers->duration->meas = value;
}

void set_quota_holding_time(struct alarm_config *config, unsigned int value) {
    config->timers->quota_holding_time = value;
}

void set_meas_dy9xd(struct alarm_config *config, unsigned int value) {
    config->timers->meas_dy9xd = value;
}

void set_periodic_meas_start(struct alarm_config *config, unsigned int value) {
    config->timers->periodic_meas_start = value;
}

void set_monitoring_time_ts(struct alarm_config *config, unsigned int value) {
    config->timers->monitoring_time_ts = value;
}

void set_monitoring_time_start(struct alarm_config *config, unsigned int value) {
    config->timers->monitoring_time_start = value;
}

void set_bti_time_interval(struct alarm_config *config, unsigned int value) {
    config->timers->duration->bti_time_interval = value;
}


