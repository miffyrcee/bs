#include <DHT.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define ssid "free"         // WiFi SSID
#define password "12345678" // WiFi password
#define DHTTYPE DHT11       // DHT type (DHT11, DHT22)
#define DHTPIN D4           // Broche du DHT / DHT Pin
#define LEDPIN D3           // Led
#define COPIN A0
#define FIREPIN D2
float t = 0;
float h = 0;
double p = 0;
String etatLed = "OFF";

// Création des objets / create Objects
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server(80);

String getPage() {
    String page =
        "<html lang=en-us><head><meta http-equiv='refresh' content='10'/>";
    page += "<title>LED</title>";
    page += "<style> body { background-color: #fffff; font-family: Arial, "
            "Helvetica, Sans-Serif; Color: #000088; }</style>";
    page += "</head><body><h1>BS</h1>";
    page += "<h3>DHT22</h3>";
    page += "<ul><li>Temperature : ";
    page += t;
    page += "°C</li>";
    page += "<li>Humidite : ";
    page += h;
    page += "%</li></ul><h3>CO</h3>";
    page += "<ul><li>co : ";
    page += p;
    page += " PPM</li></ul>";
    page += "<h3>LED</h3>";
    page += "<form action='/' method='POST'>";
    page += "<ul><li>LED (etat: ";
    page += etatLed;
    page += ")";
    page += "<INPUT type='radio' name='LED' value='1'>ON";
    page += "<INPUT type='radio' name='LED' value='0'>OFF</li></ul>";
    page += "<INPUT type='submit' value='Actualiser'>";
    page += "<br><br><p>FINISH</p>";
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
    pinMode(FIREPIN, INPUT);
    digitalWrite(FIREPIN, 1);
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
        digitalWrite(LEDPIN, backValue(FIREPIN));
    } else {
        digitalWrite(LEDPIN, 0);
    }
}

void loop() {
    server.handleClient();
    led();

    t = dht.readTemperature();
    h = dht.readHumidity();
    p = 10 + analogRead(COPIN) * 990 / 1024;

    delay(1000);
}
