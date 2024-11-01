# Bluepill
## Feature
- FreeRTOS 10.0.1
- USB CDC
- SSD1306 OLED through SPI1


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

## SPI1 pin configuration : OLED 96" SSD1306
- Transmit only
- SCK  -> PA5 -> 4.5 M bauds
- MOSI -> PA7
- CS   -> PB8
- DC   -> PB7
- RES  -> PB6
- bits order : MSBFIST
- bits length: 8
- CPOL = 0 (idle low)
- CPHA = 0 (1 edge)

## USB
- CDC

### NOTE1: CAN Bus and USB can't work together (know problem on this MCU)
### NOTE2: USB_DEVICE/App/usb_device.c --> set pin USB to signal host for reset
### NOTE3: USB_DEVICE/App/usbd_cdc_if.c --> Set CDC_SET_LINE_CODING & CDC_GET_LINE_CODING on CDC_Control_FS