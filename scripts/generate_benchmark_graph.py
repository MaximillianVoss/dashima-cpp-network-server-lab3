from __future__ import annotations

import csv
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CSV_PATH = ROOT / "docs" / "benchmark_results.csv"
SVG_PATH = ROOT / "docs" / "benchmark_percentages.svg"


def polyline(points: list[tuple[float, float]], color: str) -> str:
    data = " ".join(f"{x:.2f},{y:.2f}" for x, y in points)
    return f'<polyline fill="none" stroke="{color}" stroke-width="3" points="{data}" />'


def main() -> None:
    rows = []
    with CSV_PATH.open(newline="", encoding="utf-8") as source:
        for row in csv.DictReader(source):
            rows.append(
                {
                    "packet_count": int(row["packet_count"]),
                    "sequential_us": int(row["sequential_us"]),
                    "parallel_us": int(row["parallel_us"]),
                }
            )

    width, height = 900, 520
    left, right, top, bottom = 80, 40, 40, 80
    max_count = max(row["packet_count"] for row in rows)
    max_time = max(max(row["sequential_us"], row["parallel_us"]) for row in rows)

    def x_of(count: int) -> float:
        return left + (count / max_count) * (width - left - right)

    def y_of(time_us: int) -> float:
        return height - bottom - (time_us / max_time) * (height - top - bottom)

    seq_points = [(x_of(row["packet_count"]), y_of(row["sequential_us"])) for row in rows]
    par_points = [(x_of(row["packet_count"]), y_of(row["parallel_us"])) for row in rows]

    labels = []
    for row in rows:
        x = x_of(row["packet_count"])
        labels.append(
            f'<text x="{x:.2f}" y="{height - 42}" text-anchor="middle" '
            f'font-size="13">{row["packet_count"]}</text>'
        )

    svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">
  <rect width="100%" height="100%" fill="white"/>
  <text x="{width / 2}" y="26" text-anchor="middle" font-size="20" font-family="Arial">Packet percentage calculation benchmark</text>
  <line x1="{left}" y1="{height - bottom}" x2="{width - right}" y2="{height - bottom}" stroke="#222"/>
  <line x1="{left}" y1="{top}" x2="{left}" y2="{height - bottom}" stroke="#222"/>
  <text x="{width / 2}" y="{height - 12}" text-anchor="middle" font-size="14" font-family="Arial">packet count</text>
  <text x="18" y="{height / 2}" transform="rotate(-90 18 {height / 2})" text-anchor="middle" font-size="14" font-family="Arial">microseconds</text>
  {polyline(seq_points, "#1f77b4")}
  {polyline(par_points, "#d62728")}
  {"".join(labels)}
  <text x="{width - 260}" y="58" font-size="14" font-family="Arial" fill="#1f77b4">sequential</text>
  <text x="{width - 260}" y="80" font-size="14" font-family="Arial" fill="#d62728">parallel</text>
</svg>
'''
    SVG_PATH.write_text(svg, encoding="utf-8")


if __name__ == "__main__":
    main()
