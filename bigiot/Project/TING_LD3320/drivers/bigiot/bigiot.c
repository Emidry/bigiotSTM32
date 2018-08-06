#include "stm32f10x.h"
#include "usart2.h"
#include "bigiot.h"

/*���͵�¼��Ϣ*/
void checkin(char *DEVICEID,char *APIKEY)
{
    USART2_printf("{\"M\":\"checkin\",\"ID\":\"%s\",\"K\":\"%s\"}\n", DEVICEID, APIKEY);
}
/*�˳���¼*/
void checkout(char *DEVICEID,char *APIKEY)
{
    USART2_printf("{\"M\":\"checkout\",\"ID\":\"%s\",\"K\":\"%s\"}\n", DEVICEID, APIKEY);
}
/*��鵱ǰ��¼״̬*/
void check_status(void)
{
    USART2_printf("{\"M\":\"status\"}\n");
}
/*����ָ�Ŀ�굥λ*/
void say(char *toID, char *content)
{
    USART2_printf("{\"M\":\"say\",\"ID\":\"%s\",\"C\":\"%s\"}\n", toID, content);
}
/*�ϴ�һ���ӿڵ�ʵʱ����*/
void update1(char *did, char *inputid, float value) {
    USART2_printf("{\"M\":\"update\",\"ID\":\"%s\",\"V\":{\"%s\":\"%f\"}}\n", did, inputid, value);
}
