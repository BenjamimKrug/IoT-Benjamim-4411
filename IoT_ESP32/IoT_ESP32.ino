#include "DHTesp.h"
#include "Ticker.h"
#include "wifi_VGA_config.h"
#include "string.h"

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

//Ticker para a leitura da temperatura, humidade e gás
Ticker tempTicker;
//Para leitura dos dados do DHT11
DHTesp dht;
TempAndHumidity newValues;


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
int gasValue=0;
uint8_t lamps[6]={3,0,0,3,0,0};
char lines[MAXLINES][100];
char palavras[8][30];
uint8_t lastpos=0, palpos=0;
int anaRes[8]={0,0,0,0,0,0,0,0};
char texto[24];
char umistring[8];
char tempstring[8];
uint8_t i=0, l=1;

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
  client.publish("Benjamim/hum", umistring);  
  delay(2);
  
  dtostrf(newValues.temperature, 1, 2, tempstring);
  client.publish("Benjamim/temp", tempstring);  
  delay(2);
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
void setup()
{
  Serial.begin(115200);
  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
  
  Serial.println("Booting");

  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  bool res;
  res = wm.autoConnect("ESP32_wifi_config"); 
  if(!res) {
      Serial.println("Nao foi possivel conectar, tente novamente");
  } 
  else {  
      Serial.println("Wifi conectado");      
  }
  
  client.setServer(mqtt_server, 1883);//configura a conexão com o broker MQTT
  client.setCallback(callback);//configura a interrupção para a recepção de dados via MQTT

  PS2Controller.begin(PS2Preset::KeyboardPort0);
  
  vga.init(vga.MODE320x240, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
  //selecting the font
  vga.setFont(Font6x8);  
  vga.clear(6);
  strcpy(lines[0], "bem vindo");
  draw_msg();
  draw_lamp();
  draw_dados();
}


void callback(char* topic, byte* payload, unsigned int length) {//interrupção d recepção de dado pelo MQTT
  
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
  lamp(50, 100, lamps[0], 1);
  lamp(110, 100, lamps[1], 2);
  lamp(170, 100, lamps[2], 3);
  lamp(50, 150, lamps[3], 4);
  lamp(110, 150, lamps[4], 5);
  lamp(170, 150, lamps[5], 6);
  janela(240, 40, false);
  
}
void draw_dados(){ 
  vga.fillRect(28,198,200,50,6);
  vga.setCursor(30,200);
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
   Serial.println("analise"); 
   for(int j=0;j<8; j++){
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
        anaRes[VL]=3;
        if(anaRes[NL]>0)lamps[anaRes[NL]-1]=anaRes[VL];         
        draw_lamp();
        anaRes[L]=0;
        anaRes[NL]=0;
        anaRes[VL]=0;
        for(int j=0;j<8;j++){
          for(int k=0;k<sizeof(palavras[j]); k++){
            palavras[j][k]=32;
          }
        }
      }  
    }  
    if(palavras[j][0]=='d'&&palavras[j][1]=='e'){
      if(palavras[j][2]=='s'&&palavras[j][3]=='l'){
        anaRes[VL]=0;
        if(anaRes[NL]>0)lamps[anaRes[NL]-1]=anaRes[VL];          
        draw_lamp();
        anaRes[L]=0;
        anaRes[NL]=0;
        anaRes[VL]=0;
        for(int j=0;j<8;j++){
          for(int k=0;k<sizeof(palavras[j]); k++){
            palavras[j][k]=32;
          }
        }
      }  
    }        
  }  
}
void loop() {
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
  }
  client.loop();

  auto keyboard = PS2Controller.keyboard();
  static char mode = 'a';
  if (keyboard->virtualKeyAvailable()) {
    bool down;
    auto vk = keyboard->getNextVirtualKey(&down);    
    int c = keyboard->virtualKeyToASCII(vk);
    if(vk==122&&down==false){
      Serial.println(vk);
    }
    if (c > -1&&down==false) {  
      
      if(c==13){        
        
        i=0, l++;
        for(int j=0;j<sizeof(texto)-1;j++){
          texto[j]=32; 
        }  
      }    
      if(i==24){
        i=0, l++;
        for(int j=0;j<sizeof(texto)-1;j++){
          texto[j]=32; 
        }         
      }
      if(l>=MAXLINES)l=1;
      texto[i]=c;         
      if(c==32){
        copypal(lastpos,i, palpos);        
        Serial.println(palavras[palpos]);     
        analise_pal(); 
        lastpos=i+1;
        palpos++;
        if(palpos=8) palpos=0;
        for(int k=0;k<sizeof(palavras[palpos]); k++){
          palavras[palpos][k]=32;
        }
      }             
      strlcpy(lines[l], texto, sizeof(texto));    
      if(c!=13) i++;          
      draw_msg();
    }      
  }
  gasValue=analogRead(gasPin); 
  char msg[8];
  dtostrf(gasValue, 1, 2, msg);
  client.publish("Benjamim/gas", msg);
  client.publish("Benjamim/msg", texto);   
  yield();
}
