#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define ssid "free"         // WiFi SSID
#define password "12345678" // WiFi password
/* #define DHTTYPE DHT22        // DHT type (DHT11, DHT22) */
/* #define DHTPIN D4            // Broche du DHT / DHT Pin */
#define LEDPIN D8   // Led
#define FIREPIND D2 // Led
#define FIREPINA D1 // Led
float t = 0;
float h = 0;
float s = 0;
String p = "out of danger";
String etatLed = "OFF";

// Création des objets / create Objects

ESP8266WebServer server(80);

String getPage() {
    String page =
        "<html lang=en-US><head><meta http-equiv='refresh' content='10'/>";
    page += "<title>ESP8266 </title>";
    page += "<style> body { background-color: #fffff; font-family: Arial, "
            "Helvetica, Sans-Serif; Color: #000088; }</style>";
    page += "</head><body><h1>ESP8266</h1>";
    page += "<h3>DHT11</h3>";
    page += "<ul><li>Temperature : ";
    page += t;
    page += "°C</li>";
    page += "<li>Humidite : ";
    page += h;
    page += "%</li></ul><h3>light</h3>";
    page += "<ul><li>Pression atmospherique : ";
    page += p;
    page += "</li></ul>";
    page += "%</li></ul><h3>light</h3>";
    page += "<ul><li>strength : ";
    page += analogRead(FIREPINA);
    page += "</li></ul>";
    page += "<h3>GPIO</h3>";
    page += "<form action='/' method='POST'>";
    page += "<ul><li>D3 (etat: ";
    page += etatLed;
    page += ")";
    page += "<INPUT type='radio' name='LED' value='1'>ON";
    page += "<INPUT type='radio' name='LED' value='0'>OFF</li></ul>";
    page += "<INPUT type='submit' value='Actualiser'>";
    page += "<br><br><p><a hrf='https://diyprojects.io'>diyprojects.io</p>";
    page += "</body></html>";
    return page;
}
void handleRoot() {
    if (server.hasArg("LED")) {
        handleSubmit();
    } else {
        server.send(200, "text/html", getPage());
    }
}

void handleSubmit() {
    // Actualise le GPIO / Update GPIO
    String LEDValue;
    LEDValue = server.arg("LED");
    Serial.println("Set GPIO ");
    Serial.print(LEDValue);
    if (LEDValue == "1") {
        etatLed = "On";
        server.send(200, "text/html", getPage());
    } else if (LEDValue == "0") {
        etatLed = "Off";
        server.send(200, "text/html", getPage());
    } else {
        Serial.println("Err Led Value");
    }
}

void setup() {
    Serial.begin(115200);
    // Initialisation du BMP180 / Init BMP180

    pinMode(LEDPIN, OUTPUT);
    pinMode(FIREPIND, INPUT);
    pinMode(FIREPINA, INPUT);
    WiFi.begin(ssid, password);
    // Attente de la connexion au réseau WiFi / Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Connexion WiFi établie / WiFi connexion is OK
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // On branche la fonction qui gère la premiere page / link to the function
    // that manage launch page
    server.on("/", handleRoot);

    server.begin();
    Serial.println("HTTP server started");
}
int backValue(uint8_t pinName) {
    if (digitalRead(pinName)) {
        Serial.println("000");
        return 0;
    } else {
        Serial.println("111");
        return 1;
    }
}
void led() {
    if (server.arg("LED") == "1") {
        digitalWrite(LEDPIN, 1);
    } else if (server.arg("LED") == "0") {
        digitalWrite(LEDPIN, backValue(FIREPIND));
    }
}
void loop() {
    if (backValue(FIREPIND)) {
        p = "under danger";
    } else {
        p = "out of danger";
    }
    s = analogRead(FIREPINA);
    led();
    server.handleClient();
    delay(100);
}
