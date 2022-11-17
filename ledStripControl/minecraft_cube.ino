#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

// LED
#define NUM_LEDS 7
#define DATA_PIN D7

CRGB leds[NUM_LEDS];

unsigned long previousLEDTime = 0;
const int minBrightness = 30;
int maxBrightness = 255;
int fadeInTime = 1000;
int fadeOutTime = 1500;
int fadeInStepDelay = 0;
int fadeOutStepDelay = 0;
int hue = 0;
int brightness = maxBrightness;
bool fadingIn = true;
bool runColorsTransition = true;


// WI-FI
const char *ssid = "<WI-FI name>";
const char *password = "<WI-FI password>";

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer server(80);

unsigned long previousHTTPTime = 0;
const long httpTimeoutTime = 2000;

// Variable to store the HTTP request
String header;

void setup()
{
  Serial.begin(115200); // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  // LED
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  setRandomHue();
  udpateTransitionVars();
  updateLed();

  // WI-FI
  wifiMulti.addAP(ssid, password);
  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("cube"))
  { // Start the mDNS responder for cube.local
    Serial.println("mDNS responder started");
  }
  else
  {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/transition", HTTP_POST, handleTransition);
  server.on("/random", HTTP_POST, handleRandom);
  server.on("/set_brightness", HTTP_POST, handleBrightness);
  server.on("/fade_time", HTTP_POST, handleFadeTime);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  MDNS.update();

  updateLed();
  colorsTransition();
  server.handleClient();
}


void udpateTransitionVars()
{
  int range = maxBrightness - minBrightness;
  fadeInStepDelay = fadeInTime / range;
  fadeOutStepDelay = fadeOutTime / range;
}

void updateLed()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(hue, 255, max(minBrightness, min(brightness, maxBrightness)));
  }

  FastLED.show();
}

void setRandomHue()
{
  hue = random(0, 255);
}

void startColorsTransition()
{
  udpateTransitionVars();
  runColorsTransition = true;
  fadingIn = true;
  setRandomHue();
}

void stopColorsTransition()
{
  runColorsTransition = false;
}

void colorsTransition()
{
  if (!runColorsTransition)
  {
    return;
  }

  int currentTime = millis();
  if (currentTime - previousLEDTime >= (fadingIn ? fadeInStepDelay : fadeOutStepDelay))
  {
    previousLEDTime = currentTime;

    if (fadingIn)
    {
      brightness++;

      if (brightness >= maxBrightness)
      {
        fadingIn = false;
      }
    }
    else
    {
      brightness--;

      if (brightness <= minBrightness)
      {
        setRandomHue();
        fadingIn = true;
      }
    }
  }
}

void handleRoot()
{
  char html[1000];

  sprintf(
      html,
      "<form action=\"/transition\" method=\"POST\"> \
        <input type=\"submit\" value=\"Transition\" /> \
       </form> \
       <br/> \
       <form action=\"/random\" method=\"POST\"> \
        <input type=\"submit\" value=\"Random color\" /> \
       </form> \
       <br/> \
       <form action=\"/set_brightness\" method=\"POST\"> \
         <label for=\"saturation\">Brightness (%d-255)</label> \
         <input id=\"brightness\" type=\"number\" name=\"brightness\" min=\"%d\" max=\"255\" value=\"%d\" /> \
         <input type=\"submit\" value=\"Save\" /> \
       </form> \
       <form action=\"/fade_time\" method=\"POST\"> \
         <label for=\"fadeIn\">Fade in time (ms)</label> \
         <input id=\"fadeIn\" type=\"number\" name=\"fadeIn\" min=\"0\" value=\"%d\" /> \
         <br/> \
         <label for=\"fadeOut\">Fade out time (ms)</label> \
         <input id=\"fadeOut\" type=\"number\" name=\"fadeOut\" min=\"0\" value=\"%d\" /> \
         <br/> \
         <input type=\"submit\" value=\"Save\" /> \
      </form>",
      minBrightness,
      minBrightness,
      maxBrightness,
      fadeInTime,
      fadeOutTime
   );

  server.send(200, "text/html", html);
}

void handleTransition()
{
  startColorsTransition();
  server.sendHeader("Location", " / ");
  server.send(303);
}

void handleRandom()
{
  stopColorsTransition();
  setRandomHue();
  updateLed();
  server.sendHeader("Location", " / ");
  server.send(303);
}

void handleBrightness()
{
  if (server.hasArg("brightness") && server.arg("brightness") != NULL)
  {
    maxBrightness = max(minBrightness, atoi(server.arg("brightness").c_str()));
    udpateTransitionVars();
  }

  server.sendHeader("Location", " / ");
  server.send(303);
}

void handleFadeTime()
{
  if (
    server.hasArg("fadeIn") &&
    server.arg("fadeIn") != NULL &&
    server.hasArg("fadeOut") &&
    server.arg("fadeOut") != NULL
  )
  {
    fadeInTime = max(0, atoi(server.arg("fadeIn").c_str()));
    fadeOutTime = max(0, atoi(server.arg("fadeOut").c_str()));
    udpateTransitionVars();
  }

  server.sendHeader("Location", " / ");
  server.send(303);
}

void handleNotFound()
{
  server.send(404, "text / plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
