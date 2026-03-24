# Upper Bounds Leaderboard

This file tracks upper bounds on the expected number of questions needed to solve the generalized "Hardest Logic Puzzle Ever".

- Values marked **Optimal** are proven optimal.
- Values marked *Conjectured* are conjectured optimal.
- Values marked *Likely* could very well be optimal, too.
- Values marked as Upper bound may be improvable — you are welcome to try!

See [CONTRIBUTING.md](CONTRIBUTING.md) for how to submit improved bounds. Easiest is to just run the program and upload [best_known_bounds.csv](best_known_bounds.csv) if you find any improvement; best_known_bounds.csv is updated automatically with default options. However, then bounds in this file might be outdated --- and might stay outdated until it's updated by an AI agent.

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
./hardest -f 2 -t 5 -r 2 -b 8 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 -u 9.91 -i 1000 -s 2318762516960874543 -i 0
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
| 1 | 6 | 2 | 8.*273810* | *Likely* | |
| 2 | 5 | 2 | 9.957672 | Upper bound | |
| 3 | 4 | 2 | 10.692857 | Upper bound | |
| 0 | 6 | 3 | 7.*059524* | *Likely* | |
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
| 1 | 7 | 2 | 8.*844444* | *Likely* | |
| 2 | 6 | 2 | 10.650000 | Upper bound | |
| 3 | 5 | 2 | 11.654365 | Upper bound | |
| 4 | 4 | 2 | 11.978095 | Upper bound | |
| 0 | 7 | 3 | 7.*425000* | *Likely* | |
| 1 | 6 | 3 | 10.298810 | Upper bound | |
| 2 | 5 | 3 | 12.000992 | Upper bound | |
| 3 | 4 | 3 | 12.698929 | Upper bound | |
| 0 | 6 | 4 | 8.941220 | Upper bound | |
| 1 | 5 | 4 | 11.646063 | Upper bound | |
| 2 | 4 | 4 | 13.025176 | Upper bound | |
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
| 3 | 5 | 3 | 13.773593 | Upper bound | |
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
| 0 | 9 | 3 | 8.195455 | Upper bound | |
| 1	| 8	| 3	| 11.404040	| Upper bound	| |
| 2	| 7	| 3	| 13.427841	| Upper bound	| |
| 3	| 6	| 3	| 14.705303	| Upper bound | |
| 4	| 5	| 3	| 15.239854	| Upper bound | |
| 0	| 8	| 4	| 9.840404	| Upper bound | |
| 1	| 7	| 4	| 12.950126	| Upper bound | |
| 2	| 6	| 4	| 14.733135	| Upper bound | |
| 3	| 5	| 4	| 15.746904	| Upper bound | |
| 4	| 4	| 4	| 16.068750	| Upper bound | |
| 0 | 7 | 5 | 11.249620 | Upper bound | |
| 1 | 6 | 5 | 14.249496 | Upper bound | |
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

Commands to quickly reproduce specific upper bounds. Each command runs a single deterministic replay (via `-s SEED -i 0`) and should, typically, complete in seconds. However, harder problems could take longer.

The `-u` value does not need updating when `-H 1` is used (it is biased low).

| \|F\| | \|T\| | \|R\| | E[Qs] | Version | Reproduce Command |
|------:|------:|------:|------:|---------|-------------------|
| &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2 | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;3 | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2 | 8.161905 | 0.15.5 | `./hardest -f 2 -t 3 -r 2 -b 4 -H 1 -g 3 -i 3000 -S 6 -U 4 -s 4604948742119086823 -i 0` |
| 0 | 4 | 3 | 6.232143 | 0.15.4 | `./hardest -f 0 -t 4 -r 3 -b 5 -B 0:6 -k 20 -H 1 -g 3 -i 70000 -S 7 -U 2 -a 1.02 -e 1.01 -u 6.148 -W 0.0 -s 8347231925438940503 -i 0` |
| 1 | 3 | 3 | 8.393973 | 0.15.4 | `./hardest -f 1 -t 3 -r 3 -b 5 -B 0:6 -H 1 -i 7000 -S 7 -U 2 -a 1.02 -e 1.01 -s 2557919246952201673 -i 0` |
| 2 | 2 | 3 | 9.049702 | 0.15.4 | `./hardest -f 2 -t 2 -r 3 -b 5 -B 0:6 -H 1 -i 8 -S 7 -U 2 -a 1.02 -e 1.01 -u 12 -s 9505483977630049913 -i 0` |
| 2 | 4 | 2 | 9.109524 | 0.15.5 | `./hardest -f 2 -t 4 -r 2 -b 6 -B 0:7 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 9.03 -i 2000 -s 14103940161794951466 -i 0` |
| 3 | 3 | 2 | 9.535714 | 0.15.4 | `./hardest -f 3 -t 3 -r 2 -b 9 -B 0:10 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 9.42 -i 2000 -s 6422440067747821884 -i 0` |
| 0 | 5 | 3 | 6.571429 | 0.15.4 | `./hardest -f 0 -t 5 -r 3 -b 6 -B 0:7 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 6.48 -i 4000 -s 2026205479363403605 -i 0` |
| 1 | 4 | 3 | 9.058482 | 0.15.4 | `./hardest -f 1 -t 4 -r 3 -b 6 -B 0:7 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 8.99 -i 20 -s 16670210726696513633 -i 0` |
| 2 | 3 | 3 | 10.091741 | 0.15.4 | `./hardest -f 2 -t 3 -r 3 -b 4 -B 0:5 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 10.0 -i 10 -s 13683506237796025932 -i 0` |
| 1 | 6 | 2 | 8.273810 | 0.15.5 | `./hardest -f 3 -t 4 -r 2 -b 8 -B 0:9 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 10.57 -i 5000 -s 16955508027985230318 -i 0` |
| 2 | 5 | 2 | 9.957672 | 0.15.5 | `./hardest -f 2 -t 5 -r 2 -b 8 -B 0:9 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 9.91 -i 1000 -s 2318762516960874543 -i 0` |
| 3 | 4 | 2 | 10.692857 | 0.15.5 | `./hardest -f 3 -t 4 -r 2 -b 8 -B 0:9 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 10.57 -i 5000 -s 16955508027985230318 -i 0` |
| 0 | 6 | 3 | 7.059524 | 0.15.5 | `./hardest -f 0 -t 6 -r 3 -b 8 -B 0:9 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 7.01 -i 5000 -s 14692722917154841026 -i 0` |
| 1 | 5 | 3 | 9.721230 | 0.15.5 | `./hardest -f 1 -t 5 -r 3 -b 7 -B 0:8 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 9.617 -i 50 -s 3194072321188563417 -i 0` |
| 2 | 4 | 3 | 11.076984 | 0.15.5 | `./hardest -f 2 -t 4 -r 3 -b 6 -B 0:7 -H 1 -i 7 -S 7 -a 1.02 -e 1.01 -u 11.071 -i 50 -s 8602938993698001102 -i 0` |
| 3 | 3 | 3 | 11.446726 | 0.15.5 | `./hardest -f 3 -t 3 -r 3 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.32 -i 50 -s 7256326286338398992 -i 0` |
| 0 | 5 | 4 | 8.470703 | 0.15.5 | `./hardest -f 0 -t 5 -r 4 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 8.37 -i 50 -s 3387689341141999424 -i 0` |
| 1 | 4 | 4 | 11.057425 | 0.15.5 | `./hardest -f 1 -t 4 -r 4 -b 2 -B 0:2 -H 1 -S 7 -a 1.02 -e 1.01 -u 10.647 -i 50 -k 6 -s 4556814118304299227 -i 0` |
| 2 | 3 | 4 | 12.136675 | 0.15.5 | `./hardest -f 2 -t 3 -r 4 -b 2 -B 0:2 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.82 -i 5 -k 6 -s 10896610904631108835 -i 0` |
| 1 | 7 | 2 | 8.844444 | 0.15.5 | `./hardest -f 1 -t 7 -r 2 -b 9 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 -u 8.73 -i 600 -k 11 -s 14556849504793105477 -i 0` |
| 2 | 6 | 2 | 10.650000 | 0.15.5 | `./hardest -f 2 -t 6 -r 2 -b 7 -B 0:8 -H 1 -S 7 -a 1.02 -e 1.01 -u 10.54 -i 100 -k 11 -s 11087965487634666963 -i 0` |
| 3 | 5 | 2 | 11.654365 | 0.15.5 | `./hardest -f 3 -t 5 -r 2 -b 9 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.53 -i 1000 -k 11 -s 14946608536867708297 -i 0` |
| 4 | 4 | 2 | 11.978095 | 0.15.5 | `./hardest -f 4 -t 4 -r 2 -b 9 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.858 -i 1000 -k 11 -s 7689625710256102130 -i 0` |
| 0 | 7 | 3 | 7.425000 | 0.15.5 | `./hardest -f 0 -t 7 -r 3 -b 9 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 -u 7.331 -i 1000 -k 11 -s 7098271598830889803 -i 0` |
| 1 | 6 | 3 | 10.298810 | 0.15.5 | `./hardest -f 1 -t 6 -r 3 -b 6 -B 0:7 -H 1 -S 7 -a 1.02 -e 1.01 -u 10.1987 -i 300 -k 11 -s 18276219063542890244 -i 0` |
| 2 | 5 | 3 | 12.000992 | 0.15.5 | `./hardest -f 2 -t 5 -r 3 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.895 -i 0 -k 11 -s 17244908514822116033 -b 13` |
| 3 | 4 | 3 | 12.698929 | 0.15.5 | `./hardest -f 3 -t 4 -r 3 -b 3 -B 0:4 -H 1 -S 7 -a 1.02 -e 1.01 -u 12.568 -i 80 -k 11 -s 8033834704288936178 -i 0` |
| 0 | 6 | 4 | 8.941220 | 0.15.5 | `./hardest -f 0 -t 6 -r 4 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 8.807 -i 50 -k 11 -s 248765155733712140 -i 0` |
| 1 | 5 | 4 | 11.646063 | 0.15.5 | `./hardest -f 1 -t 5 -r 4 -b 2 -B 0:2 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.5256 -i 6 -k 11 -s 4268585735434295512 -i 0` |
| 2 | 4 | 4 | 13.025176 | 0.15.5 | `./hardest -f 2 -t 4 -r 4 -b 2 -B 0:2 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.031 -i 1 -k 11 -s 8104028515989855274 -i 0` |
| 3 | 3 | 4 | 13.419905 | 0.15.5 | `./hardest -f 3 -t 3 -r 4 -b 1 -B 0:2 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.28 -i 20 -k 11 -s 7166316949281471674 -i 0` |
| 1 | 8 | 2 | 9.189899 | 0.15.5 | `./hardest -f 1 -t 8 -r 2 -b 9 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 -u 9.1387 -i 8000 -k 11 -s 16076386538645687829 -i 0` |
| 2 | 7 | 2 | 11.220960 | 0.15.5 | `./hardest -f 2 -t 7 -r 2 -b 6 -B 0:7 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.134 -i 1000 -k 11 -s 11004992265404454055 -i 0` |
| 3 | 6 | 2 | 12.658550 | 0.15.5 | `./hardest -f 3 -t 6 -r 2 -b 7 -B 0:8 -H 1 -S 7 -a 1.02 -e 1.01 -u 12.36 -i 50 -k 11 -s 15954348099125908315 -i 0` |
| 4 | 5 | 2 | 13.051659 | 0.15.5 | `./hardest -f 4 -t 5 -r 2 -b 7 -B 0:8 -H 1 -S 7 -a 1.02 -e 1.01 -u 12.926 -i 140 -k 11 -s 4374285114512419273 -i 0` |
| 0 | 8 | 3 | 7.927273 | 0.15.5 | `./hardest -f 0 -t 8 -r 3 -b 8 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 -u 7.82 -i 440 -k 11 -s 14297119481458267428 -i 0` |
| 1 | 7 | 3 | 10.975000 | 0.15.5 | `./hardest -f 1 -t 7 -r 3 -b 3 -B 0:4 -H 1 -S 7 -a 1.02 -e 1.01 -u 10.868 -i 400 -k 11 -s 4106450522491929655 -i 0` |
| 2 | 6 | 3 | 12.769913 | 0.15.5 | `./hardest -f 2 -t 6 -r 3 -b 3 -B 0:4 -H 1 -S 7 -a 1.02 -e 1.01 -u 12.642 -i 100 -k 11 -s 15883515624880776119 -i 0` |
| 3 | 5 | 3 | 13.773593 | 0.15.5 | `./hardest -f 3 -t 5 -r 3 -b 1 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.634 -i 300 -k 11 -s 543823951367452778 -i 0`
| 4 | 4 | 3 | 14.081667 | 0.15.5 | `./hardest -f 4 -t 4 -r 3 -b 3 -B 0:4 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.948 -i 16 -k 11 -s 7124894593297389499 -i 0` |
| 0 | 7 | 4 | 9.292424 | 0.15.5 | `./hardest -f 0 -t 7 -r 4 -b 1 -B 0:2 -H 1 -S 7 -a 1.02 -e 1.01 -u 9.214 -i 5000 -k 11 -s 14022505586819668448 -i 0` |
| 1 | 6 | 4 | 12.247592 | 0.15.5 | `./hardest -f 1 -t 6 -r 4 -b 1 -B 0:2 -H 1 -S 7 -a 1.02 -e 1.01 -u 12.146 -i 250 -k 11 -s 3727540390642616461 -i 0` |
| 2 | 5 | 4 | 13.924281 | 0.15.5 | `./hardest -f 2 -t 5 -r 4 -b 1 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.787 -i 70 -k 10 -s 2868635322456824170 -i 0` |
| 3 | 4 | 4 | 14.631212 | 0.15.5 | `./hardest -f 3 -t 4 -r 4 -b 1 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 16.7827 -i 4 -k 11 -B 0:2 -B 1:2 -s 4861299490506878948 -i 0` |
| 0 | 6 | 5 | 10.905836 | 0.15.5 | `./hardest -f 0 -t 6 -r 5 -b 1 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 10.49 -i 200 -k 11 -B 0:2 -B 1:2 -b 2 -s 12443524816424922443 -i 0` |
| 1 | 5 | 5 | 13.767661 | 0.15.5 | `./hardest -f 1 -t 5 -r 5 -b 1 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.46 -i 50 -k 11 -s 8581647508762566537 -i 0` |
| 2 | 4 | 5 | 15.170539 | 0.15.5 | `./hardest -f 2 -t 4 -r 5 -b 1 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 16.46 -i 0 -k 11 -s 1773556797 -b 2` |
| 3 | 3 | 5 | 15.600163 | 0.15.5 | `./hardest -f 3 -t 3 -r 5 -b 0 -H 1 -S 7 -a 1.02 -e 1.01 -u 15.409 -i 10 -k 11 -B 0:1 -B 1:1 -B 2:1 -B 3:1 -B 4:1 -B 5:1 -B 6:1 -s 18253059765483928619 -i 0` |
| 1 | 9 | 2 | 9.660606 | 0.15.5 | `./hardest -f 1 -t 9 -r 2 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 9.59 -i 1000 -k 12 -b 7 -s 7088352904154992786 -i 0` |
| 2 | 8 | 2 | 11.839394 | 0.15.5 | `./hardest -f 2 -t 8 -r 2 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 11.717 -i 1000 -k 12 -b 6 -s 2767003436672560399 -i 0` |
| 3 | 7 | 2 | 13.203157 | 0.15.5 | `./hardest -f 3 -t 7 -r 2 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.108 -i 500 -k 12 -s 15888626169369702493 -i 0` |
| 4 | 6 | 2 | 14.030213 | 0.15.5 | `./hardest -f 4 -t 6 -r 2 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 13.8944 -i 300 -k 12 -b5 -s 17468815447494029456 -i 0` |
| 5 | 5 | 2 | 14.279161 | 0.15.5 | `./hardest -f 5 -t 5 -r 2 -b 4 -B 0:5 -H 1 -S 7 -a 1.02 -e 1.01 -u 14.169 -i 100 -k 12 -s 10408021407934587909 -i 0` |
| 0	| 9	| 3	| 8.195455 | 0.15.6	| `./hardest -f 0 -t 9 -r 3 -H 0 -S 7 -c 6 -u 8.106 -i 99 -l 1.1 -b 9 -s 14629452660196371190 -i 0`
| 1	| 8	| 3	| 11.404040	| 0.15.6 | `./hardest -f 1 -t 8 -r 3 -H 0 -S 7 -c 6 -u 11.31 -i 199 -l 1.1 -b 4 -s 15721476607074475487 -i 0`
| 2	| 7	| 3	| 13.427841	| 0.15.6 | `./hardest -f 2 -t 7 -r 3 -b 1 -B 0:1 -H 1 -S 7 -c 6 -u 13.3109 -i 59 -l 1.1 -b 2 -s 7164996095181785419 -i 0`
| 3	| 6	| 3	| 14.705303	| 0.15.6	| `./hardest -f 3 -t 6 -r 3 -b 1 -B 0:1 -H 1 -S 7 -c 6 -u 14.561 -i 19 -l 1.1 -B 0:2 -B 1:2 -B 2:2 -B 3:2 -s 410639136458424372 -i 0`
| 4	| 5	| 3	| 15.239854	|	0.15.6	| `./hardest -f 4 -t 5 -r 3 -b 1 -B 0:1 -H 1 -S 7 -c 6 -u 15.094 -i 99 -l 1.1 -s 1396675600036727563 -i 0`
| 0	| 8	| 4	| 9.840404	|	0.15.6	| `./hardest -f 0 -t 8 -r 4 -b 1 -B 0:2 -H 1 -S 7 -c 20 -u 9.72 -i 400 -l 1.2 -b 3 -s 17588120503035732877 -i 0`
| 1	| 7	| 4	| 12.950126	|	0.15.6	| `./hardest -f 1 -t 7 -r 4 -b 1 -B 0:2 -H 1 -S 7 -c 20 -u 12.81 -i 80 -l 1.3 -s 17306329951023624122 -i 0`
| 2	| 6	| 4	| 14.733135	|	0.15.6	| `./hardest -f 2 -t 6 -r 4 -b 1 -B 0:2 -H 1 -S 7 -c 20 -u 14.59 -i 20 -l 1.3 -s 11891906751021216175 -i 0`
| 3	| 5	| 4	| 15.746904	|	0.15.6	| `./hardest -f 3 -t 5 -r 4 -b 1 -B 0:2 -H 1 -S 7 -c 20 -u 15.59 -i 9 -l 1.2 -s 1773801667 -i 0`
| 4	| 4	| 4	| 16.068750	|	0.15.6	| `./hardest -f 4 -t 4 -r 4 -b 1 -B 0:1 -H 1 -S 7 -c 6 -u 15.93 -i 19 -l 1.1 -s 10150697525459360563 -i 0`
| 0 | 7 | 5 | 11.249620 | 0.15.6 | `./hardest -f 0 -t 7 -r 5 -b 1 -B 0:1 -H 1 -S 7 -c 20 -u 11.170 -i 200 -l 1.1 -s 12712224546223165494 -i 0` |
| 1 | 6 | 5 | 14.249496 | 0.15.6 | `./hardest -f 1 -t 6 -r 5 -b 1 -B 0:1 -c 20 -H 1 -S 7 -u 14.179 -i 5 -s 8041377118281470216 -i 0` |
| 2 | 5 | 5 | 15.933610 | 0.15.5 | `./hardest -f 2 -t 5 -r 5 -b 0 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 15.871 -i 10 -k 12 -B 1:1 -B 2:1 -B 3:1 -B 4:1 -s 8968094322052781071 -i 0` |
| 3 | 4 | 5 | 16.677641 | 0.15.5 | `./hardest -f 3 -t 4 -r 5 -b 1 -B 0:1 -H 1 -S 7 -a 1.02 -e 1.01 -u 16.429 -i 4 -k 12 -s 5821956879256807125 -i 0` |
