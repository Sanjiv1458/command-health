# Health Monitoring System with IoT Integration

This project implements a Health Monitoring System using IoT components for real-time data collection and transmission. It allows monitoring of vital health parameters such as heart rate, SpO2, and ECG values and enables remote access to the collected data.

## Features

- Real-time monitoring of heart rate, SpO2, and ECG values.
- Wireless transmission of health data via MQTT protocol.
- Remote control functionality for starting or stopping sensing.
- Integration with an MQTT broker for data transmission to remote servers.

## Hardware Components

- MAX30100 Pulse Oximeter
- Analog Sensors
- ESP8266

## Software and Protocols

- C/C++
- Arduino IDE
- MQTT
- WiFiNINA
- PubSubClient

## Usage

1. Connect the hardware components as per the provided schematic.
2. Upload the provided Arduino sketch to the ESP8266 using the Arduino IDE.
3. Ensure proper WiFi connectivity and MQTT broker configuration.
4. Monitor the health parameters remotely using the provided MQTT control messages.

## Contribution

Contributions to this project are welcome! Feel free to fork this repository, make improvements, and submit pull requests.

## License

This project is licensed under the [MIT License](LICENSE).
