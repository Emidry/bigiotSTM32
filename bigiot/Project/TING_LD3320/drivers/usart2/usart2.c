#include <stdarg.h>
#include "usart2.h"

u8  USART2_RX_BUF[USART2_RXBUFF_SIZE]; //���ջ���
u16 USART2_RX_STA=0;                   //����״̬���	  

void USART2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //?????,4??????,4??????;
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //???;
    //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //?????;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4; //?????;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //USART2 TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //??????;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //??A;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //USART2 RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //????;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //??A;

    USART_InitStructure.USART_BaudRate = bound; //???;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //???8?;
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //???1?;
    USART_InitStructure.USART_Parity = USART_Parity_No ; //????;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);//??????;

    //USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE); //????;
}

/*����һ���ֽ�����*/
void UART2SendByte(unsigned char SendData)
{
    USART_SendData(USART2, SendData);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/*����һ���ֽ�����*/
unsigned char UART2GetByte(unsigned char* GetData)
{
    if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
        return 0;//û���յ�����
    }
    *GetData = USART_ReceiveData(USART2);
    return 1;//�յ�����
}
/*����һ�����ݣ����Ϸ��ؽ��յ����������*/
void UART2Test(void)
{
    unsigned char i = 0;

    while(1)
    {
        while(UART2GetByte(&i))
        {
            USART_SendData(USART2, i);
        }
    }
}

/*-------------------------------------------------*/
/*������������2�жϷ�����                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void USART2_IRQHandler(void)   //����1�жϷ������������\r\n��Ϊ������־
{
    u8 Res;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //��������ж�
    {
        Res = USART_ReceiveData(USART2); //��ȡ���յ�������
        if((USART2_RX_STA & 0x8000) == 0) //==0������û��ɣ�λ15������λ��񣬴�������Ƿ����
        {
            if(Res == '\n')         //������յ�\n�����һ��\r\n���յ��ˣ���ʾ����
            {
                USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res; //��¼��\n����
                USART2_RX_STA++;	                     //�Ǳ�+1
                USART2_RX_STA |= 1 << 15;	             //λ15��λ�����������
            }
            else         //�������\n
            {
                USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = Res; //��¼������
                USART2_RX_STA++;	                     //����Ǳ�+1
            }
        }
    }
}

void  USART2_printf(char *fmt, ...)
{
    char buffer[USART2_RXBUFF_SIZE - 1];
    u16 i = 0;
    u8 len;

    va_list arg_ptr; //Define convert parameters variable
    va_start(arg_ptr, fmt); //Init variable
    len = vsnprintf(buffer, USART2_RXBUFF_SIZE+1, fmt, arg_ptr); //parameters list format to buffer
    
    while ((i < USART2_RXBUFF_SIZE) && (i < len) && (len > 0))
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2, (u8) buffer[i++]);
    }
    va_end(arg_ptr);
}

