// GarageTech 2016
// Yuriy Kushnerev
// Node #1 Rechargable battery powered ESP8266 MQTT controller with DS1820
// Device measured temperture with DS1820 each 2 minuts and publish to MQTT then go to power save mode
// Ready for long term operations on battery power
// Shematic requirments:
// ESP8266 need connect GPIO16 to RESET.
// Tantalium Capacitor 1000 mF between VCC and GND.
// Linear voltage regulator to use with full charged LiPo 4.2 V
// Optional: remove red led from ESP8266 module if presist to save power

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// EDIT THESE LINES TO MATCH YOUR SETUP
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// WiFi networks
const char* ssidWiFi = "";
const char* passWiFi = "";
WiFiClient wifiClient;

// Sensors
#define ONE_WIRE_BUS 2

// MQTT Server
#define MQTT_SERVER "192.168.1.4"
const unsigned int mqttPort = 1883;
const char* mqttLogin = "test";
const char* mqttPass = "test";

// Topic to publish the temperature
char* tempTopic = "/ha/tempDS1820_1";
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
PubSubClient mqttClient(MQTT_SERVER, mqttPort, wifiClient);

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass oneWire reference to Dallas Temperature
DallasTemperature ds1820(&oneWire);

void setup() {
	// Serial.begin(115200);
	// Serial.println("\r\nGo!");

	// Start WiFi subsystem
	WiFi.begin(ssidWiFi_1, passWiFi_1);
	if (WiFi.status() != WL_CONNECTED) {
		// Loop while we wait for connection
		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
		}
	}

	// Connect to the MQTT server
	mqtt_connect();

	// Start the temperature sensor
	ds1820.begin();
	myDS1820();

	// Serial.println("Prepare to sleep");
	ESP.deepSleep(120000000,WAKE_RF_DEFAULT);
	delay(1000);
}

void loop() {
	delay(10);
}

void myDS1820() {
	// Send the command to update temperatures
	ds1820.requestTemperatures();

	// Get the new temperature
	float currentTempFloat = ds1820.getTempCByIndex(0);

	// Convert the temp float to a string and publish to the temp topic
	char temperature[10];
	dtostrf(currentTempFloat,4,1,temperature);
	mqttClient.publish(tempTopic, temperature);
	// Serial.print("Temperature: ");
	// Serial.println(temperature);
}

// Networking functions
void mqtt_connect() {
	// Make sure we are connected to WiFi before attemping to connect MQTT
	if (WiFi.status() == WL_CONNECTED) {
	    // Serial.println("WiFi OK, try connect to MQTT");
		// Loop until we're connect to the MQTT server
		while (!mqttClient.connected()) {
			
			// Generate client name based on MAC address and last 8 bits of microsecond counter
			String clientName;
			clientName += "esp8266-";
			uint8_t mac[6];
			WiFi.macAddress(mac);
			clientName += macToStr(mac);

			mqttClient.connect((char*) clientName.c_str(), mqttLogin, mqttPass);
		}
	// Serial.println("MQTT connected!");
	}
}

// Generate unique name from MAC address
String macToStr(const uint8_t* mac) {
	String result;
	for (int i = 0; i < 6; ++i) {
		result += String(mac[i], 16);
		if (i < 5) {
			result += ':';
		}
	}
	return result;
}