#include <MeOrion.h>

MePort port(4);
Servo servo1;
Servo servo2;

void platform(float angleVal1,float angleVal2,float delayVal1,float delayVal2);


void setup(){
    servo1.attach(port.pin1());
    servo2.attach(port.pin2());	
	  platform(90,90,0.1,0.1);//初始位置
    delay(1000*2);
    
}

void loop(){

   for(int angle1=0-45;angle1<=180;angle1+=45)
     for(int angle2=180+45;angle2>=0;angle2-=45)
      platform(angle1,angle2,0.5,0.5
      );

    platform(90,90,0.5,0.5);//初始位置
    delay(1000*3);
    
   for(int angle2=0-45;angle2<=180;angle2+=45)
     for(int angle1=180+45;angle1>=0;angle1-=45)
      platform(angle1,angle2,0.5,0.5);
   
}

void platform(float angleVal1,float angleVal2,float delayVal1,float delayVal2)
{
	
	  servo1.write(angleVal1);
    delay(1000*delayVal1);
    servo2.write(angleVal2);
    delay(1000*delayVal2);

}


