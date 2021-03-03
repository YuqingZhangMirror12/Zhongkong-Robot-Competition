#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define DA 1
#define DB 2
#define DC 3
#define DD 4
Adafruit_PWMServoDriver pwm= Adafruit_PWMServoDriver();
int turn=0,claw1=1,claw2=2,claw3=3;
#define DELAYMS 20
void ServoInit()
{
  Serial.println("Start");
  pwm.begin();
  Serial.println("SEt");
  pwm.setPWMFreq(490);// Analog servos run at 490 Hz updates
  pwm.setPWM(0, 0, 0);
  pwm.setPWM(1, 0, 0);
  pwm.setPWM(2, 0, 0);
  pwm.setPWM(3, 0, 0);
  Serial.println("Done");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ServoInit();
}


#define DELAYMS 20
void ServoMove(int *SER,int po1,int po2)
{
  //po1=po1/180*1024;
  //po2=po2/180*1024;
  if(po1<po2)
    for (uint16_t pulselen = po1; pulselen <= po2; pulselen++)
    {
      pwm.setPWM(*SER,0,pulselen);
      delay(DELAYMS);
    }
  else
    for (uint16_t pulselen = po2; pulselen <= po1; pulselen++)
    {
      pwm.setPWM(*SER,0,pulselen);
      delay(DELAYMS);
    }
  
}

int CLAW1,CLAW2,CLAW3;
int TurnAngle=0;
#define ANDA 120
#define ANDB 178
#define ANDC 0
#define ANDD 58
void Servoturns(int FACE)      //指定舵机的旋转的函数，作用是将摄像头旋转到面向FACE方向。
{
  int ANGLE;
  switch (FACE)
  {
  case DA: ANGLE=ANDA;break;
  case DB: ANGLE=ANDB;break;
  case DC: ANGLE=ANDC;break;
  case DD: ANGLE=ANDD;break;
  default :return;break;
  }
  ServoMove(&turn,TurnAngle,ANGLE);      //
  TurnAngle=ANGLE;
}
void loop() {
  // put your main code here, to run repeatedly:
   for(int i=50;i>=380;i++)
   pwm.setPWM(1, 0, i);
   delay(1000);
}
