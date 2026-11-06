---
title: Repositórios técnicos e outros projetos
tags:
  - tech
  - projects
  - hub
updated: 2026-11-06
---

# Central de repositórios e projetos

Além do jogo principal Soul Resonance, `E:\Projects\` contém outros projetos e ferramentas técnicas:

---

## Emulação e sistemas de baixo nível
- [[Lichii_Emulator]]: Emulador de Nintendo Switch e Switch 2 (NX2) em C++/Vulkan.
  - Repositório: `E:\Projects\Lichii\`
  - Guia técnico: [[Lichii_Build_Optimization_Guide]] (compilação com Ninja `-j 12`, sccache e fix MSVC).
  - Binários prontos: `G:\Projects\Lichii\build\bin\lichii.exe` (e `E:\Projects\Lichii\build\bin\lichii.exe`)
  - Destaques: Shaders nativos Ampere (correção de Stage 3 Mesh Shaders da vegetação próxima), teto dinâmico de VRAM contra vazamento/paging, descritores geométricos e mitigação de explosão de texturas no *Zelda: Ocarina of Time* (`0400486024DEC000`).

---

## Foundry VTT e RPG
- `fvtt-cyberpunk-red-core`: Módulo core de Cyberpunk RED para o Foundry Virtual Tabletop.
- `fvtt-cyberpunk-red-dlc`: Pacotes de expansão, armas, netrunning e itens adicionais.

---

## Engine e assets
- `Project-Aether`: Projeto experimental de sistema / game framework em `E:\Projects\SuperClaude\Github\Project-Aether`.
- `GodotAssets`: Biblioteca centralizada de shaders, scripts reutilizáveis e assets genéricos de Godot.

---

## Web e ferramentas
- `InariverseSite` e `gabaoun.github.io`: Portais web e páginas de exibição de projetos.
- `SteamCalc`: Calculadora e analisador de métricas e conversões para a loja Steam.
- `GoLive`, `group-sharescreen`, `group-sharescreen-api`: Utilitários de streaming e compartilhamento de tela.
