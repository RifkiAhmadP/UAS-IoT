#include <Arduino.h>
#include <Wire.h>
#include <DHTesp.h>
#include <ThingsBoard.h>
#include <WiFi.h>
#include <BH1750.h> // Include the BH1750 library

#define THINGSBOARD_SERVER  "demo.thingsboard.io"
#define WIFI_SSID "Cozy binus 217"
#define WIFI_PASSWORD "12345678"

#define DHT_PIN 19

#define BH1750_ADDRESS 0x23 // Address of the BH1750 sensor

// Put your token ID here
#define THINGSBOARD_ACCESS_TOKEN "nyeLtLSbsAvzp9ZpIUGH"

WiFiClient espClient;
ThingsBoard tb(espClient);
DHTesp dht;
BH1750 lightMeter(BH1750_ADDRESS); // Create an instance of the BH1750 sensor

void WifiConnect();
void onSendSensor();

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22);
  Wire.begin(21,22);
  lightMeter.begin(); // Initialize the BH1750 sensor

  WifiConnect();
  if (tb.connect(THINGSBOARD_SERVER, THINGSBOARD_ACCESS_TOKEN))
    Serial.println("Connected to ThingsBoard");
  else
    Serial.println("Error connecting to ThingsBoard");
  Serial.println("System ready.");
  digitalWrite(LED_BUILTIN, 0);
}

void loop() {
  if (millis() % 3000 == 0 && tb.connected())
    onSendSensor();
  tb.loop();
}

void onSendSensor() {
  digitalWrite(LED_BUILTIN, 1);
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  float light = lightMeter.readLightLevel(); // Read the light level from the BH1750 sensor

  if (dht.getStatus() == DHTesp::ERROR_NONE) {
    Serial.printf("Temperature: %.2f °C, Humidity: %.2f %%\n", temperature, humidity);
    Serial.printf("Light: %.2f lx\n", light);

    if (tb.connected()) {
      tb.sendTelemetryFloat("temperature", temperature);
      tb.sendTelemetryFloat("humidity", humidity);
      tb.sendTelemetryFloat("light", light);
    }
  }
}

void WifiConnect() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.print("System connected with IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
}
