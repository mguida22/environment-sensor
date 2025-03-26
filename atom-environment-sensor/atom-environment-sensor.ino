#include <Wire.h>
#include <Adafruit_BME680.h>
#include <M5Atom.h>

// Create BME680 sensor object
Adafruit_BME680 bme;

// Configuration variables
const int READING_INTERVAL_MS = 1000;  // Default 1 second
const int SEA_LEVEL_PRESSURE_HPA = 1013.25; // Standard sea level pressure for altitude calculation

// tracks the digit we're currently displaying
// 0: nothing
// 1: hundreds
// 2: tens
// 3: ones
int currDigitPlace = 0;

const uint8_t BLANK_DIGIT_PATTERN[5][4] = {
  // _
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {1,0,0,1},
  {1,1,1,1}
};

// LED grid definitions for numerical display
const uint8_t DIGIT_PATTERNS[10][5][4] = {
  // 0
  {{1,1,1,1},
   {1,0,0,1},
   {1,0,0,1},
   {1,0,0,1},
   {1,1,1,1}},

  // 1
  {{0,0,0,1},
   {0,0,0,1},
   {0,0,0,1},
   {0,0,0,1},
   {0,0,0,1}},

  // 2
  {{1,1,1,1},
   {0,0,0,1},
   {1,1,1,1},
   {1,0,0,0},
   {1,1,1,1}},

  // 3
  {{1,1,1,1},
   {0,0,0,1},
   {0,1,1,1},
   {0,0,0,1},
   {1,1,1,1}},

  // 4
  {{1,0,0,1},
   {1,0,0,1},
   {1,1,1,1},
   {0,0,0,1},
   {0,0,0,1}},

  // 5
  {{1,1,1,1},
   {1,0,0,0},
   {1,1,1,1},
   {0,0,0,1},
   {1,1,1,1}},

  // 6
  {{1,1,1,1},
   {1,0,0,0},
   {1,1,1,1},
   {1,0,0,1},
   {1,1,1,1}},

  // 7
  {{1,1,1,1},
   {0,0,0,1},
   {0,0,0,1},
   {0,0,0,1},
   {0,0,0,1}},

  // 8
  {{1,1,1,1},
   {1,0,0,1},
   {1,1,1,1},
   {1,0,0,1},
   {1,1,1,1}},

  // 9
  {{1,1,1,1},
   {1,0,0,1},
   {1,1,1,1},
   {0,0,0,1},
   {1,1,1,1}}
};

void displayNumber(int number) {
  // make sure we only have 1 digit
  int digit = number % 10;

  for (int row = 0; row < 5; row++) {
    for(int col = 0; col < 4; col++) {
      if(DIGIT_PATTERNS[digit][row][col]) {
        M5.dis.drawpix(row * 5 + col, CRGB::White);
      }
    }
  }
}

void clearDisplay() {
  for(int row = 0; row < 5; row++) {
    for(int col = 0; col < 4; col++) {
      M5.dis.drawpix(row * 5 + col, CRGB::Black);
    }
  }
}

void displayBlank() {
  for (int row = 0; row < 5; row++) {
    for(int col = 0; col < 4; col++) {
      if(BLANK_DIGIT_PATTERN[row][col]) {
        M5.dis.drawpix(row * 5 + col, CRGB::White);
      }
    }
  }
}

void displayFahrenheit(float temperature, int place) {
  // Convert to Fahrenheit
  int tempF = round((bme.temperature * 9.0/5.0) + 32.0);

  // get each component
  int hundreds = tempF / 100;
  int tens = (tempF / 10) % 10;
  int ones = tempF % 10;

  // we need to always reset the display in between renders
  clearDisplay();
  switch(place) {
    case 0:
      break;
    case 1:
      if (hundreds > 0) {
        displayNumber(hundreds);
      } else {
        displayBlank();
      }
      break;
    case 2:
      displayNumber(tens);
      break;
    case 3:
      displayNumber(ones);
      break;
  }
}

// Color mapping functions
CRGB mapTemperatureToColor(float temp) {
  // Blue (cold) to Red (hot)
  if (temp < 10) return CRGB::LightBlue;
  if (temp < 20) return CRGB::Cyan;
  if (temp < 30) return CRGB::Yellow;
  return CRGB::Red;
}

CRGB mapHumidityToColor(float humidity) {
  // Green (low humidity) to Blue (high humidity)
  if (humidity < 30) return CRGB::Green;
  if (humidity < 60) return CRGB::SeaGreen;
  return CRGB::Blue;
}

CRGB mapPressureToColor(float pressure) {
  // Purple (low pressure) to Green (normal) to Red (high pressure)
  if (pressure < 1000) return CRGB::Purple;
  if (pressure < 1013) return CRGB::Green;
  return CRGB::Red;
}

void setup() {
  // Initialize M5Atom
  M5.begin(true, false, true);
  M5.dis.clear();

  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize I2C communication
  Wire.begin(25, 21);  // SDA, SCL pins for M5Stack Atom Matrix
  
  // Initialize BME688 sensor
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME688 sensor, check wiring!");
    M5.dis.clear();
    M5.dis.drawpix(4, CRGB::Red);  // Error indicator
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
    M5.dis.clear();
    M5.dis.drawpix(4, CRGB::Red);  // Error indicator
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
  
  // Update LED based on different sensor readings
  CRGB temperatureColor = mapTemperatureToColor(bme.temperature);
  CRGB humidityColor = mapHumidityToColor(bme.humidity);
  CRGB pressureColor = mapPressureToColor(bme.pressure / 100.0);

  M5.dis.drawpix(14, temperatureColor);
  M5.dis.drawpix(19, humidityColor);
  M5.dis.drawpix(24, pressureColor);

  // Cycle through different digits (0-4);
  displayFahrenheit(bme.temperature, currDigitPlace);
  // Cycle digit
  currDigitPlace = (currDigitPlace + 1) % 4;

  // Wait for next reading
  delay(READING_INTERVAL_MS);
}