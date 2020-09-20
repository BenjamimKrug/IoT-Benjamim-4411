//Para VGA
#include <ESP32Lib.h>
#include <Ressources/Font6x8.h>
VGA3Bit vga;
/*
 Cores do vga:
  0-preto
  1-vermelho
  2-verde
  3-amarelo
  4-azul
  5-rosa
  6-indigo
  7-branco
 */
 void lamp(uint16_t x, uint16_t y, uint8_t cor, uint8_t lamp){
  vga.line(x-3,y-18,x+3,y-18,cor);  
  vga.line(x-6,y-17,x+6,y-17,cor);
  vga.line(x-8,y-16,x+8,y-16,cor);
  vga.line(x-10,y-15,x+10,y-15,cor);
  vga.line(x-11,y-14,x+11,y-14,cor);
  vga.line(x-12,y-13,x+12,y-13,cor);
  vga.line(x-12,y-12,x+12,y-12,cor);
  vga.line(x-13,y-11,x+13,y-11,cor);
  vga.line(x-13,y-10,x+13,y-10,cor);
  vga.line(x-14,y-9,x+14,y-9,cor);
  vga.line(x-14,y-8,x+14,y-8,cor);
  vga.line(x-14,y-7,x+14,y-7,cor);
  vga.line(x-14,y-6,x+14,y-6,cor);
  vga.line(x-14,y-5,x+14,y-5,cor);
  vga.line(x-14,y-4,x+14,y-4,cor);
  vga.line(x-14,y-3,x+14,y-3,cor);
  vga.line(x-14,y-2,x+14,y-2,cor);
  vga.line(x-13,y-1,x+13,y-1,cor);
  vga.line(x-13,y,x+13,y,cor);
  vga.line(x-12,y+1,x+12,y+1,cor);
  vga.line(x-12,y+2,x+12,y+2,cor);
  vga.line(x-11,y+3,x+11,y+3,cor);
  vga.line(x-11,y+4,x+11,y+4,cor);
  vga.line(x-10,y+5,x+10,y+5,cor);
  vga.line(x-10,y+6,x+10,y+6,cor);
  vga.line(x-9,y+7,x+9,y+7,cor);
  vga.line(x-9,y+8,x+9,y+8,cor);
  vga.line(x-9,y+9,x+9,y+9,cor);
  
  vga.fillRect(x-9,y+10,19, 2, 7);
  vga.line(x-8,y+12,x+8, y+12, 0);
  vga.fillRect(x-8,y+13,17, 2, 7);
  vga.line(x-7,y+15,x+7, y+15, 0);
  vga.fillRect(x-7,y+16,15, 2, 7);
  vga.line(x-5,y+18,x+5, y+18, 0);
  vga.line(x-3,y+19,x+3, y+19, 0);
  vga.setCursor(x-24,y+20);
  vga.print("lampada ");
  vga.print(lamp);
}
void janela(uint8_t x, uint8_t y, bool state){
  if(state==true){
    vga.fillRect(x-30, y-20, 61, 41, 0);
    vga.rect(x-31, y-21, 63, 43, 7);   
    vga.line(x-51,y-24, x-31, y-21, 7);
    vga.line(x-51,y-23, x-31, y-20, 7);
    vga.line(x-51,y-22, x-31, y-19, 7);
    vga.line(x-51,y-21, x-31, y-18, 7);    
    vga.fillRect(x-51,y-20,20,41,7);      
    vga.line(x-51,y+21, x-31, y+18, 7);
    vga.line(x-51,y+22, x-31, y+19, 7);
    vga.line(x-51,y+23, x-31, y+20, 7);    
    vga.line(x-51,y+24, x-31, y+21, 7);       
    vga.line(x+31,y-21, x+51, y-24, 7);
    vga.line(x+31,y-20, x+51, y-23, 7);
    vga.line(x+31,y-19, x+51, y-22, 7);
    vga.line(x+31,y-18, x+51, y-21, 7);
    vga.fillRect(x+32,y-20,20,41,7);
    vga.line(x+31,y+18, x+51, y+21, 7);
    vga.line(x+31,y+19, x+51, y+22, 7);
    vga.line(x+31,y+20, x+51, y+23, 7);    
    vga.line(x+31,y+21, x+51, y+24, 7);    
  }  
  if(state==false){
    vga.line(x, y-20, x, y+20, 0);
    vga.rect(x-31, y-21, 63, 43, 7);     
    vga.fillRect(x-30, y-20, 30, 41, 7);
    vga.fillRect(x+1, y-20, 30, 41, 7);
  }
}


#include <WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>//biblioteca para o MQTT
WiFiServer server(80);

WiFiClient espclient;
PubSubClient client(espclient);

void reconnect() {
  // Repete até que a conexão seja reestabelicida
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");      
      // ... and resubscribe
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 0.5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}
