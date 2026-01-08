# ARCHITECTURE — Aura Glow (Scaffolding)

## Monorepo layout
- engine/        C++17 core pipeline (stub today, ML later)
- shaders/       GLSL shader library (hair dye, skin SSS, keratin)
- app/           Flutter UI shell (placeholder)
- docs/          Legal, compliance, algorithm, architecture
- scripts/       Repo verification, setup utilities

## Design principles
- On-device processing by default
- Deterministic multi-rate scheduling
- GPU-first rendering (avoid CPU copies)
- Privacy-by-design for biometric data

## Next implementation steps
1) Add FFI boundary (Dart <-> engine)
2) Add camera texture bridge (Android OES / iOS Metal)
3) Integrate ML runners (TFLite/CoreML) behind engine interface
4) Implement depth/occlusion + ROI inpainting
