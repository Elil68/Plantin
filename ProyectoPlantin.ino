#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>  
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <DHT_U.h>

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "CONTRASEÑA"

DHT dht(5, DHT22);
const int ledPin = 13;

String temperatura, humedadAmbiente, humedadTierra, postData;

void checkWiFiConnection() 
{
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to Wi-Fi...");
    delay(1000);
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Communication Started\n");

  pinMode(ledPin, OUTPUT);

  //DHT
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  checkWiFiConnection();
}

void loop()
{
  checkWiFiConnection();
  
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient http;

  int tempValue = dht.readTemperature();
  int humidityValue = dht.readHumidity();
  int lectura = analogRead(A0);
  int soilHumidityValue = map(lectura, 1024, 0, 0, 100);

  temperatura = String(tempValue);
  humedadAmbiente = String(humidityValue);
  humedadTierra = String(soilHumidityValue);

  postData = "humedadTierra=" + humedadTierra + "&temperatura=" + temperatura + "&humedadAmbiente=" + humedadAmbiente;

  Serial.print("Sending data: ");
  Serial.println(postData);

  http.begin(client, "https://comunicaciones.simix.com.ar/v1/comunicaciones/public/sensores");
  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);

  if (httpCode == HTTP_CODE_OK) 
  {
    String response = http.getString();
    Serial.println("Server response: " + response);
    digitalWrite(ledPin, HIGH);
    delay(5000);
    digitalWrite(ledPin, LOW);
  } 
  else 
  {
    Serial.print("HTTP POST request failed with error code: ");
    Serial.println(httpCode);
  }

  http.end();
  delay(10000);
}