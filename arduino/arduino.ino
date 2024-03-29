#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <WiFiNINA.h>
#include <PubSubClient.h>

#define REPORTING_PERIOD_MS 1000

PulseOximeter pox;
uint32_t tsLastReport = 0;
bool startSensing = false;

const char *ssid = "YourWiFiSSID";
const char *password = "YourWiFiPassword";
const char *mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char *topic = "health_monitor_system";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void onBeatDetected()
{
  Serial.println("Beat!");
}

void setup()
{
  Serial.begin(115200);
  Serial.print("Initializing pulse oximeter..");

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

  pox.setOnBeatDetectedCallback(onBeatDetected);

  pinMode(10, INPUT); // Setup for leads off detection LO +
  pinMode(11, INPUT); // Setup for leads off detection LO -

  setupWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnect();
}

void loop()
{
  pox.update();
  client.loop();

  if (digitalRead(10) == 1 || digitalRead(11) == 1)
  {
    Serial.println('!');
  }
  else
  {
    if (startSensing && millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
      int heartRate = pox.getHeartRate();
      int spo2 = pox.getSpO2();
      int ecgValue = analogRead(A0);

      publishSensorData(heartRate, spo2, ecgValue);

      tsLastReport = millis();
    }
  }

  delay(1000);
}

void setupWiFi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
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

void publishSensorData(int heartRate, int spo2, int ecgValue)
{
  if (client.connected())
  {
    char payload[100];
    snprintf(payload, sizeof(payload), "{\"heart_rate\": %d, \"spo2\": %d, \"ecg_value\": %d}", heartRate, spo2, ecgValue);
    client.publish(topic, payload);
  }
  else
  {
    Serial.println("MQTT not connected.");
    reconnect();
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
