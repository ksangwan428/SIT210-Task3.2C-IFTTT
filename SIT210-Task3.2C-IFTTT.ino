#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

char ssid[] = "Kartik";
char pass[] = "12345678";

WiFiClient client;
BH1750 lightMeter;

char HOST_NAME[] = "maker.ifttt.com";     // IFTTT Maker service hostname
String PATH_NAME = "/trigger/LIGHTSENSOR/with/key/outsvBk4TNtGNruAgm0cj94YFb1ekaVVzNIjB33akD5";  // IFTTT endpoint
String queryString = "?value1=57&value2=25";  // Initial query string with default values

const int thresholdLux = 500;  // Adjust the threshold Lux value as needed

void setup() {
  Serial.begin(9600);   // Initialize serial communication with a baud rate of 9600
  while (!Serial);      // Wait for the serial connection to be established

  // Connect to Wi-Fi
  connectToWiFi();

  Wire.begin();          // Initialize the I2C communication
  lightMeter.begin();    // Initialize the BH1750 light sensor
}

void loop() {
  float lux = lightMeter.readLightLevel();  // Read the current light level in Lux
  Serial.print("Current Light Level: ");
  Serial.println(lux);

  if (lux > thresholdLux) {
    sendLuxToIFTTT(lux);  // Send Lux value to IFTTT if it's above the threshold
  }

  delay(3000);  // Delay for 3 seconds before taking another reading
}

void connectToWiFi() {
  WiFi.begin(ssid, pass);  // Attempt to connect to Wi-Fi using the provided credentials
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {  // Wait until connected
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP());  // Print the local IP address once connected
}

void sendLuxToIFTTT(float lux) {
  if (client.connect(HOST_NAME, 80)) {  // Attempt to connect to the IFTTT server
    Serial.println("Connected to IFTTT Server");
    queryString = "?value1=" + String(lux) + "&value2=25";  // Update the Lux value in the query string

    // Send an HTTP GET request to IFTTT
    client.print("GET " + PATH_NAME + queryString + " HTTP/1.1\r\n");
    client.print("Host: " + String(HOST_NAME) + "\r\n");
    client.print("Connection: close\r\n\r\n");

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);  // Print the response from the IFTTT server
      }
    }

    client.stop();  // Disconnect from the IFTTT server
    Serial.println("\nDisconnected from IFTTT Server");
  } else {
    Serial.println("Failed to connect to IFTTT Server");
  }
}


