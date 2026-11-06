---
title: Lichii: Guia de otimização de código e compilação acelerada
tags:
  - emulator
  - nx2
  - cpp
  - cmake
  - ninja
  - performance
  - build-system
  - msvc
updated: 2026-11-06
---

# Lichii (NX2): Guia de otimização de código e compilação acelerada

Este guia reúne as correções de link-time e as configurações para acelerar a compilação e aumentar a taxa de quadros (framerate e frametimes) do emulador Lichii.

---

## 1. Perfil do hardware e ferramentas locais

- Processador: AMD Ryzen 7 5700X (8 núcleos / 16 threads)
- Compilador C++: Microsoft Visual C++ 2022 (MSVC v14.38 / v14.44)
- Sistema de build: CMake 3.31.5 + Ninja 1.12.1
- Localização dos utilitários de build: `C:\Users\Gabaoun\.gemini\antigravity\scratch\tools\`
- Repositório: `E:\Projects\Lichii\` (junção vinculada a `C:\Users\Gabaoun\.gemini\antigravity\scratch\Lichii\`)

---

## 2. Como compilar muito mais rápido

### A. Aproveitamento máximo da CPU (paralelismo com Ninja)
O comando padrão anterior utilizava apenas `-j 6`, deixando 10 threads do Ryzen 7 5700X ociosas.
- Configuração recomendada: usar `-j 12` a `-j 14`.
- Mantém 2 a 4 threads livres para o sistema operacional e responsividade de disco, reduzindo o tempo de compilação limpa em mais de 60%.

### B. Compilação incremental direta (pular o re-running do CMake)
Ao modificar apenas código-fonte (`.cpp` ou `.h`), não invoque o CMake novamente. O Ninja sabe exatamente quais arquivos foram alterados e compila somente o delta:
```powershell
cmd.exe /c "call \"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat\" && set \"PATH=C:\Users\Gabaoun\.gemini\antigravity\scratch\tools;C:\Users\Gabaoun\.gemini\antigravity\scratch\tools\cmake-3.31.5-windows-x86_64\bin;%PATH%\" && ninja -C build yuzu -j 12"
```

### C. Cache de compilação com `sccache`
O `sccache` (Shared Compilation Cache) armazena em disco o bytecode de tradução de objetos C++. Ao alternar entre branches (`nx2`, `fix-oot-fog`) ou refazer builds, o cache restaura arquivos idênticos em milissegundos:
1. Para ativar no CMake:
   ```powershell
   cmake -B build -G Ninja -DCMAKE_CXX_COMPILER_LAUNCHER=sccache -DCMAKE_C_COMPILER_LAUNCHER=sccache
   ```
2. O sccache evita recompilar externals pesados como `boost`, `ffmpeg`, `opus`, `zstd` e `openssl`.

### D. Redução do gargalo de linkagem (linker optimization)
O executável final (`lichii.exe`) tem mais de 60 MB e faz linkagem de dezenas de bibliotecas estáticas (Vulkan, Qt6, Boost, FFmpeg).
- Em modo debug / desenvolvimento: o MSVC suporta linkagem incremental (`/INCREMENTAL`). O CMake aplica `/OPT:REF,ICF` em Release, o que analisa funções duplicadas e leva de 10 a 20 segundos por link.
- Alternativa: usar o linker `lld-link` (do LLVM), que substitui o `link.exe` no Windows e conclui a linkagem de binários de 60 MB em ~2 a 3 segundos.

### E. Script de 1 clique (`build_fast.bat`) e orquestrador PGO (`build_pgo.bat`)
Para compilar imediatamente sem digitar comandos longos, use o `build_fast.bat` na raiz do projeto `E:\Projects\Lichii\`:
```bat
@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set "PATH=C:\Users\Gabaoun\.gemini\antigravity\scratch\tools;C:\Users\Gabaoun\.gemini\antigravity\scratch\tools\cmake-3.31.5-windows-x86_64\bin;%PATH%"
ninja -C build yuzu -j 12
```

#### Profile-Guided Optimization local (`build_pgo.bat`)
O `src/CMakeLists.txt` possui a opção `-DLICHII_PGO=OFF|GENERATE|USE` e flags nativas para o Ryzen 7 5700X (Zen 3):
- `/arch:AVX2 /Ob3 /Oi /Ot /Gw` (com remoção da flag `/QIntel-jcc-erratum`, que degradava branches em CPUs AMD).
- Fluxo PGO local:
  1. `.\build_pgo.bat instrument`: compila com `/GL` + `/GENPROFILE` (`pgort140.dll`).
  2. Jogue *Ocarina of Time* por ~5 minutos e feche o emulador (gera `lichii!*.pgc`).
  3. `.\build_pgo.bat optimize`: mescla os perfis com `pgomgr.exe` em `lichii.pgd` e recompila o binário final com `/USEPROFILE` (+8% a +15% menos branch misses no Dynarmic e no decodificador de GPU).

### F. CI/CD multiplataforma no GitHub Actions (Windows e Linux Standard + PGO) e auto-updater
O workflow [`.github/workflows/release-binaries.yml`](file:///E:/Projects/Lichii/.github/workflows/release-binaries.yml) no fork [`gabaoun/Lichii`](https://github.com/gabaoun/Lichii/releases) compila e publica automaticamente 4 pacotes por release:
1. Windows Standard (`MSVC AVX2 + /Ob3`): `Lichii-NX2-Custom-Windows-Standard-x86_64.zip`
2. Windows PGO (`Clang-CL Zen 3 + llvm-profdata`): `Lichii-NX2-Custom-Windows-PGO-Zen3-x86_64.zip`
3. Linux Standard (`GCC x86-64-v3`): `.AppImage` + `.tar.gz`
4. Linux PGO (`Clang Zen 3 + llvm-profdata`): `.AppImage` + `.tar.gz`

O auto-updater nativo (`CMakeModules/GenerateSCMRev.cmake` e `src/common/net/net.cpp`) aponta para `gabaoun/Lichii` e verifica novas releases ao abrir o emulador ou pelo menu `Help -> Verify Update`.

---

## 3. Correção de link-time: `src/yuzu/msvc_compat.cpp`

### O problema encontrado:
Ao compilar a branch mais atualizada com MSVC 2022, o linker falhava com **15 erros `LNK2019 / LNK2001` de símbolos não resolvidos** provenientes do `Qt6Core.lib`, `Qt6Gui.lib`, `libboost_filesystem` e `cubeb.lib`.

As bibliotecas pré-compiladas de Qt 6.11.1 no cache do CPM vêm de uma versão do MSVC STL que compilava rotinas vetorizadas em símbolos externos `extern "C"`. O compilador local atual inclui essas funções inline ou com nomes ligeiramente alterados.

### Os 15 símbolos resolvidos:
1. `__std_rotate` (usado por `qiconloader`, `qwindow`, `qwidget`)
2. `__std_unique_8` (usado por `qitemeditorfactory`)
3. `__std_unique_4` (usado por `qstring`)
4. `__std_search_1` (usado por `reshadefx`)
5. `__std_find_end_1` (usado por `reshadefx`)
6. `__std_find_last_trivial_2` (usado por `libboost_filesystem`, `qlocale_win`)
7. `__std_find_first_of_trivial_pos_2` (usado por `libboost_filesystem`, `qstring`, `qlocale`)
8. `_Thrd_sleep_for` (usado por `cubeb_log`, implementado com Win32 `Sleep`)
9. `__std_minmax_element_2u` (usado por `qcolor`)
10. `__std_minmax_2u` (usado por `qcolor`)
11. `__std_mismatch_4` (usado por `qkeysequence`)
12. `__std_replace_copy_1` (usado por `qbytearray`)
13. `__std_replace_copy_2` (usado por `qstring`)
14. `__std_remove_8` (usado por `qthread`)
15. `_Cnd_timedwait_for_unchecked` (usado por `qreadwritelock`, implementado com Win32 `SleepConditionVariableSRW`)

> [!IMPORTANT]
> Atenção: o arquivo `src/yuzu/msvc_compat.cpp` e sua entrada em `src/yuzu/CMakeLists.txt` devem ser **sempre preservados** em qualquer merge ou rebase com a branch upstream (`nx2`). Sem ele, a interface gráfica (`lichii.exe`) não consegue linkar no Windows.

---

## 4. Otimizações de código no emulador (performance em runtime)

### A. GPU thread split (`EDEN_NX2_SPLIT=2`)
A arquitetura Switch 2 exige alta taxa de transferência de comandos da CPU para a GPU.
- Front thread: lê GPFIFO, faz parsing de pushbuffers e o despache lógico.
- Back thread: emite os buffers de comando Vulkan, gerencia descritores e fences.
- Otimização: evite chamadas a `dma_pusher::Drain()` a cada entrada. O flush de páginas gravadas por draw deve ocorrer apenas após escritas efetivas no buffer cache (`e3d8e9c605`).

### B. Sincronização de query cache
- Em jogos como *Zelda: Ocarina of Time* (`0400486024DEC000`), a macro `0x142` do MME lê resultados de testes de oclusão via DMA.
- Otimização: chame `memory.FlushRegion(source, read_bytes, VideoCommon::CacheType::QueryCache)` apenas quando o split de GPU estiver inativo (`!VideoCommon::GpuSplit::Get().Active()`). Quando o split está ativo, os payloads de semáforos já são descarregados imediatamente, o que poupa sincronizações de barreira pesadas.

### C. Lowering de shaders Ampere (NVIDIA SM87)
- Gather per-tap com offsets literais (`TLD4.PTP`): materializar os offsets literais em registradores de rascunho de forma incondicional para evitar que a representação SSA perca os imediatos durante predicados de tempo de execução (`375c4a472b`).
- Atômicos globais em 32-bit (`UMax32`): permitem que compute shaders de auto-exposição e histogramas de luz terminem sem rejeição ou emulação lenta por CPU.

---

## 5. Switches recomendados para máximo desempenho (`nx2_switches.txt`)

Salve no arquivo `build\bin\nx2_switches.txt` (ou passe via variáveis de ambiente no launcher):

```ini
# --- Desempenho geral NX2 ---
EDEN_NX2_SPLIT=2               # Ativa split assíncrono front/back thread de GPU
EDEN_NX2_FENCE_BATCH=1         # Agrupa checagens de fence de download para reduzir IPC stalls
EDEN_NX2_INLINE_INVALIDATE=1   # Invalidação inline de intervalos de memória

# --- Compatibilidade gráfica estável ---
EDEN_NX2_A2C_REFRESH=1         # Atualização contínua de alpha-to-coverage por draw
EDEN_NX2_MESH_FIRST=1          # Corrige índice base de tarefas em Mesh Shaders
```

---

## Links e relações no cofre
- [[Lichii_Emulator]]: nota principal do emulador e histórico de correções.
- [[Tech_Hub]]: central de repositórios técnicos do usuário.
- [[HANDOFF]]: registro de continuidade entre sessões de IA.
