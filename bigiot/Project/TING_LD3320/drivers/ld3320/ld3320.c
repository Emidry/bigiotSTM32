/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#include "ld3320.h"
#include "asr.h"
#include "spi2.h"
#include "delay.h"
#include "mp3.h"

u8 nLD_Mode = LD_MODE_IDLE;		//��ǰģʽ����ģʽ

/*-------------------------------------------------*/
/*��������ld3320��λ                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void LD_reset(void)
{
	LD_RST_H();
	delay_ms(20);
	LD_RST_L();
	delay_ms(20);
	LD_RST_H();
	delay_ms(20);
	LD_CS_L();
	delay_ms(20);
	LD_CS_H();		
	delay_ms(20);
}

/*-------------------------------------------------*/
/*����������ʼ������                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void LD_Init_Common(void)
{

	LD_ReadReg(0x06);  
	LD_WriteReg(0x17, 0x35); 
	delay_ms(100);
	LD_ReadReg(0x06);  

	LD_WriteReg(0x89, 0x03);  
	delay_ms(50);
	LD_WriteReg(0xCF, 0x43);   
	delay_ms(50);
	LD_WriteReg(0xCB, 0x02);
	
	LD_WriteReg(0x11, LD_PLL_11);  
	
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD_WriteReg(0x1E, 0x00); 
		LD_WriteReg(0x19, LD_PLL_MP3_19);   
		LD_WriteReg(0x1B, LD_PLL_MP3_1B);   
		LD_WriteReg(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD_WriteReg(0x1E,0x00);
		LD_WriteReg(0x19, LD_PLL_ASR_19); 
		LD_WriteReg(0x1B, LD_PLL_ASR_1B);		
	    LD_WriteReg(0x1D, LD_PLL_ASR_1D);
	}
	delay_ms(100);
	
	LD_WriteReg(0xCD, 0x04);
	LD_WriteReg(0x17, 0x4c); 
	delay_ms(50);
	LD_WriteReg(0xB9, 0x00);
	LD_WriteReg(0xCF, 0x4F); 
	LD_WriteReg(0x6F, 0xFF); 
}

/*-------------------------------------------------*/
/*��������ld3320��ʼ��                             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void LD3320_Init(void)
{
	CLK_Configuration();
	LD3320_GPIO_init();	
	LD3320_EXTI_Init();
	SPI2_Init();
	LD_reset();
}
/*-------------------------------------------------*/
/*��������ld3320��IO��ʼ��                         */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/ 
void LD3320_GPIO_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	PBout(6)=1;	   //A0Ĭ������
	PBout(7)=0;	   //RDĬ������

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/*-------------------------------------------------*/
/*��������ld3320�жϳ�ʼ��                         */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LD3320_EXTI_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��	

	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);

	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger =EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_GenerateSWInterrupt(EXTI_Line9);		
	GPIO_SetBits(GPIOB,GPIO_Pin_9);  	    //Ĭ�������ж�����
	EXTI_ClearFlag(EXTI_Line9);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*-------------------------------------------------*/
/*��������SPI�ӿ��շ�����                          */
/*��  ����byte�����͵�����                         */
/*����ֵ�����յ�����                               */
/*-------------------------------------------------*/
u8 spi_send_byte(u8 byte)
{
	return SPI2_ReadWriteByte(byte);
}

/*-------------------------------------------------*/
/*��������дLD3320�Ĵ���                           */
/*��  ����data1���Ĵ���                            */
/*��  ����data2������                              */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void LD_WriteReg(u8 data1,u8 data2)
{
	LD_CS_L();

	LD_SPIS_L();

	spi_send_byte(0x04);

	spi_send_byte(data1);

	spi_send_byte(data2);

	LD_CS_H();

}

/*-------------------------------------------------*/
/*����������LD3320�Ĵ���                           */
/*��  ����reg_add���Ĵ���                          */
/*����ֵ������������                               */
/*-------------------------------------------------*/
u8 LD_ReadReg(u8 reg_add)
{
	u8 i;

	LD_CS_L();

	LD_SPIS_L();

	spi_send_byte(0x05);

	spi_send_byte(reg_add);

	i=spi_send_byte(0x00);	/*��SPI*/

	LD_CS_H();

	return(i);
}

/*-------------------------------------------------*/
/*��������ʱ������                                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CLK_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //���ļ����ڳ�ʼ��GPIO�õ��Ľṹ��

	//����PA8 �����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_MCOConfig(RCC_MCO_HSE);//ѡ��MCO��ʱ��ԴΪ�ⲿ����ʱ�� 8M
}
/*-------------------------------------------------*/
/*��������PB9 LD3320�жϺ���                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line9)!= RESET ) 
	{
 		if (nLD_Mode == LD_MODE_ASR) ProcessInt_ASR();
		else ProcessInt_MP3(); 
		EXTI_ClearFlag(EXTI_Line9);
	}
}

