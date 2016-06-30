#platform
######可以360度旋转，程序有接口函数，可以调舵机速度（通过延时），后期可以调用该函数接入蓝牙，wifi，遥感等模块
#Draw_structure
######1.初始设置，两个舵机角度都为90度，成一条直线
######2.通过计算，将点的坐标转换为舵机转动的角度（函数pointToangle()）
######3.添加了串口控制，可以通过输入点的坐标，控制机械臂移动到对应的位置点
######3.示例1:绘制直线——输入起点和终点坐标，通过直线公式算出一定数量的点，绘制这些点，形成一条直线
######4.示例2:绘制圆——输入圆心坐标和半径，通过圆的公式计算出一定数量的点，绘制这些点，形成圆(存在的问题:无法画成一个比较好看的圆，出现抖动)

Servo_application/pic/pic1.jpg
![图片1](https://github.com/aiolosfly/Servo_application/blob/master/pic/pic1.jpg) 
![图片1](https://github.com/aiolosfly/Servo_application/blob/master/pic/pic2.jpg) 