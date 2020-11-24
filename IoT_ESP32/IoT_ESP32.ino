#include "DHTesp.h"
#include "Ticker.h"
#include "wifi_VGA_config.h"
#include "string.h"
#include <MPU6050_tockn.h>//biblioteca para o interfaceamento com o MPU6050
#include <Wire.h>//biblioteca para o controle do periferico I2C
//Para teclado PS/2
 #include "fabgl.h"

fabgl::PS2Controller PS2Controller;


#define MAXLINES 5
#define L 0   //para saber se é uma lâmpada
#define NL 1  //número da lâmpada
#define VL 2  //valor à ser colocado na lâmpada
#define J 3   //para saber se é uma janela
#define NJ 4  //número da janela
#define S 5   //para saber se está tentando escrever a senha
#define TS 6  //número de tentativas para a senha

//Ticker para a leitura da temperatura, umidade e gás
Ticker tempTicker;
//Para leitura dos dados do DHT11
DHTesp dht;
TempAndHumidity newValues;
//MPU6050 mpu6050(Wire);//configura o giroscopio/acelerômetro

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
bool intruder=false, lock=false;// variáveis do sistema de segurança
int gasValue=0, gasMed[4]={0,0,0,0};//valor do gás e vetor para salvar os valores e calcular a média de 4 valores
char lamps[6]={'0','0','0','0','0','0'}, janelas[2]={'0','0'};//vetores com os estados das lâmpadas e janelas
char lines[MAXLINES][50];//matriz com as linhas do prompt de comandos
char palavras[8][30];
uint8_t lastpos=0, palpos=0;
int anaRes[]={0,0,0,0,0,0,5};
char texto[24];
char umistring[8], tempstring[8], msgstring[9]={32,32,32,32,32,32,32,32,'\0'};
char senha[]="123456";//senha
float accel=0;//valor da aceleração medida pelo MPU6050
uint8_t i=0, l=1, j=0, h=0, k=0; //para os múltiplos fors
long m=0, lastm=0;//para determinar o tempo passado através da função millis()

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
  msgstring[0]='t';
  msgstring[1]=tempstring[0];  
  msgstring[2]=tempstring[1];  
  msgstring[3]=tempstring[2];
  msgstring[4]=tempstring[3];
  msgstring[5]=tempstring[4];
  msgstring[6]=32;
  msgstring[7]=32;  
  msgstring[8]=13;
  client.publish("Benjamim/msg", msgstring);
  
  client.publish("Benjamim/queda"," ");
  client.publish("Benjamim/fumaca"," ");
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
bool initTemp() {//função para inicialização da leitura dos dados do DHT11
  byte resultValue = 0;
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT iniciado");

  // Inicia a função de medição de temperatura e umidade
  xTaskCreatePinnedToCore(tempTask, "tempTask ", 4000, NULL, 5, &tempTaskHandle, 1);//cria função de leitura da temperatura e humidade colocada no Core 1 do ESP32
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
               
  Serial.begin(115200);         //inicia o periférico serial UART2
  initTemp();                   //inicia a leitura dos dados do DHT11
  tasksEnabled = true;          //habilita a task de leitura dos dados do DHT11
  
  Serial.println("Booting");

  WiFi.mode(WIFI_STA);//inicia o wifi no modo estação
  WiFiManager wm;
  
  bool res;
  res = wm.autoConnect("ESP32_wifi_config"); 
  if(!res) Serial.println("Nao foi possivel conectar, tente novamente");
  else  Serial.println("Wifi conectado");    
  
  client.setServer(mqtt_server, 1883);//configura a conexão com o broker MQTT
  client.setCallback(callback);       //configura a interrupção para a recepção de dados via MQTT

  PS2Controller.begin(PS2Preset::KeyboardPort0);//configura a comunicação com o teclado PS/2
  //Wire.begin(26,25);                            //inicia SDA SCL do I2C para o ESP32
 // mpu6050.begin();                              //inicia o interfaceamento com o MPU6050
  //mpu6050.calcGyroOffsets(true);                //calcula o offset inicial do giroscópio do MPU6050
  
  vga.init(vga.MODE320x240, redPin, greenPin, bluePin, hsyncPin, vsyncPin);//inicializa a imagem no monitor VGA
  //selecting the font
  vga.setFont(Font6x8);         //configura a fonte do texto
  vga.clear(6);                 // limpa a tela inteira e preenche com indigo
  strcpy(lines[0], "bem vindo");//copia a mensagem  bem vindo para a primeira linha do prompt de comandos
  draw_msg();                   //desenha o prompt de comando
  draw_lamp();                  //desenha as lâmpadas
  draw_dados();                 //desenha os dados de temperatura, umidade e nível de gás
}

void callback(char* topic, byte* payload, unsigned int length) {//interrupção da recepção de dado pelo MQTT
  Serial.print("recebido payload:");
  for(int p=0; p<sizeof(payload);p++){
    Serial.write(payload[p]);
  }
  Serial.println();
  if(topic[9]=='p'&&topic[10]=='E'){
    if(topic[11]=='s'&&topic[12]=='p'){//se o tópico for Benjamim/pEsp
      if(payload[0]=='s'&&payload[1]=='t'){ 
        if(payload[2]=='a'){          
          draw_lamp(); 
        }
      }
      if(payload[1]=='s'&&payload[2]=='t'){ 
        if(payload[3]=='a'){ draw_lamp(); 
        }
      }           
      
        if(payload[1]=='l'){             //se a mensagem for referente às lâmpadas
          if(payload[2]>='1'&&payload[2]<='6'){
            if(payload[3]=='1')lamps[payload[2]-49]='3';
            if(payload[3]=='0')lamps[payload[2]-49]='0';
          }
          draw_lamp();    //atualiza as mudanças no estado das lâmpadas
        }
        if(payload[1]=='j'){//se a mensagem for referente às janelas
          if(payload[2]>='1'&&payload[2]<='6'){
            if(payload[3]=='1'||payload[3]=='0')janelas[payload[2]-49]=payload[3];
          }
          draw_lamp();    //atualiza as mudanças no estado das janelas
        }
        if(payload[1]=='i'){                 //se a mensagem for referente a um intruso
          if(payload[2]=='1') intruder=true; //há um intruso na casa
          if(payload[2]=='0') intruder=false;//há um intruso na casa
        }    
        
    }
  }
  
}
void draw_msg(){  
  vga.fillRect(28,3,150,52,0);    //preenche o fundo do prompt de comandos com preto
  vga.rect(27,2,152,54,7);        //desenha a borda do prompt de comandos
  vga.setCursor(30, 5);           //coloca o curso na posição (30,5) para a escrita
  for(int j=0; j<MAXLINES; j++){
    vga.println(lines[j]);        //desenha as linhas do prompt de comandos
  }  
}
void draw_lamp(){   
  msgstring[0]='j';//a mensagem se refere ao estado das lâmpadas
  msgstring[1]=janelas[0];
  msgstring[2]=janelas[1];
  msgstring[3]=32;  
  msgstring[4]=32;  
  msgstring[5]=32;  
  msgstring[6]=32;
  msgstring[7]=13; 
  client.publish("Benjamim/msg", msgstring); //publica o estado das janelas
  Serial.println(msgstring);
  delay(20);
  msgstring[0]='l';//a mensagem se refere ao estado das lâmpadas
  msgstring[1]=lamps[0];
  msgstring[2]=lamps[1];
  msgstring[3]=lamps[2];  
  msgstring[4]=lamps[3];  
  msgstring[5]=lamps[4];  
  msgstring[6]=lamps[5];
  msgstring[7]=13;
  client.publish("Benjamim/msg", msgstring); //publica o estado das lâmpadas
  Serial.println(msgstring); 


  //desenha as lâmpadas
  lamp(50, 100, lamps[0]-'0', 1);
  lamp(110, 100, lamps[1]-'0', 2);
  lamp(170, 100, lamps[2]-'0', 3);
  lamp(50, 150, lamps[3]-'0', 4);
  lamp(110, 150, lamps[4]-'0', 5);
  lamp(170, 150, lamps[5]-'0', 6);
  //desenha as janelas
  janela(255, 40, janelas[0], 1);
  janela(255, 100, janelas[1], 2);  
}
void draw_dados(){ //imprime os dados medidos na tela
  vga.fillRect(28,190,140,48,0);
  vga.setCursor(30,192);
  vga.print("umidade: ");
  vga.print(umistring);
  vga.println(" %");
  vga.print("temperatura: ");
  vga.print(tempstring);
  vga.println(" C");
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
          anaRes[L]=1;        //é uma lâmpada
        }  
      }  
      if(palavras[j][0]>='1'&&palavras[j][0]<='9'){      
        if(anaRes[L]==1){
          anaRes[NL]=palavras[j][0]-48;//se for uma lâmpada este é o número da lâmpada à qual se refere
        }  
      }   
      if(palavras[j][0]=='l'&&palavras[j][1]=='i'){
        if(palavras[j][2]=='g'&&palavras[j][3]=='a'){
          if(anaRes[L]==1){
            anaRes[VL]=3;
            if(anaRes[NL]>0)lamps[anaRes[NL]-1]=anaRes[VL]+'0'; //coloca        
            draw_lamp();  //atualiza as mudanças no estado das lâmpadas
            anaRes[L]=0;  //limpa a análise
            anaRes[NL]=0;
            anaRes[VL]=0;
            for(h=0;h<8;h++){
             for(k=0;k<sizeof(palavras[j]); k++){//esvazia a matriz com as palavras para análise
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
          draw_lamp();  //atualiza as mudanças no estado das lâmpadas
          anaRes[L]=0;  //limpa a análise
          anaRes[NL]=0;
          anaRes[VL]=0;
          for(h=0;h<8;h++){
           for(k=0;k<sizeof(palavras[j]); k++){//esvazia a matriz com as palavras para análise
              palavras[h][k]=32;
            }
          }
        }  
      }  


      //para as janelas
      if(palavras[j][0]=='j'&&palavras[j][1]=='a'){    
        if(palavras[j][2]=='n'&&palavras[j][3]=='e'){
          anaRes[J]=1;//é uma janela
        }  
      }  
      if(palavras[j][0]>='1'&&palavras[j][0]<='9'){      
        if(anaRes[J]==1){
          anaRes[NJ]=palavras[j][0]-48;//se for uma janela este é o número da janela à qual se refere
        }  
      }   
      if(palavras[j][0]=='a'&&palavras[j][1]=='b'){
        if(palavras[j][2]=='r'&&palavras[j][3]=='i'){
          if(anaRes[J]==1){
            if(anaRes[NJ]>0)janelas[anaRes[NJ]-1]='1';         
            draw_lamp();  //atualiza as mudanças no estado das janelas
            anaRes[J]=0;  //limpa a análise
            anaRes[NJ]=0; 
            for(h=0;h<8;h++){
             for(k=0;k<sizeof(palavras[j]); k++){//esvazia a matriz com as palavras para análise
                palavras[h][k]=32;
             }
            }
          }
        }  
      }  
      if(palavras[j][0]=='f'&&palavras[j][1]=='e'){
        if(palavras[j][2]=='c'&&palavras[j][3]=='h'){
          if(anaRes[J]==1){
            if(anaRes[NJ]>0)janelas[anaRes[NJ]-1]='0';          
            draw_lamp();  //atualiza as mudanças no estado das janelas
            anaRes[J]=0;  //limpa a análise
            anaRes[NJ]=0;
            for(h=0;h<8;h++){
              for(k=0;k<sizeof(palavras[j]); k++){//esvazia a matriz com as palavras para análise
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
              intruder=false;//a senha está correta, não é um intruso
              anaRes[S]=0;  //limpa a análise
              anaRes[TS]=5;
              snprintf(msgstring, sizeof(msgstring),"i0      \n");
              client.publish("Benjamim/msg", msgstring);
              for(h=0;h<8;h++){
                for(k=0;k<sizeof(palavras[j]); k++){//esvazia a matriz com as palavras para análise
                  palavras[h][k]=32;
                }
              }              
            }  
          }  
        }
      }             
    }   
  
   
}
void loop() {
  
  //mpu6050.update();//atualiza os dados do MPU6050
  //accel=mpu6050.getAccZ();  
  vga.setCursor(30,218);
//  if(accel>1.8) {
//    vga.println("Alguem caiu!!!");//caso a aceleração medida pelo MPU6050 seja maior que 1,8Gs quer dizer que alguém provavelmente caiu
//    client.publish("Benjamim/queda","Alguém Caiu!!!");
//  }
  
  if (!tasksEnabled) {    
    delay(2000);// espera 2 segundos para deixar o sistema se establecer
    tasksEnabled = true;//habilita a task para a leitura dos dados do DHT11
    if (tempTaskHandle != NULL) {
      vTaskResume(tempTaskHandle);
    }
  }
 
  if (!client.connected()) {
    reconnect();//reconecta com o servidor MQTT
    client.publish("Benjamim/msg", "sync");//envia a mensagem para a sincronizar os dos microcontroladores
  }
  client.loop();//loop de keep alive do MQTT

  auto keyboard = PS2Controller.keyboard();
  static char mode = 'a';
  if (keyboard->virtualKeyAvailable()) {
    bool down;
    auto vk = keyboard->getNextVirtualKey(&down);    
    int c = keyboard->virtualKeyToASCII(vk);
    if (c > -1&&down==false) {      //caso uma tecla tenha sido pressionada  
      texto[i]=c;                   //coloca a tecla pressionada no vetor texto
      if(l>=MAXLINES)l=1;           //caso tenha excedido o limite das linhas do prompt de comando, retorna à primeira linha            
      strlcpy(lines[l], texto, sizeof(texto));       
      if(c==13){                    //caso a tecla pressionada tenha sido ENTER
        copypal(lastpos,i, palpos); //copia os dados para a matriz palavra   
        analise_pal();              //analisa as palavras já digitadas
        i=0, l++;
        for( j=0;j<sizeof(texto)-1;j++){
          texto[j]=32;              //esvazia o vetor texto
        }  
        texto[i]=c;
        c=32;                       //coloca o primeiro caracter da próxima linha como um espaço
      }  
      if(c==32){                    //caso a tecla pressionada tenha sido SPACEBAR
        copypal(lastpos,i, palpos); //copia os dados para a matriz palavra
        analise_pal();              //analisa as palavras já digitadas          
        lastpos=i+1;
        palpos++;
        if(palpos=8) palpos=0;
        for( k=0;k<sizeof(palavras[palpos]); k++){
          palavras[palpos][k]=32;
        }
      }       
      if(c!=13) i++;                //caso a tecla pressionada não tenha sido ENTER
      draw_msg();                   //atualiza o prompt de comandos
      c=0;                          
    }      
  }
  m=millis();
  if(m-lastm>500){//caso tenha passado 0,5 segundos entre a última medição
    intruder_loop(intruder, anaRes[TS]);//loop para o aviso de intruso
    gasMed[0]=gasMed[1];                                  //desloca os dados de valor de gás
    gasMed[1]=gasMed[2];                                  //desloca os dados de valor de gás
    gasMed[2]=gasMed[3];                                  //desloca os dados de valor de gás
    gasMed[3]=analogRead(gasPin);                         //novo valor medido de gás
    gasValue=(gasMed[0]+gasMed[1]+gasMed[2]+gasMed[3])/4; //cálcula a média do valor de gás
    dtostrf(gasValue, 1, 2, msgstring);                   //converte um valor float para uma string
    client.publish("Benjamim/gas", msgstring);            //publica o valor de gás para ser mostrado no dashboard do NodeRed    
    if(gasValue>2250){//se o valor de gás for alto demais provavelmente tem um vazamento de gás ou algum incêndio gerando fumaça
      vga.setCursor(30,226);
      vga.println("Fumaca detectada");
      client.publish("Benjamim/fumaca","Fumaça detectada");
      janelas[0]='1'; //abre as janelas
      janelas[1]='1'; //abre as janelas
      draw_lamp();    //atualiza o estado da janelas
    }
    lastm=m;//salva o valor atual de millis como o valor antigo
  }
  yield();
  
}
