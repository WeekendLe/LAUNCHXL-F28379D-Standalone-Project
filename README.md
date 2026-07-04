# LAUNCHXL-F28379D Standalone Project

**Author:** Le-Nguyen Anh-Tuan (Weekend Le)  
**Date:** July 2026  
**Target Hardware:** TI C2000 Delfino LaunchPad (LAUNCHXL-F28379D)  
**Compiler Version:** TI v25.11.1.LTS (or compatible toolchain)
## License
Licensed under the MIT License (free to use).
Attribution or a quick message letting me know this project helped you will make my day :D

---

## 1. Overview
This project implements a bare-metal application for the Texas Instruments LAUNCHXL-F28379D development board. It demonstrates basic peripheral operations using:
* **Enhanced Pulse Width Modulator (ePWM1):** Serves as a time-base interrupt source.
* **Analog-to-Digital Converter (ADCA):** Samples synchronous signals.
* **Digital-to-Analog Converter (DACB):** Outputs analog signals.
* **CPU Timer1:** Tracks periodic system tasks and status reporting.
* **Serial Communication Interface (SCI):** Handles asynchronous transmission of diagnostic data.
* **General-Purpose I/O (GPIO):** Controls on-board diagnostic LEDs.

The application `main` loop is intentionally empty; all logic and peripheral operations are fully interrupt-driven.

---

## 2. Standalone Implementation (No C2000Ware Needed)
This repository is entirely **self-contained**. All required peripheral register headers, assembly boot files (`F2837xD_CodeStartBranch.asm`), memory mapping configurations, and device initialization source files are packaged directly inside this repository. 

You **DO NOT** need to download, include, or path an external Texas Instruments C2000Ware SDK to compile this codebase. It builds entirely out of the box using only a standard Code Composer Studio installation and the standard C2000 compiler.

---

## 3. Hardware Configuration
* **ePWM1:** Configured as a periodic time-base interrupt source designed to drive power converter.
* **ADCA:** Sampled synchronously with the ePWM1 timer-base events (results extracted from register `ADCRESULT0`).
* **DACB:** Writes a constant default value to the active `DACVALS` register on every ePWM interrupt cycle.
* **CPU Timer1:** Configured for low-rate execution. Periodically to process reporting routines.
* **SCI (UART):** Configured as a non-blocking communication line using software ring buffers and dedicated ring management functions.

---

## 4. Firmware Flow

### `main`
* Serves as the firmware entry point.
* Runs core hardware and clock setup routines by calling `vHardwareSetup()`.
* Configures baseline initialization states for `GPIO31`, `EPWM1`, `GPIO34`, and `CpuTimer1`.
* Outputs the initial initialization splash layout using `vPrintWelcomeBanner()`.
* Enters an infinite background loop (`for(;;)`) where the processor polls serial lines or sleeps while awaiting hardware interrupts.

### `vEPWM1_InterruptHandler`
* Reads the current ADC hardware sample directly into the global variable `ADCA`.
* Drives the global value of variable `DACB` into the shadow/active DAC data structures.
* Increments a global diagnostic runtime counter `cnt`.
* Clears localized ePWM event flag registers and acknowledges PIE Group 3.

### `vCpuTimer1_InterruptHandler`
* Clears the hardware overflow status flag (`TIF`) belonging to CPU Timer 1.
* Re-broadcasts the retro startup graphic using `vPrintWelcomeBanner()`.
* Samples the current value of the `ADCA` global variable and transforms it into character strings:
* Pumps data out to the physical wire by calling the ring buffer manager `vWriteSCI()`.
* Toggles the physical Red LED (`GPIO34` / `LD3` on the LaunchPad) to establish a distinct heartbeat.

### Global Command Buffer
* A global tracking array `ucCmdBuffer` of strict dimension length `CMD_BUFFER_SIZE` is declared.
* Kept reserved in static memory allocations for future expansion of command interpreters or interactive SCI line strings.

---
