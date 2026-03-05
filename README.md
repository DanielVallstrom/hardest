# hardest — a solver for "The Hardest Logic Puzzle Ever" and its generalization

📄 **Paper**: [How to Solve "The Hardest Logic Puzzle Ever" and Its Generalization](https://arxiv.org/abs/2201.09801) ([PDF](https://arxiv.org/pdf/2201.09801.pdf)) by Daniel Vallstrom

**hardest** is a solver for the classic "Hardest Logic Puzzle Ever" and its generalization to arbitrary numbers of gods.

## The Puzzle

Three gods — one truthful, one lying, and one random — answer yes/no questions using two unknown words (meaning "yes" and "no"). The challenge is to determine which god is which using as few questions as possible.

The generalization considers _n_ gods with arbitrary numbers of each type (false gods, true gods, and random gods). The key result from the paper is:

> **The puzzle is solvable if and only if the number of random gods is strictly fewer than the number of non-random gods** (this holds for arbitrary cardinals, including infinite).

## Building

**hardest** is a standard C program. Build with:

```sh
make
```

Source files: `hardest.c`, `hard.c`, `options.c`, `common.c`, `conjHash.c`

Headers: `hard.h`, `conjHash.h`, `options.h`, `common.h`, `compilerMacros.h`

Current version: **0.12.4** — Default compiler: `gcc`

## Usage

```
Usage: hardest [options] [outfile]
hardest tries to find the fewest questions that solve the generalization of
"The hardest logic puzzle ever". The algorithm is derived from the paper
'How to Solve "The Hardest Logic Puzzle Ever" and Its Generalization'.
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
| `-E`   | Best lvl 0 pos estimate |
| `-N`   | Node limit |
| `-i`   | Number of iterations |
| `-b`   | Number of sub-search iterations |
| `-B`   | Level-specific sub-search iterations (e.g. `-B 0:4`) |
| `--help` | Show full help |

### Examples

Reproduce the optimal solution for the classic puzzle (with questions shown):
```sh
./hardest -v -f 0 -t 3 -r 2 -i 0 -b 0 -s 4
```

Reproduce a current upper bound (1 false, 6 true, 2 random gods):
```sh
./hardest -f 1 -t 6 -r 2 -i 5000 -B 0:4 -B 1:3 -B 3:2 -B 4:1 -B 5:1 -i 0 -s 13030756753776470731 -u 8.299603174603174 -N 2
```

Reproduce another current upper bound (1 false, 5 true, 4 random gods):
```sh
./hardest -f 1 -t 5 -r 4 -i 2000 -B 0:4 -B 1:3 -B 3:2 -B 4:1 -B 5:1 -i 0 -s 13210365863729163090 -u 12.363476562500001 -B 6:1 -N 2
```

Show why the optimal solution is optimal (1 false, 3 true, 1 random):
```sh
./hardest -f 1 -t 3 -r 1 -i 0 -b 0 -v
```

## Community / Upper Bounds

The [leaderboard](LEADERBOARD.md) tracks the best known upper bounds on the expected number of questions needed to solve the generalized puzzle. Community contributions of improved bounds are very welcome!

- See [LEADERBOARD.md](LEADERBOARD.md) for current best known values.
- See [CONTRIBUTING.md](CONTRIBUTING.md) for how to submit improved bounds.
- Use [GitHub Discussions](../../discussions) to share ideas or ask questions.
- Use [GitHub Issues](../../issues) to report improved bounds or bugs.

## License

[Reciprocal Public License, version 1.1 (RPL-1.1)](LICENSE)

## Contact

daniel.vallstrom@gmail.com

## Citation

If you use this solver or the results in your work, please cite the paper:

Daniel Vallstrom. *How to Solve "The Hardest Logic Puzzle Ever" and Its Generalization*. arXiv:2201.09801. https://arxiv.org/abs/2201.09801