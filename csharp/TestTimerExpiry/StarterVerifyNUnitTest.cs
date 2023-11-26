
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
        ulong min_value_ms = Int64.MaxValue;

        // the configuration of all the various timers
        var timerConfig = new TimerConfig();
        timerConfig.Timers = new Timers();
        timerConfig.Timers.Duration = new Duration();

        timerConfig.how_long_until_next_timer_expiry(timerConfig, now_sec, out min_value_ms);

        // no timers are set, so min_value_ms is not updated and remains at the value we set earlier
        return Verifier.Verify(min_value_ms);
    }
}