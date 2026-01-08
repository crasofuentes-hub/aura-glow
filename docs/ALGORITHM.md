# ALGORITHM — Aura Glow Core Pipeline (Deterministic Multi-Rate)

This document defines the on-device pipeline for:
- Hair dye (COLOR)
- Makeup (MAKEUP)
- Keratin sheen (KERATIN)
- Haircuts (CUT)

## Core principle
Multi-rate scheduling + temporal stabilization + physically plausible shading.

## Inputs / Outputs
(See README for repo guarantees)

## Per-frame pipeline
1) Capture (GPU texture)
2) Landmarks + pose @30fps
3) Face seg @15fps (or warp)
4) Hair seg @6–10fps (or warp)
5) Temporal smoothing (EMA/OneEuro)
6) Matte refinement (alpha)
7) Optional depth/occlusion
8) Shader render

## Frequencies
- Landmarks: 30
- Face seg: 15
- Hair seg: 6–10
- Depth: 0–6 (on-demand)
- Inpaint: on-change + ROI

## Blending models
- Dye preserves luminance/texture
- Makeup uses SSS-like warmth + soft-light
- Keratin uses anisotropic specular along tangent/flow
- Cut uses template clipping + ROI inpainting + matting

## Acceptance checks
- No mask flicker (temporal smoothing + warp)
- No hair over hand/clothes (depth/occlusion)
- No plastic skin (detail-preserving makeup)
- Thermal degrade mode supported
