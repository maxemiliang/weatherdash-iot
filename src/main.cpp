/*
 *  This sketch demonstrates how to scan WiFi networks. 
 *  The API is almost the same as with the WiFi Shield library, 
 *  the most obvious difference being the different file you need to include:
 */
/*
    HTTP over TLS (HTTPS) example sketch
    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.
    Limitations:
      only RSA certificates
      no support of Perfect Forward Secrecy (PFS)
      TLSv1.2 is supported since version 2.4.0-rc1
    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN D4 // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);

#ifndef STASSID
#define STASSID "iot-net"
#define STAPSK "CHANGEME"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

const char *host = "HOSTNAME";
const int httpsPort = 443;
const int httpPort = 80;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char fingerprint[] PROGMEM = "9c 78 2d 96 9a 80 20 16 8e dc d9 6d 2b 72 a6 69 e8 c6 d9 e3";

unsigned long delayMS;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);

  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = 10000;
}

void printValues()
{

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }

  char temp[10];

  dtostrf(event.temperature, 8, 3, temp);

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }

  HTTPClient http;
  Serial.print("connecting to ");
  Serial.println(host);
  //http.setFingerprint(fingerprint);

  String url = "http://weatherdash-api.app.maxemiliang.cloud/sensor";
  Serial.print("requesting URL: ");
  Serial.println(url);

  http.begin(url);

  Serial.println("request sent");

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  char request[128] = "source_name=nodemcu_1_turku&token=SecretTokenHello&temperature=";

  char humText[30] = "&humidity=";
  char humidity[10];

  dtostrf(event.relative_humidity, 8, 3, humidity);

  strcat(request, temp);
  strcat(humText, humidity);
  strcat(request, humText);

  Serial.println(request);

  int httpCode = http.POST(request);

  String payload = http.getString(); //Get the response payload

  Serial.println(httpCode); //Print HTTP return code
  Serial.println(payload);  //Print request response payload

  http.end();

  delay(delayMS);
  printValues();
}

void loop()
{
  printValues();
}
