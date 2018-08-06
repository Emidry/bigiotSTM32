/*-------------------------------------------------*/
/*               ��γ���ӿ�����                    */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef __MP3_H
#define __MP3_H

#include "main.h"

extern u8  bMp3Play;				   //������¼����MP3��״̬  0������  1������

#define        RESUM_OF_MUSIC               0x01
#define        CAUSE_MP3_SONG_END           0x20

#define        MASK_INT_SYNC				0x10
#define        MASK_INT_FIFO				0x04
#define    	   MASK_AFIFO_INT				0x01
#define        MASK_FIFO_STATUS_AFULL		0x08

u8 PlayDemoSound_mp3(char *path);
void LD_Init_MP3(void);
void LD_AdjustMIX2SPVolume(u8 val);
void LD_play(void);
void fill_the_fifo(void);
void ProcessInt_MP3(void);

#endif
