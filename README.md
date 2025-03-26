# M5 Atom Matrix + BME688

Read environment data (temp, humidity, pressure, altitude and gas resistance) using a M5 Atom Matrix wired up to a BME688 sensor (I'm using a seengreat one).

## Setup

### Wiring

Connect the sensor to your controller. I wired the following pins together.

| Seengreat BME688 | M5 Atom       |
| ---------------- | ------------- |
| VCC              | 5V            |
| GND              | GND           |
| SLC/SCK          | G21           |
| SDA/MOSI         | G25           |
| MISO/ADDR        | not connected |
| CS               | not connected |

References:

- https://seengreat.com/wiki/162/bme680-environmental-sensor-bme688-environmental-sensor#toc14
- https://docs.m5stack.com/en/core/ATOM%20Matrix

### Controller

Follow https://docs.m5stack.com/en/arduino/arduino_ide to get setup with Arduino and the M5

## Running

Select your device in the Arduino IDE and upload the code. Open the Serial Monitor and set the baud rate to 115200. You should see sensor readings.
