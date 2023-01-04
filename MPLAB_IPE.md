# Programming the WFI32E01 Module using the MPLAB IPE (Integrated Production Environment)

What is a programming environment? It is a simplified application that provides an interface for microcontroller (MCU) production programmers that allows a technician to easily load, program and verify software and avoid the complexities of full-featured Integrated Development Environments (IDEs).

MPLAB® [Integrated Programming Environment](https://www.microchip.com/en-us/tools-resources/production/mplab-integrated-programming-environment) (IPE) provides this type of solution. It is a simple interface that provides quick access to key production programming features. MPLAB IPE has a technician-level user interface for use on the manufacturing floor and provides production programming capabilities for all Microchip programmers and products supported by MPLAB X Integrated Development Environment (IDE).

## MPLAB IPE Features

- Support for two modes: Advanced and Production
- Import and export of HEX files
- Import of environments
- Import an SQTP number file for unique device serialization
- Docking/undocking of windows
- Capability of adding functionality via plug-ins
- Support for MPLAB IPECMD for production programming scripts

Download the [MPLAB IPE User Guide](https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/UserGuides/MPLAB-IPE-Integrated-Programming-Environment-Users-Guide-50002227.pdf) for complete details.

## Programming a HEX file into the WFI32E01 Module

1. Launch the MPLAB Integrated `Production` Environment (which should have been installed at the same time and in the same location as the MPLAB X Integrated `Development` Environment)

    <img src=".//media/ipe_01.png" width=250/>

2. Set the following fields under `Device and Tool Selection`:

    - `Device`: PIC32MZ1025W104132
    - `Tool`: "Curiosity/Starter Kits (PKOB4)" or "Starter Kits (PKOB)" (one should automatically show up by default if the board is detected)

    <img src=".//media/ipe_02.png" width=400/>

3. Click on the `Connect` button and confirm that the target device has been found

     <img src=".//media/ipe_03.png" width=400/>

4. Click on the `Browse` button and navigate to one of the following pre-built HEX files for your specific board:

 - WFI32-IoT: `[your_path]\AzureDemo_WFI32E01\firmware\AzureDemo_WFI32-IoT.X\dist\pic32mz_w1\production\AzureDemo_WFI32-IoT.X.production.hex`
 
  - WFI32-Curiosity: `[your_path]\AzureDemo_WFI32E01\firmware\AzureDemo_WFI32-Curiosity.X\dist\wfi32_curiosity\production\AzureDemo_WFI32-Curiosity.X.production.hex`

    <img src=".//media/ipe_04.png" width=600/>

5. Click on the `Program` button and wait for the programming sequence to finish

    <img src=".//media/ipe_05.png" width=600/>

6. Quit the MPLAB IPE program

7. Fully power-cycle the board by disconnecting it from the PC and then reconnecting it
