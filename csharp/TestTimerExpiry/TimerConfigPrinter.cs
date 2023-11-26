using System.Text;

namespace TestTimerExpiry;

public class TimerConfigPrinter
{
    public static string PrintTimerConfig(TimerConfig timerConfig)
    {
        StringBuilder sb = new StringBuilder();
        sb.AppendLine($"operational flags: {timerConfig.operational_flags}");
        sb.AppendLine($"reporting flags: {timerConfig.reporting_flags}");
        if (timerConfig.last_pkt > 0)
            sb.AppendLine($"last packet: {timerConfig.last_pkt}");
        if (timerConfig.idt_alarm_time != 0)
            sb.AppendLine($"idt alarm time: {timerConfig.idt_alarm_time}");
        if (timerConfig.time_threshold != 0)
        {
            sb.AppendLine($"time threshold: {timerConfig.time_threshold}");
            sb.AppendLine($"duration meas: {timerConfig.Timers.Duration.Meas}");
            sb.AppendLine($"duration meas threshold: {timerConfig.Timers.Duration.MeasThresholdUsed}");
        }
        if (timerConfig.time_quota != 0)
        {
            sb.AppendLine($"time quota: {timerConfig.time_quota}");
            sb.AppendLine($"duration meas: {timerConfig.Timers.Duration.Meas}");
        }
        if (timerConfig.Timers.QuotaHoldingTime != 0)
        {
            sb.AppendLine($"time quota holding time: {timerConfig.Timers.QuotaHoldingTime}");
        }
        if (timerConfig.Timers.MeasDy9xd != 0)
        {
            sb.AppendLine($"DY9Xd: {timerConfig.Timers.MeasDy9xd}");
            sb.AppendLine($"periodic meas start: {timerConfig.Timers.PeriodicMeasStart}");
        }
        if (timerConfig.Timers.MonitoringTimeTs != 0)
        {
            sb.AppendLine($"monitoring time start: {timerConfig.Timers.MonitoringTimeStart}");
            sb.AppendLine($"monitoring time ts: {timerConfig.Timers.MonitoringTimeTs}");
        }
        return sb.ToString();
    }

}