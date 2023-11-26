
using System.Text;
using NUnit.Framework;
using Verifier = VerifyNUnit.Verifier;

namespace TestTimerExpiry;

[TestFixture]
public class TimerExpiryVerifyNUnitTest
{
    
    [Test]
    public Task HowLongUntilNextTimerExpiry()
    {
        // seconds since the epoch
        uint now_sec = 100;
        // in-parameter for when the next timer will expire, in milliseconds after now_sec
        ulong min_value_ms = UInt64.MaxValue;

        // the configuration of all the various timers
        var timerConfig = new TimerConfig();
        timerConfig.Timers = new Timers();
        timerConfig.Timers.Duration = new Duration();

        timerConfig.how_long_until_next_timer_expiry(timerConfig, now_sec, out min_value_ms);
        var sb = new StringBuilder();
        sb.Append(TimerConfigPrinter.PrintTimerConfig(timerConfig));
        sb.AppendLine("NEXT_TIMER: ");
        if (min_value_ms != UInt64.MaxValue) {
            sb.AppendLine(min_value_ms.ToString());
        } else {
            sb.AppendLine("LONG_MAX");
        }
        // no timers are set, so min_value_ms is not updated and remains at the value we set earlier
        return Verifier.Verify(sb.ToString());
    }
}