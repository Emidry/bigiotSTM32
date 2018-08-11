/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __ASR_H
#define __ASR_H

#include "main.h"

extern u8 nAsrStatus;
extern u8 ucRegVal;


#define DATE_A 50         //�����ؼ���
#define DATE_B 80		 //�ؼ��ʵĴ�С


//�������״̬����������¼������������ASRʶ������е��ĸ�״̬
#define LD_ASR_NONE				0x00	//	��ʾû������ASRʶ��
#define LD_ASR_RUNING			0x01	//	��ʾLD3320������ASRʶ����
#define LD_ASR_FOUNDOK			0x10	//	��ʾһ��ʶ�����̽�������һ��ʶ����
#define LD_ASR_FOUNDZERO 		0x11	//	��ʾһ��ʶ�����̽�����û��ʶ����
#define LD_ASR_ERROR	 		0x31	//	��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬

//MIC����
#define MIC_VOL 0X4c

u8 RunASR(u8 b8, char* path);
void LD_Init_ASR(u8 b8);
u8 LD_Check_ASRBusyFlag_b2(void);
u8 LD_AsrRun(void);
u8 LD_GetResult(void);
u8 LD_AsrAddFixed(char* path);
void ProcessInt_ASR(void);
void Stop_ASR(void);

#endif
