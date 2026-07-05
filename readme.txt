========================================================================
LAUNCHXL-F28379D Standalone Project Setup
========================================================================
Author: Le-Nguyen Anh-Tuan (Weekend Le)
Date: July 2026
Target Hardware: TI C2000 Delfino LaunchPad (LAUNCHXL-F28379D)

===========
1. Overview
===========
This project implements a bare-metal application for the Texas Instruments 
LAUNCHXL-F28379D development board. It demonstrates basic peripheral 
operations using:
- Enhanced Pulse Width Modulator (ePWM1) for a time-base interrupt
- Analog-to-Digital Converter (ADCA) for sampling
- Digital-to-Analog Converter (DACB) for output
- CPU Timer1 for periodic status reporting
- Serial Communication Interface (SCI) for printing debug/status messages
- General-Purpose I/O (GPIO) for controlling LEDs

The main loop is intentionally empty; all functionality is interrupt-driven.

=========================
2. Firmware Overview
=========================
- ePWM1 is configured as a time-base interrupt source intended for converter drive.
- ADCA is sampled synchronously with ePWM1 using register ADCRESULT0.
- DACB writes a constant value (initially 1000) each time the ePWM interrupt fires.
- CPU Timer1 triggers at a low periodic rate to re-print the welcome banner, 
  send the latest ADCA reading over SCI, and toggle the on-board LED (GPIO34).
- SCI (serial port) handles asynchronous data transmission via software ring 
  buffers and functions like vMsgToSend and vWriteSCI.
- All initial clock, pin, and peripheral configurations are handled inside 
  vHardwareSetup().

==================
3. Firmware Flow
==================

[main]
- Entry point for the application.
- Calls vHardwareSetup() to configure peripherals and enables core blocks 
  (GPIO31, EPWM1, GPIO34, CPU Timer1).
- Prints the initial boot screen via vPrintWelcomeBanner().
- Enters an empty infinite loop (for(;;)); all operations are handled by ISRs.

[vEPWM1_InterruptHandler]
- Reads the current ADC measurement into the global variable ADCA.
- Writes the global variable DACB value to the DAC output register (DACVALS).
- Increments a debug counter (cnt).
- Clears the ePWM interrupt flag and acknowledges PIE Group 3.

[vCpuTimer1_InterruptHandler]
- Clears the Timer1 overflow flag (TIF).
- Re-displays the startup graphic by calling vPrintWelcomeBanner().
- Converts the current ADCA value to a character string and queues it to send 
  over serial as "ADCA: [value]\n\r".
- Flushes the software transmit buffer into the hardware UART FIFO via vWriteSCI().
- Toggles the Red LED on GPIO34 (LD3 on the board) to show a visual heartbeat.

[* Command Buffer]
- Global array ucCmdBuffer of size CMD_BUFFER_SIZE is defined in memory.
- Reserved for future command-line parsing or SCI string reception.


============================================================================
GIT WORKFLOW REFERENCE FOR CODE COMPOSER STUDIO
============================================================================
Note: CCS does not feature a built-in terminal. All operations below
should be executed using the external Windows Command Prompt (cmd).

1. NAVIGATE TO REPOSITORY DIRECTORY
   > cd [your_absolute_path_to_workspace]
   Example: cd c:\Users\unkno\workspace_ccstheia

2. BRANCH MANAGEMENT (Create and switch to a new branch)
   > git checkout -b [new_branch_name]

3. STAGE CHANGES (Add modified or new files to the index)
   > git add .

4. COMMIT CHANGES (Record snapshot with an inline message)
   > git commit -m "[your_commit_message]"
   
   Bypass message requirement (empty message):
   > git commit --allow-empty-message -m ""

5. PUSH TO REMOTE REPOSITORY
   > git push [remote_name] [branch_name]
   Example: git push origin main

6. MERGE CHANGES BACK TO TARGET BRANCH
   > git checkout [target_branch_name]
   > git merge [feature_branch_name]
============================================================================

Note:
   - This project use TI v25.11.1.LTS Compiler version