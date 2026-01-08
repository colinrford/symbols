/*
 * orbital_mechanics.cpp
 * Jupiter-Io Orbital Mechanics Example for lam.symbols
 *
 * Demonstrates compile-time trajectory computation using symbolic calculus.
 * The entire 500-frame orbital trajectory is computed during compilation,
 * then simply written to SVG files at runtime.
 *
 * Author: Colin Ford
 * License: CC0-1.0 Universal
 */

import std;
import lam.symbols;

using namespace lam::symbols;

// ============================================================
// COMPILE-TIME MATH UTILITIES
// ============================================================

// constexpr square root using Newton-Raphson iteration
constexpr double ce_sqrt(double x) noexcept
{
  if (x < 0)
    return 0;
  if (x == 0)
    return 0;
  double guess = x / 2.0;
  for (int i = 0; i < 30; ++i)
    guess = 0.5 * (guess + x / guess);
  return guess;
}

// ============================================================
// ORBITAL SIMULATION DATA STRUCTURES
// ============================================================

struct Frame
{
  double x, y;
  double vx, vy;
  double t;
  double speed;
  double ke, pe;
};

// ============================================================
// SYMBOLIC PHYSICS (the lam.symbols showcase!)
// ============================================================

constexpr symbol pos_x;
constexpr symbol pos_y;
constexpr symbol gm;

// Symbolic gravitational acceleration (direction component)
constexpr auto ax_symbolic = -gm * pos_x;
constexpr auto ay_symbolic = -gm * pos_y;

// ============================================================
// COMPILE-TIME TRAJECTORY COMPUTATION
// ============================================================

constexpr std::size_t NUM_FRAMES = 500;
constexpr double DT = 0.02;
constexpr double GM_VALUE = 1.0;

consteval auto compute_trajectory()
{
  std::array<Frame, NUM_FRAMES> frames{};

  double x = 1.0, y = 0.0;
  double vx = 0.0, vy = 1.05;
  double t = 0.0;

  for (std::size_t i = 0; i < NUM_FRAMES; ++i)
  {
    double r = ce_sqrt(x * x + y * y);
    double speed = ce_sqrt(vx * vx + vy * vy);
    double ke = 0.5 * speed * speed;
    double pe = -GM_VALUE / r;

    frames[i] = {x, y, vx, vy, t, speed, ke, pe};

    double r3 = r * r * r;
    double ax = ax_symbolic(gm = GM_VALUE, pos_x = x) / r3;
    double ay = ay_symbolic(gm = GM_VALUE, pos_y = y) / r3;

    vx += ax * DT * 0.5;
    vy += ay * DT * 0.5;
    x += vx * DT;
    y += vy * DT;

    r = ce_sqrt(x * x + y * y);
    r3 = r * r * r;
    ax = ax_symbolic(gm = GM_VALUE, pos_x = x) / r3;
    ay = ay_symbolic(gm = GM_VALUE, pos_y = y) / r3;

    vx += ax * DT * 0.5;
    vy += ay * DT * 0.5;
    t += DT;
  }

  return frames;
}

// Trajectory computed at compile time!
constexpr auto trajectory = compute_trajectory();

// Compile-time verification
static_assert(trajectory[0].x == 1.0, "Initial x should be 1.0");
static_assert(trajectory[0].y == 0.0, "Initial y should be 0.0");
static_assert(trajectory.size() == NUM_FRAMES, "Should have 500 frames");

// ============================================================
// SVG GENERATION
// ============================================================

struct orbital_bounds
{
  double min_x, max_x, min_y, max_y;
  double center_x, center_y, rx, ry;
};

orbital_bounds compute_orbital_bounds(const std::array<Frame, NUM_FRAMES>& traj)
{
  double min_x = traj[0].x, max_x = traj[0].x;
  double min_y = traj[0].y, max_y = traj[0].y;
  for (const auto& f : traj)
  {
    min_x = std::min(min_x, f.x);
    max_x = std::max(max_x, f.x);
    min_y = std::min(min_y, f.y);
    max_y = std::max(max_y, f.y);
  }
  double cx = (min_x + max_x) / 2.0;
  double cy = (min_y + max_y) / 2.0;
  double rx = (max_x - min_x) / 2.0;
  double ry = (max_y - min_y) / 2.0;
  return {min_x, max_x, min_y, max_y, cx, cy, rx, ry};
}

std::string speed_to_color(double speed, double min_speed, double max_speed)
{
  double t = (speed - min_speed) / (max_speed - min_speed);
  t = std::clamp(t, 0.0, 1.0);
  int r = static_cast<int>(0 + t * 255);
  int g = static_cast<int>(188 - t * 36);
  int b = static_cast<int>(212 - t * 212);
  return std::format("#{:02x}{:02x}{:02x}", r, g, b);
}

void write_svg_frame(std::ostream& out, std::size_t frame_idx, const std::array<Frame, NUM_FRAMES>& traj,
                     double min_speed, double max_speed, const orbital_bounds& bounds)
{
  const auto& f = traj[frame_idx];

  constexpr double SCALE = 100.0;
  double io_x = f.x * SCALE;
  double io_y = -f.y * SCALE;

  std::string io_color = speed_to_color(f.speed, min_speed, max_speed);
  std::string speed_label = f.speed > (min_speed + max_speed) / 2 ? "FAST" : "SLOW";

  double ke_ratio = f.ke / (f.ke - f.pe);
  int ke_width = static_cast<int>(ke_ratio * 236);
  ke_width = std::clamp(ke_width, 10, 226);
  int pe_width = 236 - ke_width;
  int pe_x = -118 + ke_width;

  double vscale = 25.0;
  double arrow_x = io_x + f.vx * vscale;
  double arrow_y = io_y - f.vy * vscale;

  // Write SVG header
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  out << "<svg viewBox=\"-150 -150 300 300\" xmlns=\"http://www.w3.org/2000/svg\">\n";
  out << "  <rect x=\"-150\" y=\"-150\" width=\"300\" height=\"300\" fill=\"#0a0a12\"/>\n";
  out << "\n";

  // Stars
  out << "  <circle cx=\"-120\" cy=\"-80\" r=\"0.8\" fill=\"white\" opacity=\"0.6\"/>\n";
  out << "  <circle cx=\"100\" cy=\"-110\" r=\"0.5\" fill=\"white\" opacity=\"0.4\"/>\n";
  out << "  <circle cx=\"-80\" cy=\"120\" r=\"0.6\" fill=\"white\" opacity=\"0.5\"/>\n";
  out << "  <circle cx=\"130\" cy=\"60\" r=\"0.7\" fill=\"white\" opacity=\"0.5\"/>\n";
  out << "\n";

  // Orbital path (computed from actual trajectory bounds)
  out << std::format("  <ellipse cx=\"{:.1f}\" cy=\"{:.1f}\" rx=\"{:.1f}\" ry=\"{:.1f}\" "
                     "fill=\"none\" stroke=\"#445\" stroke-width=\"1\" stroke-dasharray=\"4,4\"/>\n",
                     bounds.center_x * SCALE, -bounds.center_y * SCALE, bounds.rx * SCALE, bounds.ry * SCALE);
  out << "\n";

  // Trail
  if (frame_idx > 0)
  {
    out << "  <polyline points=\"";
    std::size_t start = frame_idx > 50 ? frame_idx - 50 : 0;
    for (std::size_t j = start; j <= frame_idx; ++j)
    {
      out << (traj[j].x * SCALE) << "," << (-traj[j].y * SCALE);
      if (j < frame_idx)
        out << " ";
    }
    out << "\" fill=\"none\" stroke=\"" << io_color << "\" stroke-width=\"3\" opacity=\"0.7\"/>\n";
  }

  // Jupiter defs and body
  out << "  <defs>\n";
  out << "    <marker id=\"arrowhead\" markerWidth=\"10\" markerHeight=\"7\" refX=\"9\" refY=\"3.5\" orient=\"auto\">\n";
  out << "      <polygon points=\"0 0, 10 3.5, 0 7\" fill=\"#4fc3f7\"/>\n";
  out << "    </marker>\n";
  out << "    <radialGradient id=\"jupiterGradient\" cx=\"40%\" cy=\"40%\">\n";
  out << "      <stop offset=\"0%\" style=\"stop-color:#f4d59e\"/>\n";
  out << "      <stop offset=\"50%\" style=\"stop-color:#d4a574\"/>\n";
  out << "      <stop offset=\"100%\" style=\"stop-color:#8b5a2b\"/>\n";
  out << "    </radialGradient>\n";
  out << "    <filter id=\"glow\" x=\"-50%\" y=\"-50%\" width=\"200%\" height=\"200%\">\n";
  out << "      <feGaussianBlur stdDeviation=\"2\" result=\"blur\"/>\n";
  out << "      <feMerge><feMergeNode in=\"blur\"/><feMergeNode in=\"SourceGraphic\"/></feMerge>\n";
  out << "    </filter>\n";
  out << "  </defs>\n";
  out << "  <circle cx=\"0\" cy=\"0\" r=\"35\" fill=\"url(#jupiterGradient)\"/>\n";
  out << "  <ellipse cx=\"0\" cy=\"-8\" rx=\"34\" ry=\"4\" fill=\"#c4956a\" opacity=\"0.6\"/>\n";
  out << "  <ellipse cx=\"0\" cy=\"5\" rx=\"33\" ry=\"5\" fill=\"#a07850\" opacity=\"0.5\"/>\n";
  out << "  <ellipse cx=\"0\" cy=\"15\" rx=\"32\" ry=\"3\" fill=\"#bc8c64\" opacity=\"0.4\"/>\n";
  out << "  <ellipse cx=\"12\" cy=\"8\" rx=\"8\" ry=\"5\" fill=\"#c45c3a\" opacity=\"0.7\"/>\n";
  out << "\n";

  // Io
  out << std::format("  <circle cx=\"{:.1f}\" cy=\"{:.1f}\" r=\"8\" fill=\"{}\" filter=\"url(#glow)\"/>\n", io_x, io_y,
                     io_color);

  // Velocity vector with arrowhead
  out << std::format(
    "  <line x1=\"{:.1f}\" y1=\"{:.1f}\" x2=\"{:.1f}\" y2=\"{:.1f}\" stroke=\"#4fc3f7\" stroke-width=\"2\" marker-end=\"url(#arrowhead)\"/>\n", io_x,
    io_y, arrow_x, arrow_y);

  // Energy bar
  out << "\n";
  out << "  <rect x=\"-120\" y=\"125\" width=\"240\" height=\"15\" rx=\"3\" fill=\"#1a1a2e\" stroke=\"#333\"/>\n";
  out << std::format(
    "  <rect x=\"-118\" y=\"127\" width=\"{}\" height=\"11\" rx=\"2\" fill=\"#00bcd4\" opacity=\"0.8\"/>\n", ke_width);
  out << std::format(
    "  <rect x=\"{}\" y=\"127\" width=\"{}\" height=\"11\" rx=\"2\" fill=\"#9c27b0\" opacity=\"0.8\"/>\n", pe_x,
    pe_width);
  out << "  <text x=\"-115\" y=\"135\" fill=\"white\" font-size=\"7\" font-family=\"monospace\">KE</text>\n";
  out << "  <text x=\"100\" y=\"135\" fill=\"white\" font-size=\"7\" font-family=\"monospace\">PE</text>\n";
  out << "\n";

  // Info text
  out << "  <text x=\"-140\" y=\"-132\" fill=\"#888\" font-size=\"11\" font-family=\"sans-serif\">Jupiter-Io "
         "System</text>\n";
  out << "  <text y=\"-115\" fill=\"#bbb\" font-size=\"11\">\n";
  out << "    <tspan x=\"-140\" font-family=\"Times, Georgia, serif\" font-style=\"italic\">t</tspan>\n";
  out << std::format("    <tspan font-family=\"monospace\"> = {:.2f} days</tspan>\n", f.t);
  out << "  </text>\n";
  out << "  <text y=\"-101\" fill=\"#bbb\" font-size=\"11\">\n";
  out << "    <tspan x=\"-140\" font-family=\"Times, Georgia, serif\" font-style=\"italic\">v</tspan>\n";
  out << std::format("    <tspan font-family=\"monospace\"> = {:.2f} km/s</tspan>\n", f.speed * 17.3);
  out << "  </text>\n";

  // Speed label
  out << std::format("  <text x=\"{:.0f}\" y=\"{:.0f}\" fill=\"{}\" font-size=\"7\" font-weight=\"bold\">{}</text>\n",
                     io_x - 10, io_y - 15, io_color, speed_label);

  out << "</svg>\n";
}

int main()
{
  std::filesystem::create_directories("output");

  double min_speed = trajectory[0].speed;
  double max_speed = trajectory[0].speed;
  for (const auto& f : trajectory)
  {
    min_speed = std::min(min_speed, f.speed);
    max_speed = std::max(max_speed, f.speed);
  }

  auto bounds = compute_orbital_bounds(trajectory);

  for (std::size_t i = 0; i < trajectory.size(); ++i)
  {
    std::ofstream out(std::format("output/frame_{:04d}.svg", i));
    write_svg_frame(out, i, trajectory, min_speed, max_speed, bounds);
  }

  std::println("Generated {} frames in output/", trajectory.size());
  std::println("Trajectory was computed at compile time!");
  std::println("Speed range: {:.3f} to {:.3f}", min_speed, max_speed);

  return 0;
}
