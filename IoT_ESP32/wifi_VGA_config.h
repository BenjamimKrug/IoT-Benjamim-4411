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
  vga.fillRect(x-12,y-13,25,2,cor);
  vga.fillRect(x-13,y-11,27,2,cor);
  vga.fillRect(x-14,y-9,29,8,cor);
  vga.fillRect(x-13,y-1,27,2,cor);
  vga.fillRect(x-12,y+1,25,2,cor);
  vga.fillRect(x-11,y+3,23,2,cor);
  vga.fillRect(x-10,y+5,21,2,cor);
  vga.fillRect(x-9,y+7,19,3,cor);  
  vga.fillRect(x-9,y+10,19, 2, 7);
  vga.line(x-8,y+12,x+8, y+12, 0);
  vga.fillRect(x-8,y+13,17, 2, 7);
  vga.line(x-7,y+15,x+7, y+15, 0);
  vga.fillRect(x-7,y+16,15, 2, 7);
  vga.line(x-5,y+18,x+5, y+18, 0);
  vga.line(x-3,y+19,x+3, y+19, 0);
  
  vga.fillRect(x-24,y+21,53,8,0);
  vga.setCursor(x-24,y+21);
  vga.print("lampada ");
  vga.print(lamp);
}
void janela(uint8_t x, uint8_t y, char state, uint8_t jan){
  vga.fillRect(x-51,y-26, 140, 67, 6);
  if(state=='1'){
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
  if(state=='0'){
    vga.line(x, y-20, x, y+20, 0);
    vga.rect(x-31, y-21, 63, 43, 7);     
    vga.fillRect(x-30, y-20, 30, 41, 7);
    vga.fillRect(x+1, y-20, 30, 41, 7);
  }
  vga.fillRect(x-27,y+25,50,9,0);
  vga.setCursor(x-25,y+25);
  vga.print("Janela ");
  vga.print(jan); 
}

bool on=true, pass=false;
void intruder_loop(bool intr, int tentativas){
  int x=215,y=150;
    if(intr==true){
      vga.fillRect(x-12,y,110,90,6); 
      vga.line(x+38,y+7,x+48,y+7,1);
      vga.line(x+36,y+8,x+50,y+8,1);
      vga.line(x+34,y+9,x+52,y+9,1);
      for(uint8_t yp=10, len=0;yp<30;yp++, len++){
        vga.line(x+33-len/3,y+yp,x+53+len/3,y+yp,1);
      }
      vga.fillRect(x+27,y+29,33,6,7); 
      vga.line(x+28,y+29,x+58,y+29,1);
      vga.line(x+30,y+30,x+56,y+30,1);
      vga.line(x+34,y+31,x+52,y+31,1);   
      vga.line(x+40,y+32,x+46,y+32,1); 

      vga.line(x+28,y+35,x+58,y+35,7);
      vga.line(x+30,y+36,x+56,y+36,7);
      vga.line(x+34,y+37,x+52,y+37,7);   
      vga.line(x+40,y+38,x+46,y+38,7); 
      if(on==true&&pass==false){
        for(uint8_t yp=10, len=0;yp<20;yp++, len++){
          vga.line(x+6,y+yp,x+6+len*2,y+yp,1);
        }
        for(uint8_t yp=20, len=10;yp<31;yp++, len--){
          vga.line(x+6,y+yp,x+6+len*2,y+yp,1);
        }      
        vga.line(x+2,y+18,x+2,y+22,1);
        vga.line(x+3,y+16,x+3,y+24,1);
        vga.line(x+4,y+13,x+4,y+27,1);
        vga.line(x+5,y+11,x+5,y+29,1);
        
        for(uint8_t yp=10, len=10;yp<20;yp++, len--){
          vga.line(x+60+len*2,y+yp,x+80,y+yp,1);
        }
        for(uint8_t yp=20, len=0;yp<31;yp++, len++){
          vga.line(x+60+len*2,y+yp,x+80,y+yp,1);
        } 
        vga.line(x+84,y+18,x+84,y+22,1);
        vga.line(x+83,y+16,x+83,y+24,1);
        vga.line(x+82,y+13,x+82,y+27,1);
        vga.line(x+81,y+11,x+81,y+29,1);      
        on=false;
        pass=true;
      }
      if(on==false&&pass==false)on=true,pass=true;
      pass=false;
      vga.fillRect(x-12,y+40,110,24,0);
      vga.setCursor(x-10,y+40);
      vga.println("ALERTA DE INTRUSO!");
      vga.println("Digite a senha");
      vga.print("voce tem ");
      vga.print(tentativas);
      vga.print(" chances");
      
    }
    if(intr==false) vga.fillRect(x-12,y,110,90,6);   
  
}

#include <WiFi.h>
#include <WiFiClientSecure.h>
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
      client.subscribe("Benjamim/pEsp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 0.5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}
