## Watcher

Prints whether D3 is low or high four times a second.

Note that AVR pins are very high impedance so, if you're not driving
them (either externally or with the internal pullup), they will switch
for any reason.  Try this: leave D0, D1, and D2 disconnected, print
their state, drive D3 high, then print them again.  They will likely
change state even though they have nothing to do with D3.


