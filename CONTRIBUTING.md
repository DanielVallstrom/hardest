# Contributing

Contributions of improved upper bounds are very welcome!

To improve an upper bound, now, you typically, first, try to home in on good abort leeway values, aiming, maybe, to get almost all searches to abort, e.g. by using the -K 960 option. Then you can fix good abort parameters, with, say, -K 1960. Next you try to find out which of -S 7 and -S 8 is best --- or, possibly, even -S 6. For -S 8, you also test different, small, values of -U m. Then you trade -b n for -i k --- start with -b 0. See the replication commands in [`best_known_bounds.csv`](best_known_bounds.csv). 

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
