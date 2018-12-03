/*
JOSE MONTALVO
 */

#include "SerialEmg.h"  

SerialEmg::SerialEmg() {  
}

/* Setup the registers on the ADS1298 */
void SerialEmg::setupADS() {
  ADS = ADS129X(ADS_DRDY, ADS_CS);

  Serial.printf("Reset pin: %d\n", ADS_RESET);
  Serial.printf("Data ready pin: %d\n", ADS_DRDY);
  Serial.printf("CS pin: %d\n", ADS_CS);

  pinMode(ADS_RESET, OUTPUT);

  digitalWrite(ADS_RESET, HIGH);
  delay(100); // delay for power-on-reset (Datasheet, pg. 48)
  // reset pulse
  digitalWrite(ADS_RESET, LOW);
  digitalWrite(ADS_RESET, HIGH);
  delay(1); // Wait for 18 tCLKs AKA 9 microseconds, we use 1 millisec

  ADS.SDATAC(); // device wakes up in RDATAC mode, so send stop signal
  ADS.WREG(ADS129X_REG_CONFIG1,(1<<ADS129X_BIT_CLK_EN) | ADS129X_SAMPLERATE_256); // enable 8kHz sample-rate (p.67)
  ADS.WREG(ADS129X_REG_CONFIG3, (1<<ADS129X_BIT_PD_REFBUF) | (1<<6) | (1<<ADS129X_BIT_RLD_MEAS) | (1<<ADS129X_BIT_RLDREF_INT) | (1<<ADS129X_BIT_PD_RLD)); // enable internal reference
  ADS.WREG(ADS129X_REG_CONFIG2, (1<<ADS129X_BIT_INT_TEST | ADS129X_TEST_FREQ_2HZ));

  for (int i = 1; i <= NUM_CHANNELS; i++) {
    ADS.configChannel(i, false, ADS129X_GAIN_12X, ADS129X_MUX_NORMAL);
  }

  delay(1);
  ADS.RDATAC();
  ADS.START();

  Serial.begin(0); // always at 12Mbit/s
}

/* Check if there is data available, if so copy it to data */
bool SerialEmg::hasData(long* data) {
  return ADS.getData(data);
}
