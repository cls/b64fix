Base64 Fixpoint
===============

A few days ago I saw someone by the name of **p4bl0** mention that the Base64
encoding has a fixpoint. That is, if you repeatedly Base64-encode any string,
the result will tend towards the same unique (infinite) string of characters.

This caught my interest. I wrote this C++ program to compute that fixpoint up
to a specified precision (in sextets). It does this by, instead of repeatedly
Base64-encoding a string, simply keeping track of the delta between the 8-bit
input and 6-bit output (prior to table lookup).

The result is reasonably fast:

    cls@chalk$ n=$((32*1024*1024)) # 32 MiB
    cls@chalk$ time ./b64fix $n > b64fix.txt
    real    0m0.888s
    user    0m0.772s
    sys     0m0.099s
    cls@chalk$ cmp -n $((n*4/3)) b64fix.txt <(base64 -w 0 b64fix.txt)
    cls@chalk$

For more information, see https://news.ycombinator.com/item?id=37987225
