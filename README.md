# STM32 CAN to USB

This is a small project for connecting a Nucleo STM32F103RB together with the SPI MCP2515 CAN module.

Pin configuration:

 * CAN_CS : D10 (PB6)
 * SPI_MOSI : D11 (PA7)
 * SPI_MISO : D12 (PA6)
 * SPI_SCK : D13 (PA5)
 
 Don't forget to set the baudrate inside the C code function `CANSPI_Initialize()`.
 
 ![a](https://raw.githubusercontent.com/DanielMartensson/STM32-CAN-to-USB/refs/heads/master/pinconfiguration.png)

Original source:
https://blog.naver.com/eziya76/221188525127

