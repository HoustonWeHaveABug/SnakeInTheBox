# SnakeInTheBox

Solution to challenge https://www.reddit.com/r/dailyprogrammer/comments/7gvned/20171201_challenge_342_hard_snake_in_a_box/.

You should be able to compile this program on any platform implementing inttypes.h (gcc makefile provided).

The program reads 2 parameters on the standard input:
- The number of dimensions of the hypercube (minimum 1, maximum 31)
- Verbose mode (On = 1, Off = 0)

An exhaustive search is performed using the optimization described in this paper: http://cobweb.cs.uga.edu/~potter/CompIntell/kochut.pdf.

In addition, the number of valid vertices is maintained throughout the search. If the sum of this number plus the current path length is lower than the best path length, then the solver backtracks.

Completes the search for dimension 7 in 1h30 on my computer (Windows 7 - Intel Core I3 3.50 GHz).
