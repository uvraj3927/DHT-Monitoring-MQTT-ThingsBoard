#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include "config.h"

// --- Configuration ---
#define DHTPIN 4          // D2 pin on NodeMCU corresponds to GPIO4
#define DHTTYPE DHT11     // Defining the sensor type

// Wi-Fi Credentials (loaded from include/config.h)
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// ThingsBoard Configuration
const char* tb_server = "eu.thingsboard.cloud";  // Change to "demo.thingsboard.io" or your local IP if applicable
const int tb_port = 1883;                     // Default unencrypted MQTT port
const char* tb_token = TB_ACCESS_TOKEN;

// --- Global Objects & Variables ---
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastSendTime = 0;
const long interval = 5000; // Send telemetry data every 5 seconds (5000 ms)

// --- Function Prototypes ---
void connectWiFi();
void connectMQTT();
void sendTelemetry();

// --- Main Setup ---
void setup() {
  Serial.begin(115200);
  delay(10);
  
  dht.begin();
  Serial.println("\n--- DHT11 to ThingsBoard MQTT Initializing ---");
  
  connectWiFi();
  mqttClient.setServer(tb_server, tb_port);
}

// --- Main Loop ---
void loop() {
  // 1. Maintain MQTT Connection
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop(); // Keeps the MQTT background processes alive

  // 2. Non-blocking Timer to read and transmit data
  unsigned long now = millis();
  if (now - lastSendTime >= interval) {
    lastSendTime = now;
    sendTelemetry();
  }
}

// --- Function Definitions ---

// Handles the Wi-Fi connection logic
void connectWiFi() {
  Serial.print("Connecting to Wi-Fi SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n[Wi-Fi] Connected successfully!");
  Serial.print("[Wi-Fi] Local IP Address: ");
  Serial.println(WiFi.localIP());
}

// Handles the MQTT connection/reconnection to ThingsBoard
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("[MQTT] Attempting connection to ThingsBoard Broker...");
    
    // ThingsBoard requires the Access Token as the Username, Password can be NULL
    if (mqttClient.connect("ESP8266_Device", tb_token, NULL)) {
      Serial.println("\n[MQTT] Connected successfully to ThingsBoard!");
    } else {
      Serial.print("\n[MQTT] Connection failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" -> Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Reads the DHT11 sensor data, packages it into JSON, and publishes it
void sendTelemetry() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Validate sensor readings
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("[Sensor] Warning: Failed to read from DHT sensor! Skipping transmission.");
    return;
  }

  // Construct the JSON payload string using the distinct data 'Keys'
  String payload = "{";
  payload += "\"temperature\":" + String(temperature) + ",";
  payload += "\"humidity\":" + String(humidity);
  payload += "}";

  Serial.print("[Data] Ready to send payload: ");
  Serial.println(payload);

  // Publish to ThingsBoard's required telemetry topic
  if (mqttClient.publish("v1/devices/me/telemetry", payload.c_str())) {
    Serial.println("[MQTT] Telemetry published successfully.");
  } else {
    Serial.println("[MQTT] Telemetry publication failed.");
  }
}