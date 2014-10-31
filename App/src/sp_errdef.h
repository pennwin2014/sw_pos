#ifndef __SP_ERRDEF_H__
#define __SP_ERRDEF_H__


#define COMMOM_ERROR "普通错误"
#define TYPE_ERROR "输入错误"
#define LEN_ERROR "长度错误"


#define SP_SUCCESS 0x00
#define SP_FLASH_FAIL 0x01
#define SP_FLASH_NOT_FOUNT 0x02 //flash未找到
#define SP_FLASH_READ_ERROR 0x03 //flash读取失败
#define SP_FLASH_CRC_ERROR 0x04 //flash校验失败
#define SP_FLASH_WRITE_ERROR 0x05 //flash写失败
#define SP_FLASH_NOT_EQUAL 0x06 //主从流水号不一致
#define SP_FLASH_ERASE_ERROR 0x07  //flash擦除失败
#define SP_FLASH_TRANSDTL_FULL 0x08 //流水存储已满
#define SP_RETURN_FUNC 0x09  //按了功能键返回
#define SP_RETURN_BLACKCARD_CONSUME 0x0A  //黑卡消费
#define SP_CARD_STATUS_ERROR 0x0B  //卡状态有误
#define SP_INSUFFICIENT_BALANCE 0x0C  //余额不足
#define SP_RETURN_BY_CLEAR 0x0D //按了清除键返回
#define SP_MIN_CARD_BALANCE 0x0E //卡片余额小于最小卡余额
#define SP_CARD_PASSWD_ERROR 0x0F //卡密码输入错误
#define SP_CARD_PASSWD_CANCEL 0x10 //取消卡密码输入
#define SP_WRITE_CARD_ERROR 0x11 //写卡出错
#define SP_CALC_MAC_ERROR 0x12 //计算mac出错
#define SP_NOT_SAME_CARD 0x13 //不是同一张卡
#define SP_NO_NUMBER_KEY_PRESSED 0x14 //没有输入金额
#define SP_CANCEL_CONSUME 0x15 //取消消费
#define SP_CARD_NOT_FOUND 0x16 //寻卡失败
#define SP_NOT_GET_PROVE 0x17 //没有扣掉卡上余额
#define SP_HALFWAY_PULL_CARD 0x18 //中途拔卡
#define SP_BUFFER_NO_DATA 0x19 //接收缓冲区没数据
#define SP_BUFFER_CRC_ERROR 0x1A //接收缓冲区crc错误
#define SP_WAIT_TIMEOUT 0x1B //等待响应超时
#define SP_SEQNO_NOT_MATCH 0x1C //接收到过大的顺序号
#define SP_CMD_NOT_MATCH 0x1D //接收到数据命令码不一致
#define SP_RECV_ERROR 0x1E //前置机收到数据后返回错误，暂时只定一个功能号
#define SP_DATE_REVERSE 0x1F //日期倒退
#define SP_DATE_CHANGE 0x20 //日期变化
/////////////////////////////////////////////////////////////
#define E_INSUFFICIENT_BALANCE 0x9401




#endif


