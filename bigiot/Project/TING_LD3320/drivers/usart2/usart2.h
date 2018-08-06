#ifndef __USART2_H
#define	__USART2_H

#include "stdio.h"	
#include "main.h"

#define USART2_RXBUFF_SIZE  			2048  	  //�����������ֽ��� 256
	  	
extern u8  USART2_RX_BUF[USART2_RXBUFF_SIZE];     //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		          //����״̬���	

void USART2_init(u32 bound);
void UART2Test(void);
void UART2SendByte(unsigned char SendData);
unsigned char UART2GetByte(unsigned char* GetData);
void USART2_IRQHandler(void);
void USART2_printf(char *fmt, ...);

#endif
