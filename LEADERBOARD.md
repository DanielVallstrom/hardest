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

**Example:**
```sh
./hardest -f 1 -t 6 -r 2 -i 5000 -B 0:4 -B 1:3 -B 3:2 -B 4:1 -B 5:1 -i 0 -s 13030756753776470731 -u 8.299603174603174
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
| 2 | 3 | 2 | 8.180952 | Upper bound | |
| 0 | 4 | 3 | 6.239286 | Upper bound | |
| 1 | 3 | 3 | 8.521652 | Upper bound | |
| 2 | 2 | 3 | 9.250930 | Upper bound | |
| 0 | 7 | 1 | **3.1875** | **Optimal** | |
| 1 | 6 | 1 | **5.982143** | **Optimal** | |
| 2 | 5 | 1 | **7.601190** | **Optimal** | |
| 3 | 4 | 1 | **8.296429** | **Optimal** | |
| 0 | 6 | 2 | *5.178571* | *Conjectured* | |
| 1 | 5 | 2 | *7.833333* | *Conjectured* | |
| 2 | 4 | 2 | 9.130952 | Upper bound | could not be replicated; value may be slightly off |
| 3 | 3 | 2 | 9.635714 | Upper bound | |
| 0 | 5 | 3 | 6.6250 | Upper bound | |
| 1 | 4 | 3 | 9.203181 | Upper bound | |
| 2 | 3 | 3 | 10.386091 | Upper bound | |
| 0 | 8 | 1 | **3.333333** | **Optimal** | |
| 1 | 7 | 1 | **6.333333** | **Optimal** | |
| 2 | 6 | 1 | **8.095238** | **Optimal** | |
| 3 | 5 | 1 | **9.095238** | **Optimal** | |
| 4 | 4 | 1 | **9.485714** | **Optimal** | |
| 0 | 7 | 2 | *5.527778* | *Conjectured* | |
| 1 | 6 | 2 | 8.273810 | Upper bound | |
| 2 | 5 | 2 | 10.013724 | Upper bound | |
| 3 | 4 | 2 | 10.806349 | Upper bound | |
| 0 | 6 | 3 | 7.089286 | Upper bound | |
| 1 | 5 | 3 | 9.918403 | Upper bound | |
| 2 | 4 | 3 | 11.515960 | Upper bound | |
| 3 | 3 | 3 | 12.033743 | Upper bound | |
| 0 | 5 | 4 | 8.708271 | Upper bound | |
| 1 | 4 | 4 | 11.620140 | Upper bound | |
| 2 | 3 | 4 | 13.089038 | Upper bound | |
| 0 | 9 | 1 | **3.5** | **Optimal** | |
| 1 | 8 | 1 | **6.677778** | **Optimal** | |
| 2 | 7 | 1 | **8.677778** | **Optimal** | |
| 3 | 6 | 1 | **9.880952** | **Optimal** | |
| 4 | 5 | 1 | **10.474603** | **Optimal** | |
| 0 | 8 | 2 | *5.844444* | *Conjectured* | |
| 1 | 7 | 2 | 8.866667 | Upper bound | |
| 2 | 6 | 2 | 10.748413 | Upper bound | |
| 3 | 5 | 2 | 11.800620 | Upper bound | |
| 4 | 4 | 2 | 12.067917 | Upper bound | |
| 0 | 7 | 3 | 7.500000 | Upper bound | |
| 1 | 6 | 3 | 10.618908 | Upper bound | |
| 2 | 5 | 3 | 12.369026 | Upper bound | |
| 3 | 4 | 3 | 13.259797 | Upper bound | |
| 0 | 6 | 4 | 9.175000 | Upper bound | |
| 1 | 5 | 4 | 12.324888 | Upper bound | |
| 2 | 4 | 4 | 14.229663 | Upper bound | |
| 3 | 3 | 4 | 14.773443 | Upper bound | |
| 0 | 10 | 1 | **3.636364** | **Optimal** | |
| 1 | 9 | 1 | **6.927273** | **Optimal** | |
| 2 | 8 | 1 | **9.056566** | **Optimal** | |
| 3 | 7 | 1 | **10.539394** | **Optimal** | |
| 4 | 6 | 1 | **11.317749** | **Optimal** | |
| 5 | 5 | 1 | **11.613276** | **Optimal** | |
| 0 | 9 | 2 | *6.054545* | *Conjectured* | |
| 1 | 8 | 2 | 9.218182 | Upper bound | |
| 2 | 7 | 2 | 11.315152 | Upper bound | |
| 3 | 6 | 2 | 12.658550 | Upper bound | |
| 4 | 5 | 2 | 13.163041 | Upper bound | |
| 0 | 8 | 3 | 7.978788 | Upper bound | |
| 1 | 7 | 3 | 11.162784 | Upper bound | |
| 2 | 6 | 3 | 13.190896 | Upper bound | |
| 3 | 5 | 3 | 14.366714 | Upper bound | |
| 4 | 4 | 3 | 14.829191 | Upper bound | |
| 0 | 7 | 4 | 9.612678 | Upper bound | |
| 1 | 6 | 4 | 13.066018 | Upper bound | |
| 2 | 5 | 4 | 15.257623 | Upper bound | |
| 3 | 4 | 4 | 16.242179 | Upper bound | |
| 0 | 6 | 5 | 11.581054 | Upper bound | |
| 1 | 5 | 5 | 15.484930 | Upper bound | |
| 2 | 4 | 5 | 17.597229 | Upper bound | |
| 3 | 3 | 5 | 18.327874 | Upper bound | |
| 0 | 11 | 1 | **3.750000** | **Optimal** | |
| 1 | 10 | 1 | **7.143939** | **Optimal** | |
| 2 | 9 | 1 | **9.531818** | **Optimal** | |
| 3 | 8 | 1 | **11.048990** | **Optimal** | |
| 4 | 7 | 1 | **12.048990** | **Optimal** | |
| 5 | 6 | 1 | **12.605700** | **Optimal** | |
| 0 | 10 | 2 | *6.272727* | *Conjectured* | |
| 1 | 9 | 2 | 9.709091 | Upper bound | |
| 2 | 8 | 2 | 11.963810 | Upper bound | |
| 3 | 7 | 2 | 13.319697 | Upper bound | |
| 4 | 6 | 2 | 14.127805 | Upper bound | |
| 5 | 5 | 2 | 14.438432 | Upper bound | |
| 2 | 5 | 5 | 18.176580 | Upper bound | |
| 3 | 4 | 5 | 19.274916 | Upper bound | |
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
