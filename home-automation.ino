// Libraries

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiUdp.h>

//----------------------------------------------------------------------------------------------------------------

// Initial Settings

const char* auth = "4ohHdNHHqTTjSjNUO8p2lYLhSlEiIdkS";
const char* ssid = "VIVOFIBRA-9B6D";
const char* password = "EAEA8F9B6D";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP , "a.st1.ntp.br", -3 * 3600 , 60000);

//----------------------------------------------------------------------------------------------------------------

// Initial Variables

#define led1 16
#define led2 5
#define led3 4
#define led4 0
#define sensorSom 14
#define pinoLDR A0

#define BLYNK_PRINT Serial

int hourOn = 17;
int minuteOn = 05;

int hoursOn = 03;
int minutesOn = 00;

int hourOff = hourOn + hoursOn;
int minuteOff = minuteOn + minutesOn;

#define finalDelay 100
#define durationClap 200
#define intervalBetweenClap 600

int valueLDR;
int pinVirtualV0;
int lap;

int quantidadePalmas = 0;
long momentoPalma = 0;
long waitClap = 0;

//----------------------------------------------------------------------------------------------------------------

// Functions

BLYNK_WRITE(V0){
  pinVirtualV0 = param.asInt();
  if (pinVirtualV0 == HIGH) {
    digitalWrite(led1, HIGH);
  }else{
    digitalWrite(led1, LOW);
  }
}

int horaAtual () {
  timeClient.begin();
  timeClient.update();
  int hora =  timeClient.getHours();
  timeClient.end();
  return hora; 
}

int minutoAtual () {
  timeClient.begin();
  timeClient.update();
  int minuto =  timeClient.getMinutes();
  timeClient.end();
  return minuto;
}

void timer() {

  if (horaAtual() >=  hourOn && horaAtual() <= hourOff) {
    if (minutoAtual() >= minuteOn && minutoAtual() <= minuteOff) {
        digitalWrite(led2, HIGH); 
        Serial.println("HIGH");
    }
  }

  if (horaAtual() >= hourOff) {
    if (minutoAtual() >= minuteOff) {
        digitalWrite(led2, LOW);
        Serial.println("LOW");
    }
  }
}

void ldr(){
  valueLDR = map(analogRead(pinoLDR), 0, 1024, 0, 100);
}

void clap(){
  
  int leituraSom = digitalRead(sensorSom);

  if (leituraSom == LOW) {
    if(momentoPalma == 0){
      momentoPalma = waitClap = millis();
      quantidadePalmas++;
    } else if ((millis() - momentoPalma) >= durationClap){
      momentoPalma = 0;    
    }
  }
  
  if (((millis() - waitClap) > intervalBetweenClap) && (quantidadePalmas !=0)) {
    if (quantidadePalmas == 2) {
      digitalWrite(led1, !digitalRead(led1));
      Blynk.virtualWrite(V0, digitalRead(led1));
    }
    delay(finalDelay);
    quantidadePalmas = 0;
  }
}

void jardim() {

  if (valueLDR >= 50){
    digitalWrite(led3, LOW);
  }

  if (valueLDR < 50) {
    digitalWrite(led3, HIGH);
  }
}

//----------------------------------------------------------------------------------------------------------------

// Setup

void setup() {

  Serial.begin(9600);
  Blynk.begin(auth, ssid, password);
  pinMode(sensorSom, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(pinoLDR, INPUT);
  
}

//----------------------------------------------------------------------------------------------------------------

// Loop

void loop() {

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);

  lap++;

  switch(lap){

    case 1900:
      ldr();
      Blynk.virtualWrite(V1, valueLDR);
      jardim();
    break;

    case 2850:
      timer();
    break;

    case 3000:
      lap = 0;
    break;

    default :
      Blynk.run();
      clap();
    break;
  }
}
