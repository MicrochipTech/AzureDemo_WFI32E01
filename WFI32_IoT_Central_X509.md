# Connecting the WFI32-IoT Development Board (Part No. EV36W50A) to Azure IoT Central using X.509 Certificate Attestation

NOTE: Should you encounter any issues/obstacles with the following procedure, check out the [FAQ section](./FAQ.md)

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
    NOTE: <Demo_URL> is the link to the origin repository (e.g. https://github.com/Microchip-Azure-Demos/WFI32-IoT)

2. Connect the board to PC, then make sure a device named `CURIOSITY` shows up as a disk drive (i.e. Mass Storage Device) on the `Desktop` or in a `File Explorer` window

    <img src=".//media/image17a.png" />

3. Launch the `MPLAB X` IDE (this tool should have been previously installed and most likely resides in the \Program Files\Microchip\ folder)

    <img src=".//media/image18a.png" style="width:2.0in;height:4.0833in" alt="A screenshot of a cell phone Description automatically generated" />

    Once the MPLAB X IDE has finished its initialization routines, you should notice the "Kit Window" that acknowledges an active connection to the WFI32-IoT Development Board

    <img src=".//media/image18b.png"  />

4. Navigate to the main toolbar's `File` > `Open Project` operation to load the demo project folder (\*.X) located at `[your_path]\Microchip-WFI32-IoT\firmware\WFI32-IoT_Azure.X`

    <img src=".//media/image19a.png" style="width:3.0in;height:2.0833in" alt="A screenshot of a cell phone Description automatically generated" />

    <img src=".//media/image19b.png" style="width:4.0in;height:2.23in" alt="A screenshot of a cell phone Description automatically generated" />

   If the `load error` message in red appears in the `Output` window, click on the `Resolve DFP for configuration: default` link

    <img src=".//media/image21.png" style="width:6.5in;height:1.00833in" alt="A screenshot of a cell phone Description automatically generated" />

5. Set the `WFI32-IoT_Azure` project as the main (currently focused/active) project by right-clicking on it and selecting `Set as Main Project`

    <img src=".//media/image40.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

6. In the `Projects` window, open the `sample_config.h` header file and confirm that the below three compiler definitions are active

    <img src=".//media/image41a.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

    <img src=".//media/image41b.png" style="width:5.in;height:1.0in" alt="A screenshot of a cell phone Description automatically generated" />

7. Verify the project properties are set correctly before building the project by executing the following steps:

    - right-click on the `WFI32-IoT_Azure` project
    - select `Properties`
    - under `Connected Hardware Tool`, select the name which has been pre-assigned to your specific board (e.g. "Curiosity/Starter Kits (PKOB4)", "WFI32-IoT Board-SN", etc.)
    - select the latest `PIC32MZ-W_DFP` version
    - select the latest XC32 version for `Compiler Toolchain`

        <img src=".//media/image42.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

    If any changes were made in the project properties window, the `Apply` button should become enabled.  Make sure to hit the `Apply` button before hitting `OK`

8. Right-click on the `WFI32-IoT_Azure` project and select `Clean`. Right-click the project again and select `Make and Program Device`. This operation will automatically build the project before attempting to program the target device. The build process starting from a clean state will take a while, so this might be a good time to go grab a cup of coffee :)

9. After the `BUILD SUCCESSFUL` message appears in the Output window, the application HEX file will be programmed onto the WFI32-IoT Development Board. Once programming has finished, the board will automatically reset and start running its application code. The Mass Storage Device will reinitialize and take on the new name of `WFI32-IOT`

10. Quit the MPLAB X IDE by selecting `MPLAB X IDE` &gt; `Quit MPLAB X IDE` from the main toolbar. Please confirm the application has actually been closed before proceeding...

11. Set up a Command Line Interface (CLI) to the board. Open a serial terminal (e.g. PuTTY, TeraTerm, etc.) and connect to the COM port corresponding to your board at `115200 baud` (e.g. open PuTTY Configuration window &gt; choose `session` &gt; choose `Serial`&gt; enter/select the right COMx port). You can find the right COM port number by opening your PC’s `Device Manager` &gt; expand `Ports(COM & LPT)` &gt; take note of `USB Serial Device (COMx)`

    <img src=".//media/image43.png"/>

12. Before typing anything in the terminal emulator window, **disable** the local echo feature in the terminal settings for best results.  In the terminal window, hit `[RETURN]` to bring up the Command Line Interface prompt (which is simply the `>` character). Type `help` and then hit `[RETURN]` to get the list of available commands for the CLI.  The Command Line Interface allows you to send simple ASCII-string commands to set or get the user-configurable operating parameters of the application while it is running

    <img src=".//media/image44.png" style="width:5.in;height:2.68982in" alt="A screenshot of a cell phone Description automatically generated" />

13. Enter in the `wifi` command on the CLI. You should see that the WFI32-IoT Development Board replies with a message that it is *not* currently connected to Wi-Fi

    <img src=".//media/image45.png" style="width:5.in;height:1.58982in" alt="A screenshot of a cell phone Description automatically generated" />

14. Configure the WFI32-IoT Development Board's device settings with your wireless router’s SSID and password. To be on the safe side, there should be no spaces used in the SSID and password for your network, and the Wi-Fi Access Point should be operating in the 2.4 GHz frequency band.

    The easiest way to configure the device's Wi-Fi settings is to open the `WFI32-IOT` Mass Storage Device and double-click on the `clickme.html` file - then follow the steps shown on the resulting web page

    <img src=".//media/image45a.png" style="width:5.in;height:1.58982in" alt="A screenshot of a cell phone Description automatically generated" />

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

15. Type the `reset` command on the serial terminal CLI. Within a few seconds, you should see the Blue LED on the WFI32-IoT Development Board stay constantly on - signifying that the board has successfuly connected to your Wi-Fi network using the settings stored in the `WIFI.CFG` file

    NOTE: Do not proceed until the WFI32-IoT Development Board has established a successful connection to your Wi-Fi network - the Blue LED needs to be always on!

## Confirm Access to the Root CA, Signer CA, and Device Certificates

The `WFI32-IOT` Mass Storage Device (MSD) contains the 3 certificates saved in Privacy Enhanced Format (PEM) which is a Base64 encoded binary format. PEM certificates are frequently used for web servers as they can easily be translated into readable data using a simple text editor. Generally when a PEM encoded file is opened in a text editor, it contains very distinct headers and footers.

Whenever the demo application is reset, it will check for the presence of all 3 files on the `WFI32-IOT` drive. Each file has the *.PEM suffix and is named to distinguish between the root, signer, and device certificates; e.g.

<img src=".//media/image46.png">

If any of the files do not exist when the demo application is reset, the missing file(s) will be automatically generated. In case any of these files are accidentally edited, simply delete the certificate(s) from the `WFI32-IOT` MSD and reset the application so that they will be automatically regenerated.

## Create an IoT Central Application

IoT Central allows you to create an application dashboard to monitor the telemetry and take appropriate actions based on customized rules.

1. Create a custom IoT Central application by accessing (clicking on) the existing [Microchip WFI32-IoT Application Template](https://apps.azureiotcentral.com/build/new/db598a83-644a-4e2c-b4e1-f3e52978d006) (if there is a problem with loading the template, refer to the [Create an application](https://docs.microsoft.com/en-us/azure/iot-central/core/quick-deploy-iot-central) section to create your IoT Central application from scratch). If you are not currently logged into your [Microsoft account](https://account.microsoft.com/account), you will be prompted to sign in with your credentials to proceed. If you do not have an existing Microsoft account, go ahead and create one now by clicking on the `Create one!` link

2. Azure IoT Builder will guide you through the process of creating your application. Review and select the various settings for your IoT Central application (if needed, refer to [Create an application](https://docs.microsoft.com/en-us/azure/iot-central/core/quick-deploy-iot-central) for additional guidance on selecting the settings for your application). Do not click the `Create` button just yet - only after reviewing and taking into consideration the following recommendations:
  
    - Choose a unique `Application name` (which will result in a unique `URL`) for accessing your application. Azure IoT Builder will populate a suggested unique `Application name` which can/should be leveraged, resulting in a unique `URL` which can be used to access this specific application in the future

        <img src=".//media/image80a.png">

    - If you select the **Free** plan, you can connect up to 5 devices for free.  However, the free trial period will expire after 7 days which means a [paid pricing plan](https://azure.microsoft.com/en-us/pricing/details/iot-central/) will need to be selected to continue using the application.  Of course, there is nothing to stop you from creating a new free trial application but the device will need to be configured for the app from scratch.  Since the **Standard** plans each allow 2 free devices with no time-restricted trial period, if you only plan on evaluating 1 or 2 devices for connecting to the IoT Central app, then it's best to choose the **Standard 2** plan to get the highest total allowable number of messages (30K per month)

        <img src=".//media/image80b.png">

    - `Billing info` section: If there is an issue with selecting an existing subscription in the drop-down list (or no subscriptions appear in the list at all), click on the `Create subscription` link to create a new subscription to use for the creation of this application.  Take note of the exact subscription name (e.g. "Azure subscription 1" like shown in the below screen shot) which was selected as it will be needed in a future step (suggest copying/pasting the exact text into a text editor file as temporary storage for the name)
       
        <img src=".//media/image80c.png">

        NOTE: If the message `Something went wrong` appears underneath the `Azure subscription` field, open up a web browser and log into your account using the [Azure portal](https://portal.azure.com) then retry selecting (or creating) a valid subscription

        <img src=".//media/image80d.png">
        
3. Click the `Create` button (the application will be automatically saved in your [IoT Central Portal](https://apps.azureiotcentral.com))

4. [FYI] Look up the settings for your application by using the left-hand navigation pane to select `Settings` &gt; `Application` &gt; `Management`

5. [FYI] To access your IoT Central application(s) in the future, log into the [Azure IoT Central Portal](https://apps.azureiotcentral.com) and click on `My apps` (or go directly to the custom URL defined for your application)

    <img src=".//media/image108.png" style="width:5.in;height:1.98982in" alt="A screenshot of a cell phone Description automatically generated" />

## Connect and Test the WFI32-IoT Device

1.	Look up the `ID Scope` for your IoT Central application (using the left-hand navigation pane, select `Permissions` &gt; `Device connection groups`)

    <img src=".//media/image84a.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

2. Enter in the `cloud` command on the CLI. You should see that the WFI32-IoT Development Board replies with a message that it is *not* currently connected to the Cloud

    <img src=".//media/image85.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />

3. Set the ID Scope value in the WFI32-IoT Development Board by editing the `CLOUD.CFG` file which is accessed via the `WFI32-IOT` disk drive. The unique ID Scope value will be used by the WFI32-IoT Development Board to connect to the built-in DPS used by your IoT Central application. Using the text editor of your choice, modify the existing line in the file so that the ID_SCOPE string is set to the value assigned to your IoT Central application, for example
    ```bash
    "ID_SCOPE":	"0ne0041E397",
    ```

4. Save the changes to the `CLOUD.CFG` file and then re-open the file to confirm that your changes were correctly saved for the `ID_SCOPE` value

## Create an Enrollment Group

An enrollment group is an entry for a group of devices that share a common attestation mechanism (e.g. device certificates all derived from a common root and/or signer certificate(s)). Using an enrollment group is recommended for a large number of devices that share an initial configuration and/or for devices that all go to the same tenant. Create an X.509 enrollment group for your IoT Central application so that hundreds, thousands, or even millions/billions of devices can automatically connect to your cloud application without any human intervention.

1. Using the left-hand side navigation pane of your IoT Central application, under `Security` select `Permissions` &gt; `Device connection groups`

   <img src=".//media/image81a.png" style="width:6.5.in;height:3.63506in" />

2. Click on the `+ New` button and create a new enrollment group using any name (with Group type = `IoT devices` and attestation type = `Certificates (X.509)`).  Hit the `Save` icon when finished

   <img src=".//media/image81b.png" style="width:6.5.in;height:3.63506in" />

3. Now that the new enrollment group has been created, click on `Manage Primary`

   <img src=".//media/image82.png" style="width:5.5.in;height:2.53506in" />

4. Click on `+ Add certificate` and browse to the **signer** certificate file (which should be located in the `WFI32-IOT` Mass Storage Device). Click the `Upload` button (then click on `Close` when the certificate has been accepted)

   <img src=".//media/image75.png" style="width:5.5.in;height:2.13506in" />

5. Click on the `Save` icon at the top of the page, and note the ID Scope which was created for the enrollment group. The X.509 enrollment group has been successfully created and should be ready to go!

    <img src=".//media/image83.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />

6. Enter in the `reset` command on the CLI (or hit the `RESET` button on the WFI32-IoT Development Board)

    <img src=".//media/image89b.png" style="width:5.in;height:1.58982in" alt="A screenshot of a cell phone Description automatically generated" />

7. View the diagnostic messages output to the serial terminal program to confirm that the demo application is going through its startup sequence. When the "Connected to Azure IoT Hub" message appears, both Blue (Wi-Fi) and Green (Cloud) LEDs should stay constantly on. By default, the temperature and light sensor readings should be sent to your IoT Central application every 5 seconds. The Yellow LED (Data) should be toggling approximately once per second

    <img src=".//media/image89c.png"/>

## Test Device Interaction with the Cloud

1. Press each of the `SW1` & `SW2` user buttons on the WFI32-IoT Development Board a few times. The Red LED should blink on each user button press that is detected

    <img src=".//media/image89d.png" style="width:5.in;height:2.08982in" alt="A screenshot of a cell phone Description automatically generated" />

2. Click on the `Raw data` tab and confirm that the button press telemetry messages were received (scroll the web page to the right to view the `Button Press Count` & `Button Push Event` columns)

    <img src=".//media/image90.png" style="width:5.in;height:1.48982in" alt="A screenshot of a cell phone Description automatically generated" />

3. Click on the `Refresh` icon (in the top right area of the page) to display all messages received since the previous page refresh operation.  Confirm that periodic telemetry messages are being continuously received approximately every 5 seconds (which is the default interval value for the `telemetryInterval` property that dictates the telemetry reporting frequency)

    <img src=".//media/image91a.png" style="width:5.in;height:1.08982in" alt="A screenshot of a cell phone Description automatically generated" />

    <img src=".//media/image92a.png"/>

4. Increase the ambient light source shining on top of the board. Wait approximately 10-15 seconds.  Click on the `Refresh` icon to confirm that the light sensor value has increased

    <img src=".//media/image93a.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />

5. Note there are other views besides the `Raw data` tab. Click on the `About` and `Overview` tabs to view the property values and telemetry data of the device. If any of the Click boards are installed, you should see the telemetry being reported from the additional sensors (e.g. ULP_temperature, ULP_pressure, VAV_temperature, VAV_pressure)

6. Click on the `Command` tab. Type a text message in the `String to send` box and then click on the `Run` button. Confirm that the message was received in the serial console window

    <img src=".//media/image93b_01.png" style="width:5.0in;height:2.2in" alt="A screenshot of a cell phone Description automatically generated" />

    <img src=".//media/image93b_02.png" style="width:5.0in;height:1.2in" alt="A screenshot of a cell phone Description automatically generated" />

7. Click on the `command history` link and note the response that was received from the device

    <img src=".//media/image93c.png" style="width:3.0in;height:0.7in" alt="A screenshot of a cell phone Description automatically generated" />

    <img src=".//media/image93d.png" style="width:4.0in;height:1.25in" alt="A screenshot of a cell phone Description automatically generated" />

8. Click on the `Properties` tab. This view allows you to change the state of the Yellow LED and update the telemetry reporting interval since these are properties that have been defined as writable by the cloud. The remaining LEDs are used as status indicators so they are not writable from the cloud. Feel free to change the Yellow LED's state between On, Off, and Blinking and visually confirm if the Yellow LED physically changes its state. Change the telemetry interval and verify if the telemetry is being updated more or less frequently based on your selection. For any property changes to actually take effect, the `Save` icon must be clicked after making your selections

    <img src=".//media/image94.png" style="width:5.0in;height:4.18982in" alt="A screenshot of a cell phone Description automatically generated" />

## Configure the Dashboard for Data Visualization

1. Navigate to the left-hand vertical toolbar and click on the `Dashboards` icon

    <img src=".//media/image100.png" style="width:5.in;height:0.98982in" alt="A screenshot of a cell phone Description automatically generated" />

2. Towards the top of the web page, click on the dashboard selector and change the view to `WFI32-IoT Light and Temperature Sensors`

    <img src=".//media/image100a.png">

3. Towards the top of the web page, click on the `Edit` icon

    <img src=".//media/image101.png" style="width:5.in;height:0.38982in" alt="A screenshot of a cell phone Description automatically generated" />

4. For **all** of the existing tiles named `Light` or `Temperature`, click on the upper right-hand corner of the tile to select `Edit` in the drop-down menu

    <img src=".//media/image102a.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />
    <img src=".//media/image102b.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />

5. Select `Device Group` > `WFI32-IoT WM;x - All devices` and then check the box for your specific device name for `Devices`

    <img src=".//media/image103a.png" style="width:5.in;height:2.08982in" alt="A screenshot of a cell phone Description automatically generated" />

6. Under the `Telemetry` category, click on `+ Capability` and select the parameter pertaining to the title of the tile (e.g. `Brightness from Light Sensor (WFI32-IoT)` for each of the `Light` tiles or `Temperature (WFI32-IoT)` for each of the `Temperature` tiles)

    <img src=".//media/image104a.png" style="width:5.in;height:0.89082in" alt="A screenshot of a cell phone Description automatically generated" />
    <img src=".//media/image104b.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />
    <img src=".//media/image104c.png" style="width:5.in;height:1.18982in" alt="A screenshot of a cell phone Description automatically generated" />

7. Click on `Update` and repeat the process for the remainder of the existing tiles

    <img src=".//media/image105.png" style="width:5.in;height:0.48982in" alt="A screenshot of a cell phone Description automatically generated" />

8. After every tile has been configured to visualize your device's sensor data, click on the `Save` icon to save the latest changes to the dashboard

    <img src=".//media/image106.png" style="width:5.in;height:0.38982in" alt="A screenshot of a cell phone Description automatically generated" />

9. Confirm that the dashboard is being continuously updated with the expected telemetry data received from the device.  For example, adjust the ambient light source directed at the board and observe that the light sensor values are changing accordingly

    <img src=".//media/image107.png" style="width:5.in;height:2.58982in" alt="A screenshot of a cell phone Description automatically generated" />

10. [FYI] To access your IoT Central application(s) in the future, log into the [Azure IoT Central Portal](https://apps.azureiotcentral.com) and click on `My apps` (or go directly to the custom URL defined for your application)

    <img src=".//media/image108.png" style="width:5.in;height:1.98982in" alt="A screenshot of a cell phone Description automatically generated" />

## WFI32-IoT Differential Pressure Sensors

If you have one or both of the optional MikroElektronika "Ultra-Low Press" and/or "VAV Press" Click boards installed on the WFI32-IoT Development Board's mikroBUS™ socket, use the pre-configured example dashboard for visualizing the differential pressure and temperature sensor telemetry from the Click board(s).

1. Using the left-hand navigation pane, select `Analyze` &gt; `Dashboards`

2. Towards the top of the web page, click on the dashboard selector and change the view to `WFI32-IoT Differential Pressure Sensors`

    <img src=".//media/image109.png">

3. Click on the `Edit` icon

4. For each of the tiles used to display device data (and does not currently show any data), configure each tile by adding your device to the tile by executing the following steps:

    - click `Edit` icon
    - select `WFI32_IoT_WM;x` for `Device group`
    - click `Select All` for `Devices`
    - click the `Update` button

    NOTE: Whenever a device group has been reselected, the tile may inadvertently drop the telemetry parameters that were selected previously - so you may have to add those telemetry parameters back into the tile before clicking on the `Update` button

5. Click on the `Save` icon

    <img src=".//media/image110.png" style="width:7.0in;height:6.0in" alt="A screenshot of a cell phone Description automatically generated" />

6. The demo application uses two of the WFI32 module's pins as GPIO's to convey the status of the board. Pins 1 & 15 of the mikroBUS™ connector can be read simultaneously as a 2-bit value representing 4 possible states ('0' = Logic LOW, '1' = Logic HIGH)

    <img src=".//media/image111.png" style="width:5.0in;height:1.5in" alt="A screenshot of a cell phone Description automatically generated" />

    NOTE: An alarm condition is triggered whenever the differential pressure reading of either sensor has exceeded a pre-set value which can be easily changed in the `app.h` header file of the project

    ```bash
    #define ALARM_PRESSURE_PA 25.0
    ```

    <img src=".//media/image112.png" style="width:2.5in;height:2.7in" alt="A screenshot of a cell phone Description automatically generated" />
 
## Expand the Dashboard with Additional Tiles

To create additional tiles for your IoT Central dashboard, refer to [Configure the IoT Central application dashboard](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-add-tiles-to-your-dashboard). The below screen captures show additional possibilities of dashboard components that can highlight the telemetry data and properties facilitated by the `Plug and Play` interface.  Note that multiple devices can be selected for each tile to allow groups of devices to be visualized within a single tile. 

<img src=".//media/image95.png" style="width:5.in;height:3.34982in" alt="A screenshot of a cell phone Description automatically generated" />

<img src=".//media/image96.png" style="width:5.in;height:4.4182in" alt="A screenshot of a cell phone Description automatically generated" />

<img src=".//media/image97.png" style="width:5.in;height:3.34982in" alt="A screenshot of a cell phone Description automatically generated" />

<img src=".//media/image98.png" style="width:5.in;height:3.34982in" alt="A screenshot of a cell phone Description automatically generated" />
