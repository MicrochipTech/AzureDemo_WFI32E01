# Adding Extra Sensors to the WFI32-IoT Development Board

The WFI32-IoT Development Board, like many Microchip development boards, features a 16-pin (2 rows x 8 pins) expansion socket which conforms to the [mikroBUS™ specification](https://download.mikroe.com/documents/standards/mikrobus/mikroBUS-standard.pdf). The mikroBUS™ standard defines mainboard sockets used for interfacing microcontrollers or microprocessors (mainboards) with integrated circuits and peripheral modules (add-on boards).

The standard specifies the physical layout of the mikroBUS™ pinout, the communication and power supply pins used, the positioning of the mikroBUS™ socket on the mainboard, and finally, the silkscreen marking conventions for both the sockets. The purpose of mikroBUS™ is to enable easy hardware expandability with a large number of standardized compact add-on boards, each one carrying a single sensor, transceiver, display, encoder, motor driver, connection port, or any other electronic module or integrated circuit. Created by [MikroElektronika](https://www.mikroe.com), mikroBUS™ is an open standard — anyone can implement mikroBUS™ in their hardware design.

<img src=".//media/image8.png" style="width:4in;height:5in"/>

MikroElektronika manufactures hundreds of ["Click" boards](https://www.mikroe.com/click) which conform to the mikroBUS™ standard. This demonstration supports the optional addition of up to 4 MikroElektronika Click boards which feature various sensors manufactured by [TE Connectivity](https://www.te.com/usa-en/home.html).

<br>

## "Altitude 2" Click Board

[Altitude 2 Click](https://www.mikroe.com/altitude-2-click) is a high-resolution barometric pressure sensor add-on Click board™. It carries the MS5607, a barometric pressure sensor IC with the stainless steel cap, produced by TE Connectivity. This sensor provides very accurate measurements of temperature and atmospheric pressure, which can be used to calculate the altitude with a very high resolution of 20cm per step. Besides that, the Besides that, the device also includes features such as the ultra-low noise delta-sigma 24bit ADC, low power consumption, fast conversion times, pre-programmed unique compensation values, and more. Low count of external components requirement, along with the simple interface which requires no extensive configuration programming, makes this sensor very attractive for building altitude or air pressure measuring applications.

<img src=".//media/Altitude2.png" style="width:2in;height:2.8in"/>

<br>

## "PHT" Click Board

[PHT Click](https://www.mikroe.com/pht-click) is a compact add-on board that contains a PHT combo sensor. This board features the MS8607, a digital combination sensor providing 3 environmental measurements all-in-one: pressure, humidity, and temperature from TE Connectivity. This sensor is based on leading MEMS technologies, provides factory-calibrated PHT data available over an I2C serial interface. The standout feature of the MS8607, alongside its very respectable low power consumption at as low as 0.78 µA, is also ultra-low power consumption and high PHT accuracy. This Click board™ is appropriate for environmental monitoring, as well as PHT applications such as HVAC and weather stations.

<img src=".//media/PHT.png" style="width:2in;height:2.8in"/>

<br>

## "TEMPHUM14" Click Board

[TEMPHUM14 Click](https://www.mikroe.com/temphum-14-click) is a compact add-on board that contains one of the smallest and most accurate humidity and temperature sensors on the market. This board features the HTU31D, a highly accurate digital relative humidity sensor with temperature output from TE Connectivity. With power consumption down to 3.78μW and accuracy of ±2%RH and ±0.2°C, this Click board™ provides fast response time, precision measurement, low hysteresis, and sustained performance even when exposed to extreme temperature up to 125°C and humidity environments. This Click board™ is suitable for relative humidity and temperature measuring applications, including weather stations, reliable monitoring systems, and more.

<img src=".//media/TempHum14.png" style="width:2in;height:2.8in"/>

<br>

## "Ultra-Low Press" Click Board

[Ultra-Low Press Click](https://www.mikroe.com/ultra-low-press-click) is a compact add-on board that contains a mountable gage pressure sensor for pneumatic pressure measurements. This board features the SM8436, an I2C configurable ultra-low pressure sensor with high accuracy and long-term stability from Silicon Microstructure (part of TE Connectivity). A state-of-the-art MEMS pressure transducer technology and CMOS mixed-signal processing technology produces a digital, fully conditioned, multi-order pressure and temperature compensated sensor like this available in a gage pressure configuration. It also features superior sensitivity needed for ultra-low pressure measurements ranging from 0 to 250Pa Differential / 500 Pa Gauge. Therefore, this Click board™ is suitable for differential pressure measurements found in pressure monitoring applications like building fire safety systems, isolation rooms, and high purity work stations as well as positive pressure solutions found in hospital surgical environments.

<img src=".//media/ULP.png" style="width:2in;height:3.5in"/>

<br>

## "VAV Press" Click Board

[VAV Press Click](https://www.mikroe.com/vav-press-click) is a compact add-on board that contains a board-mount pressure sensor. This board features the LMIS025B, a low differential pressure sensor from First Sensor (part of TE Connectivity). It is based on thermal flow measurement of gas through a micro-flow channel integrated within the sensor chip. The innovative LMI technology features superior sensitivity, especially for ultra-low pressures ranging from 0 to 25 Pa. The extremely low gas flow through the sensor ensures high immunity to dust contamination, humidity, and long tubing compared to other flow-based pressure sensors. This Click board™ is suitable for pressure measurements in Variable Air Volume (VAV) building ventilation systems, industrial, and respiratory applications in medical.

<img src=".//media/VAV.png" style="width:2in;height:3.5in"/>

<br>

## Stacking Click Boards

Any combination of up to 4 of these Click boards can be connected to the WFI32-IoT Development Board at the same time using the MikroElektronika [Shuttle Bundle](https://www.mikroe.com/mikrobus-shuttle-bundle) accessory kit. The bundle features the [Shuttle click](https://www.mikroe.com/shuttle-click) 4-socket expansion board, which provides an easy and elegant solution for stacking up to four Click boards™ onto a single mikroBUS™ socket. It is a perfect solution for expanding the capacity of the development system with additional mikroBUS™ sockets when there is a demand for using more Click boards™ than the used development system is able to support.

<img src=".//media/image10a.png">

<img src=".//media/image10b.png">
