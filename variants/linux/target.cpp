#include <Arduino.h>
#include "target.h"

class PortduinoHal : public ArduinoHal
{
public:
  PortduinoHal(SPIClass &spi, SPISettings spiSettings) : ArduinoHal(spi, spiSettings){};

  void spiTransfer(uint8_t *out, size_t len, uint8_t *in) {
    spi->transfer(out, in, len);
  }
};

LinuxBoard board;

SPISettings spiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0);
ArduinoHal *hal = new PortduinoHal(SPI, spiSettings);
RADIO_CLASS radio = new Module(hal, P_LORA_NSS, -1, -1, P_LORA_BUSY);
WRAPPER_CLASS radio_driver(radio, board);

LinuxRTCClock rtc_clock;
EnvironmentSensorManager sensors;

#ifdef DISPLAY_CLASS
  DISPLAY_CLASS display;
  MomentaryButton user_btn(PIN_USER_BTN, 1000, true);
#endif

bool radio_init() {
  rtc_clock.begin();

  int nss_pin = P_LORA_NSS;
  int busy_pin = P_LORA_BUSY;
  int irq_pin = RADIOLIB_NC;
  int reset_pin = RADIOLIB_NC;
  if (board.config.lora_nss_pin != UINT32_MAX) {
    nss_pin = (int)board.config.lora_nss_pin;
  }
  if (board.config.lora_busy_pin != UINT32_MAX) {
    busy_pin = (int)board.config.lora_busy_pin;
  }
  if (board.config.lora_irq_pin != UINT32_MAX) {
    irq_pin = (int)board.config.lora_irq_pin;
  }
  if (board.config.lora_reset_pin != UINT32_MAX) {
    reset_pin = (int)board.config.lora_reset_pin;
  }
  radio = new Module(hal, nss_pin, irq_pin, reset_pin, busy_pin);
  return radio.std_init(&SPI);
}

uint32_t radio_get_rng_seed() {
  return radio.random(0x7FFFFFFF);
}

void radio_set_params(float freq, float bw, uint8_t sf, uint8_t cr) {
  radio.setFrequency(freq);
  radio.setSpreadingFactor(sf);
  radio.setBandwidth(bw);
  radio.setCodingRate(cr);
}

void radio_set_tx_power(uint8_t dbm) {
  radio.setOutputPower(dbm);
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);  // create new random identity
}
