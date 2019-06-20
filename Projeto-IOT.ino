
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>  // Including library for dht
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN    D1    
#define SS_PIN     D8   
#define DHTPIN 0

//bool a = true;
String apiKey = "FTT451B849DO4LY9";     //  <-- seu Write API key do site ThingSpeak
const char* server = "api.thingspeak.com";
char auth[] = "58cb28dc188a4af9ae2d4f215d4fbea2";              //Your Project authentication key
char ssid[] = "brisa-244748";                                       // Name of your network (HotSpot or Router name)
char pass[] = "4ixblksy";
BlynkTimer timer;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
const int LM35 = D0; // Define o pino que lera a saída do LM35
float temperatura; // Variável que armazenará a temperatura medida
int luz;
DHT dht(DHTPIN, DHT11);

WiFiClient client;

void setup() 
{
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  dht.begin();
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();

  
  //timer.setInterval(1000L, acesso);
  
 
}

void loop() 
{
  // Procura por cartao RFID
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
 // Procura por cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  conteudo.toUpperCase();

  //imprime os detalhes tecnicos do cartão/tag
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  if ((conteudo.substring(1) == "69 76 0A A9")||(conteudo.substring(1) == "D5 3C B1 79")) //UID 1 - Cartao
  {
    Serial.println("Acesso liberado !");
    Serial.println();
    Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
    Blynk.begin(auth, ssid, pass);
    
  }
  else
  {
    Serial.println("Acesso negado !");
     return;
  }
  while(true){
   mostra();
   Blynk.run();
   timer.run();}
    
}


void mostra(){
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      luz = analogRead(A0);
      if(luz<20){
        digitalWrite(D2, HIGH);
      }
      else{
        digitalWrite(D2, LOW);
      }
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1="; //<-- atenÃ§Ã£o, esse Ã:copyright: o campo 1 que vocÃª escolheu no canal do ThingSpeak
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(luz);
                             
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperatura: ");
                             Serial.print(t);
                             Blynk.virtualWrite(V0, t);
                             Serial.print(" degrees Celcius, Humidade: ");
                             Serial.print(h);
                             Blynk.virtualWrite(V2, h);
                             Serial.print("Luminosidade: ");
                             Serial.println(luz);
                             Blynk.virtualWrite(V1, luz);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 20 seconds
  delay(500);

}
