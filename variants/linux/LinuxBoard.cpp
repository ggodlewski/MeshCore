#include "linux/gpio/LinuxGPIOPin.h"
#include "LinuxBoard.h"
//#include "PortduinoGPIO.h"

void LinuxBoard::begin() {
}

int initGPIOPin(int pinNum, const std::string gpioChipName, int line)
{
#ifdef PORTDUINO_LINUX_HARDWARE
  char gpio_name[32];
  snprintf(gpio_name, sizeof(gpio_name), "GPIO%d", pinNum);

  try {
    GPIOPin *csPin;
    csPin = new LinuxGPIOPin(pinNum, gpioChipName.c_str(), line, gpio_name);
    csPin->setSilent();
    gpioBind(csPin);
    return 0;
  } catch (...) {
    printf("Warning, cannot claim pin %d\n", pinNum);
    return 1;
  }
#else
  return 0;
#endif
}

void portduinoSetup() {
#if defined(LORA_SPIDEV)
  SPI.begin(LORA_SPIDEV);
#endif
  initGPIOPin(P_LORA_DIO_0, "gpiochip0", P_LORA_DIO_0);
  initGPIOPin(P_LORA_RESET, "gpiochip0", P_LORA_RESET);
}
