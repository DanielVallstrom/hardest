# Contributing

Contributions of improved upper bounds are very welcome!

## How to Submit Improved Bounds

There are three ways to submit an improved bound:

### 1. Pull Request (preferred)

Update [`best_known_bounds.csv`](best_known_bounds.csv) and [`LEADERBOARD.md`](LEADERBOARD.md) with your improved bound, including full reproduction information (see below). Open a pull request with these changes.

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
Problem: 1-6-2 (1 false, 6 true, 2 random gods)
E[Qs]: 8.273810
Solver version: 0.12.3
Reproduction: ./hardest -f 1 -t 6 -r 2 -i 5000 -B 0:4 -B 1:3 -B 3:2 -B 4:1 -B 5:1 -i 0 -s 13030756753776470731 -u 8.299603174603174
```

## Status Markers

- **Optimal** (bold in leaderboard): Proven optimal — cannot be improved.
- *Conjectured* (italic in leaderboard): Believed to be optimal but not yet proven.
- Upper bound (plain in leaderboard): May be improvable — this is where your contributions are most valuable!
