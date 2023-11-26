using NUnit.Framework;
using Assert = NUnit.Framework.Assert;

namespace TestTimerExpiry;

using NUnit;

[TestFixture]
public class TimerExpiryNUnitTest
{
    [Test]
    public void HowLongUntilNextTimerExpiry()
    {
        // seconds since the epoch
        uint now_sec = 100;
        // in-parameter for when the next timer will expire, in milliseconds after now_sec
        ulong min_value_ms = UInt64.MaxValue;

        // the configuration of all the various timers
        var timerConfig = new TimerConfig();
        timerConfig.Timers = new Timers();
        timerConfig.Timers.Duration = new Duration();

        TimerConfig.how_long_until_next_timer_expiry(timerConfig, now_sec, out min_value_ms);

        // no timers are set, so min_value_ms is not updated and remains at the value we set earlier
        Assert.AreEqual(min_value_ms, UInt64.MaxValue);
    }
}