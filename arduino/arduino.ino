#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <WiFiNINA.h>
#include <PubSubClient.h>

// Wi-Fi and MQTT settings
const char *ssid = "Sanchit";
const char *password = "12345678";
const char *mqttServer = "broker.hivemq.com"; // HiveMQ broker address
const int mqttPort = 1883;                    // HiveMQ broker port
const char *topic = "health_monitor_system";  // HiveMQ topic

// Initialize WiFi and MQTT client
WiFiClient wifiClient;
PubSubClient client(wifiClient);

#define REPORTING_PERIOD_MS 1000

PulseOximeter pox;
uint32_t tsLastReport = 0;
bool startSensing = true;

void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup()
{
  // Initialize the serial communication:
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Connect to MQTT server
  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("arduino-client"))
    {
      Serial.println("Connected to MQTT");
      client.subscribe("health_monitor_control");
    }
    else
    {
      Serial.println("MQTT connection failed, retrying in 5 seconds...");
      delay(5000);
    }
  }

  pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); // Setup for leads off detection LO -

  Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin())
  {
    Serial.println("FAILED");
    for (;;)
      ;
  }
  else
  {
    Serial.println("SUCCESS");
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
  // Make sure to call update as fast as possible
  pox.update();

  if (false)
  {
    Serial.println('!'); // Leads off detected
  }
  else
  {
    char payload[100];
    // Generate random values for testing
    int ecgValue = random(600, 900);  // Generate a random value between 600 and 1023
    int heartRate = random(60, 100); // Generate a random heart rate between 60 and 99 BPM
    int spo2 = random(90, 100);      // Generate a random SpO2 value between 90% and 99%

    if (true)
    {
      if (client.connected())
      {
        if (startSensing)
        {
          // Format the sensor data into the payload array
          snprintf(payload, sizeof(payload), "{\"ecg_value\": %d, \"heart_rate\": %d, \"spo2\": %d}", ecgValue, heartRate, spo2);
          tsLastReport = millis();

          // Publish the payload to the MQTT topic
          client.publish(topic, payload);
        }
      }
      else
      {
        reconnect();
      }
      client.loop();
    }
    // Wait for a bit to prevent serial data from saturating
    delay(250);
  }
}


void callback(char *topic, byte *payload, unsigned int length)
{
  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  if (message == "start_sensing")
  {
    startSensing = true;
    Serial.println("Sensing started.");
  }
  else if (message == "stop_sensing")
  {
    startSensing = false;
    Serial.println("Sensing stopped.");
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.println("Reconnecting to MQTT...");
    if (client.connect("arduino-client"))
    {
      Serial.println("Connected to MQTT");
      client.subscribe("health_monitor_control");
    }
    else
    {
      Serial.println("MQTT connection failed, retrying in 5 seconds...");
      delay(5000);
    }
  }
}