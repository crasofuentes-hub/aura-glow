# Contributing

Thanks for contributing to **Aura Glow**.

## License & Attribution (Non-Negotiable)
By contributing, you agree that your contributions are licensed under **AGPL-3.0-or-later** and that:
- Existing copyright headers must remain intact.
- NOTICE must be preserved.
- CITATION.cff must remain valid.
See docs/LEGAL.md.

## Standards
- Keep changes minimal, auditable, and well-scoped.
- Prefer deterministic behavior; avoid hidden network calls by default.
- Do not add telemetry that collects biometric payloads (frames/embeddings/masks).
- Do not commit secrets. CI includes a basic secret-scan.

## Pull Request Checklist
- [ ] pwsh ./scripts/verify.ps1 -Strict passes locally
- [ ] Docs updated when behavior changes
- [ ] No secrets, tokens, or private keys in commits
- [ ] Licensing is compatible with AGPL

## Style
- Use clear naming and keep modules cohesive.
- Add comments only where they prevent future mistakes.
