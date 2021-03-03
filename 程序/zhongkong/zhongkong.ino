#define RUNNING
#ifdef RUNNING
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

/*===========================================================
 * 引脚使用表：
 * 14 LED继电器
 * 24-28 传感器1-5
 * 20-24 传感器6-10
 * 25-29 传感器11-15
 * 30-34 传感器16-20
 * 
 * 步进电机 44-PUL 45-DIR
 * A7 A8 A9 红外传感器
 * 2 3 A向电机
 * 4 5 B向电机
 * 6 7 C向电机
 * 8 9 D向电机
 * 10 11 12 三个爪子舵机
 * 13 旋转台舵机
 * 还剩44 45 46可以输出PWM
 */
#define MOTOR1_IN 2
#define MOTOR1_OUT 3
#define MOTOR2_IN 6
#define MOTOR2_OUT 7
#define MOTOR3_IN 4
#define MOTOR3_OUT 5
#define MOTOR4_IN 8
#define MOTOR4_OUT 9
#define DETECTORNUMBER 20
#define OBPINSTART 23
#define WHITE 0
#define BLACK 1
#define DELAYTIME 5
#define MOTORSPEED_MOVING 100
#define MOTORSPEED_MOVING_ADJUSTMENT_FAST 25
#define MOTORSPEED_MOVING_LEAST 30
#endif
#ifdef TEST
#include <stdio.h>
#endif
#define D_X 1
#define D_Y 0

int DBOX[4][12]={{0,0,0,1,0,0,1,0,0,0,1,0},{2,2,2,2,2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2,2,2,2,2}};

#define MAXSTDIS 30
int storage_dis[4]={0,0,0,0};
//srtorage_dis数组存储了当前位置到E区仓库EAA到EDD4个点的坐标，为了方便，请在抓取之后将对应的仓库距离记为MAXSTDIS（最长距离）。
#define EAA 0
#define EBB 1
#define ECC 2
#define EDD 3
//EAA到EDD代表面朝（ABCD）区的仓库编号。

int signal(int x)      //符号函数，返回值0，1，-1.
{
  return((x>0)?1:-1)*((x==0)?0:1);
}
//int i,j; //万能的循环变量

typedef struct POSITION
{
  int x;
  int y;
}POSITION;

//POSITION POSITIONADD(POSITION p1,POSITION p2);  //输出坐标相加的结果。
struct POSITION POS(int a,int b)
{
  POSITION P;
  P.x=a;
  P.y=b;
  return P;
}                     //方便的创建一个坐标常量

#define POSITION_A POS(0,-1)
#define POSITION_B POS(-1,0)
#define POSITION_C POS(0,1)
#define POSITION_D POS(1,0)
int STOR[12]={0,0,0,0,0,0,0,0,0,0,0,0};
#ifdef RUNNING
class MOTOR
{
  public:
  int sped;
  int pinIN;
  int pinOUT;
  MOTOR(int pin_IN,int pin_OUT);
  void motor_speed(int speeds=0);
};

MOTOR::MOTOR(int pin_IN,int pin_OUT)
{
  sped=0;
  pinMode(pin_IN,OUTPUT);
  pinMode(pin_OUT,OUTPUT);
  pinIN=pin_IN;
  pinOUT=pin_OUT;
}
MOTOR MA(MOTOR1_IN,MOTOR1_OUT);
MOTOR MB(MOTOR2_IN,MOTOR2_OUT);
MOTOR MC(MOTOR3_IN,MOTOR3_OUT);
MOTOR MD(MOTOR4_IN,MOTOR4_OUT);
void MOTOR::motor_speed(int speeds)
{
  sped=speeds;
  if(sped>=0)
  {
    analogWrite(pinIN,sped);
    analogWrite(pinOUT,0);
  }
  else
  {
    analogWrite(pinIN,0);
    analogWrite(pinOUT,-sped);
  }
}
class DETECTOR
{
  public:
  int NAME;
  int pin;
  bool WoB;
  DETECTOR(){};
  void init(int pins,int names);
  bool GET();
};
void DETECTOR::init(int pins,int names)
{
  NAME=names;
  pin=pins;
  pinMode(pins,INPUT);
}
bool DETECTOR::GET()
{
  WoB=digitalRead(pin);
  return WoB;
}
DETECTOR dec[DETECTORNUMBER+1];
#endif
#define DA 1
#define DB 2
#define DC 3
#define DD 4
int Direct;
POSITION car_p;   //car_p 为全局变量，记录小车当前坐标值，半格时用负数计算（暂定），如2.5记录为-2云云。
#ifdef RUNNING
void D_GET()
{
  for(int i=1;i<=DETECTORNUMBER;i++)
    dec[i].GET();
}

int sp_lf;
int sp_rt;
void moving(int DIR,int speeds1)
{
  moving(DIR,speeds1,speeds1);
}
void moving(int DIR,int speeds1,int speeds2)
{
  Direct=DIR;
  switch (Direct)
  {
    case DA:MA.motor_speed(0);MB.motor_speed(speeds1*1.12);MC.motor_speed(0);MD.motor_speed(speeds2);break  ;
    case DB:MA.motor_speed(speeds2);MB.motor_speed(0);MC.motor_speed(speeds1);MD.motor_speed(0); break;
    case DC:MA.motor_speed(0);MB.motor_speed(-speeds2*1.12);MC.motor_speed(0);MD.motor_speed(-speeds1);  break;
    case DD:MA.motor_speed(-speeds1);MB.motor_speed(0);MC.motor_speed(-speeds2);MD.motor_speed(0);break;
    default:break;
  }
  sp_lf=speeds1;
  sp_rt=speeds2;
}
#define LEFT -1
#define RIGHT 1
void moving_adjustment(int DIR,int po,int AVER=MOTORSPEED_MOVING,int MOTORSPEED_MOVING_ADJUSTMENT=MOTORSPEED_MOVING_ADJUSTMENT_FAST)
{
  int SIG;
  
  if(po==RIGHT)
  {
    sp_lf = MOTORSPEED_MOVING_ADJUSTMENT + AVER;
    sp_rt =  AVER-MOTORSPEED_MOVING_ADJUSTMENT;
    moving(DIR,sp_lf,sp_rt);
  }
  else if(po==LEFT)
  {
    sp_rt = MOTORSPEED_MOVING_ADJUSTMENT + AVER;
    sp_lf =  AVER-MOTORSPEED_MOVING_ADJUSTMENT;
    moving(DIR,sp_lf,sp_rt);
  }
}

void park(int DIR,int dly)
{
  bool ob[5]={0};
  for(int i=MOTORSPEED_MOVING;i>MOTORSPEED_MOVING_LEAST;i--)
  {
    moving(DIR,i);
    delay(dly);
  }
  moving(DIR,0);
  
}

#endif

void local_adjust()
{
  int i=0;
  while(i<100)
  {
    D_GET();
    if(dec[3].WoB==0 && dec[4].WoB==1 && dec[2].WoB==1
    && dec[8].WoB==0 && dec[7].WoB==1 && dec[9].WoB==1
    && dec[13].WoB==0 && dec[12].WoB==1 && dec[11].WoB==1
    && dec[18].WoB==0 && dec[19].WoB==1 && dec[17].WoB==1)
    {
      MA.motor_speed(0);MB.motor_speed(0);MC.motor_speed(0);MD.motor_speed(0);break;
     }
    if(!(dec[1].WoB && dec[2].WoB))
      MA.motor_speed(-80);
    if(!(dec[4].WoB && dec[5].WoB))
      MA.motor_speed(80);
    if(dec[3].WoB==0 && dec[4].WoB==1 && dec[2].WoB==1)
      MA.motor_speed(0);
    if(!(dec[6].WoB && dec[7].WoB))
      MB.motor_speed(80);
    if(!(dec[9].WoB && dec[10].WoB))
      MB.motor_speed(-80);
    if(dec[8].WoB==0 && dec[7].WoB==1 && dec[9].WoB==1)
      MB.motor_speed(0);
    if(!(dec[11].WoB && dec[12].WoB))
      MC.motor_speed(80);
    if(!(dec[14].WoB && dec[15].WoB))
      MC.motor_speed(-80);
    if(dec[13].WoB==0 && dec[12].WoB==1 && dec[11].WoB==1)
      MC.motor_speed(0);
    if(!(dec[16].WoB && dec[17].WoB))
      MD.motor_speed(-80);
    if(!(dec[19].WoB && dec[20].WoB))
      MD.motor_speed(80);
    if(dec[18].WoB==0 && dec[19].WoB==1 && dec[17].WoB==1)
      MD.motor_speed(0);
     delay(30);
     i++;
  }
}

void car_move_single(int DIR)
{
  bool ob[5]={0};
  D_GET();
  for(int i=0;i<5;i++)
  ob[i]=dec[5*DIR-4+i].WoB;
  moving(DIR,MOTORSPEED_MOVING);
  delay(300);
  D_GET();
  for(int i=0;i<5;i++)
    ob[i]=dec[5*DIR-4+i].WoB;
  while(1)
  {
    if( dec[(5*DIR-4+2+5+20)%20].WoB==WHITE || dec[(5*DIR-4+2-5+20)%20].WoB==WHITE )
      break;
    if(ob[4]==WHITE && ob[0]==WHITE)
      moving(DIR,MOTORSPEED_MOVING);
    else if(ob[3]==WHITE)
      moving_adjustment(DIR,LEFT);
    else if(ob[1]==WHITE)
      moving_adjustment(DIR,RIGHT);
    else
      moving(DIR,MOTORSPEED_MOVING);
       D_GET();
     for(int i=0;i<5;i++)
         ob[i]=dec[5*DIR-4+i].WoB;
  }
   switch(DIR)
   {
    case DA:car_p=POSITIONADD(car_p,POSITION_A);break;
    case DB:car_p=POSITIONADD(car_p,POSITION_B);break;
    case DC:car_p=POSITIONADD(car_p,POSITION_C);break;
    case DD:car_p=POSITIONADD(car_p,POSITION_D);break;
    default:break;
   }
}
void car_move_single_and_stop(int DIR)
{
  bool ob[5]={0};
  D_GET();
  for(int i=0;i<5;i++)
    ob[i]=dec[5*DIR-4+i].WoB;
    moving(DIR,MOTORSPEED_MOVING);
    delay(200);
  D_GET();
  for(int i=0;i<5;i++)
    ob[i]=dec[5*DIR-4+i].WoB;
  while(1)
  {
    if( dec[(5*DIR-4+2+4+20)%20].WoB==WHITE || dec[(5*DIR-4+2-4+20)%20].WoB==WHITE )
      break;
    if(ob[2]==WHITE)
      moving(DIR,MOTORSPEED_MOVING);
    else if(ob[3]==WHITE)
      moving_adjustment(DIR,LEFT);
    else if(ob[1]==WHITE)     
      moving_adjustment(DIR,RIGHT);
    else
      moving(DIR,MOTORSPEED_MOVING);
    delay(DELAYTIME);
       D_GET();
     for(int i=0;i<5;i++)
         ob[i]=dec[5*DIR-4+i].WoB;
  }
   switch(DIR)
   {
    case DA:car_p=POSITIONADD(car_p,POSITION_A);break;
    case DB:car_p=POSITIONADD(car_p,POSITION_B);break;
    case DC:car_p=POSITIONADD(car_p,POSITION_C);break;
    case DD:car_p=POSITIONADD(car_p,POSITION_D);break;
    default:break;
   }
  park(DIR,0);
  delay(500);
  local_adjust();
}
/*#endif

#ifdef RUNNING
void setoff(int DIR)//不只是从起点走出来，所有从非十字路口走到十字路口都用这个函数
{
  //int DIR=DB;
  bool ob[5];
  D_GET();
  for(int i=0;i<5;i++)
  {
    ob[i]=dec[5*DIR-4+i].WoB;
  }
  moving(DIR,MOTORSPEED_MOVING);
   while(((dec[(5*DIR-4+2+5+20)%20].WoB)==BLACK)&&((dec[(5*DIR-4+2-5+20)%20].WoB)==BLACK))
  {
    if((ob[1]==BLACK)&&(ob[3]==BLACK))
    {
      moving(DIR,MOTORSPEED_MOVING);
    }
    else if(ob[1]==WHITE)
    {
      moving_adjustment(DIR,RIGHT);
    }
    else if(ob[3]==WHITE)
    {
      moving_adjustment(DIR,LEFT);
    }
    delay(DELAYTIME);
       D_GET();
     for(int i=0;i<5;i++)
      {
         ob[i]=dec[5*DIR-4+i].WoB;
      }
  }
   park(DIR,1);
}

#endif
/*
*/
#ifdef RUNNING
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void MOVE_TO(POSITION po1,POSITION po2,int dir)
{
  int dx,dy;
  if(dir==D_X)
  {
    if(po1.x==po2.x)
     return;
    for(dx=po1.x;dx!=(po2.x-(2*(po2.x>po1.x)-1));)
    { 
      dx+=(2*(po2.x>po1.x)-1);
      car_move_single((po2.x>po1.x)?DD:DB);
    }  
    car_move_single_and_stop((po2.x>po1.x)?DD:DB);
  }
  else
  {
    if(po1.y==po2.y)
    return;
    for(dy=po1.y;dy!=(po2.y-(2*(po2.y>po1.y)-1));)
    {
      dy+=(2*(po2.y>po1.y)-1);
      car_move_single((po2.y>po1.y)?DC:DA);
    }
      car_move_single_and_stop((po2.y>po1.y)?DC:DA);
  }
}
#endif
//=============================底层==============================================================


//=============================贪心==============================================================
int distance(POSITION po1,POSITION po2)
{
  return (po1.x-po2.x)*((po1.x>po2.x)*2-1)+(po1.y-po2.y)*((po1.y>po2.y)*2-1);
}
char Map[12][12];
#ifdef TEST
void MapPrint()
{
  for(int i=0;i<=11;i++)
  {
    for(int j=0;j<=11;j++)  printf("%2d   ",Map[i][j]);
    printf("\n");
  }
}
#endif
void MapSet()
{
  for(int i=0;i<=11;i++)
  {
    for(int j=0;j<=11;j++)
    {
      if(i==0 ||j==0 || i==11 || j==11 || i==1 || j==1 || i==10 || j==10) Map[i][j]=-1;
      else if((i>=4)&&(i<=7)&&(j>=4)&&(j<=7)) Map[i][j]=-1;
      else Map[i][j]=0;
    }
  }
  Map[1][8]=0;
  Map[1][9]=0;
  Map[3][1]=0;
  Map[2][1]=0;
  Map[10][2]=0;
  Map[10][3]=0;
  Map[10][9]=0;
  Map[10][8]=0;
}

POSITION PointMove(POSITION p,int DIR)
{
  POSITION p0=p;
  switch(DIR)
  {
    case DA:p0.y--;break;
    case DB:p0.x--;break;
    case DC:p0.y++;break;
    case DD:p0.x++;break;
    default:break;
  }
  return p0;
}

void pMove(POSITION *p,int DIR)
{
  switch(DIR)
  {
    case DA:(p->y)--;break;
    case DB:(p->x)--;break;
    case DC:(p->y)++;break;
    case DD:(p->x)++;break;
    default:break;
  }
}
char MapPo(POSITION p)
{
  return Map[p.x][p.y];
}
int Greese(POSITION p1,POSITION p2)
{
  int d1,d2,d3,d4;
  d1=distance(PointMove(p1,DA),p2);
  d2=distance(PointMove(p1,DB),p2);
  d3=distance(PointMove(p1,DC),p2);
  d4=distance(PointMove(p1,DD),p2);
  if(MapPo(PointMove(p1,DA))!=0) d1=242;
  if(MapPo(PointMove(p1,DB))!=0) d2=242;
  if(MapPo(PointMove(p1,DC))!=0) d3=242;
  if(MapPo(PointMove(p1,DD))!=0) d4=242;
  if( d1<=d2 && d1<=d3 && d1<=d4 ) return DA;
  else if((d2<=d3)&&(d2<=d4)) return DB;
  else if((d3<=d4)) return DC;
  else return DD;
}

int MapMarking(POSITION p1,POSITION p2)
{
  int steps=0;
  MapSet();
  while((p1.x!=p2.x)||(p1.y!=p2.y))
  {
    steps++;
  Map[p1.x][p1.y]=Greese(p1,p2);
  pMove(&p1,Map[p1.x][p1.y]);
  }
  return steps;
}

#define AA 1
#define BB 2
#define CC 3
#define DD 4
POSITION W_R1(int w)
{
  POSITION p;
  switch(w)
  {
    case AA:p.x=10;p.y=2;break;
    case BB:p.x=1;p.y=1;break;
    case CC:p.x=1;p.y=10;break;
    case DD:p.x=10;p.y=10;break;
    default:p.x=0;p.y=0;break;
  }
  return p;
}
POSITION W_R2(int w)
{
  POSITION p;
  switch(w)
  {
    case AA:p.x=5;p.y=2;break;
    case BB:p.x=2;p.y=6;break;
    case CC:p.x=6;p.y=9;break;
    case DD:p.x=9;p.y=5;break;
    default:p.x=0;p.y=0;break;
  }
  return p;
}
POSITION E_R1(int E)
{
  switch(E)
  {
    case EAA:return POS(5,3);break;
    case EBB:return POS(3,6);break;
    case ECC:return POS(6,8);break;
    case EDD:return POS(8,5);break;
    default:return POS(0,0);break;
  }
}
POSITION E_R2(int E)
{
  switch(E)
  {
    case EAA:return POS(7,3);break;
    case EBB:return POS(3,4);break;
    case ECC:return POS(4,8);break;
    case EDD:return POS(8,7);break;
    default:return POS(0,0);break;
  }
}
int MIN(int a,int b,int c,int d)
{
 return ((a<b?a:b)<(c<d?c:d)?(a<b?a:b):(c<d?c:d));
}
POSITION MapRunning(int wa,int wb,int wc,POSITION p)
{
  POSITION W;
  POSITION W0;
  W0.x=0;W0.y=0;
  int pa,pb,pc;
  if((wa==0)&&(wb==0)&&(wc==0))
  return W0;
  else if((wa==0)&&(wb==0))
  {
    W.x=wc;W.y=3;
  }
  else if((wa==0)&&(wc==0))
  {
    W.x=wb;W.y=2;
  }
  else if((wb==0)&&(wa==0))
  {
    W.x=wa;W.y=1;
  }
  else
  {
    if(wa!=0) pa=MapMarking(p,W_R2(wa));else pa=50;
    if(wb!=0) pb=MapMarking(p,W_R2(wb));else pb=50;
    if(wc!=0) pc=MapMarking(p,W_R2(wc));else pc=50;
  }
  if((pa<=pb)&&(pa<=pc)){W.x=wa;W.y=1;}
  else if(pb<=pc){W.x=wb;W.y=2;}
  else {W.x=wc;W.y=3;}
 return W; 
}

int D_change(int D)
{
  if((D==DD)||(D==DB))
  return D_X;
  else return D_Y;
}
void MapTraveling(POSITION p1,POSITION p2)
{
  POSITION OB;
  int di_f;
  int counter;
  while((p1.x!=p2.x)||(p1.y!=p2.y))
  {
    OB=p1;
    di_f=Map[p1.x][p1.y];
    counter=0;
    do
    {
    pMove(&OB,di_f);
    counter++;
    }
    while(Map[OB.x][OB.y]==di_f);
    MOVE_TO(p1,OB,D_change(di_f));
    p1=OB;
  }
}



//函数作用：给定仓库区编号，获取最短距离（并编码地图[没什么用]）。
int st_dis_get(int MARK,POSITION P=car_p)
{
  int DIS;
  switch(MARK)
  {
  case EAA:DIS= MapMarking(P,POS(7,3));break;
  case EBB:DIS= MapMarking(P,POS(3,4));break;
  case ECC:DIS= MapMarking(P,POS(4,8));break;
  case EDD:DIS= MapMarking(P,POS(8,7));break;
  default:return 0;break;
}   return DIS;
}
//函数说明：给定当前位置（实际传参时务必MapMarkingReturn(car_p))
//标记当前位置到最近货架的地图
//返回目的地的坐标
int CAR_E;
POSITION MapMarkingReturn(POSITION p=car_p)
{
  int MINDISTANCE;
  int FORI;
  POSITION ENDING;
  for(FORI=EAA;FORI<=EDD;FORI++)
  {
    if(storage_dis[FORI]!=MAXSTDIS)
      storage_dis[FORI]=st_dis_get(FORI,p);
  }
  MINDISTANCE=MIN(storage_dis[EAA],storage_dis[EBB],storage_dis[ECC],storage_dis[EDD]);
  if     (MINDISTANCE==storage_dis[EAA])    {MapMarking(p,POS(7,3));CAR_E=EAA;return E_R1(EAA);}
  else if(MINDISTANCE==storage_dis[EBB])    {MapMarking(p,POS(3,4));CAR_E=EBB;return E_R1(EBB);}
  else if(MINDISTANCE==storage_dis[ECC])    {MapMarking(p,POS(4,8));CAR_E=ECC;return E_R1(ECC);}
  else if(MINDISTANCE==storage_dis[EDD])    {MapMarking(p,POS(8,7));CAR_E=EDD;return E_R1(EDD);}
  else return POS(0,0);
}
#ifdef RUNNING
//动作：从小车当前位置到达仓库位置。
void return_to_storage(POSITION p=car_p)
{
  POSITION END=MapMarkingReturn(p);
  if(END.x!=0)                             //防止蠢蠢的小车真往（0，0）走。
  MapTraveling(p,END);
}
#endif

POSITION POSITIONADD(POSITION p1,POSITION p2)
{
  POSITION p;
  p.x=p1.x+p2.x;
  p.y=p1.y+p2.y;
  return p;
}

//==============================================================================================================
//=======================码盘===========================
//======================================================
//======================================================
#define MOTORA_MP 18
#define MOTORB_MP 19
#define MOTORC_MP 20
#define MOTORD_MP 21     //四个电机的码盘引脚
#define PI 3.14159
#define LUNZI_D 58
#define MPERREV 390                   //行进距离=(PI*LUNZI_D*N)/MPERREV
#define IOPEN 0
#define ICLOSE 1
#define SLOWPWM 70
#define SLOWPWMADJ 10
int MAMP;
int MBMP;
int MCMP;
int MDMP;
void mapan_init()
{
  pinMode(MOTORA_MP,INPUT);
  pinMode(MOTORB_MP,INPUT);
  pinMode(MOTORC_MP,INPUT);
  pinMode(MOTORD_MP,INPUT);
}
void MP_Interrupt(bool STA=IOPEN)         //码盘中断初始化和关闭
{
  if(STA==IOPEN)
  {
    attachInterrupt(5,MAMPADD,FALLING);
    attachInterrupt(4,MBMPADD,FALLING);
    attachInterrupt(3,MCMPADD,FALLING);
    attachInterrupt(2,MDMPADD,FALLING);
    MAMP=0;MBMP=0;MCMP=0;MDMP=0;
  }
  else
  {
    detachInterrupt(2);
    detachInterrupt(3);
    detachInterrupt(4);
    detachInterrupt(5);
  }

}

void MPMOVE(int DIR,double dis)        //dis以毫米为单位。
{
  int N=int((dis*MPERREV)/(PI*LUNZI_D));   //目标码盘数
  int SPE1=0;
  int SPE2=0;
  MP_Interrupt(IOPEN);
  int *MLEFT;
  int *MRIGHT;

  moving(DIR,SLOWPWM,SLOWPWM);
  switch (DIR)
  {
  case DA:MLEFT=&MBMP;MRIGHT=&MDMP;break;
  case DB:MLEFT=&MCMP;MRIGHT=&MAMP;break;
  case DC:MLEFT=&MDMP;MRIGHT=&MBMP;break;
  case DD:MLEFT=&MAMP;MRIGHT=&MCMP;break;
  default:break;
  }
  while((*MLEFT)<=N)
    delay(5);
  moving(DIR,0);
}
void MAMPADD()
{
  MAMP++;
}
void MBMPADD()
{
  MBMP++;
}
void MCMPADD()
{
  MCMP++;
}
void MDMPADD()
{
  MDMP++;
}
//============================码盘结束=================================
//=====================================================================
//============================码盘结束=================================

/*==============================================================
****************************步进电机******************************
===============================================================*/
/**/
#define STEP_DIR   45        //步进电机的引脚，待修改
#define STEP_PUL   44
#define RISE 0              //0或1取决于步进电机dir的连接方式
#define FALL 1
#define PUL_PER_REV  400    //每转的圈数，通过拨码开关调节
#define LUOJU        5      //螺距，单位毫米
                            //步进电机上升距离计算公式：DIS=(PUL*LUOJU)/PUL_PER_REV    单位毫米
#define PULDELAY    300     //每组脉冲之间的delay，单位微秒，实际值乘2，决定了步进电机速度。速度过快就把这个值调大。
                            //步进电机速度计算公式：Vec=(PUL*LUOJU*100000)/(PUL_PER_REV*PULDELAY)  单位厘米/秒
#define CHUSHIWEIZHI  185   //初始位置爪子下沿距地面距离
#define XIACENGGUIZI  210   //下层柜子放置时，爪子下沿距地面距离
#define SHANGCENGGUIZI 410  //上层柜子放置时，爪子下沿距地面距离
#define ZHONGYANGWUJIA 210  //中央仓库抓取时，爪子下沿距地面距离
int YUNTAI_HEIGHT;           //实时记录的云台高度值。
void StepInit()            //把STEP_INIT写到setup里面
{
  pinMode(STEP_DIR,OUTPUT);
  pinMode(STEP_PUL,OUTPUT);
}
void STEP_MOVE(bool DIR,int DIS)       //输入变量DIR是RISE/FALL，DIS是以毫米为单位的距离。
{
  int PUL=(DIS*PUL_PER_REV)/LUOJU;
  digitalWrite(STEP_DIR,DIR);  //说明：DIR为RISE或FALL，如果步进电机跑反了就修改上面define的RISE和FALL即可。
  while(PUL--)
  {
    digitalWrite(STEP_PUL,HIGH);
    delayMicroseconds(PULDELAY);
    digitalWrite(STEP_PUL,LOW);
    delayMicroseconds(PULDELAY);
  }
  YUNTAI_HEIGHT=YUNTAI_HEIGHT+DIS*(RISE==1)*(DIR*2-1);
}
void STEP_SETUP()                 //用来把爪子从初始位置上升到200mm柜子处,在考虑要不要有一个0位校准电路
{
  YUNTAI_HEIGHT=185;
  STEP_MOVE(RISE,XIACENGGUIZI-CHUSHIWEIZHI);  
}

//====================步进电机结束========================================================================
//=======================================================================================================
bool HONGWAI()
{
    int distance,cnt=0;
    for(int i=0;i<10;i++)
    {
        distance=analogRead(A7);
        Serial.println(distance);
        if(distance>=120)
          cnt++;
    }
    if(cnt<=8)
        return 0;
    return 1;
}
//=============================================================================================================
char SerialBuffer[15];
/*void blink_(int j)
{
    for(int i=1;i<=j;i++)
    {
    digitalWrite(14,LOW);
    delay(500);
    digitalWrite(14,HIGH);
    delay(500);
    }
}*/
void ReadSerial()
{
  int cnt=0;
  for(int i=0;i<15;i++)
    SerialBuffer[i]=0;
  while(Serial.available()>0)
    SerialBuffer[cnt++] = Serial.read();//读串口第一个字节
}
int GetWhatIs()
{
  Serial.begin(115200);//初始化串口
  digitalWrite(14,LOW);
  do
  {
    Serial.println("R");
    delay(500);
  }
  while(Serial.available()==0);
  delay(100);
    ReadSerial();
   //blink_(SerialBuffer[0]-'A'+1);
  Serial.end();
  digitalWrite(14,HIGH);
  switch(SerialBuffer[0])
  {
    case 'A': return AA; 
    case 'B': return BB;
    case 'C': return CC; 
    case 'D': return DD;  
    default: return 0;
   }
}
//================================================================================================================
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
/*Servo turn,claw1,claw2,claw3;
void ServoInit()
{
  turn.attach(13);
  claw1.attach(10);
  claw2.attach(11);
  claw3.attach(12);
  turn.write(0);
  
  claw1.write(0);
  claw2.write(0);
  claw3.write(0);  
}

#define DELAYMS 20
void ServoMove(Servo *SER,int po1,int po2)    //180度舵机
{
  int po;
  if(po1<po2)
  {
    for(po=po1;po<=po2;po++)
    {
      (*SER).write(po);
      delay(DELAYMS);
    }
  }
  else    
  {
    for(po=po1;po>=po2;po--)
    {
      (*SER).write(po);
      delay(DELAYMS);
    }
  }

}*/

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
/////////////////
void RELEASEIT(int* S)
{
  ServoMove(S,110,0);
}
/////////////////////
void LEDInit()
{
  pinMode(14,OUTPUT);
  digitalWrite(14,HIGH);
}

void PUT_TO_HUOJIA(int H)
{
  int dir1,dir2,dir3,dir4;  //dir1-dir4从面向方向按逆时针方向转四个角度。
  int I;                    //I用以判断是否过了5（是否回头）
  int HW;                   //HW用来接收红外传感器的数据，有为1，无是0。
  int STESIGN=0;
  STEP_MOVE(RISE,20);
  dir2=(DA+(H-AA))%4+1;
  dir3=(DB+(H-AA))%4+1;
  dir4=(DC+(H-AA))%4+1;
  dir1=(DD+(H-AA))%4+1;
  //car_move_single_and_stop(dir3);
  delay(300);
  Servoturns(dir1);
  delay(300);
  car_move_single_and_stop(dir1);
  delay(1000);
  
  I=0;
  while((CLAW1==H)||(CLAW2==H)||(CLAW3==H))
  {
    if(I<=4)
    {
      if(DBOX[H-AA][I]==2)
      {
        delay(500);
        DBOX[H-AA][I]=HONGWAI();
        Serial.println(DBOX[H-AA][I]);
        delay(500);     
      }
      if(DBOX[H-AA][I]==0)
      {
        //Serial.println("1002");
        if(CLAW1==H)
        {
          Serial.println("1005");
          car_move_single_and_stop(dir3);
          Servoturns(dir2);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw1);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW1=0;
        }
        else if(CLAW2==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir3);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw2);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW2=0;
        }
        else if(CLAW3==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir4);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw3);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW3=0;
        }
        else ;
      }
      car_move_single_and_stop(dir4);
      I++;
    }
    else if (I==5)                                //整体上复制了上面的代码
    {
      if(DBOX[H-AA][I]==2)
        DBOX[H-AA][I]=HONGWAI();
      if(DBOX[H-AA][I]==0)
      {
        if(CLAW1==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir2);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw1);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW1=0;
        }
        else if(CLAW2==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir3);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw2);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW2=0;
        }
        else if(CLAW3==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir4);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw3);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW3=0;
        }
        else ;
      }
      
      STEP_MOVE(RISE,170);
      STESIGN=1;
      I++;
    }
    else                                  //同样复制了上面的代码。
    {
      if(DBOX[H-AA][I]==2)
        DBOX[H-AA][I]=HONGWAI();
      if(DBOX[H-AA][I]==0)
      {
        if(CLAW1==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir2);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw1);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW1=0;
        }
        else if(CLAW2==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir3);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw2);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW2=0;
        }
        else if(CLAW3==H)
        {
          car_move_single_and_stop(dir3);
          Servoturns(dir4);
          car_move_single_and_stop(dir1);
          RELEASEIT(&claw3);
          car_move_single_and_stop(dir3);
          Servoturns(dir1);
          car_move_single_and_stop(dir1);
          CLAW3=0;
        }
        else ;
      }
      car_move_single_and_stop(dir2);
      I++;
  }
}
delay(300);
car_move_single_and_stop(dir3);
if(STESIGN==1)
STEP_MOVE(FALL,170);
STEP_MOVE(FALL,20);
}
//////////////////////////////////////////////////////
void PUTTING(int wa,int wb,int wc,POSITION p=car_p)
{
  POSITION W=MapRunning(wa,wb,wc,p);
  MapMarking(p,W_R2(W.x));
  MapTraveling(p,W_R2(W.x));
  PUT_TO_HUOJIA(W.x);
}
void PUT_3(int wa=CLAW1,int wb=CLAW2,int wc=CLAW3)
{
  while(CLAW1!=0 || CLAW2!=0 ||CLAW3!=0)
  {
    PUTTING(CLAW1,CLAW2,CLAW3,car_p);
  }
}
/////////////////////////////////////////////
void niu()
{}

//////////////////////////////////////////////////////
void SHIBIE(int Eir)
{
  STEP_MOVE(RISE,15);
  delay(500);
  niu();
  int dir1,dir2,dir3,dir4;
  dir2=(DC+(Eir-EAA))%4+1;
  dir3=(DD+(Eir-EAA))%4+1;
  dir4=(DA+(Eir-EAA))%4+1;
  dir1=(DB+(Eir-EAA))%4+1;
  Servoturns(dir1);
  delay(300);
  niu();
  MPMOVE(dir1,100);
  delay(1000);
  MPMOVE(dir4,195);
  delay(1000);
  if(STOR[Eir*3]==0)
  STOR[Eir*3]=GetWhatIs();
  delay(500);
  niu();
  MPMOVE(dir4,195);
  delay(1000);
  if(STOR[Eir*3+1]==0)
  STOR[Eir*3+1]=GetWhatIs();
  delay(500);
  MPMOVE(dir4,195);
  delay(1000);
  if(STOR[Eir*3+2]==0)
  STOR[Eir*3+2]=GetWhatIs();
  delay(500);
  MPMOVE(dir3,100);
  delay(500);
  niu();
  car_move_single_and_stop(dir4);
  
  car_p=E_R1(Eir);
  delay(300);
  STEP_MOVE(FALL,15);
  niu();
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void ZHUAQU(int Eir)
{
  int dir1,dir2,dir3,dir4;
  dir2=(DC+(Eir-EAA))%4+1;
  dir3=(DD+(Eir-EAA))%4+1;
  dir4=(DA+(Eir-EAA))%4+1;
  dir1=(DB+(Eir-EAA))%4+1;
  Servoturns(dir2);
  delay(300);
  MPMOVE(dir2,195);
  delay(1000);
  MPMOVE(dir1,190);
  delay(1000);
  Servoturns(dir2);
  delay(300);
  CLAW1=STOR[Eir*3+2];
  ServoMove(&claw1,0,110);           //爪1抓取
  delay(500);
  STEP_MOVE(RISE,70);
  delay(500);
  Servoturns(dir3);
  delay(300);
  MPMOVE(dir2,195);
  delay(1000);
  STEP_MOVE(FALL,70);
  delay(500);
  CLAW2=STOR[Eir*3+1];
  ServoMove(&claw2,0,110);           //爪2抓取    
   delay(500);
  STEP_MOVE(RISE,70);
  delay(500);
  MPMOVE(dir3,190);
  delay(500);
  local_adjust();
  delay(500);
  MPMOVE(dir1,190);
  Servoturns(dir4);
  delay(300);
  MPMOVE(dir2,195);
  delay(1000);
  STEP_MOVE(FALL,70);
  delay(500);
  CLAW3=STOR[Eir*3];
  ServoMove(&claw3,0,110);           //爪3抓取    
  MPMOVE(dir3,190);
  car_move_single_and_stop(dir2);
  delay(1000);
  car_p=E_R2(Eir);
  storage_dis[Eir]=MAXSTDIS;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void setoff()
{
  MPMOVE(DB,590);
  local_adjust();
  car_move_single(DB);
  car_move_single_and_stop(DB);
  delay(1000);
}
void Go(POSITION p1,POSITION p2)
{
  MapMarking(p1,p2);
  MapTraveling(p1,p2);
  delay(500);
}
#ifdef RUNNING
void setup()
{
  // put your setup code here, to run once:
  /*for(int i=1;i<=DETECTORNUMBER;i++)//初始化传感器
    dec[i].init(i+OBPINSTART,i);
  //MapSet();//初始化地图
  mapan_init();
 ServoInit();
 LEDInit();
  StepInit();// =========================================================步进电机初始化，待完成
  car_p=POS(2,10); //=======================出发时小车的位置为(10,2)*/
  
  Serial.begin(38400);
  ServoInit();
}


//=============================结束======================================================
#define RUN
#ifdef TESTS
void loop()
{
  GetWhatIs();
}
#endif
#ifdef RUN
void loop()
{
 /* int TIMES;
 //ServoMove(&claw1,0,110);
// delay(1000);
// ServoMove(&claw2,0,110);
// delay(1000);
// ServoMove(&claw3,0,110);
// delay(1000);
 //delay(5000);
//
//
delay(20000);
setoff();
delay(500);

CAR_E=EAA;
//for(TIMES=4;TIMES>0;TIMES--)
//{
SHIBIE(CAR_E);
Servoturns(DD);
delay(1000);
Go(car_p,POS(3,4));
CAR_E=EBB;
SHIBIE(CAR_E);
Servoturns(DA);
delay(1000);
Go(car_p,POS(4,8));
CAR_E=ECC;
SHIBIE(CAR_E);
Servoturns(DB);
delay(1000);
Go(car_p,POS(8,7));
CAR_E=EDD;
SHIBIE(CAR_E);
delay(1000);
// car_move_single_and_stop(DB);
for(int i=0;i<=3;i++)
{
local_adjust();
ZHUAQU(CAR_E);
delay(1000);
PUT_3();
delay(500);
return_to_storage();
delay(500);
}*/
//delay(2000);
Serial.println('0');
ServoMove(&claw1,0,250);
Serial.println('1');
delay(1000);
ServoMove(&claw2,0,250);
Serial.println('2');
delay(1000);
ServoMove(&claw3,0,250);
Serial.println('3');
delay(1000);
Servoturns(DB);
Serial.println('4');
while(1);

}
#endif
#endif

