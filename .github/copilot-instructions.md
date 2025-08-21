# Instruções do Copilot para este repositório (Luau)

Este arquivo orienta o GitHub Copilot sobre a estrutura, alvos de build e convenções deste repositório para sugerir mudanças coerentes.

## Visão geral

- Projeto: Implementação do compilador, analisador, VM e ferramentas do Luau (um dialeto de Lua com tipagem gradual).
- Linguagem: C++ (C++17 para a maioria dos componentes; VM usa C++11). C/headers para partes compatíveis com Lua 5.1.
- Build: CMake (mín. 3.10; 3.15+ para `LUAU_STATIC_CRT`), com Makefile auxiliar em Linux/macOS.
- Testes: `doctest` (C++). Conjunto unitário e de conformidade.
- CLIs: `luau` (REPL), `luau-analyze` (analisador), além de utilitários de AST/bytecode/reduce/compile.

## Layout de pastas (topo)

- `Ast/` — AST, lexer, parser e utilidades.
- `Compiler/` — bytecode builder e compilador de fonte para bytecode.
- `Analysis/` — checagem de tipos, lint e ferramentas de análise (Autocomplete, RequireTracer, etc.).
- `CodeGen/` — JIT/codegen A64/X64, IR e otimizadores.
- `VM/` — runtime/VM derivado de Lua 5.1 (API compatível), libs padrão, bytecode loader/executor.
- `Config/` — leitura de `.luaurc` e configuração de linter/analisador.
- `CLI/` — código compartilhado e entradas das ferramentas de linha de comando.
- `Require/` — runtime de `require` e navegador de caminhos (Navigator/Runtime).
- `EqSat/` — e-graphs e simplificações (apoio a Analysis).
- `tests/` — testes unitários e de conformidade (C++ e scripts Luau).
- `fuzz/`, `bench/`, `tools/` — fuzzing, bench scripts e utilidades (inclui `.natvis` e `extern/isocline`).

## Alvos CMake (bibliotecas e executáveis)

- Bibliotecas (STATIC):
  - `Luau.Common` (INTERFACE) — headers utilitários comuns.
  - `Luau.Ast` — depende de `Luau.Common`.
  - `Luau.Compiler` — depende de `Luau.Ast`.
  - `Luau.Config` — depende de `Luau.Ast`.
  - `Luau.Analysis` — depende de `Luau.Ast`, `Luau.EqSat`, `Luau.Config`; usa `Luau.Compiler` e `Luau.VM` como privados.
  - `Luau.EqSat` — estruturas de e-graph; depende de `Luau.Common`.
  - `Luau.CodeGen` — depende de `Luau.Common` e (privado) `Luau.VM`/`Luau.VM.Internals`.
  - `Luau.VM` — runtime (C++11), depende de `Luau.Common`.
  - `Luau.Require` — público: `Luau.VM`; privado: `Luau.RequireNavigator`.
  - `Luau.RequireNavigator` — público: `Luau.Config`.
  - `Luau.CLI.lib` — utilitários comuns às CLIs.
  - `isocline` — biblioteca de linha de comando embutida (externo/estático).

- Executáveis (quando `LUAU_BUILD_CLI=ON`):
  - `Luau.Repl.CLI` → binário `luau`
  - `Luau.Analyze.CLI` → binário `luau-analyze`
  - `Luau.Ast.CLI` → `luau-ast`
  - `Luau.Reduce.CLI` → `luau-reduce`
  - `Luau.Compile.CLI` → `luau-compile`
  - `Luau.Bytecode.CLI` → `luau-bytecode`

- Testes (quando `LUAU_BUILD_TESTS=ON`):
  - `Luau.UnitTest` — testes de compilador/análise/codegen.
  - `Luau.Conformance` — testes de VM e conformidade.
  - `Luau.CLI.Test` — testes do CLI/REPL.

- Web (opcional):
  - `Luau.Web` (quando `LUAU_BUILD_WEB=ON`).

## Opções CMake relevantes

- `LUAU_BUILD_CLI` (ON), `LUAU_BUILD_TESTS` (ON), `LUAU_BUILD_WEB` (OFF)
- `LUAU_WERROR` (OFF), `LUAU_STATIC_CRT` (OFF), `LUAU_EXTERN_C` (OFF)

## Fontes e mapeamento de arquivos

- Todos os arquivos de cada alvo são declarados em `Sources.cmake` via `target_sources(...)`.
- Headers públicos residem em `*/include` e são exportados via `target_include_directories(... PUBLIC ...)`.
- Implementações estão em `*/src`.
- CLIs usam código comum de `CLI/` via a lib `Luau.CLI.lib`.

## Regras de dependência entre componentes (importante)

Validadas no `CMakeLists.txt`:

- Runtime (VM/CodeGen) NÃO pode depender de componentes “offline” (Ast/Analysis/Config/Compiler).
- Componentes “offline” (Ast/Compiler) NÃO podem depender de runtime (VM/CodeGen) nem de Analysis/Config (para `Ast`/`Compiler`).
- Use `target_link_libraries` coerente com as regras acima; violações falham o configure.

## Convenções de código

- Padrões C++: `Luau.VM` usa C++11; demais usam C++17. Configure via `target_compile_features` conforme já existente.
- Avisos: `-Wall`, `-Wsign-compare`, etc. opcionalmente `-Werror` quando `LUAU_WERROR=ON`.
- Headers expostos em `*/include/Luau/...`. Inclua com caminhos relativos ao prefixo `Luau/` quando possível.
- Preserve a licença no topo de arquivos novos (veja cabeçalhos existentes).

## Testes

- Unitários ficam em `tests/*.test.cpp`, com `doctest`. `tests/main.cpp` provê a entrada.
- Alvos: `Luau.UnitTest` e `Luau.Conformance` recebem fontes via `Sources.cmake`.
- Ao criar um teste novo, adicione o arquivo em `tests/` e liste em `Sources.cmake` dentro do bloco do alvo correspondente.

## Build e execução

- CMake (recomendado):
  - Configure: `cmake -S . -B cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo`
  - Build CLIs: `cmake --build cmake --target Luau.Repl.CLI Luau.Analyze.CLI`
- Make (Linux/macOS):
  - `make config=release luau luau-analyze`

## Dicas para geração/edição de código pelo Copilot

- Ao adicionar fontes/headers:
  - Coloque headers públicos em `*/include/Luau/` e fontes em `*/src`.
  - Atualize `Sources.cmake` no bloco do alvo com `target_sources(...)` apropriado.
  - Não altere as propriedades globais dos alvos; preserve `target_compile_features`, `include_directories` e dependências existentes.

- Ao criar um novo utilitário CLI:
  - Use `CLI/` para compartilhados e crie um novo executável análogo aos existentes (ex.: `Luau.Reduce.CLI`).
  - Declare o executável em `CMakeLists.txt` (dentro de `if(LUAU_BUILD_CLI)`), configure `OUTPUT_NAME` e adicione fontes em `Sources.cmake`.

- Ao mexer em dependências:
  - Respeite as regras de runtime vs. offline (ver seção “Regras de dependência”).
  - Prefira dependências `PRIVATE` quando o consumo não precisar expor headers.

- Estilo e segurança:
  - Use C++ moderno dentro do padrão do alvo (C++11 para VM; C++17 nos demais).
  - Evite dependências externas novas; o projeto quase não usa libs externas, exceto `doctest` (testes) e `isocline` (CLI).

## Referências úteis

- Entrada/saída do compilador/VM: `luacode.h`, `luacodegen.h`, `lua.h`, `lualib.h`.
- Ferramentas e exemplos de uso: veja `CLI/src/*.cpp`.
- Configuração e linter: `Config/include/Luau/*.h` e `Config/src/*.cpp`.

---

Com este contexto, o Copilot deve sugerir mudanças que:

- Atualizem `Sources.cmake` ao criar/renomear arquivos.
- Mantenham as fronteiras de dependência entre módulos.
- Usem os diretórios `include/` e `src/` corretos por componente.
- Evitem alterações de build fora do padrão já definido.
