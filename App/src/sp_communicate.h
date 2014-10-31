#ifndef __SP_CAN_H__
#define __SP_CAN_H__

#include "config.h"
#include "sp_info.h"
#include "sp_pubfunc.h"

#define SP_CMD_AUTH 0x02
#define SP_CMD_RT_TRANSDTL 0x01







#ifdef SP_GLOBALS
#define SP_EXT
#else
#define SP_EXT extern
#endif

typedef struct
{
  uint8 cmd;
  int (* prepare_req_data)(sp_context* , uint8* , uint16 , uint16* , void*);
  int (* resp_process_handler)(sp_context*, uint8* , uint16);
} protocol_process_handler, *pprotocol_process_handler;

typedef struct
{
  //接收缓冲区
  uint8 recv_buffer[TCP_RECV_BUFFER_SIZE];
  //接收缓冲区长度
  uint16 recv_len;
  //当前网络请求流水号
  uint32 current_tcp_seqno;
  //当前的请求
  sp_tcp_trans_unit current_response_unt;
  //将新的响应数据插入
  void (* insert_response)(uint8* , uint16);
  //等待响应数据
  int (* wait_response)(uint32, sp_tcp_trans_unit*);
  //解析接收到的请求
  void (* parse_response)(uint8*);
  //根据命令查找对应的处理结构体
  pprotocol_process_handler(*find_handler)(uint8);
} sp_com_buffer_handler;

//全局的网络数据处理器
SP_EXT sp_com_buffer_handler global_tcp_handler;


/////////////////////发送网络请求函数/////////////////////////////
//int sp_send_request(sp_context* ctx, uint8 cmd);

uint8 sp_send_struct(const sp_tcp_trans_unit* pstruct);

void sp_init_com(void);

int sp_prcocess_message(void);



#endif
