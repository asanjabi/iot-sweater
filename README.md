### This project is still work in progress, **code and instructions are not complete**
# IoT ugly-sweater

*Inspired by: Jim Bennett's [iot ugly sweater project](https://github.com/jimbobbennett/IoTUglySweater ) which is a python implementation of this.*
  
There are two parts for this project:
1. Setup and configure [Azure IoT Central](https://azure.microsoft.com/en-us/services/iot-central/) 
by following the instructions in the [iot-central folder](iot-central/README.md)

2. Configure your device to connect to your instance of Azure IoT Central.  There are two options for this:
    1. You can use an [ESP-32](http://esp32.net/) development board and C# using [.NET Nano Framework](https://www.nanoframework.net/) 
    for instructions on how to do this refer to documentation in the [iot-sweater-nf folder](iot-sweater-nf/README.md)
    1. You can also use [MXCHIP AZ3166 IOT Developer Kit](https://www.seeedstudio.com/AZ3166-IOT-Developer-Kit.html) using *C* 
    for instructions on how to do this refer to documentation in the [iot-sweater-mxchip](iot-sweater-mxchip/README.md) 
    for more information about the iot devkit you can look at the [MXCHIP AZ3166 devkit Quick Start](https://aka.ms/iot-devkit)



