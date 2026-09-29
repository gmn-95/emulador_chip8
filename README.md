# Emulador CHIP-8

Projeto experimental de um interpretador/emulador de CHIP-8 escrito em C++20. A janela e a renderização são feitas com [SDL3](https://www.libsdl.org/), que já está incluída no repositório em `vendored/SDL`.

O projeto foi desenvolvido com base no [guia para criar um emulador de CHIP-8](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).

## Objetivo do projeto

Este projetinho foi feito com o objetivo de estudar, entender o funcionamento de um CHIP-8 e tentar criar um emulador por conta própria. A inteligência artificial foi utilizada apenas como apoio para gerar explicações mais simples e exemplos durante o aprendizado; a implementação foi desenvolvida como parte desse processo de estudo.

## Requisitos

- CMake 4.3 ou superior;
- compilador com suporte a C++20, como GCC 13+ ou Clang equivalente;
- ambiente gráfico compatível com SDL3;
- Git, caso o projeto seja clonado do repositório.

Não é necessário instalar o SDL3 separadamente: o código-fonte da biblioteca já está em `vendored/SDL` e é compilado junto com o projeto.

## Compilação

Na raiz do projeto, execute:

```bash
cmake -S . -B build
cmake --build build
```

O executável será gerado em:

```text
build/chip8
```

No Windows, usando um gerador que cria configurações (`Debug`/`Release`), o executável pode ficar em um caminho como `build/Debug/chip8.exe` ou `build/Release/chip8.exe`.

## Execução

Execute o programa a partir da raiz do repositório:

```bash
./build/chip8
```

O diretório atual é importante porque o emulador procura a ROM usando um caminho relativo, atualmente:

```text
rom_teste/Pong (1 player).ch8
```

Se o programa for iniciado de outro diretório, ele pode não encontrar a ROM e terminar com o erro `Arquivo não existe`.

## Escolhendo outra ROM

As ROMs de teste disponíveis estão em `rom_teste/`:

- `Pong (1 player).ch8` — ROM carregada atualmente;
- `IBM Logo.ch8`;
- `Breakout (Brix hack) [David Winter, 1997].ch8`;
- `test_opcode.ch8`.

Para trocar a ROM, abra `main.cpp` e altere o caminho na função `loadROM()`:

```cpp
FILE* rom = fopen("rom_teste/Pong (1 player).ch8", "rb");
```

Por exemplo:

```cpp
FILE* rom = fopen("rom_teste/IBM Logo.ch8", "rb");
```

Depois, compile novamente:

```bash
cmake --build build
```

O programa ainda não recebe o nome da ROM pela linha de comando; a seleção é feita no código-fonte.

## Controles

O teclado do CHIP-8 é mapeado para as teclas abaixo:

```text
1 2 3 4       ->  1 2 3 C
Q W E R       ->  4 5 6 D
A S D F       ->  7 8 9 E
Z X C V       ->  A 0 B F
```

Feche a janela para encerrar a execução.

## Estrutura principal

```text
.
├── CMakeLists.txt       # configuração da compilação
├── main.cpp             # emulador, teclado e janela SDL
├── rom_teste/           # ROMs usadas nos testes manuais
└── vendored/SDL/        # código-fonte do SDL3
```

## Usando no CLion

1. Abra a pasta do projeto no CLion.
2. Aguarde o carregamento do `CMakeLists.txt`.
3. Selecione o alvo `chip8`.
4. Execute com o diretório de trabalho configurado como a raiz do projeto.

O último passo é necessário para que o caminho relativo da ROM seja resolvido corretamente.

## Limitações atuais

- a ROM é escolhida diretamente em `main.cpp`;
- não há argumentos de linha de comando;
- a implementação ainda está em desenvolvimento e pode não suportar todos os comportamentos e detalhes das diferentes variantes de CHIP-8;
- não há uma suíte automatizada de testes configurada; a validação atual é feita principalmente executando as ROMs de teste.
