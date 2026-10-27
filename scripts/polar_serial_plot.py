"""Display polar coordinates received from an Arduino over a serial port.

Accepted input examples:
    120,1.57
    Radius: 120 Angle: 1.57
    radius=120 angle=1.57

The angle must be in radians.
"""

from __future__ import annotations

import argparse
import re
import sys

import matplotlib.pyplot as plt
import serial
from matplotlib.animation import FuncAnimation


VALUE_PATTERN = re.compile(
    r"(?i)(?:radius|r)\s*[:=]\s*(-?\d+(?:\.\d+)?)"
    r".*?(?:angle|a)\s*[:=]\s*(-?\d+(?:\.\d+)?)"
)


def parse_coordinates(line: str) -> tuple[float, float] | None:
    """Parse radius and angle in radians from one serial line."""
    line = line.strip()
    if not line:
        return None

    match = VALUE_PATTERN.search(line)
    if match:
        return float(match.group(1)), float(match.group(2))

    values = [value for value in re.split(r"[,;\s]+", line) if value]
    if len(values) >= 2:
        try:
            return float(values[0]), float(values[1])
        except ValueError:
            return None
    return None


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Plot polar coordinates received over a serial port."
    )
    parser.add_argument("port", help="Serial port, e.g. COM5")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--max-radius", type=float, default=512.0)
    args = parser.parse_args()

    try:
        connection = serial.Serial(args.port, args.baud, timeout=0)
    except serial.SerialException as error:
        print(f"Could not open {args.port}: {error}", file=sys.stderr)
        return 1

    figure, axis = plt.subplots(subplot_kw={"projection": "polar"})
    axis.set_ylim(0, args.max_radius)
    axis.set_title(f"Polar balance - {args.port}")
    marker, = axis.plot([], [], "o", markersize=12)
    status = figure.text(0.02, 0.02, "Waiting for data...")

    def update(_frame: int):
        latest = None
        while connection.in_waiting:
            line = connection.readline().decode("utf-8", errors="replace")
            coordinates = parse_coordinates(line)
            if coordinates is not None:
                latest = coordinates

        if latest is not None:
            radius, angle = latest
            radius = max(0.0, min(radius, args.max_radius))
            marker.set_data([angle], [radius])
            status.set_text(f"radius={radius:.1f}  angle={angle:.3f} rad")

        return marker, status

    animation = FuncAnimation(
        figure,
        update,
        interval=20,
        blit=False,
        cache_frame_data=False,
    )

    try:
        plt.show()
    finally:
        animation.event_source.stop()
        connection.close()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
