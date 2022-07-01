/*
 By:Farkad Adnan
 E-mail: farkad.hpfa95@gmail.com
 inst : farkadadnan
 #farkadadnan , #farkad_adnan , فرقد عدنان#
 FaceBook: كتاب عالم الاردوينو
 inst : arduinobook
 #كتاب_عالم_الاردوينو  #كتاب_عالم_الآردوينو 
 */ 

  
#include <Servo.h> 
#include <Oscillator.h>
#include <EEPROM.h>
#include <BatReader.h>
#include <US.h>
#include <LedMatrix.h>
#include <EnableInterrupt.h> 
#include <OttoSerialCommand.h>
OttoSerialCommand SCmd; 
 #include <Otto.h>
Otto Otto;   

/*
         --------------- 
        |     O   O     |
        |---------------|
YR 3==> |               | <== YL 2
         --------------- 
            ||     ||
            ||     ||
RR 5==>   -----   ------  <== RL 4
         |-----   ------|

*/
  #define PIN_YL 2 //servo[0]
  #define PIN_YR 3 //servo[1]
  #define PIN_RL 4 //servo[2]
  #define PIN_RR 5 //servo[3]

//---Otto Buttons
#define PIN_SecondButton 6
#define PIN_ThirdButton 7


const char programID[]="Otto_todo"; 

const char name_fac='$'; 
const char name_fir='#'; 

int T=1000;              
int moveId=0;         
int moveSize=15;      

volatile int MODE=0; 

volatile bool buttonPushed=false;  
volatile bool buttonAPushed=false; 
volatile bool buttonBPushed=false; 
unsigned long previousMillis=0;
int randomDance=0;
int randomSteps=0;
bool obstacleDetected = false;
void setup(){
  Serial.begin(115200);  
  pinMode(PIN_SecondButton,INPUT);
  pinMode(PIN_ThirdButton,INPUT);
    Otto.init(PIN_YL,PIN_YR,PIN_RL,PIN_RR,true);
    randomSeed(analogRead(A6));
  enableInterrupt(PIN_SecondButton, secondButtonPushed, RISING);
  enableInterrupt(PIN_ThirdButton, thirdButtonPushed, RISING);

  SCmd.addCommand("S", receiveStop);    
  SCmd.addCommand("L", receiveLED);       
  SCmd.addCommand("T", recieveBuzzer);   
  SCmd.addCommand("M", receiveMovement);
  SCmd.addCommand("H", receiveGesture);  
  SCmd.addCommand("K", receiveSing);     
  SCmd.addCommand("C", receiveTrims);    
  SCmd.addCommand("G", receiveServo);    
  SCmd.addCommand("D", requestDistance);
  SCmd.addCommand("N", requestNoise);
  SCmd.addCommand("B", requestBattery);
  SCmd.addCommand("I", requestProgramId);
  SCmd.addDefaultHandler(receiveStop);
  Otto.sing(S_connection);
  Otto.home();
  delay(50);

  for(int i=0; i<2; i++){
      for (int i=0;i<8;i++){
        if(buttonPushed){break;}  
        Otto.putAnimationMouth(littleUuh,i);
        delay(150);
      }
  }


  if(!buttonPushed){ 
    Otto.putMouth(smile);
    Otto.sing(S_happy);
    delay(200);
  }
  if (EEPROM.read(5)==name_fir){ 
    if(!buttonPushed){  
        Otto.jump(1,700);
        delay(200); 
    }
    if(!buttonPushed){  
        Otto.shakeLeg(1,T,1); 
    }  
    if(!buttonPushed){ 
        Otto.putMouth(smallSurprise);
        Otto.swing(2,800,20);  
        Otto.home();
    }  
  }
  if(!buttonPushed){ 
    Otto.putMouth(happyOpen);
  }
  previousMillis = millis();
}


void loop() {
       if(obstacleDetected){
              Otto.sing(S_surprise);
              Otto.playGesture(OttoFretful);
              Otto.sing(S_fart3);
              Otto.walk(2,1300,-1);
              Otto.turn(2,1000,-1);               
            delay(50);
            obstacleDetector();
            }       
        else{
            Otto.walk(1,1000,1);
            obstacleDetector();
        }          
  } 
void secondButtonPushed(){ 
    buttonAPushed=true;
    if(!buttonPushed){
        buttonPushed=true;
        Otto.putMouth(smallSurprise);
    }    
}
void thirdButtonPushed(){ 
    buttonBPushed=true;
    if(!buttonPushed){
        buttonPushed=true;
        Otto.putMouth(smallSurprise);
    }
}
void obstacleDetector(){
   int distance = Otto.getDistance();
        if(distance<15){
          obstacleDetected = true;
        }else{
          obstacleDetected = false;
        }
}
void receiveStop(){

    sendAck();
    Otto.home();
    sendFinalAck();
}
void receiveLED(){  
    sendAck();
    Otto.home();
    unsigned long int matrix;
    char *arg;
    char *endstr;
    arg=SCmd.next();
    if (arg != NULL) {
      matrix=strtoul(arg,&endstr,2);  
      Otto.putMouth(matrix,false);
    }else{
      Otto.putMouth(xMouth);
      delay(2000);
      Otto.clearMouth();
    }
    sendFinalAck();
}
void recieveBuzzer(){
    sendAck();
    Otto.home(); 
    bool error = false; 
    int frec;
    int duration; 
    char *arg; 
    arg = SCmd.next(); 
    if (arg != NULL) { frec=atoi(arg); }
    else {error=true;}
    arg = SCmd.next(); 
    if (arg != NULL) { duration=atoi(arg); } 
    else {error=true;}
    if(error==true){
      Otto.putMouth(xMouth);
      delay(2000);
      Otto.clearMouth();
    }else{ 

      Otto._tone(frec, duration, 1);   
    }
    sendFinalAck();
}
void receiveTrims(){  
    sendAck();
    Otto.home(); 
    int trim_YL,trim_YR,trim_RL,trim_RR;
    bool error = false;
    char *arg;
    arg=SCmd.next();
    if (arg != NULL) { trim_YL=atoi(arg); }   
    else {error=true;}
    arg = SCmd.next(); 
    if (arg != NULL) { trim_YR=atoi(arg); }  
    else {error=true;}
    arg = SCmd.next(); 
    if (arg != NULL) { trim_RL=atoi(arg); }  
    else {error=true;}
    arg = SCmd.next(); 
    if (arg != NULL) { trim_RR=atoi(arg); }   
    else {error=true;}
    if(error==true){
      Otto.putMouth(xMouth);
      delay(2000);
      Otto.clearMouth();
    }else{
      Otto.setTrims(trim_YL, trim_YR, trim_RL, trim_RR);
      Otto.saveTrimsOnEEPROM(); 
    } 
    sendFinalAck();
}
void receiveServo(){  

    sendAck(); 
    moveId = 30;
    bool error = false;
    char *arg;
    int servo_YL,servo_YR,servo_RL,servo_RR;
    arg=SCmd.next();
    if (arg != NULL) { servo_YL=atoi(arg); }    
    else {error=true;}
    arg = SCmd.next(); 
    if (arg != NULL) { servo_YR=atoi(arg); } 
    else {error=true;}

    arg = SCmd.next(); 
    if (arg != NULL) { servo_RL=atoi(arg); } 
    else {error=true;}
    arg = SCmd.next(); 
    if (arg != NULL) { servo_RR=atoi(arg); }  
    else {error=true;}
    if(error==true){
      Otto.putMouth(xMouth);
      delay(2000);
      Otto.clearMouth();

    }else{ 
      int servoPos[4]={servo_YL, servo_YR, servo_RL, servo_RR}; 
      Otto._moveServos(200, servoPos);        
    }
    sendFinalAck();
}
void receiveMovement(){
    sendAck();
    if (Otto.getRestState()==true){
        Otto.setRestState(false);
    }  
    char *arg; 
    arg = SCmd.next(); 
    if (arg != NULL) {moveId=atoi(arg);}
    else{
      Otto.putMouth(xMouth);
      delay(2000);
      Otto.clearMouth();
      moveId=0; 
    }
    
    arg = SCmd.next(); 
    if (arg != NULL) {T=atoi(arg);}
    else{
      T=1000;
    }

    arg = SCmd.next(); 
    if (arg != NULL) {moveSize=atoi(arg);}
    else{
      moveSize =15;
    }
}
void move(int moveId){
  bool manualMode = false;
  switch (moveId) {
    case 0:
      Otto.home();
      break;
    case 1: 
      Otto.walk(1,T,1);
      break;
    case 2:
      Otto.walk(1,T,-1);
      break;
    case 3: //M 3 1000 
      Otto.turn(1,T,1);
      break;
    case 4: //M 4 1000 
      Otto.turn(1,T,-1);
      break;
    case 5: //M 5 1000 30 
      Otto.updown(1,T,moveSize);
      break;
    case 6: //M 6 1000 30
      Otto.moonwalker(1,T,moveSize,1);
      break;
    case 7: //M 7 1000 30
      Otto.moonwalker(1,T,moveSize,-1);
      break;
    case 8: //M 8 1000 30
      Otto.swing(1,T,moveSize);
      break;
    case 9: //M 9 1000 30 
      Otto.crusaito(1,T,moveSize,1);
      break;
    case 10: //M 10 1000 30 
      Otto.crusaito(1,T,moveSize,-1);
      break;
    case 11: //M 11 1000 
      Otto.jump(1,T);
      break;
    case 12: //M 12 1000 30 
      Otto.flapping(1,T,moveSize,1);
      break;
    case 13: //M 13 1000 30
      Otto.flapping(1,T,moveSize,-1);
      break;
    case 14: //M 14 1000 20
      Otto.tiptoeSwing(1,T,moveSize);
      break;
    case 15: //M 15 500 
      Otto.bend(1,T,1);
      break;
    case 16: //M 16 500 
      Otto.bend(1,T,-1);
      break;
    case 17: //M 17 500 
      Otto.shakeLeg(1,T,1);
      break;
    case 18: //M 18 500 
      Otto.shakeLeg(1,T,-1);
      break;
    case 19: //M 19 500 20
      Otto.jitter(1,T,moveSize);
      break;
    case 20: //M 20 500 15
      Otto.ascendingTurn(1,T,moveSize);
      break;
    default:
        manualMode = true;
      break;
  }

  if(!manualMode){
    sendFinalAck();
  }      
}

void receiveGesture(){
    sendAck();
    Otto.home();  
    int gesture = 0;
    char *arg; 
    arg = SCmd.next(); 
    if (arg != NULL) {gesture=atoi(arg);}
    else 
    {
      Otto.putMouth(xMouth);
      delay(2000);
      Otto.clearMouth();
    }

    switch (gesture) {
      case 1: 
        Otto.playGesture(OttoHappy);
        break;
      case 2: 
        Otto.playGesture(OttoSuperHappy);
        break;
      case 3: 
        Otto.playGesture(OttoSad);
        break;
      case 4: 
        Otto.playGesture(OttoSleeping);
        break;
      case 5: 
        Otto.playGesture(OttoFart);
        break;
      case 6: 
        Otto.playGesture(OttoConfused);
        break;
      case 7: //H 7 
        Otto.playGesture(OttoLove);
        break;
      case 8: //H 8 
        Otto.playGesture(OttoAngry);
        break;
      case 9: //H 9  
        Otto.playGesture(OttoFretful);
        break;
      case 10: //H 10
        Otto.playGesture(OttoMagic);
        break;  
      case 11: //H 11
        Otto.playGesture(OttoWave);
        break;   
      case 12: //H 12
        Otto.playGesture(OttoVictory);
        break; 
      case 13: //H 13
        Otto.playGesture(OttoFail);
        break;         
      default:
        break;
    }
    sendFinalAck();
}
void receiveSing(){
    sendAck();
    Otto.home();     
    int sing = 0;
    char *arg; 
    arg = SCmd.next(); 
    if (arg != NULL) {sing=atoi(arg);}
    else 
    {
      Otto.putMouth(xMouth);
      delay(2000);
      Otto.clearMouth();
    }

    switch (sing) {
      case 1: //K 1 
        Otto.sing(S_connection);
        break;
      case 2: //K 2 
        Otto.sing(S_disconnection);
        break;
      case 3: //K 3 
        Otto.sing(S_surprise);
        break;
      case 4: //K 4 
        Otto.sing(S_OhOoh);
        break;
      case 5: //K 5  
        Otto.sing(S_OhOoh2);
        break;
      case 6: //K 6 
        Otto.sing(S_cuddly);
        break;
      case 7: //K 7 
        Otto.sing(S_sleeping);
        break;
      case 8: //K 8 
        Otto.sing(S_happy);
        break;
      case 9: //K 9  
        Otto.sing(S_superHappy);
        break;
      case 10: //K 10
        Otto.sing(S_happy_short);
        break;  
      case 11: //K 11
        Otto.sing(S_sad);
        break;   
      case 12: //K 12
        Otto.sing(S_confused);
        break; 
      case 13: //K 13
        Otto.sing(S_fart1);
        break;
      case 14: //K 14
        Otto.sing(S_fart2);
        break;
      case 15: //K 15
        Otto.sing(S_fart3);
        break;    
      case 16: //K 16
        Otto.sing(S_mode1);
        break; 
      case 17: //K 17
        Otto.sing(S_mode2);
        break; 
      case 18: //K 18
        Otto.sing(S_mode3);
        break;   
      case 19: //K 19
        Otto.sing(S_buttonPushed);
        break;                      
      default:
        break;
    }

    sendFinalAck();
}

void requestDistance(){

    Otto.home();  //stop if necessary  

    int distance = Otto.getDistance();
    Serial.print(F("&&"));
    Serial.print(F("D "));
    Serial.print(distance);
    Serial.println(F("%%"));
    Serial.flush();
}

void requestNoise(){

    Otto.home();  //stop if necessary

    int microphone= Otto.getNoise(); //analogRead(PIN_NoiseSensor);
    Serial.print(F("&&"));
    Serial.print(F("N "));
    Serial.print(microphone);
    Serial.println(F("%%"));
    Serial.flush();
}

void requestBattery(){

    Otto.home();  
    double batteryLevel = Otto.getBatteryLevel();

    Serial.print(F("&&"));
    Serial.print(F("B "));
    Serial.print(batteryLevel);
    Serial.println(F("%%"));
    Serial.flush();
}
void requestProgramId(){

    Otto.home();
    Serial.print(F("&&"));
    Serial.print(F("I "));
    Serial.print(programID);
    Serial.println(F("%%"));
    Serial.flush();
}

//-- Function to send Ack comand (A)
void sendAck(){

  delay(30);

  Serial.print(F("&&"));
  Serial.print(F("A"));
  Serial.println(F("%%"));
  Serial.flush();
}


void sendFinalAck(){

  delay(30);

  Serial.print(F("&&"));
  Serial.print(F("F"));
  Serial.println(F("%%"));
  Serial.flush();
}
void OttoLowBatteryAlarm(){
    double batteryLevel = Otto.getBatteryLevel();
    if(batteryLevel<45){
        
      while(!buttonPushed){

          Otto.putMouth(thunder);
          Otto.bendTones (880, 2000, 1.04, 8, 3);  //A5 = 880
          delay(30);
          Otto.bendTones (2000, 880, 1.02, 8, 3);  //A5 = 880
          Otto.clearMouth();
          delay(500);
      } 
    }
}
void OttoSleeping_withInterrupts(){
  int bedPos_0[4]={100, 80, 60, 120}; 
  if(!buttonPushed){
    Otto._moveServos(700, bedPos_0);  
  }
  for(int i=0; i<4;i++){
    if(buttonPushed){break;}
      Otto.putAnimationMouth(dreamMouth,0);
      Otto.bendTones (100, 200, 1.04, 10, 10);
    if(buttonPushed){break;}
      Otto.putAnimationMouth(dreamMouth,1);
      Otto.bendTones (200, 300, 1.04, 10, 10);  

    if(buttonPushed){break;}
      Otto.putAnimationMouth(dreamMouth,2);
      Otto.bendTones (300, 500, 1.04, 10, 10);   

    delay(500);
    
    if(buttonPushed){break;}
      Otto.putAnimationMouth(dreamMouth,1);
      Otto.bendTones (400, 250, 1.04, 10, 1); 

    if(buttonPushed){break;}
      Otto.putAnimationMouth(dreamMouth,0);
      Otto.bendTones (250, 100, 1.04, 10, 1); 
    
    delay(500);
  } 

  if(!buttonPushed){
    Otto.putMouth(lineMouth);
    Otto.sing(S_cuddly);
  }
  Otto.home();
  if(!buttonPushed){Otto.putMouth(happyOpen);}  
}
