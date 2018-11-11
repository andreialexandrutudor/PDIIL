// Libraries for LCD control
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Libraries for temperature sensors
#include <OneWire.h>
#include <DallasTemperature.h>

// Libraries for ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>   // Include the WebServer library

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Create an instance of the ESP8266WiFiMulti class called 'wifiMulti'
ESP8266WiFiMulti wifiMulti;

// Create a webserver object that listens for HTTP request on port 80
ESP8266WebServer server(80);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);


// Functions definitions
void handleRoot();
void handleNotFound();

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  delay(10);

  // Start up the library needed to read value from temperature sensors
  sensors.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  wifiMulti.addAP("c109", "");
  wifiMulti.addAP("DIGI-202A", "A2ETczbj");

  Serial.println("Connecting ...");
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  int i = 0;
  // Wait for the Wi-Fi to connect: scan for Wi-Fi networks
  // and connect to the strongest of the networks above
  while (wifiMulti.run() != WL_CONNECTED) { 
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  lcd.setCursor(0, 0);
  lcd.print("Connected to:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.SSID());
  delay(3000);
  lcd.clear();

  // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/", HTTP_GET, handleRoot);
  // When a client requests an unknown URI (i.e. something other than "/")
  // call function "handleNotFound"
  server.onNotFound(handleNotFound);

  // Actually start the server
  server.begin();

  // Start up the library for temperature sensors
  sensors.begin();
}


void loop(void)
{
  lcd.setCursor(0, 0);
  lcd.print("IP Address:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  
  server.handleClient();
}

void handleRoot()
{
  // Call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(1000);

  float s0 = sensors.getTempCByIndex(0);
  float s1 = sensors.getTempCByIndex(1);

  char t0[16];
  char t1[16];
  sprintf(t0, "s0: %.2f", s0);
  sprintf(t1, "s1: %.2f", s1);
  char temp[30];
  sprintf(temp, "%s %s", t0, t1);
  char text[200];
  sprintf(text, "<meta http-equiv=\"refresh\" content=\"5\"> <br> %s", temp);
  server.send(200, "text/html", text);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(t0);
  lcd.setCursor(0, 1);
  lcd.print(t1);
  delay(2500);
  lcd.clear();

  Serial.println(temp);
}

void handleNotFound()
{
  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
  server.send(404, "text/plain", "404: Not found");
}
