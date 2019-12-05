#include <ESP8266WiFi.h>
 
//defines
#define SSID_NETWORK ""
#define PASSWORD_NETWORK ""
#define INTERVAL_SEND_THINGSPEAK 30000
 
//constantes e variáveis globais
char UrlAPI[] = "api.thingspeak.com";
String ApiWriteKeyThingSpeak = "713U5W87STXFDUGO";
long lastConnectionTime; 
WiFiClient client;
 
/*
 * Prototypes
 */
bool checkIfCanSend(void);
void connectWifi(void);
float getHumidity(void);
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
    Serial.println("- Informações enviadas ao ThingSpeak!");
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
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
    delay(1000);
}
 
//Função: faz a leitura do nível de umidade
//Parâmetros: nenhum
//Retorno: umidade percentual (0-100)
//Observação: o ADC do NodeMCU permite até, no máximo, 3.3V. Dessa forma,
//            para 3.3V, obtem-se (empiricamente) 978 como leitura de ADC
float getHumidity(void)
{
    int adc;
    float percentHumidity;
     adc = analogRead(0);   //978 -> 3,3V
     Serial.print("[Leitura ADC] ");
     Serial.println(adc); 
     percentHumidity = 100 * ((978-(float)adc) / 978);
     Serial.print("[Umidade Percentual] ");
     Serial.print(percentHumidity);
     Serial.println("%");
 
     return percentHumidity;
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
    Serial.begin(9600);
    lastConnectionTime = 0; 
    connectWifi();
    Serial.println("Me Regue IoT with ESP8266.");
}

void loop()
{
  float percentHumidity;
  int percentHumidityTruncate;
  char FieldHumidity[11];
  
  //Força desconexão ao ThingSpeak (se ainda estiver desconectado)
  if (client.connected())
  {
      client.stop();
      Serial.println("- Desconectado do ThingSpeak");
      Serial.println();
  }
  
  percentHumidity = getHumidity();
  percentHumidityTruncate = (int)percentHumidity;
     
    
  if(checkIfCanSend())
  {
    sprintf(FieldHumidity,"field1=%d", percentHumidityTruncate);
    sendInformationsAPI(FieldHumidity);
  }
 
  delay(1000);
}
