#include <limits.h>
#include "alarm_clock.h"

void
how_long_until_next_timer_expiry(struct timer_config *timerConfig, const unsigned int now_sec,
                                 unsigned long *min_value_ms) {
    unsigned int time_sec;
    unsigned int diff_sec;
    unsigned int min_value_sec = INT_MAX;

    unsigned int op_flags = get_operational_flags(timerConfig);
    unsigned int reporting_flags = get_reporting_flags(timerConfig);
    unsigned int last_pkt_diff = now_sec - get_time_of_last_pkt(timerConfig);

    if (duration_measurement_active(timerConfig)) {
        unsigned int pkt_rx_diff = now_sec - get_duration_meas_start(timerConfig);

        if (get_idt_alarm_time(timerConfig) != 0) {
            time_sec = get_idt_alarm_time(timerConfig);
            if ((time_sec - last_pkt_diff) < min_value_sec) {
                min_value_sec = time_sec - last_pkt_diff;
            }
        }
        if ((reporting_flags & REPORTING_TRIGGERS_P88N) && (get_time_threshold(timerConfig) != 0)) {
            time_sec = get_time_threshold(timerConfig) -
                       ((get_duration_meas(timerConfig) + get_duration_meas_threshold_used(timerConfig))
                        + pkt_rx_diff);
            if (time_sec < min_value_sec) {
                min_value_sec = time_sec;
            }
        }
        if ((op_flags & OPERATIONAL_FLAG_TIME_QUOTA_PRESENT) && (get_time_quota(timerConfig) != 0)) {
            time_sec = get_time_quota(timerConfig) - (get_duration_meas(timerConfig) + pkt_rx_diff);
            if (time_sec < min_value_sec) {
                min_value_sec = time_sec;
            }
        }
    }

    if (reporting_flags & REPORTING_TRIGGERS_ZB12) {
        if ((get_quota_holding_time(timerConfig) != 0) &&
            !get_operational_flag_state(timerConfig, OPERATIONAL_FLAG_ZB12_STOPPED)) {
            /* If ZB12 is just provisioned, start timer with provisioned value
             * If ZB12 is modified, start timer with modified value and reset ZB12 modified flag
             * If ZB12 is not modified (and not just provisioned), restart the session timer for remainder of the provisioned value
             */
            time_sec = get_quota_holding_time(timerConfig);
            if (!test_and_clear_op_flag(timerConfig, OPERATIONAL_FLAG_ZB12_MODIFIED) &&
                (get_time_of_last_pkt(timerConfig) != 0)) {
                time_sec -= last_pkt_diff;
            }

            if (time_sec < min_value_sec) {
                min_value_sec = time_sec;
            }
        }
    }

    if ((reporting_flags & REPORTING_TRIGGERS_DY9X) && (get_meas_DY9Xd(timerConfig) != 0)) {
        diff_sec = now_sec - get_periodic_meas_start(timerConfig);
        time_sec = get_meas_DY9Xd(timerConfig) - diff_sec;
        if (time_sec < min_value_sec) {
            min_value_sec = time_sec;
        }
    }
    // Handle timer for Monitoring Time
    if (get_monitoring_time_ts(timerConfig) != 0) {
        diff_sec = now_sec - get_monitoring_time_start(timerConfig);
        time_sec = get_monitoring_time_ts(timerConfig) - diff_sec;
        if (time_sec < min_value_sec) {
            min_value_sec = time_sec;
        }
    }

    if (min_value_sec != INT_MAX) {
        if (min_value_sec * 1000 < *min_value_ms) {
            *min_value_ms = min_value_sec * 1000;
        }
    }
}

void clear_operational_flag(struct timer_config *timerConfig,
                            unsigned int flag) {
    timerConfig->operational_flags &= ~flag;
}

bool test_and_clear_op_flag(struct timer_config *timerConfig, unsigned int flag) {
    bool result = get_operational_flag_state(timerConfig, flag);
    if (result)
        clear_operational_flag(timerConfig, flag);
    return result;
}

bool get_operational_flag_state(const struct timer_config *timerConfig, unsigned int flag) {
    return timerConfig->operational_flags & flag;
}

unsigned int get_monitoring_time_start(const struct timer_config *timerConfig) {
    return timerConfig->timers->monitoring_time_start;
}

unsigned int get_monitoring_time_ts(const struct timer_config *timerConfig) {
    return timerConfig->timers->monitoring_time_ts;
}

unsigned int get_periodic_meas_start(const struct timer_config *timerConfig) {
    return timerConfig->timers->periodic_meas_start;
}

unsigned int get_meas_DY9Xd(const struct timer_config *timerConfig) {
    return timerConfig->timers->meas_dy9xd;
}

unsigned int get_quota_holding_time(const struct timer_config *timerConfig) {
    return timerConfig->timers->quota_holding_time;
}

unsigned int get_time_quota(const struct timer_config *timerConfig) {
    return timerConfig->time_quota;
}

unsigned int get_duration_meas_threshold_used(const struct timer_config *timerConfig) {
    return timerConfig->timers->duration->meas_threshold_used;
}

unsigned int get_duration_meas(const struct timer_config *timerConfig) {
    return timerConfig->timers->duration->meas;
}

unsigned int get_time_threshold(const struct timer_config *timerConfig) {
    return timerConfig->time_threshold;
}

unsigned int get_idt_alarm_time(const struct timer_config *timerConfig) {
    return timerConfig->idt_alarm_time;
}

unsigned int get_duration_meas_start(const struct timer_config *timerConfig) {
    return timerConfig->timers->duration->meas_start;
}

unsigned int get_reporting_flags(const struct timer_config *timerConfig) {
    return timerConfig->reporting_flags;
}

unsigned int get_time_of_last_pkt(const struct timer_config *timerConfig) {
    return timerConfig->last_pkt;
}

bool duration_measurement_active(const struct timer_config *timerConfig) {
    return timerConfig->timers->duration->meas_active;
}

unsigned int get_operational_flags(const struct timer_config *timerConfig) {
    return timerConfig->operational_flags;
}

unsigned int get_bti_time_interval(const struct timer_config *config) {
    return config->timers->duration->bti_time_interval;
}

void set_duration_meas_active(struct timer_config *config, bool value) {
    config->timers->duration->meas_active = value;
}

void set_duration_meas_start(struct timer_config *config, unsigned int value) {
    config->timers->duration->meas_start = value;
}

void set_idt_alarm_time(struct timer_config *config, unsigned int value) {
    config->idt_alarm_time = value;
}

void set_last_pkt_time(struct timer_config *config, unsigned int value) {
    config->last_pkt = value;
}

void add_reporting_flag(struct timer_config *config, unsigned int flag) {
    config->reporting_flags += flag;
}

void set_time_threshold(struct timer_config *config, unsigned int value) {
    config->time_threshold = value;
}

void set_duration_meas_threshold(struct timer_config *config, unsigned int value) {
    config->timers->duration->meas_threshold_used = value;
}

void add_operational_flag(struct timer_config *config, unsigned int flag) {
    config->operational_flags += flag;
}

void set_time_quota(struct timer_config *config, unsigned int value) {
    config->time_quota = value;
}

void set_duration_meas(struct timer_config *config, unsigned int value) {
    config->timers->duration->meas = value;
}

void set_quota_holding_time(struct timer_config *config, unsigned int value) {
    config->timers->quota_holding_time = value;
}

void set_meas_dy9xd(struct timer_config *config, unsigned int value) {
    config->timers->meas_dy9xd = value;
}

void set_periodic_meas_start(struct timer_config *config, unsigned int value) {
    config->timers->periodic_meas_start = value;
}

void set_monitoring_time_ts(struct timer_config *config, unsigned int value) {
    config->timers->monitoring_time_ts = value;
}

void set_monitoring_time_start(struct timer_config *config, unsigned int value) {
    config->timers->monitoring_time_start = value;
}

void set_bti_time_interval(struct timer_config *config, unsigned int value) {
    config->timers->duration->bti_time_interval = value;
}


