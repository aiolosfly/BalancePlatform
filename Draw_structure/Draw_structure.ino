#include <MeOrion.h>
#include <math.h>

int value = 0;
String buffer = "";

double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;

MePort port(4);

Servo servo1;//inside
Servo servo2;//outside

const double L1=13.0;//armLength,inside
const double L2=10.0;//armLength,outside
const int accuracy = 10;

struct Angle
{
  double angleVal1;//inside
  double angleVal2; //outside
};

struct Point
{
  double xVal;
  double yVal;
};
void moveTopoint(Angle);
Angle pointToangle(double,double);
Point angleTopoint(double,double);
Angle angles[20];
void drawLine(Point startpos,Point endpos);
void drawCircle(Point centre,double radius);

void setpoint(String x,String y);

void runAngle();

void parseBuffer();

long lastTime = 0; 
long runTime = 0;
int len = 0;
Angle currentAngle;
Angle targetAngle;
void setup() {
    Serial.begin(115200);
    Serial.println("order:/pos/x/y/");
    Serial.println("please input the point(x,y):");
        
    servo1.attach(port.pin1());
    servo2.attach(port.pin2());     
    
    targetAngle.angleVal1 = 90;
    targetAngle.angleVal2 = 90;
    currentAngle.angleVal1 = 90;
    currentAngle.angleVal2 = 90;
  
    delay(2000);
    setpoint(2,20);
    setpoint(-2,20);  
    setpoint(-2,16);
    setpoint(2,16);
    setpoint(2,20);

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
    setpoint(values[1].toFloat(),values[2].toFloat());
  }
  
  Serial.println(buffer);
  buffer = "";
}

void setpoint(float x,float y)
{
  Serial.println();
  Serial.print("Move to the point:");
  Serial.print("(");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.println(")");
  
  Angle angle = pointToangle(x,y);
  pushTopoint(angle);
}

void moveTopoint(Angle angle)
{
  targetAngle = angle;
}
void runAngle(){
    if(micros()-lastTime>runTime){
      float dA1=(targetAngle.angleVal1-currentAngle.angleVal1);
      float dA2=(targetAngle.angleVal2-currentAngle.angleVal2);
      if(abs(dA1)>abs(dA2)){
        dA1 = dA1>0.5?(-abs(dA1)/abs(dA2)):(dA1<-0.5?(abs(dA1)/abs(dA2)):0);
        dA2 = dA2>0.5?-1:(dA2<-0.5?1:0);
      }else if(abs(dA1)<abs(dA2)){
        dA2 = dA2>0.5?(-abs(dA2)/abs(dA1)):(dA2<-0.5?(abs(dA2)/abs(dA1)):0);
        dA1 = dA1>0.5?-1:(dA1<-0.5?1:0);
      }else{
        dA1 = dA1>0.5?-1:(dA1<-0.5?1:0);
        dA2 = dA2>0.5?-1:(dA2<-0.5?1:0);
      }
      
      currentAngle.angleVal1 -= dA1;
      currentAngle.angleVal2 -= dA2;
      servo1.write(currentAngle.angleVal1);
      servo2.write(currentAngle.angleVal2);
      lastTime = micros();
      runTime = 45000;
      if(dA1==0||dA2==0){
        currentAngle.angleVal1 = targetAngle.angleVal1;
        currentAngle.angleVal2 = targetAngle.angleVal2;
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
float rads = 180/PI;
Angle pointToangle(double x,double y)
{
    Angle angle;
    
    angle.angleVal2=acos((x*x+y*y-L1*L1-L2*L2)/(2*L1*L2))*(rads)+90;
    angle.angleVal1=(atan2(y,x)-atan2(L2*sin((angle.angleVal2-90)/(rads)),(L1+L2*cos((angle.angleVal2-90)/rads))))*(rads);
    return angle;
  }

 Point angleTopoint(double angleVal1,double angleVal2)
 {
  Point point;
  
  point.xVal = L1*cos(angleVal1*(3.14/180))+L2*cos(angleVal1*(3.14/180)+angleVal2*(3.14/180)-90*(3.14/180));
  point.yVal = L1*sin(angleVal1*(3.14/180))+L2*sin(angleVal1*(3.14/180)+angleVal2*(3.14/180)-90*(3.14/180));
  
  return point;
  }

void drawLine(Point startpos,Point endpos)
{
  Angle angle;
  Point pos=startpos;
  angle = pointToangle(startpos.xVal,startpos.yVal);  
  pushTopoint(angle);
  
  if(endpos.xVal!=startpos.xVal){
  double k = (endpos.yVal-startpos.yVal)/(endpos.xVal-startpos.xVal); //斜率
  
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
  else{
  for(int i=1;i<15;i++)//垂直的情况
    {
      pos.xVal=startpos.xVal+pow(-1,(double)i)*0.2;
      pos.yVal=pos.yVal+(endpos.yVal-startpos.yVal)/20;
      Serial.println("===========point===============");    
      Serial.println(pos.xVal);
      Serial.println(pos.yVal);
      angle=pointToangle(pos.xVal,pos.yVal);
      Serial.println("===========angle===============");    
      Serial.println(angle.angleVal1);
      Serial.println(angle.angleVal2);
      pushTopoint(angle);
      
      }
  }
}
void drawCircle(Point centre,double radius)
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

    for(int i=0;i<=360;i=i+360/accuracy)
    {
      pos.xVal=centre.xVal+radius*cos(i*angle_rad);
      pos.yVal=centre.yVal+radius*sin(i*angle_rad);

      Serial.println("==============circle=============");
      Serial.println((pos.xVal-centre.xVal)*(pos.xVal-centre.xVal)+(pos.yVal-centre.yVal)*(pos.yVal-centre.yVal));
      Serial.println(radius*radius);

      Serial.println("==============circle_angle=============");
      Serial.println(i);
      Serial.println(i*angle_rad);

      Serial.println("==============point===============");
      Serial.println(pos.xVal);
      Serial.println(pos.yVal);

      angle=pointToangle(pos.xVal,pos.yVal);
      Serial.println("==============angle===============");
      Serial.println(angle.angleVal1);
      Serial.println(angle.angleVal2);      
      pushTopoint(angle);
      
    }
    
  }
