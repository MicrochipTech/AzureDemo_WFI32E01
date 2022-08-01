# Connecting the WFI32-IoT Board (Part No. EV36W50A) to Azure IoT Central

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

1. Clone/download the MPLAB X demo project by issuing the following commands in a `Command Prompt` or `PowerShell` window

    ```bash
    git clone https://github.com/Azure-Samples/Microchip-WFI32-IoT.git
    ```

2. Connect the board to PC, then make sure a device named `WFI32-IOT` shows up as a disk drive (i.e. Mass Storage Device) on the `Desktop` or in a `File Explorer` window

3. Launch the MPLAB X IDE and navigate to the main toolbar's `File` > `Open Project` operation to load the demo project folder (\*.X) located at `[your_path]\Microchip-WFI32-IoT\firmware\WFI32-IoT_Azure.X`

   If the `load error` message in red appears in the `Output` window, click on the `Resolve DFP for configuration: default` link

    <img src=".//media/image21.png" style="width:6.5in;height:1.00833in" alt="A screenshot of a cell phone Description automatically generated" />

4. Set the `WFI32-IoT_Azure` project as the main (currently focused/active) project by right-clicking on it and selecting `Set as Main Project`

    <img src=".//media/image41.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

5. Verify the project properties are set correctly before building the project. Right-click the project `WFI32-IoT_Azure` &gt; select `Properties` &gt; select "WFI32-IoT Board-SN" for `Connected Hardware Tool` &gt; select the latest `PIC32MZ-W_DFP` version &gt; select the latest XC32 version for `Compiler Toolchain`. If any changes were made in the project properties window, the `Apply` button should become enabled.  Make sure to hit the `Apply` button before hitting `OK`

6. Right-click the `WFI32-IoT_Azure` project and select `Make and Program Device`.  This operation will automatically clean and build the project before attempting to program the target device. After the `BUILD SUCCESSFUL` message appears in the Output window, the application HEX file will be programmed onto the WFI32-IoT Board. Once programming has finished, the board will automatically reset and start running its application code

7. Set up a Command Line Interface (CLI) to the board. Open a serial terminal (e.g. PuTTY, TeraTerm, etc.) and connect to the COM port corresponding to your board at `115200 baud` (e.g. open PuTTY Configuration window &gt; choose `session` &gt; choose `Serial`&gt; Enter the right COMx port). You can find the COM info by opening your PC’s `Device Manager` &gt; expand `Ports(COM & LPT)` &gt; take note of `Curiosity Virtual COM Port` 

 8. Before typing anything in the terminal emulator window, **disable** the local echo feature in the terminal settings for best results.  In the terminal window, hit `[RETURN]` to bring up the Command Line Interface prompt (which is simply the `>` character). Type `help` and then hit `[RETURN]` to get the list of available commands for the CLI.  The Command Line Interface allows you to send simple ASCII-string commands to set or get the user-configurable operating parameters of the application while it is running

    <img src=".//media/image44.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

9. Configure the WFI32-IoT Board's internal Wi-Fi settings with your wireless router’s SSID and password by editing the `WIFI.CFG` file located in the `WFI32-IOT` disk drive. Using the text editor of your choice, modify the one existing line in the file so that `my_SSID` and `my_PSWD` reflect the correct authentication settings for the wireless router you'd like the WFI32-IoT Board to connect with. Save the changes to the file and then re-open the file to confirm that your changes were correctly saved. To be on the safe side, there should be no spaces used in the SSID and password.
    ```bash
    CMD:SEND_UART=wifi my_SSID,my_PSWD,2
    ```

## Create an IoT Central Application for your Device

IoT Central allows you to create an application dashboard to monitor the telemetry and take appropriate actions based on customized rules.

1. Create a custom IoT Central application by starting with an existing [Microchip IoT Development Board Template](https://apps.azureiotcentral.com/build/new/e54c7769-30ed-4223-979f-2667013845fd) (if there is a problem with loading the template, refer to the [Create an application](https://docs.microsoft.com/en-us/azure/iot-central/core/quick-deploy-iot-central) section to create your IoT Central application from scratch). If you are not currently logged into your [Microsoft account](https://account.microsoft.com/account), you will be prompted to sign in with your credentials to proceed. If you do not have an existing Microsoft account, go ahead and create one now by clicking on the `Create one!` link

2. Azure IoT Builder will guide you through the process of creating your application. Review and select the various settings for your IoT Central application (if needed, refer to [Create an application](https://docs.microsoft.com/en-us/azure/iot-central/core/quick-deploy-iot-central) for additional guidance on selecting the settings for your application). Do not click the `Create` button just yet - only after reviewing and taking into consideration the following recommendations:
  
    - Choose a unique `Application name` (which will result in a unique `URL`) for accessing your application. Azure IoT Builder will populate a suggested unique `Application name` which can/should be leveraged, resulting in a unique `URL` shown on the screen. Take note of the unique/customizable portion of the `URL` (e.g. "custom-1pfphmras2b" like shown in the below screen shot) as it will be needed in a future step (suggest copy/pasting the exact text into a text editor doc file as temporary storage for the name)

        <img src=".//media/image80a.png">

    - If you select the **Free** plan, you can connect up to 5 devices for free.  However, the free trial period will expire after 7 days which means a [paid pricing plan](https://azure.microsoft.com/en-us/pricing/details/iot-central/) will need to be selected to continue using the application.  Of course, there is nothing to stop you from creating a new free trial application but the device will need to be configured for the app from scratch.  Since the **Standard** plans each allow 2 free devices with no time-restricted trial period, if you only plan on evaluating 1 or 2 devices for connecting to the IoT Central app, then it's best to choose the **Standard 2** plan to get the highest total allowable number of messages (30K per month)

        <img src=".//media/image80b.png">

    - `Billing info` section: If there is an issue with selecting an existing subscription in the drop-down list (or no subscriptions appear in the list at all), click on the `Create subscription` link to create a new subscription to use for the creation of this application.  Take note of the exact subscription name (e.g. "Azure subscription 1" like shown in the below screen shot) which was selected as it will be needed in a future step (suggest copying/pasting the exact text into a text editor file as temporary storage for the name)
       
        <img src=".//media/image80c.png">

        NOTE: If the message `Something went wrong` appears underneath the `Azure subscription` field, open up a web browser and log into your account using the [Azure portal](https://portal.azure.com) then retry selecting (or creating) a valid subscription

        <img src=".//media/image80d.png">
        
3. Click the `Create` button (the application will be automatically saved in your [IoT Central Portal](https://apps.azureiotcentral.com))

4.	Create the device template for the WFI32-IoT Board. This template defines the capabilities (telemetry, properties, commands) of the device model. You could manually create and add the device template to your IoT Central application, but why not import the device model from a JSON file that has already been created for the WFI32-IoT Board? Using the navigation pane on the left-hand side of the application, select `Connect` > `Device templates`

5.	If there is no existing device template that corresponds to "WFI32_IoT", click on the `+ New` button at the top of the page

    <img src=".//media/image70.png">

6.	Click on the `IoT device` tile to import an existing capability model, then scroll down to the bottom of the page and select `Next: Customize`

    <img src=".//media/image71.png">

7.	Type `WFI32_IoT_WM;1` in the `Device template name` field and click on the `Next: Review` button

    <img src=".//media/image72.png">

8.	Confirm that the information listed under `Basic info` is correct and click on the `Create` button

    <img src=".//media/image73.png">

9.	Click on the `Import a model` tile which will allow you to upload an existing device model file to IoT Central

    <img src=".//media/image74.png">

10.	Navigate to the `wfi32_iot_wm-1.json` file that is located in the `device_model` folder (should be located in the main demo folder) and then click on `Upload`

    <img src=".//media/image77.png">

11.	Note all the capabilities that were imported from the JSON file and then click on the `Publish` icon at the top of the page.  A pop-up window should appear to confirm the details for the new template to be added - then click on the `Publish` button

12.	Using the navigation pane on the left-hand side of the application, select `Connect` > `Device templates` and confirm that the new device template has been created/published

    <img src=".//media/image78.png">

13.	Look up the `ID Scope` for your IoT Central application (navigate to your application's web page and using the left-hand navigation pane, select `Permissions` > `Device connection groups`)

    <img src=".//media/image84a.png" style="width:5.in;height:3.18982in" alt="A screenshot of a cell phone Description automatically generated" />

14. Set the ID Scope value in the WFI32-IoT Board by editing the `CLOUD.CFG` file which is accessed via the `WFI32-IOT` disk drive. The unique ID Scope value will be used by the WFI32-IoT Board to connect to the built-in DPS used by your IoT Central application. Using the text editor of your choice, modify the existing line in the file so that the ID_SCOPE string is set to the value assigned to your IoT Central application, for example
    ```bash
    "ID_SCOPE":	"0ne0041E397",
    ```

15. Save the changes to the `CLOUD.CFG` file and then re-open the file to confirm that your changes were correctly saved for the `ID_SCOPE` value

16. Go back to your web browser to access the Azure IoT Central application.  Use the left-hand side pane and select `Connect` > `Devices`. Click `+ New` at the top to add a new device to your application (a one-time individual enrollment)

    <img src=".//media/image86.png">

17. In the `Create a new device` window, it is suggested to accept the default device name and ID that are randomly-generated (but can be changed to more meaningful names if you like). Select `WFI32_IoT_WM;1` for the device template. Click on the `Create` button

    <img src=".//media/image87.png">

18. Confirm that the new device is listed as `Registered` and that `Simulated = No`

    <img src=".//media/image88.png">

19. Click on the `Device name` instance and then click on `Connect` at the top of the device's page

    <img src=".//media/image89.png" style="width:5.in;height:1.28982in" alt="A screenshot of a cell phone Description automatically generated" />

20. The `Device connection groups` pop-up window should appear. Confirm that `Authentication type` is set to `Shared access signature (SAS)`. Edit the `CLOUD.CFG` file one last time by updating the `REGISTRATION_ID` string with the Device ID and then `PRIMARY_KEY` string with the Primary key values that are shown on the Device connection groups pop-up window. Close the pop-up window when finished and make sure to save these latest changes to the `CLOUD.CFG` file
    ```bash
    "REGISTRATION_ID":	"7tb1plo0w6",
    "PRIMARY_KEY": "GRPke4QlR99LSUGoqUfHtN0uPzRh+Sroi0v2tCrD46w="
    ```
    <img src=".//media/image89a.png"/>

21. Hit the `RESET` button on the WFI32-IoT Board and use the serial terminal program to confirm that the demo application is going through its startup sequence. When the "Connected to Azure IoT Hub" message appears, both Blue (Wi-Fi) and Green (Cloud) LEDs should be constantly on. By default, the temperature and light sensor readings should be sent to your IoT Central application every 5 seconds. The Yellow LED (Data) should be toggling approximately once per second

    <img src=".//media/image89b.png"/>

    <img src=".//media/image89c.png"/>

22. Press each of the `SW1` & `SW2` user buttons on the WFI32-IoT Board at least a couple of times. The Red LED should blink on each user button press that is detected

    <img src=".//media/image89d.png"/>

23. Click on the `Raw data` tab and confirm that the button press telemetry messages were received (scroll the web page to the right to view the `Button Press Count` & `Button Push Event` columns)

    <img src=".//media/image90.png"/>

24. Click on the `Refresh` icon to display all messages received since the previous page refresh operation.  Confirm that periodic telemetry messages are being continuously received approximately every 5 seconds (which is the default interval value for the `telemetryInterval` property that dictates the telemetry reporting frequency)

    <img src=".//media/image91a.png" style="width:5.in;height:1.58982in" alt="A screenshot of a cell phone Description automatically generated" />

    <img src=".//media/image92a.png"/>

25. Increase the ambient light source shining on top of the board. Wait approximately 10-15 seconds.  Click on the `Refresh` icon to confirm that the light sensor value has increased

    <img src=".//media/image93a.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />

## Connect your Device to the Dashboard for Data Visualization

1. Navigate to the left-hand vertical toolbar and click on the `Dashboards` icon

    <img src=".//media/image100.png" style="width:5.in;height:0.98982in" alt="A screenshot of a cell phone Description automatically generated" />

2. Towards the top of the web page, click on the dashboard selector and change the view to `Microchip IoT Light and Temperature Sensors`

    <img src=".//media/image100a.png">

3. Towards the top of the web page, click on the `Edit` icon

    <img src=".//media/image101.png" style="width:5.in;height:0.38982in" alt="A screenshot of a cell phone Description automatically generated" />

4. For **all** of the existing tiles named `Light` or `Temperature`, click on the upper right-hand corner of the tile to select `Configure`

    <img src=".//media/image102a.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />
    <img src=".//media/image102b.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />

5. Select `Device Group` > `WFI32-IoT WM;1 - All devices` and then check the box for your specific device name for `Devices`

    <img src=".//media/image103a.png" style="width:5.in;height:2.08982in" alt="A screenshot of a cell phone Description automatically generated" />

6. Under the `Telemetry` category, click on `+ Capability` and select the parameter pertaining to the title of the tile (e.g. `Brightness from Light Sensor` for each of the `Light` tiles or `Ambient Temperature` for each of the `Temperature` tiles)

    <img src=".//media/image104a.png" style="width:5.in;height:0.89082in" alt="A screenshot of a cell phone Description automatically generated" />
    <img src=".//media/image104b.png" style="width:5.in;height:2.18982in" alt="A screenshot of a cell phone Description automatically generated" />
    <img src=".//media/image104c.png" style="width:5.in;height:1.18982in" alt="A screenshot of a cell phone Description automatically generated" />

7. Click on `Update` and repeat the process for the remainder of the existing tiles

    <img src=".//media/image105.png" style="width:5.in;height:0.48982in" alt="A screenshot of a cell phone Description automatically generated" />

8. After every tile has been configured to visualize your device's sensor data, click on the `Save` icon to save the latest changes to the dashboard

    <img src=".//media/image106.png" style="width:5.in;height:0.38982in" alt="A screenshot of a cell phone Description automatically generated" />

9. Confirm that the dashboard is being continuously updated with the expected telemetry data received from the device.  For example, adjust the ambient light source directed at the board and observe that the light sensor values are changing accordingly

    <img src=".//media/image107.png" style="width:5.in;height:2.58982in" alt="A screenshot of a cell phone Description automatically generated" />

10. To access your IoT Central application(s) in the future, go to [Azure IoT Central](https://apps.azureiotcentral.com) and click on `My apps`

    <img src=".//media/image108.png" style="width:5.in;height:1.98982in" alt="A screenshot of a cell phone Description automatically generated" />

## Expand the Dashboard with Additional Tiles

To create additional tiles for your IoT Central dashboard, refer to [Configure the IoT Central application dashboard](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-add-tiles-to-your-dashboard). The below screen captures show additional possibilities of dashboard components that can highlight the telemetry data and properties facilitated by the `Plug and Play` interface.  Note that multiple devices can be selected for each tile to allow groups of devices to be visualized within a single tile. 

<img src=".//media/image95.png" style="width:5.in;height:3.34982in" alt="A screenshot of a cell phone Description automatically generated" />

<img src=".//media/image96.png" style="width:5.in;height:4.4182in" alt="A screenshot of a cell phone Description automatically generated" />

<img src=".//media/image97.png" style="width:5.in;height:3.34982in" alt="A screenshot of a cell phone Description automatically generated" />

<img src=".//media/image98.png" style="width:5.in;height:3.34982in" alt="A screenshot of a cell phone Description automatically generated" />
