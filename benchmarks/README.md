# BENCHMARKS

Benchmarks have been realized with a *Intel(R) Core(TM) i5-3350P CPU @ 3.10GHz* (4 cores).
To compile and execute:
  ```
  clang++ -Wall -std=c++11 -obench bench.cpp -I.. -O2 && ./bench 1000 1000000
  ```

Each benchmark executes 1000 times a loop with 1000000 iterations where each iteration contains one dynamic_cast or one fastcast.
The structure is the following: B & C derived from A, D from B, E from D, F from E and G from F.
The tested pointer is typed G* and upcasted to A*.
The results are the following:

```
dynamic_cast<G*>((A*)G*):
1000 loops with 1000000 iterations: mean=9025(microsecs) and sigma=284.975
fastcast::cast<G>((A*)G*):
1000 loops with 1000000 iterations: mean=648(microsecs) and sigma=38.2242
fastcast is 13.9275 times faster.

dynamic_cast<B*>((A*)G*):
1000 loops with 1000000 iterations: mean=66273(microsecs) and sigma=339.866
fastcast::cast<B>((A*)G*):
1000 loops with 1000000 iterations: mean=1374(microsecs) and sigma=22.6188
fastcast is 48.2336 times faster.

dynamic_cast<C*>((A*)G*):
1000 loops with 1000000 iterations: mean=99025(microsecs) and sigma=234.17
fastcast::cast<C>((A*)G*):
1000 loops with 1000000 iterations: mean=1294(microsecs) and sigma=21.3689
fastcast is 76.5263 times faster.
```
