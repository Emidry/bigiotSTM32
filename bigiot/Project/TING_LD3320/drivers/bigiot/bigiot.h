#ifndef __BIGIOT_H
#define	__BIGIOT_H

/*���͵�¼��Ϣ*/
void checkin(char *DEVICEID,char *APIKEY);
/*�˳���¼*/
void checkout(char *DEVICEID,char *APIKEY);
/*��鵱ǰ��¼״̬*/
void check_status(void);
/*����ָ�Ŀ�굥λ*/
void say(char *toID, char *content);
/*�ϴ�һ���ӿڵ�ʵʱ����*/
void update1(char *did, char *inputid, float value);
#endif
