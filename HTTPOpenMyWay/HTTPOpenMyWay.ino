#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

/*
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   4     
#define SS_PIN 5 

MFRC522 mfrc522(SS_PIN, RST_PIN);
*/

const char* ssid = "...";
const char* password = "...";
const char* host= "localhost";

String tag="ID";

ESP8266WiFiMulti WiFiMulti;

HTTPClient http;

const unsigned long BAUD_RATE=115200;
const unsigned long HTTP_TIMEOUT= 10000;


void initSerial();
void connectWifi(const char* ssid, const char* password);
boolean postRFID(String id) ;

void setup() {
  initSerial();
  delay(100);
  Serial.println("Setup");
  while(!Serial);
 // SPI.begin();         
 // mfrc522.PCD_Init();
  delay(100);
  connectWiFi(ssid,password);
}

void loop() {
/*
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
}

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
Serial.println();

*/

if(postRFID(tag+"666666")){
  Serial.println("GET OUT MY WAY BITCH!!");
}

}


void initSerial(){
  Serial.begin(BAUD_RATE);
  while(!Serial){
    ;
  }
  Serial.println("Serial Pronto");
}

void connectWiFi(const char* ssid, const char* password){
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid,password);
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
}

boolean postRFID(String id){

  
  boolean b=false;
  //String url = "localhost:8080/acesso/solicitarAcesso";
   if((WiFiMulti.run()==WL_CONNECTED)){
    Serial.print("[HTTP] begin...\n");
    http.begin("http://openmyway.herokuapp.com/acesso/solicitarAcessoEntrada/"+id);
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();

    if( httpCode>0){
      Serial.printf("[HTTP] GET ... Response Code: %d\n", httpCode);
      if(httpCode== HTTP_CODE_OK){
        String payload = http.getString();
        Serial.println(payload);
        b=true;
      }
         
    }else{
      Serial.printf("[HTTP] GET... failed, Error: %s \n", http.errorToString(httpCode).c_str());
    }

    http.end();
    delay(10000);
   }
  return b;
}
