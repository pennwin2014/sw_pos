#define SP_GLOBALS

#include "sp_communicate.h"
//////////////////////////////���ߺ���/////////////////////////////////////
static uint8 do_com_sendbuf(uint8 uComm, uint8 *pcSendBuf, int iLen)
{
  return (0 == COM_SendBuf(uComm, pcSendBuf, iLen)) ? 1 : 0;
}
	  /*
static uint16 get_data_len(void * data_len_buffer)
{
  uint16 data_len = 0;
  memcpy(&data_len, data_len_buffer, 2);
  return data_len;
}
	
static int compare_tcp_checksum(uint8* data, uint16 data_len)
{
  return (data[data_len - 1] == sp_calc_crc8(data, data_len - 1)) ? 0 : 1;
}
  */
///////////////////////////////������////////////////////////////////////////////////

static int do_send_auth(sp_context* ctx, uint8* send_buffer, uint16 buffer_max_len, uint16* data_len, void* arg)
{
  return 0;
}
static int do_recv_auth(sp_context* ctx, uint8* buffer, uint16 data_len)
{
//  if(SP_CMD_AUTH != tcp_unt->header.cmd_code)
 //   return SP_CMD_NOT_MATCH;
  return 0;
}

static int do_send_transdtl(sp_context* ctx, uint8* send_buffer, uint16 buffer_max_len, uint16* data_len, void* arg)
{
//	sp_tcp_trans_unit tcp_unt;
//	tcp_unt.header.cmd_code = SP_CMD_RT_TRANSDTL;
	//memcpy(tcp_unt.header.guide_code, "";
  return 0;
}

static int do_recv_transdtl(sp_context* ctx, uint8* buffer, uint16 data_len)
{
/*
  if(SP_CMD_RT_TRANSDTL != tcp_unt->header.cmd_code)
    return SP_CMD_NOT_MATCH;
  if(tcp_unt->data[0] == 0)
    return 0;
  else
    return SP_RECV_ERROR;
	*/
	return 0;
}

//��Ÿ������ʹ���Ľӿ�
static protocol_process_handler global_process_handler[] =
{
  {0x01, do_send_transdtl, do_recv_transdtl},
  {0x02, do_send_auth, do_recv_auth}
};

////////////////////////////////global_tcp_hander�Ľӿں���//////////////////////////////////////////////
static pprotocol_process_handler do_find_handler(uint8 cmd)
{
  int  i = 0;
  for(i = 0; i < 2; i++)
  {
    if(global_process_handler[i].cmd == cmd)
    {
      return &global_process_handler[i];
    }
  }
  return NULL;
}

static int do_wait_response(uint32 seqno, sp_tcp_trans_unit* tcp_unt)
{
return 0;
/*
  uint32 cnt = TIMEOUT_CNT;
  //ÿ�ζ��ǽ��յ�һ���ṹ��
  while(1)
  {
    if(cnt-- <= 0)
      break;
    if(global_tcp_handler.current_response_unt.seqno > seqno)
      return SP_SEQNO_NOT_MATCH;
    if(global_tcp_handler.current_response_unt.seqno == seqno)
    {
      memcpy(tcp_unt, &global_tcp_handler.current_response_unt, sizeof(sp_tcp_trans_unit));
      return 0;
    }
    sp_sleep(10);
  }
  return SP_WAIT_TIMEOUT;
  */
}


static void do_parse_response(uint8* cmd)
{
/*
  uint16 data_len = 0;
  uint8 checksum = 0;
  uint16 offset = 0;
  data_len = get_data_len(global_tcp_handler.recv_buffer);
  if(global_tcp_handler.recv_len - 2 >= data_len)
  {
    //crcһ�¾͸��ǵ�ǰ�Ľ��սṹ��
    if(!compare_tcp_checksum(global_tcp_handler.recv_buffer + 2, data_len))
    {
      global_tcp_handler.recv_len -= 2;
      //��bufferΪ�ṹ�帳ֵ
      offset = 2;
      memcpy(&global_tcp_handler.current_response_unt.header, global_tcp_handler.recv_buffer + offset, sizeof(sp_tcp_header));
      offset += sizeof(sp_tcp_header);
      memcpy(global_tcp_handler.current_response_unt.data, global_tcp_handler.recv_buffer + offset, global_tcp_handler.current_response_unt.header.data_len);
      offset += global_tcp_handler.current_response_unt.header.data_len;
      global_tcp_handler.current_response_unt.check_sum = global_tcp_handler.recv_buffer[offset];
      //���½��ջ���������
      global_tcp_handler.recv_len -= data_len;
      memcpy(global_tcp_handler.recv_buffer, global_tcp_handler.recv_buffer + 2 + data_len, global_tcp_handler.recv_len);
    }
  }
	   */
}


static void do_insert_response(uint8* data, uint16 data_len)
{
/*
  //����һ�η������ݴ���һ���ṹ��
  // TODO:�о����ڷ��͵�һ����һ�������ǿ��ܻ�������һ��,������һ��һ��������ȴ���
  if(global_tcp_handler.recv_len + data_len > TCP_RECV_BUFFER_SIZE)
  {
    //��ջ�����
    memset(global_tcp_handler.recv_buffer, 0 , TCP_RECV_BUFFER_SIZE);
    global_tcp_handler.recv_len = 0;
  }
  //���뻺����
  memcpy(global_tcp_handler.recv_buffer + global_tcp_handler.recv_len, data, data_len);
  global_tcp_handler.recv_len += data_len;
  //����Ƿ����ݳ����Ƿ�ﵽ���ﵽ���������ǰ�ṹ��
  global_tcp_handler.parse_response();
*/
}
static void do_init_tcp_handler()
{
  memset(global_tcp_handler.recv_buffer, 0, TCP_RECV_BUFFER_SIZE);
  global_tcp_handler.recv_len = 0;
  global_tcp_handler.insert_response = do_insert_response;
  global_tcp_handler.find_handler = do_find_handler;
  global_tcp_handler.parse_response = do_parse_response;
  global_tcp_handler.wait_response = do_wait_response;
  global_tcp_handler.current_tcp_seqno = 0;
  memset(&global_tcp_handler.current_response_unt, 0, sizeof(sp_tcp_trans_unit));
}






////////////////////////////������������/////////////////////////////////////
int sp_prcocess_message(void)
{
	
	return 0;
}

int sp_send_request(sp_context* ctx, uint8 cmd, void* arg)
{
/*
  int ret = 0;
  uint32 seqno = 0;
  sp_tcp_trans_unit tcp_unt;
  pprotocol_process_handler handler = global_tcp_handler.find_handler(cmd);
  if(handler == NULL)
  {
    return SP_FLASH_FAIL;
  }
  seqno = global_tcp_handler.current_tcp_seqno++;
  ret = handler->send_request(ctx, arg);
  if(ret)
    return ret;
	*/
	return 0;
}















/////////////////////////////����������غ���//////////////////////////////////
static void get_std_ctl_len(uint8* ctl_len, uint16 msg_len)
{
  memcpy(ctl_len, &msg_len, 2);
}

static uint8 do_send_msg(const uint8 * msg, uint16 msg_len)
{
  uint8 send_msg[MAX_SENDMSG_LEN] = {0};
  uint8 ctl_len[2] = {0};
  int send_len = 0;
  if(msg_len > MAX_SENDMSG_LEN)
    return 2;
  send_len = msg_len + 2;

  get_std_ctl_len(ctl_len, msg_len);
  memcpy(send_msg, ctl_len, 2);
  memcpy(send_msg + 2, msg, msg_len);
  // disp_send_msg(send_msg , send_len);
  return do_com_sendbuf(COM1, send_msg, send_len);
}

////////////////////////�ⲿ����////////////////////////////
uint8 sp_send_struct(const sp_tcp_trans_unit* pstruct)
{
  uint8 send_buf[510] = {0};
  memcpy(send_buf, pstruct, sizeof(sp_tcp_trans_unit));
  return do_send_msg(send_buf, sizeof(sp_tcp_trans_unit));
  //return sp_send_msg("\x01\x02\x03\x04\x05\x06", 6);
}

static void receive_data(void *pBuf, uint16 nLen)
{
  //�ͼ�����ջ�����
  //global_tcp_handler.insert_response(pBuf, nLen);
}


void sp_init_com(void)
{
  DCB sDCB;
  sDCB.baudrate   = 38400;
  sDCB.datab    = 8;
  sDCB.parity   = 0;
  sDCB.stopb    = 1;
  sDCB.limitlen = 1000;
  sDCB.timeout  = 100;//300
  do_init_tcp_handler();
  sDCB.pRecvfuc = receive_data;
  StartComm(COM1, &sDCB);
}









































#ifdef SP_DEBUG
///////////////////////////////////////////////////////////////////////
//
// ������       : SendAddrToCan
// ��������     : ����CAN�ĵ�ַ
// ����         : unsigned char cAddr �����õ�CAN��ַ
// ����ֵ       : void
//
///////////////////////////////////////////////////////////////////////
uint8 SetAddrForCan(unsigned char cAddr)
{
  uint8 cTemp[7];
  uint8 i;
  uint8 count;
  int cLength;

  //���
  cTemp[0] = 0x5a;                // ��־
  cTemp[1] = 0x01;                // ����
  cTemp[2] = cAddr;         // Ҫ���͵ĵ�ַ
  cTemp[3] = CANH;        // �ն���ϵͳ

  /*
      SF_Read(FPA_CFGS, FLA_CANbps, &cTemp[4],1); //��CANͨ������
      if(cTemp[4] == 0x02)    //20k
      cLength = 4;
    else
    {
        cTemp[1] = 0x02;             // ����
      cLength = 5;
    }
    */

  SF_Read(FPA_CFGS, FLA_CANbps, &cTemp[4], 1); //��CANͨ������0x00:5K,0x01:10K,0x02:20K,0x03:50K
  if(cTemp[4] > 0x07) // 0x04:100K,0x05:125K,0x06:250K,0x07:500K,0x08:1000K
    cTemp[4] = 0x02;
  cLength = 5;

#if(POS_COMM == COMM_INTERRUPT)
  memcpy(DataBuf, cTemp, cLength);
#endif
  // �ȴ���Ӧ
  count = 3;

  do
  {
    UART_SendBuf(COM1, cTemp, cLength);   // ����ַ��CAN
    for(i = 0x2f; i > 0 ; i--)
    {
      KillWatchDog();
      DelayNS(10);          //��Ҫ����ʱ,��Ȼ����ɹ�(ԭ�����)
      if(giRcvLen != 0)                    // ���ճ��Ȳ�Ϊ0, �յ�����
      {
        if(giRcvLen == 4)                // �������
        {
          if((rcv_buf[0] == 0x5a)  // �ж������Ƿ�Ϊ���͹�ȥ������
             && (rcv_buf[1] == 0x01)) //&&(rcv_buf[2] == cAddr)
            //&&(rcv_buf[3] == CANH)
          {
            if(rcv_buf[3] < 0x08)
            {
              rcv_buf[2] = 0;
              rcv_buf[3] = 0;
            }
            SF_Write(FPA_CFGS, FLA_CAN_VER, (uint8 *)&rcv_buf[2], 2, TRUE);
            gbResetCanAddr = FALSE;
            giRcvLen = 0;
            Beep(3);
            return TRUE;          // �ɹ����򷵻�
          }
          else
          {
            break;
          }
        }
      }
    }
    gnBaudrate = gnBaudrateBuf[3 - count];
    SF_Write(FPA_CFGS, FLA_COMM_BAUDRATE, (uint8*)&gnBaudrate, 2, TRUE);
    COMM1_Init();
    DelayNS(10);
    giRcvLen = 0;
  }
  while(--count > 0);

  // ��ʱ����յ������ݲ���ȷ����ʾ����Ա�����ϵ�ȡ��ַ
  gsPosCurrentState.sPosSystemState = State_Error;
#if(HD_POS == HD_SC)
  CLS();
  DelayNS(1);
  SP_PRINT(0, 0, "����ͨѶģ��ʧ��");
  SP_PRINT(0, 1, "��������--ȷ�ϼ�");
  SP_PRINT(0, 2, "��������ͨ�ŷ�ʽ");
  DelayNS(DISPDELAY);
#elif(HD_POS == HD_S2)
  ShowState(LED_ERROR, ERR_CommCAN);
  Beep(5) ;
#endif
  giRcvLen = 0;
  return FALSE;

}

#endif

