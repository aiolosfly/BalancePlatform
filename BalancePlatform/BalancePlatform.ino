#include "MeMegaPi.h"
#include <math.h>

double rad = PI / 180.0;//degree->rad
double deg = 180.0 / PI;//rad->degree
//
//int xLow = A6;
//int yLow = A7;
//int xHigh = A8;
//int yHigh = A9;
int xLow = A1;
int yLow = A2;
int xHigh = A3;
int yHigh = A4;

struct Point
{
  float x;
  float y;
};

String buffer = "";

//电机的角度，固定

const int microStep = 16;//设置电机的细分量
const long R = 12;//托盘的半径，cm
Point points;
const float tiltRatio = 10.0;

float mDownVal1,mDownVal2,mDownVal3;

long lastTime = 0;
long runTime = 0;
bool testMode = false;
int testAngle = 0;
float testTilt = 0;


void parseBuffer();
float getDownVal(float tiltAngle,float dirAngle,float motorAngle);
float getMotorAngle(float downVal);
void  moveToPos(int stepperNum,int pos);
void ctrlPlatfAngle(float tiltAngle,float dirAngle);
Point getPoint();
void touch();
void spin();
void ctrlByJoystick();


MeStepperOnBoard steppers[3] = 
{MeStepperOnBoard(1),MeStepperOnBoard(2),MeStepperOnBoard(3)}; 

MeJoystick joystick(PORT_6);


const float motorAngle[3]={0,120,-120};

void setup(){
  Serial.begin(115200);
 // delay(100);

 //电机初始设置
   for(int i=0;i<3;i++){
    steppers[i].setMaxSpeed(10000);
    steppers[i].setAcceleration(20000);
    steppers[i].setMicroStep(microStep);
    steppers[i].enableOutputs();
    steppers[i].moveTo(-400);
  }

  Serial.println("*show all orders:  /order/      ");
  Serial.println("*top:              /top/");
  Serial.println("*bottom:           /bottom/");
  Serial.println("*touch:            /touch/");
  Serial.println("*testMode(true):   /mode/t/");
  Serial.println("*testMode(false):  /mode/f/");
  Serial.println("*getDownVal:       /d/tiltAngle/dirAngle/");
  Serial.println("*getMotorAngle:    /a/downVal/");
  Serial.println("*moveToPos:        /s/stepperNum/angle/");  
  Serial.println("*PlatfAngle_3:     /t/tiltAngle/dirAngle/");
  Serial.println("============================================");  
  
//  mDownVal1 = downVal(30,90,30);
//  mDownVal2 = downVal(30,90,150);
//  mDownVal3 = downVal(30,90,-90);
//
//  Serial.println("*********************");
//  
//  Serial.print("mDownVal1: ");
//  Serial.println(mDownVal1);
//  
//  Serial.print("mDownVal2: ");
//  Serial.println(mDownVal2);
//
//  Serial.print("mDownVal3: ");
//  Serial.println(mDownVal3);
 
}

void loop(){

  //隔一段时间采集一次坐标
   if(micros()-lastTime>runTime){
     points = getPoint();
      lastTime = micros();
      runTime = 15000;
      if(testMode==true){

     // spin();
      touch();
     // ctrlByJoystick();

      }
   }



  //getPoint();
    //touch(); 
   
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      parseBuffer();
    } else {
      buffer += c;
    }
  } 
    for(int i=0;i<3;i++){
    steppers[i].run();
   } 
}

void parseBuffer() {
  buffer = buffer + "/";
  int count = 0;
  int startIndex = 0;
  int endIndex = 0;
  int len = buffer.length();
  if (len < 1) {
    return;
  }
  String tmp;
  String values[6];
  while (true) {
    startIndex = buffer.indexOf("/", endIndex);
    endIndex = buffer.indexOf("/", startIndex + 1);
    tmp = buffer.substring(startIndex + 1, endIndex);
    values[count] = tmp;
    count++;
    if (endIndex == len - 1) break;
  }

  if (values[0].equals("order")){

  Serial.println("*order:          show all orders");
  Serial.println("*top:           /top/");
  Serial.println("*bottom:        /bottom/");  
  Serial.println("*touch:          /touch/");
  Serial.println("*getDownVal:    /d/tiltAngle/dirAngle/");
  Serial.println("*getMotorAngle: /a/downVal/");
  Serial.println("*moveToPos:     /s/stepperNum/angle/");  
  Serial.println("*PlatfAngle_3:  /t/tiltAngle/dirAngle/");
  Serial.println("============================================");

   
  }
 
  //测试下降量
  else if (values[0].equals("d")) {
    getDownVal(values[1].toFloat(), values[2].toFloat(),motorAngle[0]);
    getDownVal(values[1].toFloat(), values[2].toFloat(),motorAngle[1]);
    getDownVal(values[1].toFloat(), values[2].toFloat(),motorAngle[2]);
  }
  
  //测试电机角度
  else if (values[0].equals("a")){
    getMotorAngle(values[1].toFloat());   
  }

  //测试电机转动到指定位置
  else if (values[0].equals("s")){
    moveToPos(values[1].toInt(),values[2].toFloat());   

  }



  //测试三个电机
  else if (values[0].equals("t")){
     
      ctrlPlatfAngle(values[1].toInt(),values[2].toInt());
      
  }

  else if (values[0].equals("top")){
     for(int i = 0;i<3;i++)
    {      
      steppers[i].moveTo(-400);
      }  
  }

  else if (values[0].equals("bottom")){
     for(int i = 0;i<3;i++)
    {      
      steppers[i].moveTo(0);
      }
    
  }


 

  else if (values[0].equals("touch")){
    Serial.println("touch()");
    touch();
  }

    else if(values[0].equals("mode")){

    if(values[1].equals("t"))
    testMode = true;
    else if (values[1].equals("f"))
    testMode = false;
  }
  
  Serial.println(buffer);
  buffer = "";
}

void ctrlPlatfAngle(float tiltAngle,float dirAngle )
{
  for(int stepperNum = 0;stepperNum<3;stepperNum++){
  float downVal = getDownVal(tiltAngle, dirAngle, motorAngle[stepperNum]);
  float angle = getMotorAngle(downVal);

  moveToPos(stepperNum,angle); 

  }
 }

//输入下降量，返回电机应到的角度
float getMotorAngle(float downVal)
{
  float angle;
  angle = atan(1-(downVal+sqrt(50))/sqrt(50))*deg;//

 //angle = (downVal!=0)?atan(1-(downVal)/sqrt(50))*deg:0;
  //angle = atan(-sqrt(sqrt(2)-1))*deg;
//  Serial.print(" ** motorAngle: ");
//  Serial.println(angle+45);

  return (angle+45);
    
  }


//输入角度，获取变化量
float getDownVal(float tiltAngle,float dirAngle,float motorAngle)
{
  float downVal=0;
  float diffAngle = abs(motorAngle-dirAngle);
  
  if (diffAngle<=90)
    downVal = R*(1+cos(diffAngle*rad))*sin(tiltAngle*rad);
  else if(diffAngle>90)
    downVal = R*(1-cos(PI-diffAngle*rad))*sin(tiltAngle*rad);
  else
    Serial.println("Wrong");
//
//  Serial.println("************");
//  Serial.print(" ** diffAngle: ");
//  Serial.println(diffAngle);
//  Serial.print(" ** downVal: ");
//  Serial.println(downVal);

   return downVal;
 }

 //移动到对应角度
void  moveToPos(int stepperNum,float angle)
{
    int pos;
    pos = -floor(microStep*angle/1.8);//这里的负号是根据电机转向添加的
//    Serial.print("microStep: ");
//    Serial.println(microStep);
//    Serial.print("angle: ");
//    Serial.println(angle);
//    Serial.println(microStep*angle/1.8);
//    Serial.print("pos:");
//    Serial.println(pos);
   // steppers[stepperNum].moveTo(pos); 
    steppers[stepperNum].moveTo(pos);//(stepperNum,pos);
 }


Point getPoint()
{
  Point point;
  pinMode(xLow,OUTPUT);
  pinMode(xHigh,OUTPUT);
  digitalWrite(xLow,LOW);
  digitalWrite(xHigh,HIGH);

  digitalWrite(yLow,LOW);
  digitalWrite(yHigh,LOW);

  pinMode(yLow,INPUT);
  pinMode(yHigh,INPUT);
  //delay(10);

  float x = analogRead(yLow)-510;//平移到中心
  point.x=(x>-455&&x<455)?17.5*x/455:0;//过滤掉无效值,且与现实长度对应
  

  pinMode(yLow,OUTPUT);
  pinMode(yHigh,OUTPUT);
  digitalWrite(yLow,LOW);
  digitalWrite(yHigh,HIGH);

  digitalWrite(xLow,LOW);
  digitalWrite(xHigh,LOW);

  pinMode(xLow,INPUT);
  pinMode(xHigh,INPUT);
  //delay(10);

  float y=analogRead(xLow)-500;//平移到中心
  point.y=(y>-365&&y<365)?10*y/365:0;//过滤无效值,且与现实长度对应
////
//  Serial.println("==============point============");
//  Serial.print(point.x);  
//  Serial.print(":");  
//  Serial.println(point.y);
//  delay(300);  

  return point;
  
}

//通过遥感模块控制
void ctrlByJoystick()
{
  Point point= {0,0};
  float tiltAngle;
  float dirAngle; 
  float x = joystick.readX()+3;
  point.x=(x>-2&&x<2)?0:x;//过滤掉无效值
  
  float y = joystick.readY();//根据实际的遥感模块偏移
  point.y=(x>-3&&x<3)?0:y;//过滤掉无效值
//  Serial.println("=========Joystick==========");
//  Serial.print("x::");
//  Serial.println(point.x);

//  Serial.print("y::");
//  Serial.println(point.y);

  dirAngle = atan2(point.y,point.x)*deg;

//  Serial.print("dirAngle::");
//  Serial.println(dirAngle);

  long dx = abs(point.x);
  long dy = abs(point.y);
  long powL = dx*dx+dy*dy;
  double L = sqrt(powL);
  tiltAngle = tiltRatio*L/490;//根据实际的遥感修改，490
//  Serial.print("L::");
//  Serial.println(L);
//  Serial.print("tiltAngle::");
//  Serial.println(tiltAngle);  

  ctrlPlatfAngle(tiltAngle,dirAngle );

  
  
  
  
  }

void touch()
{
  
  float tiltAngle;
  float dirAngle; 
  dirAngle = atan2(points.y,points.x)*deg+180;
//  Serial.println("=========touch==========");
//  Serial.print("dirAngle::");
//  Serial.println(dirAngle);

  long dx = abs(points.x);
  long dy = abs(points.y);
  long powL = dx*dx+dy*dy;
  
  double L = sqrt(powL);
  tiltAngle = tiltRatio*L/R;
//  Serial.print("tiltAngle::");
//  Serial.println(tiltAngle);  

     
      ctrlPlatfAngle(tiltAngle,dirAngle );
      

    
}

void spin()
{
        testAngle++;
        testTilt=20;
        
        if(testAngle==180)
          testAngle = -testAngle;//从-180到0度
          
        ctrlPlatfAngle(testTilt,testAngle);
  
  }






