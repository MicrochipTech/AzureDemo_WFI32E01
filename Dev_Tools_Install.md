# Development Tools Installation for Microchip WFI32-IoT Board (Part No. EV36W50A)

## Install Software Required for the Host Computer

Please install the following software in the exact order listed:

1. Install the following components that make up the Microchip `MPLAB X` tool chain for embedded code development on 32-bit MCU platforms

    - [MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)

    - [MPLAB XC32 Compiler](https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-xc-compilers#tabs)

        NOTE: The last known XC32 compiler version which successfully builds this demonstration project was version `4.10`.  If the currently available download version is greater than this last known version and does not successfully build the provided demo project, download the last known working XC32 version from the [MPLAB Development Ecosystem Downloads Archive](https://www.microchip.com/en-us/tools-resources/archives/mplab-ecosystem)

    - [MPLAB Harmony Software Framework](https://microchipdeveloper.com/harmony3:mhc-overview)

2. Install [Git](https://git-scm.com) (a free and open source distributed version control system) by executing the following steps:

- Download/install the latest version of [Git for Windows, macOS, or Linux](https://git-scm.com/downloads)

- Verify working operation of the `Git Bash` prompt (e.g. for Windows: click `Start` > type `Git Bash`)

    <img src=".//media/image15.png"/>

3. Install the Microsoft [Azure Command Line Interface (CLI)](https://docs.microsoft.com/cli/azure/?view=azure-cli-latest). The Azure CLI is a set of commands used to create and manage Azure resources. The Azure CLI is available across Azure services and is designed to get you working quickly with Azure, with an emphasis on automation

4. Install the latest [Python](https://www.python.org) release. During the installation setup, be sure to enable/check the option for `Add Python <X.Y> to PATH`

5. Install and verify [pyazureutils](https://pypi.org/project/pyazureutils/) (a Microchip utility for interacting with Microsoft Azure web services via the Azure CLI) by executing the following steps:

   - Launch a `Command Prompt` or `PowerShell` window (e.g. for Windows: click on `Start` > type `PowerShell` in the Search field > `Open`)
   - Execute the following command line to install the utility program

        ```shell
        pip install --upgrade pyazureutils
        ```
   - Bring up the IoT Central's specific help menu to verify that the `pyazureutils.exe` program can be found
 
        ```shell
        pyazureutils iotcentral --help
        ```

        NOTE: If the `pyazureutils` command cannot be found nor executed, search for the location of the `pyazureutils.exe` program and add the absolute path of its location to the Windows `PATH` environment variable. Launch a new command line window and try bringing up the help menu again

    - Confirm the version of `pyazureutils` that is currently installed (should match the version that was last downloaded)
 
        ```shell
        pyazureutils -R
        ```
    - If desired, `pyazureutils` can be run to output additional debug messages
 
        ```shell
        pyazureutils -v debug
        ```
## Verify the Device Certificate is in PEM Format

 The device certificate for your WFI32-IoT Board should already be pre-programmed in the `WFI32-IOT` Mass Storage Device. However, the certificate file needs to be in the text-based Privacy Enhanced Mail (PEM) format.

 1. Connect the board to PC, then make sure a device named `WFI32-IOT` shows up as a Mass Storage Device on the `Desktop` or in a `File Explorer` window

 2. Using a `File Explorer` window, access the `WFI32-IOT` Mass Storage Device and look for a file with an extention of `CER`. Most likely, the default name of the device certificate file is the unique serial number of your device (e.g. sn012319AAC99CF42A01). If this file exists, then your device certificate is already in the correct format (PEM). If the file has the `DER` file extention, the certificate file is in binary format and needs to be converted to the text-based format, so please proceed with the next steps...

 3. Make a copy of the `DER` file and put it into any convenient folder on your hard drive

 4. Using a command line window such as `PowerShell` or `Command Prompt`, navigate to the folder where the `DER` file was copied to and then execute the following command line

     ```bash
     openssl x509 -inform der -in <device_certificate_name>.der -out <device_certificate_name>.pem
     ```

 5. Rename the <`device_certificate_name`>.PEM file to <`device_certificate_name`>.CER

 6. Make a copy of the <`device_certificate_name`>.CER file and put it into the `WFI32-IOT` Mass Storage Device

 7. Confirm that the `WFI32-IOT` Mass Storage Device now contains both `CER` & `DER` files. Both files contain the same device certificate information but in 2 different formats

     <img src=".//media/image20.png">