 <p float="left">
  <img src="https://github.com/user-attachments/assets/9ba798cd-5b05-4b91-8be9-79a7a77e9f15" alt="robot with sensor 1" width="48%"/>
  <img src="https://github.com/user-attachments/assets/18f8f003-e607-4c0a-b3bf-f71097e73d23" alt="robot with sensor 2" width="48%"/>
</p>

# M5 Atom Matrix + BME688

Read environment data (temperature, humidity, pressure, altitude and gas resistance) using a M5 Atom Matrix wired up to a BME688 sensor.

An optional Rust server will log MCAP data files and stream it to [Foxglove](https://foxglove.dev/) using the [Foxglove Rust SDK](https://docs.foxglove.dev/docs/sdk/introduction).

<img width="1502" alt="foxglove streaming data over websocket" src="https://github.com/user-attachments/assets/45d171b6-0eb6-41b5-9c61-c47e051e076e" />

## Setup

### Wiring

Connect the sensor to your controller. I'm using a Seengreat BME688 sensor and wired the following pins together.

| Seengreat BME688 | M5 Atom       |
| ---------------- | ------------- |
| VCC              | 5V            |
| GND              | GND           |
| SLC/SCK          | G21           |
| SDA/MOSI         | G25           |
| MISO/ADDR        | not connected |
| CS               | not connected |

<img width="600" alt="m5 atom matrix wired to a seengreat bme688" src="https://github.com/user-attachments/assets/c4ed06b2-46bb-4fd5-9801-6673f98d9d4d" />


References:

- https://seengreat.com/wiki/162/bme680-environmental-sensor-bme688-environmental-sensor#toc14
- https://docs.m5stack.com/en/core/ATOM%20Matrix

### Controller

Follow https://docs.m5stack.com/en/arduino/arduino_ide to get setup with Arduino and the M5 Atom Matrix.

You'll likely need to install the Adafruit BME680 and M5Stack libraries, along with their dependencies.

## Running

Select your device in the Arduino IDE and upload the code. Open the Serial Monitor and set the baud rate to 115200. You should see sensor readings.

```
{"temperature":19.37,"humidity":38.15,"pressure":900.16,"altitude":985.17,"gas_resistance":135.63}
```

These have the following units:

- Temperature: °C
- Humidity: %
- Pressure: hPa
- Altitude: m
- Gas Resistance: kΩ

## ESP32 Logger

If enabled in the `atom-environment-sensor` Arduino code, data will be sent over UDP to the rust server.

This will stream data to [Foxglove](https://foxglove.dev/) via a websocket connection, and write logs to MCAP files which can also be viewed in Foxglove.

```
cd esp32_logger
cargo run
```
