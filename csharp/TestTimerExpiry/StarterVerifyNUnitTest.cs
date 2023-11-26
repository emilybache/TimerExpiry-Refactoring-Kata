
using System.Text;
using NUnit.Framework;
using Verifier = VerifyNUnit.Verifier;

namespace TestTimerExpiry;

[TestFixture]
public class TimerExpiryVerifyNUnitTest
{
    private uint now_sec;
    private ulong min_value_ms;
    private TimerConfig timerConfig;

    [SetUp]
    public void Init()
    {
        // seconds since the epoch
        now_sec = 100;
        // in-parameter for when the next timer will expire, in milliseconds after now_sec
        min_value_ms = UInt64.MaxValue;

        timerConfig = new TimerConfig();
        timerConfig.Timers = new Timers();
        timerConfig.Timers.Duration = new Duration();

    }
    
    [Test]
    public Task NoTimersSet()
    {
        var beforeState = PrintBeforeState();
        
        timerConfig.how_long_until_next_timer_expiry(timerConfig, now_sec, out min_value_ms);
        
        var testState = PrintTestState();
        return Verifier.Verify(beforeState + testState);
    }    
    [Test]
    public Task ZB12Modified()
    {
        timerConfig.reporting_flags += (uint)ReportingTriggers.ZB12;
        timerConfig.Timers.QuotaHoldingTime = 7;
        timerConfig.clear_operational_flag(OperationalFlags.ZB12Stopped);
        timerConfig.operational_flags += (uint)OperationalFlags.ZB12Modified;
        var beforeState = PrintBeforeState();
        
        timerConfig.how_long_until_next_timer_expiry(timerConfig, now_sec, out min_value_ms);
        
        var testState = PrintTestState();
        return Verifier.Verify(beforeState + testState);
    }    
    [Test]
    public Task AllTimersSet()
    {
        timerConfig.Timers.Duration.MeasActive = true;
        timerConfig.Timers.Duration.MeasStart = now_sec - 1;
        timerConfig.last_pkt = now_sec;
        timerConfig.idt_alarm_time = 30;
        
        
        timerConfig.reporting_flags += (uint)ReportingTriggers.P88N;
        timerConfig.time_threshold = 26;
        
        timerConfig.operational_flags += (uint)OperationalFlags.TimeQuotaPresent;
        timerConfig.time_quota = 21;
        
        timerConfig.reporting_flags += (uint)ReportingTriggers.ZB12;
        timerConfig.Timers.QuotaHoldingTime = 17;
        timerConfig.clear_operational_flag(OperationalFlags.ZB12Stopped);
        
        timerConfig.reporting_flags += (uint)ReportingTriggers.DY9X;
        timerConfig.Timers.MeasDy9xd = 15;
        timerConfig.Timers.PeriodicMeasStart = now_sec - 2;

        timerConfig.Timers.MonitoringTimeTs = 7;
        timerConfig.Timers.MonitoringTimeStart = now_sec - 4;
        var beforeState = PrintBeforeState();
        
        timerConfig.how_long_until_next_timer_expiry(timerConfig, now_sec, out min_value_ms);
        
        var testState = PrintTestState();
        return Verifier.Verify(beforeState + testState);
    }

    private string PrintBeforeState()
    {
        var sb = new StringBuilder();
        sb.AppendLine("BEFORE: ");
        sb.Append(TimerConfigPrinter.PrintTimerConfig(timerConfig));
        return sb.ToString();
    }    
    private string PrintTestState()
    {
        var sb = new StringBuilder();
        sb.AppendLine("AFTER: ");
        sb.Append(TimerConfigPrinter.PrintTimerConfig(timerConfig));
        sb.AppendLine("NEXT_TIMER: ");
        if (min_value_ms != UInt64.MaxValue)
        {
            sb.AppendLine(min_value_ms.ToString());
        }
        else
        {
            sb.AppendLine("LONG_MAX");
        }

        return sb.ToString();
    }
}