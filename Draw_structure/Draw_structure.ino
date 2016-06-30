#include <MeOrion.h>
#include <math.h>
#

MePort port(4);

Servo servo1;//inside
Servo servo2;//outside

const float L1=13.0;//armLength,inside
const float L2=10.0;//armLength,outside
const int accuracy = 10;

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
Angle pointToangle(float,float);//
Point angleTopoint(float,float);//

void drawLine(Point startpos,Point endpos);
void drawCircle(Point centre,float radius);

void setup() {
    servo1.attach(port.pin1());
    servo2.attach(port.pin2());  
    
    Serial.begin(9600);
//  //验证
//    Serial.println(pointToangle(0,26.0).angleVal1);
//    Serial.println(pointToangle(0,26.0).angleVal2);
//
//    Serial.println(pointToangle(26,0).angleVal1);
//    Serial.println(pointToangle(26,0).angleVal2);
//
//    Serial.println(pointToangle(-26,0).angleVal1);
//    Serial.println(pointToangle(-26,0).angleVal2);
//
//    Serial.println(pointToangle(-4,20).angleVal1);
//    Serial.println(pointToangle(-4,20).angleVal2);
//
//    Serial.println(pointToangle(4,20).angleVal1);
//    Serial.println(pointToangle(4,20).angleVal2);    
//       
  //Circle
  Point centre = {0,20};
  float radius = 3;
  drawCircle(centre,radius);

}

void loop() {
////Line
//  Point startpos,endpos;
//  startpos = {-5,20};
//  endpos = {5,20};
//  drawLine(startpos,endpos);
//  delay(1000);
//  drawLine(endpos,startpos);




//  Angle angletest = {90,90};
//  moveTopoint(angletest);//调整为正前方方向

}


void moveTopoint(Angle angle)
{
    
    servo1.write(angle.angleVal1);
    servo2.write(angle.angleVal2);
    delay(50);
   
    
  }

Angle pointToangle(float x,float y)
{
    Angle angle;
  
    angle.angleVal2=acos((x*x+y*y-L1*L1-L2*L2)/(2*L1*L2))*(180/3.14)+90;
    angle.angleVal1=atan2(y,x)*(180/3.14)-atan2(L2*sin((angle.angleVal2-90)*(3.14/180)),(L1+L2*cos((angle.angleVal2-90)*(3.14/180))))*(180/3.14);

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
  moveTopoint(angle);
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
    
    
    moveTopoint(angle);         //移动
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
  
  angle = pointToangle(centre.xVal,centre.yVal);
   
  moveTopoint(angle);
  
  pos={centre.xVal+radius,centre.yVal}; 
  angle = pointToangle(pos.xVal,pos.yVal);
  moveTopoint(angle);

    for(int i=0 ;i<accuracy;i++)
    {
    pos.xVal=pos.xVal+(pos.xVal-2*radius)/(accuracy-1);
    pos.yVal=sqrt(radius*radius-(pos.xVal-centre.xVal)*(pos.xVal-centre.xVal))+centre.yVal;

//
    Serial.println("===========cal1==============="); 
    Serial.println(pos.xVal);
    Serial.println(centre.xVal);   
    Serial.println(radius*radius);
    Serial.println((pos.xVal-centre.xVal)*(pos.xVal-centre.xVal));
    Serial.println(sqrt(radius*radius-(pos.xVal-centre.xVal)*(pos.xVal-centre.xVal)));
    
    Serial.println("===========point1===============");    
    Serial.println(pos.xVal);
    Serial.println(pos.yVal);

    
    angle = pointToangle(pos.xVal,pos.yVal);          //位置转换为角度

    Serial.println("===========angle1===============");    
    Serial.println(angle.angleVal1);
    Serial.println(angle.angleVal2);
    
    moveTopoint(angle);   
  }

  //转接点
  pos={centre.xVal-radius,centre.yVal}; 
  angle = pointToangle(pos.xVal,pos.yVal);
  moveTopoint(angle);
  
  //另一半圆
    for(int i=0 ;i<accuracy;i++)
    {
    pos.xVal=pos.xVal-(pos.xVal-2*radius)/(accuracy);
    pos.yVal= -sqrt(radius*radius-(pos.xVal-centre.xVal)*(pos.xVal-centre.xVal))+centre.yVal;

    Serial.println("===========cal2==============="); 
    Serial.println(pos.xVal);
    Serial.println(centre.xVal);   
    Serial.println(radius*radius);
    Serial.println((pos.xVal-centre.xVal)*(pos.xVal-centre.xVal));
    Serial.println(-sqrt(radius*radius-(pos.xVal-centre.xVal)*(pos.xVal-centre.xVal)));
    
    Serial.println("===========point2===============");    
    Serial.println(pos.xVal);
    Serial.println(pos.yVal);
    
    angle = pointToangle(pos.xVal,pos.yVal);          //位置转换为角度
    Serial.println("===========angle2===============");    
    Serial.println(angle.angleVal1);
    Serial.println(angle.angleVal2);
    
    moveTopoint(angle);   
  }
   
  }
