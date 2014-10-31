#ifndef __SP_ERRDEF_H__
#define __SP_ERRDEF_H__


#define COMMOM_ERROR "��ͨ����"
#define TYPE_ERROR "�������"
#define LEN_ERROR "���ȴ���"


#define SP_SUCCESS 0x00
#define SP_FLASH_FAIL 0x01
#define SP_FLASH_NOT_FOUNT 0x02 //flashδ�ҵ�
#define SP_FLASH_READ_ERROR 0x03 //flash��ȡʧ��
#define SP_FLASH_CRC_ERROR 0x04 //flashУ��ʧ��
#define SP_FLASH_WRITE_ERROR 0x05 //flashдʧ��
#define SP_FLASH_NOT_EQUAL 0x06 //������ˮ�Ų�һ��
#define SP_FLASH_ERASE_ERROR 0x07  //flash����ʧ��
#define SP_FLASH_TRANSDTL_FULL 0x08 //��ˮ�洢����
#define SP_RETURN_FUNC 0x09  //���˹��ܼ�����
#define SP_RETURN_BLACKCARD_CONSUME 0x0A  //�ڿ�����
#define SP_CARD_STATUS_ERROR 0x0B  //��״̬����
#define SP_INSUFFICIENT_BALANCE 0x0C  //����
#define SP_RETURN_BY_CLEAR 0x0D //�������������
#define SP_MIN_CARD_BALANCE 0x0E //��Ƭ���С����С�����
#define SP_CARD_PASSWD_ERROR 0x0F //�������������
#define SP_CARD_PASSWD_CANCEL 0x10 //ȡ������������
#define SP_WRITE_CARD_ERROR 0x11 //д������
#define SP_CALC_MAC_ERROR 0x12 //����mac����
#define SP_NOT_SAME_CARD 0x13 //����ͬһ�ſ�
#define SP_NO_NUMBER_KEY_PRESSED 0x14 //û��������
#define SP_CANCEL_CONSUME 0x15 //ȡ������
#define SP_CARD_NOT_FOUND 0x16 //Ѱ��ʧ��
#define SP_NOT_GET_PROVE 0x17 //û�п۵��������
#define SP_HALFWAY_PULL_CARD 0x18 //��;�ο�
#define SP_BUFFER_NO_DATA 0x19 //���ջ�����û����
#define SP_BUFFER_CRC_ERROR 0x1A //���ջ�����crc����
#define SP_WAIT_TIMEOUT 0x1B //�ȴ���Ӧ��ʱ
#define SP_SEQNO_NOT_MATCH 0x1C //���յ������˳���
#define SP_CMD_NOT_MATCH 0x1D //���յ����������벻һ��
#define SP_RECV_ERROR 0x1E //ǰ�û��յ����ݺ󷵻ش�����ʱֻ��һ�����ܺ�
#define SP_DATE_REVERSE 0x1F //���ڵ���
#define SP_DATE_CHANGE 0x20 //���ڱ仯
/////////////////////////////////////////////////////////////
#define E_INSUFFICIENT_BALANCE 0x9401




#endif


