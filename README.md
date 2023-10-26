# Low-level 16-bit Virtual Machine

**THIS PROJECT IS HIGHLY IN DEVELOPMENT, CURRENTLY IT IS BEING USED FOR MY PROGRAMMING COURSE CUSTOM PROGRAM DEMONSTRATION. USE AT YOUR OWN RISK**

This project is a low-level 16-bit virtual machine that simulates a CPU with 8 general-purpose registers, an accumulator, an instruction pointer, a stack pointer, and a stack frame pointer. It also includes a memory mapper that connects the CPU with multiple devices like RAM, a screen device, and a keyboard device. Additionally, it provides an assembler that can parse assembly code into a binary executable format for use with the emulator.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Usage](#usage)
- [Building the Emulator](#building-the-emulator)
- [Running Programs](#running-programs)
- [Assembler](#assembler)
- [Contributing](#contributing)
- [License](#license)

## Overview

This 16-bit emulator project is designed to help you understand and experiment with low-level computer architecture. It provides a CPU emulator with essential components and an assembler for converting assembly code into a binary format.

### CPU

The CPU of this emulator is a simplified 16-bit architecture and includes the following components:

- 8 general-purpose registers from `r1-r8`
- An accumulator register `acc`
- An instruction pointer `ip`
- A stack pointer `sp`
- A stack frame pointer `fp`

### Memory Mapper

The memory mapper is responsible for connecting the CPU with various devices, including:

- RAM: For storing data and program instructions.
- Screen Device: Emulates a display for visual output.
- Keyboard Device: Simulates user input from a keyboard.

## Features

- A 16-bit CPU emulator with a set of general-purpose registers and essential components.
- A memory mapper to connect the CPU with RAM, a screen device, and a keyboard device.
- An assembler for converting assembly code into a binary executable format.
- Easily extendable and customizable for educational or experimental purposes.

## Usage

To use this 16-bit emulator, you can follow these steps:

### Building the Emulator

1. Clone this repository to your local machine:

   ```
   git clone https://github.com/Aragami1408/LL16VM.git
   cd LL16VM
   ```

2. Build and run the emulator according to your platform's specifications.
   ```
   make && ./LL16VM64
   ```

### Assembler

The assembler included in this project allows you to write programs in assembly language and convert them into binary format for execution on the emulator. Refer to the [assembler documentation](assembler/README.md) for more details.

## Contributing

Contributions to this project are welcome. If you have suggestions, bug reports, or feature requests, please open an issue on the GitHub repository.

## License

This project is licensed under the [MIT License](LICENSE). You are free to use, modify, and distribute this code as long as you comply with the terms of the license.

---

Enjoy exploring low-level 16-bit architecture with this emulator and assembler! If you have any questions or need assistance, please don't hesitate to reach out to the project maintainers.
