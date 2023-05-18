#ifndef LIBI2C_H
#define LIBI2C_H 1
#include "../include/libI2C.h"
#endif

static int fd;

int setupI2C(uint8_t i2cAddress)
{
  fd = wiringPiI2CSetup(i2cAddress);
  if (fd < 0)
  {
    perror("error in) wiringPiI2CSetup");
    return -1;
  }
  return 1;
}

uint16_t readRegister(uint8_t i2cFd, uint8_t reg)
{
  wiringPiI2CWrite(i2cFd, ADS1015_REG_POINTER_CONVERT);
  uint16_t reading = wiringPiI2CReadReg16(i2cFd, reg);
  reading = (reading >> 8) | (reading << 8); // yes, wiringPi did not assemble the bytes as we want
  return reading;
}
void writeRegister(uint8_t i2cFd, uint8_t reg, uint16_t value)
{
  wiringPiI2CWriteReg16(i2cFd, reg, (value >> 8) | (value << 8));
}
uint16_t readADC_SingleEnded(uint8_t channel, int gain)
{
  if (channel > 3)
  {
    return 0;
  }

  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_1600SPS |   // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range

  // Set single-ended input channel
  switch (channel)
  {
  case (0):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
    break;
  case (1):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
    break;
  case (2):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
    break;
  case (3):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
    break;
  }
  config |= gain;

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(fd, ADS1015_REG_POINTER_CONFIG, config);

  // Wait for the conversion to complete
  usleep(1000 * ADS1115_CONVERSIONDELAY);

  // Read the conversion results
  // Shift 12-bit results right 4 bits for the ADS1015
  return readRegister(fd, ADS1015_REG_POINTER_CONVERT);
}