#include "DHTesp.h"
#include "Ticker.h"
#include "wifi_VGA_config.h"
#include "string.h"
#include <MPU6050_tockn.h>//biblioteca para o interfaceamento com o MPU6050
#include <Wire.h>//biblioteca para o controle do periferico I2C

//Para teclado PS/2
#include "fabgl.h"

fabgl::PS2Controller PS2Controller;

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

#define MAXLINES 5
#define L 0
#define NL 1
#define VL 2
#define J 3
#define NJ 4
#define VJ 5
#define S 6
#define TS 7
#define VS 8

//Ticker para a leitura da temperatura, umidade e gás
Ticker tempTicker;
//Para leitura dos dados do DHT11
DHTesp dht;
TempAndHumidity newValues;
MPU6050 mpu6050(Wire);//configura o giroscopio/acelerômetro

const char* mqtt_server = "broker.hivemq.com";//broker MQTT

//pin configuration
const int redPin = 14;//fio vermelho
const int greenPin = 19;//fio verde
const int bluePin = 27;//fio azul
const int hsyncPin = 12;//fio cinza
const int vsyncPin = 13;//fio amarelo
int dhtPin = 17;
int gasPin = 36;

//variaveis
bool intruder=false, lock=false;
int gasValue=0, gasMed[4]={0,0,0,0};
char lamps[6]={'0','0','0','0','0','0'}, janelas[2]={'0','0'};
char lines[MAXLINES][100];
char palavras[8][30];
uint8_t lastpos=0, palpos=0;
int anaRes[]={0,0,0,0,0,0,0,5,0};
char texto[24];
char umistring[8], tempstring[8], msgstring[9]={32,32,32,32,32,32,32,32,'\0'};
char senha[]="123456";
float accel=0;
uint8_t i=0, l=1, j=0, h=0, k=0; 
long m=0, lastm=0;

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;

/** Flag if task should run */
bool tasksEnabled = false;

void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
     xTaskResumeFromISR(tempTaskHandle);
  }
}
bool getTemperature() {
  // Medição de temperatura e umidade leva 250 milisegundos
  // medição do sensor pode ser de 2 segundos antes
  newValues = dht.getTempAndHumidity();  
  // Confere se alguma das leituras falhou e termina a funçaão para medir novamente
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }
  
  Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity) + " G:"+String(gasValue));
  
  dtostrf(newValues.humidity, 1, 2, umistring);
  client.publish("Benjamim/umi", umistring);  
  
  dtostrf(newValues.temperature, 1, 2, tempstring);
  client.publish("Benjamim/temp", tempstring);  
  draw_dados();
  return true;
}
void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started");
  while (1) // loop da função de temperatura
  {
    if (tasksEnabled) {
      // Mede temperatura
      getTemperature();
    }
    // função fica em repouso
    vTaskSuspend(NULL);
  }
}
bool initTemp() {
  byte resultValue = 0;
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT iniciado");

  // Inicia a função de medição de temperatura e umidade
  xTaskCreatePinnedToCore(tempTask, "tempTask ", 4000, NULL, 5, &tempTaskHandle, 1);//função de leitura da temperatura e humidade colocada no Core 1 do ESP32
  if (tempTaskHandle == NULL) {
    Serial.println("Falha ao iniciar a função de atualização da temperatura");
    return false;
  } else {
    // Inicia a atualização da medição de dados à cada 20 segundos
    tempTicker.attach(20, triggerGetTemp);
  }
  return true;
}
void setup(){
  vga.init(vga.MODE320x240, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
  //selecting the font
  vga.setFont(Font6x8);  
  vga.clear(6);
  strcpy(lines[0], "bem vindo");
  draw_msg();
  draw_lamp();
  draw_dados();
  Serial.begin(115200);
  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
  
  Serial.println("Booting");

  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  
  bool res;
  res = wm.autoConnect("ESP32_wifi_config"); 
  if(!res) Serial.println("Nao foi possivel conectar, tente novamente");
  else  Serial.println("Wifi conectado");    
  
  client.setServer(mqtt_server, 1883);//configura a conexão com o broker MQTT
  client.setCallback(callback);//configura a interrupção para a recepção de dados via MQTT

  PS2Controller.begin(PS2Preset::KeyboardPort0);
  Wire.begin(26,25); // inicia SDA SCL do I2C para o ESP32
  mpu6050.begin();//inicia o interfaceamento com o MPU6050
  mpu6050.calcGyroOffsets(true);//calcula o offset inicial do giroscópio do MPU6050

    
  
  
}

void callback(char* topic, byte* payload, unsigned int length) {//interrupção da recepção de dado pelo MQTT
  for(int p=0; p<13;p++){
    Serial.write(topic[p]);
  }
  Serial.print("recebido payload:");
  for(int p=0; p<sizeof(payload);p++){
    Serial.write(payload[p]);
  }
  Serial.println();
  if(topic[9]=='p'&&topic[10]=='E'){
    if(topic[11]=='s'&&topic[12]=='p'){
      if(payload[1]=='l'){
        if(payload[2]>='1'&&payload[2]<='6'){
          if(payload[3]=='1')lamps[payload[2]-49]='3';
          if(payload[3]=='0')lamps[payload[2]-49]='0';
        }
        draw_lamp();    
      }
      if(payload[1]=='j'){
        if(payload[2]>='1'&&payload[2]<='6'){
          if(payload[3]=='1'||payload[3]=='0')janelas[payload[2]-49]=payload[3];
        }
        draw_lamp();    
      }
      if(payload[1]=='i'){
        if(payload[2]=='1') intruder=true;
        if(payload[2]=='0') intruder=false;    
      }      
    }
  }
  
}
void draw_msg(){  
  vga.fillRect(28,3,150,52,0); 
  vga.rect(27,2,152,54,7);
  vga.setCursor(30, 5);   
  for(int j=0; j<MAXLINES; j++){
    vga.println(lines[j]);
  }  
}
void draw_lamp(){  
  msgstring[0]='j';
  msgstring[1]=janelas[0];
  msgstring[2]=janelas[1];
  msgstring[3]=32;
  msgstring[4]=32;
  msgstring[5]=32;
  msgstring[6]=32;
  msgstring[7]=32;
  client.publish("Benjamim/msg", msgstring); 
  Serial.println(msgstring);
  delay(20);
  msgstring[0]='l';
  msgstring[1]=lamps[0];
  msgstring[2]=lamps[1];
  msgstring[3]=lamps[2];  
  msgstring[4]=lamps[3];  
  msgstring[5]=lamps[4];  
  msgstring[6]=lamps[5];
  msgstring[7]=32;
  client.publish("Benjamim/msg", msgstring); 
  Serial.println(msgstring);  
  
  lamp(50, 100, lamps[0]-'0', 1);
  lamp(110, 100, lamps[1]-'0', 2);
  lamp(170, 100, lamps[2]-'0', 3);
  lamp(50, 150, lamps[3]-'0', 4);
  lamp(110, 150, lamps[4]-'0', 5);
  lamp(170, 150, lamps[5]-'0', 6);
  janela(255, 40, janelas[0], 1);
  janela(255, 100, janelas[1], 2);  
}
void draw_dados(){ 
  vga.fillRect(28,190,140,40,0);
  vga.setCursor(30,192);
  vga.print("umidade: ");
  vga.println(umistring);
  vga.print("temperatura: ");
  vga.println(tempstring);
  vga.print("Nivel de gas: ");
  vga.println(gasValue);  
}
void copypal(int start, int fim, int pos){
    for(int j=start, k=0;j<=fim;j++, k++){
      palavras[pos][k]=texto[j];
    }
}
void analise_pal(){  
    for(j=0;j<8; j++){   
      //para as lâmpadas 
      if(palavras[j][0]=='l'&&palavras[j][1]=='a'){    
        if(palavras[j][2]=='m'&&palavras[j][3]=='p'){
          anaRes[L]=1;        
        }  
      }  
      if(palavras[j][0]>='1'&&palavras[j][0]<='9'){      
        if(anaRes[L]==1){
          anaRes[NL]=palavras[j][0]-48;
        }  
      }   
      if(palavras[j][0]=='l'&&palavras[j][1]=='i'){
        if(palavras[j][2]=='g'&&palavras[j][3]=='a'){
          if(anaRes[L]==1){
            anaRes[VL]=3;
            if(anaRes[NL]>0)lamps[anaRes[NL]-1]=anaRes[VL]+'0';         
            draw_lamp();
            anaRes[L]=0;
            anaRes[NL]=0;
            anaRes[VL]=0;
            for(h=0;h<8;h++){
             for(k=0;k<sizeof(palavras[j]); k++){
                palavras[h][k]=32;
             }
            }
          }
        }  
      }  
      if(palavras[j][0]=='d'&&palavras[j][1]=='e'){
        if(palavras[j][2]=='s'&&palavras[j][3]=='l'){
          anaRes[VL]=0;
          if(anaRes[NL]>0)lamps[anaRes[NL]-1]=anaRes[VL]+'0';          
          draw_lamp();
          anaRes[L]=0;
          anaRes[NL]=0;
          anaRes[VL]=0;
          for(h=0;h<8;h++){
           for(k=0;k<sizeof(palavras[j]); k++){
              palavras[h][k]=32;
            }
          }
        }  
      }  


      //para as janelas
      if(palavras[j][0]=='j'&&palavras[j][1]=='a'){    
        if(palavras[j][2]=='n'&&palavras[j][3]=='e'){
          anaRes[J]=1;        
        }  
      }  
      if(palavras[j][0]>='1'&&palavras[j][0]<='9'){      
        if(anaRes[J]==1){
          anaRes[NJ]=palavras[j][0]-48;
        }  
      }   
      if(palavras[j][0]=='a'&&palavras[j][1]=='b'){
        if(palavras[j][2]=='r'&&palavras[j][3]=='e'){
          if(anaRes[J]==1){
            anaRes[VJ]=1;
            if(anaRes[NJ]>0)janelas[anaRes[NJ]-1]=anaRes[VJ]+'0';         
            draw_lamp();
            anaRes[J]=0;
            anaRes[NJ]=0;
            anaRes[VJ]=0;
            for(h=0;h<8;h++){
             for(k=0;k<sizeof(palavras[j]); k++){
                palavras[h][k]=32;
             }
            }
          }
        }  
      }  
      if(palavras[j][0]=='f'&&palavras[j][1]=='e'){
        if(palavras[j][2]=='c'&&palavras[j][3]=='h'){
          if(anaRes[J]==1){
            anaRes[VJ]=0;
            if(anaRes[NJ]>0)janelas[anaRes[NJ]-1]=anaRes[VJ]+'0';          
            draw_lamp();
            anaRes[J]=0;
            anaRes[NJ]=0;
            anaRes[VJ]=0;
            for(h=0;h<8;h++){
              for(k=0;k<sizeof(palavras[j]); k++){
                palavras[h][k]=32;
              }
            }
          }  
        } 
      }

      //para a segurança                    
      if(palavras[j][0]=='s'&&palavras[j][1]=='e'){    
        if(palavras[j][2]=='n'&&palavras[j][3]=='h'){
          anaRes[S]=1; 
          if(anaRes[TS]==0&&lock!=true)lock=true;
          if(anaRes[TS]>0)anaRes[TS]--;                   
        }  
      }         
      else if(palavras[j][0]==senha[0]&&palavras[j][1]==senha[1]){
        if(palavras[j][2]==senha[2]&&palavras[j][3]==senha[3]){
          if(palavras[j][4]==senha[4]&&palavras[j][5]==senha[5]){
            if(anaRes[S]==1&&lock!=true){            
              intruder=false;    
              anaRes[S]=0;
              anaRes[TS]=5;
              anaRes[VS]=0;
              for(h=0;h<8;h++){
                for(k=0;k<sizeof(palavras[j]); k++){
                  palavras[h][k]=32;
                }
              }              
            }  
          }  
        }
      }     
      
    } 
    anaRes[VS]=0;  
  
   
}
void loop() {
  
  mpu6050.update();
  accel=mpu6050.getAccZ();  
  vga.setCursor(30,218);
  if(accel>1.8) vga.println("Alguem caiu!!");
  if (!tasksEnabled) {
    // Wait 2 seconds to let system settle down
    delay(2000);
    // Enable task that will read values from the DHT sensor
    tasksEnabled = true;
    if (tempTaskHandle != NULL) {
      vTaskResume(tempTaskHandle);
    }
  }
 
  if (!client.connected()) {
    reconnect();
    client.publish("Benjamim/msg", "sync");
  }
  client.loop();

  auto keyboard = PS2Controller.keyboard();
  static char mode = 'a';
  if (keyboard->virtualKeyAvailable()) {
    bool down;
    auto vk = keyboard->getNextVirtualKey(&down);    
    int c = keyboard->virtualKeyToASCII(vk);
    if (c > -1&&down==false) {  
      
      texto[i]=c;      
      if(l>=MAXLINES)l=1;                  
      strlcpy(lines[l], texto, sizeof(texto));  
      
      if(c==13){ 
        copypal(lastpos,i, palpos);        
        analise_pal();         
        i=0, l++;
        for( j=0;j<sizeof(texto)-1;j++){
          texto[j]=32; 
        }  
        texto[i]=c;
        c=32;
      }  
      if(c==32){
        copypal(lastpos,i, palpos);
        analise_pal();                        
        lastpos=i+1;
        palpos++;
        if(palpos=8) palpos=0;
        for( k=0;k<sizeof(palavras[palpos]); k++){
          palavras[palpos][k]=32;
        }
      }       
      
      if(c!=13) i++;          
      draw_msg();
      c=0;
    }      
  }
  m=millis();
  if(m-lastm>500){
    intruder_loop(intruder, anaRes[TS]);
    gasMed[0]=gasMed[1];
    gasMed[1]=gasMed[2];
    gasMed[2]=gasMed[3];
    gasMed[3]=analogRead(gasPin);  
    gasValue=(gasMed[0]+gasMed[1]+gasMed[2]+gasMed[3])/4;
    dtostrf(gasValue, 1, 2, msgstring);
    client.publish("Benjamim/gas", msgstring);   
    if(gasValue>2050){
      janelas[0]='1';
      janelas[1]='1';
      draw_lamp();
    }
    lastm=m;
  }
  yield();
  
}
