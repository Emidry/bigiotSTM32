/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __SPI2_H
#define __SPI2_H

#include "main.h"
				  	    													  
void SPI2_Init(void);			    //��ʼ��SPI2��
void SPI2_SetSpeed(u8 SpeedSet);    //����SPI2�ٶ�   
u8   SPI2_ReadWriteByte(u8 TxData); //SPI2���߶�дһ���ֽ�
		 
#endif

