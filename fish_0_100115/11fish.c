//ICC-AVR application builder : 2006-11-23 22:56:28
// Target : M128
// Crystal: 8.0000Mhz
/*  
   comunicating protocol with the radio controller:
   ����8λ��������
   1��ʾ����
   0��ʾ�ɿ�
   eg:   1111   X  X  X  X
                ǰ �� �� ��
*/  

/*
*
*  �ֶ�ģʽ���Զ�ģʽ���л���unsigned char is_uart=0;//���������ε��������е��Զ�ģʽ���ô˱�־λΪ1
*
*
*
*/
#include <iom128v.h>
#include <macros.h>
#include "TWI_Master.h"

#define FLK 8 	   	  	 //���� crystal
#define PI 3.1416        //**Բ����
#define Joint_Num  3

#define SLA_W 0xA0
#define SLA_R 0xA1

////////////////////////////----i2c��ַ--BEGIN----///////////////////////
//��Щi2cһ�ο���д16��������Щһ��ֻ��д8����Ϊ��ͨ���ԣ��˴�ÿ��д��8��
//����������
#define ADD_i 0x10//ID*1
#define ADD_f 0x20//Ƶ��*1
#define ADD_t 0x30//��ֱ*3
#define ADD_s1 0x40//�ٶ�*16
#define ADD_s2 0x48
#define ADD_d 0x50//�ͺ��*3
#define ADD_1 0x60//����*48
#define ADD_2 0x68
#define ADD_3 0x70
#define ADD_4 0x78
#define ADD_5 0x80
#define ADD_6 0x88
#define ADD_10 0x90//ת��ƫ��*24
#define ADD_20 0x98
#define ADD_30 0xa0
////////////////////////////----i2c��ַ--END----///////////////////////
#define PWM_Period_ms 20	     //ms
#define PWM_Duty_Middle_us 1500  //us
#define PWM_Duty_LeftLimit_us 900  //us
#define PWM_Duty_RightLimit_us 2100  //us

#define D_MAX 60  //Dynamic_Offset_degree_origin[0]�����ֵ
#define D_Y_MAX 60  //Dynamic_Offset_degree_y[0]�����ֵ

#define A_MAX 20 //*0.85=Amplitude_degree_tem[0]�����ֵ

#define m_time 0x10 //ģ̬�仯�п���ʱ��
#define y_time 0x2 //ԭʼת��ģʽ�п���ʱ��//ÿ���ż�2


int LeftLimit_value, RightLimit_value, MiddlePosition_value,PWM_Period_value;
unsigned char Speed=0;   
unsigned char Instant=0;
unsigned char redata;//��������
unsigned char redata_fan=0;//���ٶȻ���ָ��ʱ���洢��ָ��
unsigned char id_fan=0;//��FISHID[0]��redata_fan��ɵ�1���ֽ���������ֵ
unsigned char redata_tem;//���������ݴ�
unsigned char redata_i2c;//i2c���������ݴ�
unsigned char baocun_i=0;
unsigned char t_dynamic=0;//ת������󣬴�ԭ״̬����״̬�Ĺ���ʱ�䣬�յ�������0��Speed_dynamic/2ʱ���Ϊ����״̬
unsigned char Speed_dynamic=0;//�յ�ת������ʱ�̵�Speedֵ

unsigned char n=0;
unsigned char m=0;//����ʱѭ����������
unsigned char s=0;//��������Ӻ�����ʹ��

//////////////----��Ҫд��i2c������--BEGIN----//////////////////
unsigned char FISHID[1]={0x01};//��ַADD_i
unsigned char FISHID_init[1]={0x01};
unsigned char FISHID_I2C[3]={0,0,0x01};

unsigned char freqs[1]={0};//��ַADD_f,freq[freqs[x]]
unsigned char freqs_init[1]={0};
unsigned char freqs_I2C[3]={0,0,0};

unsigned char Static_Offset_degree[3]={0,0,0};//��ַADD_t
unsigned char Static_Offset_degree_init[3]={0,0,0};
unsigned char degree_I2C[5]={0,0,0,0,0};

//Speed_tem[16]//��ַADD_s1,s2
unsigned char Speed_tem_init[16]={0,255,240,225,210,195,180,165,150,135,120,105,90,75,60,45};
unsigned char Speed_tem_I2C[18]={0};  

unsigned char Speed_tem_init1[8]={0};
unsigned char Speed_tem_init2[8]={0};

unsigned char Speed_tem_z[16]={0};//Dynamic_Offset_degree_origin[8][3]ֱ��ת�ɵ�1ά����
unsigned char Speed_tem_z1[8]={0};//0-7
unsigned char Speed_tem_z2[8]={0};//8-16

unsigned char Speed_tem_I2C1[10]={0};
unsigned char Speed_tem_I2C2[10]={0};

//Neighbouring_Offset_degree[3]//��ַADD_d
unsigned char Neighbouring_Offset_degree_init[3]={0,80,120};
unsigned char Neighbouring_Offset_degree_I2C[5]={0};


//Amplitude_degree_tem[16][3]//��ַADD_1,2,3,4,5,6
unsigned char Amplitude_degree_tem_init[48]=
              {
			    0.1*A_MAX,0.1*A_MAX+10,0.1*A_MAX+15,//0 ���ٶ�С�ڷ�С
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
                0.85*A_MAX,0.85*A_MAX+10,0.85*A_MAX+15//15���ٶȴ�ڷ���
			  };
			  //unsigned char Amplitude_degree_tem_I2C[50]={0};
unsigned char Amplitude_degree_tem_init1[8]={0};
unsigned char Amplitude_degree_tem_init2[8]={0};
unsigned char Amplitude_degree_tem_init3[8]={0};
unsigned char Amplitude_degree_tem_init4[8]={0};
unsigned char Amplitude_degree_tem_init5[8]={0};
unsigned char Amplitude_degree_tem_init6[8]={0};
unsigned char Amplitude_degree_tem_z[48]={0};//Amplitude_degree_tem[16][3]ֱ��ת�ɵ�1ά����
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

//Dynamic_Offset_degree_origin [8][3],��ַADD_10,20,30,40
unsigned char Dynamic_Offset_degree_origin_init[24]=
{
  D_MAX,D_MAX-5,0,  //��14����15����0����//�Ӹ�������
  0.8*D_MAX,0.8*D_MAX-5,0, //��13����1��q
  0.6*D_MAX,0.6*D_MAX-5,0,  //��12����2,w
  0.45*D_MAX,0.45*D_MAX-5,0, //��11����3,e
  0.3*D_MAX,0.3*D_MAX-5,0,  //��10����4.r
  0.15*D_MAX,0.15*D_MAX-5,0, //��9����5.t
  0.1*D_MAX,0.1*D_MAX-5,0,  //��8����6����.y
    0,  0,  0 //��7����ת
};
unsigned char Dynamic_Offset_degree_origin_init1[8]={0};
unsigned char Dynamic_Offset_degree_origin_init2[8]={0};
unsigned char Dynamic_Offset_degree_origin_init3[8]={0};
unsigned char Dynamic_Offset_degree_origin_z[24]={0};//Dynamic_Offset_degree_origin[8][3]ֱ��ת�ɵ�1ά����
unsigned char Dynamic_Offset_degree_origin_z1[8]={0};//0-11
unsigned char Dynamic_Offset_degree_origin_z2[8]={0};//12-23
unsigned char Dynamic_Offset_degree_origin_z3[8]={0};//12-23
unsigned char Dynamic_Offset_degree_origin_I2C1[10]={0};//
unsigned char Dynamic_Offset_degree_origin_I2C2[10]={0};//
unsigned char Dynamic_Offset_degree_origin_I2C3[10]={0};//
//////////////----��Ҫд��i2c������--END----//////////////////


/////////////----�����־λ--BEGIN----//////////////////
unsigned char isSrlStart=0;//�յ���ʼλʱ��1����Ϊ1ʱ����ʼ������һλ��ַ�룬���ж��Ƿ�ƥ��
unsigned char isSrlStartI2c=0;//�յ���ʼ��I2c����ʼλʱ��1����ʼ������һλ��ַ�룬���ж��Ƿ�ƥ��
unsigned char addr=0;//��ַλƥ��ʱ��1����ʼ������һλ����λ
unsigned char addrI2c=0;//i2c��ַλƥ��ʱ��1����ʼ������һλi2c����λ
unsigned char s_change=0;//�ٶȸı�ʱ��1 
unsigned char d_change=0;//����ı�ʱ��1
unsigned char d_change_t=0;//����ı�,����״̬��Ϊ1���ȶ�������
unsigned char d_change_t_v=0;//����ı�ʱ��1����ʵʱƫ�ƽǸ���last��Ϊǰһ״̬ƫ�ƽǣ�Ȼ������ñ�־λ
unsigned char t_change=0;//��ֱʱ��1
unsigned char ts_change=0;//�����ȡ����ֱ״̬ʱ��1
unsigned char a_change=0;//���ȸı�ʱ��1
unsigned char f_change=0;//Ƶ�ʸı�ʱ��1
unsigned char id_change=0;//id�ı�ʱ��1
unsigned char e_change=0;//���յ�����λʱ��1
unsigned char i_change=0;//���յ�i2c��ʼ������ʱ��Ϊ1
unsigned char i2c_change=0;//���յ�i2c��ʼ������Ľ���λʱ��Ϊ1
unsigned char isInstruction=0;//�Ƿ���յ�ָ��ѽ�����Ϊ1��δ���գ���Ϊ0�����ø���־λ����ָ֤���������λ���ܹ�ִ�У�
//����aa 9_ d5 d0 fc������ָ������Ч�ģ������޴˱�־λ�����ָ�������Ч�ģ�����ִ��d0���� 
unsigned char isInstructionI2c=0;//�Ƿ���յ�i2cָ��ѽ�����Ϊ1��δ���գ���Ϊ0�����ø���־λ��
//��ָ֤���������λ���ܹ�ִ�У�����cc 74 8_ aa fc������ָ������Ч�ģ������޴˱�־λ�����ָ�������Ч�ģ�����ִ��8_���� 
//���ϱ�־λ�����յ�����λ��ִ���������0

unsigned char is_uart=1;//�����Ƿ���յ������տ���ʱ���˱�־λ��Ϊ0���Զ������趨��ģʽ�������ڽ��ܵ���������ʱ�򣬴˱�־λ��Ϊ1�������ֶ�����ģʽ
unsigned char m_change=0;//���յ�ģ̬��ʱ��1�����յ���������ʱ��0
unsigned char m_cishu=0;//mm_cishu,mmm_cishu��ͬ����ģ̬����1��ѭ���Ĵ���
unsigned char mm_cishu=0;//
unsigned char mmm_cishu=m_time-1;//
unsigned char a_cishu=0;//aa_cishu,aaa_cishu��ͬ�����Զ�״̬ѭ���Ĵ���//*auto
unsigned char aa_cishu=0;//
unsigned char aaa_cishu=m_time-1;//
unsigned char cishu=0;//ģ̬�ж���ѭ������3Ϊ���ڣ���0,1,2,0,1,2....
unsigned char auto_cishu=0;//ģ̬�ж���ѭ������5Ϊ���ڣ���0,1,2,3,4,0,1,2,3,4....
unsigned char y_change=0;//���յ�ԭʼת�䷽ʽʱ��Ϊ1�����յ���������ʱ��0
unsigned char y_changing=0;//����ִ��ԭʼת�䷽ʽʱ��Ϊ1����ʱ��������������
unsigned char yy_time=0xff;//��y_time��ͬ����ԭʼת��ģʽ��ʱ��
unsigned char y_cishu=0;   //
unsigned char yy_cishu=0;
unsigned char yyy_cishu=y_time-1;
unsigned char cishu_y=0;//0ʱת�䣬1ʱ�ص��յ�����ǰ���ٶȣ�ֱ��
unsigned char y_tem=0;//ת�䵵
unsigned char y_speed=0;//����ת��ǰ�ٶ�
/////////////----�����־λ--END----//////////////////


/////////////----��������--BEGIN----//////////////////
unsigned char s_tem=0;//�ٶȵ���Speed_tem[s_tem],Amplitude_degree_tem[s_tem][i]
unsigned char m_tem=0;//m_tem&0x0f=ģ̬��
unsigned char d_tem=0;//���򵵣�Dynamic_Offset_degree_origin [d_tem][i]
unsigned char dd_tem=0;//��������ͬһ��
unsigned char flag=0;
// d_tem 0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15 //7ǰ�󣬺���
//dd_tem 0  1  2  3  4  5  6  7  6  5  4   3   2   1   0   0 //�Ӹ�����
////flag=1ʱ����תd_temС��7���Ӹ��ţ�flag=0����תd_tem���ڵ���7
//unsigned char y_tem=0;//ԭʼת�䵵
unsigned char i2c_canshu=0;//��������޸�ʱ��1

//4��Ƶ�ʣ���ͨ��ģ��ĸ�Ƶ�ʣ���ͨ��ģ�鲻�ô˱���
unsigned char freq[4]={0xff,0xf7,0xef,0xdf};

//16���ٶ�ֵ,0-15,SpeedֵԽ���ٶ�ԽС
unsigned char Speed_tem[16]=  
{
  0,255,240,225,210,195,180,165,150,135,120,105,90,75,60,45//��ʼֵ��8-150
};//���255//����150�ȽϺ���

//16��ת��ֵ��0-15,7��ת
unsigned char Dynamic_Offset_degree_origin[8][Joint_Num]=
{
  {D_MAX,D_MAX-5,0},  //��14����15����0����//�Ӹ�������
  {0.9*D_MAX,0.9*D_MAX-5,0}, //��13����1��q
  {0.8*D_MAX,0.8*D_MAX-5,0},  //��12����2,w
  {0.7*D_MAX,0.7*D_MAX-5,0}, //��11����3,e
  {0.5*D_MAX,0.5*D_MAX-5,0},  //��10����4.r
  {0.3*D_MAX,0.3*D_MAX-5,0}, //��9����5.t
  {0.1*D_MAX,0.1*D_MAX-5,0},  //��8����6����.y
    {0,  0,  0} //��7����ת
};//�������飬����Dynamic_Offset_degree��ֵ

//ԭʼת�䷽ʽ��16��ת��ֵ��0-15,7��ת
int Dynamic_Offset_degree_y[16][Joint_Num] =
{
  {-D_Y_MAX,-D_Y_MAX-5,-D_Y_MAX-10},  //��0����
  {-0.9*D_Y_MAX,-0.9*D_Y_MAX-5,-0.9*D_Y_MAX-10}, //��1
  {-0.8*D_Y_MAX,-0.8*D_Y_MAX-5,-0.8*D_Y_MAX-10},  //��2
  {-0.7*D_Y_MAX,-0.7*D_Y_MAX-5,-0.7*D_Y_MAX-10}, //��3
  {-0.5*D_Y_MAX,-0.5*D_Y_MAX-5,-0.5*D_Y_MAX-10},  //��4
  {-0.3*D_Y_MAX,-0.3*D_Y_MAX-5,-0.3*D_Y_MAX-10}, //��5
  {-0.1*D_Y_MAX,-0.1*D_Y_MAX-5,-0.1*D_Y_MAX-10},  //��6����
  {  0,  0,  0}, //��7����ת
  { 0.1*D_Y_MAX, 0.1*D_Y_MAX-5,0.1*D_Y_MAX-10},  //��8����
  { 0.2*D_Y_MAX, 0.2*D_Y_MAX-5,0.2*D_Y_MAX-10}, //��9
  { 0.3*D_Y_MAX, 0.3*D_Y_MAX-5,0.3*D_Y_MAX-10},  //��10
  { 0.5*D_Y_MAX, 0.5*D_Y_MAX-5,0.5*D_Y_MAX-10}, //��11
  { 0.7*D_Y_MAX, 0.7*D_Y_MAX-5,0.7*D_Y_MAX-10},  //��12
  { 0.8*D_Y_MAX, 0.8*D_Y_MAX-5,0.8*D_Y_MAX-10}, //��13
  { 0.9*D_Y_MAX, 0.9*D_Y_MAX-5,0.9*D_Y_MAX-10},  //��14
  { D_Y_MAX,  D_Y_MAX-5, D_Y_MAX-10} //��15����
};//�������飬����Dynamic_Offset_degree��ֵ

int Joint_Angle_value [Joint_Num] =     //the final value to be written in duty cycle register
{
  0, 0, 0
};//��ʼ��ֵ

int Static_Offset_value [Joint_Num] =//��ֱ��, ��λΪ��
{
  0, 0, 0
};


int Dynamic_Offset_degree [Joint_Num] =// modified through communicationת����
{
 0, 0, 0           
};//��������Dynamic_Offset_value

unsigned char Dynamic_Offset_degree_char[Joint_Num]={0,0,0};

int Dynamic_Offset_value [Joint_Num] =
{
 0, 0, 0
};//����Dynamic_Offset_degree����õ�,�洢�յ���ת�������Dynamic_Offset_valueֵ��Ҳ����Ҫ�ﵽ��ֵ

int Dynamic_Offset_value_per[Joint_Num]=
{
 0,0,0
};//����״̬ʱ��ÿ�����ӵ�ƫ�ƽ�

int Dynamic_Offset_value_last[Joint_Num]=
{
 0,0,0
};//�յ�ת������֮ǰ��Dynamic_Offset_valueֵ

int Dynamic_Offset_value_t[Joint_Num]=
{
 0,0,0
};//ʵʱ��Dynamic_Offset_valueֵ

unsigned char Neighbouring_Offset_degree[Joint_Num] =// modified through communication ÿ�ؽ���λ��
{
// 0,60,120
  0,80,120
};
//������������λ��
unsigned char Amplitude_degree[Joint_Num] =// modified through communication ���ؽڰڷ�
{
  15,20,25
//   90,20,25
};//����,���ưڷ���Asin(),���90��
unsigned char Amplitude_degree_tem[16][Joint_Num] =//16���ڷ�����Ӧ�ٶȣ�0-15
{
  {0.1*A_MAX,0.1*A_MAX+10,0.1*A_MAX+15},//0 ���ٶ�С�ڷ�С
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
  {0.85*A_MAX,0.85*A_MAX+10,0.85*A_MAX+15}//15���ٶȴ�ڷ���
};
/////////////----��������--END----//////////////////


//*************************delay��ʱ����**********************
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


//*************************���ø��˿�************************
void port_init(void)
{
 PORTA = 0x00;//0000 0001
 DDRA  = 0x00;//0000 0001 PA0��������ߵ�ƽ
// PORTB = 0xFF;//0xF0;//1111 0000
// DDRB  = 0xFF;//0xF0;//1111 0000
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
// PORTD = 0xFF;//0xD0;//1101 0000
// DDRD  = 0xFF;//0xD0;//1101 0000
// PORTE = 0xFE;//1111 1110
// DDRE  = 0xFE;//1111 1110
  PORTE = 0xFE;//��2��PE2
 DDRE  = 0xFE;
// PORTF = 0xFF;//0x00;
// DDRF  = 0xFF;//0x00;
// PORTG = 0x1F;//0x00;
// DDRG  = 0x1F;//0x00;
///////////////
// PORTB = 0xF0;//1111 0000
// DDRB  = 0xF0;//1111 0000
 PORTB = 0xF4;//��1��PB2��&&&&&&&&&&&&&&&&&&
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

//*****************************TIMER3 ��ʼ��***************************
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
 TCCR3B= 0x1A; //start Timer��8��Ƶ,����PWMģʽ
}

//*****************************TIMER1 ��ʼ��***************************
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
 TCCR1B= 0x1A; //start Timer��8��Ƶ,����PWMģʽ
}




void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;       //*���ܺͷ�������֡������λ�ַ����ȱ���
 
 UBRR0H = 0x00; //set baud rate hi
 UBRR0L = 0x33; //set baud rate lo
  
 UCSR0A = 0x00; //enable
 UCSR0B = 0x98;
}

//*************************initialize all peripherals***************
void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts,���������ж�
 XDIV  = 0x00; //xtal divider,ϵͳʱ�ӷ�Ƶ���ƼĴ���
 XMCRA = 0x00; //external memory���ⲿ��չ�洢�����ƼĴ���A
 port_init();//�˿ڳ�ʼ��
 //timer3_init();//��ʱ��3��ʼ��
 //timer1_init();//��ʱ��1��ʼ��
 uart0_init();//����ͨ���ж�
 MCUCR = 0x00;//MCU���ƼĴ�����57ҳ
 //EICRA = 0x03; //extended ext ints���ⲿ�жϿ��ƼĴ���A
 EICRA=0x00;
 EICRB = 0x00; //extended ext ints���ⲿ�жϿ��ƼĴ���B
 //EIMSK = 0x01;//�ⲿ�ж����μĴ���
 EIMSK = 0x00;
// TIMSK = 0x00; //timer interrupt sources��T/C�ж����μĴ���
 TIMSK = 0x44;
 ETIMSK = 0x04; //extended timer interrupt sources����չT/C�ж����μĴ���
 SEI(); //re-enable interrupts
}



 

void senddata(unsigned char data)
{
   /* �ȴ����ͻ�����Ϊ�� */
   while (!( UCSR0A & (1<<UDRE0)))
   ;
   /* �����ݷ��뻺��������������*/
     UDR0 = data;
}


void uart_putchar(char c) //����һ���ֽ�
{
   while( !( UCSR0A & (1<<UDRE0)))
      ;
   UDR0=c;
}

void uart_word(char String[])//����һ���ַ���
{
   int i;
   while(String[i]!='\0') 
   {
      uart_putchar(String[i]);
	  i++;
   }
}


//*********************sin����***************************
double Sin(unsigned char instant, unsigned char speed, int Offset_degree)
{

	double x,y;
	double temp;
        if(speed == 0)
          return 0;
        else
        {
          x = PI *2.0 *  ((double)instant / (double)speed) - PI * (double)Offset_degree / 180.0;       
		 //100115 �Ԫ�?����ͺ�ǳ���һ����Χ�󣬵���Ķ�������         
         if(x>(2.0*PI))
            x=x-2.0*PI;
        else if(x<0.0)
            x=x+2.0*PI;

		  if(x<=(PI/2.0))
		y=x-x*x*x/6.0;//sinx,����

          if(x>(PI/2.0)&&x<=PI)
	   {
		temp=PI-x;
		y=temp-temp*temp*temp/6.0;//����
	   }

          if(x>PI&&x<=(PI*3.0/2.0))
	   {
		temp=x-PI;
		y=temp*temp*temp/6.0-temp;//����
	   }

	  if(x>(PI*3.0/2.0))
	   {
	        temp=2.0*PI-x;
	        y=temp*temp*temp/6.0-temp;//����
	   }

	 return y;

        }
}

//**********************************��̬ƫ��ת��Ϊ������*************************
void init_static_offset(void)//����������void
{
  unsigned char i;

  for(i=0; i<Joint_Num; i++)
  {
       Static_Offset_value[i] =((int) ((Static_Offset_degree[i] * 10) * FLK)>>3)-1;//1��10us
  }
}

//**********************************��̬ƫ��ת��Ϊ������*************************
void init_dynamic_offset(void)//����������void
{
  unsigned char i;
  
   for(i=0; i<Joint_Num; i++)
   {
   Dynamic_Offset_value[i] = ((int) ((Dynamic_Offset_degree[i] * 10) * FLK)>>3)-1;
   }
}
//***********************************�������տ�����******************************

void calculate_data(void)   //���ÿ������жϸĶ���OCRnXֵ//����������void
{
  unsigned char i;
  if(d_change_t_v==1)//*����ı�ʱ��1����ʵʱƫ�ƽǸ���last��Ϊǰһ״̬ƫ�ƽ�
  {
     d_change_t_v=0;
	 for(i=0;i<Joint_Num;i++)
	 {
	   Dynamic_Offset_value_last[i]=Dynamic_Offset_value_t[i];
     }
  }
  if((d_change_t==1)&&(t_dynamic>0)&&(t_dynamic<=(Speed_dynamic/2)))//����״̬
   {
      for(i=0;i<Joint_Num;i++)
	  {
	    Dynamic_Offset_value_per[i]=(Dynamic_Offset_value[i]-Dynamic_Offset_value_last[i])/(Speed_dynamic/2);
	    Dynamic_Offset_value_t[i]=Dynamic_Offset_value_last[i]+(Dynamic_Offset_value_per[i])*t_dynamic;
	  }
      for(i=0; i< Joint_Num; i++)
       Joint_Angle_value[i] =MiddlePosition_value + Static_Offset_value[i]+Dynamic_Offset_value_t[i]+  ((int) (Amplitude_degree[i] * Sin( Instant, Speed, Neighbouring_Offset_degree[i]) *10*FLK)>>3)-1;
    }
   else//�ȶ�״̬
   {
      d_change_t=0;
      for(i=0; i< Joint_Num; i++)
        Joint_Angle_value[i] =MiddlePosition_value + Static_Offset_value[i]+Dynamic_Offset_value[i]+  ((int) (Amplitude_degree[i] * Sin( Instant, Speed, Neighbouring_Offset_degree[i]) *10*FLK)>>3)-1;
   }  
}  //��������ж�д��Ĵ�����ֵ

//*��λ���ģ�������ʲô��

//***********************************��ʱ����ת��Ϊ������************************
void init_data(void)    // void init_data() ��Ϊvoid init_data(void) wsg20120907
{
  PWM_Period_value=((int)(PWM_Period_ms*1000*FLK)>>3)-1; //pwm���� 20ms����ɼ�����ֵ
  MiddlePosition_value=((int)(PWM_Duty_Middle_us*FLK )>>3)-1; //1500us �м�λ�û��ɼ���ֵ
  LeftLimit_value=((int)(PWM_Duty_LeftLimit_us*FLK)>>3)-1; //900us ����λ�ü�����ֵ��60��  1��10us
  RightLimit_value=((int)(PWM_Duty_RightLimit_us*FLK)>>3)-1; //2100us �Ҽ���λ�ü�����ֵ��60��
}
	
//********************��unsigned char����ת��Ϊint����**************************
//flag=1ʱתΪ��������ת��flag=0ʱתΪ��������ת
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



//*********************һ��16λ���飬��Ϊ2��8λ����****************************
//������Ϊ16λ������data_init[16]���2������Ϊ8������
//�ֱ�Ϊdata_a[8],data_b[8]
//unsigned char��
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

//*******************��2��8λ������˳�����һ��16λ����************************
//��2������Ϊ8������data_aa[8],data_bb[8]
//˳�����һ������Ϊ16λ������data_inits[16]
//unsigned char��
void two2one(unsigned char *data_inits,unsigned char *data_aa,unsigned char *data_bb)
{
  for(s=0;s<=7;s++)
   {
   data_inits[s]=data_aa[s];//*��data_aa��ֵ˳�θ�ֵ��data_inits
   }
  for(s=0;s<=7;s++)
   {
   data_inits[s+8]=data_bb[s];//*��data_bb��ֵ˳�θ�ֵ��data_bb�ĸ߰�λ
   }
}

//*********************һ��24λ���飬��Ϊ3��8λ����****************************
//������Ϊ32λ������data_init[24]���3������Ϊ8������
//�ֱ�Ϊdata_a[8],data_b[8],data_c[8]
//unsigned char��
void one2three(unsigned char *data_init,unsigned char *data_a,unsigned char *data_b,unsigned char *data_c)
{
  for(s=0;s<=7;s++)
   {
   data_a[s]=data_init[s];//*��data_init[]��ǰ��λ�γ�һ���µ�����data_a[]
   }
  for(s=8;s<=15;s++)
   {
   data_b[s-8]=data_init[s];//*��data_init[]�ھ�λ����ʮ��λ�γ�һ���µ�����data_b[]
   }
  for(s=16;s<=23;s++)
   {
   data_c[s-16]=data_init[s];//*��data_init[]��ʮ��λ�����ڶ�ʮ��λ�γ�һ���µ�����data_c[]
   }
}

//*******************��3��8λ������˳�����һ��24λ����************************
//��3������Ϊ24������data_aa[8],data_bb[8],data_cc[8]
//˳�����һ������Ϊ32λ������data_inits[32]
//unsigned char��
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


//*********************һ��48λ���飬��Ϊ6��8λ����****************************
//������Ϊ48λ������data_init[48]���6������Ϊ8������
//�ֱ�Ϊdata_a[8],data_b[8],data_c[8],data_d[8],data_e[8],data_f[8]
//unsigned char��
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

//*******************��6��8λ������˳�����һ��48λ����************************
//��6������Ϊ8������data_aa[8],data_bb[8],data_cc[8],data_dd[8]
//˳�����һ������Ϊ48λ������data_inits[48]
//unsigned char��
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



// d_tem 0  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15 //7ǰ�󣬺���
//dd_tem 0  1  2  3  4  5  6  7  6  5  4   3   2   1   0   0 //�Ӹ�����
//********************���ȵ�d_temת��Ϊ����ͳһ��dd_tem****************
//flag_c=1ʱ����תd_cС��7���Ӹ��ţ�flag_c=0����תd_c���ڵ���7
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


//********************************��дi2c****************************************
//дi2c
//������Ϊdata_l��data_tem����д���ַΪi2c_address��data_i2c����
//unsigend char��
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

//��i2c
//������Ϊdata_l�ĵ�ַΪi2c_address��data_i2c���������data_tem
//unsigned char��
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


//***********************��һ��ʹ��i2c�����ݳ�ʼ��*******************************
void i2c_init(void)
{	
      int i;
	  senddata(0xab);
	  switch(redata_i2c&0x0f)
	  {
	    case 0x01://FISHID ��Ϊ0x01;
		          i2c_write(ADD_i,FISHID_I2C,FISHID_init,1); 
				  break;
		case 0x02://Ƶ����ΪƵ��1
		          i2c_write(ADD_f,freqs_I2C,freqs_init,1);
				  break;
	    case 0x03://��ֱ����
		          i2c_write(ADD_t,degree_I2C,Static_Offset_degree_init,3);
				  break;
		case 0x04://Speed_tem[16]�ٶȣ�s_tem
		          one2two(Speed_tem_init,Speed_tem_init1,Speed_tem_init2);
		          i2c_write(ADD_s1,Speed_tem_I2C1,Speed_tem_init1,8);
				  delay_nms(200);
		          i2c_write(ADD_s2,Speed_tem_I2C2,Speed_tem_init2,8);
				  break;
	    case 0x05://Neighbouring_Offset_degree[3]�ͺ��
			      i2c_write(ADD_d,Neighbouring_Offset_degree_I2C,Neighbouring_Offset_degree_init,3);	         
				  break;
	    case 0x06://Amplitude_degree_tem[16][3]�ڷ�,s_tem
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
	    case 0x0A://Dynamic_Offset_degree_origin[8][3]ת��ƫ�ƽ�,d_tem
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
		         //��i2c����
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


//***********************************�����ж�*****************************
#pragma interrupt_handler uart0_rx_isr:19
void uart0_rx_isr(void) 
{
       redata=UDR0;
	   if(1)//?//�жϼĴ�����ֵ���Ƿ����
	   {
		 if(isSrlStart)//���Ѿ����Ϳ�ʼλ���жϵ�ַ���Ƿ�ƥ��
		 {
		   if(addr)//����ַ���Ѿ�ƥ�䣬׼����������
		   {
			   if(isInstruction==1|y_changing)//�Ѿ����ܹ������ˣ���ô�ж��Ƿ��ǽ���λ
			   {
				   if(redata==0xfc)
				   { 
					   e_change=1;//
					   is_uart=1;//�˳��Զ�ģʽ�������ֶ�����ģʽ
				   }
				   else//���յ��Ĳ��ǽ���λ����ô����ָ����Ч�����½�����һ������
				   {
			  	      isSrlStart=0;
					  addr=0;
					  isInstruction=0;
				   }
			   }
			   else//û�н��ܹ�������������
			   {
				   switch(redata&0xf0)
				   {
				   case 0xD0://�ٶ�
					   redata_fan=redata;
					   s_change=1;
					   m_change=0;
					   y_change=0;
					   isInstruction=1;
					   redata_tem=redata;
					   break;
				   case 0xE0://����
			            redata_fan=redata;
						d_change=1;
						//d_change_t=1;
						//d_change_t_v=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x00://��ֱ
			            t_change=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x10://��ֱ�Ƿ񱣴�
			            ts_change=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x20://������id
			            id_change++;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						break;
				   case 0x40://����Ƶ��
			            f_change=1;
						m_change=0;
						y_change=0;
						isInstruction=1;
						redata_tem=redata;
						//PORTD=0xf4;						
						break;
				   case 0x70:///����ģ̬
			            m_change=1;
						y_change=0;
						m_cishu=0;
						mm_cishu=0;
						mmm_cishu=m_time-1;
						isInstruction=1;
						m_tem=redata;
						break;
				   case 0x30://ԭʼ��ת��ģʽ
			            y_change=1;
                        y_changing=1;
						m_change=0;
						y_cishu=0;
						yy_cishu=0;
						yyy_cishu=y_time-1;
						cishu_y=0;
						y_tem=redata;
						switch(y_tem)//ת�䵵��ͬ�����ֵ�ʱ�䲻ͬ����Խ��ʱ��Խ����
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
/*			       case 0xb0://���ķ���
			            a_change=1;
						m_change=0;
						y_change=0;
						redata_tem=redata;
						break;
*/
	/////////----����ָ��--BEGIN----//////////////

			       case 0x50:
						isInstruction=1;
			  	   		if(redata==0x55)//�����ٶȷ���ֵ
						{
			  			 senddata(0x11);//��ʼ��־λ
			  			 senddata(FISHID[0]);//���id�ţ� 
			  			 senddata(freqs[0]+1);//Ƶ�ʵ���1-4
			  			 senddata(Static_Offset_degree[0]);//��һ�ؽڵ�ֱ����
			  			 senddata(Static_Offset_degree[1]);//�ڶ��ؽڵ�ֱ����
			  			 senddata(Static_Offset_degree[2]);//�����ؽڵ�ֱ����
			  			 senddata(s_tem);//�ٶȵ�����Ӧ�ٶȡ�����
			  			 senddata(Speed);//�ٶ�ֵ
			  			 senddata(Amplitude_degree[0]);//��һ�ؽڰڷ�
			  			 senddata(Amplitude_degree[1]);//�ڶ��ؽڰڷ�
			  			 senddata(Amplitude_degree[2]);//�����ؽڰڷ�
			  			 senddata(d_tem);//���򵵣���Ӧת��ƫ�ƽǶ�
			  			 senddata(flag);//��ת������ת
			  			 senddata(Dynamic_Offset_degree_char[0]);//��һ�ؽ�ת��ƫ�ƽǶ�
			  			 senddata(Dynamic_Offset_degree_char[1]);//�ڶ��ؽ�ת��ƫ�ƽǶ�
			  			 senddata(Dynamic_Offset_degree_char[2]);//�����ؽ�ת��ƫ�ƽǶ�
			  			 senddata(Neighbouring_Offset_degree[0]);//0
			  			 senddata(Neighbouring_Offset_degree[1]);//�ڶ��ؽ��ͺ��
			  			 senddata(Neighbouring_Offset_degree[2]);//�����ؽ��ͺ��
						}//if(redata==0x55)	
						break;
				   case 0x60:
						isInstruction=1;
			            if(redata==0x66)//�������Ĳ������浽I2C
						{
			  			 i2c_canshu=1;
						}//if(redata==0x66)	
						break;
				   
				   case 0xb0:
						isInstruction=1;
			            if(redata==0xb1)//��speed
			            {
			            Speed--;
			  			Speed_tem_z[s_tem]=Speed;
						}//if(redata==0xb1)
						
						if(redata==0xb2)
						{
			 			Speed++;
			 		    Speed_tem_z[s_tem]=Speed;
						}//if(redata==0xb2)
						
						if(redata==0xb3)//��һ�ؽ�ƫ�ƽǣ���1
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
			
						if(redata==0xb4)//��һ�ؽ�ƫ�ƽǣ���1
						{
			 			 dd_tem=change_dd(d_tem);
			 			 flag=change_flag(d_tem);
			  			 Dynamic_Offset_degree_char[0]++;
			  			 Dynamic_Offset_degree[0]=char2int(Dynamic_Offset_degree_char[0],flag);
			  			 init_dynamic_offset();			  
			  			 Dynamic_Offset_degree_origin[dd_tem][0]=Dynamic_Offset_degree_char[0];
						}//if(redata==0xb4)
			
						if(redata==0xb5)//�ڶ��ؽ�ƫ�ƽǣ���1
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
			
					   if(redata==0xb6)//�ڶ��ؽ�ƫ�ƽǣ���1
					   {
			  		    dd_tem=change_dd(d_tem);
			  			flag=change_flag(d_tem);
			  			Dynamic_Offset_degree_char[1]++;
			  			Dynamic_Offset_degree[1]=char2int(Dynamic_Offset_degree_char[1],flag);
			  			init_dynamic_offset();			  
			  			Dynamic_Offset_degree_origin[dd_tem][1]=Dynamic_Offset_degree_char[1];
					   }//if(redata==0xb6)
			
					   if(redata==0xb7)//�����ؽ�ƫ�ƽǣ���1
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
			
					   if(redata==0xb8)//�����ؽ�ƫ�ƽǣ���1
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
			           if(redata==0xc1)//�ĵ�һ�ؽڰڶ�����
					   {
			  		     if(Amplitude_degree[0]==0)
			  			 {
			  			 }
			  			 else
			  			 {
			  			 Amplitude_degree[0]--;
			  			 }
			  		   Amplitude_degree_tem[s_tem][0]=Amplitude_degree[0];//��Ϊ��ǰ�ٶȵ��İڷ�ֵ
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
					  
					  if(redata==0xc3)//�ĵڶ��ؽڰڶ�����
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
					  
					  if(redata==0xc5)//�ĵ����ؽڰڶ�����
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
					 
					 if(redata==0xc7)//�ڶ��ؽڼ��ͺ��
					 {
			  		  if(Neighbouring_Offset_degree[1]==0)
			  		  {
			  		  }
			  		  else
			  		  {
			  		  Neighbouring_Offset_degree[1]--;
			  		  }					  
					 }//if(redata==0xc7)
					 
					 if(redata==0xc8)//�ڶ��ؽڼ��ͺ��
					 {
			  		  if(Neighbouring_Offset_degree[1]==255)
			  		  {
			  		  }
			  		  else
			  		  {
			 		  Neighbouring_Offset_degree[1]++;
			  		  }
					 }//if(redata==0xc8)
					 
					 if(redata==0xc9)//�����ؽڼ��ͺ��
					 {
			  		  if(Neighbouring_Offset_degree[2]==0)
			  		  {
			  		  }
			  		  else
			  		  {
			  		  Neighbouring_Offset_degree[2]--;
			  		  }
					 }//if(redata==0xc9)
					 
					 if(redata==0xc0)//�����ؽڼ��ͺ��
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

	/////////----����ָ��--END----//////////////

			    case 0xf0://�Ƿ����λ
			            if(redata==0xfc)
						{
							e_change=1;//
							is_uart=1; //�˳��Զ�ģʽ�������ֶ�����ģʽ
						}//if(redata==0xfc)
						break;		
			    default://���յ��Ĳ���ָ��Ҳ���ǽ���λ�����½�����һ������
			  	      isSrlStart=0;
					  addr=0;
			     }//switch(redata&0xf0)
			  } //if(isInstruction==1) else
			  
		   }//if(addr)
		   
		   else//��ַ��û���գ����Ѿ����յ���ƥ��
		   {
		     if(redata==(0x90|FISHID[0]))//���յ�ַ�룬ƥ�䣬��λ��־λ
			 {
			 addr=1;
			 }//if(redata==(0x90|FISHID))
			 
			 else//��ַ�벻ƥ�䣬���㿪ʼλ��־λ�����½�����һ��ָ��
			 {
			 isSrlStart=0;
			 }//if(redata==(0x90|FISHID)) else
			 
		   }//if(addr) else
		   
		 }//if(isSrlStart)

	/////////----��ʼ��i2c��ͨ��Э��--BEGIN----//////////////

		 else if(isSrlStartI2c)//���Ѿ����ͳ�ʼ��i2c����ʼλ���жϵ�ַ�루0x74����ȷ��ָ������ã��Ƿ�ƥ��
		 {
			 if(addrI2c)//����ַ���Ѿ�ƥ�䣬׼����������
			 {
				 if(isInstructionI2c)//�Ѿ����ܹ������ˣ���ô�ж��Ƿ��ǽ���λ
				 {
					 if(redata==0xfb)
					 {
						 i2c_change=1;
						 is_uart=1;
					 }
					 else//���յ��Ĳ��ǽ���λ����ô����ָ����Ч�����½�����һ������
					 {
						 isSrlStartI2c=0;
						 addrI2c=0;
						 isInstructionI2c=0;
					 }
				 }
				 else//û�н���������������
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


			 else//��ַ��û�н��գ������ѽ��յ���ƥ��
			 {
				 if(redata==0x74)//���յ�ַ�룬ƥ�䣬��λ��־λ
				 {
					 addrI2c=1;
				 }
				 else//��ַ�벻ƥ�䣬���㿪ʼλ��־λ�����½�����һ��ָ��
				 {
					 isSrlStartI2c=0;
				 }					
			 }
		 }
		 else
		 {
		    if(redata==0xaa)//�ж���ʼλ����λ��־λ
			{
			  isSrlStart=1;
			}
			if(redata==0xcc)//�жϳ�ʼ��i2c����ʼλ
			{
				isSrlStartI2c=1;
			}
		 }//if(isSrlStart) else

	/////////----��ʼ��i2c��ͨ��Э��--END----//////////////

	}//if(1)
}


//***********************************������3����ж�*****************************
#pragma interrupt_handler timer3_ovf_isr:30
void timer3_ovf_isr(void)
{
  unsigned char i;
  
 TCNT3H = 0x00 ; //reload counter high value  INVALID SETTING
 TCNT3L = 0x00 ; //reload counter low value   INVALID SETTING
 
 if(Speed!=0)   //��û��������һֱ1500���м�λ��
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

//***********************************������1����ж�*****************************
#pragma interrupt_handler timer1_ovf_isr:15
void timer1_ovf_isr(void)
{
    unsigned char i;
  
    TCNT1H = 0x00 ; //reload counter high value  INVALID SETTING
    TCNT1L = 0x00 ; //reload counter low value   INVALID SETTING
 
    if(Speed!=0)   //��û��������һֱ1500���м�λ��
    {
          Instant++;
		    //ע�⣬��������ʱ�����ж��У���������Instant++��ʵ���ǲ���Ҫ�ģ�ֻ��һ����ʱ���жϼӾͿ����ˡ�
	        //�������Ӻ�ֻ��һ��������ֻ�ǵ���˶����ˣ���Ϊ�������ӣ��൱����ɢ��ļ����ԭ����һ�루ԭ��20ms������10ms�������۷���ʱע�⼴�ɣ���Ӱ��ʹ�á�
	        //��ϸ����  ��һ���ڶ�����T����Ϊ ��ߡ����м䡪���ұߡ����м䡪����ߣ�
	        //ֻ��һ����Instant++��һ���ڶ����ڱ���ɢΪspeed�ݣ�ÿ�ݼ��20ms������T=��speed-1��X20    ��ʱ����ٶ�ʱ��speed=23
	        //��������Instant++��һ���ڶ����ڱ���ɢΪspeed�ݣ�ÿ�ݼ��20/2ms������T=��speed-1��X10    ��ʱ����ٶ�ʱ��speed=45(������23һ��Ч��) 
	        //���ϼ��㹫ʽ�����������¡���09.06.25
			//ֻ��һ����Instant++��һ���ڶ����ڱ���ɢΪspeed�ݣ�ÿ�ݼ��20ms������T=��speed��X20    ��ʱ����ٶ�ʱ��speed=23
			//��������Instant++��һ���ڶ����ڱ���ɢΪspeed�ݣ�ÿ�ݼ��20/2ms������T=��speed��X10    ��ʱ����ٶ�ʱ��speed=45(������23һ��Ч��)
  	    Instant=Instant%Speed;
   }
    calculate_data(); 
	OCR1A=Joint_Angle_value[0];	 
	OCR1B=Joint_Angle_value[1];  
}
//*************************************************************************
//********************************������***********************************
//*************************************************************************
void main(void)
{   
     unsigned char i;
  	 unsigned char tem;
	 unsigned char tem_y;
	 unsigned char j; 
 	 init_devices();//��ʼ��
	 TWI_Master_Initialise(); //*IIC���Ƴ�ʼ��
	 SEI();
     /////////////////////////////////////////////////////
	 //��i2c����
	 i2c_read(ADD_i,FISHID_I2C,FISHID,1);
	 i2c_read(ADD_f,freqs_I2C,freqs,1);
	 //PORTD=freq[freqs[0]];
	 i2c_read(ADD_t,degree_I2C,Static_Offset_degree,3);
	 i2c_read(ADD_s1,Speed_tem_I2C1,Speed_tem_z1,8);
	 i2c_read(ADD_s2,Speed_tem_I2C2,Speed_tem_z2,8);
	 two2one(Speed_tem_z,Speed_tem_z1,Speed_tem_z2);
	 for(i=0;i<=15;i++)
	 {
	      Speed_tem[i]=Speed_tem_z[i];    //Speed_tem[i] = Speed_Amplitudeem_z[i];//Speed_Amplitudeem_z[i];//*Ϊʲô���д�����
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
	 TCCR3B=0x00;		//��ͣ��ʱ��������3  �����λ��Ϊ0����
	 TCCR1B=0x00;		//��ͣ��ʱ��������3  �����λ��Ϊ0����
	
	 ICR3=PWM_Period_value;//������3����ֵTOP
	 ICR1=PWM_Period_value;//������1����ֵTOP
	 OCR1A=Joint_Angle_value[0];
	 OCR1B=Joint_Angle_value[1];
	 OCR3A=Joint_Angle_value[2];
	 /*wang genggai*/
	// OCR3B=Joint_Angle_value[2];
	 
	 SEI();         //re-enable interrupts
	 TCCR3B=0x1A;	//������ʱ��������3
	 TCCR1B=0x1A;	//������ʱ��������1
	 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	while(1)
	{
///////////----�ֶ�ģʽ--BEGIN//////////////////////////////////////////
	   if(is_uart==1)//�ֶ�ģʽ
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
		        i2c_write(ADD_d,Neighbouring_Offset_degree_I2C,Neighbouring_Offset_degree,3);//�ͺ��
		        senddata(baocun_i);
				break;
		 case 3://����
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
		 case 7://ת��ƫ�ƽ�
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

	   if(e_change)//���յ�����λ,����ָ��
	   {
	   //senddata(FISHID[0]);
	   redata=redata_tem;
	     if(s_change)//�ٶȱ仯
		 {
			 id_change=0;
			id_fan=(FISHID[0]<<5)|(redata_fan&0x0f);
		    if((redata_fan>>4)<<4==0xd0)
		      id_fan=id_fan|0x10;
		    ///////senddata(id_fan);	//ǰ��λ������ʾid�ţ��ڣ�λΪ��ʱΪ�ٶȣ���Ϊ���򣬺���λΪ������ٶȻ��Ƿ����ֵ
	   
			//senddata(t_dynamic);////////////////////
			s_tem=redata&0x0f;//�ٶȵ�0-15
			if(s_tem==0)//�ٶ�0ʱ��ֹͣ��ֱ
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
			for(i=0;i<=2;i++)//��Ӧ�ڷ��仯
			   Amplitude_degree[i]=Amplitude_degree_tem[s_tem][i];
//		    init_dynamic_offset();
		 }//if(s_change)
		 
		 if(d_change)//����仯,�й���״̬08.01.14
		 {
			 id_change=0;
			id_fan=(FISHID[0]<<5)|(redata_fan&0x0f);
		    if((redata_fan>>4)<<4==0xd0)
		      id_fan=id_fan|0x10;
		    /////senddata(id_fan);	//ǰ��λ������ʾid�ţ��ڣ�λΪ��ʱΪ�ٶȣ���Ϊ���򣬺���λΪ������ٶȻ��Ƿ����ֵ
	   
			// senddata(t_dynamic);////////////////////
			 t_dynamic=0;//����״̬��ʼ
			 d_tem=redata&0x0f;//ת�䵵0-15
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
		 
/*		 if(a_change)//���ȱ仯
		 {
			 tem=redata&0x0f;//���ȵ�0-15
			 for(i=0;i<=2;i++)
			    Amplitude_degree[i]=Amplitude_degree_tem[tem][i];
		 }//if(a_change)
*/		 
		 if(t_change)//��ֱ
		 {
			 id_change=0;
		     senddata(FISHID[0]);
			 tem=redata&0x07;//�ؽ���,0,1,2
			 if(tem>2)
			 {
			 }//if(tem>2)
			 else
			 {
			    if(redata&0x08)//�ҵ�ֱ,1��
				{
				   Static_Offset_degree[tem]++;
				}//if(redata&0x08)
				else//���ֱ,1��
				{
				   Static_Offset_degree[tem]--;
				}//if(redata&0x08) else
			 }//if(tem>2) else
			 init_static_offset();
		 }//if(t_change)
		 
		 if(ts_change)//��ֱ�Ƿ񱣴�
		 {
			 id_change=0;
		   senddata(FISHID[0]);
		   if(redata&0x04)//����
		   {
		       i2c_write(ADD_t,degree_I2C,Static_Offset_degree,3);
		   }//if(redata&0x04)
		   else//������
		   {
		       i2c_read(ADD_t,degree_I2C,Static_Offset_degree,3);		   
		   }//if(redatat&0x04) else
		 }//if(ts_change)
		 
		 if(id_change==2)//������id
		 {
			 id_change=0;
            senddata(FISHID[0]);
			tem=redata&0x0f;
		    FISHID[0]=tem;
            i2c_write(ADD_i,FISHID_I2C,FISHID,1);
			senddata(FISHID[0]);
		 }//if(id_change)
		 
		 if(f_change)//����Ƶ��
		 {
			 id_change=0;

		    if(redata_tem==0x41)//Ƶ��1
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x02);
			  senddata(0xbb);
			}
			else if(redata_tem==0x42)//Ƶ��2
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x16);
			  senddata(0xbb);
			}
			else if(redata_tem==0x43)//Ƶ��3
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x2A);
			  senddata(0xbb);
			}
			else if(redata_tem==0x44)//Ƶ��4
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x3B);
			  senddata(0xbb);
			}
			else if(redata_tem==0x45)//Ƶ��5
			{
			  PORTD=PORTD&0xf4;
			  senddata(0xaa);//0xfc
			  senddata(0x49);
			  senddata(0xff);
			  senddata(0x33);
			  senddata(0xbb);
			}
			else if(redata_tem==0x46)//Ƶ��6
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
						//tem=redata&0x07-4;//0x44-0x47,Ƶ��1-4,tem(0-3)
		    //freqs[0]=tem;
		   // PORTD=freq[freqs[0]];
           // i2c_write(ADD_f,freqs_I2C,freqs,1);
		 }//if(f_change)		
	
	   //�������б�־λ,id_change������if(id_change)֮���if(*_change)������
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
	   
	   if(m_change)//����ģ̬��m_temģ̬��
	   {
	     m_cishu--;//��++��һ�η�Ӧ��
	     if(m_cishu==0xff)
	     {
		   mm_cishu--;//��++��һ�η�Ӧ��
		   if(mm_cishu==0xff)
		   {
			 mmm_cishu++;
			 if(mmm_cishu==m_time)
			 {
		     mmm_cishu=0;
			 //ִ�д���
			 cishu++;
			 if(cishu==3)
			 {
			  cishu=0;
			 }//if(cishu==3)
			 switch(m_tem&0x0f)
			 {
			 case 0x01://ģ̬1
					   switch(cishu)
					   {
					   case 0x00://0,����1
					             Speed=Speed_tem_z[1];
		                         for(i=0;i<=2;i++)		  
		                            Dynamic_Offset_degree [i]=0;
		                         init_dynamic_offset();
								 
								 break;//0
					   case 0x01://1������15
								 Speed=Speed_tem_z[15];
								 for(i=0;i<=2;i++)
								    Dynamic_Offset_degree [i]=0;
								 init_dynamic_offset();
								 break;//1  
					   case 0x02://2��ת��1
		 	                     for(i=0;i<=2;i++)  
				                   Dynamic_Offset_degree [i]=Dynamic_Offset_degree_origin [1][i];
		                         init_dynamic_offset(); 
					             break;//2
					   }//switch(cishu)
			           break;//caseģ̬1
		     case 0x02://ģ̬2
					   switch(cishu)
					   {
					   case 0x00://0����ת1
		 	                     for(i=0;i<=2;i++)
				                   Dynamic_Offset_degree [i]=Dynamic_Offset_degree_origin [1][i];
		                         init_dynamic_offset(); 
     							 break;//0
					   case 0x01://1����ת12
		 	                     for(i=0;i<=2;i++)
				                   Dynamic_Offset_degree [i]=Dynamic_Offset_degree_origin [12][i];
		                         init_dynamic_offset(); 
								 break;//1
					   case 0x02://2��ǰ��12
								 Speed=Speed_tem_z[12];
								 for(i=0;i<=2;i++)
								    Dynamic_Offset_degree [i]=0;
								 init_dynamic_offset();					             
								 break;//2
					   }//switch(cishu)			           
			           break;//caseģ̬2
			 }//switch(m_tem)
			 //�������
			 }//if(mmm_cishu)
		   }//if(mm_cishu)		   
		 }//if(m_cishu)...��ʱ����
       }//if(m_change)
	   
		if(y_change)//ԭʼת��ģʽ
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

						//ִ�д���
						if(cishu_y==1)//�ص��յ�����ǰ���ٶȣ�ֱ��
						{
							y_change=0;
							y_changing=0;
							Speed=y_speed;
							for(i=0;i<=2;i++)		  
								Dynamic_Offset_degree [i]=0;
							for(i=0;i<=2;i++)//��Ӧ�ڷ��仯
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
						//�������

					}//if(yyy_cishu)
				}//if(yy_cishu)		   
			}//if(y_cishu)...��ʱ����
		}//if(y_change)	
	}//if(is_uart==1)

///////////----�ֶ�ģʽ--END//////////////////////////////////////////	

///////////----�Զ�ģʽ--BEGIN//////////////////////////////////////////	

		else//�Զ�ģʽ,is_uart==0
		{
             a_cishu--;//��++��һ�η�Ӧ��
	    	    if(a_cishu==0xff)
	            {
		          aa_cishu--;//��++��һ�η�Ӧ��
		           if(aa_cishu==0xff)
		           {
			           aaa_cishu++;
			           if(aaa_cishu==m_time)
			           {
		                aaa_cishu=0; 
			            //ִ�д���
			            auto_cishu++;
			            if(auto_cishu==7)
			            {
			                 auto_cishu=1;
			             }//if(auto_cishu==6)

					   switch(auto_cishu)
					   {
					   case 0x01://ֱ�Σ�15
						   Speed=Speed_tem_z[15];
						   for(i=0;i<=2;i++)//��Ӧ�ڷ��仯
							   Amplitude_degree[i]=Amplitude_degree_tem[15][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   case 0x02://��ת
						   Speed=Speed_tem_z[15];
						   for(i=0;i<=2;i++)
							{
								Dynamic_Offset_degree_char[i]=Dynamic_Offset_degree_origin[3][i];
								Dynamic_Offset_degree[i]=char2int(Dynamic_Offset_degree_char[i],0);
							}
							init_dynamic_offset();
						   break;
					   case 0x03://ֱ�Σ�11
						   Speed=Speed_tem_z[11];
						   for(i=0;i<=2;i++)//��Ӧ�ڷ��仯
							   Amplitude_degree[i]=Amplitude_degree_tem[11][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   case 0x04://ֱ�Σ�11
						   Speed=Speed_tem_z[11];
						   for(i=0;i<=2;i++)//��Ӧ�ڷ��仯
							   Amplitude_degree[i]=Amplitude_degree_tem[11][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   case 0x05://��ת
						   Speed=Speed_tem_z[11];
                           for(i=0;i<=2;i++)
							{
								Dynamic_Offset_degree_char[i]=Dynamic_Offset_degree_origin[3][i];
								Dynamic_Offset_degree[i]=char2int(Dynamic_Offset_degree_char[i],0);
							}
							init_dynamic_offset();
							break;
						case 0x06://ֱ�Σ�15
						   Speed=Speed_tem_z[15];
						   for(i=0;i<=2;i++)//��Ӧ�ڷ��仯
							   Amplitude_degree[i]=Amplitude_degree_tem[15][i];
						   for(i=0;i<=2;i++)
							   Dynamic_Offset_degree[i]=0;
						   init_dynamic_offset();
						   break;
					   }//switch(auto_cishu)
			           //�������
			         }//if(aaa_cishu)
		         }//if(aa_cishu)		   
		       }//if(a_cishu)...��ʱ����
		    }//�Զ�ģʽ,is_uart==0

///////////----�Զ�ģʽ--END//////////////////////////////////////////	

	}//while(1)
}//main
