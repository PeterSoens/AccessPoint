//Import Libraries
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D10;     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

unsigned long delayTime;

// Configure connection for MIT Guest WiFi
const char* ssid     = ""; // Set up the WiFi SSID
const char* password = "";        // There is no WiFi password on MIT's guest network
const char* pi_name="";        // Change this value to reflect the number of your Pi. This will be the server for your data handling. 
const int mqtt_port = 1883;       // The port number to use for the MQTT broker connection

String id = " ";

// Define strings for MQTT topics (both to publish and for subscriptions)
String MQTT_inTopic="";          // This is the MQTT topic to which this device subscribes
String MQTT_outTopic="";        // MQTT topic to which output data are published
String MQTT_deviceTopic="";  // MQTT topic to which device status is published
String MQTT_username="";         // MQTT Username
String MQTT_password="";        // MQTT Password

//This will be used by the code to store some variables.---------------------------------------



String pi_ip="";              // Get Pi's IP and put it here, this will change and needs to be updated



String CPU_ID="";             // This is a unique ID composed from the CPU-ID and MAC address
String WIFI_IP="";            // This stores the current WiFi IP address
int temperature=999;

WiFiClient espClient;            // Creates an connection object for use in the connection. 
PubSubClient client(espClient);  // Sets connection to an MQTT client object for use in the connection. 
WiFiClient http_client;              // Creates an HTTP client object for use in the connection.
long lastLoop = 0;                // Last time loop was run
char msg[50];                    // MQTT message character buffer (null to start). Kind of like a string. 



//Get Device Info CPU ID and IP address to create our unique identifier -----------------------------
void get_device_info(){
  byte mac[6];                                  //Sets up variable to get ready for MAC address
  WiFi.macAddress(mac);                         //Get Wifi MAC address and store it in the variable "mac"
  CPU_ID = "ESP_"+String(ESP.getChipId())+"_";  //Saves the CHIP ID to a variable.
  CPU_ID = CPU_ID + String(mac[5],HEX);         //Add the first part of the MAC Addres to a variable.
  CPU_ID = CPU_ID + String(mac[4],HEX);         //Add the second part of the MAC Addres to a variable.
  CPU_ID = CPU_ID + String(mac[3],HEX);         //Add the third part of the MAC Addres to a variable.
  CPU_ID = CPU_ID + String(mac[2],HEX);         //Add the fourth part of the MAC Addres to a variable.
  CPU_ID = CPU_ID + String(mac[1],HEX);         //Add the fifth part of the MAC Addres to a variable.
  CPU_ID = CPU_ID + String(mac[0],HEX);         //Add the sixth part of the MAC Addres to a variable.
  Serial.print("CPU ID: ");                     //Print "CPU ID: " to serial port
  Serial.println(CPU_ID);                       //Print CPU_ID to serial port
  IPAddress ip =WiFi.localIP();                 //Save Wifi IP Address to variable "ip".
  WIFI_IP = String(ip[0])+".";                  //Format ip address into the format I wanted.
  WIFI_IP = WIFI_IP + String(ip[1])+".";        // - - - - - - - - - - - - - - - - - - - - -
  WIFI_IP = WIFI_IP + String(ip[2])+".";        // - - - - - - - - - - - - - - - - - - - - -
  WIFI_IP = WIFI_IP + String(ip[3]);            // - - - - - - - - - - - - - - - - - - - - -
  Serial.print("IP: ");                         //Print "IP: " to serial port
  Serial.println(WIFI_IP);                      //Print WIFI_IP to serial port
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        id += String(buffer[i], HEX);
    }
    Serial.println(id);
    String temp_temp = String(id); 
    Serial.println("Publish message: ");
    Serial.print(temp_temp);
    client.publish(MQTT_outTopic.c_str(), temp_temp.c_str());
    id = "";
}

//Runs a bunch of setup functions like connecting to wifi and get the servers IP.---------------------
void setup() {
  Serial.begin(115200);  //Starts serial connection to allow the PC to read data
  delay(10);             //Waits 10ms after intializing the serial port before initializing another peripheral
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read and write."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    
    Serial.println(F("BEWARE: Data will be written to the PICC, in sector #1"));
  Serial.println("\r\n\r\n");        //Prints a few new lines to the serial port to clear the buffer and make it easier to identify the text appearing after the Pi's reset
  Serial.print("Connecting to ");    //Prints text line prefix to Serial
  Serial.println(ssid);              //Prints WiFi SSID name
  Serial.println("BME280 test");

  bool status;
    
    
  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();
  
  WiFi.begin(ssid, password); //Connects to WiFi, using a function defined in the WiFi library
  
  while (WiFi.status() != WL_CONNECTED) {   //Checks if wifi is not connected
    delay(500);                             //Waits 500ms
    Serial.print(".");                      //Prints "." each 500ms while not yet connected to act a bit like a progress bar in the serial terminal
  }

  Serial.println("\r\nWiFi connected!");   //Prints to Serial
  Serial.print("IP address: ");     //Prints to Serial
  Serial.println(WiFi.localIP());     //Prints Device IP as returned from the ESP8266 module
  
  get_device_info(); //Gets device info

  client.setServer(pi_ip.c_str(), mqtt_port);  //Sets the MQTT broker address and port
  client.setCallback(callback);           //Sets what callback is to be used when message is received.
  delay(100);
}

//MQTT Callback ----------------------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) { // Print the received message on a particular topic
  Serial.print("Message arrived on topic [");   //Prints to the serial to simplify debugging
  Serial.print(topic);                 //Prints to serial the name of the received topic
  Serial.print("] ");
  char message_buff[100];
  for (int i = 0; i < length; i++) {   //Steps thought each character in the character array
    Serial.print((char)payload[i]);    //Prints current character to Serial
    message_buff[i]=payload[i];        //Stores current character to message buffer.
    message_buff[i+1]='\0';            //Adds end of buffer to message_buff
  }
  Serial.println();                                     //Prints newline to Serial
  String msgString = String(message_buff);              //Converts message_buff to String and saves it in msgString
  if(msgString=="STATUS"){                              //Checks to see if message is equal to "STATUS"
    client.publish(MQTT_deviceTopic.c_str(), "ONLINE"); //Publishes ONLINE to device topic
  }
}

//Reconnect/ Connect to MQTT Broker -------------------------------------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  //delay(5500); // Wait 5.5 seconds before trying to reconnect the first time after a disconnect
  while (!client.connected()) {                            //Checks to see if it is connected to MQTT broker and if not, tries to reconnect
    Serial.print("Attempting MQTT connection...");         //Prints to Serial
    // Attempt to connect. Using CPU ID as name with Username and Password
    if (client.connect(CPU_ID.c_str(), MQTT_username.c_str(), MQTT_password.c_str())) {
      Serial.println("connected");                         //Prints newline to Serial
      // Once connected, publish an announcement...
      client.publish(MQTT_deviceTopic.c_str(), "BOOTED");  //Publishes in device topic "BOOTED"
      // ... and resubscribe
      client.subscribe(MQTT_inTopic.c_str());              //Subscribes to inTopic
    } else {
      Serial.print("failed, rc=");                         //Prints to Serial
      Serial.print(client.state());                        //Prints client state to Serial
      Serial.println(" try again in 5 seconds");           //Prints to Serial
      delay(5000);                                         // Wait 5 seconds before retrying
    }
  }
}

//The main program loop that makes the HTTP requests to save temperature data.--------------------
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
  long now = millis();
  delay(10);
}
