# Swish Kernel

Swish Kernel is a custom 64-bit bare-metal operating system kernel built in C++ and Assembly. It features a graphical user interface compositor, custom hardware drivers, and fundamental kernel subsystems.

## Core Features

* **Architecture:** Built specifically for the x86_64 architecture with custom Global Descriptor Table (GDT) and Interrupt Descriptor Table (IDT) implementations.
* **Bootloader:** Utilizes the Limine boot protocol for system initialization.
* **GUI Compositor:** Includes a custom graphical subsystem supporting windows, graphics rendering, and custom fonts.
* **Hardware Drivers:** Features built-in Programmable Interrupt Controller (PIC) support, alongside drivers for PS/2 keyboards and mice.
* **Memory & Libc:** Implements custom memory allocation and a foundational C library containing optimized `memcpy`, `memset`, and `strlen` routines.
* **Terminal Interface:** Integrates `flanterm` for robust terminal output and features a basic interactive shell.

## Project Structure

The codebase is organized into several distinct modules to separate architecture-specific code from generalized kernel tasks:

* `src/arch/` & `include/arch/`: Architecture-specific code, including CPU setup and interrupt handling for x86_64.
* `src/drivers/` & `include/drivers/`: Input drivers (keyboard, mouse, ring buffer) and PC hardware controllers.
* `src/gui/` & `include/gui/`: The windowing system, compositor, and graphics handling.
* `src/kernel/`: Core subsystems, memory management, and the system shell.
* `src/libc/`: The custom standard C library implementation.
* `scripts/`: Utility bash scripts for building, debugging, and running the OS.

## Getting Started

### Prerequisites

To compile and test Swish Kernel, ensure you have the following installed in your development environment:

* A cross-compiler toolchain targeting `x86_64`.
* CMake to generate the build files.
* An emulator like QEMU to run the compiled image.

### Building and Running

The project includes convenient scripts to streamline the development lifecycle:

* **Build the OS:** Execute `./scripts/build.sh` to compile the C++ codebase and prepare the Limine bootloader.
* **Run the OS:** Execute `./scripts/run.sh` to boot the compiled kernel inside your configured emulator.
* **Update Build:** Execute `./scripts/update.sh` to refresh the build environment.
* **Debug:** Use `./scripts/debug.sh` to attach a debugger and step through kernel routines.
