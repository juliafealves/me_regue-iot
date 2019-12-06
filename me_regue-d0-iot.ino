#include <ESP8266WiFi.h>
 
#define SSID_NETWORK ""
#define PASSWORD_NETWORK ""
#define INTERVAL_SEND_THINGSPEAK 30000
#define INPUT_D0 13
#define LED_HUMIDITY 12
#define LED_NO_HUMIDITY 14
#define LED_NETWORK 15
 
char UrlAPI[] = "api.thingspeak.com";
String ApiWriteKeyThingSpeak = "713U5W87STXFDUGO";
long lastConnectionTime; 
WiFiClient client;
 
/*
 * Prototypes
 */
bool checkIfCanSend(void);
void connectWifi(void);
int hasHumidity(void);
void sendInformationsAPI(String data);
 
/**
 * Send information to API.
 */
void sendInformationsAPI(String data)
{
  unsigned long timeout;
  
  if (client.connect(UrlAPI, 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+ ApiWriteKeyThingSpeak +"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");
    client.print(data);

    lastConnectionTime = millis();
    Serial.println("Sent to ThingSpeak!");
  }
}
 
/**
 * To do connection with Wifi.
 */
void connectWifi(void)
{
    client.stop();
    Serial.println("Connecting with WiFi network...");
    Serial.println();  
    delay(1000);
    WiFi.begin(SSID_NETWORK, PASSWORD_NETWORK);
 
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected!");  
    Serial.println("IP: ");
    Serial.println(WiFi.localIP());
    delay(1000);
}

/**
 * Verify if has humidity.
 */
int hasHumidity(void)
{
  int isHumidity;
  isHumidity = digitalRead(INPUT_D0);
  Serial.println(isHumidity);

  if(!isHumidity){
    Serial.println("WITH HUMIDITY");
    digitalWrite(LED_HUMIDITY, HIGH);
    digitalWrite(LED_NO_HUMIDITY, LOW);
  } else {
    Serial.println("WITHOUT HUMIDITY");
    digitalWrite(LED_HUMIDITY, LOW);
    digitalWrite(LED_NO_HUMIDITY, HIGH);
  }

  return isHumidity;
}

/*
 * Check if you're connected to WiFi and if it's time to send data to ThingSpeak.
 */
bool checkIfCanSend(void)
{
  return !client.connected() && (millis() - lastConnectionTime > INTERVAL_SEND_THINGSPEAK);
}

void setup()
{
  pinMode(INPUT_D0, INPUT);
  pinMode(LED_HUMIDITY, OUTPUT);
  pinMode(LED_NO_HUMIDITY, OUTPUT);
  pinMode(LED_NETWORK, OUTPUT);
  
  Serial.begin(9600);
  lastConnectionTime = 0; 
  connectWifi();
  Serial.println("Me Regue IoT with ESP8266.");
}

void loop()
{
  char FieldHumidity[11];
  
  if (client.connected())
  {
    client.stop();
    Serial.println("- Desconnect of the ThingSpeak");
    Serial.println();
  }
     
  if(checkIfCanSend())
  {
    sprintf(FieldHumidity,"field1=%d", hasHumidity());
    sendInformationsAPI(FieldHumidity);

    while(client.connected())
    {
      digitalWrite(LED_NETWORK, HIGH);
      delay(100);
      digitalWrite(LED_NETWORK, LOW);
      delay(100);
    }
  }
 
  delay(1000);
}
