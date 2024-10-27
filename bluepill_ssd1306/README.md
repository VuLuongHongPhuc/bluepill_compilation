# Bluepill
Model for future project:
- FreeRTOS 10.0.1
- USB CDC
- SSD1306 OLED through SPI1
- MCP2515 CAN through SPI2

IDE Tools version:
  - STM32CubeIDE v1.13.2
  
# Flasher pin
- SWO --> PB3 ---> pin.39
- NRST -> R -----> pin.7
- DIO --> SWDIO -> pin.34
- CLK --> SWCLK -> pin.37


# LED_BUILTIN pin configuration
- PC13 ouput

# SPI1 pin configuration : OLED 96" SSD1306
- SCK  -> PA5
- MOSI -> PA7
- CS   -> PB8
- DC   -> PB7
- RES  -> PB6
- bits order : MSBFIST
- bits length: 8
- CPOL = 0 (idle low)
- CPHA = 0 (1 edge)

# USB
- CDC

# NOTE1: CAN Bus and USB can't work together (know problem on this MCU)
# NOTE2: USB_DEVICE/App/usb_device.c --> set pin USB to signal host for reset
# NOTE3: USB_DEVICE/App/usbd_cdc_if.c --> Set CDC_SET_LINE_CODING & CDC_GET_LINE_CODING on CDC_Control_FS