# Development Tools Installation for the Microchip WFI32E01 Certified Module

## Install Software Required for the Host Computer

1. Microchip `MPLAB X` tool chain for embedded code development on 32-bit architecture MCU/MPU platforms (made up of 3 major components)

    - [MPLAB X IDE (minimum v6.00)](https://www.microchip.com/mplab/mplab-x-ide) (when prompted, enable the installation of the [MPLAB IPE](https://www.microchip.com/en-us/tools-resources/production/mplab-integrated-programming-environment) too)

    - [MPLAB XC32 Compiler (minimum v4.10)](https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-xc-compilers#tabs)

        NOTE: This demonstration project was tested successfully with XC32 v4.10, and in general should work with later versions of the compiler as they become available. If you encounter issues building the project with a newer version of the compiler, it is recommended to download XC32 v4.10 from the [MPLAB Development Ecosystem Downloads Archive](https://www.microchip.com/en-us/tools-resources/archives/mplab-ecosystem) (to fall back to the version Microchip successfully tested prior to release). 

     - [MPLAB Harmony Software Framework](https://microchipdeveloper.com/harmony3:mhc-overview)

2. [Git](https://git-scm.com) (a free and open source distributed version control system)

- Download/install the latest version of [Git for Windows, macOS, or Linux](https://git-scm.com/downloads)

- Verify working operation of the `Git Bash` prompt (e.g. for Windows: click `Start` > type `Git Bash`)

3. Any [Terminal Emulator](https://en.wikipedia.org/wiki/List_of_terminal_emulators) program of your choice

## Additional Hardware Requirements for the PIC32 WFI32E Curiosity Board

If using the [PIC32 WFI32E Curiosity](https://www.microchip.com/en-us/development-tool/EV12F11A) board, a [USB-to-UART converter](https://www.newark.com/c/cable-wire-cable-assemblies/cable-assemblies/usb-adapter-cables?conversion-type=usb-to-uart-converter) needs to be connected to the U1RX & U1TX pins of the GPIO Header (J207)

<img src=".//media/WFI32-Curiosity_J207.png"/>

NOTE: In order to enable the on-board analog temperature sensor, the jumper J209 (TEMP SENSE) must be shorted
