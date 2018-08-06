/*-------------------------------------------------*/
/*         ��γ����STM32F407ZGT6������             */
/*    �Ա���ַ�� http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/
/*-------------------------------------------------*/
/*            FAT�ļ�ϵͳ��ֲ�Ĺؼ��ļ�            */
/*        ʵ���ļ�ϵͳ��Ӳ�����̵����Ӻ���         */
/*-------------------------------------------------*/

#include "diskio.h"		
#include "sd.h"
#include "mymalloc.h"		

#define SD_CARD	 0                  //SD��,���Ϊ0
#define FLASH_SECTOR_SIZE 	512	    //������С		  
#define FLASH_BLOCK_SIZE   	8     	//ÿ��BLOCK��8������

/*-------------------------------------------------*/
/*����������ʼ�����̽ӿں���                       */
/*��  ����pdrv�����̱��                           */
/*����ֵ������״ֵ̬                               */
/*-------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv)
{
	u8 res=0;	    
	switch(pdrv)          //�ж��ĸ�����
	{
		case SD_CARD:   res=SD_Init();//SD����ʼ��
						if(res)       
						{
							SD_SPI_SpeedLow();
							SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
							SD_SPI_SpeedHigh();
						}
						break;		
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;  //��ʼ��ʧ�ܷ���STA_NOINIT��1
	else return 0;              //��ʼ���ɹ�����0
}

/*-------------------------------------------------*/
/*����������ô���״̬�ӿں���                     */
/*��  ����pdrv�����̱��                           */
/*����ֵ������״ֵ̬                               */
/*-------------------------------------------------*/
DSTATUS disk_status (BYTE pdrv)
{ 
	return 0;   //ֱ�ӷ���0����ʾ�ɹ���û�о���Ĳ���
} 

/*-------------------------------------------------*/
/*���������������ӿں���                           */
/*��  ����pdrv�����̱��                           */
/*��  ����buff: ���յ�ַ                           */
/*��  ����count:��Ҫ��ȡ��������                   */
/*����ֵ������״ֵ̬                               */
/*-------------------------------------------------*/
DRESULT disk_read (BYTE pdrv,BYTE *buff,DWORD sector,UINT count)
{
	u8 res=0; 
    if (!count)return RES_PARERR;     //��Ҫ��ȡ�����������ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)                      //�ж��ĸ�����
	{
		case SD_CARD:   res=SD_ReadDisk(buff,sector,count);	 
						if(res)
						{
							SD_SPI_SpeedLow();
							SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
							SD_SPI_SpeedHigh();
						}
						break;
		default:
			res=1; 
	}
    if(res==0x00)return RES_OK;	   //��ȷ����RES_OK
    else return RES_ERROR;	       //���󷵻�RES_ERROR
}

/*-------------------------------------------------*/
/*��������д�����ӿں���                           */
/*��  ����pdrv�����̱��                           */
/*��  ����buff: д�뻺����                         */
/*��  ����count:��Ҫд���������                   */
/*����ֵ������״ֵ̬                               */
/*-------------------------------------------------*/

#if _USE_WRITE   //������Ȩ�ޣ�"diskio.h"������

DRESULT disk_write (BYTE pdrv,const BYTE *buff,DWORD sector,UINT count)
{
	u8 res=0;  
    if (!count)return RES_PARERR;         //��Ҫд������������ܵ���0�����򷵻ز�������		 	 
	switch(pdrv)                                        //�ж��ĸ�����
	{ 
		case SD_CARD:    res=SD_WriteDisk((u8*)buff,sector,count);   //д���� 
						 break;
		default:
			res=1; 
	}
    if(res==0x00)return RES_OK;	   //��ȷ����RES_OK
    else return RES_ERROR;	       //���󷵻�RES_ERROR	
}
#endif

/*-------------------------------------------------*/
/*��������ָ�����                                 */
/*��  ����pdrv�����̱��                           */
/*��  ����ctrl:���ƴ���                            */
/*��  ����buff:����/���ջ�����ָ��                 */
/*����ֵ������״ֵ̬                               */
/*-------------------------------------------------*/

#if _USE_IOCTL     //��������Ȩ�ޣ�"diskio.h"������

DRESULT disk_ioctl (BYTE pdrv,BYTE cmd,void *buff)
{
	DRESULT res;						  			     
	if(pdrv==SD_CARD) //�ж���SD��
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				SD_CS=0;
		        if(SD_WaitReady()==0)res = RES_OK; 
		        else res = RES_ERROR;	  
				SD_CS=1;
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
	else res=RES_ERROR;//�����Ĳ�֧��
    return res;
}
#endif

/*-------------------------------------------------*/
/*�����������ʱ��                                 */
/*��  ������                                       */
/*����ֵ��                                         */
/*-------------------------------------------------*/                                                                                                                                                                                                                                               
DWORD get_fattime (void)
{				 
	return 0;   //ûʵ���������
}

/*-------------------------------------------------*/
/*����������̬�����ڴ�                             */
/*��  ����size����Ҫ�Ŀռ��С                     */
/*����ֵ��������ڴ�ָ��                           */
/*-------------------------------------------------*/    
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(size);
}

/*-------------------------------------------------*/
/*���������ͷ��ڴ�                                 */
/*��  ������Ҫ�ͷŵ��ڴ��ָ��                     */
/*����ֵ����                                       */
/*-------------------------------------------------*/  
void ff_memfree (void* mf)		 
{
	myfree(mf);
}

















