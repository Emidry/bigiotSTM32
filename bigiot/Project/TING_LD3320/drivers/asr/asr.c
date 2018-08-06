/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#include "asr.h"
#include "ld3320.h"
#include "delay.h"

u8 nAsrStatus=0;                //�����е�״̬����������
								//	LD_ASR_NONE:		��ʾû������ASRʶ��
								//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
								//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
								//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
								//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
								
/*-------------------------------------------------*/
/*��������ld3320����ʶ��                           */
/*��  ������                                       */
/*����ֵ��1���ɹ�  0��ʧ��                         */
/*-------------------------------------------------*/
u8 RunASR(void)
{
	u8 i=0;                           //����forѭ��
	u8 asrflag=0;                     //��־λ
	
	for (i=0; i<5; i++)			      //��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_Init_ASR();			      //��ʼ��ASR
		delay_ms(5);                  //��ʱ5ms
		if (LD_AsrAddFixed()==0)	  //��ӹؼ����ﵽLD3320оƬ��
		{
			LD_reset();			      //���ʧ�ܣ���������LD3320оƬ
			delay_ms(5);		      //��ʱ5ms
			continue;                 //����һ��
		}
		delay_ms(10);                 //��ӳɹ�����ʱ5ms

		if (LD_AsrRun() == 0)         //ִ������ʶ�������
		{
			LD_reset();			      //ʧ�ܣ���������LD3320оƬ
			delay_ms(5);		      //��ʱ5ms
			continue;                 //����һ��
		}

		asrflag=1;                    //��־λ=1����ʾ�ɹ�
		break;					      //ASR���������ɹ����˳���ǰforѭ����
	}

	return asrflag;                   //���ر�־λ ���5�ζ����ɹ�����0  �ɹ�����1
}

/*-------------------------------------------------*/
/*����������ʼ��ʶ������                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 	
void LD_Init_ASR(void)
{
	nLD_Mode=LD_MODE_ASR;
	LD_Init_Common();

	LD_WriteReg(0xBD, 0x00);
	LD_WriteReg(0x17, 0x48);	
	delay_ms(5);
	LD_WriteReg(0x3C, 0x80);    
	LD_WriteReg(0x3E, 0x07);
	LD_WriteReg(0x38, 0xff);    
	LD_WriteReg(0x3A, 0x07);
	LD_WriteReg(0x40, 0);          
	LD_WriteReg(0x42, 8);
	LD_WriteReg(0x44, 0);    
	LD_WriteReg(0x46, 8); 
    LD_WriteReg(0xB8, 0xff); 
	delay_ms(1);
}

/*-------------------------------------------------*/
/*������������Ƿ�ʶ�� æ                          */
/*��  ������                                       */
/*����ֵ��0��æ 1������                            */
/*-------------------------------------------------*/ 
u8 LD_Check_ASRBusyFlag_b2(void)
{
	u8 j;
	u8 flag = 0;
	
	for (j=0; j<10; j++)
	{
		if (LD_ReadReg(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		delay_ms(10);		
	}
	return flag;
}

/*-------------------------------------------------*/
/*��������ִ��ʶ�����̺���                         */
/*��  ������                                       */
/*����ֵ��0��ʶ��æ 1����ȷ                        */
/*-------------------------------------------------*/  
u8 LD_AsrRun(void)
{
	LD_WriteReg(0x35, MIC_VOL);
	LD_WriteReg(0x1C, 0x09);
	LD_WriteReg(0xBD, 0x20);
	LD_WriteReg(0x08, 0x01);
	delay_ms(5);
	LD_WriteReg(0x08, 0x00);
	delay_ms(5);

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD_WriteReg(0xB2, 0xff);	
	LD_WriteReg(0x37, 0x06);
	LD_WriteReg(0x37, 0x06);
	delay_ms(5);
	LD_WriteReg(0x1C, 0x0b);
	LD_WriteReg(0x29, 0x10);
	
	LD_WriteReg(0xBD, 0x00);   
	return 1;
}

/*-------------------------------------------------*/
/*����������ȡʶ����                             */
/*��  ������                                       */
/*����ֵ�����                                     */
/*-------------------------------------------------*/
u8 LD_GetResult(void)
{
	return LD_ReadReg(0xc5);
}

/*-------------------------------------------------*/
/*����������ӹؼ���                               */
/*��  ������                                       */
/*����ֵ��1���ɹ�                                  */
/*-------------------------------------------------*/
u8 LD_AsrAddFixed(void)
{
	u8 k, flag;
	u8 nAsrAddLength;
	
	u8 sRecog[DATE_A][DATE_B] =    //�ؼ����б�����ƴ�����û��Լ��޸�
	{
		"chong qi xi tong",\
		"jiang ge xiao hua",\
		"xiao bei xiao bei",\
		"bei ke wu lian",               };	
	
	u8  pCode[DATE_A] =            //�ؼ��ʣ���Ӧ��ʶ���롣�û��Լ��޸�
	{
		CODE_CQXT,\
		CODE_JGXH,\
		CODE_DK,\
		CODE_GB,                };
	
	flag = 1;
	for (k=0; k<DATE_A; k++)
	{			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD_WriteReg(0xc1, pCode[k] );
		LD_WriteReg(0xc3, 0 );
		LD_WriteReg(0x08, 0x04);
		delay_ms(1);
		LD_WriteReg(0x08, 0x00);
		delay_ms(1);

		for (nAsrAddLength=0; nAsrAddLength<DATE_B; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD_WriteReg(0x5, sRecog[k][nAsrAddLength]);
		}
		LD_WriteReg(0xb9, nAsrAddLength);
		LD_WriteReg(0xb2, 0xff);
		LD_WriteReg(0x37, 0x04);
	}	 

    return flag;
}

/*-------------------------------------------------*/
/*��������ʶ������                             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void ProcessInt_ASR(void)
{
	u8 nAsrResCount=0;

	ucRegVal = LD_ReadReg(0x2B);

	// ����ʶ��������ж�
	// ���������룬����ʶ��ɹ���ʧ�ܶ����ж�
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;

	if((ucRegVal & 0x10) && LD_ReadReg(0xb2)==0x21 && LD_ReadReg(0xbf)==0x35)		
	{	 
		nAsrResCount = LD_ReadReg(0xba);

		if(nAsrResCount>0 && nAsrResCount<=4) 
		{
			nAsrStatus=LD_ASR_FOUNDOK; 				
		}
		else
		{
			nAsrStatus=LD_ASR_FOUNDZERO;
		}	
	}
	else
	{
	    nAsrStatus=LD_ASR_FOUNDZERO;	//ִ��û��ʶ��
	}

    LD_WriteReg(0x2b, 0);
    LD_WriteReg(0x1C,0);/*д0:ADC������*/

	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
	LD_WriteReg(0x2B,0);
	LD_WriteReg(0xBA,0);	
	LD_WriteReg(0xBC,0);	
	LD_WriteReg(0x08,1);	/*���FIFO_DATA*/
	LD_WriteReg(0x08,0);	/*���FIFO_DATA�� �ٴ�д0*/

	delay_ms(1);
}

