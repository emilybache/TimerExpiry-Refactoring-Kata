#ifndef VALIDATE_AND_ADD_PRODUCT_REFACTORING_KATA_ALARMCLOCK_H
#define VALIDATE_AND_ADD_PRODUCT_REFACTORING_KATA_ALARMCLOCK_H

#include <climits>

// operational flags
unsigned int OPERATIONAL_FLAG_TIME_QUOTA_PRESENT = 1;
unsigned int OPERATIONAL_FLAG_ZB12_STOPPED = 2;
unsigned int OPERATIONAL_FLAG_ZB12_MODIFIED = 3;

// reporting flags
unsigned int ZJ77_REPORTING_TRIGGERS_ZB12 = 1;
unsigned int ZJ77_REPORTING_TRIGGERS_P88N = 2;
unsigned int ZJ77_REPORTING_TRIGGERS_DY9X = 3;

struct duration
{
    unsigned int meas;
    unsigned int meas_threshold_used;
    unsigned int meas_start;
    bool meas_active;
};

struct timers
{
    duration *duration;
    unsigned int quota_holding_time;
    unsigned int monitoring_time_ts;
    unsigned int periodic_meas_start;
    unsigned int monitoring_time_start;
    unsigned int meas_dy9xd;
};

struct alarm_config
{
    timers *timers;
    unsigned int operational_flags; // bit field
    unsigned int reporting_flags; // bit field
    unsigned int idt_time;
    unsigned int time_threshold;
    unsigned int time_quota;
    unsigned int last_pkt;
};

unsigned int get_duration_meas_start(alarm_config *pAlarmConfig);
unsigned int get_duration_meas(alarm_config *pAlarmConfig);
unsigned int get_duration_meas_threshold_used(alarm_config *pAlarmConfig);
unsigned int get_quota_holding_time(alarm_config *pAlarmConfig);
unsigned int get_meas_DY9Xd(alarm_config *pAlarmConfig);
unsigned int get_periodic_meas_start(alarm_config *pAlarmConfig);
unsigned int get_monitoring_time_ts(alarm_config *pAlarmConfig);
unsigned int get_monitoring_time_start(alarm_config *pAlarmConfig);
unsigned int get_time_of_last_pkt(alarm_config* pAlarmConfig);
unsigned int get_reporting_flags(alarm_config *pAlarmConfig);
unsigned int get_idt_time(alarm_config *pAlarmConfig);
unsigned int get_time_threshold(alarm_config *pAlarmConfig);
unsigned int get_time_quota(alarm_config *pAlarmConfig);

unsigned int get_operational_flags(alarm_config* pAlarmConfig);
bool duration_measurement_active(alarm_config* pAlarmConfig);
bool get_operational_flag_state(alarm_config *pAlarmConfig, unsigned int flag);
bool test_and_clear_op_flag(alarm_config *pAlarmConfig, unsigned int flag);

// This function updates the input parameter 'min_value_ms' to the number of milliseconds until the next alarm.
// The parameter 'now_sec' is the current time (in seconds since the epoch)
// The parameter 'alarmConfig' is the alarm clock configuration
void
how_long_until_the_next_alarm(alarm_config* alarmConfig, const unsigned int now_sec, unsigned long *min_value_ms)
{
    unsigned int time_sec;
    unsigned int diff_sec;
    unsigned int min_value_sec = INT_MAX;

    unsigned int op_flags    = get_operational_flags(alarmConfig);
    unsigned int reporting_flags = get_reporting_flags(alarmConfig);
    unsigned int last_pkt_diff   = now_sec - get_time_of_last_pkt(alarmConfig);

    if (duration_measurement_active(alarmConfig)) {
        unsigned int pkt_rx_diff = now_sec - get_duration_meas_start(alarmConfig);

        if (get_idt_time(alarmConfig) != 0) {
            time_sec = get_idt_time(alarmConfig);
            if ((time_sec - last_pkt_diff) < min_value_sec) {
                min_value_sec = time_sec - last_pkt_diff;
            }
        }
        if ((reporting_flags & ZJ77_REPORTING_TRIGGERS_P88N) && (get_time_threshold(alarmConfig) != 0)) {
            time_sec = get_time_threshold(alarmConfig) -
                       ((get_duration_meas(alarmConfig) + get_duration_meas_threshold_used(alarmConfig)) + pkt_rx_diff);
            if (time_sec < min_value_sec) {
                min_value_sec = time_sec;
            }
        }
        if ((op_flags & OPERATIONAL_FLAG_TIME_QUOTA_PRESENT) && (get_time_quota(alarmConfig) != 0)) {
            time_sec = get_time_quota(alarmConfig) - (get_duration_meas(alarmConfig) + pkt_rx_diff);
            if (time_sec < min_value_sec) {
                min_value_sec = time_sec;
            }
        }
    }

    if (reporting_flags & ZJ77_REPORTING_TRIGGERS_ZB12) {
        if ((get_quota_holding_time(alarmConfig) != 0) && !get_operational_flag_state(alarmConfig, OPERATIONAL_FLAG_ZB12_STOPPED)) {
            /* If ZB12 is just provisioned, start timer with provisioned value
             * If ZB12 is modified, start timer with modified value and reset ZB12 modified flag
             * If ZB12 is not modified (and not just provisioned), restart the session timer for remainder of the provisioned value
             */
            time_sec = get_quota_holding_time(alarmConfig);
            if (!test_and_clear_op_flag(alarmConfig, OPERATIONAL_FLAG_ZB12_MODIFIED) &&
                (get_time_of_last_pkt(alarmConfig) != 0))
            {
                time_sec -= last_pkt_diff;
            }

            if (time_sec < min_value_sec) {
                min_value_sec = time_sec;
            }
        }
    }

    if ((reporting_flags & ZJ77_REPORTING_TRIGGERS_DY9X) && (get_meas_DY9Xd(alarmConfig) != 0)) {
        diff_sec = now_sec - get_periodic_meas_start(alarmConfig);
        time_sec = get_meas_DY9Xd(alarmConfig) - diff_sec;
        if (time_sec < min_value_sec) {
            min_value_sec = time_sec;
        }
    }
    // Handle timer for Monitoring Time
    if (get_monitoring_time_ts(alarmConfig) != 0) {
        diff_sec = now_sec - get_monitoring_time_start(alarmConfig);
        time_sec = get_monitoring_time_ts(alarmConfig) - diff_sec;
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

bool test_and_clear_op_flag(alarm_config *pAlarmConfig, unsigned int flag) {
    bool result = pAlarmConfig->operational_flags & flag;
    if (result)
        pAlarmConfig->operational_flags &= ~flag;
    return result;
}

bool get_operational_flag_state(alarm_config *pAlarmConfig, unsigned int flag) {
    return pAlarmConfig->operational_flags & flag;
}

unsigned  int get_monitoring_time_start(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->monitoring_time_start;
}

unsigned int get_monitoring_time_ts(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->monitoring_time_ts;
}

unsigned  int get_periodic_meas_start(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->periodic_meas_start;
}

unsigned int get_meas_DY9Xd(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->meas_dy9xd;
}

unsigned int get_quota_holding_time(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->quota_holding_time;
}

unsigned int get_time_quota(alarm_config *pAlarmConfig) {
    return pAlarmConfig->time_quota;
}

unsigned int get_duration_meas_threshold_used(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_threshold_used;
}

unsigned int get_duration_meas(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas;
}

unsigned int get_time_threshold(alarm_config *pAlarmConfig) {
    return pAlarmConfig->time_threshold;
}

unsigned int get_idt_time(alarm_config *pAlarmConfig) {
    return pAlarmConfig->idt_time;
}

unsigned  int get_duration_meas_start(alarm_config *pAlarmConfig) {
    return pAlarmConfig->timers->duration->meas_start;
}

unsigned int get_reporting_flags(alarm_config *pAlarmConfig) {
    return pAlarmConfig->reporting_flags;
}

unsigned int get_time_of_last_pkt(alarm_config* pAlarmConfig)
{
    return pAlarmConfig->last_pkt;
}

bool duration_measurement_active(alarm_config* pAlarmConfig)
{
    return pAlarmConfig->timers->duration->meas_active;
}

unsigned int get_operational_flags(alarm_config* pAlarmConfig)
{
    return pAlarmConfig->operational_flags;
}
#endif //VALIDATE_AND_ADD_PRODUCT_REFACTORING_KATA_ALARMCLOCK_H
