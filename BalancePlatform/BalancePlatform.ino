#include "MeMegaPi.h"
#include <SoftwareSerial.h>
#include <math.h>
 
/******* system params *******/
MeStepperOnBoard steppers[3] =
{MeStepperOnBoard(1), MeStepperOnBoard(2), MeStepperOnBoard(3)};

//MeJoystick joystick(PORT_6);

Me4Button butP(PORT_8);
Me4Button butI(PORT_7);
Me4Button butD(PORT_6);

MeSerial mySerial(PORT_5);
  

struct Point
{
  float x;
  float y;
};

Point targetP = {0,0};
Point lastPoint;

const double rad = PI / 180.0;//degree->rad
const double deg = 180.0 / PI;//rad->degree

const int microstep = 16;
const float tiltRatio = 2;
const long R = 13;//托盘的半径，cm
const float motorDirAngle[3] = {0, 120, -120};

//电机转动的角度**
struct Angle{
  float m[3];
  int length;
  
};
Angle motorMoveAngle;
Angle lastAngle;

 //电机连接处的角度

long lastTime = 0;
long runTime = 8000;
bool testMode = true;
int testAngle = 0;

String buffer = "";

/******* TouchPad params*******/
int xLow = A1;
int yLow = A2;
int xHigh = A3;
int yHigh = A4;

/******* PID params *******/
//PID系数
float Kp = 0.84;
float Ki = 0.03;
float Kd = 0.46;

double pItem = 0;
double iItem = 0;
double dItem = 0;

Point currentPos;

/******* testPID *******/
uint8_t keyPreP = KEY_NULL;
uint8_t keyPrePrevP = KEY_NULL;

uint8_t keyPreI = KEY_NULL;
uint8_t keyPrePrevI = KEY_NULL;

uint8_t keyPreD = KEY_NULL;
uint8_t keyPrePrevD = KEY_NULL;

//用按键开关调试PID参数,"-""+"
void testPidParams()
{

  keyPrePrevP = keyPreP;
  keyPreP = butP.pressed();

  keyPrePrevI = keyPreI;
  keyPreI = butI.pressed();

  keyPrePrevD = keyPreD;
  keyPreD = butD.pressed();

if (keyPrePrevP != keyPreP)
{
  switch (keyPreP)
  {
    case KEY_1:
      {
        Kp+= 0.02;
//        Serial.print("yKp");
//        Serial.println(yKp);
        break;
      }

    case KEY_3:
      {
        Kp-=0.02;
//        Serial.print("yKp");
//        Serial.println(yKp);        
        break;
      }

  }
}
 if (keyPrePrevI != keyPreI)
{
  switch (keyPreI)
  {
    case KEY_1:
      {
        Ki+=0.02;
//        Serial.print("yKi");
//        Serial.println(yKi);       
        break;
      }


      
    case KEY_3:
      {
        Ki-=0.02;
//        Serial.print("yKi");
//        Serial.println(yKi);        
        break;
      }

  }
}
if (keyPrePrevD != keyPreD)
{
  switch (keyPreD)
  {
    case KEY_1:
      {
        Kd+=0.02;
//        Serial.print("yKd");
//        Serial.println(yKd);
        break;
      }

    case KEY_3:
      {
        Kd-=0.02;
        break;
      }

  }
 }
}

/******* function *******/

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

  if (values[0].equals("order")) {

    Serial.println("*show all orders:  /order/");
    Serial.println("*top:              /top/");
    Serial.println("*bottom:           /bottom/");
    Serial.println("*testMode(true):   /mode/t/");
    Serial.println("*testMode(false):  /mode/f/");

    Serial.println("============================================");

  }


  //上升到预设高度
  else if (values[0].equals("top")) {
    for (int i = 0; i < 3; i++)
    {
      steppers[i].moveTo(-400);
    }
  }
  //下降到起点
  else if (values[0].equals("bottom")) {
    for (int i = 0; i < 3; i++)
    {
      steppers[i].moveTo(0);
    }

  }

  //调试模式
  else if (values[0].equals("mode")) {

    if (values[1].equals("t"))
      testMode = true;
    else if (values[1].equals("f"))
      testMode = false;
  }

  else if (values[0].equals("pid")) {

    Serial.print("PID:");
    Serial.print(Kp);
    Serial.print(":");
    Serial.print(Ki);
    Serial.print(":");    
    Serial.println(Kd);

  }

  Serial.println(buffer);
  buffer = "";
}



void ctrlPlatfAngle(Point point){
  
  int len = 1;

    float tx = targetP.x+(point.x-targetP.x);
    float ty = targetP.y+(point.y-targetP.y); 
    pushPoint(pointToAngles(tx,ty));
        
}
Angle pointToAngles(float tx,float ty)
{
    float tiltAngle;
    float dirAngle;
  
    float px = tx;
    float py = ty;
    float ppowL = px * px + py * py;
    double pL= sqrt(ppowL);
    pItem = pL;

    iItem = (pL>2)?0:(pL<0.5)?0:(iItem+ppowL);
//    iItem = (pL>2&&iItem>2)?0:(iItem+ppowL);


    float dx = tx-lastPoint.x;
    float dy = ty-lastPoint.y;
    float dpowL = dx * dx + dy * dy;
    double dL = sqrt(dpowL);
    dItem = dL;
//
//    Serial.println("tx");
//    Serial.print(tx);
//    Serial.print(":");
//    Serial.println(ty);
//
//    Serial.println("lastPoint");
//    Serial.print(lastPoint.x);
//    Serial.print(":");
//    Serial.println(lastPoint.y);    
//
//    Serial.println("dx");
//    Serial.print(dx);
//    Serial.print(":");
//    Serial.print(dy);
//    Serial.print(":");
//    Serial.println(atan2(dy, dx) * deg + 180);

//    Serial.println("pid item");
//    Serial.print(pItem);
//    Serial.print(":");
//    Serial.print(iItem);
//    Serial.print(":");
//    Serial.println(dItem);
    
   
      dirAngle = (pL>1)?(atan2(dy, dx) * deg + 180):(atan2(py, px) * deg + 180);//方向角，速度的方向  
      tiltAngle = (pL>1)?(Kp*pItem+Kd*dItem):(Kp*pItem+Ki*iItem);//倾斜角
//      tiltAngle = (pL>1)?(Kp*pItem+Kd*dItem):0.5;//倾斜角

//      dirAngle = (atan2(dy, dx) * deg + 180);
//      tiltAngle =(Kp*pItem+Kd*dItem);
//
//       if((int)pL>0)
//      Serial.println(pL);
      
//    Serial.print("dirAngle:");
//    Serial.println(dirAngle);
//
//    Serial.print("tiltAngle:");
//    Serial.println(tiltAngle);
//
//    Serial.print("pL:");
//    Serial.println(pL);
//
//    Serial.print("dL:");
//    Serial.println(dL);
  
  //获取变化量,并算出电机应转角度
  float downVal;
  float sinA = sin(tiltAngle * rad);
  for(int i = 0;i<3;i++){
    
    float diffAngle = fabs(motorDirAngle[i] - dirAngle);   
    float cosA = cos(diffAngle * rad);

    downVal = R * cosA * sinA;
  
  motorMoveAngle.m[i] = atan(-downVal*0.1414 ) * deg+45;/// 0.1414=1/sqrt(50)
  }
  
  return motorMoveAngle;
    
}


void pushPoint(Angle p)
{
  if(lastAngle.m[0]!=p.m[0]||lastAngle.m[1]!=p.m[1]||lastAngle.m[2]!=p.m[2]){ 
    moveToPos(p);
  }
  
  lastAngle = p;
    
}

void moveToPos(Angle a){
  
   int pos;
  for(int i = 0;i<3;i++){  
    pos = -floor(microstep * a.m[i] *0.556); //这里的负号是根据电机转向添加的//0.556=1/1.8
    steppers[i].moveTo(pos);
  
  }
    
}

//触摸板点的获取
Point getPoint()
{
  Point point;
  pinMode(xLow, OUTPUT);
  pinMode(xHigh, OUTPUT);
  
  digitalWrite(xLow, LOW);
  digitalWrite(xHigh, HIGH);

  digitalWrite(yLow, LOW);
  digitalWrite(yHigh, LOW);

  pinMode(yLow, INPUT);
  pinMode(yHigh, INPUT);

  float x = (analogRead(yLow) > 10) ? analogRead(yLow) : 511.5024;
  //  float x = analogRead(yLow);

  pinMode(yLow, OUTPUT);
  pinMode(yHigh, OUTPUT);
  
  digitalWrite(yLow, LOW);
  digitalWrite(yHigh, HIGH);

  digitalWrite(xLow, LOW);
  digitalWrite(xHigh, LOW);

  pinMode(xLow, INPUT);
  pinMode(xHigh, INPUT);
  //delay(10);

  float y = (analogRead(xLow) > 30) ? analogRead(xLow) : 496.02189;//过滤
//     float y=analogRead(xLow);

//    Serial.print(x);
//    Serial.print(":");
//    Serial.println(y);


  point.x = 0.0201 * x - 10.2812; //x= A1*X+C1;
  point.y = 0.0137 * y - 6.7955; //y = A2*Y+C2;

//      Serial.println("==========point=========");
//      Serial.print(point.x);
//      Serial.print(":");
//      Serial.println(point.y);

  return point;

}


void spin(float tiltAngle)
{
   float downVal;
   float sinA = sin(tiltAngle * rad);
   testAngle+=2;
    for(int i = 0;i<3;i++){
    
    float diffAngle = fabs(motorDirAngle[i] - testAngle);   
    float cosA = cos(diffAngle * rad);

    downVal = R * cosA * sinA; 
  motorMoveAngle.m[i] = atan(-downVal*0.1414 ) * deg+45;/// 0.1414=1/sqrt(50)
  }
  pushPoint(motorMoveAngle);
  if (testAngle == 180)
    testAngle = -testAngle;//从-180到0度
}
void setup() {


  Serial.begin(115200);
  //电机初始设置
  for (int i = 0; i < 3; i++) {
    steppers[i].setMaxSpeed(1000);
    steppers[i].setAcceleration(6000);
    steppers[i].setMicroStep(microstep);
    steppers[i].enableOutputs();
   // steppers[i].setSpeed(200);   
    steppers[i].moveTo(-400);

  }
  delay(100);

}

void loop() { 

//  隔一段时间采集一次坐标
  if (micros() - lastTime > runTime) {  
      testPidParams();   
    currentPos = getPoint();
     // spin(20);
    ctrlPlatfAngle(currentPos);
    lastPoint = currentPos;

    lastTime = micros();

  }
//    testPidParams(); 
//    currentPos = getPoint();




    for (int i = 0; i < 3; i++) {
    steppers[i].run();
  }    


  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      parseBuffer();
    } else {
      buffer += c;
    }
  }

  
}








