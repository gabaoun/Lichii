---
title: "Lichii (NX2): Central de engenharia e emulação"
tags:
  - emulacao
  - switch2
  - nx2
  - vulkan
  - lichii
  - hub
  - home
updated: 2026-11-06
---

# Lichii (NX2): Central de engenharia e emulação

> Projeto: Fork do emulador Nintendo Switch e Switch 2 (NX2)
> Linguagem e renderizador: C++20 / Vulkan / SPIR-V recompiler
> Hardware de referência: AMD Ryzen 7 5700X (8C/16T - AVX2) + NVIDIA GeForce RTX 4060 Ti
> Repositório: `https://github.com/gabaoun/Lichii` (upstream: `Djihads80/Eglantine`, fork base: `zortos293/Lichii`)
> Branch ativa: `nx2-custom`

---

## Acesso rápido

- [[Lichii_Emulator|Dossiê geral do Lichii]]: Visão técnica, arquitetura de memória de 12 GB, GPU Split e jogos testados.
- [[Lichii_Build_Optimization_Guide|Guia de build e otimização]]: Compilação via Ninja, sccache, flags Ryzen 7 5700X (`/arch:AVX2 /Ob3`) e CI multiplataforma.
- [[Tech_Hub|Tech Hub e outros projetos]]: Outros repositórios técnicos (`fvtt-cyberpunk-red`, `Project-Aether`).
- [Guia de Coding](Coding.md) e [Guia de Build Oficial](Build.md): Convenções do projeto e flags de compilação.
- [Notas de arquitetura NX2](nx2/): Documentação de shaders, DLSS contract e pipeline caching.

---

## Status dos jogos e fixes

| ID do programa | Jogo | Status e principais correções |
| :--- | :--- | :--- |
| `0400486024DEC000` | *The Legend of Zelda: Ocarina of Time* | 60 FPS com Dynamic FPS. Fix de stutter em Lost Woods, 3D na Great Fairy Fountain, bypass de sharpening (#319) e correção de Mesh Shaders (#321). |
| `0400C3F00006E000` | *Mario Kart World* | Pacing estável, upscale Kepler Compute e shader HSETP2 lowered. |
| `040074A01BF12000` | *Donkey Kong Bananza* | Atômicos globais e mitigação de stalls de pipeline. |
| `0400E88025B1A000` | *Star Fox* | Patch de 60 FPS automático, suporte de áudio e shaders. |
| `040057C02159A000` | *Nintendo Switch 2 Welcome Tour* | Relayout de vídeo pitch-to-block linear e controle de rumble. |

---

## Comandos de compilação e automação local

```powershell
# Compilação local com MSVC 2022 + AVX2 + /Ob3 (12 jobs Ninja)
.\build_fast.bat

# Geração de build PGO (Profile Guided Optimization) Clang Zen 3
.\build_pgo.bat

# Servidor de debug em tempo real (MCP)
python tools/nx2/debug_mcp.py --server 127.0.0.1:6545 --call '{"method":"status"}'
```

---

## Pipeline de releases (GitHub Actions)

O workflow `.github/workflows/release-binaries.yml` gera automaticamente 4 edições a cada push de tag:
1. `lichii-windows-x86_64.zip` (MSVC Standard + AVX2)
2. `lichii-windows-x86_64-pgo.zip` (Clang-CL PGO Zen 3)
3. `lichii-linux-x86_64.AppImage` / `tar.gz` (GCC Standard)
4. `lichii-linux-x86_64-pgo.AppImage` / `tar.gz` (Clang PGO Zen 3)
