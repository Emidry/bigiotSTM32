/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "main.h"

#define USART1_RXBUFF_SIZE  			1024  	  //�����������ֽ��� 256
	  	
extern u8  USART1_RX_BUF[USART1_RXBUFF_SIZE];     //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		          //����״̬���	

void Usart1_init(u32 bound);
void USART1_IRQHandler(void);
#endif


