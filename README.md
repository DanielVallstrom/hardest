# hardest — a solver for The Hardest Logic Puzzle Ever and its generalization

📄 **Paper**: [Solving The Hardest Logic Puzzle Ever and its generalizations](https://arxiv.org/abs/2201.09801) ([PDF](https://arxiv.org/pdf/2201.09801.pdf)) by Daniel Vallstrom

**hardest** is a solver for the classic Hardest Logic Puzzle Ever and its generalization to arbitrary numbers of gods.

## The Puzzle

Three gods — one truthful, one lying, and one random — answer yes/no questions using two unknown words (meaning "yes" and "no"). The challenge is to determine which god is which using as few questions as possible.

The generalization considers _n_ gods with arbitrary numbers of each type (false gods, true gods, and random gods). The key result from the paper is:

> **The puzzle is solvable if and only if the number of random gods is strictly fewer than the number of non-random gods** (this holds for arbitrary cardinals, including infinite).

## Building

**hardest** is a standard C program. Build with:

```sh
make
```

Source files: `hardest.c`, `hard.c`, `options.c`, `common.c`, `conjHash.c`, `readBounds.c`

Headers: `hard.h`, `conjHash.h`, `options.h`, `common.h`, `compilerMacros.h`, `readBounds.h`

Current version: **0.18.31** — Default compiler: `gcc`

## Usage

```
Usage: hardest [options] [outfile]
hardest tries to find the fewest questions that solve the generalization of
The Hardest Logic Puzzle Ever. The algorithm is derived from the paper
Solving The Hardest Logic Puzzle Ever and its generalizations.
  Gods are named g0, g1, ...
  To succinctly reproduce a solution, or see its questions, you can do a long
search, note the seed, the upper bound, and maybe the best lvl 0 pos estimate,
for the one best sub-search, and then rerun with that seed (-s), that upper bound (-u),
and that best lvl 0 pos estimate (-E), and with -v for the questions, and maybe -i 0.
```

Run `./hardest --help` for the full list of options.

### Key Options

| Option | Description |
|--------|-------------|
| `-f`   | Number of false (lying) gods |
| `-t`   | Number of true (truthful) gods |
| `-r`   | Number of random gods |
| `-v`   | Verbose — show the questions in the solution |
| `-s`   | Seed for reproducibility |
| `-u`   | Upper bound on expected number of questions |
| `-i`   | Number of iterations |
| `-b`   | Number of sub-search iterations |
| `-B`   | Level-specific sub-search iterations (e.g. `-B 0:4`) |
| `-H`   | Heuristic for estimating expected number of questions |
| `--help` | Show full help |

### Examples

Reproduce the optimal solution for the classic puzzle (with questions shown):
```sh
./hardest -v -f 0 -t 3 -r 2 -i 0 -b 0
```

Reproduce a current upper bound (2 false, 2 true, 3 random gods):
```sh
./hardest -f 2 -t 2 -r 3 -b 4 -i 999 -a 1.00001 -e 1.00 -K 1960 -S 8 -z -99 -U 4 -l 1.01 -Vno -s 6179805106105280146 -i 0 -u 8.9940099999999994 -M 1 -a 1.0000100000000001 -e 1
```

Show why the solution is optimal (1 false, 3 true, 1 random):
```sh
./hardest -f 1 -t 3 -r 1 -i 0 -b 0 -v
```

## Community / Upper Bounds

The [leaderboard](LEADERBOARD.md) tracks the best known upper bounds on the expected number of questions needed to solve the generalized puzzle. Community contributions of improved bounds are very welcome!

- See [LEADERBOARD.md](LEADERBOARD.md) for current best known values.
- See [CONTRIBUTING.md](CONTRIBUTING.md) for how to submit improved bounds, and a discussion of what options to use.
- Use [GitHub Discussions](../../discussions) to share ideas or ask questions.
- Use [GitHub Issues](../../issues) to report improved bounds or bugs.

## License

[Reciprocal Public License, version 1.1 (RPL-1.1)](LICENSE)

## Contact

daniel.vallstrom@gmail.com

## Citation

If you use this solver or the results in your work, please cite the paper:

Daniel Vallstrom. *Solving The Hardest Logic Puzzle Ever and its generalizations*. arXiv:2201.09801. https://arxiv.org/abs/2201.09801
