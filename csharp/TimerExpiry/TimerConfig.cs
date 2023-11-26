using System;

namespace TestTimerExpiry;

public class TimerConfig
{
    public Timers Timers { get; set; }
    public uint operational_flags { get; set; }
    public uint reporting_flags { get; set; }
    public uint idt_alarm_time { get; set; }
    public uint time_threshold { get; set; }
    public uint time_quota { get; set; }
    public uint last_pkt { get; set; }

    public void how_long_until_next_timer_expiry(uint now_sec, out ulong min_value_ms)
    {
        uint time_sec;
        uint diff_sec;
        uint min_value_sec = UInt32.MaxValue;

        uint last_pkt_diff = now_sec - last_pkt;

        if (Timers.Duration.MeasActive)
        {
            uint pkt_rx_diff = now_sec - Timers.Duration.MeasStart;

            if (idt_alarm_time != 0)
            {
                time_sec = idt_alarm_time;
                if ((time_sec - last_pkt_diff) < min_value_sec)
                {
                    min_value_sec = time_sec - last_pkt_diff;
                }
            }

            if (((reporting_flags & (uint)ReportingTriggers.P88N) > 0) && (time_threshold != 0))
            {
                time_sec = time_threshold -
                           ((Timers.Duration.Meas + Timers.Duration.MeasThresholdUsed)
                            + pkt_rx_diff);
                if (time_sec < min_value_sec)
                {
                    min_value_sec = time_sec;
                }
            }

            if (((operational_flags & (uint)OperationalFlags.TimeQuotaPresent) > 0) && (time_quota != 0))
            {
                time_sec = time_quota - (Timers.Duration.Meas + pkt_rx_diff);
                if (time_sec < min_value_sec)
                {
                    min_value_sec = time_sec;
                }
            }
        }

        if ((reporting_flags & (uint)ReportingTriggers.ZB12) > 0)
        {
            if ((Timers.QuotaHoldingTime != 0) &&
                !getOperationalFlagState(OperationalFlags.ZB12Stopped))
            {
                /* If ZB12 is just provisioned, start timer with provisioned value
                 * If ZB12 is modified, start timer with modified value and reset ZB12 modified flag
                 * If ZB12 is not modified (and not just provisioned), restart the session timer for remainder of the provisioned value
                 */
                time_sec = Timers.QuotaHoldingTime;
                if (!test_and_clear_op_flag(OperationalFlags.ZB12Modified) &&
                    (last_pkt != 0))
                {
                    time_sec -= last_pkt_diff;
                }

                if (time_sec < min_value_sec)
                {
                    min_value_sec = time_sec;
                }
            }
        }

        if ((reporting_flags & (uint)ReportingTriggers.DY9X) > 0 && (Timers.MeasDy9xd != 0))
        {
            diff_sec = now_sec - Timers.PeriodicMeasStart;
            time_sec = Timers.MeasDy9xd - diff_sec;
            if (time_sec < min_value_sec)
            {
                min_value_sec = time_sec;
            }
        }

        // Handle timer for Monitoring Time
        if (Timers.MonitoringTimeTs != 0)
        {
            diff_sec = now_sec - Timers.MonitoringTimeStart;
            time_sec = Timers.MonitoringTimeTs - diff_sec;
            if (time_sec < min_value_sec)
            {
                min_value_sec = time_sec;
            }
        }

        min_value_ms = UInt64.MaxValue;
        if (min_value_sec != UInt32.MaxValue)
        {
            if (min_value_sec * 1000 < min_value_ms)
            {
                min_value_ms = min_value_sec * 1000;
            }
        }
    }

    private bool test_and_clear_op_flag(OperationalFlags flag)
    {
        bool result = (this.operational_flags & (uint)flag) > 0;
        if (result)
            clear_operational_flag(flag);
        return result;
    }

    private void clear_operational_flag(OperationalFlags flag)
    {
        operational_flags &= ~(uint)flag;
    }

    private bool getOperationalFlagState(OperationalFlags flag)
    {
        return (operational_flags & (uint)flag) > 0;
    }
}