/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#include "mp3.h"
#include "ld3320.h"
#include "delay.h"
#include "ff.h"   
#include "usart1.h"

FIL   MP3_file;	  		//�ļ�����
UINT  Read_Bytes;       //���������ڱ����ȡ������ʵ�ʶ�ȡ�ĸ���
UINT  Write_Bytes ;     //���������ڱ���дȡ������ʵ��д��ĸ���

u32 nMp3Size=0;			//mp3�ļ��Ĵ�С
u32 nMp3Pos=0;			//mp3�ļ���ƫ��(����ƫ�ƾ����ļ���С)
u8  bMp3Play=0;			//������¼����MP3��״̬  0������  1������

/*-------------------------------------------------*/
/*������������MP3                                  */
/*��  ����path��MP3�ļ�·��                        */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
u8 PlayDemoSound_mp3(char *path)
{
	u8 res;
	
	res=f_open(&MP3_file,path,FA_OPEN_EXISTING | FA_READ );    //��һ��mp3�ļ�
	if(res != FR_OK)                                           //���������
	{
		printf("���ļ�������\r\n");                            //������ʾ��Ϣ
		f_close(&MP3_file);                                    //�ر��ļ�
		return res;		                                       //���ش���ֵ
	}

	bMp3Play = 1;                              //1������״̬
	nMp3Size = f_size(&MP3_file)-1;            //��ȡ�ļ���С	
	printf("�ļ���С=%d�ֽ�\r\n",nMp3Size);    //������ʾ��Ϣ
	
	LD_Init_MP3();                             //��LD3320��ʼ��Ϊ����MP3ģʽ		
	LD_play();                                 //��ʼ����

    return 0;
}

/*-------------------------------------------------*/
/*����������ʼ��MP3ģʽ                            */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void LD_Init_MP3(void)	
{
	nLD_Mode = LD_MODE_MP3;	   //��ǰ����MP3����
	LD_Init_Common();		   //ͨ�ó�ʼ��

	LD_WriteReg(0xBD,0x02);	   //�ڲ�������� ��ʼ��ʱд��FFH
	LD_WriteReg(0x17, 0x48);   //д48H���Լ���DSP
	delay_ms(100);

	LD_WriteReg(0x85, 0x52); 	//�ڲ��������� ��ʼ��ʱд��52H
	LD_WriteReg(0x8F, 0x00);  	//LineOut(��·���)ѡ�� ��ʼ��ʱд��00H
	LD_WriteReg(0x81, 0x00);	//���������� ����Ϊ00HΪ�������
	LD_WriteReg(0x83, 0x00);	//���������� ����Ϊ00HΪ�������
	LD_WriteReg(0x8E, 0x05);	//�����������  ���Ĵ�������Ϊ00HΪ�������	�˴������ر�
	LD_WriteReg(0x8D, 0xff);	//�ڲ�������� ��ʼ��ʱд��FFH
	delay_ms(100);
	LD_WriteReg(0x87, 0xff);	//ģ���·���� MP3���ų�ʼ��ʱд FFH
	LD_WriteReg(0x89, 0xff);    //ģ���·���� MP3����ʱд FFH
	delay_ms(100);
	LD_WriteReg(0x22, 0x00);   //FIFO_DATA���޵�8λ
	LD_WriteReg(0x23, 0x00);	//FIFO_DATA���޸�8λ
	LD_WriteReg(0x20, 0xef);   //FIFO_DATA���޵�8λ
	LD_WriteReg(0x21, 0x07);	//FIFO_DATA���޸�8λ
	LD_WriteReg(0x24, 0x77);    
	LD_WriteReg(0x25, 0x03);	
	LD_WriteReg(0x26, 0xbb);    
	LD_WriteReg(0x27, 0x01); 	
}

/*-------------------------------------------------*/
/*��������׼������mp3                              */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void LD_play(void)	 
{
	nMp3Pos=0;         
	bMp3Play=1;

	if (nMp3Pos >=  nMp3Size)
		return ; 

	fill_the_fifo();

    LD_WriteReg(0xBA, 0x00);
	LD_WriteReg(0x17, 0x48);
	LD_WriteReg(0x33, 0x01);
	LD_WriteReg(0x29, 0x04);
	
	LD_WriteReg(0x02, 0x01); 
	LD_WriteReg(0x85, 0x5A);

}

/*-------------------------------------------------*/
/*������������fifoΪ����mp3��׼��                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void fill_the_fifo(void)
{
    u8 ucStatus;
	
	ucStatus = LD_ReadReg(0x06);
	while (!(ucStatus&MASK_FIFO_STATUS_AFULL))  //fifo�Ƿ�����
	{
		LD_WriteReg(0x01,0xff);
		ucStatus = LD_ReadReg(0x06);
	}	
}

/*-------------------------------------------------*/
/*��������������ȡmp3�ļ����ݵ�fifo                */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void LD_ReloadMp3Data_Again(void)
{
	u8 val;
    u8 ucStatus;

	ucStatus = LD_ReadReg(0x06);
	while (!(ucStatus&MASK_FIFO_STATUS_AFULL) && nMp3Pos<=nMp3Size)	//fifo�Ƿ�����
	{
		nMp3Pos++;
		f_read(&MP3_file,&val,1,&Read_Bytes);	
		LD_WriteReg(0x01,val);
		ucStatus = LD_ReadReg(0x06);
	}

	if(nMp3Pos>=nMp3Size)
	{
	    LD_WriteReg(0xBC, 0x01);
		LD_WriteReg(0x29, 0x10);
		
		while(!(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END));  //�ȴ�MP3�������

		LD_WriteReg(0x2B,  0);
      	LD_WriteReg(0xBA, 0);	
		LD_WriteReg(0xBC,0x0);	
		bMp3Play=0;					// ��������ȫ����������޸�bMp3Play�ı���
		LD_WriteReg(0x08,1);

      	LD_WriteReg(0x08,0);
		LD_WriteReg(0x33, 0);

		f_close(&MP3_file);                                    //�ر��ļ�

	}		
}

/*-------------------------------------------------*/
/*���������жϴ�����                             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
u8 ucRegVal;   //�Ĵ������ݱ���
u8 ucHighInt;  //�Ĵ������ݱ���
u8 ucLowInt;   //�Ĵ������ݱ���

void ProcessInt_MP3(void)	 
{
	ucRegVal = LD_ReadReg(0x2B);

	// �������Ų������жϣ������֣�
	// A. ����������ȫ�������ꡣ
	// B. ���������ѷ�����ϡ�
	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	
	ucHighInt = LD_ReadReg(0x29); 
	ucLowInt=LD_ReadReg(0x02); 
	LD_WriteReg(0x29,0) ;
	LD_WriteReg(0x02,0) ;
	
	// A. ����������ȫ�������ꡣ
    if(LD_ReadReg(0xBA)&CAUSE_MP3_SONG_END)
    {
		LD_WriteReg(0x2B,  0);
      	LD_WriteReg(0xBA, 0);	
		LD_WriteReg(0xBC,0x0);	
		bMp3Play=0;					// ��������ȫ����������޸�bMp3Play�ı���
		LD_WriteReg(0x08,1);

      	LD_WriteReg(0x08,0);
		LD_WriteReg(0x33, 0);
		f_close(&MP3_file);         //�ر��ļ�
		return ;
     }
	
	// B. ���������ѷ�����ϡ�
	if(nMp3Pos>=nMp3Size)
	{	
		LD_WriteReg(0xBC, 0x01);
		LD_WriteReg(0x29, 0x10);
		f_close(&MP3_file);                                    //�ر��ļ�
		return;	
	}

	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	
	LD_ReloadMp3Data_Again();			
	LD_WriteReg(0x29,ucHighInt); 
	LD_WriteReg(0x02,ucLowInt) ;
}
