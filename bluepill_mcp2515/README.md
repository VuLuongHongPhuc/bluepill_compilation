# Bluepill
## Features
- FreeRTOS 10.0.1
- USB CDC
- MCP2515 CAN through SPI2

IDE version:
  - STM32CubeIDE v1.13.2
  
## Flasher pin
| J-Link/stlink | Board          | Pin           |
| :------------ | :------------- | :------------ |
| SWO           | PB3            | 39            |
| NRST          | R              | 7             |
| DIO           | SWDIO          | 34            |
| CLK           | SWCLK          | 37            |


## LED_BUILTIN
- PC13 ouput

## SPI2 pin configuration : MCP2515 CAN module 8 Mhz
- Full-duplex master
- SCK  -> PB13 -> 9M bauds
- MOSI -> PB15
- MISO -> PB14
- CS   -> PB12
- CPOL = 0 (idle low)
- CPHA = 0 (1 edge)
- bit order  : MSBFIRST
- bits length: 8
- External interrupt -> PA8 (falling edge)

## USB
- CDC

### NOTE1: CAN Bus and USB can't work together (know problem on this MCU)
### NOTE2: USB_DEVICE/App/usb_device.c --> set pin USB to signal host for reset
### NOTE3: USB_DEVICE/App/usbd_cdc_if.c --> Set CDC_SET_LINE_CODING & CDC_GET_LINE_CODING on CDC_Control_FS