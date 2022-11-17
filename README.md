# Provisioning the Microchip WFI32-IoT Development Board (Part No. EV36W50A) for Azure IoT Central

## Introduction

 This document describes how to connect the WFI32-IoT Development Board (featuring the fully certified, highly integrated WFI32E01PC wireless module) to Azure IoT Central which leverages Microsoft’s Azure RTOS to enable better experiences of embedded firmware development for Cloud applications.

<img src=".//media/image1.png" />

## Table of Contents

- [Introduction](#introduction)
- [Background Knowledge](#background-knowledge)
  - [WFI32-IoT Development Board Overview & Features](#wfi32-iot-development-board-overview--features-smart--connected--secure)
  - [Microchip “Provisioning” vs. Microsoft “Provisioning”](#microchip-provisioning-vs-microsoft-provisioning)
  - [TLS Connection](#tls-connection)
  - [MQTT Connection](#mqtt-connection)
- [Create an Azure Account and Subscription](#create-an-azure-account-and-subscription)
- [Adding Extra Sensors to the WFI32 IoT Board](#adding-extra-sensors-to-the-wfi32-iot-board)
- [Program the WFI32-IoT Development Board](#program-the-wfi32-iot-development-board)
  - [1. Install the Development Tools](#1-install-the-development-tools)
  - [2. Connect to Azure IoT Central](#2-connect-to-azure-iot-central)
- [References](#references)
- [Conclusion](#conclusion)

## Background Knowledge

### WFI32-IoT Development Board Overview & Features (SMART \| CONNECTED \| SECURE)

<img src=".//media/image2.png"/>

 Download the [WFI32-IoT Development Board User Guide](https://www.microchip.com/content/dam/mchp/documents/WSG/ProductDocuments/UserGuides/EV36W50A-WFI32-IoT-Board-Users-Guide-DS50003262.pdf) for more details including the schematics for the board (but do not follow the setup procedure in the document)

### Microchip “Provisioning” vs. Microsoft “Provisioning”

The term “provisioning” is used throughout this document (e.g. provisioning key, provisioning device, Device Provisioning Service). On the Microchip side, the provisioning process is to securely inject certificates into the hardware. From the context of Microsoft, provisioning is defined as the relationship between the hardware and the Cloud (Azure). [Azure IoT Hub Device Provisioning Service (DPS)](https://docs.microsoft.com/azure/iot-dps/#:~:text=The%20IoT%20Hub%20Device%20Provisioning%20Service%20%28DPS%29%20is,of%20devices%20in%20a%20secure%20and%20scalable%20manner.)
allows the hardware to be provisioned securely to the right IoT Hub.

### High Level Architecture between the Client (Microchip WFI32-IoT) and the Cloud (Microsoft Azure)

This high-level architecture description summarizes the interactions between the WFI32-IoT Development Board and Azure. These are the major puzzle pieces that make up this enablement work of connecting WFI32-IoT Development Board to Azure through DPS using the most secure authentication:

- [Trust&GO™ Platform](https://www.microchip.com/en-us/products/security/trust-platform/trust-and-go): Microchip-provided implementation for secure authentication.  Each Trust&GO secure element comes with a pre-established locked configuration for thumbprint authentication and keys, streamlining the process of enabling network authentication using the [ATECC608B](https://www.microchip.com/en-us/product/ATECC608B) secure elements. 

- [Azure RTOS](https://azure.microsoft.com/en-us/services/rtos/): Microsoft-provided API designed to allow small, low-cost embedded IoT devices to communicate with Azure services, serving as translation logic between the application code and transport client

- [Azure IoT Central](https://docs.microsoft.com/en-us/azure/iot-central/core/overview-iot-central): IoT Central is an IoT application platform that reduces the burden and cost of developing, managing, and maintaining enterprise-grade IoT solutions

- [Azure IoT Hub](https://docs.microsoft.com/en-us/azure/iot-hub/about-iot-hub): IoT Hub is a managed service, hosted in the Cloud, that acts as a central message hub for bi-directional communication between your IoT application and the devices it manages

- [Device Provisioning Service (DPS)](https://docs.microsoft.com/en-us/azure/iot-dps/): a helper service for IoT Hub that enables zero-touch, just-in-time provisioning to the right IoT Hub without requiring human intervention, allowing customers to automatically provision millions of devices in a secure and scalable manner

On successful authentication, the WFI32-IoT Development Board will be provisioned to the correct IoT Hub that is pre-linked to DPS during the setup process. We can then leverage Azure IoT Central's application platform services (easy-to-use, highly intuitive web-based graphical tools used for interacting with and testing your IoT devices at scale).

### TLS connection

The TLS connection performs both authentication and encryption.
Authentication consists of two parts:

- Authentication of the server (the device authenticates the server)
- Authentication of the client (the server authenticates the device)

Server authentication happens transparently to the user since the WFI32E01PC certified module (integrating Microchip's Trust&GO secure element) on the WFI32-IoT  board comes preloaded with the required CA certificate. During client authentication the client private key must be used, but since this is stored inside the secure element and cannot be extracted, all calculations must be done inside the secure element. The main application will in turn call the secure element's library API’s to perform the calculations. Before the TLS connection is complete, a shared secret key must be negotiated between the server and the client. This key is used to encrypt all future communications during the connection.

### MQTT Connection

After successfully connecting on the TLS level, the board starts establishing the MQTT connection. Since the TLS handles authentication and security, MQTT does not require a username nor password.

## Create an Azure Account and Subscription

Before connecting to Azure, you must first create a user account with a valid subscription. The Azure free account includes free access to popular Azure products for 12 months, $200 USD credit to spend for the first 30 days, and access to more than 25 products that are always free. This is an excellent way for new users to get started and explore.

To sign up, you need to have a phone number, a credit card, and a Microsoft or GitHub account. Credit card information is used for identity verification only. You won't be charged for any services unless you upgrade. Starting is free, plus you get $200 USD credit to spend during the first 30 days and free amounts of services. At the end of your first 30 days or after you spend your $200 USD credit (whichever comes first), you'll only pay for what you use beyond the free monthly amounts of services. To keep getting free services after 30 days, you can move to [pay-as-you-go](https://azure.microsoft.com/en-us/offers/ms-azr-0003p/) pricing. If you don't move to the **pay-as-you-go** plan, you can't purchase Azure services beyond your $200 USD credit — and eventually your account and services will be disabled. For additional details regarding the free account, check out the [Azure free account FAQs](https://azure.microsoft.com/en-us/free/free-account-faq/).

When you sign up, an Azure subscription is created by default. An Azure subscription is a logical container used to provision resources in Azure. It holds the details of all your resources like virtual machines (VMs), databases, and more. When you create an Azure resource like a VM, you identify the subscription it belongs to. As you use the VM, the usage of the VM is aggregated and billed monthly.  You can create multiple subscriptions for different purposes.

Sign up for a free Azure account for evaluation purposes by following the process outlined in the [Microsoft Azure online tutorial](https://docs.microsoft.com/en-us/learn/modules/create-an-azure-account/). It is highly recommended to go through the entire section of the tutorial so that you fully understand what billing and support plans are available and how they all work.

Should you encounter any issues with your account or subscription, [submit a technical support ticket](https://azure.microsoft.com/en-us/support/options/).

## Adding Extra Sensors to the WFI32-IoT Development Board

Even though the WFI32-IoT Development Board has its own on-board light and temperature sensors, additional sensors can optionally be [added](./Clicks.md) relatively quickly using existing off-the-shelf hardware.

## Program the WFI32-IoT Development Board

### 1. Install the Development Tools

Embedded software development tools from Microchip need to be pre-installed in order to properly program the WFI32-IoT Development Board and provision it for use with Microsoft Azure IoT services.

Click this link for the setup procedure (when completed, return to this page): [Development Tools Installation](./Dev_Tools_Install.md)

### 2. Connect to Azure IoT Central

Azure IoT technologies and services provide you with options to create a wide variety of IoT solutions that enable digital transformation for your organization. Use [Azure IoT Central](https://docs.microsoft.com/en-us/azure/iot-central/core/overview-iot-central), a managed IoT application platform, to build and deploy a secure, enterprise-grade IoT solution. IoT Central features a collection of industry-specific application templates, such as retail and healthcare, to accelerate your solution development processes.

[Azure IoT Central](https://docs.microsoft.com/en-us/azure/iot-central/core/overview-iot-central) is an IoT application platform that reduces the burden and cost of developing, managing, and maintaining enterprise-grade IoT solutions. Choosing to build with IoT Central gives you the opportunity to focus time, money, and energy on transforming your business with IoT data, rather than just maintaining and updating a complex and continually evolving IoT infrastructure.

The web UI lets you quickly connect devices, monitor device conditions, create rules, and manage millions of devices and their data throughout their life cycle. Furthermore, it enables you to act on device insights by extending IoT intelligence into line-of-business applications.

This demonstration platform provides 2 different ways of programming the WFI32-IoT Development Board to authenticate itself with the Microsoft Azure Cloud service. It is strongly recommended to use X.509 certificate-based authentication to take full advantage of the [Trust&GO™](https://www.microchip.com/en-us/products/security/trust-platform/trust-and-go) secure element integrated into the WFI32 module.
1. [X.509 CA-Signed Certificates](./WFI32_IoT_Central_X509.md)
2. [Shared Access Signature (SAS Token)](./WFI32_IoT_Central_SAS.md)

## References

Refer to the following links for additional information for IoT Explorer, IoT Hub, DPS, Plug and Play model, and IoT Central

•	[Manage cloud device messaging with Azure-IoT-Explorer](https://github.com/Azure/azure-iot-explorer/releases)

•	[Import the Plug and Play model](https://docs.microsoft.com/en-us/azure/iot-pnp/concepts-model-repository)

•	[Configure to connect to IoT Hub](https://docs.microsoft.com/en-us/azure/iot-pnp/quickstart-connect-device-c)

•	[How to use IoT Explorer to interact with the device](https://docs.microsoft.com/en-us/azure/iot-pnp/howto-use-iot-explorer#install-azure-iot-explorer)

•	[Azure IoT Central - All Documentation](https://docs.microsoft.com/en-us/azure/iot-central/)

•	[Create an Azure IoT Central application](https://docs.microsoft.com/en-us/azure/iot-central/core/quick-deploy-iot-central)

•	[Manage devices in your Azure IoT Central application](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-manage-devices)

•	[How to connect devices with X.509 certificates for IoT Central](https://docs.microsoft.com/en-us/azure/iot-central/core/how-to-connect-devices-x509)

•	[Configure the IoT Central application dashboard](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-add-tiles-to-your-dashboard)

•	[Customize the IoT Central UI](https://docs.microsoft.com/en-us/azure/iot-central/core/howto-customize-ui)

## Conclusion

You are now able to connect WFI32-IoT to Azure IoT Central and should have deeper knowledge of how all the pieces of the puzzle fit together between Microchip's hardware and Microsoft's Azure Cloud services. Let’s start thinking out of the box and see how you can apply this project to provision securely and quickly a massive number of Microchip devices to Azure and safely manage them through the entire product life cycle.