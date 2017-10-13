//ICC-AVR application builder : 2006-11-23 22:56:28
// Target : M128
// Crystal: 8.0000Mhz
/*  
   comunicating protocol with the radio controller:
   发送8位二进制数
   1表示接上
   0表示松开
   eg:   1111   X  X  X  X
                前 后 左 右
*/  

/*
*
*  手动模式和自动模式的切换：unsigned char is_uart=0;//若是想屏蔽掉开电运行的自动模式，置此标志位为1
*
*
*
*/
#include <iom128v.h>
#include <macros.h>
#include "TWI_Master.h"

#define FLK 8 	   	  	 //晶振 crystal
#define PI 3.1416        //**圆周率
#define Joint_Num  3

#define SLA_W 0xA0
#define SLA_R 0xA1

////////////////////////////----i2c地址--BEGIN----///////////////////////
//有些i2c一次可以写16个，而有些一次只能写8个，为了通用性，此处每次写入8个
//读不受限制
#define ADD_i 0x10//ID*1
#define ADD_f 0x20//频率*1
#define ADD_t 0x30//调直*3
#define ADD_s1 0x40//速度*16
#define ADD_s2 0x48
#define ADD_d 0x50//滞后角*3
#define ADD_1 0x60//幅度*48
#define ADD_2 0x68
#define ADD_3 0x70
#define ADD_4 0x78
#define ADD_5 0x80
#define ADD_6 0x88
#define ADD_10 0x90//转角偏移*24
#define ADD_20 0x98
#define ADD_30 0xa0
////////////////////////////----i2c地址--END----///////////////////////
#define PWM_Period_ms 20	     //ms
#define PWM_Duty_Middle_us 1500  //us
#define PWM_Duty_LeftLimit_us 900  //us
#define PWM_Duty_RightLimit_us 2100  //us

#define D_MAX 60  //Dynamic_Offset_degree_origin[0]的最大值
#define D_Y_MAX 60  //Dynamic_Offset_degree_y[0]的最大值

#define A_MAX 20 //*0.85=Amplitude_degree_tem[0]的最大值

#define m_time 0x10 //模态变化中控制时间
#define y_time 0x2 //原始转弯模式中控制时间//每秒大概加2


int LeftLimit_value, RightLimit_value, MiddlePosition_value,PWM_Period_value;
unsigned char Speed=0;   
unsigned char Instant=0;
unsigned char redata;//串口数据
unsigned char redata_fan=0;//是速度或方向指令时，存储该指令
unsigned char id_fan=0;//由FISHID[0]和redata_fan组成的1个字节数，返回值
unsigned char redata_tem;//命令数据暂存
unsigned char redata_i2c;//i2c命令数据暂存
unsigned char baocun_i=0;
unsigned char t_dynamic=0;//转弯命令后，从原状态到新状态的过渡时间，收到命令后的0到Speed_dynamic/2时间段为过渡状态
unsigned char Speed_dynamic=0;//收到转弯命令时刻的Speed值

unsigned char n=0;
unsigned char m=0;//测试时循环发数据用
unsigned char s=0;//拆分数组子函数中使用

//////////////----需要写入i2c的数据--BEGIN----//////////////////
unsigned char FISHID[1]={0x01};//地址ADD_i
unsigned char FISHID_init[1]={0x01};
unsigned char FISHID_I2C[3]={0,0,0x01};

unsigned char freqs[1]={0};//地址ADD_f,freq[freqs[x]]
unsigned char freqs_init[1]={0};
unsigned char freqs_I2C[3]={0,0,0};

unsigned char Static_Offset_degree[3]={0,0,0};//地址ADD_t
unsigned char Static_Offset_degree_init[3]={0,0,0};
unsigned char degree_I2C[5]={0,0,0,0,0};

//Speed_tem[16]//地址ADD_s1,s2
unsigned char Speed_tem_init[16]={0,255,240,225,210,195,180,165,150,135,120,105,90,75,60,45};
unsigned char Speed_tem_I2C[18]={0};  

unsigned char Speed_tem_init1[8]={0};
unsigned char Speed_tem_init2[8]={0};

unsigned char Speed_tem_z[16]={0};//Dynamic_Offset_degree_origin[8][3]直接转成的1维数组
unsigned char Speed_tem_z1[8]={0};//0-7
unsigned char Speed_tem_z2[8]={0};//8-16

unsigned char Speed_tem_I2C1[10]={0};
unsigned char Speed_tem_I2C2[10]={0};

//Neighbouring_Offset_degree[3]//地址ADD_d
unsigned char Neighbouring_Offset_degree_init[3]={0,80,120};
unsigned char Neighbouring_Offset_degree_I2C[5]={0};


//Amplitude_degree_tem[16][3]//地址ADD_1,2,3,4,5,6
unsigned char Amplitude_degree_tem_init[48]=
              {
			    0.1*A_MAX,0.1*A_MAX+10,0.1*A_MAX+15,//0 ，速度小摆幅小
                0.15*A_MAX,0.15*A_MAX+10,0.15*A_MAX+15,//1
                0.2*A_MAX,0.2*A_MAX+10,0.2*A_MAX+15,//
                0.25*A_MAX,0.25*A_MAX+10,0.25*A_MAX+15,//3
                0.3*A_MAX,0.3*A_MAX+10,0.3*A_MAX+15,//
                0.35*A_MAX,0.35*A_MAX+10,0.35*A_MAX+15,//5
                0.4*A_MAX,0.4*A_MAX+10,0.4*A_MAX+15,//
                0.45*A_MAX,0.45*A_MAX+10,0.45*A_MAX+15,//7
                0.5*A_MAX,0.5*A_MAX+10,0.5*A_MAX+15,//8******
                0.55*A_MAX,0.55*A_MAX+10,0.55*A_MAX+15,//9
                0.6*A_MAX,0.6*A_MAX+10,0.6*A_MAX+15,//
                0.65*A_MAX,0.65*A_MAX+10,0.65*A_MAX+15,//11
                0.7*A_MAX,0.7*A_MAX+10,0.7*A_MAX+15,//
                0.75*A_MAX,0.75*A_MAX+10,0.75*A_MAX+15,//13
                0.8*A_MAX,0.8*A_MAX+10,0.8*A_MAX+15,//
                0.85*A_MAX,0.85*A_MAX+10,0.85*A_MAX+15//15，速度大摆幅大
			  };
			  //unsigned char Amplitude_degree_tem_I2C[50]={0};
unsigned char Amplitude_degree_tem_init1[8]={0};
unsigned char Amplitude_degree_tem_init2[8]={0};
unsigned char Amplitude_degree_tem_init3[8]={0};
unsigned char Amplitude_degree_tem_init4[8]={0};
unsigned char Amplitude_degree_tem_init5[8]={0};
unsigned char Amplitude_degree_tem_init6[8]={0};
unsigned char Amplitude_degree_tem_z[48]={0};//Amplitude_degree_tem[16][3]直接转成的1维数组
unsigned char Amplitude_degree_tem_z1[8]={0};//0-11
unsigned char Amplitude_degree_tem_z2[8]={0};//12-23
unsigned char Amplitude_degree_tem_z3[8]={0};//24-35
unsigned char Amplitude_degree_tem_z4[8]={0};//36-47
unsigned char Amplitude_degree_tem_z5[8]={0};//36-47
unsigned char Amplitude_degree_tem_z6[8]={0};//36-47
unsigned char Amplitude_degree_tem_I2C1[10]={0};
unsigned char Amplitude_degree_tem_I2C2[10]={0};
unsigned char Amplitude_degree_tem_I2C3[10]={0};
unsigned char Amplitude_degree_tem_I2C4[10]={0};
unsigned char Amplitude_degree_tem_I2C5[10]={0};
unsigned char Amplitude_degree_tem_I2C6[10]={0};

//Dynamic_Offset_degree_origin [8][3],地址ADD_10,20,30,40
unsigned char Dynamic_Offset_degree_origin_init[24]=
{
  D_MAX,D_MAX-5,0,  //右14，右15，左0，急//加负号是左
  0.8*D_MAX,0.8*D_MAX-5,0, //右13，左1，q
  0.6*D_MAX,0.6*D_MAX-5,0,  //右12，左2,w
  0.45*D_MAX,0.45*D_MAX-5,0, //右11，左3,e
  0.3*D_MAX,0.3*D_MAX-5,0,  //右10，左4.r
  0.15*D_MAX,0.15*D_MAX-5,0, //右9，左5.t
  0.1*D_MAX,0.1*D_MAX-5,0,  //右8，左6，缓.y
    0,  0,  0 //中7，不转
};
unsigned char Dynamic_Offset_degree_origin_init1[8]={0};
unsigned char Dynamic_Offset_degree_origin_init2[8]={0};
unsigned char Dynamic_Offset_degree_origin_init3[8]={0};
unsigned char Dynamic_Offset_degree_origin_z[24]={0};//Dynamic_Offset_degree_origin[8][3]直接转成的1维数组
unsigned char Dynamic_Offset_degree_origin_z1[8]={0};//0-11
unsigned char Dynamic_Offset_degree_origin_z2[8]={0};//12-23
unsigned char Dynamic_Offset_degree_origin_z3[8]={0};//12-23
unsigned char Dynamic_Offset_degree_origin_I2C1[10]={0};//
unsigned char Dynamic_Offset_degree_origin_I2C2[10]={0};//
unsigned char Dynamic_Offset_degree_origin_I2C3[10]={0};//
//////////////----需要写入i2c的数据--END----//////////////////


/////////////----定义标志位--BEGIN----//////////////////
unsigned char isSrlStart=0;//收到开始位时置1；当为1时，开始接收下一位地址码，并判断是否匹配
unsigned char isSrlStartI2c=0;//收到初始化I2c的起始位时置1；开始接收下一位地址码，并判断是否匹配
unsigned char addr=0;//地址位匹配时置1；开始接收下一位命令位
unsigned char addrI2c=0;//i2c地址位匹配时置1；开始接受下一位i2c命令位
unsigned char s_change=0;//速度改变时置1 
unsigned char d_change=0;//方向改变时置1
unsigned char d_change_t=0;//方向改变,过渡状态中为1，稳定后清零
unsigned char d_change_t_v=0;//方向改变时置1，将实时偏移角赋给last作为前一状态偏移角，然后清零该标志位
unsigned char t_change=0;//调直时置1
unsigned char ts_change=0;//保存或取消调直状态时置1
unsigned char a_change=0;//幅度改变时置1
unsigned char f_change=0;//频率改变时置1
unsigned char id_change=0;//id改变时置1
unsigned char e_change=0;//接收到结束位时置1
unsigned char i_change=0;//接收到i2c初始化命令时置为1
unsigned char i2c_change=0;//接收到i2c初始化命令的结束位时置为1
unsigned char isInstruction=0;//是否接收到指令，已接收置为1，未接收，置为0；设置个标志位，保证指令必须是四位才能够执行，
//比如aa 9_ d5 d0 fc这样的指令是无效的，但若无此标志位，这个指令就是有效的，并且执行d0命令 
unsigned char isInstructionI2c=0;//是否接收到i2c指令，已接收置为1，未接收，置为0；设置个标志位，
//保证指令必须是四位才能够执行，比如cc 74 8_ aa fc这样的指令是无效的，但若无此标志位，这个指令就是有效的，并且执行8_命令 
//以上标志位，接收到结束位，执行命令后，清0

unsigned char is_uart=1;//串口是否接收到命令，鱼刚开电时，此标志位若为0，自动运行设定的模式，当串口接受到控制命令时候，此标志位置为1，进入手动控制模式
unsigned char m_change=0;//接收到模态码时置1，接收到其它命令时清0
unsigned char m_cishu=0;//mm_cishu,mmm_cishu共同决定模态码置1后循环的次数
unsigned char mm_cishu=0;//
unsigned char mmm_cishu=m_time-1;//
unsigned char a_cishu=0;//aa_cishu,aaa_cishu共同决定自动状态循环的次数//*auto
unsigned char aa_cishu=0;//
unsigned char aaa_cishu=m_time-1;//
unsigned char cishu=0;//模态中动作循环，以3为周期，即0,1,2,0,1,2....
unsigned char auto_cishu=0;//模态中动作循环，以5为周期，即0,1,2,3,4,0,1,2,3,4....
unsigned char y_change=0;//接收到原始转弯方式时置为1，接收到其他命令时清0
unsigned char y_changing=0;//正在执行原始转弯方式时置为1，此时不接受其他命令
unsigned char yy_time=0xff;//与y_time共同决定原始转弯模式的时间
unsigned char y_cishu=0;   //
unsigned char yy_cishu=0;
unsigned char yyy_cishu=y_time-1;
unsigned char cishu_y=0;//0时转弯，1时回到收到命令前的速度，直游
unsigned char y_tem=0;//转弯档
unsigned char y_speed=0;//保存转弯前速度
/////////////----定义标志位--END----//////////////////


/////////////----各档参数--BEGIN----//////////////////
unsigned char s_tem=0;//速度档，Speed_tem[s_tem],Amplitude_degree_tem[s_tem][i]
unsigned char m_tem=0;//m_tem&0x0f=模态档
unsigned char d_tem=0;//方向档，Dynamic_Offset_degree_origin [d_tem][i]
unsigned char dd_tem=0;//方向档左右同一后，
unsigned char flag=0;
// d_tem 0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15 //7前左，后右
//dd_tem 0  1  2  3  4  5  6  7  6  5  4   3   2   1   0   0 //加负号左
////flag=1时，左转d_tem小于7，加负号，flag=0，右转d_tem大于等于7
//unsigned char y_tem=0;//原始转弯档
unsigned char i2c_canshu=0;//保存参数修改时置1

//4个频率，旧通信模块的改频率，新通信模块不用此变量
unsigned char freq[4]={0xff,0xf7,0xef,0xdf};

//16档速度值,0-15,Speed值越大，速度越小
unsigned char Speed_tem[16]=  
{
  0,255,240,225,210,195,180,165,150,135,120,105,90,75,60,45//初始值，8-150
};//最大255//最慢150比较合适

//16档转弯值，0-15,7不转
unsigned char Dynamic_Offset_degree_origin[8][Joint_Num]=
{
  {D_MAX,D_MAX-5,0},  //右14，右15，左0，急//加负号是左
  {0.9*D_MAX,0.9*D_MAX-5,0}, //右13，左1，q
  {0.8*D_MAX,0.8*D_MAX-5,0},  //右12，左2,w
  {0.7*D_MAX,0.7*D_MAX-5,0}, //右11，左3,e
  {0.5*D_MAX,0.5*D_MAX-5,0},  //右10，左4.r
  {0.3*D_MAX,0.3*D_MAX-5,0}, //右9，左5.t
  {0.1*D_MAX,0.1*D_MAX-5,0},  //右8，左6，缓.y
    {0,  0,  0} //中7，不转
};//常量数组，给出Dynamic_Offset_degree的值

//原始转弯方式，16档转弯值，0-15,7不转
int Dynamic_Offset_degree_y[16][Joint_Num] =
{
  {-D_Y_MAX,-D_Y_MAX-5,-D_Y_MAX-10},  //左0，急
  {-0.9*D_Y_MAX,-0.9*D_Y_MAX-5,-0.9*D_Y_MAX-10}, //左1
  {-0.8*D_Y_MAX,-0.8*D_Y_MAX-5,-0.8*D_Y_MAX-10},  //左2
  {-0.7*D_Y_MAX,-0.7*D_Y_MAX-5,-0.7*D_Y_MAX-10}, //左3
  {-0.5*D_Y_MAX,-0.5*D_Y_MAX-5,-0.5*D_Y_MAX-10},  //左4
  {-0.3*D_Y_MAX,-0.3*D_Y_MAX-5,-0.3*D_Y_MAX-10}, //左5
  {-0.1*D_Y_MAX,-0.1*D_Y_MAX-5,-0.1*D_Y_MAX-10},  //左6，缓
  {  0,  0,  0}, //中7，不转
  { 0.1*D_Y_MAX, 0.1*D_Y_MAX-5,0.1*D_Y_MAX-10},  //右8，缓
  { 0.2*D_Y_MAX, 0.2*D_Y_MAX-5,0.2*D_Y_MAX-10}, //右9
  { 0.3*D_Y_MAX, 0.3*D_Y_MAX-5,0.3*D_Y_MAX-10},  //右10
  { 0.5*D_Y_MAX, 0.5*D_Y_MAX-5,0.5*D_Y_MAX-10}, //右11
  { 0.7*D_Y_MAX, 0.7*D_Y_MAX-5,0.7*D_Y_MAX-10},  //右12
  { 0.8*D_Y_MAX, 0.8*D_Y_MAX-5,0.8*D_Y_MAX-10}, //右13
  { 0.9*D_Y_MAX, 0.9*D_Y_MAX-5,0.9*D_Y_MAX-10},  //右14
  { D_Y_MAX,  D_Y_MAX-5, D_Y_MAX-10} //右15，急
};//常量数组，给出Dynamic_Offset_degree的值

int Joint_Angle_value [Joint_Num] =     //the final value to be written in duty cycle register
{
  0, 0, 0
};//初始化值

int Static_Offset_value [Joint_Num] =//调直用, 单位为度
{
  0, 0, 0
};


int Dynamic_Offset_degree [Joint_Num] =// modified through communication转弯用
{
 0, 0, 0           
};//用来计算Dynamic_Offset_value

unsigned char Dynamic_Offset_degree_char[Joint_Num]={0,0,0};

int Dynamic_Offset_value [Joint_Num] =
{
 0, 0, 0
};//根据Dynamic_Offset_degree计算得到,存储收到的转弯命令的Dynamic_Offset_value值，也就是要达到的值

int Dynamic_Offset_value_per[Joint_Num]=
{
 0,0,0
};//过渡状态时，每次增加的偏移角

int Dynamic_Offset_value_last[Joint_Num]=
{
 0,0,0
};//收到转弯命令之前的Dynamic_Offset_value值

int Dynamic_Offset_value_t[Joint_Num]=
{
 0,0,0
};//实时的Dynamic_Offset_value值

unsigned char Neighbouring_Offset_degree[Joint_Num] =// modified through communication 每关节相位差
{
// 0,60,120
  0,80,120
};
//常量，绝对相位差
unsigned char Amplitude_degree[Joint_Num] =// modified through communication 各关节摆幅
{
  15,20,25
//   90,20,25
};//常量,控制摆幅，Asin(),最大90度
unsigned char Amplitude_degree_tem[16][Joint_Num] =//16档摆幅，对应速度，0-15
{
  {0.1*A_MAX,0.1*A_MAX+10,0.1*A_MAX+15},//0 ，速度小摆幅小
  {0.15*A_MAX,0.15*A_MAX+10,0.15*A_MAX+15},//1
  {0.2*A_MAX,0.2*A_MAX+10,0.2*A_MAX+15},//
  {0.25*A_MAX,0.25*A_MAX+10,0.25*A_MAX+15},//3
  {0.3*A_MAX,0.3*A_MAX+10,0.3*A_MAX+15},//
  {0.35*A_MAX,0.35*A_MAX+10,0.35*A_MAX+15},//5
  {0.4*A_MAX,0.4*A_MAX+10,0.4*A_MAX+15},//
  {0.45*A_MAX,0.45*A_MAX+10,0.45*A_MAX+15},//7
  {0.5*A_MAX,0.5*A_MAX+10,0.5*A_MAX+15},//8******
  {0.55*A_MAX,0.55*A_MAX+10,0.55*A_MAX+15},//9
  {0.6*A_MAX,0.6*A_MAX+10,0.6*A_MAX+15},//
  {0.65*A_MAX,0.65*A_MAX+10,0.65*A_MAX+15},//11
  {0.7*A_MAX,0.7*A_MAX+10,0.7*A_MAX+15},//
  {0.75*A_MAX,0.75*A_MAX+10,0.75*A_MAX+15},//13
  {0.8*A_MAX,0.8*A_MAX+10,0.8*A_MAX+15},//
  {0.85*A_MAX,0.85*A_MAX+10,0.85*A_MAX+15}//15，速度大摆幅大
};
/////////////----各档参数--END----//////////////////


//*************************delay延时函数**********************
void delay(double interval)
{
	double k;
	for(k=0;k<interval;k++);
}

void delay_1ms(void) 
{ 
 unsigned int i; 
 for (i=0;i<1142;i++) 
  { 
  } 
} 

void delay_nms(unsigned int n) 
{ 
 unsigned int i; 
 for(i=0;i<n;i++) 
 delay_1ms(); 
}


//*************************设置各端口************************
void port_init(void)
{
 PORTA = 0x00;//0000 0001
 DDRA  = 0x00;//0000 0001 PA0口输出，高电平
// PORTB = 0xFF;//0xF0;//1111 0000
// DDRB  = 0xFF;//0xF0;//1111 0000
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
// PORTD = 0xFF;//0xD0;//1101 0000
// DDRD  = 0xFF;//0xD0;//1101 0000
// PORTE = 0xFE;//1111 1110
// DDRE  = 0xFE;//1111 1110
  PORTE = 0xFE;//灯2，PE2
 DDRE  = 0xFE;
// PORTF = 0xFF;//0x00;
// DDRF  = 0xFF;//0x00;
// PORTG = 0x1F;//0x00;
// DDRG  = 0x1F;//0x00;
///////////////
// PORTB = 0xF0;//1111 0000
// DDRB  = 0xF0;//1111 0000
 PORTB = 0xF4;//灯1，PB2，&&&&&&&&&&&&&&&&&&
 DDRB  = 0xF4;
// PORTD = 0xD0;//1101 0000
// DDRD  = 0xD0;//1101 0000
 PORTD=0xfc;//0xfc;//xxxx xx00
 DDRD=0xfc;//xxxx xx00
 //PORTF = 0x00;
 //DDRF  = 0x00;
 PORTF = 0xFF;
 DDRF  = 0x0F;

 
 PORTG = 0x00;
 DDRG  = 0x00;


}

//*****************************TIMER3 初始化***************************
void timer3_init(void)
{
 TCCR3B= 0x00; //stop
 TCNT3H= 0x00 /*INVALID SETTING*/; //setup
 TCNT3L= 0x00 /*INVALID SETTING*/;
 OCR3AH= 0x00 /*INVALID SETTING*/;
 OCR3AL= 0x00;
 OCR3BH= 0x00 /*INVALID SETTING*/;
 OCR3BL= 0x00;
 OCR3CH= 0x00 /*INVALID SETTING*/;
 OCR3CL= 0x00;
 TCCR3A= 0xFE;
 TCCR3B= 0x1A; //start Timer用8分频,快速PWM模式
}

//*****************************TIMER1 初始化***************************
void timer1_init(void)
{
 TCCR1B= 0x00; //stop
 TCNT1H= 0x00 /*INVALID SETTING*/; //setup
 TCNT1L= 0x00 /*INVALID SETTING*/;
 OCR1AH= 0x00 /*INVALID SETTING*/;
 OCR1AL= 0x00;
 OCR1BH= 0x00 /*INVALID SETTING*/;
 OCR1BL= 0x00;
 OCR1CH= 0x00 /*INVALID SETTING*/;
 OCR1CL= 0x00;
 TCCR1A= 0xFE;
 TCCR1B= 0x1A; //start Timer用8分频,快速PWM模式
}




void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;       //*接受和发送数据帧的数据位字符长度保留
 
 UBRR0H = 0x00; //set baud rate hi
 UBRR0L = 0x33; //set baud rate lo
  
 UCSR0A = 0x00; //enable
 UCSR0B = 0x98;
}

//*************************initialize all peripherals***************
void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts,屏蔽所有中断
 XDIV  = 0x00; //xtal divider,系统时钟分频控制寄存器
 XMCRA = 0x00; //external memory，外部扩展存储器控制寄存器A
 port_init();//端口初始化
 //timer3_init();//定时器3初始化
 //timer1_init();//定时器1初始化
 uart0_init();//串口通信中断
 MCUCR = 0x00;//MCU控制寄存器，57页
 //EICRA = 0x03; //extended ext ints，外部中断控制寄存器A
 EICRA=0x00;
 EICRB = 0x00; //extended ext ints，外部中断控制寄存器B
 //EIMSK = 0x01;//外部中断屏蔽寄存器
 EIMSK = 0x00;
// TIMSK = 0x00; //timer interrupt sources，T/C中断屏蔽寄存器
 TIMSK = 0x44;
 ETIMSK = 0x04; //extended timer interrupt sources，扩展T/C中断屏蔽寄存器
 SEI(); //re-enable interrupts
}



 

void senddata(unsigned char data)
{
   /* 等待发送缓冲器为空 */
   while (!( UCSR0A & (1<<UDRE0)))
   ;
   /* 将数据放入缓冲器，发送数据*/
     UDR0 = data;
}


void uart_putchar(char c) //发送一个字节
{
   while( !( UCSR0A & (1<<UDRE0)))
      ;
   UDR0=c;
}

void uart_word(char String[])//发送一个字符串
{
   int i;
   while(String[i]!='\0') 
   {
      uart_putchar(String[i]);
	  i++;
   }
}


//*********************sin计算***************************
double Sin(unsigned char instant, unsigned char speed, int Offset_degree)
{

	double x,y;
	double temp;
        if(speed == 0)
          return 0;
        else
        {
          x = PI *2.0 *  ((double)instant / (double)speed) - PI * (double)Offset_degree / 180.0;       
		 //100115 驹?解决滞后角超过一定范围后，电机的抖动问题         
         if(x>(2.0*PI))
            x=x-2.0*PI;
        else if(x<0.0)
            x=x+2.0*PI;

		  if(x<=(PI/2.0))
		y=x-x*x*x/6.0;//sinx,正数

          if(x>(PI/2.0)&&x<=PI)
	   {
		temp=PI-x;
		y=temp-temp*temp*temp/6.0;//正数
	   }

          if(x>PI&&x<=(PI*3.0/2.0))
	   {
		temp=x-PI;
		y=temp*temp*temp/6.0-temp;//负数
	   }

	  if(x>(PI*3.0/2.0))
	   {
	        temp=2.0*PI-x;
	        y=temp*temp*temp/6.0-temp;//负数
	   }

	 return y;

        }
}

//**********************************静态偏移转换为控制量*************************
void init_static_offset(void)//括号内添加void
{
  unsigned char i;

  for(i=0; i<Joint_Num; i++)
  {
       Static_Offset_value[i] =((int) ((Static_Offset_degree[i] * 10) * FLK)>>3)-1;//1度10us
  }
}

//**********************************动态偏移转换为控制量*************************
void init_dynamic_offset(void)//括号内添加void
{
  unsigned char i;
  
   for(i=0; i<Joint_Num; i++)
   {
   Dynamic_Offset_value[i] = ((int) ((Dynamic_Offset_degree[i] * 10) * FLK)>>3)-1;
   }
}
//***********************************计算最终控制量******************************

void calculate_data(void)   //算出每次溢出中断改动的OCRnX值//括号内添加void
{
  unsigned char i;
  if(d_change_t_v==1)//*方向改变时置1，将实时偏移角赋给last作为前一状态偏移角
  {
     d_change_t_v=0;
	 for(i=0;i<Joint_Num;i++)
	 {
	   Dynamic_Offset_value_last[i]=Dynamic_Offset_value_t[i];
     }
  }
  if((d_change_t==1)&&(t_dynamic>0)&&(t_dynamic<=(Speed_dynamic/2)))//过渡状态
   {
      for(i=0;i<Joint_Num;i++)
	  {
	    Dynamic_Offset_value_per[i]=(Dynamic_Offset_value[i]-Dynamic_Offset_value_last[i])/(Speed_dynamic/2);
	    Dynamic_Offset_value_t[i]=Dynamic_Offset_value_last[i]+(Dynamic_Offset_value_per[i])*t_dynamic;
	  }
      for(i=0; i< Joint_Num; i++)
       Joint_Angle_value[i] =MiddlePosition_value + Static_Offset_value[i]+Dynamic_Offset_value_t[i]+  ((int) (Amplitude_degree[i] * Sin( Instant, Speed, Neighbouring_Offset_degree[i]) *10*FLK)>>3)-1;
    }
   else//稳定状态
   {
      d_change_t=0;
      for(i=0; i< Joint_Num; i++)
        Joint_Angle_value[i] =MiddlePosition_value + Static_Offset_value[i]+Dynamic_Offset_value[i]+  ((int) (Amplitude_degree[i] * Sin( Instant, Speed, Neighbouring_Offset_degree[i]) *10*FLK)>>3)-1;
   }  
}  //最终溢出中断写入寄存器的值

//*如何换算的，作用是什么？

//***********************************各时间量转换为控制量************************
void init_data(void)    // void init_data() 改为void init_data(void) wsg20120907
{
  PWM_Period_value=((int)(PWM_Period_ms*1000*FLK)>>3)-1; //pwm周期 20ms换算成计数器值
  MiddlePosition_value=((int)(PWM_Duty_Middle_us*FLK )>>3)-1; //1500us 中间位置换成计数值
  LeftLimit_value=((int)(PWM_Duty_LeftLimit_us*FLK)>>3)-1; //900us 左极限位置计数器值，60度  1度10us
  RightLimit_value=((int)(PWM_Duty_RightLimit_us*FLK)>>3)-1; //2100us 右极限位置计数器值，60度
}
	
//********************将unsigned char型数转化为int型数**************************
//flag=1时转为负数，左转；flag=0时转为正数，右转
int char2int(unsigned char a_char,unsigned char a_flag)
{
  int a_int;
  if(a_flag==0)
  {
     a_int=a_char;
  }
  if(a_flag==1)
  {
     a_int=a_char;
	 a_int=-a_int;
  }
return a_int;
}



//*********************一个16位数组，拆为2个8位数组****************************
//将长度为16位的数组data_init[16]拆成2个长度为8的数组
//分别为data_a[8],data_b[8]
//unsigned char型
void one2two(unsigned char *data_init,unsigned char *data_a,unsigned char *data_b)
{
  for(s=0;s<=7;s++)
   {
   data_a[s]=data_init[s];
   }
  for(s=8;s<=15;s++)
   {
   data_b[s-8]=data_init[s];
   }
}

//*******************将2个8位数组依顺序组成一个16位数组************************
//将2个长度为8的数组data_aa[8],data_bb[8]
//顺次组成一个长度为16位的数组data_inits[16]
//unsigned char型
void two2one(unsigned char *data_inits,unsigned char *data_aa,unsigned char *data_bb)
{
  for(s=0;s<=7;s++)
   {
   data_inits[s]=data_aa[s];//*把data_aa数值顺次赋值给data_inits
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+8]=data_bb[s];//*把data_bb数值顺次赋值给data_bb的高八位
   }
}

//*********************一个24位数组，拆为3个8位数组****************************
//将长度为32位的数组data_init[24]拆成3个长度为8的数组
//分别为data_a[8],data_b[8],data_c[8]
//unsigned char型
void one2three(unsigned char *data_init,unsigned char *data_a,unsigned char *data_b,unsigned char *data_c)
{
  for(s=0;s<=7;s++)
   {
   data_a[s]=data_init[s];//*把data_init[]的前八位形成一个新的数组data_a[]
   }
  for(s=8;s<=15;s++)
   {
   data_b[s-8]=data_init[s];//*把data_init[]第九位到第十六位形成一个新的数组data_b[]
   }
  for(s=16;s<=23;s++)
   {
   data_c[s-16]=data_init[s];//*把data_init[]第十七位到到第二十四位形成一个新的数组data_c[]
   }
}

//*******************将3个8位数组依顺序组成一个24位数组************************
//将3个长度为24的数组data_aa[8],data_bb[8],data_cc[8]
//顺次组成一个长度为32位的数组data_inits[32]
//unsigned char型
void three2one(unsigned char *data_inits,unsigned char *data_aa,unsigned char *data_bb,unsigned char *data_cc)
{
  for(s=0;s<=7;s++)
   {
   data_inits[s]=data_aa[s];
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+8]=data_bb[s];
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+16]=data_cc[s];
   }
}


//*********************一个48位数组，拆为6个8位数组****************************
//将长度为48位的数组data_init[48]拆成6个长度为8的数组
//分别为data_a[8],data_b[8],data_c[8],data_d[8],data_e[8],data_f[8]
//unsigned char型
void one2six(unsigned char *data_init,unsigned char *data_a,unsigned char *data_b,unsigned char *data_c,unsigned char *data_d,unsigned char *data_e,unsigned char *data_f)
{
  for(s=0;s<=7;s++)
   {
   data_a[s]=data_init[s];
   }
  for(s=8;s<=15;s++)
   {
   data_b[s-8]=data_init[s];
   }
  for(s=16;s<=23;s++)
   {
   data_c[s-16]=data_init[s];
   }
  for(s=24;s<=31;s++)
   {
   data_d[s-24]=data_init[s];
   }
  for(s=32;s<=39;s++)
   {
   data_e[s-32]=data_init[s];
   }
  for(s=40;s<=47;s++)
   {
   data_f[s-40]=data_init[s];
   }
}

//*******************将6个8位数组依顺序组成一个48位数组************************
//将6个长度为8的数组data_aa[8],data_bb[8],data_cc[8],data_dd[8]
//顺次组成一个长度为48位的数组data_inits[48]
//unsigned char型
void six2one(unsigned char *data_inits,unsigned char *data_aa,unsigned char *data_bb,unsigned char *data_cc,unsigned char *data_dd,unsigned char *data_ee,unsigned char *data_ff)
{
  for(s=0;s<=7;s++)
   {
   data_inits[s]=data_aa[s];
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+8]=data_bb[s];
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+16]=data_cc[s];
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+24]=data_dd[s];
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+32]=data_ee[s];
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+40]=data_ff[s];
   }
}



// d_tem 0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15 //7前左，后右
//dd_tem 0  1  2  3  4  5  6  7  6  5  4   3   2   1   0   0 //加负号左
//********************幅度档d_tem转化为左右统一的dd_tem****************
//flag_c=1时，左转d_c小于7，加负号，flag_c=0，右转d_c大于等于7
unsigned char change_dd(unsigned char d_c)
{
   unsigned char dd_c;
   if((d_c>=0)&&(d_c<=7))
   {
     dd_c=d_c;
   }
   if((d_c>=7)&&(d_c<=14))
   {
     dd_c=7-(d_c-7);//14-d_c
   }
   if(d_c==15)
   {
     dd_c=0;
   }
return dd_c;
}
unsigned char change_flag(unsigned char d_c)
{
   unsigned char flag_c;
   if(d_c<=6)
   {
   flag_c=1;
   }
   else
   {
   flag_c=0;
   }
   return flag_c;
}


//********************************读写i2c****************************************
//写i2c
//将长度为data_l的data_tem数组写入地址为i2c_address的data_i2c数组
//unsigend char型
void i2c_write(unsigned char i2c_address,unsigned char *data_i2c,unsigned char *data_tem,unsigned char data_l)
{
      int i=0;
	  data_i2c[0]=SLA_W;
	  data_i2c[1]=i2c_address;
	  for(i=0;i<=data_l-1;i++)
	  {
	     data_i2c[i+2]=data_tem[i];
	  }
	  TWI_Start_Transceiver_With_Data(data_i2c,data_l+2);
}

//读i2c
//将长度为data_l的地址为i2c_address的data_i2c数组读出到data_tem
//unsigned char型
void i2c_read(unsigned char i2c_address,unsigned char *data_i2c,unsigned char *data_tem,unsigned char data_l)
{
      int i=0;
	  data_i2c[0]=SLA_W;
	  data_i2c[1]=i2c_address;
	  TWI_Start_Transceiver_With_Data(data_i2c,2);
	  data_i2c[0]=SLA_R;
	  TWI_Start_Transceiver_With_Data(data_i2c,data_l+1);
	  TWI_Get_Data_From_Transceiver(data_i2c,data_l+1);
	  for(i=0;i<=data_l-1;i++)
	  {
	     data_tem[i]=data_i2c[i+1];
	  }			    	
}


//***********************第一次使用i2c，数据初始化*******************************
void i2c_init(void)
{	
      int i;
	  senddata(0xab);
	  switch(redata_i2c&0x0f)
	  {
	    case 0x01://FISHID 设为0x01;
		          i2c_write(ADD_i,FISHID_I2C,FISHID_init,1); 
				  break;
		case 0x02://频率设为频率1
		          i2c_write(ADD_f,freqs_I2C,freqs_init,1);
				  break;
	    case 0x03://调直数据
		          i2c_write(ADD_t,degree_I2C,Static_Offset_degree_init,3);
				  break;
		case 0x04://Speed_tem[16]速度，s_tem
		          one2two(Speed_tem_init,Speed_tem_init1,Speed_tem_init2);
		          i2c_write(ADD_s1,Speed_tem_I2C1,Speed_tem_init1,8);
				  delay_nms(200);
		          i2c_write(ADD_s2,Speed_tem_I2C2,Speed_tem_init2,8);
				  break;
	    case 0x05://Neighbouring_Offset_degree[3]滞后角
			      i2c_write(ADD_d,Neighbouring_Offset_degree_I2C,Neighbouring_Offset_degree_init,3);	         
				  break;
	    case 0x06://Amplitude_degree_tem[16][3]摆幅,s_tem
		          one2six(Amplitude_degree_tem_init,Amplitude_degree_tem_init1,Amplitude_degree_tem_init2,Amplitude_degree_tem_init3,Amplitude_degree_tem_init4,Amplitude_degree_tem_init5,Amplitude_degree_tem_init6);
				  i2c_write(ADD_1,Amplitude_degree_tem_I2C1,Amplitude_degree_tem_init1,8);//+++++++
				  delay_nms(200);
				  i2c_write(ADD_2,Amplitude_degree_tem_I2C2,Amplitude_degree_tem_init2,8);//+++++++
     			  break;
		case 0x07:
				  i2c_write(ADD_3,Amplitude_degree_tem_I2C3,Amplitude_degree_tem_init3,8);//+++++++
				  delay_nms(200);
				  i2c_write(ADD_4,Amplitude_degree_tem_I2C4,Amplitude_degree_tem_init4,8);
				  break;
		case 0x08:
				  i2c_write(ADD_5,Amplitude_degree_tem_I2C5,Amplitude_degree_tem_init5,8);
				  break;
		case 0x09:
				  i2c_write(ADD_6,Amplitude_degree_tem_I2C6,Amplitude_degree_tem_init6,8);				  
                  break;
	    case 0x0A://Dynamic_Offset_degree_origin[8][3]转弯偏移角,d_tem
		          one2three(Dynamic_Offset_degree_origin_init,Dynamic_Offset_degree_origin_init1,Dynamic_Offset_degree_origin_init2,Dynamic_Offset_degree_origin_init3);
				  i2c_write(ADD_10,Dynamic_Offset_degree_origin_I2C1,Dynamic_Offset_degree_origin_init1,8);//+++++++
     			  break;
		case 0x0B:
				  i2c_write(ADD_20,Dynamic_Offset_degree_origin_I2C2,Dynamic_Offset_degree_origin_init2,8);//+++++++
				  delay_nms(200);
				  i2c_write(ADD_30,Dynamic_Offset_degree_origin_I2C3,Dynamic_Offset_degree_origin_init3,8);
				  break;
	    case 0x0C:
				  delay_nms(200);
		         //读i2c数据
				 i2c_read(ADD_i,FISHID_I2C,FISHID,1);
     			 i2c_read(ADD_f,freqs_I2C,freqs,1);
	 			 //PORTD=freq[freqs[0]];
	 			 i2c_read(ADD_t,degree_I2C,Static_Offset_degree,3);
	 			 i2c_read(ADD_s1,Speed_tem_I2C1,Speed_tem_z1,8);
	 			 i2c_read(ADD_s2,Speed_tem_I2C2,Speed_tem_z2,8);
				 two2one(Speed_tem_z,Speed_tem_z1,Speed_tem_z2);
				 for(i=0;i<=15;i++)
				 {
                     Speed_tem[i]=Speed_tem_z[i];
				 }
	 			 i2c_read(ADD_d,Neighbouring_Offset_degree_I2C,Neighbouring_Offset_degree,3);
	 			 i2c_read(ADD_1,Amplitude_degree_tem_I2C1,Amplitude_degree_tem_z1,8);
	 			 i2c_read(ADD_2,Amplitude_degree_tem_I2C2,Amplitude_degree_tem_z2,8);
	 			 i2c_read(ADD_3,Amplitude_degree_tem_I2C3,Amplitude_degree_tem_z3,8);
	 			 i2c_read(ADD_4,Amplitude_degree_tem_I2C4,Amplitude_degree_tem_z4,8);
	 			 i2c_read(ADD_5,Amplitude_degree_tem_I2C5,Amplitude_degree_tem_z5,8);
	 			 i2c_read(ADD_6,Amplitude_degree_tem_I2C6,Amplitude_degree_tem_z6,8);
	 			 six2one(Amplitude_degree_tem_z,Amplitude_degree_tem_z1,Amplitude_degree_tem_z2,Amplitude_degree_tem_z3,Amplitude_degree_tem_z4,Amplitude_degree_tem_z5,Amplitude_degree_tem_z6);  
	 			   for(i=0;i<=47;i++)
	   			   {
	   			     Amplitude_degree_tem[i/3][i%3]=Amplitude_degree_tem_z[i];
	   			   }	
	 			 i2c_read(ADD_10,Dynamic_Offset_degree_origin_I2C1,Dynamic_Offset_degree_origin_z1,8);
	 			 i2c_read(ADD_20,Dynamic_Offset_degree_origin_I2C2,Dynamic_Offset_degree_origin_z2,8);
	 			 i2c_read(ADD_30,Dynamic_Offset_degree_origin_I2C3,Dynamic_Offset_degree_origin_z3,8);
	 			 three2one(Dynamic_Offset_degree_origin_z,Dynamic_Offset_degree_origin_z1,Dynamic_Offset_degree_origin_z2,Dynamic_Offset_degree_origin_z3);  
	  			   for(i=0;i<=23;i++)
	   			   {
	     		     Dynamic_Offset_degree_origin[i/3][i%3]=Dynamic_Offset_degree_origin_z[i];
	  			   }	
				senddata(0xcc);
		    	 break;
	  }
}


//***********************************串口中断*****************************
#pragma interrupt_handler uart0_rx_isr:19
void uart0_rx_isr(void) 
{
       redata=UDR0;
	   if(1)//?//判断寄存器的值，是否存在
	   {
		 if(isSrlStart)//若已经发送开始位，判断地址码是否匹配
		 {
		   if(addr)//若地址码已经匹配，准备接收命令
		   {
			   if(isInstruction==1|y_changing)//已经接受过命令了，那么判断是否是结束位
			   {
				   if(redata==0xfc)
				   { 
					   e_change=1;//
					   is_uart=1;//退出自动模式，进入手动控制模式
				   }
				   else//接收到的不是结束位，那么这条指令无效，重新接收下一次命令
				   {
			  	      isSrlStart=0;
					  addr=0;
					  isInstruction=0;
				   }
			   }
			   else//没有接受过命令，则接受命令
			   {
				   switch(redata&0xf0)
				   {
				   case 0xD0://速度
					   redata_fan=redata;
					   s_change=1;
					   m_change=0;
					   y_change=0;
					   isInstruction=1;
					   redata_tem=redata;
					   break;
				   case 0xE0://方向
			            redata_fan=redata;
						d_change=1;
						//d_change_t=1;
						//d_change_t_v=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x00://调直
			            t_change=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x10://调直是否保存
			            ts_change=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x20://更改鱼id
			            id_change++;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x40://更改频率
			            f_change=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						//PORTD=0xf4;						
						break;
				   case 0x70:///更改模态
			            m_change=1;
						y_change=0;
						m_cishu=0;
						mm_cishu=0;
						mmm_cishu=m_time-1;
						isInstruction=1;
						m_tem=redata;
						break;
				   case 0x30://原始的转弯模式
			            y_change=1;
                        y_changing=1;
						m_change=0;
						y_cishu=0;
						yy_cishu=0;
						yyy_cishu=y_time-1;
						cishu_y=0;
						y_tem=redata;
						switch(y_tem)//转弯档不同，保持的时间不同，弯越大，时间越长。
						{
						case 0:
							yy_time=0xff;
						case 1:
							yy_time=0xf0;
						case 2:
							yy_time=0xe0;
						case 3:
							yy_time=0xd0;
						case 4:
							yy_time=0xc0;
						case 5:
							yy_time=0xb0;
						case 6:
							yy_time=0xa0;
						case 15:
							yy_time=0xff;
						case 14:
							yy_time=0xff;
						case 13:
							yy_time=0xf0;
						case 12:
							yy_time=0xe0;
						case 11:
							yy_time=0xd0;
						case 10:
							yy_time=0xc0;
						case 9:
							yy_time=0xb0;
						case 7:
							y_change=0;
						}
						y_speed=Speed;					
						isInstruction=1;
						break;
/*			       case 0xb0://更改幅度
			            a_change=1;
						m_change=0;
						y_change=0;
						redata_tem=redata;
						break;
*/
	/////////----测试指令--BEGIN----//////////////

			       case 0x50:
						isInstruction=1;
			  	   		if(redata==0x55)//返回速度幅度值
						{
			  			 senddata(0x11);//起始标志位
			  			 senddata(FISHID[0]);//鱼的id号， 
			  			 senddata(freqs[0]+1);//频率档，1-4
			  			 senddata(Static_Offset_degree[0]);//第一关节调直数据
			  			 senddata(Static_Offset_degree[1]);//第二关节调直数据
			  			 senddata(Static_Offset_degree[2]);//第三关节调直数据
			  			 senddata(s_tem);//速度档，对应速度、幅度
			  			 senddata(Speed);//速度值
			  			 senddata(Amplitude_degree[0]);//第一关节摆幅
			  			 senddata(Amplitude_degree[1]);//第二关节摆幅
			  			 senddata(Amplitude_degree[2]);//第三关节摆幅
			  			 senddata(d_tem);//方向档，对应转弯偏移角度
			  			 senddata(flag);//左转还是右转
			  			 senddata(Dynamic_Offset_degree_char[0]);//第一关节转弯偏移角度
			  			 senddata(Dynamic_Offset_degree_char[1]);//第二关节转弯偏移角度
			  			 senddata(Dynamic_Offset_degree_char[2]);//第三关节转弯偏移角度
			  			 senddata(Neighbouring_Offset_degree[0]);//0
			  			 senddata(Neighbouring_Offset_degree[1]);//第二关节滞后角
			  			 senddata(Neighbouring_Offset_degree[2]);//第三关节滞后角
						}//if(redata==0x55)	
						break;
				   case 0x60:
						isInstruction=1;
			            if(redata==0x66)//将调整的参数保存到I2C
						{
			  			 i2c_canshu=1;
						}//if(redata==0x66)	
						break;
				   
				   case 0xb0:
						isInstruction=1;
			            if(redata==0xb1)//改speed
			            {
			            Speed--;
			  			Speed_tem_z[s_tem]=Speed;
						}//if(redata==0xb1)
						
						if(redata==0xb2)
						{
			 			Speed++;
			 		    Speed_tem_z[s_tem]=Speed;
						}//if(redata==0xb2)
						
						if(redata==0xb3)//第一关节偏移角，减1
						{
			  			dd_tem=change_dd(d_tem);
			  			flag=change_flag(d_tem);
			  			 if(Dynamic_Offset_degree_char[0]==0)
			  			 {
						 }
			  			 else
			 			 {
			  			 Dynamic_Offset_degree_char[0]--;
			  			 }
			  			Dynamic_Offset_degree[0]=char2int(Dynamic_Offset_degree_char[0],flag);
			  			init_dynamic_offset();			  
			  			Dynamic_Offset_degree_origin[dd_tem][0]=Dynamic_Offset_degree_char[0];
					    }//if(redata==0xb3)
			
						if(redata==0xb4)//第一关节偏移角，加1
						{
			 			 dd_tem=change_dd(d_tem);
			 			 flag=change_flag(d_tem);
			  			 Dynamic_Offset_degree_char[0]++;
			  			 Dynamic_Offset_degree[0]=char2int(Dynamic_Offset_degree_char[0],flag);
			  			 init_dynamic_offset();			  
			  			 Dynamic_Offset_degree_origin[dd_tem][0]=Dynamic_Offset_degree_char[0];
						}//if(redata==0xb4)
			
						if(redata==0xb5)//第二关节偏移角，减1
						{
			  			 dd_tem=change_dd(d_tem);
			  			 flag=change_flag(d_tem);
			  			 if(Dynamic_Offset_degree_char[1]==0)
			  			 {
						 }
			  			 else
			  			 {
			  			 Dynamic_Offset_degree_char[1]--;
			  			 }
			  			Dynamic_Offset_degree[1]=char2int(Dynamic_Offset_degree_char[1],flag);
			  			init_dynamic_offset();			  
			  			Dynamic_Offset_degree_origin[dd_tem][1]=Dynamic_Offset_degree_char[1];
					   }//if(redata==0xb5)
			
					   if(redata==0xb6)//第二关节偏移角，加1
					   {
			  		    dd_tem=change_dd(d_tem);
			  			flag=change_flag(d_tem);
			  			Dynamic_Offset_degree_char[1]++;
			  			Dynamic_Offset_degree[1]=char2int(Dynamic_Offset_degree_char[1],flag);
			  			init_dynamic_offset();			  
			  			Dynamic_Offset_degree_origin[dd_tem][1]=Dynamic_Offset_degree_char[1];
					   }//if(redata==0xb6)
			
					   if(redata==0xb7)//第三关节偏移角，减1
					   {
			  		    dd_tem=change_dd(d_tem);
			  			flag=change_flag(d_tem);
			  			if(Dynamic_Offset_degree_char[1]==0)
			  			{
						}
			  			else
			  			{
			  			 Dynamic_Offset_degree_char[1]--;
			  			}
			  		   Dynamic_Offset_degree[2]=char2int(Dynamic_Offset_degree_char[2],flag);
			  		   init_dynamic_offset();			  
			  		   Dynamic_Offset_degree_origin[dd_tem][2]=Dynamic_Offset_degree_char[2];
					   }//if(redata==0xb7)
			
					   if(redata==0xb8)//第三关节偏移角，加1
					   {
			  		   dd_tem=change_dd(d_tem);
			  		   flag=change_flag(d_tem);
			  		   Dynamic_Offset_degree_char[2]++;
			  		   Dynamic_Offset_degree[2]=char2int(Dynamic_Offset_degree_char[2],flag);
			  		   init_dynamic_offset();			  
			  		   Dynamic_Offset_degree_origin[dd_tem][2]=Dynamic_Offset_degree_char[2];
					   }//if(redata==0xb8)								
					   senddata(redata); 
					   break;
			  

				   case 0xc0:
						isInstruction=1;
			           if(redata==0xc1)//改第一关节摆动幅度
					   {
			  		     if(Amplitude_degree[0]==0)
			  			 {
			  			 }
			  			 else
			  			 {
			  			 Amplitude_degree[0]--;
			  			 }
			  		   Amplitude_degree_tem[s_tem][0]=Amplitude_degree[0];//存为当前速度档的摆幅值
					   }//if(redata==0xc1)
					   
					   if(redata==0xc2)
					   {
			  		   	 if(Amplitude_degree[0]==90)
			 			 {
			  			 }
			  			 else
			  			 {
			  			 Amplitude_degree[0]++;
			  			 }
			  		  Amplitude_degree_tem[s_tem][0]=Amplitude_degree[0];
					  }//if(redata==0xc2)	
					  
					  if(redata==0xc3)//改第二关节摆动幅度
					  {
			 		   if(Amplitude_degree[1]==0)
			     	   {
			 		   }
			 		   else
			           {
			  		   Amplitude_degree[1]--;
			  		   }
			  		  Amplitude_degree_tem[s_tem][1]=Amplitude_degree[1];
					  }//if(redata==0xc3)
					  
					  if(redata==0xc4)
					  {
			  		   if(Amplitude_degree[1]==90)//90=0x5A
			  		   {
			  		   }
			  		   else
			  		   {
			  		   Amplitude_degree[1]++;
			  		   }
			          Amplitude_degree_tem[s_tem][1]=Amplitude_degree[1];
					  }//if(redata==0xc4)	
					  
					  if(redata==0xc5)//改第三关节摆动幅度
					  {
			 		   if(Amplitude_degree[2]==0)
			 		   {
			  		   }
			  		   else
			  		   {
			  		   Amplitude_degree[2]--;
			  		   }
			          Amplitude_degree_tem[s_tem][2]=Amplitude_degree[2];
					  }//if(redata==0xc5)
					  
					  if(redata==0xc6)
					  {
			           if(Amplitude_degree[2]==90)
			           {
			           }
			           else
			           { 
			 		   Amplitude_degree[2]++;
			  		   }
			         Amplitude_degree_tem[s_tem][2]=Amplitude_degree[2];
					 }//if(redata==0xc6)
					 
					 if(redata==0xc7)//第二关节减滞后角
					 {
			  		  if(Neighbouring_Offset_degree[1]==0)
			  		  {
			  		  }
			  		  else
			  		  {
			  		  Neighbouring_Offset_degree[1]--;
			  		  }					  
					 }//if(redata==0xc7)
					 
					 if(redata==0xc8)//第二关节加滞后角
					 {
			  		  if(Neighbouring_Offset_degree[1]==255)
			  		  {
			  		  }
			  		  else
			  		  {
			 		  Neighbouring_Offset_degree[1]++;
			  		  }
					 }//if(redata==0xc8)
					 
					 if(redata==0xc9)//第三关节减滞后角
					 {
			  		  if(Neighbouring_Offset_degree[2]==0)
			  		  {
			  		  }
			  		  else
			  		  {
			  		  Neighbouring_Offset_degree[2]--;
			  		  }
					 }//if(redata==0xc9)
					 
					 if(redata==0xc0)//第三关节加滞后角
					 {
			  		  if(Neighbouring_Offset_degree[2]==255)
			  		  {
			  		  }
			  		  else
			  		  {
			  		  Neighbouring_Offset_degree[2]++;
			  		  }
					 }//if(redata==0xc0)
					 senddata(redata);
					 break;

	/////////----测试指令--END----//////////////

			    case 0xf0://是否结束位
			            if(redata==0xfc)
						{
							e_change=1;//
							is_uart=1; //退出自动模式，进入手动控制模式
						}//if(redata==0xfc)
						break;		
			    default://接收到的不是指令也不是结束位，重新接收下一次命令
			  	      isSrlStart=0;
					  addr=0;
			     }//switch(redata&0xf0)
			  } //if(isInstruction==1) else
			  
		   }//if(addr)
		   
		   else//地址码没接收，或已经接收但不匹配
		   {
		     if(redata==(0x90|FISHID[0]))//接收地址码，匹配，置位标志位
			 {
			 addr=1;
			 }//if(redata==(0x90|FISHID))
			 
			 else//地址码不匹配，清零开始位标志位，重新接收下一次指令
			 {
			 isSrlStart=0;
			 }//if(redata==(0x90|FISHID)) else
			 
		   }//if(addr) else
		   
		 }//if(isSrlStart)

	/////////----初始化i2c的通信协议--BEGIN----//////////////

		 else if(isSrlStartI2c)//若已经发送初始化i2c的起始位，判断地址码（0x74，起确定指令的作用）是否匹配
		 {
			 if(addrI2c)//若地址码已经匹配，准备接收命令
			 {
				 if(isInstructionI2c)//已经接受过命令了，那么判断是否是结束位
				 {
					 if(redata==0xfb)
					 {
						 i2c_change=1;
						 is_uart=1;
					 }
					 else//接收到的不是结束位，那么这条指令无效，重新接收下一次命令
					 {
						 isSrlStartI2c=0;
						 addrI2c=0;
						 isInstructionI2c=0;
					 }
				 }
				 else//没有接收命令，则接收命令
				 {
					 switch(redata&0xf0)
					 {
					 case 0x80:
						 redata_i2c=redata;
						 i_change=1;
						 isInstructionI2c=1;
						 break;
					 case 0xfb:
						 if(redata==0xfb)
						 {
							 i2c_change=1;
							  is_uart=1;
						 }
						 break;
					 default:
						 isSrlStartI2c=1;
						 addrI2c=0;
						 break;
					 }
				 }					 
			 }


			 else//地址码没有接收，或者已接收但不匹配
			 {
				 if(redata==0x74)//接收地址码，匹配，置位标志位
				 {
					 addrI2c=1;
				 }
				 else//地址码不匹配，清零开始位标志位，重新接收下一次指令
				 {
					 isSrlStartI2c=0;
				 }					
			 }
		 }
		 else
		 {
		    if(redata==0xaa)//判断起始位，置位标志位
			{
			  isSrlStart=1;
			}
			if(redata==0xcc)//判断初始化i2c的起始位
			{
				isSrlStartI2c=1;
			}
		 }//if(isSrlStart) else

	/////////----初始化i2c的通信协议--END----//////////////

	}//if(1)
}


//***********************************计数器3溢出中断*****************************
#pragma interrupt_handler timer3_ovf_isr:30
void timer3_ovf_isr(void)
{
  unsigned char i;
  
 TCNT3H = 0x00 ; //reload counter high value  INVALID SETTING
 TCNT3L = 0x00 ; //reload counter low value   INVALID SETTING
 
 if(Speed!=0)   //即没有启动则一直1500，中间位置
 {
    Instant++;
  	Instant=Instant%Speed;
 }
 	t_dynamic++;
	if(t_dynamic>Speed)
	{
	  t_dynamic=Speed;
	}	
    calculate_data();
	OCR3A=Joint_Angle_value[2];
	/*wang genghai*/
	//OCR3B=Joint_Angle_value[2];	   
 
}

//***********************************计数器1溢出中断*****************************
#pragma interrupt_handler timer1_ovf_isr:15
void timer1_ovf_isr(void)
{
    unsigned char i;
  
    TCNT1H = 0x00 ; //reload counter high value  INVALID SETTING
    TCNT1L = 0x00 ; //reload counter low value   INVALID SETTING
 
    if(Speed!=0)   //即没有启动则一直1500，中间位置
    {
          Instant++;
		    //注意，在两个定时器的中断中，都增加了Instant++，实际是不必要的，只在一个定时器中断加就可以了。
	        //两处都加和只加一处的区别，只是电机运动快了，因为两处都加，相当于离散点的间隔是原来的一半（原来20ms，现在10ms）。理论分析时注意即可，不影响使用。
	        //详细解释  （一个摆动周期T定义为 左边——中间——右边——中间——左边）
	        //只在一处加Instant++：一个摆动周期被离散为speed份，每份间隔20ms，即，T=（speed-1）X20    此时最快速度时用speed=23
	        //两处都加Instant++：一个摆动周期被离散为speed份，每份间隔20/2ms，即，T=（speed-1）X10    此时最快速度时用speed=45(与上面23一个效果) 
	        //以上计算公式有误差，修正如下——09.06.25
			//只在一处加Instant++：一个摆动周期被离散为speed份，每份间隔20ms，即，T=（speed）X20    此时最快速度时用speed=23
			//两处都加Instant++：一个摆动周期被离散为speed份，每份间隔20/2ms，即，T=（speed）X10    此时最快速度时用speed=45(与上面23一个效果)
  	    Instant=Instant%Speed;
   }
    calculate_data(); 
	OCR1A=Joint_Angle_value[0];	 
	OCR1B=Joint_Angle_value[1];  
}
//*************************************************************************
//********************************主函数***********************************
//*************************************************************************
void main(void)
{   
     unsigned char i;
  	 unsigned char tem;
	 unsigned char tem_y;
	 unsigned char j; 
 	 init_devices();//初始化
	 TWI_Master_Initialise(); //*IIC控制初始化
	 SEI();
     /////////////////////////////////////////////////////
	 //读i2c数据
	 i2c_read(ADD_i,FISHID_I2C,FISHID,1);
	 i2c_read(ADD_f,freqs_I2C,freqs,1);
	 //PORTD=freq[freqs[0]];
	 i2c_read(ADD_t,degree_I2C,Static_Offset_degree,3);
	 i2c_read(ADD_s1,Speed_tem_I2C1,Speed_tem_z1,8);
	 i2c_read(ADD_s2,Speed_tem_I2C2,Speed_tem_z2,8);
	 two2one(Speed_tem_z,Speed_tem_z1,Speed_tem_z2);
	 for(i=0;i<=15;i++)
	 {
	      Speed_tem[i]=Speed_tem_z[i];    //Speed_tem[i] = Speed_Amplitudeem_z[i];//Speed_Amplitudeem_z[i];//*为什么会有错？？
	 }
	 i2c_read(ADD_d,Neighbouring_Offset_degree_I2C,Neighbouring_Offset_degree,3);
	 i2c_read(ADD_1,Amplitude_degree_tem_I2C1,Amplitude_degree_tem_z1,8);
	 i2c_read(ADD_2,Amplitude_degree_tem_I2C2,Amplitude_degree_tem_z2,8);
	 i2c_read(ADD_3,Amplitude_degree_tem_I2C3,Amplitude_degree_tem_z3,8);
	 i2c_read(ADD_4,Amplitude_degree_tem_I2C4,Amplitude_degree_tem_z4,8);
	 i2c_read(ADD_5,Amplitude_degree_tem_I2C5,Amplitude_degree_tem_z5,8);
	 i2c_read(ADD_6,Amplitude_degree_tem_I2C6,Amplitude_degree_tem_z6,8);
	 six2one(Amplitude_degree_tem_z,Amplitude_degree_tem_z1,Amplitude_degree_tem_z2,Amplitude_degree_tem_z3,Amplitude_degree_tem_z4,Amplitude_degree_tem_z5,Amplitude_degree_tem_z6);  
	 for(i=0;i<=47;i++)
	 {
		Amplitude_degree_tem[i/3][i%3]=Amplitude_degree_tem_z[i];
	 }	
	 i2c_read(ADD_10,Dynamic_Offset_degree_origin_I2C1,Dynamic_Offset_degree_origin_z1,8);
	 i2c_read(ADD_20,Dynamic_Offset_degree_origin_I2C2,Dynamic_Offset_degree_origin_z2,8);
	 i2c_read(ADD_30,Dynamic_Offset_degree_origin_I2C3,Dynamic_Offset_degree_origin_z3,8);
	 three2one(Dynamic_Offset_degree_origin_z,Dynamic_Offset_degree_origin_z1,Dynamic_Offset_degree_origin_z2,Dynamic_Offset_degree_origin_z3);  
	 for(i=0;i<=23;i++)
	 {
		Dynamic_Offset_degree_origin[i/3][i%3]=Dynamic_Offset_degree_origin_z[i];
     }
////////////////////////////////////////////////////
	 CLI();
	 timer3_init();
	 timer1_init();
	 SEI();
	 init_data();
	 init_static_offset();
	 calculate_data();
	 
	 CLI();
	 TCCR3B=0x00;		//暂停定时／计数器3  最后两位设为0即可
	 TCCR1B=0x00;		//暂停定时／计数器3  最后两位设为0即可
	
	 ICR3=PWM_Period_value;//计数器3上限值TOP
	 ICR1=PWM_Period_value;//计数器1上限值TOP
	 OCR1A=Joint_Angle_value[0];
	 OCR1B=Joint_Angle_value[1];
	 OCR3A=Joint_Angle_value[2];
	 /*wang genggai*/
	// OCR3B=Joint_Angle_value[2];
	 
	 SEI();         //re-enable interrupts
	 TCCR3B=0x1A;	//启动定时／计数器3
	 TCCR1B=0x1A;	//启动定时／计数器1
	 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	while(1)
	{
///////////----手动模式--BEGIN//////////////////////////////////////////
	   if(is_uart==1)//手动模式
	   {
	   if(i2c_canshu)
	   {
	     i2c_canshu=0;
		 baocun_i++;
		 switch(baocun_i)
		 {
		 case 1:
				one2two(Speed_tem_z,Speed_tem_z1,Speed_tem_z2);
		        i2c_write(ADD_s1,Speed_tem_I2C1,Speed_tem_z1,8);
				delay_nms(200);
		        i2c_write(ADD_s2,Speed_tem_I2C2,Speed_tem_z2,8);
		        senddata(baocun_i);
				break;
		 case 2:
		        i2c_write(ADD_d,Neighbouring_Offset_degree_I2C,Neighbouring_Offset_degree,3);//滞后角
		        senddata(baocun_i);
				break;
		 case 3://幅度
				for(i=0;i<=15;i++)
				{
				  for(j=0;j<=2;j++)
				  {
				    Amplitude_degree_tem_z[i*3+j]=Amplitude_degree_tem[i][j];
				  }
				}
				one2six(Amplitude_degree_tem_z,Amplitude_degree_tem_z1,Amplitude_degree_tem_z2,Amplitude_degree_tem_z3,Amplitude_degree_tem_z4,Amplitude_degree_tem_z5,Amplitude_degree_tem_z6);			
				i2c_write(ADD_1,Amplitude_degree_tem_I2C1,Amplitude_degree_tem_z1,8);
		        senddata(baocun_i);
				break;
		 case 4:
				i2c_write(ADD_2,Amplitude_degree_tem_I2C2,Amplitude_degree_tem_z2,8);
				delay_nms(200);
				i2c_write(ADD_3,Amplitude_degree_tem_I2C3,Amplitude_degree_tem_z3,8);
				senddata(baocun_i);
				break;
		 case 5:
				i2c_write(ADD_4,Amplitude_degree_tem_I2C4,Amplitude_degree_tem_z4,8);
				delay_nms(200);
				i2c_write(ADD_5,Amplitude_degree_tem_I2C5,Amplitude_degree_tem_z5,8);
		        senddata(baocun_i);
				break;
		 case 6:
				i2c_write(ADD_6,Amplitude_degree_tem_I2C6,Amplitude_degree_tem_z6,8);
				senddata(baocun_i);
    			break;
		 case 7://转弯偏移角
				for(i=0;i<=7;i++)
				{
				  for(j=0;j<=2;j++) 
				  {
				    Dynamic_Offset_degree_origin_z[i*3+j]=Dynamic_Offset_degree_origin[i][j];
				  }
				}
				one2three(Dynamic_Offset_degree_origin_z,Dynamic_Offset_degree_origin_z1,Dynamic_Offset_degree_origin_z2,Dynamic_Offset_degree_origin_z3);			
				i2c_write(ADD_10,Dynamic_Offset_degree_origin_I2C1,Dynamic_Offset_degree_origin_z1,8);
		        senddata(baocun_i);
				break;
		 case 8:
				i2c_write(ADD_20,Dynamic_Offset_degree_origin_I2C2,Dynamic_Offset_degree_origin_z2,8);
				senddata(baocun_i);
				break;
		 case 9:baocun_i=0;
				i2c_write(ADD_30,Dynamic_Offset_degree_origin_I2C3,Dynamic_Offset_degree_origin_z3,8);
		        senddata(baocun_i);
				break;  
		 }//switch baocun_i		  
	   }//if(i2c_canshu)
	   	   
	   if(i2c_change == 1)
	   {
			i2c_init();
			i2c_change=0;
			isSrlStartI2c=0;
			addrI2c=0;
			isInstructionI2c=0;
			i_change=0;
	   }//if(i2c_change)

	   if(e_change)//接收到结束位,处理指令
	   {
	   //senddata(FISHID[0]);
	   redata=redata_tem;
	     if(s_change)//速度变化
		 {
			 id_change=0;
			id_fan=(FISHID[0]<<5)|(redata_fan&0x0f);
		    if((redata_fan>>4)<<4==0xd0)
		      id_fan=id_fan|0x10;
		    ///////senddata(id_fan);	//前三位用来表示id号，第４位为１时为速度，０为方向，后四位为具体的速度或是方向的值
	   
			//senddata(t_dynamic);////////////////////
			s_tem=redata&0x0f;//速度档0-15
			if(s_tem==0)//速度0时，停止，直
			{
			  for(i=0;i<=2;i++)
				Dynamic_Offset_degree [i]=0;
			  init_dynamic_offset();
	          for(i=0;i<Joint_Num;i++)
	            {
	            Dynamic_Offset_value_last[i]=0;
	            }
	 		}
		    Speed=Speed_tem_z[s_tem];
			for(i=0;i<=2;i++)//对应摆幅变化
			   Amplitude_degree[i]=Amplitude_degree_tem[s_tem][i];
//		    init_dynamic_offset();
		 }//if(s_change)
		 
		 if(d_change)//方向变化,有过渡状态08.01.14
		 {
			 id_change=0;
			id_fan=(FISHID[0]<<5)|(redata_fan&0x0f);
		    if((redata_fan>>4)<<4==0xd0)
		      id_fan=id_fan|0x10;
		    /////senddata(id_fan);	//前三位用来表示id号，第４位为１时为速度，０为方向，后四位为具体的速度或是方向的值
	   
			// senddata(t_dynamic);////////////////////
			 t_dynamic=0;//过渡状态开始
			 d_tem=redata&0x0f;//转弯档0-15
		 	 dd_tem=change_dd(d_tem);
			 flag=change_flag(d_tem);
			// senddata(d_tem);
			// senddata(dd_tem);
			// senddata(flag);
			 for(i=0;i<=2;i++)
			 {
				Dynamic_Offset_degree_char[i]=Dynamic_Offset_degree_origin[dd_tem][i];
			    Dynamic_Offset_degree[i]=char2int(Dynamic_Offset_degree_char[i],flag);
		     }
			 init_dynamic_offset();
			 Speed_dynamic=Speed;
			 d_change_t=1;
			 d_change_t_v=1; 
			// senddata(t_dynamic);////////////////////
		 }//if(d_change)
		 
/*		 if(a_change)//幅度变化
		 {
			 tem=redata&0x0f;//幅度档0-15
			 for(i=0;i<=2;i++)
			    Amplitude_degree[i]=Amplitude_degree_tem[tem][i];
		 }//if(a_change)
*/		 
		 if(t_change)//调直
		 {
			 id_change=0;
		     senddata(FISHID[0]);
			 tem=redata&0x07;//关节数,0,1,2
			 if(tem>2)
			 {
			 }//if(tem>2)
			 else
			 {
			    if(redata&0x08)//右调直,1度
				{
				   Static_Offset_degree[tem]++;
				}//if(redata&0x08)
				else//左调直,1度
				{
				   Static_Offset_degree[tem]--;
				}//if(redata&0x08) else
			 }//if(tem>2) else
			 init_static_offset();
		 }//if(t_change)
		 
		 if(ts_change)//调直是否保存
		 {
			 id_change=0;
		   senddata(FISHID[0]);
		   if(redata&0x04)//保存
		   {
		       i2c_write(ADD_t,degree_I2C,Static_Offset_degree,3);
		   }//if(redata&0x04)
		   else//不保存
		   {
		       i2c_read(ADD_t,degree_I2C,Static_Offset_degree,3);		   
		   }//if(redatat&0x04) else
		 }//if(ts_change)
		 
		 if(id_change==2)//更改鱼id
		 {
			 id_change=0;
            senddata(FISHID[0]);
			tem=redata&0x0f;
		    FISHID[0]=tem;
            i2c_write(ADD_i,FISHID_I2C,FISHID,1);
			senddata(FISHID[0]);
		 }//if(id_change)
		 
		 if(f_change)//更改频率
		 {
			 id_change=0;

		    if(redata_tem==0x41)//频率1
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x02);
			  senddata(0xbb);
			}
			else if(redata_tem==0x42)//频率2
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x16);
			  senddata(0xbb);
			}
			else if(redata_tem==0x43)//频率3
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x2A);
			  senddata(0xbb);
			}
			else if(redata_tem==0x44)//频率4
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x3B);
			  senddata(0xbb);
			}
			else if(redata_tem==0x45)//频率5
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x33);
			  senddata(0xbb);
			}
			else if(redata_tem==0x46)//频率6
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x0c);
			  senddata(0xbb);
			}
			
			//senddata(FISHID[0]);
			//senddata(PORTD&0xf7);//0xf4
						//tem=redata&0x07-4;//0x44-0x47,频率1-4,tem(0-3)
		    //freqs[0]=tem;
		   // PORTD=freq[freqs[0]];
           // i2c_write(ADD_f,freqs_I2C,freqs,1);
		 }//if(f_change)		
	
	   //清零所有标志位,id_change在以上if(id_change)之外的if(*_change)中清零
	     isSrlStart=0;
	     addr=0;
		 isInstruction=0;
	     s_change=0;
		 a_change=0;
	     d_change=0;
	     t_change=0;
	     ts_change=0;
	     //id_change=0;
	     f_change=0;
	     e_change=0;
		}//if(e_change)
	   
	   if(m_change)//更改模态，m_tem模态码
	   {
	     m_cishu--;//若++第一次反应慢
	     if(m_cishu==0xff)
	     {
		   mm_cishu--;//若++第一次反应慢
		   if(mm_cishu==0xff)
		   {
			 mmm_cishu++;
			 if(mmm_cishu==m_time)
			 {
		     mmm_cishu=0;
			 //执行代码
			 cishu++;
			 if(cishu==3)
			 {
			  cishu=0;
			 }//if(cishu==3)
			 switch(m_tem&0x0f)
			 {
			 case 0x01://模态1
					   switch(cishu)
					   {
					   case 0x00://0,低速1
					             Speed=Speed_tem_z[1];
		                         for(i=0;i<=2;i++)		  
		                            Dynamic_Offset_degree [i]=0;
		                         init_dynamic_offset();
								 
								 break;//0
					   case 0x01://1，高速15
								 Speed=Speed_tem_z[15];
								 for(i=0;i<=2;i++)
								    Dynamic_Offset_degree [i]=0;
								 init_dynamic_offset();
								 break;//1  
					   case 0x02://2，转弯1
		 	                     for(i=0;i<=2;i++)  
				                   Dynamic_Offset_degree [i]=Dynamic_Offset_degree_origin [1][i];
		                         init_dynamic_offset(); 
					             break;//2
					   }//switch(cishu)
			           break;//case模态1
		     case 0x02://模态2
					   switch(cishu)
					   {
					   case 0x00://0，左转1
		 	                     for(i=0;i<=2;i++)
				                   Dynamic_Offset_degree [i]=Dynamic_Offset_degree_origin [1][i];
		                         init_dynamic_offset(); 
     							 break;//0
					   case 0x01://1，右转12
		 	                     for(i=0;i<=2;i++)
				                   Dynamic_Offset_degree [i]=Dynamic_Offset_degree_origin [12][i];
		                         init_dynamic_offset(); 
								 break;//1
					   case 0x02://2，前进12
								 Speed=Speed_tem_z[12];
								 for(i=0;i<=2;i++)
								    Dynamic_Offset_degree [i]=0;
								 init_dynamic_offset();					             
								 break;//2
					   }//switch(cishu)			           
			           break;//case模态2
			 }//switch(m_tem)
			 //代码结束
			 }//if(mmm_cishu)
		   }//if(mm_cishu)		   
		 }//if(m_cishu)...延时作用
       }//if(m_change)
	   
		if(y_change)//原始转弯模式
		{
			y_cishu--;
			if(y_cishu==0xff)
			{
				yy_cishu--;

				if(yy_cishu==0xff)
				{
					yyy_cishu++;
					yy_cishu=yy_time;

					if(yyy_cishu==y_time)
					{
						yyy_cishu=0;

						//执行代码
						if(cishu_y==1)//回到收到命令前的速度，直游
						{
							y_change=0;
							y_changing=0;
							Speed=y_speed;
							for(i=0;i<=2;i++)		  
								Dynamic_Offset_degree [i]=0;
							for(i=0;i<=2;i++)//对应摆幅变化
								Amplitude_degree[i]=Amplitude_degree_tem[s_tem][i];
							init_dynamic_offset();
						}//if(cishu_y==1)

						if(cishu_y==0)
						{
							cishu_y++;
							tem_y=y_tem&0x0f;
							Speed=0;
							for(i=0;i<=2;i++)
								Dynamic_Offset_degree [i]=Dynamic_Offset_degree_y [tem_y][i];
							init_dynamic_offset(); 			 
						}//if(cishu_y==0)
						//代码结束

					}//if(yyy_cishu)
				}//if(yy_cishu)		   
			}//if(y_cishu)...延时作用
		}//if(y_change)	
	}//if(is_uart==1)

///////////----手动模式--END//////////////////////////////////////////	

///////////----自动模式--BEGIN//////////////////////////////////////////	

		else//自动模式,is_uart==0
		{
             a_cishu--;//若++第一次反应慢
	    	    if(a_cishu==0xff)
	            {
		          aa_cishu--;//若++第一次反应慢
		           if(aa_cishu==0xff)
		           {
			           aaa_cishu++;
			           if(aaa_cishu==m_time)
			           {
		                aaa_cishu=0; 
			            //执行代码
			            auto_cishu++;
			            if(auto_cishu==7)
			            {
			                 auto_cishu=1;
			             }//if(auto_cishu==6)

					   switch(auto_cishu)
					   {
					   case 0x01://直游，15
						   Speed=Speed_tem_z[15];
						   for(i=0;i<=2;i++)//对应摆幅变化
							   Amplitude_degree[i]=Amplitude_degree_tem[15][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   case 0x02://右转
						   Speed=Speed_tem_z[15];
						   for(i=0;i<=2;i++)
							{
								Dynamic_Offset_degree_char[i]=Dynamic_Offset_degree_origin[3][i];
								Dynamic_Offset_degree[i]=char2int(Dynamic_Offset_degree_char[i],0);
							}
							init_dynamic_offset();
						   break;
					   case 0x03://直游，11
						   Speed=Speed_tem_z[11];
						   for(i=0;i<=2;i++)//对应摆幅变化
							   Amplitude_degree[i]=Amplitude_degree_tem[11][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   case 0x04://直游，11
						   Speed=Speed_tem_z[11];
						   for(i=0;i<=2;i++)//对应摆幅变化
							   Amplitude_degree[i]=Amplitude_degree_tem[11][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   case 0x05://右转
						   Speed=Speed_tem_z[11];
                           for(i=0;i<=2;i++)
							{
								Dynamic_Offset_degree_char[i]=Dynamic_Offset_degree_origin[3][i];
								Dynamic_Offset_degree[i]=char2int(Dynamic_Offset_degree_char[i],0);
							}
							init_dynamic_offset();
							break;
						case 0x06://直游，15
						   Speed=Speed_tem_z[15];
						   for(i=0;i<=2;i++)//对应摆幅变化
							   Amplitude_degree[i]=Amplitude_degree_tem[15][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   }//switch(auto_cishu)
			           //代码结束
			         }//if(aaa_cishu)
		         }//if(aa_cishu)		   
		       }//if(a_cishu)...延时作用
		    }//自动模式,is_uart==0

///////////----自动模式--END//////////////////////////////////////////	

	}//while(1)
}//main

