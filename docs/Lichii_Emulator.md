---
title: Lichii - Emulador Nintendo Switch & Switch 2 (NX2)
tags:
  - emulator
  - switch2
  - nx2
  - vulkan
  - tech
  - projects
updated: 2026-11-06
---

# Lichii (Eden / Eglantine fork) - Switch 2 (NX2)

O Lichii é um emulador experimental para a arquitetura Nintendo Switch 2 (NX2) e para o Switch, derivado do [Eglantine](https://github.com/Djihads80/Eglantine) e do [Eden](https://github.com/eden-emulator) / Yuzu. O fork, mantido por Zortos, foca em patches de renderização Vulkan, suporte a instruções nativas Nvidia Ampere e correções para títulos de próxima geração.

---

## Localização nos projetos locais

| Recurso | Caminho no Sistema | Descrição |
|---|---|---|
| Repositório Código-Fonte (E:) | `E:\Projects\Lichii\` | Árvore Git primária com todas as branches upstream e fixes locais. |
| Repositório Código-Fonte (G:) | `G:\Projects\Lichii\` | Árvore Git espelhada no SSD G:, com build e ambiente nativos. |
| Atalho / Espelho Github | `E:\SuperClaude\Github\Lichii\` | Junção vinculada à pasta de projetos técnicos do SuperClaude. |
| Pacote Release Pronto | `G:\Games\Switch\Lichii-Release-NX2\` | Versão consolidada e autônoma, com atalho no Desktop. |
| Build Executável Atual | `G:\Projects\Lichii\build\bin\lichii.exe` (e `E:\Projects\Lichii\build\bin\lichii.exe`) | Compilação mais recente com fixes de Near LOD, PR #670, PR #663, Format Lookup e Preflush. |
| Mod Dynamic FPS | `%APPDATA%\eden\load\0400486024DEC000\Dynamic-FPS\exefs\` | Patch IPS que desbloqueia a taxa de quadros dinâmica (de 30 para 60 FPS). |
| Switches de Configuração | `%APPDATA%\eden\nx2_switches.txt` | Arquivo mestre de switches lido automaticamente em qualquer build. |
| Versão Alternativa (Sharkiix) | `G:\Games\Switch\Lichii (sharkiix's version)\` | Versão modificada da comunidade com mitigação de canal de GPU. |
| Script de Build 1-Clique | `G:\Projects\Lichii\build_fast.bat` / `E:\Projects\Lichii\build_fast.bat` | Script local de compilação rápida via MSVC + Ninja (`-j 12`). |
| Pasta de ROMs | `G:\Games\Switch\games\` | Imagens `.nup`, `.xci` e `.nsp`. |

---

## The Legend of Zelda: Ocarina of Time (NX2)

- Title ID: `0400486024DEC000`
- Arquivo testado: `G:\Games\Switch\games\The Legend of Zelda Ocarina of TIme [04004860224DEC000].nup`

### Mod de Dynamic FPS (framerate acima de 30 FPS)

- Origem: `G:\Games\Switch\Dynamic-fps`
- Patch IPS: `exefs\E0F3CCFAF50065AE4EA01EB5AEB13B1A45DA3348.ips` (145 bytes, offsets `0x6E6DC4` e `0x3C46300`).
- Instalação: instalado em `%APPDATA%\eden\load\0400486024DEC000\Dynamic-FPS\exefs\`.
- Efeito: o jogo sai do travamento em 30 FPS e opera em framerate dinâmico proporcional à GPU, de 34 a 60 FPS, com física estável.

---

### Solução do céu branco, bloom excessivo e névoa (Fog e DoF)

1. Causa do bloom e fog residual:
   - O jogo possui dois shaders de pós-processamento: o shader de computação volumétrica de névoa (`780c0536b89fc89e`) e o shader final de log-bloom/exposição (`bded5ce917183214`).
   - Por padrão, a acumulação de espalhamento da névoa no horizonte e o multiplicador de exposição estouram o brilho no céu e produzem a camada de neblina espessa visível em Kokiri Forest.
2. Visual workaround do Zortos (`4aa1e429a4`):
   - Incorporado na branch `test-all-in-one`.
   - Controlado pelo switch `EDEN_NX2_OOT_VISUAL_WORKAROUND=1`.
   - O que faz:
     - Multiplica a distância de corte de integração da névoa por 0.25, o que reduz a névoa em 75%.
     - Multiplica as cargas da constante de exposição de log-bloom por 0.65 antes da curva fílmica, o que reduz o estouro de luz em 35%.
   - Ativado automaticamente no `%APPDATA%\eden\nx2_switches.txt`.

---

### Diagnóstico da performance (8-15 FPS)

Ao investigar por que o jogo rodava a 8-15 FPS no PC do usuário (Ryzen 7 5700X + RTX 4060 Ti), identifiquei os fatores:

1. `EDEN_NX2_INVALIDATE_BEFORE_DISPATCH=list` (causa imediata da queda de FPS):
   - No commit `c6204e777e`, esse switch executava `gpu->InvalidateGPUCache()` a cada submissão de lista de comandos pelo guest (`DmaPusher::DispatchCalls()`).
   - Como Ocarina of Time despacha dezenas de listas de comandos por frame, o emulador limpava todos os caches da GPU repetidamente, congelando os pipelines Vulkan e derrubando a taxa de quadros para 8-15 FPS.
   - Solução: o switch experimental foi removido do `nx2_switches.txt`.

2. Configuração do `qt-config.ini` (`gpu_accuracy`, `sync_memory_operations`, `use_asynchronous_shaders`):
   - Precisão da GPU: estava em `gpu_accuracy=1` (High). Em High, o emulador força cercas e queries síncronas, o que bloqueia a thread de emulação. Os testes e benchmarks dos desenvolvedores em `AGENTS.md` usam Normal ou Low Accuracy (`gpu_accuracy=0`).
   - Sincronização de operações de memória: estava em `sync_memory_operations=true`, o que forçava esperas de cerca da GPU no host a cada bit de sync do Maxwell DMA.
   - Compilação assíncrona de shaders: estava em `use_asynchronous_shaders=false`, o que travava a renderização a cada compilação de shader.
   - Solução: o `qt-config.ini` agora usa `gpu_accuracy=0` (Normal), `sync_memory_operations=false` e `use_asynchronous_shaders=true`.

3. Neblina rasteira de Kokiri Forest (`EDEN_NX2_SKIP_HASH=c72b6ea746c00433`, port do Sharkiix):
   - Em Kokiri Forest, o shader de névoa rasteira (`c72b6ea746c00433`) gera manchas opacas no chão e consome muito fillrate e cálculos de mesclagem.
   - Portei o suporte ao `EDEN_NX2_SKIP_HASH` do Sharkiix diretamente em [`vk_rasterizer.cpp`](file:///E:/Projects/Lichii/src/video_core/renderer_vulkan/vk_rasterizer.cpp#L445-L465). Ativado no `nx2_switches.txt`, ele descarta essa passagem pesada sem impacto visual.

4. Fix do patch `Max Resolution` em `patch_manager.cpp` (causa dos 15 FPS / 63 ms):
   - O código verificava `std::getenv("EDEN_NX2_MAX_RES") != nullptr`. Com `EDEN_NX2_MAX_RES=0` no `nx2_switches.txt`, o `getenv` retornava a string `"0"`, que é não nula, e ativava o patch de resolução máxima por engano.
   - Esse patch desativa o controlador dinâmico de resolução da Nintendo e trava a renderização 3D em 1080p nativo, mesmo em áreas densas como a ponte suspensa com vista total de Kokiri Forest. Quando o frame excede 33.3 ms (chegou a 35 ms), o motor crava em 15 FPS (66.6 ms) por VSync.
   - O código agora verifica se o valor não é `'0'`. Com a resolução dinâmica reativada, o jogo escala a carga gráfica e mantém 30-40 FPS.

5. Pré-compilação de shaders no boot (`EDEN_NX2_PRELOAD_ALL_PIPELINES=1`):
   - Compila previamente todos os pipelines e shaders conhecidos durante a tela de inicialização do jogo, usando as 16 threads do Ryzen 7 5700X.
   - Elimina o stuttering que ocorria ao entrar em novas áreas ou disparar efeitos novos.

6. Atualizações das branches `origin/nx2` e `upstream/nx2-experimental`:
   - `f356d39ba3` (lowering dos compute shaders de culling): implementa instruções Maxwell/Ampere faltantes (`ULDC` indexado, `ULEA` carry out, `F2IP` packed byte). Ocarina rejeitava esses shaders ao olhar para a floresta densa e não descartava a geometria invisível. Com o culling corrigido, a GPU processa menos vértices.
   - `27226d2568` (skip de upload para render targets limpos): detecta se um render target novo será completamente limpo antes do uso e pula transferências desnecessárias para a GPU. Elimina 15% de overhead na thread de GPU, com ganho de até +6.5 FPS em Kokiri Forest.
   - `f4bd4f639b` e `df92fc6c95` (exclusão mútua no scheduler Vulkan com GPU split): garante que threads auxiliares de CPU e serviço não submetam trabalhos de memória ao mesmo tempo que a thread traseira do `GpuSplit`. Elimina travamentos e crashes de corrida de dados em `Scheduler::SubmitExecution`.
   - `7c0b5e791e` e `a6e5217c7b`: cache de `MaxContinuousRange` nas tabelas de páginas da GPU e suporte a `ATOM.CAS` global de 32 bits.

7. Auditoria do log `eden_log.txt` (trava de 15 FPS e soluções no `qt-config.ini`):
   - Causa principal encontrada no log: `[ 219.200388] HW.GPU <Info> video_core\gpu_split.cpp:108:MaybeStart: GPU thread split: stopped, the accuracy settings need the caches on the front`.
     - Quando `gpu_accuracy` está em High (1), o Lichii desliga a engine de `EDEN_NX2_SPLIT=2`, que exige caches sincronizados na thread frontal. Isso reduz a taxa de quadros em mais de 50%, forçando o pipeline gráfico e a execução de macros a rodar em uma única thread.
     - Solução: `gpu_accuracy=0` (Normal) fixado no arquivo per-game `0400486024DEC000.ini`, o que mantém o split de GPU ativo o tempo todo.
   - Trava de VSync double-buffer (`use_vsync=2` para `use_vsync=0` Immediate): no log, o VSync estava em FIFO. Em FIFO, se a renderização demora 34 ms (0.7 ms acima do limite de 30 FPS), o swapchain espera o próximo vblank e cai para 15 FPS (63.1 ms). Mudei para Immediate (VSync off, taxa livre com Dynamic FPS).
   - Troca de filtro (`ScaleForce` para `FSR`): na captura dos 15 FPS, o filtro estava em ScaleForce, que adiciona 4-6 ms de processamento em shader compute. Na captura a 34 FPS, estava em FSR. Ajustei `scaling_filter=6` (FSR com 88% de nitidez RCAS).
   - Gargalo de transcode ASTC na GPU (`ASTC GPU decodes: 129 in 5062 ms`): o emulador transcodificava texturas ASTC para BC3 na GPU (`accelerate_astc=1`) e travava a fila de comandos por 5 a 15 segundos. Ajustei para `accelerate_astc=0` (CPU), delegando a decodificação para as 16 threads do Ryzen 7 5700X.
   - Apresentação assíncrona (`async_presentation=true`): desacopla a entrega de quadros da thread de renderização da GPU.
   - Modo de memória VRAM (`vram_usage_mode=1`, Aggressive): libera os 8 GB da RTX 4060 Ti e remove o teto de 4 GiB do modo Conservative, que forçava expurgos contínuos de texturas.

8. Comparação com o vídeo da comunidade (`VID-20261105-WA0092.mp4`) e análise das capturas:
   - Build usada: `Lichii | 47b414bcb1-test/all-in-one | MSVC 19.51.36252.0`. A build local atual parte dessa base, com o commit `47b4146c61` mais as correções de exclusão mútua do scheduler Vulkan (`f4bd4f639b`) e culling de vegetação (`f356d39ba3`).
   - Captura 1 (comunidade, 34 FPS / 29.83 ms): `HANDHELD | FSR 1 SOFT | FXAA | VSync Off | Dynamic FPS`. Céu limpo e 34 FPS com o Dynamic FPS Mod e o FSR 1.
   - Captura 2 (build com ScaleForce, 15 FPS / 63.10 ms): o filtro ScaleForce é um algoritmo multipass de detecção de bordas para pixel art 2D. Em cena 3D densa, o frametime subiu para 63.10 ms e o VSync FIFO cortou o framerate para 15 FPS.
   - Otimização de CPU Ryzen 7 5700X (`/arch:AVX2`): adicionada vetorização AVX2/FMA de 256 bits ao `CMakeLists.txt`. Build concluída em `build\bin\lichii.exe` (commit `d13432b92a`).
   - Cenário: Death Mountain / Gerudo Valley com 6 corações. Céu azul, nuvens volumétricas, iluminação solar e framerate estável entre 27 e 34 FPS.
   - Configurações exibidas:
     - Console mode: `Docked`, ou `Handheld` para +20-30% de FPS.
     - GPU clocks: `Boost`.
     - VSync mode: `Immediate (VSync Off)`, que elimina o piso de 15 FPS.
     - Window adapting filter: `AMD FidelityFX Super Resolution 1 (FSR 1)` com nitidez RCAS em 88%. Não use `ScaleForce` em jogos 3D.
     - Anti-aliasing: `None` ou `FXAA`.
     - Decodificação ASTC: `CPU` com `Uncompressed (Best quality)`.
     - Decodificador NVDEC: `GPU Video Decoder`.
     - Recursos gráficos ativos: `Barrier feedback loops`, `Extended Dynamic State 2` e `Vertex Input Dynamic State`.

---

### Outras correções integradas na build `test-all-in-one`

- Sumiço de legendas: o patch `Max Resolution` fica opcional via `EDEN_NX2_MAX_RES=1`, então por padrão as 526 instruções de interface e legendas não são puladas.
- Anti-stutter no gameplay: `MinPipelinesAlwaysBuilt = 2000` em `vk_pipeline_cache.cpp` pré-compila as pipelines no boot e evita os micro-congelamentos de 6-30 ms por shader.
- Vegetação e grama: resolução de ponteiros dinâmicos e carregados e expansão do pool de tabelas até 64 MiB, para carregar todas as lâminas de grama sem culling indevido.
- Iluminação ambiente do céu: dobramento de multiplicações por 0 e 1 (`FoldIMul32`) para restaurar a vinculação do buffer ambiente de harmônicos esféricos.
- Estabilidade no Windows: shims `src/yuzu/msvc_compat.cpp` e correção de assert não sequencial de macro no Maxwell 3D (`7acb148f87`).
- Dynamic PTP gather (`ba6c26d074`, port de `Duck912 993a31c593`): emite gathers dinâmicos por tap quando os offsets de textura não podem ser dobrados como imediatos na compilação. Elimina o erro `Not all arguments in PTP are immediate, ignoring`, que quebrava a filtragem de sombras no OoT.

---

### Análise da branch de testes `upstream/scratch/oot-shader-lab` (Duck912 / Eglantine)

A branch de testes `scratch/oot-shader-lab`, mantida pelo desenvolvedor Duck912, recebeu uma atualização recente:

- `993a31c593` (Dynamic PTP Gather): portado e integrado na branch local (`ba6c26d074`).
- `e88024e6b0` (Compact Software HMMA): lowering para a rede pós-tonemap de Ocarina of Time com multiplicação de matrizes compacta. O commit cita que foi portado do Lichii (`zortos293/Lichii #5`), então o emulador já tinha essa base.
- `61bfe3dce6` (lowering de compute de OoT): ativa lowering por padrão para vegetação e culling, já presente na base via commit `f356d39ba3`.

---

### Análise do repositório Frizti (`https://github.com/Frizti/Lichii`)

1. Por que a comunidade elogia esse repo:
   - Frizti e Kajcsi corrigiram as piscadas brancas no Link em Ocarina of Time com `EDEN_NX2_SPLIT=2` ativo.
   - Causa raiz: o método `EndPendingOperations` em `vk_scheduler.cpp` executava `CounterReset(ZPassPixelCount64)` antes de cada submissão Vulkan. Com o GPU split ativado, o envio de fences e listas de comando divididas resetava a contagem de amostras no meio, o shader recebia contagem de oclusão zerada ou baixa e pintava o Link de branco.
   - Solução (`e44ee5c79f` e `87203d5936`): o contador de oclusão é pausado (`CounterClose`) em vez de resetado, acumula corretamente através das submissões e é limitado a 256 slots para não vazar memória nas telas de título.
   - Status no código local: integrado. O código entrou na branch oficial do Zortos via PR #121 (commit `c4d7ae9801`) e já faz parte da branch `test-all-in-one`.
2. Comparativo de recursos (repo local vs. Frizti):
   - O que o Frizti tem: uma ferramenta de telemetria (`EDEN_NX2_FLASH_WATCH=<dir>`) que captura e gera relatórios HTML de flashes de 1 quadro.
   - O que o repo local tem a mais, que o Frizti não tem:
     - Shims de linkagem MSVC para Windows (`src/yuzu/msvc_compat.cpp`); o repo do Frizti falha ao compilar com MSVC 2022.
     - Otimização nativa de vetorização `/arch:AVX2` para o Ryzen 7 5700X.
     - Lowering de compute shaders para árvores e culling de OoT (`f356d39ba3`, PR #144).
     - Skip de upload para render targets limpos (`27226d2568`, PR #141).
     - Implementação de atômicos de 32 bits CAS (`a6e5217c7b`, PR #140).
     - Scheduler Vulkan thread-safe (`f4bd4f639b`), que previne deadlocks de GPU split.
     - Correção de crashes de vídeo VP9 (`7e1d645557`).
   - Conclusão: o repo local já contém o benefício que tornou o repo do Frizti conhecido, além das correções de performance e compatibilidade listadas acima.

---

## Configuração ativa de switches (`%APPDATA%\eden\nx2_switches.txt`)

```ini
# Divisão de threads de GPU e sincronização
EDEN_NX2_SPLIT=2
EDEN_NX2_FENCE_BATCH=1

# Ajustes específicos para Ocarina of Time
EDEN_NX2_A2C_REFRESH=1
EDEN_NX2_MESH_FIRST=1

# Pula a névoa rasteira pesada em Kokiri Forest (remove manchas opacas e alivia a GPU)
EDEN_NX2_SKIP_HASH=c72b6ea746c00433

# Resolução Máxima (0 = Padrão / Legendas preservadas e alta fluidez)
EDEN_NX2_MAX_RES=0

# Pré-carrega todos os shaders/pipelines na inicialização (elimina micro-stutter em gameplay)
EDEN_NX2_PRELOAD_ALL_PIPELINES=1

# Sincronização de escrita de CPU em listas de comandos (elimina flickers de frames antigos - Selevet P7)
EDEN_NX2_LIST_CPU_SYNC=1
EDEN_NX2_CMDPAGE_BATCH=1
EDEN_NX2_UBOREUSE=1

# Otimizações de fluidez e prevenção de micro-stutters
EDEN_NX2_DMA_RANGES=1
EDEN_NX2_ACQUIRE_PEEK=1
EDEN_NX2_PREFLUSH=1

# Controle fino de VRAM para GPUs de 8 GB (evita paging para RAM do Windows)
EDEN_NX2_TC_EXPECTED_MIB=3600
EDEN_NX2_TC_CRITICAL_MIB=4500

# Telemetria e Diagnósticos (Desativados em gameplay para evitar stutters de I/O em disco)
EDEN_NX2_GPU_PROFILE=0
EDEN_DKB_FRAME_TRACE=0
EDEN_NX2_FPS_LOG=0
EDEN_NX2_DEBUG_SERVER=127.0.0.1:6545
```

---

## Correção da vegetação que sumia ao chegar perto (Near LOD e mesh shaders Ampere)

### Diagnóstico do problema

Em *The Legend of Zelda: Ocarina of Time* (`0400486024DEC000`), ao explorar Kokiri Forest e se aproximar de arbustos, flores e vegetação alta, partes das plantas desapareciam da tela e reapareciam quando o jogador se afastava:

- Mapeamento de LOD (Level of Detail): o motor do Switch 2 usa desenhos padrão Maxwell (Standard Draw Calls) para vegetação a média e longa distância. Ao aproximar a câmera, o jogo transiciona para o nível máximo de detalhes (Near LOD) e aciona Stage 3 mesh shaders nativos de Ampere.
- Falha de compilação silenciosa: o pipeline Vulkan rejeitava o shader de malha com o erro `Shader:Debug register_pressure.h: unhandled opcode 0x219 / 0xa19`.
- O emulador marcava `last_shaders_valid = false` e descartava a chamada de desenho da vegetação próxima.
- Além do opcode não mapeado, a base de registradores uniformes virtuais (`shadow_base`) violava o alinhamento em quartetos do Maxwell (`high_uniform_base & 3 != 0`) e falhava na emulação de registradores de hardware Ampere.

### Implementação da correção

Foram modificados dois arquivos no retranspilador de shaders Ampere:

1. [`src/shader_recompiler/frontend/ampere/register_pressure.h`](file:///E:/Projects/Lichii/src/shader_recompiler/frontend/ampere/register_pressure.h):
   - Adicionado mapeamento para as instruções `0x219` e `0xa19`, que informa ao compilador a pressão de registradores da passagem de malha.
2. [`src/shader_recompiler/frontend/ampere/lower_to_maxwell.h`](file:///E:/Projects/Lichii/src/shader_recompiler/frontend/ampere/lower_to_maxwell.h):
   - Ajustado o cálculo do `shadow_base` para forçar o alinhamento de quarteto:
     ```cpp
     shadow_base = (uses_uniform_bank ? high_uniform_base : 246U) & ~3U;
     ```
- Validação ao vivo: testado com o servidor de debug (`debug_mcp.py` na porta 6545) durante execução contínua:
  - Status retornado: `"rejected": []` (zero shaders rejeitados).
  - A vegetação mantém os detalhes mesmo colada na câmera do Link.

---

## Investigação e mitigação do consumo de memória (memory management e VRAM paging)

### Mecanismo do paging de VRAM em placas de 8 GB

- Placa de vídeo: NVIDIA GeForce RTX 4060 Ti (8 GB VRAM), com orçamento utilizável de ~6.14 GiB reportado pelo driver Vulkan.
- O Texture Cache original usava limites estáticos: `expected_memory = 4352 MiB` e `critical_memory = 5120 MiB`.
- Somando o Texture Cache com buffers de staging, pipelines Vulkan e swapchain, o consumo ultrapassava 6.6 GiB, estourava a VRAM física da GPU e forçava o driver a paginar para a RAM do sistema via PCIe, o que causava micro-stutters de leitura.
- Solução integrada ([`src/video_core/texture_cache/texture_cache.h`](file:///E:/Projects/Lichii/src/video_core/texture_cache/texture_cache.h)):
  - `critical_memory`: limitado dinamicamente a 75% da VRAM utilizável (~4.6 GiB).
  - `expected_memory`: limitado dinamicamente a 60% da VRAM utilizável (~3.6 GiB).
  - Crescimento geométrico de descritores via `std::bit_ceil` em [`descriptor_table.h`](file:///E:/Projects/Lichii/src/video_core/texture_cache/descriptor_table.h).

---

## Otimizações de código no ecossistema NX2 (upstream e laboratório)

### Cache direto de 4096 entradas em `MaxContinuousRange` (PR #670, Duck912)

- O gargalo: em Ocarina of Time, a cada draw e dispatch o jogo vincula storage buffers atrás de ponteiros carregados dinamicamente, ~220.000 chamadas por segundo em Kokiri Forest. A tabela de 64 entradas sofria ~25.000 misses por segundo e forçava a thread traseira de GPU a varrer ~7 MiB de tabelas de página. Isso consumia 44% de todo o tempo de GPU.
- A solução: implementado um cache direto `thread_local std::vector<Entry> cache(4096)` indexado por hash pseudo-aleatório `((gpu_addr ^ size) * 0x9E3779B97F4A7C15ULL) >> 52`, invalidado por `map_serial` apenas quando a tabela de páginas sofre remapeamento real.
- Impacto: os misses caem de 25.000/s para menos de 7.000/s e o tempo de caminhada de páginas cai de 200 ms/s para 20 ms/s. Ganho medido em Kokiri Forest: +~20% de FPS.
- Status: integrado e compilado em [`src/video_core/memory_manager.cpp`](file:///E:/Projects/Lichii/src/video_core/memory_manager.cpp).

### Expansão da Turing macro RAM para 0x8000 palavras (PR #663, brandonbunnelle)

- O gargalo: jogos de Switch 2 compilam e despacham macros Turing em offsets superiores a `0x1000`. O buffer fixo original de `0x3000` palavras causava truncamento ou falha no interpretador nativo/JIT MME.
- A solução: expansão da memória de macros para `0x8000` palavras (32K palavras / 128 KiB) e suporte a offsets de 64 bits em `ClearCode`.
- Status: integrado e compilado em [`src/video_core/macro.h`](file:///E:/Projects/Lichii/src/video_core/macro.h).

---

## Sessão de gameplay e auditoria de logs (06/11/2026, PID 12140, 03:08)

Durante a sessão de jogo de mais de 18 minutos, monitoramos a telemetria e auditamos o log gerado (`eden_log.txt`). A análise revelou as causas do stuttering e da corrupção de texturas que fechou o jogo:

### Consumo de RAM em ~14.7 GB (comportamento esperado do Switch 2)

- A dúvida: o processo do emulador subiu de ~4 GB até 14.7 GB de RAM, o que levantou suspeita de vazamento de memória.
- A descoberta: não é vazamento de memória.
  - Especificação do Switch 2 (NX2): o Nintendo Switch 2 tem 12 GB de DRAM LPDDR5 unificada, compartilhada entre CPU Horizon 2 e GPU Ampere.
  - Alocação por demanda (`Core::DeviceMemory`): o emulador reserva o espaço de endereçamento total do guest via `VirtualAlloc(..., MEM_RESERVE)`. À medida que o jogo carrega cenários, texturas, malhas e áudio, o Horizon 2 toca nas páginas e o Windows efetiva o commit físico (`VirtualAlloc(..., MEM_COMMIT)`).
  - Composição: 12 GB de DRAM do console emulado mais ~2.7 GB de buffers de staging Vulkan, descritores de textura e binários de pipelines somam ~14.7 GB de memória de trabalho. O consumo estabiliza nessa faixa.

### Causa do stuttering: 95.425 bloqueios síncronos CPU-GPU (`WaitForSyncOperation`)

- Evidência no log: avisos de desempenho repetidos ao longo de 18 minutos de gameplay: `CPU read of GPU-written memory ... (95425 so far)`.
- O mecanismo:
  - Em [`src/video_core/buffer_cache/buffer_cache.h`](file:///E:/Projects/Lichii/src/video_core/buffer_cache/buffer_cache.h), a flag `preflush` estava desativada para a arquitetura Ampere (`!Tegra::g_nx2_ampere`).
  - Como o jogo roda em Ampere, nenhuma página de memória escrita pela GPU era marcada para pré-descarga assíncrona.
  - Toda vez que a CPU do Switch precisava ler uma área da memória (física, animações, lógica de jogo, colisão), o emulador congelava a thread de CPU e executava um flush síncrono bloqueante (`WaitForSyncOperation(fence)`).
  - Em 18 minutos, o emulador congelou a CPU 95.425 vezes, uma média de ~86 por segundo, o que explica os micro-engasgos.
- A solução:
  - Habilitei o switch `EDEN_NX2_PREFLUSH=1` em `buffer_cache.h`. Os dados são descarregados de forma assíncrona antecipada, o que elimina os micro-congelamentos de CPU.

### Causa da corrupção de texturas aos 1086 s: leitura de TIC fora da faixa (`index > tic_limit`)

- Evidência no log: 48 dos 54 asserts de formato ocorreram logo após `assert index <= tic_limit`.
- O mecanismo:
  - Quando um shader buscava um descritor de textura com índice além do limite da tabela (`index > tic_limit`), a build antiga continuava a execução e lia memória arbitrária fora do buffer.
  - Esses bytes aleatórios iam para `PixelFormatFromTextureInfo`, que não reconhecia o formato (54, 61, 99, 9, 26, 6) e caía no fallback `A8B8G8R8_UNORM`.
  - A GPU criava e vinculava imagens 3D/2D com dimensões e endereços corrompidos, e o shader projetava polígonos no infinito, o que causava a corrupção de texturas aos 1086 s.
- A solução:
  - Em [`src/video_core/shader_environment.cpp`](file:///E:/Projects/Lichii/src/video_core/shader_environment.cpp), adicionei a guarda `if (index > tic_limit) return {};`, que impede leituras fora dos limites da memória de texturas.
  - Em [`src/video_core/texture_cache/format_lookup_table.cpp`](file:///E:/Projects/Lichii/src/video_core/texture_cache/format_lookup_table.cpp), adicionei tratamento para `TextureFormat{}` (formato 0, nulo), normalização de `UNORM_FORCE_FP16` e `SNORM_FORCE_FP16` e mapeamento de formatos de canal único (`R8`, `R16`, etc.).

---

## Correção de Lost Woods, Fonte da Grande Fada, crashes de shaders e CI/CD PGO (06/11/2026, 15:18)

### Fim do stuttering em Lost Woods (de 0.2-3 FPS para fluido)

- Causa raiz:
  1. Em [`src/core/memory.cpp`](file:///E:/Projects/Lichii/src/core/memory.cpp), `ReadGuestMemory` fazia chamadas `rasterizer->FlushRegion` página por página dentro do loop de leitura, forçando múltiplos stalls síncronos (`WaitForSyncOperation`) na thread de GPU.
  2. Em [`src/video_core/buffer_cache/buffer_cache.h`](file:///E:/Projects/Lichii/src/video_core/buffer_cache/buffer_cache.h), `FlushCachedWrites` invalidava cada buffer individualmente sem agrupar os intervalos e `CommitAsyncFlushesHigh` não limpava `committed_ranges` quando `nothing_to_commit` ocorria.
  3. Shaders nativos Ampere adiavam a emissão SPIR-V para threads worker (`DefersShaderEmission`), o que aumentava a contenção durante rajadas de compilação.
- Solução (commit `819a2229e4`):
  - Coalescência de `FlushRegion(address, size)` em uma única chamada antes do loop de páginas em `ReadGuestMemory` (`memory.cpp`).
  - Batching de `WriteMemory` em `FlushCachedWrites` e refresh de buffers inline via `MultiRangeBufferRefresh` (`buffer_cache.h` / `vk_buffer_cache.h`).
  - Emissão imediata na thread principal para programas nativos Ampere (`if (native) return false;` em `async_draw_policy.h`).

### Restauração da renderização 3D na Fonte da Grande Fada (Great Fairy Fountain)

- Sintoma: ao entrar na Fonte da Grande Fada, toda a geometria 3D sumia e apenas a HUD (corações e botões) aparecia na tela.
- Causa raiz:
  1. Barreira `WaitForIdle` quebrada (`vk_rasterizer.cpp`): o emulador usava um `vk::Event` (`wfi_event`) em `VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT` sem barreira de memória e nunca resetava o evento. `vkCmdSetEvent` em um evento já sinalizado não cria dependência nas esperas seguintes.
  2. Falta de barreiras compute para graphics e `DRAW_INDIRECT`: escritas de compute shaders (usadas pelo jogo para gerar argumentos indiretos e buffers de iluminação e vértices da Fonte da Fada) não eram ordenadas antes da abertura do próximo render pass (`vk_scheduler.cpp`).
  3. Desalinhamento de SSBO de 64/128 bits (`emit_spirv_memory.cpp` e `spirv_emit_context.cpp`): leituras e escritas vetoriais (`offset >> 3` ou `>> 4`) em buffers alinhados a 4 bytes liam o vetor anterior quando `min_ssbo_alignment < 8/16`.
  4. Opcodes Ampere em `register_pressure.h` e `lower_to_maxwell.h`: adicionado suporte a `0xc24` (`IMAD` com multiplicador uniforme), `0x9a8` (`ATOMG`), `0xb6c`/`0xf6c` (`TXD` com gradientes) e `0xa12`/`0xc12` (`LOP3.LUT`).
- Solução (commit `819a2229e4`, port do PR #295 `18cfe85563`):
  - Substituído `wfi_event` por `cmdbuf.PipelineBarrier(flags, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, WFI_BARRIER)` com barreira completa de memória.
  - Adicionada barreira `COMPUTE_TO_GRAPHICS` em `Scheduler::BeginRenderPassImpl` e `VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT` antes dos dispatches.
  - Divisão palavra-por-palavra (`U32`) para acessos globais e SSBO de 64/128 bits quando o alinhamento não é múltiplo exato.

### Diagnóstico por minidump e correção dos 2 crashes de shaders

1. Crash 1: estouro de memória no carregamento de shaders no boot (`lichii.exe.4864.dmp`, commit `59e777d889`):
   - Evidência no minidump: exceção `0xC0000409` (`std::bad_alloc`) em `Vulkan::ShaderPools::ShaderPools()` durante `PipelineCache::LoadDiskResources`.
   - Causa raiz: `GenericEnvironment::CanBeSerialized()` permitia serializar shaders nativos com trap prefix (`lowered_native->trap_prefix`) no disco, o que inflava `vulkan-frizti.fork-v52.bin` de 62 MB para 402 MB (8.218 pipelines). No boot, `LoadDiskResources` não deduplicava hashes repetidos e alocava um `ShaderPools` novo no heap para cada uma das 8.218 tarefas.
   - Correção:
     - Restaurada a guarda `(!lowered_native || lowered_native->trap_prefix.empty())` em `CanBeSerialized()` ([`shader_environment.cpp`](file:///E:/Projects/Lichii/src/video_core/shader_environment.cpp#L269-L271)).
     - Adicionada deduplicação imediata (`!disk_pipelines.insert(hash).second`), teto de `MaxPipelinesWithoutUsedList = 3000` e `thread_local ShaderPools pools; pools.ReleaseContents();` nas threads worker de [`vk_pipeline_cache.cpp`](file:///E:/Projects/Lichii/src/video_core/renderer_vulkan/vk_pipeline_cache.cpp#L917-L995).
     - Removido o cache corrompido de 402 MB e mantido o cache de 62 MB (`vulkan.fork-v52.bin`).
2. Crash 2: abort no Sirit ao carregar o save (`lichii.exe.24676.dmp`, commit `a048f58d41`):
   - Evidência no minidump: exceção `0xC0000409` (`FAST_FAIL_FATAL_APP_EXIT` e `std::abort()` em `ucrtbase.dll+0xA527E`) na thread `11564`: `Vulkan::EmitPipelineShader`, `EmitSPIRV`, [`MeshEpilogue`](file:///E:/Projects/Lichii/src/shader_recompiler/backend/spirv/emit_spirv_mesh.cpp#L141-L144) e `Sirit::Module::OpAccessChain`.
   - Causa raiz: ao carregar o save, o jogo compilou o primeiro mesh shader com topologia de pontos (`primitive_vertices == 1`). Em [`EmitContext::DefineMesh`](file:///E:/Projects/Lichii/src/shader_recompiler/backend/spirv/spirv_emit_context.cpp#L1867-L1899), uma variável local `Id point_size{}` sombreava o membro `ctx.mesh_point_size` e o deixava com ID 0 (inválido). Quando `MeshEpilogue` chamava `ctx.OpAccessChain(ctx.output_f32, ctx.mesh_point_size, vertex)`, o Sirit disparava `assert(base.value != 0)` e abortava o processo.
   - Correção: inicializado `Id& point_size{mesh_point_size};` e `PrimitivePointIndicesEXT` e `PrimitiveLineIndicesEXT` em `EmitContext::DefineMesh` ([`spirv_emit_context.cpp`](file:///E:/Projects/Lichii/src/shader_recompiler/backend/spirv/spirv_emit_context.cpp#L1867-L1899)).

### Otimizações Ryzen 7 5700X (`AVX2` e `/Ob3`), PGO local e releases automáticas (Windows e Linux) com auto-updater

- Flags Zen 3 (`src/CMakeLists.txt`): `/arch:AVX2`, `/Ob3` (inlining nível 3), `/Oi`, `/Ot`, `/Gw` e remoção da flag da Intel `/QIntel-jcc-erratum`.
- Script PGO local (`E:\Projects\Lichii\build_pgo.bat`): Profile-Guided Optimization em 3 passos (`build_pgo.bat instrument`, jogar 5 min, `build_pgo.bat optimize`).
- GitHub Actions CI/CD multiplataforma ([`.github/workflows/release-binaries.yml`](file:///E:/Projects/Lichii/.github/workflows/release-binaries.yml)):
  - Publica 4 edições por release no repositório [`gabaoun/Lichii`](https://github.com/gabaoun/Lichii/releases):
    1. Windows Standard Edition (`MSVC AVX2 + /Ob3`)
    2. Windows PGO Edition (`Clang-CL -march=znver3 + llvm-profdata`)
    3. Linux Standard Edition (`GCC -march=x86-64-v3` AppImage + `.tar.gz`)
    4. Linux PGO Edition (`Clang -march=znver3 + llvm-profdata` AppImage + `.tar.gz`)
- Auto-updater (`GenerateSCMRev.cmake` e `net.cpp`): o emulador consulta `https://api.github.com/repos/gabaoun/Lichii/releases/latest` (ou via menu `Help -> Verify Update`).

---

## Pipeline de compilação no Windows

- Branch ativa: `nx2-custom` (commit `a048f58d41` em `gabaoun/Lichii`)
- Compilador: MSVC 19.44 / Visual Studio 2022 Community (`/arch:AVX2 /Ob3 /Oi /Ot /Gw`)
- Gerador: Ninja 1.12.1 + CMake 3.31.5
- Executável gerado e testado:
  - `E:\Projects\Lichii\build\bin\lichii.exe` (compilado às 15:17 de 06/11/2026)
- Scripts locais:
  - `build_fast.bat` (compilação incremental em `-j 12`)
  - `build_pgo.bat` (Profile-Guided Optimization)

---

## Links e relações no cofre

- [[Lichii_Build_Optimization_Guide]]: guia de otimização de código e compilação rápida.
- [[Tech_Hub]]: central de projetos e repositórios técnicos.
- [[Home]]: índice geral do cofre.
- [[HANDOFF]]: registro de continuidade entre assistentes de IA.
