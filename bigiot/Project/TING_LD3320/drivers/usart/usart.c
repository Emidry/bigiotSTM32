/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#include "usart.h"	  
	
u8  USART1_RX_BUF[USART1_RXBUFF_SIZE]; //���ջ���
u16 USART1_RX_STA=0;                   //����״̬���	  

/*-------------------------------------------------*/
/*����������ʼ������1                              */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void Usart1_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//ʹ��USART1
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOA	
	USART_DeInit(USART1);                                   //��λ����1
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ��PA9
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  //��ʼ��PA10

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;       //����1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                        //����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	USART_InitStructure.USART_BaudRate = bound;                //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure);     //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
}

/*�������´���,֧��printf����,������Ҫѡ��use MicroLIB*/	  
#if 1
#pragma import(__use_no_semihosting)                             
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
_sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
/*-------------------------------------------------*/
/*������������1�жϷ�����                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART1_IRQHandler(void)   //����1�жϷ������������\r\n��Ϊ������־
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //��������ж�
	{
		Res =USART_ReceiveData(USART1); //��ȡ���յ�������		
		if((USART1_RX_STA&0x8000)==0)    //==0������û��ɣ�λ15������λ��񣬴�������Ƿ����
		{			
			if(USART1_RX_STA&0x4000)     //λ14��λ�������յ���\r
			{
				if(Res=='\n')           //������յ�\n�����һ��\r\n���յ��ˣ���ʾ����
				{				
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //��¼��\n����
					USART1_RX_STA++;	                     //�Ǳ�+1
					USART1_RX_STA|=1<<15;	                 //λ15��λ����������� 
				}
				else 
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //��¼������
					USART1_RX_STA++;	                        //�Ǳ�+1
					USART1_RX_STA&=~(1<<14);                    //���λ14�����յ���\r���Ǵ������������������	
				}
			}
			else
			{
				if(Res=='\r') //������յ���\r
				{
					USART1_RX_STA|=(1<<14);                  //λ14��λ�������յ���\r 
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //��¼��\r
					USART1_RX_STA++;	                     //����Ǳ�+1
				}
				else         //�������\r
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res; //��¼������
					USART1_RX_STA++;	                     //����Ǳ�+1
				}				
			}
		} 
  } 
} 

