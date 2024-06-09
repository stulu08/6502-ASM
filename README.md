# C6502 Emulator, Compiler, and Linker

Welcome to the C6502 Emulator, Compiler, and Linker repository. This project provides tools for emulating, compiling, and linking 6502 assembly code.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
  - [Compilation and Linking](#compilation-and-linking)
  - [Standalone Compiler](#standalone-compiler)
  - [Standalone Linker](#standalone-linker)
  - [Standalone Emulator](#standalone-emulator)
- [Example](#example)
- [Building](#building)
- [License](#license)

## Features

- **Emulator**: Emulate 6502 assembly code.
- **Compiler**: Compile assembly code into object files.
- **Linker**: Link object files into a binary.
## Installation

Clone the repository from GitHub:

```bash
git clone https://github.com/stulu08/6502-ASM
cd 6502-ASM
```

## Usage

### Compilation and Linking

To compile and link assembly files, navigate to the `/BuildTools/Out/` directory and use the `SBuild.exe` tool:

```bash
SBuild.exe {path} -e -d
```

- `-e`: Emulate after building.
- `-d`: Dump the ROM after emulation.

### Standalone Compiler

Use `Compiler.exe` to compile one or more assembly files:

```bash
Compiler.exe in.asm in2.asm -o /out/ -g globalData.comp
```

- `-o`: Specify the output path.
- `-g`: Specify the global compiler data output path.

### Standalone Linker

Use `Linker.exe` to link object files into a binary:

```bash
Linker.exe in.obj in2.obj -o out.bin -g globalData.comp
```

- `-o`: Specify the output binary file path.
- `-g`: Specify the global compiler data input path.

### Standalone Emulator

Use `Emulator.exe` to emulate a binary file:

```bash
Emulator.exe in.bin -d
```

- `-d`: Dump the ROM after emulation.

## Example

Here is an example of 6502 assembly code:

```assembly
;import symboles
global main

;export symboles
extern put_char

;entry point
:main
LDA #72          ; 'H'
JSR put_char					

LDA #69          ; 'E'
JSR put_char					

LDA #76          ; 'L'
JSR put_char					
JSR put_char					

LDA #79          ; 'O'		
JSR put_char

LDA #32          ; ' '				
JSR put_char				

LDA #87          ; 'W'
JSR put_char		

LDA #79          ; 'O'		
JSR put_char

LDA #82          ; 'R'		
JSR put_char

LDA #76          ; 'L'
JSR put_char

LDA #68          ; 'D'
JSR put_char

LDA #33          ; '!'		
JSR put_char

LDA #10          ; '\n'		
JSR put_char	

;push and pull from stack
LDA #72
PHA
LDA #0
PLA

BRK		
```

To compile and link the example code, navigate to the `/BuildTools/Out/` directory and run:

```bash
SBuild.exe ../test -e -d
```

## Building

To build the tools, navigate to the `scripts` folder and execute the appropriate batch file for your build system. The repository includes Premake for project generation.

## License

This project is licensed under the GPL-2.0 License. See the [LICENSE](LICENSE) file for details.
