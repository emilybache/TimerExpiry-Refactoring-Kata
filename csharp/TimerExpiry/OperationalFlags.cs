namespace TestTimerExpiry;

public enum OperationalFlags : uint
{
    TimeQuotaPresent = 1,
    ZB12Stopped = 2,
    ZB12Modified = 4,
    BTIPresent = 8
}