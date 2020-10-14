#define APPROVALS_GOOGLETEST

#include <gtest/gtest.h>
#include "ApprovalTests.hpp"
#include "printers.h"

extern "C"
{
#include "timers.h"
}

class TimerExpiryTest : public ::testing::Test {
protected:
    void SetUp() override {
        timerConfig = new timer_config();
        timerConfig->timers = new timers();
        timerConfig->timers->duration = new duration();
    }

    void TearDown() override {
        delete timerConfig;
    }

    // the timer configuration for this test case
    timer_config *timerConfig;
    // seconds since the epoch
    unsigned int now_sec = 100;
    // in-parameter for when the next timer will expire, in milliseconds after now_sec
    unsigned long min_value_ms = LONG_MAX;

};

TEST_F(TimerExpiryTest, NoTimersSet) {
    how_long_until_next_timer_expiry(timerConfig, now_sec, &min_value_ms);

    std::stringstream to_approve;
    to_approve << *timerConfig;
    to_approve << "NEXT_TIMER: ";

    if (min_value_ms != LONG_MAX) {
        to_approve << min_value_ms;
    } else {
        to_approve << "LONG_MAX";
    }

    auto t = to_approve.str();
    ApprovalTests::Approvals::verify(t);
}
