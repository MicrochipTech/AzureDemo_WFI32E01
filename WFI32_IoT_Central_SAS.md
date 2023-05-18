# Connecting the WFI32E01 to Azure IoT Central using Symmetric Key Attestation (SAS Token)

## Introduction

[Azure IoT Central](https://docs.microsoft.com/en-us/azure/iot-central/core/overview-iot-central) is an IoT application platform that reduces the burden and cost of developing, managing, and maintaining enterprise-grade IoT solutions. Choosing to build with IoT Central gives you the opportunity to focus time, money, and energy on transforming your business with IoT data, rather than just maintaining and updating a complex and continually evolving IoT infrastructure.

The web UI lets you quickly connect devices, monitor device conditions, create rules, and manage millions of devices and their data throughout their life cycle. Furthermore, it enables you to act on device insights by extending IoT intelligence into line-of-business applications.

[IoT Plug and Play](https://docs.microsoft.com/en-us/azure/iot-develop/overview-iot-plug-and-play) enables solution builders to integrate IoT devices with their solutions without any manual configuration. At the core of IoT Plug and Play, is a device model that a device uses to advertise its capabilities to an IoT Plug and Play-enabled application. This model is structured as a set of elements that define:

- `Properties` that represent the read-only or writable state of a device or other entity. For example, a device serial number may be a read-only property and a target temperature on a thermostat may be a writable property

- `Telemetry` which is the data emitted by a device, whether the data is a regular stream of sensor readings, an occasional error, or an information message

- `Commands` that describe a function or operation that can be done on a device. For example, a command could reboot a gateway or take a picture using a remote camera

As a solution builder, you can use IoT Central to develop a cloud-hosted IoT solution that uses IoT Plug and Play devices. IoT Plug and Play devices connect directly to an IoT Central application where you can use customizable dashboards to monitor and control your devices. You can also use device templates in the IoT Central web UI to create and edit [Device Twins Definition Language (DTDL)](https://github.com/Azure/opendigitaltwins-dtdl) models.

## Program the Plug and Play Demo

1. Clone/download the MPLAB X demo project by issuing the following commands in a `Command Prompt` or `PowerShell` window. This operation will create a new folder in the directory where this command has been executed

    ```bash
    git clone <Demo_URL>
    ```
    NOTE: <Demo_URL> is the link to the origin repository (e.g. https://github.com/MicrochipTech/AzureDemo_WFI32E01)

    RECOMMENDATION: Put the newly-created folder in a root directory (e.g. C:\) to minimize the length of the path. Due to the large number of source files included in each project, it is advised to minimize the absolute path names for each file. In some operating systems (e.g. Windows), the longer path names could cause issues with loading/building the projects

2. Perform one of the following steps based on the specific WFI32E01 development board being used: 

- [WFI32-Curiosity](https://www.microchip.com/en-us/development-tool/EV12F11A): Use a jumper to short the `PKOB` pins of jumper `J302`. Connect the board to the PC using the connector (located in one corner of the board) labeled `USB DEBUG` (note there are 2 different USB connectors on the board).

- [WFI32-IoT](https://www.microchip.com/en-us/development-tool/ev36w50a): Connect the board to PC, then make sure a device named `CURIOSITY` shows up as a disk drive (i.e. Mass Storage Device) on the `Desktop` or in a `File Explorer` window

    <img src=".//media/image17a.png" />

3. Launch the `MPLAB X` IDE (this tool should have been previously installed and most likely resides in the \Program Files\Microchip\ folder)

    <img src=".//media/image18a.png" width=200 />

    Once the MPLAB X IDE has finished its initialization routines, you should notice the "Kit Window" that acknowledges an active connection to the WFI32-IoT Development Board (this does not apply to the PIC32 WFI32E Curiosity Development Board - the window will say there is no kit connected which is fine)

    <img src=".//media/image18b.png"  />

4. Navigate to the main toolbar's `File` > `Open Project` operation to load the demo project folder (\*.X) located at `[your_path]\AzureDemo_WFI32E01\firmware\AzureDemo_WFI32-IoT.X` (or `AzureDemo_WFI32-Curiosity.X`)

    <img src=".//media/image19a.png" width=200 />

   If the `load error` message in red appears in the `Output` window, click on the `Resolve DFP for configuration: default` link

    <img src=".//media/image21.png" style="width:6.5in;height:1.00833in" alt="A screenshot of a cell phone Description automatically generated" />

5. Set the `AzureDemo_WFI32-IoT` (or `AzureDemo_WFI32-Curiosity`) project as the main (currently focused/active) project by right-clicking on it and selecting `Set as Main Project`

    <img src=".//media/image40.png" width=200 />

6. In the `Projects` window, open the `sample_config.h` header file and modify the below lines of code so that the three compiler definitions are **disabled** (commented out)

    <img src=".//media/image41a.png" width=200 />

    <img src=".//media/image41c.png" style="width:5.in;height:1.0in" alt="A screenshot of a cell phone Description automatically generated" />

7. Verify the project properties are set correctly before building the project by executing the following steps:

    - right-click on the `AzureDemo_WFI32-IoT` (or `AzureDemo_WFI32-Curiosity`) project
    - select `Properties`
    - under `Connected Hardware Tool`, select the name which has been pre-assigned to your specific board (e.g. "Curiosity/Starter Kits (PKOB4)", "WFI32-IoT Board-SN", etc.)
    - select the latest `PIC32MZ-W_DFP` version
    - select XC32 version 4.10 for `Compiler Toolchain`

        <img src=".//media/image42.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

    If any changes were made in the project properties window, the `Apply` button should become enabled.  Make sure to hit the `Apply` button before hitting `OK`

8. Right-click on the active project and select `Clean`. Right-click the project again and select `Make and Program Device`. This operation will automatically build the project before attempting to program the target device. The build process starting from a clean state will take a while, so this might be a good time to go grab a cup of coffee :)

    **NOTE**: If the project fails to build, use the [MPLAB IPE method](MPLAB_IPE.md) to program the board instead of using the MPLAB X IDE.

9. After the `BUILD SUCCESSFUL` message appears in the Output window, the application HEX file will be programmed onto the development board. Once programming has finished, the board will automatically reset and start running its application code. The Mass Storage Device will reinitialize and take on the name of `WFI32-IOT`

10. Once the `WFI32-IOT` Mass Storage Device appears on the Desktop or in a File Explorer window, open the drive and confirm that the `CLOUD.CFG` file exists. Open the file and confirm that the contents contain the following structure fields:

        {
	        "ID_SCOPE":	"0ne008AC844",
	        "REGISTRATION_ID":	"PIC32MZW1",
	        "PRIMARY_KEY":	"mujHRQMx8dUsZETtlWxSonGZ24++L69c8KjIvZDT+5M="
        }

    If the structure contains different fields, copy the `CLOUD.CFG` file from this [folder](./WFI32-IOT%20MSD/) and overwrite the file in the `WFI32-IOT` MSD (or just copy and paste the above and save changes).

11. Quit the MPLAB X IDE by selecting `MPLAB X IDE` &gt; `Quit MPLAB X IDE` from the main toolbar. Please confirm the application has actually been closed before proceeding

12. Set up a Command Line Interface (CLI) to the board - choose one of the following based on the development board being used:

- If using the [PIC32 WFI32E Curiosity](https://www.microchip.com/en-us/development-tool/EV12F11A) board, a [USB-to-UART converter](https://www.newark.com/c/cable-wire-cable-assemblies/cable-assemblies/usb-adapter-cables?conversion-type=usb-to-uart-converter) needs to be connected to the U1RX & U1TX pins of the GPIO Header (J207). Next, set the jumper on `J302` so that the `VBUS` pins are shorted. Then, disconnect the USB cable from the `USB DEBUG` port and connect it to the other USB port labeled `USB POWER` port. Open a serial terminal (e.g. PuTTY, TeraTerm, etc.) and connect to the COM port corresponding to the USB-to-UART converter at `115200 baud`. Press the `MCLR` button on the board.

    <img src=".//media/WFI32-Curiosity_J207.png"/>

- If using the [WFI32-IoT](https://www.microchip.com/en-us/development-tool/ev36w50a) board, open a serial terminal (e.g. PuTTY, TeraTerm, etc.) and connect to the COM port corresponding to your board at `115200 baud` (e.g. open PuTTY Configuration window &gt; choose `session` &gt; choose `Serial`&gt; enter/select the right COMx port). You can find the right COM port number by opening your PC’s `Device Manager` &gt; expand `Ports(COM & LPT)` &gt; take note of `USB Serial Device (COMx)`

    <img src=".//media/image43.png"/>

13. Before typing anything in the terminal emulator window, **disable** the local echo feature in the terminal settings for best results.  In the terminal window, hit `[RETURN]` to bring up the Command Line Interface prompt (which is simply the `>` character). Type `help` and then hit `[RETURN]` to get the list of available commands for the CLI.  The Command Line Interface allows you to send simple ASCII-string commands to set or get the user-configurable operating parameters of the application while it is running

    <img src=".//media/image44.png" width=200 />

14. Enter in the `wifi` command on the CLI. You should see that the WFI32 Development Board replies with a message that it is *not* currently connected to Wi-Fi

    <img src=".//media/image45.png" width=200 />

15. Configure the WFI32 Development Board's device settings with your wireless router’s SSID and password. To be on the safe side, there should be no spaces used in the SSID and password for your network, and the Wi-Fi Access Point should be operating in the 2.4 GHz frequency band.

    The easiest way to configure the device's Wi-Fi settings is to open the `WFI32-IOT` Mass Storage Device and double-click on the `clickme.html` file - then follow the steps shown on the resulting web page

    <img src=".//media/image45a.png" width=400 />

    If for some reason this method does not work, you can also go to [Microchip IoT WiFi Configuration](https://iot.microchip.com/wificfg). As a last resort, you can always manually edit the `WIFI.CFG` file located in the `WFI32-IOT` Mass Storage Device as the network credentials are saved to this file.
    
    You can use the text editor of your choice to confirm that the authentication settings for your Wi-Fi network were correctly saved to the `WIFI.CFG` file. Based on your Wi-Fi network's security standard, the one line in the `WIFI.CFG` file should look like one of the following examples: 

    - Open Unsecured Network (no password protection)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,,1
        ```
    - Wi-Fi Protected Access 2 (WPA2)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,MY_PSWD,2
        ```
    - Wired Equivalent Privacy (WEP)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,MY_PSWD,3
        ```
    - Wi-Fi Protected Access 3 (WPA3)
        ```bash
        CMD:SEND_UART=wifi MY_SSID,MY_PSWD,4
        ```

16. [WFI32-IoT only - WFI32 Curiosity users can skip this step] Type the `reset` command on the serial terminal CLI. Within a few seconds, you should see the Blue LED on the WFI32-IoT Development Board stay constantly on - signifying that the board has successfuly connected to your Wi-Fi network using the settings stored in the `WIFI.CFG` file

    NOTE: Do not proceed until the WFI32 Development Board has established a successful connection to your Wi-Fi network - the Blue LED needs to be always on!

## Create an IoT Central Application

IoT Central allows you to create an application dashboard to monitor the telemetry and take appropriate actions based on customized rules.  To access all of your custom applications, you must be signed into the [Azure IoT Central Portal](https://apps.azureiotcentral.com) (it is recommended to bookmark this link for later use).

1. Review the below recomendations for creating the IoT Central application using the [Azure IoT Central Builder](https://apps.azureiotcentral.com/build) and then create your IoT Central application by clicking [here](https://docs.microsoft.com/en-us/azure/iot-central/core/quick-deploy-iot-central).

    NOTE: **Only** perform the steps in the first 2 sections titled "Prerequisites" and "Create an application"; once the application is created, stop and return to this page (i.e. **DO NOT** do the section titled "Register a device" as that will be done in a future step).

    If you are not currently logged into your [Microsoft account](https://account.microsoft.com/account), you will be prompted to sign in with your credentials to proceed.  
 
    - Choose a unique `Application name` which (will result in a unique `URL`) for accessing your application. Azure IoT Builder will populate a suggested unique `Application name` which can/should be leveraged, resulting in a unique `URL`. Take note of the unique/customizable portion of the `URL` (e.g. "custom-120683lb8ae") as it will be needed in a future step (suggest copy and pasting the exact text into a temporary doc file)

        <img src=".//media/image80a.png">

    - If you select the **Free** plan, you can connect up to 5 devices for free.  However, the free trial period will expire after 7 days which means a [paid pricing plan](https://azure.microsoft.com/en-us/pricing/details/iot-central/) will need to be selected to continue using the application.  Of course, there is nothing to stop you from creating a new free trial application but the device will need to be configured for the app from scratch.  Since the **Standard** plans each allow 2 free devices with no time-restricted trial period, if you only plan on evaluating 1 or 2 devices for connecting to the IoT Central app, then it's best to choose the **Standard 2** plan to get the highest total allowable number of messages (30K per month)

        <img src=".//media/image80b.png">

    - `Billing info` section: If there is an issue with selecting an existing subscription in the drop-down list (or no subscriptions appear in the list at all), click on the `Create subscription` link to create a new subscription to use for the creation of this application.  Take note of the exact subscription name (e.g. "Azure subscription 1") which was selected as it will be needed in a future step (suggest typing the exact text into a temporary doc file)
    
        <img src=".//media/image80c.png">

    - NOTE: If the message `Something went wrong` appears underneath the `Azure subscription` field, open up a web browser and log into your account using the [Azure portal](https://portal.azure.com) then retry selecting (or creating) a valid subscription

        <img src=".//media/image80d.png">

2. Once the application has been created, the application will be automatically saved and accessible in your [IoT Central Portal](https://apps.azureiotcentral.com) (it might be a good idea to bookmark this link now).

3. [FYI] Look up the settings for your application by using the left-hand navigation pane to select `Settings` &gt; `Application` &gt; `Management`

4. [FYI] To access your IoT Central application(s) in the future, log into the [Azure IoT Central Portal](https://apps.azureiotcentral.com) and click on `My apps` (or go directly to the custom URL defined for your application)

    <img src=".//media/image108.png" style="width:5.in;height:1.98982in" alt="A screenshot of a cell phone Description automatically generated" />

## Connect and Test the WFI32E01 Device

1.	Look up the `ID Scope` for your IoT Central application (using the left-hand navigation pane, select `Permissions` &gt; `Device connection groups`)

    <img src=".//media/image84a.png" width=400 />

2. Enter in the `cloud` command on the CLI. You should see that the WFI32 Development Board replies with a message that it is *not* currently connected to the Cloud

    <img src=".//media/image85.png" width=200 />

3. Set the ID Scope value in the WFI32E01 Development Board by editing the `CLOUD.CFG` file which is accessed via the `WFI32-IOT` disk drive. The unique ID Scope value will be used by the development board to connect to the built-in DPS used by your IoT Central application. Using the text editor of your choice, modify the existing line in the file so that the ID_SCOPE string is set to the value assigned to your IoT Central application, for example
    ```bash
    "ID_SCOPE":	"0ne0041E397",
    ```

4. Save the changes to the `CLOUD.CFG` file and then re-open the file to confirm that your changes were correctly saved for the `ID_SCOPE` value

5. Go back to your web browser to access the Azure IoT Central application.  Use the left-hand side pane and select `Connect` > `Devices`. Click `+ New` at the top to add a new device to your application (a one-time individual enrollment)

    <img src=".//media/image86.png">

6. In the `Create a new device` window, it is suggested to accept the default device name and ID that are randomly-generated (but can be changed to more meaningful names if you like). Select `WFI32_IoT_WM;3` or `WFI32_Curiosity_WM;1` for the device template, based on the specific board being used. Click on the `Create` button

    <img src=".//media/image87.png" style="width:5.in;height:3.68982in" alt="A screenshot of a cell phone Description automatically generated" />

7. Confirm that the new device is listed as `Registered` and that `Simulated = No`

    <img src=".//media/image88.png">

8. Click on the `Device name` instance and then click on `Connect` at the top of the device's page

    <img src=".//media/image89.png" style="width:5.in;height:1.28982in" alt="A screenshot of a cell phone Description automatically generated" />

9. The `Device connection groups` pop-up window should appear. Confirm that `Authentication type` is set to `Shared access signature (SAS)`. Edit the `CLOUD.CFG` file one final time by updating the `REGISTRATION_ID` string with the Device ID and then `PRIMARY_KEY` string with the Primary key values that are shown on the Device connection groups pop-up window. Click on each of the "Copy to clipboard" icons and paste the corresponding text into the `CLOUD.CFG` file. Close the pop-up window when finished and make sure to save this last set of changes to the `CLOUD.CFG` file
    ```bash
    "REGISTRATION_ID":	"2ep4cjs1gka",
    "PRIMARY_KEY": "d59fA5UptJ6bh+Il6YgOIZTjly6hN0h0p5REcUm7osk="
    ```
    <img src=".//media/image89a.png" width=200 />

10. Enter in the `reset` command on the CLI

11. View the diagnostic messages output to the serial terminal program to confirm that the demo application is going through its startup sequence. When the "Connected to Azure IoT Hub" message appears, the Green LED should stay constantly on. By default, the temperature and light sensor readings should be sent to your IoT Central application every 5 seconds. For the WFI32-IoT Development Board, the Yellow LED (Data) should be toggling approximately once per second

    <img src=".//media/image89c.png"/>

12. Press each of the `SW1` & `SW2` user buttons on the WFI32-IoT Development Board a few times (the WFI32 Curiosity Board only has `SW1`). The Red LED should toggle for each user button press that is detected

    <img src=".//media/image89d.png" style="width:5.in;height:2.08982in" alt="A screenshot of a cell phone Description automatically generated" />

13. Click on the `Raw data` tab and confirm that the button press telemetry messages were received (scroll the web page to the right to view the `Button Press Count` & `Button Push Event` columns)

    <img src=".//media/image90.png" width=200 />

14. Click on the `Refresh` icon (in the top right area of the page) to display all messages received since the previous page refresh operation.  Confirm that periodic telemetry messages are being continuously received approximately every 5 seconds (which is the default interval value for the `telemetryInterval` property that dictates the telemetry reporting frequency)

    <img src=".//media/image91a.png" style="width:5.in;height:1.08982in" alt="A screenshot of a cell phone Description automatically generated" />

    <img src=".//media/image92a.png"/>

15. [WFI32-IoT users only] Increase the ambient light source shining on top of the board. Wait approximately 10-15 seconds.  Click on the `Refresh` icon to confirm that the light sensor value has increased

    <img src=".//media/image93a.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />

16. Click [here](./DeviceTemplate_CreatingViews.md) to create an additional "Properties" view that allows you to change any of the Cloud-writable properties. Once this new view has been added to the device template, click on the Properties view and type in a new value for the Telemetry Interval. Click on the **Save** icon to send the property update request to the physical device. You should see the status of the property listed as "Pending" until a confirmation has been received from the physical device that the property was successfully updated. At this point in time, the status of the property should revert back to the "Accepted" state.

    Depending how quickly the write property response is received, it is possible that IoT Central will show the value as "Pending". If the device is offline or doesn't respond to a writable property request, the value can display as "Pending" indefinitely in IoT Central until a valid property update acknowledge has been received from the device.

17. Note there are other views besides the `Raw data` tab. Click on the `About` and `Overview` tabs to view the property values and telemetry data of the device. If any of the Click boards are installed, you should see the telemetry being reported from the additional sensors (e.g. ULP_temperature, ULP_pressure, VAV_temperature, VAV_pressure)

18. Click on the `Command` tab. Type a text message in the `String to send` box and then click on the `Run` button. Confirm that the message was received in the serial console window

    <img src=".//media/image93b_01.png" width=200 />

    <img src=".//media/image93b_02.png" style="width:5.0in;height:1.2in" alt="A screenshot of a cell phone Description automatically generated" />

19. Click on the `command history` link and note the response that was received from the device

    <img src=".//media/image93c.png" width=200 />

    <img src=".//media/image93d.png" width=200 />

20. Click on the `Properties` tab. This view allows you to change the state of the Yellow LED (WFI32-IoT board only) and update the telemetry reporting interval since these are properties that have been defined as "writable" by the cloud. The remaining LEDs are used as status indicators so they are not writable from the cloud (read only properties).

    Feel free to change the Yellow LED state between On, Off, and Blinking and then visually confirm if the Yellow LED physically changes its state. In addition, try changing the telemetry interval and verify if the telemetry is being updated more or less frequently based on your selection. For any property changes to actually take effect, the `Save` icon must be clicked after making your selections

    <img src=".//media/image94.png" width=200 />

## Creating a Dashboard for Custom Data Visualization

Try creating an IoT Central dashboard by clicking [here](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-add-tiles-to-your-dashboard) and following the procedure outlined in the online guide. The below screen captures show the possibilities of dashboard components that can highlight the telemetry data and properties facilitated by the `IoT Plug and Play` interface.  Note that multiple devices can be selected for each tile to allow groups of devices to be visualized within a single tile. 

<img src=".//media/HVAC_Dashboard.png" />

<img src=".//media/image95.png" />

<img src=".//media/image96.png" />

<img src=".//media/image97.png" />

<img src=".//media/image98.png" />
