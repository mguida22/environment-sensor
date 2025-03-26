#include <Wire.h>
#include <Adafruit_BME680.h>

// Create BME680 sensor object
Adafruit_BME680 bme;

// Configuration variables
const int READING_INTERVAL_MS = 1000;  // Default 1 second, can be easily changed
const int SEA_LEVEL_PRESSURE_HPA = 1013.25; // Standard sea level pressure for altitude calculation

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize I2C communication
  Wire.begin(25, 21);  // SDA, SCL pins for M5Stack Atom Matrix
  
  // Initialize BME688 sensor
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME688 sensor, check wiring!");
    while (1);
  }

  // Configure oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms
}

void loop() {
  // Perform sensor reading
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  // Print sensor readings
  Serial.println("--- BME688 Sensor Readings ---");
  Serial.print("Temperature: ");
  Serial.print(bme.temperature);
  Serial.println(" °C");
  
  Serial.print("Humidity: ");
  Serial.print(bme.humidity);
  Serial.println(" %");
  
  Serial.print("Pressure: ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");
  
  Serial.print("Altitude: ");
  Serial.print(bme.readAltitude(SEA_LEVEL_PRESSURE_HPA));
  Serial.println(" m");
  
  Serial.print("Gas Resistance: ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" kΩ");
  
  Serial.println("-----------------------------");
  
  // Wait for next reading
  delay(READING_INTERVAL_MS);
}
