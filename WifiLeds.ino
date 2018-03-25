#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

const char* ssid = "Tree Fitty";
const char* password = "callie0827";

#define DATA_PIN    D3
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    150
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

ESP8266WebServer server(80);


const int led = 13;
int animation = 0;
String value;
//char charBuf[60];
int z=0;
int pixelsInText;
int r,g,b;

void handleRoot() {
  String message2 = "<html>\n\n<head>\n    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>\n    <script type=\"text/javascript\">\n        $(document).ready(function() {\n\n            $(\"#setLedButton\").click(function() {\n                m = \"/setleds?r=\";\n                m += $(\"#r\").val() + \"&g=\";\n                m += $(\"#g\").val() + \"&b=\";\n                m += $(\"#b\").val();\n                $.get(m);\n\n            });\n\n            $(\"#off\").click(function() {\n                $.get(\"/off\");\n\n            });\n            $(\"#rainbow\").click(function() {\n                $.get(\"/rainbow\");\n\n            });\n            $(\"#sinelon\").click(function() {\n                $.get(\"/sinelon\");\n\n            });\n            $(\"#confetti\").click(function() {\n                $.get(\"/confetti\");\n\n            });\n             $(\"#bpm\").click(function() {\n                $.get(\"/bpm\");\n\n            });\n\n            $(\"#juggle\").click(function() {\n                $.get(\"/juggle\");\n\n            });\n\n        });\n    </script>\n    <style type=\"text/css\">\n\n      p {\n        height: 100px;\n        width: 25%;\n      }\n\n      .slider{\n        height: 100px; \n        width: 75%;\n      }\n\n      .slider::-webkit-slider-thumb {\n        background: red;\n        -webkit-appearance: none;\n        appearance: none;\n        width: 150px;\n        height: 150px;\n      }\n\n      .animButton{\n        height: 10%; \n        width: 20%; \n        font-size: 32px; \n        margin: 5%;\n      }\n\n    </style>\n</head>\n\n<body style='font-family: sans-serif; font-size: 64px; margin: 2.5%'>Following functions are available:\n    <br>\n    <br>\n    \n        <button id=\"off\" class=\"animButton\">Off</button>\n    \n    \n        <button id=\"rainbow\" class=\"animButton\">Rainbow</button>\n    \n    \n        <button id=\"sinelon\" class=\"animButton\">Sinelon</button>\n    \n    <br>\n    \n        <button id=\"confetti\" class=\"animButton\">Confetti</button>\n    \n    \n        <button id=\"bpm\" class=\"animButton\">Bpm</button>\n    \n    \n        <button id=\"juggle\" class=\"animButton\">Juggle</button>\n    \n    <br>\n    <p>Red</p>\n        <input id=\"r\" class=\"slider\" type=\"range\" min=\"0\" max=\"255\" value=\"0\" autofocus>\n    \n    <br>\n    <p>Green</p>\n        <input id=\"g\" class=\"slider\" type=\"range\" min=\"0\" max=\"255\" value=\"0\" autofocus>\n    \n    <br>\n    <p>Blue</p>\n        <input id=\"b\" class=\"slider\" type=\"range\" min=\"0\" max=\"255\" value=\"0\" autofocus>\n    \n    <br>\n    <button id=\"setLedButton\" class=\"animButton\">Set Leds</button>\n</body>\n\n</html>";
  
  
  server.send(200, "text/html", message2);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }


  
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


void setup(void){
  
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/off", [](){
  //  server.send(200, "text/plain", "this works as well");
    animation = 0;
  });
  
  server.on("/sinelon", [](){
    //server.send(200, "text/plain", "this works as well");
    animation = 1;
  });
  
   server.on("/confetti", [](){
    //server.send(200, "text/plain", "this works as well");
    animation = 2;
  });

  server.on("/rainbow", [](){
   // server.send(200, "text/plain", "this works as well");
    animation = 3;
  });

  server.on("/juggle", [](){
  //  server.send(200, "text/plain", "this works as well");
    animation = 4;
  });

  server.on("/bpm", [](){
  //  server.send(200, "text/plain", "this works as well");
    animation = 5;
  });

  server.on("/setleds", [](){
   // server.send(200, "text/plain", "this works as well");
    animation = 6;
    Serial.print('r '+server.arg(0).toInt() + ' g ' + server.arg(1).toInt() + ' b ' + server.arg(2).toInt());
    r = server.arg(0).toInt();
    g = server.arg(1).toInt();
    b = server.arg(2).toInt();
    
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  server.handleClient();
  
  switch(animation){
    case 0:
      fill_solid( leds, NUM_LEDS, CRGB::Black);
      break;
    case 1:
      sinelon();
        break;
    case 2: 
      confetti();
      break;
    case 3:
      rainbow();
      break;
    case 4:
      juggle();
      break;
    case 5:
      bpm();
      break;
    case 6:
      setleds();
      break;
    default:
    fill_solid( leds, NUM_LEDS, CRGB::Black);
      break;
  }
   // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}
int c = 0;
void sinelon2()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, c++ % 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void setleds() 
{
  fill_solid( leds, NUM_LEDS, CRGB(r,g,b));
}

