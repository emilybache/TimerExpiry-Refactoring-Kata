#include <limits.h>
#include "alarm_clock.h"

void convert_to_ms_if_set(unsigned long *min_value_ms, unsigned int min_value_sec);

unsigned int idt_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int p88n_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int time_quota_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int dy9x_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int monitoring_time_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

unsigned int zb12_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec);

void clear_zb12_flag_if_set(const struct alarm_config *alarmConfig);

void
how_long_until_the_next_alarm(struct alarm_config *alarmConfig, const unsigned int now_sec,
                              unsigned long *min_value_ms) {

    unsigned int (*pointers_to_alarm_functions[6]) (const struct alarm_config*, const unsigned int);
    pointers_to_alarm_functions[0] = idt_alarm;
    pointers_to_alarm_functions[1] = p88n_alarm;
    pointers_to_alarm_functions[2] = time_quota_alarm;
    pointers_to_alarm_functions[3] = zb12_alarm;
    pointers_to_alarm_functions[4] = dy9x_alarm;
    pointers_to_alarm_functions[5] = monitoring_time_alarm;
    
    unsigned  int smallest = INT_MAX;
    for (int i = 0; i < 6; ++i) {
        unsigned int alarm_time = pointers_to_alarm_functions[i](alarmConfig, now_sec);
        if (alarm_time < smallest) {
            smallest = alarm_time;
        }
    }

    clear_zb12_flag_if_set(alarmConfig);
    convert_to_ms_if_set(min_value_ms, smallest);
}

void clear_zb12_flag_if_set(const struct alarm_config *alarmConfig) {
    unsigned int reporting_flags = get_reporting_flags(alarmConfig);
    if (reporting_flags & ZJ77_REPORTING_TRIGGERS_ZB12) {
        if ((get_quota_holding_time(alarmConfig) != 0) &&
            !get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED)) {

            if (get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED))
                add_operational_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED);
        }
    }
}

unsigned int zb12_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    unsigned int lreporting_flags = get_reporting_flags(alarmConfig);
    if (lreporting_flags & ZJ77_REPORTING_TRIGGERS_ZB12) {
        if ((get_quota_holding_time(alarmConfig) != 0) &&
            !get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED)) {
            /* If ZB12 is just provisioned, start timer with provisioned value
             * If ZB12 is modified, start timer with modified value and reset ZB12 modified flag
             * If ZB12 is not modified (and not just provisioned), restart the session timer for remainder of the provisioned value
             */
            time_sec = get_quota_holding_time(alarmConfig);

            if (!get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED) &&
                (get_time_of_last_pkt(alarmConfig) != 0)) {
                unsigned int last_pkt_diff = now_sec - get_time_of_last_pkt(alarmConfig);
                time_sec -= last_pkt_diff;
            }
        }
    }
    return time_sec;
}

unsigned int monitoring_time_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    unsigned int diff_sec;
    // Handle timer for Monitoring Time
    if (get_monitoring_time_ts(alarmConfig) != 0) {
        diff_sec = now_sec - get_monitoring_time_start(alarmConfig);

        time_sec = get_monitoring_time_ts(alarmConfig) - diff_sec;
    }
    return time_sec;
}

unsigned int dy9x_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    if ((get_reporting_flags(alarmConfig) & ZJ77_REPORTING_TRIGGERS_DY9X) && (get_meas_DY9Xd(alarmConfig) != 0)) {
        unsigned int diff_sec = now_sec - get_periodic_meas_start(alarmConfig);

        time_sec = get_meas_DY9Xd(alarmConfig) - diff_sec;
    }
    return time_sec;
}

unsigned int time_quota_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    unsigned int op_flags = get_operational_flags(alarmConfig);
    if (duration_measurement_active(alarmConfig)
    && (op_flags & OPERATIONAL_FLAG_TIME_QUOTA_PRESENT) && (get_time_quota(alarmConfig) != 0)) {
        unsigned int pkt_rx_diff = now_sec - get_duration_meas_start(alarmConfig);

        time_sec = get_time_quota(alarmConfig) - (get_duration_meas(alarmConfig) + pkt_rx_diff);
    }
    return time_sec;
}

unsigned int p88n_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    unsigned int local_reporting_flags = get_reporting_flags(alarmConfig);
    if (duration_measurement_active(alarmConfig) && (local_reporting_flags & ZJ77_REPORTING_TRIGGERS_P88N) && (get_time_threshold(alarmConfig) != 0)) {
        unsigned int pkt_rx_diff = now_sec - get_duration_meas_start(alarmConfig);

        time_sec = get_time_threshold(alarmConfig) -
                                ((get_duration_meas(alarmConfig) + get_duration_meas_threshold_used(alarmConfig))
                                 + pkt_rx_diff);
    }
    return time_sec;
}

unsigned int idt_alarm(const struct alarm_config *alarmConfig, const unsigned int now_sec) {
    unsigned int time_sec = INT_MAX;
    if (duration_measurement_active(alarmConfig) && get_idt_alarm_time(alarmConfig) != 0) {
        unsigned int last_pkt_diff = now_sec - get_time_of_last_pkt(alarmConfig);
        time_sec = get_idt_alarm_time(alarmConfig);
        time_sec = time_sec - last_pkt_diff;
    }
    return time_sec;
}

void convert_to_ms_if_set(unsigned long *min_value_ms, unsigned int min_value_sec) {
    if (min_value_sec != INT_MAX) {
        if (min_value_sec * 1000 < *min_value_ms) {
            *min_value_ms = min_value_sec * 1000;
        }
    }
}

bool get_operational_flag_state(struct alarm_config *pAlarmConfig, unsigned int flag) {
    return pAlarmConfig->operational_flags & flag;
}

unsigned int get_monitoring_time_start(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->monitoring_time_start;
}

unsigned int get_monitoring_time_ts(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->monitoring_time_ts;
}

unsigned int get_periodic_meas_start(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->periodic_meas_start;
}

unsigned int get_meas_DY9Xd(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->meas_dy9xd;
}

unsigned int get_quota_holding_time(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->quota_holding_time;
}

unsigned int get_time_quota(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->time_quota;
}

unsigned int get_duration_meas_threshold_used(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_threshold_used;
}

unsigned int get_duration_meas(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas;
}

unsigned int get_time_threshold(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->time_threshold;
}

unsigned int get_idt_alarm_time(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->idt_alarm_time;
}

unsigned int get_duration_meas_start(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_start;
}

unsigned int get_reporting_flags(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->reporting_flags;
}

unsigned int get_time_of_last_pkt(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->last_pkt;
}

bool duration_measurement_active(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_active;
}

unsigned int get_operational_flags(struct alarm_config *pAlarmConfig) {
    return pAlarmConfig->operational_flags;
}

unsigned int get_bti_time_interval(struct alarm_config *config) {
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

void set_periodig_meas_start(struct alarm_config *config, unsigned int value) {
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

