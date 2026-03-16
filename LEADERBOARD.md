# Upper Bounds Leaderboard

This file tracks upper bounds on the expected number of questions needed to solve the generalized "Hardest Logic Puzzle Ever".

- Values marked **Optimal** are proven optimal.
- Values marked *Conjectured* are conjectured optimal.
- Values marked as Upper bound may be improvable — you are welcome to try!

See [CONTRIBUTING.md](CONTRIBUTING.md) for how to submit improved bounds.

## How to Reproduce

To reproduce a solution you need:

- Solver version: `./hardest --version`
- The full invocation command with all flags used
- The seed (`-s`) from the state before the search that found the solution
- The upper bound (`-u`) from the state before the search that found the solution
- The best lvl 0 pos estimate (`-E`) if relevant

These values are printed together with a solution, so they should be easy to find and post.

Per-entry reproduction commands are listed in the [Reproduction Commands](#reproduction-commands) section below, and also in the `reproduce_command` column of [`best_known_bounds.csv`](best_known_bounds.csv).

**Example:**
```sh
./hardest -f 1 -t 6 -r 2 -i 5000 -B 0:4 -B 1:3 -B 3:2 -B 4:1 -B 5:1 -i 0 -s 13030756753776470731 -u 8.299603174603174 -N 2
```

## Note on Symmetry

`E[Qs]` for _k_ false gods and _m_ true gods equals `E[Qs]` for _m_ false gods and _k_ true gods, for symmetry reasons.

## Results

| \|F\| | \|T\| | \|R\| | E[Qs] | Status | Notes |
|------:|------:|------:|------:|--------|-------|
| 0 | 2 | 1 | **2.0** | **Optimal** | |
| 1 | 1 | 1 | **3.0** | **Optimal** | |
| 0 | 3 | 1 | **2.375** | **Optimal** | |
| 1 | 2 | 1 | **3.916667** | **Optimal** | |
| 0 | 4 | 1 | **2.6** | **Optimal** | |
| 1 | 3 | 1 | **4.6** | **Optimal** | |
| 2 | 2 | 1 | **5.133333** | **Optimal** | |
| 0 | 3 | 2 | *4.13750* | *Conjectured* | |
| 1 | 2 | 2 | *5.683333* | *Conjectured* | |
| 0 | 5 | 1 | **2.833333** | **Optimal** | |
| 1 | 4 | 1 | **5.1** | **Optimal** | |
| 2 | 3 | 1 | **6.1** | **Optimal** | |
| 0 | 4 | 2 | *4.4* | *Conjectured* | |
| 1 | 3 | 2 | *6.4* | *Conjectured* | |
| 2 | 2 | 2 | *7.055556* | *Conjectured* | |
| 0 | 6 | 1 | **3.0** | **Optimal** | |
| 1 | 5 | 1 | **5.619048** | **Optimal** | |
| 2 | 4 | 1 | **6.923810** | **Optimal** | |
| 3 | 3 | 1 | **7.314286** | **Optimal** | |
| 0 | 5 | 2 | *4.904762* | *Conjectured* | |
| 1 | 4 | 2 | *7.161905* | *Conjectured* | |
| 2 | 3 | 2 | 8.161905 | Upper bound | |
| 0 | 4 | 3 | 6.232143 | Upper bound | |
| 1 | 3 | 3 | 8.393973 | Upper bound | |
| 2 | 2 | 3 | 9.049702 | Upper bound | |
| 0 | 7 | 1 | **3.1875** | **Optimal** | |
| 1 | 6 | 1 | **5.982143** | **Optimal** | |
| 2 | 5 | 1 | **7.601190** | **Optimal** | |
| 3 | 4 | 1 | **8.296429** | **Optimal** | |
| 0 | 6 | 2 | *5.178571* | *Conjectured* | |
| 1 | 5 | 2 | *7.833333* | *Conjectured* | |
| 2 | 4 | 2 | 9.109524 | Upper bound | |
| 3 | 3 | 2 | 9.535714 | Upper bound | |
| 0 | 5 | 3 | 6.571429 | Upper bound | |
| 1 | 4 | 3 | 9.058482 | Upper bound | |
| 2 | 3 | 3 | 10.091741 | Upper bound | |
| 0 | 8 | 1 | **3.333333** | **Optimal** | |
| 1 | 7 | 1 | **6.333333** | **Optimal** | |
| 2 | 6 | 1 | **8.095238** | **Optimal** | |
| 3 | 5 | 1 | **9.095238** | **Optimal** | |
| 4 | 4 | 1 | **9.485714** | **Optimal** | |
| 0 | 7 | 2 | *5.527778* | *Conjectured* | |
| 1 | 6 | 2 | *8.273810* | *Conjectured* | |
| 2 | 5 | 2 | 9.957672 | Upper bound | |
| 3 | 4 | 2 | 10.692857 | Upper bound | |
| 0 | 6 | 3 | *7.059524* | *Conjectured* | |
| 1 | 5 | 3 | 9.721230 | Upper bound | |
| 2 | 4 | 3 | 11.076984 | Upper bound | |
| 3 | 3 | 3 | 11.446726 | Upper bound | |
| 0 | 5 | 4 | 8.470703 | Upper bound | |
| 1 | 4 | 4 | 11.057425 | Upper bound | |
| 2 | 3 | 4 | 12.136675 | Upper bound | |
| 0 | 9 | 1 | **3.5** | **Optimal** | |
| 1 | 8 | 1 | **6.677778** | **Optimal** | |
| 2 | 7 | 1 | **8.677778** | **Optimal** | |
| 3 | 6 | 1 | **9.880952** | **Optimal** | |
| 4 | 5 | 1 | **10.474603** | **Optimal** | |
| 0 | 8 | 2 | *5.844444* | *Conjectured* | |
| 1 | 7 | 2 | *8.844444* | *Conjectured* | |
| 2 | 6 | 2 | 10.650000 | Upper bound | |
| 3 | 5 | 2 | 11.654365 | Upper bound | |
| 4 | 4 | 2 | 11.978095 | Upper bound | |
| 0 | 7 | 3 | *7.425000* | *Conjectured* | |
| 1 | 6 | 3 | 10.298810 | Upper bound | |
| 2 | 5 | 3 | 12.000992 | Upper bound | |
| 3 | 4 | 3 | 12.698929 | Upper bound | |
| 0 | 6 | 4 | 8.941220 | Upper bound | |
| 1 | 5 | 4 | 11.646063 | Upper bound | |
| 2 | 4 | 4 | 13.035486 | Upper bound | |
| 3 | 3 | 4 | 13.419905 | Upper bound | |
| 0 | 10 | 1 | **3.636364** | **Optimal** | |
| 1 | 9 | 1 | **6.927273** | **Optimal** | |
| 2 | 8 | 1 | **9.056566** | **Optimal** | |
| 3 | 7 | 1 | **10.539394** | **Optimal** | |
| 4 | 6 | 1 | **11.317749** | **Optimal** | |
| 5 | 5 | 1 | **11.613276** | **Optimal** | |
| 0 | 9 | 2 | *6.054545* | *Conjectured* | |
| 1 | 8 | 2 | 9.189899 | Upper bound | |
| 2 | 7 | 2 | 11.220960 | Upper bound | |
| 3 | 6 | 2 | 12.658550 | Upper bound | |
| 4 | 5 | 2 | 13.051659 | Upper bound | |
| 0 | 8 | 3 | 7.927273 | Upper bound | |
| 1 | 7 | 3 | 10.975000 | Upper bound | |
| 2 | 6 | 3 | 12.769913 | Upper bound | |
| 3 | 5 | 3 | 12.772294 | Upper bound | |
| 4 | 4 | 3 | 14.081667 | Upper bound | |
| 0 | 7 | 4 | 9.292424 | Upper bound | |
| 1 | 6 | 4 | 12.247592 | Upper bound | |
| 2 | 5 | 4 | 13.924281 | Upper bound | |
| 3 | 4 | 4 | 14.631212 | Upper bound | |
| 0 | 6 | 5 | 10.905836 | Upper bound | |
| 1 | 5 | 5 | 13.767661 | Upper bound | |
| 2 | 4 | 5 | 15.170539 | Upper bound | |
| 3 | 3 | 5 | 15.600163 | Upper bound | |
| 0 | 11 | 1 | **3.750000** | **Optimal** | |
| 1 | 10 | 1 | **7.143939** | **Optimal** | |
| 2 | 9 | 1 | **9.531818** | **Optimal** | |
| 3 | 8 | 1 | **11.048990** | **Optimal** | |
| 4 | 7 | 1 | **12.048990** | **Optimal** | |
| 5 | 6 | 1 | **12.605700** | **Optimal** | |
| 0 | 10 | 2 | *6.272727* | *Conjectured* | |
| 1 | 9 | 2 | 9.660606 | Upper bound | |
| 2 | 8 | 2 | 11.839394 | Upper bound | |
| 3 | 7 | 2 | 13.203157 | Upper bound | |
| 4 | 6 | 2 | 14.030213 | Upper bound | |
| 5 | 5 | 2 | 14.279161 | Upper bound | |
| 2 | 5 | 5 | 15.933610 | Upper bound | |
| 3 | 4 | 5 | 16.677641 | Upper bound | |
| 0 | 12 | 1 | **3.846154** | **Optimal** | |
| 0 | 13 | 1 | **3.928571** | **Optimal** | |
| 0 | 14 | 1 | **4.0** | **Optimal** | |
| 0 | 15 | 1 | **4.093750** | **Optimal** | |
| 0 | 16 | 1 | **4.176471** | **Optimal** | |
| 0 | 17 | 1 | **4.277778** | **Optimal** | |
| 0 | 18 | 1 | **4.368421** | **Optimal** | |
| 0 | 19 | 1 | **4.450000** | **Optimal** | |
| 0 | 20 | 1 | **4.523810** | **Optimal** | |
| 0 | 25 | 1 | **4.807692** | **Optimal** | |
| 0 | 30 | 1 | **5.0** | **Optimal** | |
| 0 | 35 | 1 | **5.25** | **Optimal** | |
| 0 | 40 | 1 | **5.463415** | **Optimal** | |
| 0 | 45 | 1 | **5.630435** | **Optimal** | |
| 0 | 50 | 1 | **5.764706** | **Optimal** | |
| 0 | 60 | 1 | **5.967213** | **Optimal** | |
| 0 | 62 | 1 | **6.0** | **Optimal** | |
| 0 | 70 | 1 | **6.211268** | **Optimal** | |
| 0 | 80 | 1 | **6.432099** | **Optimal** | |
| 0 | 90 | 1 | **6.604396** | **Optimal** | |
| 0 | 100 | 1 | **6.742574** | **Optimal** | |
| 0 | 126 | 1 | **7.0** | **Optimal** | |
| 0 | 150 | 1 | **7.311258** | **Optimal** | |
| 0 | 200 | 1 | **7.731343** | **Optimal** | |
| 0 | 250 | 1 | **7.984064** | **Optimal** | |
| 0 | 254 | 1 | **8.0** | **Optimal** | |
| 0 | 11 | 2 | *6.551282* | *Conjectured* | |
| 0 | 12 | 2 | *6.769231* | *Conjectured* | |
| 0 | 13 | 2 | *6.942857* | *Conjectured* | |
| 0 | 14 | 2 | *7.075000* | *Conjectured* | |
| 0 | 15 | 2 | *7.257353* | *Conjectured* | |
| 0 | 16 | 2 | *7.457516* | *Conjectured* | |
| 0 | 17 | 2 | *7.625731* | *Conjectured* | |
| 0 | 18 | 2 | *7.768421* | *Conjectured* | |
| 0 | 19 | 2 | *7.890476* | *Conjectured* | |
| 0 | 20 | 2 | *7.995671* | *Conjectured* | |

## Reproduction Commands

Commands to quickly reproduce specific upper bounds. Each command runs a single deterministic replay (via `-s SEED -i 0`) and should complete in seconds.

The `-u` value does not need updating when `-H 1` is used (it is biased low).

| \|F\| | \|T\| | \|R\| | E[Qs] | Version | Reproduce Command |
|------:|------:|------:|------:|---------|-------------------|
| 2 | 5 | 2 | 9.957672 | 0.15.5 | `./hardest -f 2 -t 5 -r 2 -b 8 -B 0:9 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 9.91 -i 1000 -s 2318762516960874543 -i 0` |
