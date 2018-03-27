#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "FastLED.h"

FASTLED_USING_NAMESPACE

const char* ssid = "";
const char* password = "";

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
int brightness = 1;
int x = 1;

void handleRoot() {
 String message2 = "<html>\n\n<head>\n    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>\n    <script type=\"text/javascript\">\n        $(document).ready(function() {\n\n            $(\"#setLedButton\").click(function() {\n                m = \"/setleds?r=\";\n                m += $(\"#r\").val() + \"&g=\";\n                m += $(\"#g\").val() + \"&b=\";\n                m += $(\"#b\").val();\n                $.get(m);\n\n            });\n\n            $(\"#off\").click(function() {\n                $.get(\"/off\");\n\n            });\n            $(\"#rainbow\").click(function() {\n                $.get(\"/rainbow\");\n\n            });\n            $(\"#sinelon\").click(function() {\n                $.get(\"/sinelon\");\n\n            });\n            $(\"#confetti\").click(function() {\n                $.get(\"/confetti\");\n\n            });\n             $(\"#bpm\").click(function() {\n                $.get(\"/bpm\");\n\n            });\n\n            $(\"#juggle\").click(function() {\n                $.get(\"/juggle\");\n\n            });\n\n            $(\"#pulseFade\").click(function() {\n                $.get(\"/pulseFade\");\n\n            });\n\n            $(\"#cyclonfill\").click(function() {\n                $.get(\"/cyclonfill\");\n\n            });\n\n            $(\"#twinkle\").click(function() {\n                $.get(\"/twinkle\");\n\n            });\n\n            $(\"#blink\").click(function() {\n                $.get(\"/blink\");\n\n            });\n\n        });\n    </script>\n    <style type=\"text/css\">\n\n      p {\n        height: 100px;\n        width: 25%;\n      }\n\n      .slider{\n        height: 100px; \n        width: 75%;\n      }\n\n      .slider::-webkit-slider-thumb {\n        background: red;\n        -webkit-appearance: none;\n        appearance: none;\n        width: 150px;\n        height: 150px;\n      }\n\n      .animButton{\n        height: 10%; \n        width: 20%; \n        font-size: 32px; \n        margin: 5%;\n      }\n\n    </style>\n</head>\n\n<body style='font-family: sans-serif; font-size: 64px; margin: 2.5%'>Following functions are available:\n    <br>\n    <br>\n        <!--Functions Buttons-->\n        <button id=\"off\" class=\"animButton\">Off</button>\n    \n    \n        <button id=\"rainbow\" class=\"animButton\">Rainbow</button>\n    \n    \n        <button id=\"sinelon\" class=\"animButton\">Sinelon</button>\n    \n    <br>\n    \n        <button id=\"confetti\" class=\"animButton\">Confetti</button>\n    \n    \n        <button id=\"bpm\" class=\"animButton\">Bpm</button>\n    \n    \n        <button id=\"juggle\" class=\"animButton\">Juggle</button>\n    \n    <br>\n        <button id=\"pulseFade\" class=\"animButton\">Pulse Fade</button>\n\n        <button id=\"cyclonfill\" class=\"animButton\">Cyclon Fill</button>\n\n        <button id=\"twinkle\" class=\"animButton\">Twinkle</button>\n    <br>\n        <button id=\"blink\" class=\"animButton\">Blink</button>\n    <br>\n    <p>Red</p>\n        <input id=\"r\" class=\"slider\" type=\"range\" min=\"0\" max=\"255\" value=\"0\" autofocus>\n    \n    <br>\n    <p>Green</p>\n        <input id=\"g\" class=\"slider\" type=\"range\" min=\"0\" max=\"255\" value=\"0\" autofocus>\n    \n    <br>\n    <p>Blue</p>\n        <input id=\"b\" class=\"slider\" type=\"range\" min=\"0\" max=\"255\" value=\"0\" autofocus>\n    \n    <br>\n    <button id=\"setLedButton\" class=\"animButton\">Set Leds</button>\n</body>\n\n</html>";
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
    animation = 5;
  });

  server.on("/setleds", [](){
    animation = 6;
    Serial.print('r '+server.arg(0).toInt() + ' g ' + server.arg(1).toInt() + ' b ' + server.arg(2).toInt());
    r = server.arg(0).toInt();
    g = server.arg(1).toInt();
    b = server.arg(2).toInt();
    
  });

   server.on("/pulseFade", [](){
    animation = 7;
  });

   server.on("/cyclonfill", [](){
    animation = 8;
  }); 

  server.on("/twinkle", [](){
    animation = 9;
  });

  server.on("/blink", [](){
    animation = 10;
  });
  

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  ArduinoOTA.handle();
  server.handleClient();
  
  switch(animation){
    case 0:
      fill_solid( leds, NUM_LEDS, CRGB::Black);
      // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
      break;
    case 1:
      sinelon();
      // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
        break;
    case 2: 
      confetti();
      // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
      break;
    case 3:
      rainbow();
      // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
      break;
    case 4:
      juggle();
      // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
      break;
    case 5:
      bpm();
      // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
      break;
    case 6:
      setleds();
       // send the 'leds' array out to the actual LED strip
        FastLED.show();  
        // insert a delay to keep the framerate modest
        FastLED.delay(1000/FRAMES_PER_SECOND);
      break;
    case 7:
      glow();
      break;
    case 8:
      cylonfill();
      break;
    case 9:
      twinkle();
      break;
    case 10:
      blink();
      break;
    default:
    fill_solid( leds, NUM_LEDS, CRGB::Black);
      break;
  }
  

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
  FastLED.setBrightness(96);
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

void glow(){
  for(int i = 0; i<NUM_LEDS;i++){
    leds[i] = CRGB::Green;
  }
  //ramp up in brightness
  for(int i =0; i<250;i= i+10){
    FastLED.setBrightness(i);
    FastLED.show();
    delay(100);
  }
  //ramp down from brightest to darkest
  for(int i=250 ; i>0;i=i-10){
    FastLED.setBrightness(i);
    FastLED.show();
 delay(160);
  }
}

void cylonfill(){
  // First slide the led in one direction
  for(int i = 0; i < NUM_LEDS-x; i++) {
    // Set the i'th led to red 
    leds[i] = CRGB::Blue;
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Green;
    // Wait a little bit before we loop around and do it again
    delay(16);
   }
        
        leds[NUM_LEDS -x] = CRGB::Orange;
        FastLED.show();
        x++;
        if (x ==NUM_LEDS){
          x =1;
        }
        Serial.println(x);
  // Now go in the other direction.  
  for(int i = NUM_LEDS-x; i >= 2; i--) {
    // Set the i'th led to red 
    leds[i] = CRGB::Purple;
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    leds[i] = CRGB::Black;
    // Wait a little bit before we loop around and do it again
  //  delay(1);
  }
}
void twinkle() {
  int i = random(NUM_LEDS);                                           // A random number. Higher number => fewer twinkles. Use random16() for values >255.
  if (i < NUM_LEDS) leds[i] = CHSV(random(255), random(255), random(255));              // Only the lowest probability twinkles will do. You could even randomize the hue/saturation. .
  for (int j = 0; j < NUM_LEDS; j++) leds[j].fadeToBlackBy(8);
  
  LEDS.show();                                                // Standard FastLED display
  //show_at_max_brightness_for_power();                          // Power managed FastLED display

  //delay(10);                                            // Standard delay
  LEDS.delay(50);                                     // FastLED delay 
  //delay_at_max_brightness_for_power(thisdelay);              // Power managed FastLED delay
}

void blink(){
  for(int i = 0; i<NUM_LEDS;i++){
    if(i%2 ==0){
    leds[i] = CRGB::Red;
    }
    else{
     leds[i] = CRGB:: Black;
    }    
   }
   leds[0] = CRGB::White;
   FastLED.show();
   delay(250);
   
   for(int i = 0; i<NUM_LEDS;i++){
    if(i%2 !=0){
    leds[i] = CRGB::Red;
    }
    else{
     leds[i] = CRGB:: Black;
    }    
   }
   FastLED.show();
   delay(20);
}




