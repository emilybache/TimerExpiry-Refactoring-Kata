AlarmClock Refactoring Kata
=============================

You would like to make a change to the function `how_long_until_the_next_alarm` - this change is described below. Before you make that change, you'd like to refactor the function to make it easier to make the change. (Make the change easy then make the easy change!) Before you do any refactoring, you'd like to have some test cases as insurance against mistakes.


The change you need to make
---------------------------

Add a new alarm. This alarm should be enabled when duration measurements are active and the operational flag `OPERATIONAL_FLAG_BTI_PRESENT` is set and the time since the last packet is equal to `bti_time_interval`.

