/**
  ******************************************************************************
  * @file    main.c
  * @author  www.bigiot.net
  * @version V0.1
  * @date    2017-6-9
  * @brief   STM32F103C8T6_ESP01_LED���豸��¼Ϊ������ģʽ��
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "sd.h"
#include "ff.h"
#include "ld3320.h"
#include "mp3.h"
#include "led.h"
#include "asr.h"
#include "cJSON.h"
#include "millis.h"
#include "bigiot.h"

//FATFS fs[_VOLUMES];

FATFS fs;
//FIL config_file2;

//char buf[2048];

unsigned long lastCheckStatusTime = 0;
unsigned long lastCheckOutTime = 0;
unsigned long lastSayTime = 0;
unsigned long lastWakeTime = 0;
const unsigned long postingInterval = 40000;
const unsigned long statusInterval = 60000;
bigiot_state BGT_STA = OFF_LINE;
unsigned int waked = 0;
/*�����������������޸�Ϊ�Լ����豸ID��APIKEY���ڱ��������û���������豸���ɻ��*/
char *DEVICEID = "458";
char *APIKEY = "9cb787949";

/*��CJSON������յ�����Ϣ*/
int processMessage(char *msg) {
    cJSON *jsonObj = cJSON_Parse(msg);
    cJSON *method;
    char *m;
    //json�ַ�������ʧ�ܣ�ֱ���˳�
    printf("������գ�%s", msg);
    if(!jsonObj)
    {
        //uart1.USART2_printf("json string wrong!");
        return 0;
    }
    method = cJSON_GetObjectItem(jsonObj, "M");
    m = method->valuestring;
    if(strncmp(m, "WELCOME", 7) == 0)
    {
        BGT_STA = CONNECTED;
        //��ֹ�豸����״̬δ�������ȵǳ�
        checkout(DEVICEID, APIKEY);
        lastCheckOutTime = millis();
    }
    if(strncmp(m, "connected", 9) == 0)
    {
        BGT_STA = CONNECTED;
        checkin(DEVICEID, APIKEY);
    }
    //{"M":"checkout","IP":"xx1","T":"xx2"}\n
    if(strncmp(m, "checkout", 8) == 0)
    {
        BGT_STA = FORCED_OFF_LINE;
    }
    //{"M":"checkinok","ID":"xx1","NAME":"xx2","T":"xx3"}\n
    if(strncmp(m, "checkinok", 9) == 0)
    {
        BGT_STA = CHECKED;
    }
    if(strncmp(m, "checked", 7) == 0)
    {
        BGT_STA = CHECKED;
    }
    //���豸���û���¼�����ͻ�ӭ��Ϣ
    if(strncmp(m, "login", 5) == 0)
    {
        char *from_id = cJSON_GetObjectItem(jsonObj, "ID")->valuestring;
        char new_content[] = "Dear friend, welcome to BIGIOT !";
        say(from_id, new_content);
    }
    //�յ�sayָ�ִ����Ӧ��������������Ӧ�ظ�
    if(strncmp(m, "say", 3) == 0 && millis() - lastSayTime > 10)
    {
        char *content = cJSON_GetObjectItem(jsonObj, "C")->valuestring;
        char *from_id = cJSON_GetObjectItem(jsonObj, "ID")->valuestring;
        lastSayTime = millis();
        if(strncmp(content, "play", 4) == 0)
        {
            char new_content[] = "led played";
            //do something here....
            LED_D3 = 0;
            say(from_id, new_content);
        }
        else if(strncmp(content, "stop", 4) == 0)
        {
            char new_content[] = "led stoped";
            //do something here....
            LED_D3 = 1;
            say(from_id, new_content);
        }
    }
    if(jsonObj)cJSON_Delete(jsonObj);
    return 1;
}


void setup(void)
{
    delay_init();//��ʱ��ʼ��
    USART1_init(115200);
    USART2_init(115200);
    LED_Init(); 	                   //LED��ʼ��
    while(SD_Init())                   //SD����ʼ��
    {
        printf("SD����ʼ������\r\n");  //������ʾSD����ʼ������
        delay_ms(2000);                //��ʱ2s
    }
    f_mount(&fs, "0:", 1); 	     //����SD��
    LD3320_Init();	                   //��ʼ��LD3320
    MILLIS_Init();
}

u8 write_file(char *path)
{
    u8 res;
    UINT br;
    FIL file;
    char wakeup[] = "[{\"N\":1,\"C\":\"xiao bei xiao bei\"},{\"N\":2,\"C\":\"bei ke wu lian\"},{\"N\":3,\"C\":\"xiao zhu xiao zhu\"},{\"N\":4,\"C\":\"bei ke kei kei\"}]";
    res = f_open (&file, path, FA_OPEN_ALWAYS | FA_WRITE);			//���ļ�,���������½�.
    if(res != FR_OK)
    {
        printf("��ʧ��:%s\r\n", path);                           //������ʾ��Ϣ
        f_close(&file);                                    //�ر��ļ�
        return res;
    }
    //res=f_lseek(&file,f_size(&file)); 										    //�ƶ���дָ��(�ƶ����ļ��Ľ�β�������)
    res = f_lseek(&file, 0);                                                         //�ƶ���дָ��(�ƶ����ļ��Ŀ�ͷ)
    f_write (&file, wakeup, sizeof(wakeup) - 1, &br);	//д������
    f_close(&file);		//�ر��ļ�
    printf("write file OK!%d\r\n", res);
    return res;
}
u8 write_file2(char *path)
{
    u8 res;
    UINT br;
    FIL file;
    char wakeup[] = "{\"C0\":{\"ID\":\"6589\",\"K\":\"658bb0ee3\",\"BR\":9600,\"CI\":10,\"CC\":1,\"SPK\":1},\"C1\":{\"M\":\"WAKE\",\"W\":\"wake\"},\"C2\":{\"M\":\"LED\",\"V\":1,\"W\":\"ledon\"}}";
    res = f_open (&file, path, FA_OPEN_ALWAYS | FA_WRITE);			//���ļ�,���������½�.
    if(res != FR_OK)
    {
        printf("��ʧ��:%s\r\n", path);                           //������ʾ��Ϣ
        f_close(&file);                                    //�ر��ļ�
        return res;
    }
    //res=f_lseek(&file,f_size(&file)); 										    //�ƶ���дָ��(�ƶ����ļ��Ľ�β�������)
    res = f_lseek(&file, 0);                                                         //�ƶ���дָ��(�ƶ����ļ��Ŀ�ͷ)
    f_write (&file, wakeup, sizeof(wakeup) - 1, &br);	//д������
    f_close(&file);		//�ر��ļ�
    printf("write file2 OK!%d\r\n", res);
    return res;
}

void voice_command(u8 num,char * path)
{
    FIL file;
    UINT br;
    u8 res;
    char config_buf[2048];
    cJSON *root;
    cJSON *C_n;
    cJSON *C_n_m;
    char *c_n_str;
    res=f_open(&file,path,FA_OPEN_EXISTING | FA_READ );    //��һ��mp3�ļ�
	if(res != FR_OK)                                           //���������
	{
		printf("2���ļ�������:%d\r\n",res);                            //������ʾ��Ϣ
		f_close(&file);                                    //�ر��ļ�
		return;		                                       //���ش���ֵ
        //TODO...
	}
    f_read (&file, config_buf,f_size(&file),&br);			//��ȡ�ļ���buf
    f_close(&file);	
    printf("file:%s\r\n",config_buf);
    root = cJSON_Parse(config_buf);
    //printf("%s\r\n", cJSON_Print(root));
    sprintf(c_n_str,"%s%d","C",num);
    printf("c_n_str:%s\r\n",c_n_str);
    C_n = cJSON_GetObjectItem(root, "C1");
    C_n_m = cJSON_GetObjectItem(C_n, "M");
    printf("c_n_m:%s\r\n",C_n_m->valuestring);
    if(strncmp(C_n_m->valuestring, "WAKE", 4) == 0)
    {
        printf("waked\r\n");
    }
    if(strncmp(C_n_m->valuestring, "LED", 3) == 0)
    {
        printf("LEDed\r\n");
    }
    if(root)cJSON_Delete(root);
    return;
}
int main(void)
{
    u8 nAsrRes = 0;
    u16 len;
    setup();
    write_file("asr.txt");
    write_file2("config.txt");
    //read_wakeup_file("0:/message.txt");
    printf(" ����1������ϵͳ\r\n ");
    printf(" ����2������Ц��\r\n ");
    printf(" ����3����\r\n ");
    printf(" ����4���ر�\r\n ");
    nAsrStatus = LD_ASR_NONE;		     //��ʼ״̬��û������ASR
    PlayDemoSound_mp3("ϵͳ׼��.mp3");   //�����ļ�
    //while(bMp3Play == 0);                //�������ݷ������
    //delay_ms(5000);

    while (1)
    {
        if ( BGT_STA == CONNECTED && lastCheckOutTime != 0 && millis() - lastCheckOutTime > 200) {
            checkin(DEVICEID, APIKEY);
            lastCheckOutTime = 0;
        }
        if (millis() - lastCheckStatusTime > statusInterval || (lastCheckStatusTime == 0 && millis() > 5000)) {
            check_status();
            lastCheckStatusTime = millis();
        }

        if(USART2_RX_STA & 0x8000)
        {
            len = USART2_RX_STA & 0x3fff;                   //�õ��˴ν��յ������ݳ���
            printf("�յ����ȣ�%d\r\n", len);
            USART2_RX_BUF[len] = '\0';
            //printf("usart2 �յ����ݣ�%s\r\n",USART2_RX_BUF);
            processMessage((char*)USART2_RX_BUF);
            USART2_RX_STA = 0;
        }
        //printf("bmp3play\r\n");
        if(bMp3Play == 1)continue;
        if (millis() - lastWakeTime > 11000 && lastWakeTime != 0 )
        {
            waked = 0;
            lastWakeTime = 0;
            Stop_ASR();
        }
        switch(nAsrStatus)
        {
        case LD_ASR_RUNING:
            break;

        case LD_ASR_ERROR:
            printf("LD3320оƬ�ڲ����ֲ���ȷ��\r\n");
            break;

        case LD_ASR_NONE:
            nAsrStatus = LD_ASR_RUNING;            //����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
            if (RunASR(0xff, "asr.txt") == 0)
            {
                printf("ASR_ERROR\r\n");
                nAsrStatus = LD_ASR_ERROR;
            }
            break;

        case LD_ASR_FOUNDOK:
            nAsrRes = LD_ReadReg(0xc5);	            //һ��ASRʶ��ɹ�������ȡASRʶ����
            voice_command(nAsrRes,"config.txt");
            if(nAsrRes == 1)
            {
                waked = 1;
                lastWakeTime = millis();
                printf("���յ��������ϵͳ\r\n");
                nAsrStatus = LD_ASR_NONE;
                break;
            }
            if(waked != 1)
            {
                printf("δ���ѡ�\r\n");
                nAsrStatus = LD_ASR_NONE;
                break;
            }
            if(nAsrRes == 2)
            {
                printf("���յ��������Ц��\r\n");
            }
            if(nAsrRes == 3)
            {
                printf("���յ������\r\n");
            }
            if(nAsrRes == 4)
            {
                printf("���յ�����ر�\r\n");
            }
            
            printf("���ڿ��֮��\r\n");
            nAsrStatus = LD_ASR_NONE;
            break;

        case LD_ASR_FOUNDZERO:
            printf("δ֪����\r\n");
            nAsrStatus = LD_ASR_NONE;
            break;

        default:
            nAsrStatus = LD_ASR_NONE;
            break;
        }
    }
}

