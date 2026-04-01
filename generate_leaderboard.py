#!/usr/bin/env python3
"""Generate LEADERBOARD.md from best_known_bounds.csv.

Usage:
    python3 generate_leaderboard.py [best_known_bounds.csv] [LEADERBOARD.md]

Reads best_known_bounds.csv and writes LEADERBOARD.md.
"""

import csv
import sys


def read_csv(path):
    entries = {}
    with open(path) as f:
        reader = csv.DictReader(f)
        for row in reader:
            key = (int(row['false_gods']), int(row['true_gods']),
                   int(row['random_gods']))
            entries[key] = row
    return entries


def format_value(value, status):
    """Format E[Qs] value according to status."""
    if status == 'optimal':
        return f'**{value}**'
    elif status == 'conjectured':
        return f'*{value}*'
    elif status == 'likely':
        return f'*{value}*'
    else:  # upper_bound
        return value


def format_status(status):
    """Format status text for display."""
    if status == 'optimal':
        return '**Optimal**'
    elif status == 'conjectured':
        return '*Conjectured*'
    elif status == 'likely':
        return '*Likely*'
    else:
        return 'Upper bound'


def format_notes(entry):
    """Format notes column, including replication info."""
    parts = []
    replications = int(entry.get('independent_replications', 0))
    if replications > 0:
        parts.append(f'Replicated ×{replications}')
    notes = entry.get('notes', '').strip()
    if notes:
        parts.append(notes)
    return '; '.join(parts)


def select_results_entries(entries):
    """Select which entries to include in the Results table."""
    selected = set()

    # How far to look for adjacent conjectured/likely entries when
    # deciding whether to include gap upper_bound entries.
    gap_range = 3

    for (f, t, r), e in entries.items():
        # Skip placeholder values
        if float(e['expected_questions']) >= 999:
            continue

        total = f + t + r

        # All entries with sum <= 12
        if total <= 12:
            selected.add((f, t, r))

        # All conjectured and likely entries regardless of sum
        if e['status'] in ('conjectured', 'likely'):
            selected.add((f, t, r))

        # Optimal r=1 milestones for F=0 with T > 11
        if (r == 1 and f == 0 and e['status'] == 'optimal'
                and t in range(12, 21)):
            selected.add((f, t, r))

        # Optimal r=1 milestones for larger T values
        if (r == 1 and f == 0 and e['status'] == 'optimal'
                and t in (25, 30, 35, 40, 45, 50, 60, 62, 70, 80, 90, 100,
                          126, 150, 200, 250, 254)):
            selected.add((f, t, r))

    # Include upper_bound entries adjacent to conjectured/likely ranges
    # for continuity (e.g. (0,21,2) between conjectured and likely ranges)
    for (f, t, r), e in entries.items():
        if float(e['expected_questions']) >= 999:
            continue
        if e['status'] == 'upper_bound':
            # Check if this entry is "between" conjectured/likely entries
            # of same (f, r) family
            has_lower = any(
                entries.get((f, t2, r), {}).get('status') in
                ('conjectured', 'likely')
                for t2 in range(max(r, t - gap_range), t))
            has_higher = any(
                entries.get((f, t2, r), {}).get('status') in
                ('conjectured', 'likely')
                for t2 in range(t + 1, t + gap_range + 1))
            if has_lower and has_higher:
                selected.add((f, t, r))

    return selected


def sort_entries(keys):
    """Sort entries in the same order as the original leaderboard.

    Primary grouping: by total gods (sum), then by r value,
    then by f value within each group.

    Special sections at the end:
    - Large optimal r=1 entries for F=0 (milestones)
    - Extended conjectured/likely entries for r=2+
    """
    core = []       # sum <= 12 entries
    r1_milestones = []  # optimal r=1 F=0 with T > 11
    extended = []   # conjectured/likely/gap entries beyond sum=12, r>=2

    for key in keys:
        f, t, r = key
        total = f + t + r
        if total <= 12:
            core.append(key)
        elif r == 1 and f == 0:
            r1_milestones.append(key)
        else:
            extended.append(key)

    # Sort core by (sum, r, f)
    core.sort(key=lambda k: (k[0] + k[1] + k[2], k[2], k[0]))
    # Sort r1 milestones by T
    r1_milestones.sort(key=lambda k: k[1])
    # Sort extended by (r, f, t)
    extended.sort(key=lambda k: (k[2], k[0], k[1]))

    return core + r1_milestones + extended


def generate_results_table(entries, selected_keys):
    """Generate the Results table section."""
    lines = []
    lines.append(
        '| \\|F\\| | \\|T\\| | \\|R\\| | E[Qs] | Status | Notes |')
    lines.append(
        '|------:|------:|------:|------:|--------|-------|')

    sorted_keys = sort_entries(selected_keys)

    for f, t, r in sorted_keys:
        e = entries[(f, t, r)]
        value = format_value(e['expected_questions'], e['status'])
        status = format_status(e['status'])
        notes = format_notes(e)
        lines.append(
            f'| {f} | {t} | {r} | {value} | {status} | {notes} |')

    return '\n'.join(lines)


def select_repro_entries(entries, results_keys):
    """Select which entries get reproduction commands."""
    selected = []
    for key in results_keys:
        e = entries[key]
        cmd = e.get('reproduce_command', '').strip()
        if cmd and e['status'] != 'optimal':
            selected.append(key)
    return selected


def generate_repro_table(entries, repro_keys):
    """Generate the Reproduction Commands table."""
    lines = []
    lines.append(
        '| \\|F\\| | \\|T\\| | \\|R\\| | E[Qs] | Version |'
        ' Reproduce Command |')
    lines.append(
        '|------:|------:|------:|------:|---------|'
        '-------------------|')

    sorted_keys = sort_entries(repro_keys)

    for f, t, r in sorted_keys:
        e = entries[(f, t, r)]
        cmd = e['reproduce_command'].strip()
        version = e.get('version', '').strip()
        lines.append(
            f'| {f} | {t} | {r} | {e["expected_questions"]}'
            f' | {version} | `{cmd}` |')

    return '\n'.join(lines)


HEADER = """\
# Upper Bounds Leaderboard

This file tracks upper bounds on the expected number of questions needed \
to solve the generalized "Hardest Logic Puzzle Ever".

- Values marked **Optimal** are proven optimal.
- Values marked *Conjectured* are conjectured optimal.
- Values marked *Likely* could very well be optimal, too.
- Values marked as Upper bound may be improvable — you are welcome to try!

See [CONTRIBUTING.md](CONTRIBUTING.md) for how to submit improved bounds. \
Easiest is to just run the program and upload \
[best_known_bounds.csv](best_known_bounds.csv) if you find any improvement; \
[best_known_bounds.csv](best_known_bounds.csv) is updated automatically \
with default options.

**[best_known_bounds.csv](best_known_bounds.csv) is the authoritative \
source of bounds.** This file is a curated summary, generated by \
`python3 generate_leaderboard.py`. Replication counts and further details \
are in the CSV.

That a bound is reproduced independently means that it was replicated \
without use of the seed in the reproduction command.

## How to Reproduce

To reproduce a solution you need:

- Solver version: `./hardest --version`
- The full invocation command with all flags used
- The seed (`-s`) from the state before the search that found the solution
- The upper bound (`-u`) from the state before the search that found the \
solution
- The best lvl 0 pos estimate (`-E`) if relevant

These values are printed together with a solution, so they should be easy \
to find and post.

Per-entry reproduction commands are listed in the \
[Reproduction Commands](#reproduction-commands) section below, and also in \
the `reproduce_command` column of \
[`best_known_bounds.csv`](best_known_bounds.csv).

**Example:**
```sh
./hardest -f 2 -t 5 -r 2 -b 8 -B 0:9 -H 1 -S 7 -a 1.02 -e 1.01 \
-u 9.91 -i 1000 -s 2318762516960874543 -i 0
```

## Note on Symmetry

`E[Qs]` for _k_ false gods and _m_ true gods equals `E[Qs]` for \
_m_ false gods and _k_ true gods, for symmetry reasons.
"""

REPRO_HEADER = """\

## Reproduction Commands

Commands to quickly reproduce specific upper bounds. Each command runs a \
single deterministic replay (via `-s SEED -i 0`) and should, typically, \
complete in seconds. However, harder problems could take longer.

The `-u` value does not need updating when `-H 1` is used (it is biased \
low).
"""


def main():
    csv_path = sys.argv[1] if len(sys.argv) > 1 else 'best_known_bounds.csv'
    out_path = sys.argv[2] if len(sys.argv) > 2 else 'LEADERBOARD.md'

    entries = read_csv(csv_path)
    results_keys = select_results_entries(entries)
    repro_keys = select_repro_entries(entries, results_keys)

    results_table = generate_results_table(entries, results_keys)
    repro_table = generate_repro_table(entries, repro_keys)

    with open(out_path, 'w') as f:
        f.write(HEADER)
        f.write('\n## Results\n\n')
        f.write(results_table)
        f.write('\n')
        f.write(REPRO_HEADER)
        f.write('\n')
        f.write(repro_table)
        f.write('\n')

    n_results = len(results_keys)
    n_repro = len(repro_keys)
    print(f'Generated {out_path}: {n_results} results entries, '
          f'{n_repro} reproduction commands.')


if __name__ == '__main__':
    main()
