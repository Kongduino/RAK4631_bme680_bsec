/**
   @file bme680_bsec.ino
   @author rakwireless.com
   @brief Setup and read values from a BME680 environment sensor using the BOSCH BSEC library
   @version 0.1
   @date 2020-07-28
   @copyright Copyright (c) 2020
   @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   IO1 <-> P0.17 (Arduino GPIO number 17)
   IO2 <-> P1.02 (Arduino GPIO number 34)
   IO3 <-> P0.21 (Arduino GPIO number 21)
   IO4 <-> P0.04 (Arduino GPIO number 4)
   IO5 <-> P0.09 (Arduino GPIO number 9)
   IO6 <-> P0.10 (Arduino GPIO number 10)
   SW1 <-> P0.01 (Arduino GPIO number 1)
*/
#include "bsec.h" // http://librarymanager/All#BSEC_BME680

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output, header;
uint8_t count = 0;

// Entry point for the example
void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Wire.begin();
  iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();
  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE, BSEC_OUTPUT_RAW_PRESSURE, BSEC_OUTPUT_RAW_HUMIDITY, BSEC_OUTPUT_RAW_GAS, BSEC_OUTPUT_IAQ, BSEC_OUTPUT_STATIC_IAQ, BSEC_OUTPUT_CO2_EQUIVALENT, BSEC_OUTPUT_BREATH_VOC_EQUIVALENT, BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE, BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();
  // Print the header
  header = "Timestamp | raw temp [°C] | pressure [hPa] | raw humidity [%] |  gas [Ohm]  |    IAQ     | IAQ accuracy | temp [°C] | humidity [%] | Static IAQ | CO2 equivalent | breath VOC equivalent";
  Serial.println(header);
}

// Function that is looped forever
void loop(void) {
  unsigned long time_trigger = millis();
  if (iaqSensor.run()) {
    // If new data is available
    printf("%-10u | ", time_trigger);
    printf("%-12f | ", iaqSensor.rawTemperature);
    printf("%-14f | ", iaqSensor.pressure / 100.0);
    printf("%-16f | ", iaqSensor.rawHumidity);
    printf("%-11f | ", iaqSensor.gasResistance);
    printf("%-10f | ", iaqSensor.iaq);
    printf("%-12f | ", iaqSensor.iaqAccuracy);
    printf("%-9f | ", iaqSensor.temperature);
    printf("%-12f | ", iaqSensor.humidity);
    printf("%-10f | ", iaqSensor.staticIaq);
    printf("%-14f | ", iaqSensor.co2Equivalent);
    printf("%-21f\n", iaqSensor.breathVocEquivalent);
    count++;
    if (count == 20) {
      Serial.println(header);
      count = 0;
    }
  } else {
    checkIaqSensorStatus();
  }
}

// Helper function definitions
void checkIaqSensorStatus(void) {
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }
  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
