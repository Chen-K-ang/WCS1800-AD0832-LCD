#include "ADC0832.h"
#include "delay.h"
#include <reg52.h>
#include "c_lcd1602.h"
#include <math.h>
#include <stdio.h>

#define ABS(x) ((x > 0) ? (x) : (-x))

#define CUR_LIMIT  2.0  //��������ֵ
#define CUR_CHANGE 0.2  //���ε���ֵ
#define LINEARITY  0.06 //wcs1800���Զ�V/A
#define AVERAGE_V  2.5  //ģ�鹩��/2

unsigned char disp[16]  = {"Cur:         A"}; //��һ����ʾ�����ݡ�
unsigned char disp1[16] = {"Limit:       A"}; //�ڶ�����ʾ�����ݡ�

sbit Relay  = P2^0;	 //�̵��������˿�
sbit Reduc  = P3^7;	 //����������˿�
sbit Add    = P3^6;	 //����������˿�
//sbit PWR    = P3^5;

void main(void)
{
	unsigned char voltage_temp = 0;
	unsigned int voltage_result = 0;
	float current_data = 0, voltage_data = 0;
//	double current_data = 0, voltage_data = 0;
	int collect = 0;
	float limit_data = CUR_LIMIT;
	unsigned char cur_buf[6];
	unsigned char limit_buf[6];

	LCD1602_init();
	Delay(200);
	LCD1602_show_str(0, 0, disp); //�ڵ�һ����ʾ�����ݡ�
	LCD1602_show_str(0, 1, disp1); //�ڵڶ�����ʾ�����ݡ�
	sprintf(limit_buf, "%.1f", limit_data);
	LCD1602_show_str(7, 1, limit_buf);
	
	while(1)
	{
		//��ȡ��������
		voltage_temp = ADC0832_conv(1);
		voltage_result += voltage_temp;
		if (++collect >= 20) {
			voltage_result = voltage_result / 20.0f;
			voltage_data = voltage_result / 255.0 * 5.0;
		  	collect = 0;
			voltage_temp = 0;
			//�����������
			current_data = ABS(voltage_data - AVERAGE_V) / LINEARITY;
			//��ʾ����
			sprintf(cur_buf, "%2.2f", (float)current_data);
			LCD1602_show_str(5, 0, cur_buf);
		}

		//�̵�������
		if (current_data > CUR_LIMIT) {	
			Relay = 1;
		}

		/*******���ñ���ֵ����********/
		//��
		if (Add == 0) {
			Delay(200);	 
			limit_data += CUR_CHANGE;
			sprintf(limit_buf, "%0.1f", limit_data);
			LCD1602_show_str(7, 1, limit_buf);
		}
		//��
		if (Reduc == 0) {
			Delay(200);
			limit_data -= CUR_CHANGE;
			sprintf(limit_buf, "%0.1f", limit_data);
			LCD1602_show_str(7, 1, limit_buf);
		}
	}
}

