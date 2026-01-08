#!/usr/bin/env python3
"""
make_video.py - All-in-one Jupiter-Io orbital animation generator

Runs the orbital_mechanics binary, converts SVG frames to PNG,
and stitches them into an MP4 video.

Usage: python make_video.py
"""

import subprocess
import shutil
import sys
from pathlib import Path

def find_tool(names: list[str]) -> str | None:
    """Find first available tool from list."""
    for name in names:
        if shutil.which(name):
            return name
    return None


def run(cmd: list[str], desc: str) -> None:
    """Run command with error handling."""
    print(f"  {desc}...")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        sys.exit(1)


def main():
    script_dir = Path(__file__).parent
    output_dir = script_dir / "output"
    
    # Find required tools
    converter = find_tool(["rsvg-convert", "inkscape"])
    ffmpeg = find_tool(["ffmpeg"])
    
    if not converter:
        print("Error: No SVG converter found. Install librsvg or inkscape.")
        sys.exit(1)
    if not ffmpeg:
        print("Error: ffmpeg not found. Please install ffmpeg.")
        sys.exit(1)
    
    print(f"Using: {converter}, {ffmpeg}")
    
    # Step 1: Run the C++ binary
    print("\n[1/3] Generating SVG frames...")
    binary = script_dir / "orbital_mechanics"
    if not binary.exists():
        # Try build/bin directory (standard output location)
        binary = script_dir.parent.parent / "build" / "bin" / "orbital_mechanics"
    if not binary.exists():
        binary = Path("orbital_mechanics")  # In current dir or PATH
    
    result = subprocess.run([str(binary)], cwd=script_dir, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error running orbital_mechanics: {result.stderr}")
        print("Make sure you've built the project first: ninja orbital_mechanics")
        sys.exit(1)
    print(result.stdout.strip())
    
    # Step 2: Convert SVGs to PNGs
    print("\n[2/3] Converting SVG → PNG...")
    svg_files = sorted(output_dir.glob("frame_*.svg"))
    if not svg_files:
        print("Error: No SVG files found in output/")
        sys.exit(1)
    
    for i, svg in enumerate(svg_files):
        png = svg.with_suffix(".png")
        if converter == "rsvg-convert":
            subprocess.run(["rsvg-convert", "-w", "800", str(svg), "-o", str(png)],
                          check=True, capture_output=True)
        else:
            subprocess.run(["inkscape", "--export-type=png", f"--export-width=800",
                          f"--export-filename={png}", str(svg)],
                          check=True, capture_output=True)
        
        if (i + 1) % 100 == 0 or i == len(svg_files) - 1:
            print(f"  Converted {i + 1}/{len(svg_files)} frames")
    
    # Step 3: Stitch to video
    print("\n[3/3] Stitching video...")
    output_video = script_dir / "orbit.mp4"
    subprocess.run([
        "ffmpeg", "-y",
        "-framerate", "30",
        "-i", str(output_dir / "frame_%04d.png"),
        "-c:v", "libx264",
        "-pix_fmt", "yuv420p",
        "-crf", "18",
        str(output_video)
    ], check=True, capture_output=True)
    
    # Cleanup PNGs (keep SVGs for inspection)
    print("\nCleaning up PNG files...")
    for png in output_dir.glob("frame_*.png"):
        png.unlink()
    
    print(f"\n✓ Created: {output_video}")
    print(f"  Size: {output_video.stat().st_size / 1024 / 1024:.1f} MB")


if __name__ == "__main__":
    main()
