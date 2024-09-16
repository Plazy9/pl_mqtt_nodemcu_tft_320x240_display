#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

//Wifi MQTT
const char* ssid = wifiSSID;
const char* password = wifiPassword;

const char* mqtt_server = mqttServer;
const char* mqttServerUser = mqttUser;
const char* mqttServerPWD = mqttPassword;

const String mqttMainTopic = mqttMainTopic_CFG;
const String mqttDeviceName = mqttDeviceName_CFG; 

String full_mqtt_topic = (mqttMainTopic+"/"+mqttDeviceName).c_str();

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
unsigned long lastMsg2 = 0;
unsigned long lastProcessTime = 0;

#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
String ElectricityDelivered = "";
String ElectricityReturned = "";
String PoolTemp_0 = "";
String PoolTemp_1 = "";
String deviceStatus = "";


// Define pin connections
#define TFT_CS   D1
#define TFT_RST  D2
#define TFT_DC   D3
#define TFT_MOSI D7
#define TFT_SCK  D5
#define TFT_MISO D6

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// Create Adafruit ILI9341 object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Get screen dimensions
int screenWidth = 0;
int screenHeight = 0;

int infoBlockY =0;

void setup_wifi() {
  // We start by connecting to a WiFi network
  
  tft.fillScreen(ILI9341_BLUE);

  tft.setCursor(0,10);
  tft.println("Connecting to Wifi....." );


  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tft.print("." );
  }

  tft.println("." );
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //tft.clearDisplay();
  tft.setCursor(0,50);
  tft.println("IP address: " );
  tft.setCursor(0,70);
  tft.println(WiFi.localIP());
  //tft.display();
}
char tempBuffer[50];
void callback(char* topic, byte* payload, unsigned int length) {
  

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(1);
  char messageText[14];

  if(strcmp(topic, "dsmr/reading/electricity_currently_delivered") ==  0 ){
    ElectricityDelivered = "";
    for (int i = 0; i < length; i++) {
      ElectricityDelivered += (char)payload[i];
    }
    //Serial.print(ElectricityDelivered);
    tft.setCursor(5,5);
    tft.println("Delivered: ");
    tft.setCursor(5,20);
    tft.setTextSize(2);
    snprintf(tempBuffer, sizeof(tempBuffer), "%s kW", ElectricityDelivered);
    snprintf(messageText, sizeof(messageText), "%10s", tempBuffer);
    tft.print(messageText);
  }

  if(strcmp(topic, "dsmr/reading/electricity_currently_returned") ==  0 ){
    ElectricityReturned = "";
    for (int i = 0; i < length; i++) {
      ElectricityReturned += (char)payload[i];
    }
    //Serial.print(ElectricityReturned);
    tft.setCursor(5,45);
    tft.setTextSize(1);
    tft.println("Returned: ");
    tft.setTextSize(2);
    tft.setCursor(5,60);
    snprintf(tempBuffer, sizeof(tempBuffer), "%s kW", ElectricityReturned);
    snprintf(messageText, sizeof(messageText), "%10s", tempBuffer);
    tft.print(messageText);
  }

  if(strcmp(topic, "dsmr/reading/electricity_delivered") ==  0 ){
    ElectricityReturned = "";
    for (int i = 0; i < length; i++) {
      ElectricityReturned += (char)payload[i];
    }
    //Serial.print(ElectricityReturned);
    tft.setCursor(5,135);
    tft.setTextSize(1);
    tft.println("Delivered: ");
    tft.setTextSize(2);
    tft.setCursor(5,150);
    snprintf(tempBuffer, sizeof(tempBuffer), "%.2f kWh", ElectricityReturned.toFloat());
    snprintf(messageText, sizeof(messageText), "%12s", tempBuffer);
    tft.print(messageText);
  }

  if(strcmp(topic, "dsmr/reading/electricity_returned") ==  0 ){
    ElectricityReturned = "";
    for (int i = 0; i < length; i++) {
      ElectricityReturned += (char)payload[i];
    }
    Serial.print(ElectricityReturned);
    tft.setCursor(5,175);
    tft.setTextSize(1);
    tft.println("Returned: ");
    tft.setTextSize(2);
    tft.setCursor(5,190);
    snprintf(tempBuffer, sizeof(tempBuffer), "%.2f kWh", ElectricityReturned.toFloat());
    snprintf(messageText, sizeof(messageText), "%12s", tempBuffer);
    //sprintf(messageText, "%s kWh", ElectricityReturned);
    tft.print(messageText);
  }

  if(strcmp(topic, "pl_devices/pool_temperature/temperature0") ==  0 ){
    PoolTemp_0 = "";
    for (int i = 0; i < length; i++) {
      PoolTemp_0 += (char)payload[i];
    }
    //Serial.print(PoolTemp_0);
    tft.setTextSize(1);
    tft.setCursor(screenWidth/2+5,5);
    tft.println("Temp0: ");
    tft.setCursor(screenWidth/2+5,20);
    tft.setTextSize(2);
    sprintf(messageText, "%8s C", PoolTemp_0);
    tft.print(messageText);
    tft.println();
  }

  if(strcmp(topic, "pl_devices/pool_temperature/temperature1") ==  0 ){
    PoolTemp_1 = "";
    for (int i = 0; i < length; i++) {
      PoolTemp_1 += (char)payload[i];
    }
    //Serial.print(PoolTemp_1);
    tft.setCursor(screenWidth/2+5,45);
    tft.setTextSize(1);
    tft.println("Temp1: ");
    tft.setTextSize(2);
    tft.setCursor(screenWidth/2+5,60);
    sprintf(messageText, "%8s C", PoolTemp_1);
    tft.print(messageText);
  }

//-------esp01_DHT22_01-------------------------------------------------------------
  String deviceName = "pl_devices/esp01_DHT22_01/";
  infoBlockY = 90;
  strcpy(messageText, "");
  if(strcmp(topic, (deviceName + "status").c_str()) ==  0 ){
    deviceStatus = "";
    for (int i = 0; i < length; i++) {
      deviceStatus += (char)payload[i];
    }
    
    if(deviceStatus == "offline"){
      tft.setTextSize(2);
      tft.setCursor(screenWidth/2+5, infoBlockY+15 );
      sprintf(messageText, "  offline  ");
      tft.print(messageText);
      tft.setCursor(screenWidth/2+5, infoBlockY+50);
      tft.print(messageText);
    }
  }
  strcpy(messageText, "");
  if(strcmp(topic, (deviceName + "temperature").c_str()) ==  0 ){
    PoolTemp_1 = "";
    for (int i = 0; i < length; i++) {
      PoolTemp_1 += (char)payload[i];
    }
    //Serial.print(PoolTemp_1);
    tft.setCursor(screenWidth/2+5, infoBlockY);
    tft.setTextSize(1);
    tft.println("Terasz: ");
    tft.setTextSize(2);  
    tft.setCursor(screenWidth/2+5, infoBlockY+15);
    sprintf(messageText, "%8s C", PoolTemp_1);
    tft.print(messageText);
  }
  
  strcpy(messageText, "");
  if(strcmp(topic, (deviceName + "humidity").c_str()) ==  0 ){
    PoolTemp_1 = "";
    for (int i = 0; i < length; i++) {
      PoolTemp_1 += (char)payload[i];
    }
    //Serial.print(PoolTemp_1);
    tft.setCursor(screenWidth/2+5, infoBlockY+35);
    tft.setTextSize(1);
    tft.println("Terasz humi: ");
    tft.setTextSize(2);
    tft.setCursor(screenWidth/2+5, infoBlockY+50);
    sprintf(messageText, "%8s %%", PoolTemp_1);
    tft.print(messageText);
  }

//-------esp01_DHT22_02-------------------------------------------------------------
  deviceName = "pl_devices/esp01_DHT22_02/";
  infoBlockY = 165;
  strcpy(messageText, "");
  if(strcmp(topic, (deviceName + "status").c_str()) ==  0 ){
    deviceStatus = "";
    for (int i = 0; i < length; i++) {
      deviceStatus += (char)payload[i];
    }
    
    if(deviceStatus == "offline"){
      tft.setTextSize(2);
      tft.setCursor(screenWidth/2+5, infoBlockY+15 );
      sprintf(messageText, "  offline  ");
      tft.print(messageText);
      tft.setCursor(screenWidth/2+5, infoBlockY+50);
      tft.print(messageText);
    }
  }
  strcpy(messageText, "");
  if(strcmp(topic, (deviceName + "temperature").c_str()) ==  0 ){
    PoolTemp_1 = "";
    for (int i = 0; i < length; i++) {
      PoolTemp_1 += (char)payload[i];
    }
    //Serial.print(PoolTemp_1);
    tft.setCursor(screenWidth/2+5, infoBlockY);
    tft.setTextSize(1);
    tft.println("Back: ");
    tft.setTextSize(2);  
    tft.setCursor(screenWidth/2+5, infoBlockY+15);
    sprintf(messageText, "%8s C", PoolTemp_1);
    tft.print(messageText);
  }
  
  strcpy(messageText, "");
  if(strcmp(topic, (deviceName + "humidity").c_str()) ==  0 ){
    PoolTemp_1 = "";
    for (int i = 0; i < length; i++) {
      PoolTemp_1 += (char)payload[i];
    }
    //Serial.print(PoolTemp_1);
    tft.setCursor(screenWidth/2+5, infoBlockY+35);
    tft.setTextSize(1);
    tft.println("Back humi: ");
    tft.setTextSize(2);
    tft.setCursor(screenWidth/2+5, infoBlockY+50);
    sprintf(messageText, "%8s %%", PoolTemp_1);
    tft.print(messageText);
  }


  //  tft.clearDisplay();

  //tft.fillScreen(ILI9341_BLACK);

  //Serial.println();


  // Switch on the LED if an 1 was received as first character
  /*
  if ((char)payload[0] == '1') {
    client.publish("pl_esp12_currency_display/pl_stateTopic", "on");
    digitalWrite(MY_BLUE_LED_PIN , LOW);   // Turn the LED on (Note that LOW is the voltage level
    digitalWrite(RELAY_PIN , LOW);
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    client.publish("pl_esp12_currency_display/pl_stateTopic", "off");
    digitalWrite(MY_BLUE_LED_PIN , HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(RELAY_PIN , HIGH);
  }
  */
}

void reconnect() {
  // Loop until we're reconnected

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqttServerUser, mqttServerPWD, (full_mqtt_topic+"/status").c_str(), 0, true, "offline")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      snprintf (msg, MSG_BUFFER_SIZE, "online");
      client.publish((full_mqtt_topic+"/status").c_str(), msg);
      // ... and resubscribe
      

      client.subscribe("dsmr/reading/electricity_currently_returned");
      client.subscribe("dsmr/reading/electricity_currently_delivered");
      
      // pool temp 
      //client.subscribe("pl_nodemcu_temp/pl_outTopic/0/");
      //client.subscribe("pl_nodemcu_temp/pl_outTopic/1/");

      client.subscribe("pl_devices/pool_temperature/temperature0");
      client.subscribe("pl_devices/pool_temperature/temperature1");


      client.subscribe("dsmr/reading/electricity_delivered");
      client.subscribe("dsmr/reading/electricity_returned");

      //terace temp & humi
      client.subscribe("pl_devices/esp01_DHT22_01/temperature");
      client.subscribe("pl_devices/esp01_DHT22_01/humidity");
      client.subscribe("pl_devices/esp01_DHT22_01/status");
      
      //behind the garage temp & humi
      client.subscribe("pl_devices/esp01_DHT22_02/temperature");
      client.subscribe("pl_devices/esp01_DHT22_02/humidity");
      client.subscribe("pl_devices/esp01_DHT22_02/status");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void drawFrame(){
  // Fill screen with black color
  tft.fillScreen(ILI9341_BLACK);

  // Set text color to white
  tft.setTextColor(WHITE);

  // Set text size
  tft.setTextSize(2);
  
  tft.drawRect(0, 0, screenWidth, screenHeight, RED);
  tft.drawLine(screenWidth/2, 0, screenWidth/2, screenHeight, BLUE);
  tft.drawLine(0, screenHeight/2, screenWidth/2, screenHeight/2, GREEN);

  tft.drawLine(screenWidth/2, screenHeight/3, screenWidth, screenHeight/3, YELLOW);
  tft.drawLine(screenWidth/2, screenHeight/3*2, screenWidth, screenHeight/3*2, YELLOW);

}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize the TFT screen
  tft.begin();

  // Rotate the screen (optional)
  tft.setRotation(1);

  
// Get screen dimensions
  screenWidth = tft.width();
  screenHeight = tft.height();

  drawFrame();
  // Create the text to display
  String screenSizeText = String(screenWidth) + "x" + String(screenHeight);

  // Calculate the position to center the text
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(screenSizeText, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (screenWidth - w) / 2;
  int16_t y = (screenHeight - h) / 2;

  // Print the screen size to the center of the screen
  tft.setCursor(x, y);
  tft.println(screenSizeText);

  setup_wifi();
  delay(2000);
  
  drawFrame();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.setKeepAlive(600);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  unsigned long now = millis();
  //Serial.print("lastmsg: " + String(lastMsg));
  client.loop();
/*
  if (now - lastMsg > 30000 || lastMsg == 0) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish((full_mqtt_topic+"/status").c_str(), msg);
  }
*/
}
