/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define ssid "free"             // WiFi SSID
#define password "12345678"
#define DHTTYPE DHT11           // DHT type (DHT11, DHT22)
#define DHTPIN D6               // Broche du DHT / DHT Pin
#define LEDPIN D0               // Led
#define COPIN A0
#define FIREPIN D5
float t = 0;
float h = 0;
double p = 0;

ESP8266WiFiMulti WiFiMulti;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    // Serial.setDebugOutput(true);
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, 1);
    for (uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}
void led()
{
    digitalWrite(D0, backValue(FIREPIN));
}
int backValue(uint8_t pinName)
{
    if (digitalRead(pinName)) {
        Serial.println("000");
        return 0;
    } else {
        Serial.println("111");
        return 1;
    }
}

void loop()
{
    t = dht.readTemperature();
    h = dht.readHumidity();
    p = 10 + analogRead(COPIN) * 990 / 1024;
    // wait for WiFi connection
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        WiFiClient client;

        HTTPClient http;
        String urlNmae = "http://172.96.234.105:8000/add/?a=";
        urlNmae += t;
        urlNmae += "&b=";
        urlNmae += h;
        urlNmae += "&c=";
        urlNmae += p;


        if (http.begin(client, urlNmae)) { // HTTP
            int httpCode = http.GET();

            if (httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTP] GET... code: %d\n", httpCode);

                // file found at server
                if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                    String payload = http.getString();
                    Serial.println(payload);
                    if (payload == "0") {
                        Serial.println("this is a bug");
                        if (digitalRead(FIREPIN) == 0)
                            digitalWrite(LEDPIN, 1);
                        else
                            digitalWrite(LEDPIN, 0);
                    } else {
                        digitalWrite(LEDPIN, 1);
                    }
                }
            } else {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }

            http.end();
        } else {
            Serial.printf("[HTTP} Unable to connect\n");
        }
    }
    delay(10000);
}
