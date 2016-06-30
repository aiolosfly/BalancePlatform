#include <MeOrion.h>
#include <math.h>

int value = 0;
String buffer = "";


MePort port(4);

Servo servo1;//inside
Servo servo2;//outside

const float L1=13.0;//armLength,inside
const float L2=10.0;//armLength,outside
const int accuracy = 20;

struct Angle
{
  float angleVal1;//inside
  float angleVal2; //outside
  };

struct Point
{
  float xVal;
  float yVal;
  
  };
void moveTopoint(Angle);
Angle pointToangle(float,float);
Point angleTopoint(float,float);
Angle angles[50];
void drawLine(Point startpos,Point endpos);
void drawCircle(Point centre,float radius);

void runAngle();

void parseBuffer();

long lastTime = 0; 
int runTime = 0;
int len = 0;
Angle currentAngle;
Angle targetAngle;
void setup() {
    Serial.begin(9600);
    Serial.println("order:/pos/x/y/");
    Serial.println("please input the point(x,y):");
    
    servo1.attach(port.pin1());
    servo2.attach(port.pin2());     
    
    targetAngle.angleVal1 = 90;
    targetAngle.angleVal2 = 90;
    currentAngle.angleVal1 = 90;
    currentAngle.angleVal2 = 90;
  
//     Point startpos,endpos;
//    startpos = {-5,20};
//    endpos = {5,20};
//    drawLine(startpos,endpos);

  //Circle
  Point centre = {0,20};
  float radius = 2;
  drawCircle(centre,radius);

 



}

void loop() {

    runAngle();

   if(Serial.available()){
    char c = Serial.read();
    if(c=='\n'){
      parseBuffer();
    }else{
      buffer+=c;
    }
  }

    
}

void parseBuffer(){
  
  buffer = buffer+"/";
  int count = 0;
  int startIndex = 0;
  int endIndex = 0;
  int len = buffer.length();
  if(len<1){
    return;
  }
  String tmp;
  String values[5];
  while(true) {
    startIndex = buffer.indexOf("/", endIndex);
    endIndex = buffer.indexOf("/", startIndex + 1);
    tmp = buffer.substring(startIndex+1, endIndex);
    values[count] = tmp;
    count++;
    if(endIndex==len-1) break;
  }
  
  if(values[0].equals("pos")){
    setpoint(values[1],values[2]);
  }
  
  Serial.println(buffer);
  buffer = "";
}

void setpoint(String x,String y)
{
  Serial.println();
  Serial.print("Move to the point:");
  Serial.print("(");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.println(")");
  
  Point pos;
  Angle angle;
  pos.xVal = x.toFloat();
  pos.yVal = y.toFloat();
  angle = pointToangle(pos.xVal,pos.yVal);
  pushTopoint(angle);
    
  }

void moveTopoint(Angle angle)
{
  targetAngle = angle;
}
void runAngle(){
    if(micros()-lastTime>runTime){
      int dA1=(targetAngle.angleVal1-currentAngle.angleVal1);
      dA1 = dA1>0?-1:(dA1<0?1:0);
      int dA2=(targetAngle.angleVal2-currentAngle.angleVal2);
      dA2 = dA2>0?-1:(dA2<0?1:0);
      currentAngle.angleVal1 -= dA1;
      currentAngle.angleVal2 -= dA2;
      servo1.write(currentAngle.angleVal1);
      servo2.write(currentAngle.angleVal2);
      lastTime = micros();
      runTime = max(abs(dA1),abs(dA2))*20000;
      if(dA1==0&&dA2==0){
        callNextAngle();
      }
    }
  }
void callNextAngle(){
  if(len>0){
    moveTopoint(angles[0]);
    for(int i=1;i<len;i++){
      angles[i-1]=angles[i];
    }
    len--;
  }
}
void pushTopoint(Angle angle){
  angles[len] = angle;
  len++;
}
Angle pointToangle(float x,float y)
{
    Angle angle;
  
    angle.angleVal2=acos((x*x+y*y-L1*L1-L2*L2)/(2*L1*L2))*(180/3.14)+90;
    angle.angleVal1=atan2(y,x)*(180/3.14)-atan2(L2*sin((angle.angleVal2-90)*(3.14/180)),(L1+L2*cos((angle.angleVal2-90)*(3.14/180))))*(180/3.14);

//    if(angle.angleVal1>180||angle.angleVal2<0||angle.angleVal2>180||angle.angleVal2<0){
//      Serial.println("Can not draw!");
//      angle = {90,90};  //回到起点 
//      }
    return angle;
  }

 Point angleTopoint(float angleVal1,float angleVal2)
 {
  Point point;
  
  point.xVal = L1*cos(angleVal1*(3.14/180))+L2*cos(angleVal1*(3.14/180)+angleVal2*(3.14/180));
  point.yVal = L1*sin(angleVal1*(3.14/180))+L2*sin(angleVal1*(3.14/180)+angleVal2*(3.14/180));
  
  return point;
  }

void drawLine(Point startpos,Point endpos)
{
  float k = (endpos.yVal-startpos.yVal)/(endpos.xVal-startpos.xVal); //斜率
  Angle angle;
  angle = pointToangle(startpos.xVal,startpos.yVal);  
  pushTopoint(angle);
  Point pos=startpos;
  for(int i=1;i<=10;i++)
  {
    pos.xVal=pos.xVal+(endpos.xVal-startpos.xVal)/10;              //取点
    pos.yVal=k*(pos.xVal-startpos.xVal)+startpos.yVal;   //直线方程
    Serial.println("===========point===============");    
    Serial.println(pos.xVal);
    Serial.println(pos.yVal);


    angle = pointToangle(pos.xVal,pos.yVal);          //位置转换为角度
    
    Serial.println("===========angle===============");    
    Serial.println(angle.angleVal1);
    Serial.println(angle.angleVal2);
    
    
    pushTopoint(angle);         //移动
    }  
}
void drawCircle(Point centre,float radius)
{
  Angle angle;
  Point pos;
  
  if((centre.xVal)*(centre.xVal)+(centre.yVal)*(centre.yVal)>(L1+L2)*(L1+L2)
  ||(centre.xVal)*(centre.xVal)+(centre.yVal)*(centre.yVal)<(L1*L1+L2*L2))
  {
    Serial.println("centre is Wrong!");
    }
    else if(sqrt(centre.xVal*centre.xVal+centre.yVal*centre.yVal)+radius>(L1+L2))
  {
    
    Serial.println("radius is Wrong!");
    
    }
  
  pos={centre.xVal+radius,centre.yVal}; 
    Serial.println("===========point_start===============");    
    Serial.println(pos.xVal);
    Serial.println(pos.yVal);
  angle = pointToangle(pos.xVal,pos.yVal);
    Serial.println("===========angle_start===============");    
    Serial.println(angle.angleVal1);
    Serial.println(angle.angleVal2);
   pushTopoint(angle);

    for(int i=0 ;i<accuracy;i++)
    {
    pos.xVal=pos.xVal+(-2*radius)/(accuracy);
//    Serial.println("");
//    Serial.println((pos.xVal-2*radius)/(accuracy-1));
    pos.yVal=sqrt(radius*radius-(pos.xVal-centre.xVal)*(pos.xVal-centre.xVal))+centre.yVal;

   
    Serial.println("===========point1===============");    
    Serial.println(pos.xVal);
    Serial.println(pos.yVal);
   
    angle = pointToangle(pos.xVal,pos.yVal);          //位置转换为角度

    Serial.println("===========angle1===============");    
    Serial.println(angle.angleVal1);
    Serial.println(angle.angleVal2);
//    
    pushTopoint(angle);
  }

  //转接点
//  pos={centre.xVal-radius,centre.yVal}; 
    Serial.println("===========point_mid===============");    
    Serial.println(pos.xVal);
    Serial.println(pos.yVal);
//  angle = pointToangle(pos.xVal,pos.yVal);
    Serial.println("===========angle_mid===============");    
    Serial.println(angle.angleVal1);
    Serial.println(angle.angleVal2);
  // pushTopoint(angle);
  
  //另一半圆
    for(int i=0 ;i<accuracy;i++)
    {
    pos.xVal=pos.xVal+(2*radius)/(accuracy);
    pos.yVal= -sqrt(radius*radius-(pos.xVal-centre.xVal)*(pos.xVal-centre.xVal))+centre.yVal;

    
    Serial.println("===========point2===============");    
    Serial.println(pos.xVal);
    Serial.println(pos.yVal);
    
    angle = pointToangle(pos.xVal,pos.yVal);          //位置转换为角度
    Serial.println("===========angle2===============");    
    Serial.println(angle.angleVal1);
    Serial.println(angle.angleVal2);
    
   pushTopoint(angle);  
  }
   
  }
