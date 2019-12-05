#include <ESP8266WiFi.h>  //essa biblioteca já vem com a IDE. Portanto, não é preciso baixar nenhuma biblioteca adicional
 
//defines
#define SSID_REDE "brisa-669671"  //coloque aqui o nome da rede que se deseja conectar
#define SENHA_REDE "zvgtskf4"  //coloque aqui a senha da rede que se deseja conectar
#define INTERVALO_ENVIO_THINGSPEAK 30000  //intervalo entre envios de dados ao ThingSpeak (em ms)
 
//constantes e variáveis globais
char UrlAPI[] = "api.thingspeak.com";
String ApiWriteKeyThingSpeak = "UT0P27TQPA32VGYQ";
long lastConnectionTime; 
WiFiClient client;
 
//prototypes
void sendInformationsAPI(String data);
void connectWifi(void);
float FazLeituraUmidade(void);
 
/*
 * Implementações
 */
 
/**
 * Send information to API.
 */
void sendInformationsAPI(String data)
{
  unsigned long timeout;
    if (client.connect(UrlAPI, 80))
    {         
        /*client.print("GET /update?api_key=" + ApiWriteKeyThingSpeak + "&field1=777 HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        lastConnectionTime = millis();
        Serial.println("- Information sent to API!");*/
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
    WiFi.begin(SSID_REDE, SENHA_REDE);
 
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
float FazLeituraUmidade(void)
{
    int ValorADC;
    float UmidadePercentual;
 
     ValorADC = analogRead(0);   //978 -> 3,3V
     Serial.print("[Leitura ADC] ");
     Serial.println(ValorADC);
 
     //Quanto maior o numero lido do ADC, menor a umidade.
     //Sendo assim, calcula-se a porcentagem de umidade por:
     //      
     //   Valor lido                 Umidade percentual
     //      _    0                           _ 100
     //      |                                |   
     //      |                                |   
     //      -   ValorADC                     - UmidadePercentual 
     //      |                                |   
     //      |                                |   
     //     _|_  978                         _|_ 0
     //
     //   (UmidadePercentual-0) / (100-0)  =  (ValorADC - 978) / (-978)
     //      Logo:
     //      UmidadePercentual = 100 * ((978-ValorADC) / 978)  
      
     UmidadePercentual = 100 * ((978-(float)ValorADC) / 978);
     Serial.print("[Umidade Percentual] ");
     Serial.print(UmidadePercentual);
     Serial.println("%");
 
     return UmidadePercentual;
}
void setup()
{  
    Serial.begin(9600);
    lastConnectionTime = 0; 
    connectWifi();
    Serial.println("Me Regue IoT with ESP8266.");
}
 
//loop principal
void loop()
{
    /*float UmidadePercentualLida;
    int UmidadePercentualTruncada;
    char FieldUmidade[11];
     */
    //Força desconexão ao ThingSpeak (se ainda estiver desconectado)
    if (client.connected())
    {
        client.stop();
        Serial.println("- Desconectado do ThingSpeak");
        Serial.println();
    }
 
    //UmidadePercentualLida = FazLeituraUmidade();
    //UmidadePercentualTruncada = (int)UmidadePercentualLida; //trunca umidade como número inteiro
     
    //verifica se está conectado no WiFi e se é o momento de enviar dados ao ThingSpeak
    if(!client.connected() && (millis() - lastConnectionTime > INTERVALO_ENVIO_THINGSPEAK))
    {
     //   sprintf(FieldUmidade,"field1=%d",UmidadePercentualTruncada);
        sendInformationsAPI("field1=777");
    }
 
     delay(1000);
}
