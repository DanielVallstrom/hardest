# Contributing

Contributions of improved upper bounds are very welcome!

To improve an upper bound, now, you typically, first, try to home in on good abort leeway values, aiming, maybe, to get almost all searches to abort, e.g. by using the -K 960 option. Then you can fix good abort parameters, with, say, -K 1960. Next you try to find out which of -S 7 and -S 8 is best --- or, possibly, even -S 6. For -S 8, you also test different, small, values of -U m. Then you trade -b n for -i k --- start with -b 0, maybe. See the replication commands in [`best_known_bounds.csv`](best_known_bounds.csv). 

After you find a bound, you can milk the solution: Use the milk mode, -1. You'll want to base your milking on the solution, but alter the number of sub-searches that you take the most promising search from a little, i.e. changing the -B settings. For example, if -b 3 was used to find the bound, try combinations around that setting, for a few levels, e.g. -1 -b 3 -i 3 -B 0:4 -B 1:4 -B 2:4 -B 3:4 -2 b-2. Again, see [`best_known_bounds.csv`](best_known_bounds.csv). 

If your bound didn't make it into the bounds file, you can still milk it by using the -3 option, and providing base command options yourself. Typically, this means taking a search command that you want to base the milking on, add seed, and maybe upper bound, for the one best sub-search, and perhaps -a and -e options, like in a replication command. Then add b, B, 0, or 2 options, and -i (and the milking option, -1). For instance, to try +-1 combinations of your B settings for a few, 4, levels, start with your base search command, add seed, and then add -1 -3 -i 3 -b b+1 -0 b-2, where the order of the last three options matter.

For example, the current replication command for the 0-8-4 problem is

> ./hardest -f 0 -t 8 -r 4 -i 999 -b 5 -B 0:6 -S 8 -H 1 -U 3 -a 0.9952 -e 0.9951 -K 1960 -s 6532526679816657539 -1 -3 -i 5 -b b+1 -0 b-2 -1no -B 0:6 -B 1:5 -B 2:5 -B 3:5 -B 4:6 -B 5:5 -s 6532526679816657539 -i 0 -u 9.8343429999999987 -M 1 -a 0.99519999999999997 -e 0.99509999999999998 -i 0

That came about by first doing a normal search, eventually settling on the command 

> ./hardest -f 0 -t 8 -r 4 -i 999 -b 5 -B 0:6 -S 8 -H 1 -U 3 -a 0.9952 -e 0.9951 -K 1960

After a few tries a bound fairly close to, but still worse than, the then upper bound was found, with the one search that found the close bound having seed 6532526679816657539. After that, the new solution was milked, using the command 

> ./hardest -f 0 -t 8 -r 4 -i 999 -b 5 -B 0:6 -S 8 -H 1 -U 3 -a 0.9952 -e 0.9951 -K 1960 -s 6532526679816657539 -1 -3 -i 5 -b b+1 -0 b-2

The milking then, eventually, found the current bound (adding the state at the start of the one search that found the new bound to the end of the replication command for a succinct replication).

You can remilk a milked solution, all the way down to the level where the -B option has no effect, i.e. where asked gods are never random. For instance, the replication command for the 3-5-4 problem is

> ./hardest -f 3 -t 5 -r 4 -i 2999 -b 1 -B 0:2 -S 8 -H 1 -U 3 -a 0.9991 -e 0.999 -K 1960 -s 15356446695531426074 -1 -3 -i 13 -b b+1 -0 b-2 -B 0:3 -B 1:2 -B 2:2 -B 3:1 -B 4:1 -2 0:3 -2 1:2 -2 2:2 -2 3:1 -2 4:1 -B 5:1 -B 6:0 -B 7:0 -B 8:0 -B 9:0 -2 5:1 -2 6:0 -2 7:0 -2 8:0 -2 9:0 -u 15.724351 -1no -B 0:3 -B 1:2 -B 2:2 -B 3:1 -B 4:1 -B 5:1 -B 6:0 -B 7:0 -B 8:0 -B 9:0 -B 10:2 -B 11:0 -B 12:0 -B 13:0 -s 15356446695531426074 -i 0 -u 15.724350999999997 -M 1 -a 0.99909999999999999 -e 0.999 -i 0

That solution was found by first doing a normal search, then milking that solution by iterating over the first 5 B levels, then taking the first 5 B values in the search that found the best bound, namely -B 0:3 -B 1:2 -B 2:2 -B 3:1 -B 4:1, fixing them, and then iterating over the next 5 levels. That found an improved upper bound. However, you can milk that new bound further, by iterating over the last levels in play, and remembering to add the old upper bound to the options, -u 15.724351, since the new bound was written to the bounds file, which would push the search off the base path to be milked. This third milking produced the current bound.

Regarding which estimate heuristic, -H, to use, while the probabilistic -H 0 might be more theoretically sound and appealing, and maybe more accurate, -H 1 might sometimes be more robust --- less random. You are not looking for accuracy anyway, but rather robustness and consistency, usefulness. Accuracy is as hard as the underlying problem in any case.

Maybe don't mess with the upper bound -u option, partly because it's good to read upper bounds from the bounds file. You can use leeway options -a and -e instead. You can set them to less than 1 if you want, e.g. when you use an estimate heuristic that is biased low, e.g. -H 1. 

## How to Submit Improved Bounds

There are three ways to submit an improved bound:

### 1. Pull Request (preferred)

Update [`best_known_bounds.csv`](best_known_bounds.csv) and [`LEADERBOARD.md`](LEADERBOARD.md) with your improved bound, including full reproduction information (see below). Open a pull request with these changes. Easiest is to just run the program and upload [`best_known_bounds.csv`](best_known_bounds.csv) if you find any improvement; [`best_known_bounds.csv`](best_known_bounds.csv) is updated automatically with default options. Then you also need to update [`LEADERBOARD.md`](LEADERBOARD.md), which you do with, e.g., "make leaderboard".

### 2. GitHub Issue

Open a [GitHub Issue](../../issues) with your result and reproduction information.

### 3. GitHub Discussions

Post your result in [GitHub Discussions](../../discussions).

## What to Include

The following information is critical for reproducibility. These values are printed together with a solution, so they should be easy to find and post.

- **Problem instance**: number of false (`-f`), true (`-t`), and random (`-r`) gods
- **Expected number of questions** (E[Qs]) achieved
- **Reproduction information:**
  - Solver version (`./hardest --version`)
  - The full invocation command with all flags and options used
  - The seed (`-s`) from the state before the search that found the solution
  - The upper bound (`-u`) from the state before the search that found the solution
  - The best lvl 0 pos estimate (`-E`) if relevant
  - Any other non-default state values used (typically `-s` and `-u` suffice)

### Example Submission

```
Problem: 3-4-2 (3 false, 4 true, 2 random gods)
E[Qs]: 10.692857
Solver version: 0.15.5
Reproduction: ./hardest -f 3 -t 4 -r 2 -b 8 -B 0:9 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 10.57 -i 5000 -s 16955508027985230318 -i 0
```

## Status Markers

- **Optimal** (bold in leaderboard): Proven optimal — cannot be improved.
- *Conjectured* (italic in leaderboard): Believed to be optimal but not yet proven.
- Upper bound (plain in leaderboard): May be improvable — this is where your contributions are most valuable!
