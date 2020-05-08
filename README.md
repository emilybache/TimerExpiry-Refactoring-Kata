Timer Expiry Refactoring Kata
=============================

You would like to make a change to the function `how_long_until_next_timer_expiry` - this change is described below. 
Before you make that change, you'd like to refactor the function to make it easier to make the change. (Make the change easy then make the easy change!) 
Before you do any refactoring, you'd like to have some test cases as insurance against mistakes. 

Adding tests
------------
There are three different starting points for tests, using different test frameworks. Choose one of them to build on. 
If you like, start over and build the same tests in a different framework. You might learn something about test design!

Refactoring
-----------
When you are confident in your test cases, refactor to remove duplication and to make adding the new timer easier (see below). 
Note - there is another branch in this repo called 'with_tests'. That branch has reasonable tests built with all three
frameworks that you can rely on while refactoring.

What this code does
-------------------

The `how_long_until_next_timer_expiry` function should work out the number of milliseconds until a timer will expire. It's used by another function that pauses execution of the program until then. (The calling code is not included in this exercise). The `how_long_until_next_timer_expiry` function goes through six timers, named:

- idt
- p88n
- Time Quota
- zb12
- dy9x
- Monitoring Time

It works out which of these timers are currently active, and how long until each one should expire. It returns the time left until the next one will expire, (via a parameter).


The change you need to make
---------------------------

Add a new timer, named bti. This timer should be enabled when duration measurements are active and the operational flag `OPERATIONAL_FLAG_BTI_PRESENT` is set. The timer should expire when the time since the last packet is equal to `bti_time_interval`. You can get the time of the last packet using the method `get_time_of_last_pkt`.

