"""bench/results/*.csv を集計し、段階ごとの平均・最小・最大を Markdown の表で出力する。

使い方: python bench/aggregate.py [CSVのディレクトリ or ファイル...] [--include-cold]
  run=0 は各段階の初回(コールド)として既定では集計から除外し、別表に出す。
"""
import csv
import glob
import os
import statistics
import sys
from collections import defaultdict


def load(paths):
    rows = []
    for p in paths:
        with open(p, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f):
                r["stage"] = int(r["stage"])
                r["run"] = int(r["run"])
                r["value"] = float(r["value"])
                rows.append(r)
    return rows


def label_group(metric, label):
    # B: 移行方向ごと(TITLE->TOWN は初回なので別扱い)、C: 訪問ごとの値をまとめる
    if metric.startswith("C_"):
        return "dungeon"
    return label


def per_run_values(rows):
    """(metric, group, stage) -> {run: [values]}"""
    d = defaultdict(lambda: defaultdict(list))
    for r in rows:
        key = (r["metric"], label_group(r["metric"], r["label"]), r["stage"])
        d[key][r["run"]].append(r["value"])
    return d


def summarize(values):
    return statistics.mean(values), min(values), max(values), len(values)


def table(rows, title):
    d = per_run_values(rows)
    stages = sorted({k[2] for k in d})
    metrics = sorted({(k[0], k[1]) for k in d})
    out = [f"### {title}", "", "| 指標 | 条件 | " + " | ".join(f"段階{s} 平均 (最小〜最大)" for s in stages) + " |",
           "|---|---|" + "---|" * len(stages)]
    for m, g in metrics:
        cells = []
        for s in stages:
            runs = d.get((m, g, s))
            if not runs:
                cells.append("—")
                continue
            # 1回の実行内で複数回測ったもの(B, C, D)は全サンプルをまとめる
            vals = [v for vs in runs.values() for v in vs]
            mean, lo, hi, n = summarize(vals)
            cells.append(f"{mean:.2f} ({lo:.2f}〜{hi:.2f}) n={n}")
        out.append(f"| {m} | {g} | " + " | ".join(cells) + " |")
    return "\n".join(out)


def derived(rows):
    """D: 1サイクル(生成+削除)の合計と1個あたり、A+B の合計"""
    out = []
    by = defaultdict(lambda: defaultdict(list))
    for r in rows:
        by[(r["stage"], r["run"])][(r["metric"], r["label"])].append(r["value"])
    stages = sorted({k[0] for k in by})

    lines = ["### 派生値", "", "| 指標 | " + " | ".join(f"段階{s}" for s in stages) + " |", "|---|" + "---|" * len(stages)]
    labels = sorted({l for k in by for (m, l) in by[k] if m == "D_create"})
    for lab in labels:
        n = int(lab.split("=")[1])
        cyc, per = {}, {}
        for s in stages:
            sums = []
            for (st, run), md in by.items():
                if st != s:
                    continue
                c = md.get(("D_create", lab), [])
                dl = md.get(("D_delete", lab), [])
                sums += [a + b for a, b in zip(c, dl)]
            if sums:
                cyc[s] = statistics.mean(sums)
                per[s] = cyc[s] / n * 1000.0
        lines.append(f"| D 1サイクル(生成+削除) {lab} [ms] | " + " | ".join(f"{cyc[s]:.2f}" if s in cyc else "—" for s in stages) + " |")
        lines.append(f"| D 1個あたり {lab} [µs] | " + " | ".join(f"{per[s]:.1f}" if s in per else "—" for s in stages) + " |")

    # A + B(10往復ぶんのロード時間の合計)
    tot = {}
    for s in stages:
        vals = []
        for (st, run), md in by.items():
            if st != s:
                continue
            a = md.get(("A_startup_from_process", ""), [])
            b = sum(v for (m, l), vs in md.items() if m == "B_transition" for v in vs)
            if a:
                vals.append(a[0] + b)
        if vals:
            tot[s] = statistics.mean(vals)
    lines.append("| A+B 起動 + 全シーン移行(TITLE→TOWN + 10往復)の合計 [ms] | " + " | ".join(f"{tot[s]:.0f}" if s in tot else "—" for s in stages) + " |")
    return "\n".join(lines)


def main():
    sys.stdout.reconfigure(encoding="utf-8")
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    include_cold = "--include-cold" in sys.argv
    if not args:
        args = [os.path.join(os.path.dirname(os.path.abspath(__file__)), "results")]
    paths = []
    for a in args:
        paths += sorted(glob.glob(os.path.join(a, "*.csv"))) if os.path.isdir(a) else [a]
    rows = load(paths)

    warm = rows if include_cold else [r for r in rows if r["run"] != 0]
    cold = [r for r in rows if r["run"] == 0]
    if not warm:
        warm = rows
    print(table(warm, "ウォーム(run>=1)" if not include_cold else "全実行"))
    print()
    print(derived(warm))
    if cold and not include_cold and warm is not rows:
        print()
        print(table([r for r in cold if r["metric"].startswith(("A_", "B_"))], "コールド(各段階の run=0、A・Bのみ)"))


if __name__ == "__main__":
    main()
