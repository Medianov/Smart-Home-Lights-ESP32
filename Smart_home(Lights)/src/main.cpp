
#define ESP_DRD_USE_SPIFFS true

// Include Libraries

// WiFi Library
#include <WiFi.h>
// File System Library
#include <FS.h>
// SPI Flash Syetem Library
#include <SPIFFS.h>
// WiFiManager Library
#include <ESP_WiFiManager.h>
// Arduino JSON library
#include <ArduinoJson.h>

#include <string>


#include <Firebase_ESP_Client.h>
#include <time.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
// JSON configuration file
#define JSON_CONFIG_FILE "/test_config.json"

// Insert Firebase project API Key
#define API_KEY ""

// Insert Authorized Username and Corresponding Password
#define USER_EMAIL "test@gmail.com"
#define USER_PASSWORD "qwert54321"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL ""

// Flag for saving data
bool shouldSaveConfig = false;

// Define Firebase objects
FirebaseData stream;
FirebaseData stream0;


int onoff;
int timeraktiv;
int startstunde;
int startminute;
int stopstunde;
int stopminute;
int green;
int red;
int blue;


int ledgreen = 18;
int ledblue = 21;
int ledred = 19;
std::string onoffstring="onoff";
std::string lights="/lights/";

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState = LOW; // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
int reset=0;
// constants won't change. They're used here to set pin numbers:
 int BUTTON_PIN = 27; // the number of the pushbutton pin
 int LONG_PRESS_TIME  = 5000; // 5000 milliseconds

std::string listenerPath;

std::string slash="/";

FirebaseAuth auth;
FirebaseConfig config;




// Variables to hold data from custom textboxes
char testString[50] = "test value";
char test2String[50] = "test value2";


// Define WiFiManager Object
ESP_WiFiManager wm;

void saveConfigFile()
// Save Config in JSON format
{
  Serial.println(F("Saving configuration..."));
  
  // Create a JSON document
  StaticJsonDocument<512> json;
  json["testString"] = testString;
  json["test2String"] = test2String;
  //json["testNumber"] = testNumber;

  // Open config file
  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile)
  {
    // Error, file did not open
    Serial.println("failed to open config file for writing");
  }

  // Serialize JSON data to write to file
  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0)
  {
    // Error writing file
    Serial.println(F("Failed to write to file"));
  }
  // Close file
  configFile.close();
}

bool loadConfigFile()
// Load existing configuration file
{


  // Read configuration from FS json
  Serial.println("Mounting File System...");

  // May need to make it begin(true) first time you are using SPIFFS
  if (SPIFFS.begin(false) || SPIFFS.begin(true))
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists(JSON_CONFIG_FILE))
    {
      // The file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
      if (configFile)
      {
        Serial.println("Opened configuration file");
        StaticJsonDocument<512> json;
        DeserializationError error = deserializeJson(json, configFile);
        serializeJsonPretty(json, Serial);
        if (!error)
        {
          Serial.println("Parsing JSON");

          strcpy(testString, json["testString"]);
          strcpy(test2String, json["test2String"]);

          

          return true;
        }
        else
        {
          // Error loading JSON data
          Serial.println("Failed to load json config");
        }
      }
    }
  }
  else
  {
    // Error mounting file system
    Serial.println("Failed to mount FS");
  }

  return false;
}


void saveConfigCallback()
// Callback notifying us of the need to save configuration
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback(ESP_WiFiManager *myWiFiManager)
// Called when config mode launched
{
  Serial.println("Entered Configuration Mode");

  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());

  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}




// Callback function that runs on database changes
void streamCallback(FirebaseStream data){
time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);



  // Get the path that triggered the function
  String streamPath = String(data.dataPath());

  // if the data returned is an integer, there was a change on the GPIO state on the following path listenerPath
  if ((data.dataTypeEnum() == fb_esp_rtdb_data_type_integer)){
    String gpio = streamPath.substring(1);
    int state = data.intData();

if(gpio == "timeraktiv"){
  timeraktiv = state;
}else if(gpio =="startstunde"){
  startstunde = state;
}else if (gpio == "startminute"){
  startminute = state;
}else if(gpio =="stopstunde"){
  stopstunde = state;
}else if (gpio == "stopminute"){
  stopminute = state;
}else if (gpio == "onoff"){
  onoff = state;
}else if(gpio =="green"){
  green = state;
}else if(gpio =="red"){
  red = state;
}else if (gpio == "blue"){
  blue = state;

}

  }
   

  /* When it first runs, it is triggered on the root path and returns a JSON with all keys
  and values of that path. So, we can get all values from the database and updated the GPIO states*/
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json){
    FirebaseJson json = data.to<FirebaseJson>();

    // To iterate all values in Json object
    size_t count = json.iteratorBegin();
    Serial.println("\n---------");
    for (size_t i = 0; i < count; i++){
      
        FirebaseJson::IteratorValue value = json.valueAt(i);
        String gpio = value.key.c_str();
        int state = value.value.toInt();

if(gpio == "timeraktiv"){
  timeraktiv = state;
  Serial.print("timeraktiv: ");
    Serial.println(timeraktiv);
}else if(gpio =="startstunde"){
  startstunde = state;
  Serial.print("startstunde: ");
    Serial.println(startstunde);
}else if (gpio == "startminute"){
  startminute = state;
  Serial.print("startminute: ");
    Serial.println(startminute);
}else if(gpio =="stopstunde"){
  stopstunde = state;
  Serial.print("stopstunde: ");
    Serial.println(stopstunde);
}else if (gpio == "stopminute"){
  stopminute = state;
  Serial.print("stopminute: ");
    Serial.println(stopminute);
}else if (gpio == "onoff"){
  onoff = state;
  Serial.print("onoff: ");
    Serial.println(onoff);
}else if(gpio =="green"){
  green = state;
  Serial.print("green: ");
    Serial.println(green);
}else if(gpio =="red"){
  red = state;
  Serial.print("red: ");
    Serial.println(red);
}else if (gpio == "blue"){
  blue = state;
  Serial.print("blue: ");
    Serial.println(blue);
}


    }
    json.iteratorEnd(); // required for free the used memory in iteration (node data collection)

  }
  
}

void streamTimeoutCallback(bool timeout){
  if (timeout)
    Serial.println("stream timeout, resuming...\n");
  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}





void setup()
{
  Serial.begin(115200);
  // Change to true when testing to force configuration every time we run
  bool forceConfig = false;

  bool spiffsSetup = loadConfigFile();
  if (!spiffsSetup)
  {
    Serial.println(F("Forcing config mode as there is no saved config"));
    forceConfig = true;
  }

  // Explicitly set WiFi mode
  WiFi.mode(WIFI_STA);

  char ssid[50];
snprintf(ssid, 50, "%llX", ESP.getEfuseMac());

  // Setup Serial monitor
  Serial.begin(115200);
  delay(10);

  // Reset settings (only for development)
  //wm.resetSettings();

  if(reset){
    reset = 0;
   wm.resetSettings();
   
  }
 
  
  
  

  // Set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);

  // Set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  // Custom elements

  // Text box (String) - 50 characters maximum
  ESP_WMParameter custom_text_box("key_text", "Bitte Ihre User ID eingeben:", testString, 50);
  // Need to convert numerical input to string to display the default value.
  char convertedValue[6];
  ESP_WMParameter custom_text_box2("key_text2", "Bitte Ihre Geraet ID eingeben:", test2String, 50);


  // Add all defined parameters
  wm.addParameter(&custom_text_box);
  wm.addParameter(&custom_text_box2);


  if (forceConfig)
    // Run if we need a configuration
  {
    if (!wm.startConfigPortal("Smart_Home(Lights)", ssid))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
  else
  {
    if (!wm.autoConnect("Smart Home(Lights)", ssid))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }

  // If we get here, we are connected to the WiFi

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  configTime((1*3600), 0, "pool.ntp.org","time.nist.gov");



  // Copy the string value
  strncpy(testString, custom_text_box.getValue(), sizeof(testString));
  Serial.print("testString: ");
  Serial.println(testString);
  strncpy(test2String, custom_text_box2.getValue(), sizeof(test2String));
  Serial.print("test2String: ");
  Serial.println(test2String);



  listenerPath = std::string(testString) +lights+ test2String+ slash;

  

  // Save the custom parameters to FS
  if (shouldSaveConfig)
  {
    saveConfigFile();
  }

  pinMode(ledgreen, OUTPUT);
  pinMode(ledblue, OUTPUT);
  pinMode(ledred, OUTPUT);

   ledcAttachPin(ledgreen, 0); // assign a led pins to a channel
   ledcAttachPin(ledblue, 1);
   ledcAttachPin(ledred, 2);
  // Initialize channels
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(0, 5000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);

pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(2,OUTPUT);
  
  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Streaming (whenever data changes on a path)
  // Begin stream on a database path --> board1/outputs/digital
  if (!Firebase.RTDB.beginStream(&stream, listenerPath.c_str()))
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());

  // Assign a calback function to run when it detects changes on the database
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);

  delay(2000);

}




void loop(){

time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

currentState = digitalRead(BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW){        // button is pressed
    pressedTime = millis();
  }else if(lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if( pressDuration > LONG_PRESS_TIME ){
      digitalWrite(2,HIGH);
      delay(3000);
      digitalWrite(2,LOW);
      if(pressedTime){
      reset = 1;
      setup();
      }
      
      
    }
  }

  // save the the last state
  lastState = currentState;
  

 if (onoff){
     ledcWrite(0, green);
     ledcWrite(1, blue);
     ledcWrite(2, red);
    if(timeraktiv && (startstunde != stopstunde || startminute !=stopminute)){
      if(((p_tm->tm_hour == stopstunde) && (p_tm->tm_min == stopminute))&& (p_tm->tm_sec < 2)){
       Firebase.RTDB.setInt(&stream0, listenerPath+onoffstring,0);
      }
    }
   }else {
     ledcWrite(0, 0);
     ledcWrite(1, 0);
     ledcWrite(2, 0);
     if (timeraktiv && (startstunde != stopstunde || startminute !=stopminute))
     {
       if (((p_tm->tm_hour == startstunde) && (p_tm->tm_min == startminute))&& (p_tm->tm_sec < 2))
       {
        Firebase.RTDB.setInt(&stream0,listenerPath+onoffstring,1);
       }
   }
   }

if(!Firebase.ready()){
    Firebase.reconnectWiFi(true);
   }
   if(WiFi.status() != WL_CONNECTED){
    WiFi.reconnect();
   }
}
