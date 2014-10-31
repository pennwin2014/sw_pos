#include "sp_disp.h"


void sp_disp_press_key(uint8 key_num, uint8 key_cnt)
{
  char dispmsg[100];
  memset(dispmsg, 0, 100);
  sprintf(dispmsg, "hex=%x,dec=%d", key_num, key_cnt);
  SP_PRINT(1, 0, dispmsg);
}

static int do_wait_for_retkey(const int32 retkey, const char* wait_msg, bool wait_flag)
{
  int32 key = 0;
  int msg_len = 0, page_cnt = 0, page_index = 0, lastpage_len = 0, copy_len = 0;
  char tmp_msg[100];
  bool change_flag = false;
  msg_len = strlen(wait_msg);
  page_cnt = msg_len / MAX_SCREEN_WORDS;
  if(msg_len > page_cnt * MAX_SCREEN_WORDS)
  {
    lastpage_len = msg_len - page_cnt * MAX_SCREEN_WORDS;
    page_cnt ++;
  }
  memset(tmp_msg, 0, 100);
  memcpy(tmp_msg, wait_msg, sp_get_min(MAX_SCREEN_WORDS, msg_len));
  SP_CLS_FULLSCREEN;
  SP_PRINT(0, 0, tmp_msg);

  //假如显示的字符数大于一页的最大字数，则采用分页显示，按键是+，-键可以上下翻页
  while(1)
  {
    change_flag = false;
    key = sp_get_key();
    if(wait_flag == true)
    {
      if(retkey == key)
        return 0;
    }
    else
    {
      if((key == SP_KEY_CLEAR) || (key == SP_KEY_CONFIRM) || (key == SP_KEY_MODE) || (key == SP_KEY_FUNC))
        return key;
    }
    if(SP_KEY_ADD == key)
    {
      change_flag = true;
      page_index++;
    }
    else if(SP_KEY_MINUS == key)
    {
      change_flag = true;
      page_index--;
    }
    if(page_index > page_cnt - 1)
    {
      change_flag = false;
      page_index = page_cnt - 1;
    }
    else if(page_index < 0)
    {
      change_flag = false;
      page_index = 0;
    }
    //显示第page_index页的数据
    if((change_flag == true) && (page_cnt > 1))
    {
      memset(tmp_msg, 0, 100);
      copy_len = MAX_SCREEN_WORDS;
      if(page_index == page_cnt - 1)
      {
        if(lastpage_len > 0)
        {
          copy_len = lastpage_len;
        }
      }
      memcpy(tmp_msg, wait_msg + page_index * MAX_SCREEN_WORDS, copy_len);
      // sp_disp_msg(tmp_msg, 3);
      SP_CLS_FULLSCREEN;
      SP_PRINT(0, 0, tmp_msg);
    }
  }
  //return 0;
}

void do_disp_money(const char* value)
{
  char disp_msg[30];
  int len = 0;
  SP_CLS_FULLSCREEN;
  memset(disp_msg, 0 , 30);
  len = strlen(value);
  //assert(len < 16);
  //根据len居中
  sprintf(disp_msg, "%s", value);
  SP_PRINT(1, (16 - len) / 2, disp_msg);
  SP_PRINT(2, 0, "输入完毕请按确认");
  SP_PRINT(3, 0, "输入错误请按清除");
  //SP_PRINT(3, 0, "确认--模式--清除");
}
static void do_disp_request_consume_card(sp_context* p_ctx, sp_card* p_card)
{
  char disp_msg[50];
  uint8 now_sec = 0;
  now_sec = p_ctx->current_datetime[6];
  SP_CLS_FULLSCREEN;

  if(p_ctx->online_flag)
    SP_PRINT(0, 14, " ");
  else
    SP_PRINT(0, 14, "*");
  now_sec = p_ctx->current_datetime[6];
  if(now_sec % 5 == 0)
    SP_PRINT(1, 2, "请刷卡>>>");
  else if(now_sec % 5 == 4)
  {
    SP_CLS(2);
    SP_PRINT(1, 2, "请刷卡>>");
  }
  else if(now_sec % 5 == 3)
  {
    SP_CLS(2);
    SP_PRINT(1, 2, "请刷卡>");
  }
  else if(now_sec % 5 == 2)
    SP_PRINT(1, 2, "请刷卡>>");
  else
    SP_PRINT(1, 2, "请刷卡>>>");
  memset(disp_msg, 0, 50);
  sprintf(disp_msg, "交易额 :%.2f", p_card->amount * 1.0 / 100.0);
  SP_PRINT(2, 2, disp_msg);
  /*
  memset(disp_msg, 0, 50);
  sprintf(disp_msg, "%02d/%02d %02d:%02d:%02d", p_ctx->systime.chMonth, p_ctx->systime.chDay, p_ctx->systime.chHour,
          p_ctx->systime.chMinute, p_ctx->systime.chSecond);
  SP_PRINT(3, 2, disp_msg);
  */
}

static void do_disp_request_card(sp_context* p_ctx)
{
  char disp_msg[50];
  uint8 now_sec = 0;
  /*
  memset(disp_msg, 0, 50);
  sprintf(disp_msg, "%d", p_ctx->disp_cnt++);
  SP_PRINT(1, 0, disp_msg);
  */

  //  uint8 ret = 0;
  //
  if(p_ctx->disp_type != SP_TP_REQUEST_CARD)
  {
    SP_CLS_FULLSCREEN;
  }
  // else
  // {
  //SP_CLS(0);
  //SP_CLS(3);
  // }
  if(p_ctx->online_flag)
    SP_PRINT(0, 14, " ");
  else
    SP_PRINT(0, 14, "*");
  //MAX_SCREEN_ROWS
  //SP_PRINT(1, 4, p_ctx->school_name);
  now_sec = p_ctx->current_datetime[6];
  if(now_sec % 5 == 0)
    SP_PRINT(1, 4, "请刷卡>>>");
  else if(now_sec % 5 == 4)
  {
    SP_CLS(1);
    SP_PRINT(1, 4, "请刷卡>>");
  }
  else if(now_sec % 5 == 3)
  {
    SP_CLS(1);
    SP_PRINT(1, 4, "请刷卡>");
  }
  else if(now_sec % 5 == 2)
  {
    SP_CLS(1);
    SP_PRINT(1, 4, "请刷卡>>");
  }
  else
    SP_PRINT(1, 4, "请刷卡>>>");
  memset(disp_msg, 0, 50);
  sprintf(disp_msg, "%02x/%02x %02x:%02x:%02x", p_ctx->current_datetime[2], p_ctx->current_datetime[3], p_ctx->current_datetime[4],
          p_ctx->current_datetime[5], p_ctx->current_datetime[6]);
  SP_PRINT(3, 2, disp_msg);

}

static void do_disp_user_balance(sp_card* p_card)
{
  char disp_msg[30];
  SP_CLS_FULLSCREEN;
  memset(disp_msg, 0, 30);
  sprintf(disp_msg, "姓名 :%s", p_card->username);
  SP_PRINT(1, 0, disp_msg);
  memset(disp_msg, 0, 30);
  sprintf(disp_msg, "学号 :%s", p_card->stuempno);
  SP_PRINT(2, 0, disp_msg);
  memset(disp_msg, 0, 30);
  sprintf(disp_msg, "余额 :%.2f", p_card->cardbefbal * 1.0 / 100.0);
  SP_PRINT(3, 0, disp_msg);
}

static void do_disp_cancel_consume(sp_card* p_card)
{
  char disp_msg[30];
  uint32 money;
  SP_CLS_FULLSCREEN;
  memset(disp_msg, 0, 30);

  sprintf(disp_msg, "%02x/%02x %02x:%02x:%02x", p_card->last_trans_date[2], p_card->last_trans_date[3],
          p_card->last_trans_time[0], p_card->last_trans_time[1], p_card->last_trans_time[2]);
  SP_PRINT(0, 2, disp_msg);
  memset(disp_msg, 0, 30);
  sprintf(disp_msg, "姓名 :%s", p_card->username);
  SP_PRINT(1, 0, disp_msg);
  memset(disp_msg, 0, 30);

  sp_buffer_to_uint32(p_card->last_trans_amt, &money);
  /*sp_disp_msg(100, "last_trans_amt=[%02x-%02x-%02x-%02x],money=%d", p_card->last_trans_amt[0], p_card->last_trans_amt[1],
              p_card->last_trans_amt[2], p_card->last_trans_amt[3], money);*/
  sprintf(disp_msg, "交易额 :%.2f", money * 1.0 / 100.0);
  SP_PRINT(2, 0, disp_msg);
  SP_PRINT(3, 4, "撤销请按确认");
}

static void do_disp_consume_success(sp_context* p_ctx, sp_card* p_card)
{
  //  int32 key = 0;
  char disp_msg[30];
  SP_CLS_FULLSCREEN;
  SP_PRINT(1, 2, "消费成功 !");
  memset(disp_msg, 0, 30);
  sprintf(disp_msg, "交易额 :%.2f", p_card->amount * 1.0 / 100.0);
  SP_PRINT(2, 0, disp_msg);
  memset(disp_msg, 0, 30);
  sprintf(disp_msg, "卡余额 :%.2f", (p_card->cardbefbal - p_card->amount) * 1.0 / 100.0);
  SP_PRINT(3, 0, disp_msg);
  sp_sleep(2500);
}

void sp_wait_seconds(int wait_sec)
{
  char disp_msg[20];
  int key = 0;
  while(wait_sec --)
  {
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
      return;
    memset(disp_msg, 0, 20);
    sprintf(disp_msg, "%d", wait_sec);
    SP_CLS(3);
    SP_PRINT(3, 0, disp_msg);
    sp_sleep(1000);
  }
}

static void do_disp_insufficient_balance()
{
  SP_CLS_FULLSCREEN;
  SP_PRINT(1, 2, "消费失败 !");
  SP_PRINT(2, 0, "您的卡余额不足");
  sp_sleep(2000);
  sp_wait_seconds(3);
}
static void is_cancel_consume(uint8* cancel_flag)
{
  int key = 0;
  SP_CLS_FULLSCREEN;
  SP_PRINT(1, 0, "是否取消消费 ?");
  SP_PRINT(2, 6, "确认键取消");
  SP_PRINT(3, 6, "清除键返回");
  while(1)
  {
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
    {
      *cancel_flag = SP_SWITCH_OFF;
      return;
    }
    else if(SP_KEY_CONFIRM == key)
    {
      *cancel_flag = SP_SWITCH_ON;
      return;
    }
  }
}

static void show_passwd_error()
{
  SP_CLS_FULLSCREEN;
  SP_PRINT(1, 2, "密码错误!!");
  sp_wait_seconds(3);
}

static void do_disp_revenue(sp_context* ctx, uint8* init_flag)
{
  int key = 0;
  char disp_msg[20];
  int show_cnt = 0;
  int show_amt = 0;
  uint8 upd_flag = 1;
  uint8 cur_opp_index = 0;
  if(*init_flag == 1)
  {
    show_cnt = ctx->syspara.today_total_cnt;
    show_amt = sp_d2i((ctx->syspara.today_total_amt * 1.0) / 100.0);
    cur_opp_index = 2;
  }
  else
  {
    show_cnt = ctx->syspara.yesterday_total_cnt;
    show_amt = sp_d2i((ctx->syspara.yesterday_total_amt * 1.0) / 100.0);
    cur_opp_index = 1;
  }
  while(1)
  {
    if(upd_flag)
    {
      SP_CLS_FULLSCREEN;
			if(cur_opp_index == 2)
			{
      	SP_PRINT(0, 2, "当日营业额");
			}
			else
			{
				SP_PRINT(0, 2, "昨日营业额");
			}
      memset(disp_msg, 0, 20);
      sprintf(disp_msg, "总笔数 :%d", show_cnt);
      SP_PRINT(1, 0, disp_msg);
      memset(disp_msg, 0, 20);
      sprintf(disp_msg, "总金额 :%d", show_amt);
      SP_PRINT(2, 0, disp_msg);
      //memset(disp_msg, 0, 20);
      //sprintf(disp_msg, "继续查询请按 :%d", cur_opp_index);
      //SP_PRINT(2, 0, disp_msg);
      //SP_PRINT(3, 0, "退出查询请按确认");
      upd_flag = 0;
    }
    key = sp_get_key();
    if(SP_KEY_1 == key)
    {
      if(cur_opp_index == 2)
        continue;
      show_cnt = ctx->syspara.today_total_cnt;
      show_amt = sp_d2i((ctx->syspara.today_total_amt * 1.0) / 100.0);
      cur_opp_index = 2;
      upd_flag = 1;
    }
    else if(SP_KEY_2 == key)
    {
      if(cur_opp_index == 1)
        continue;
      show_cnt = ctx->syspara.yesterday_total_cnt;
      show_amt = sp_d2i((ctx->syspara.yesterday_total_amt * 1.0) / 100.0);
      cur_opp_index = 1;
      upd_flag = 1;
    }
    else if(SP_KEY_CLEAR == key)
    {
      return;
    }
  }
}

void sp_disp_by_type(uint8 disp_type, sp_context* p_ctx, sp_card* p_card, void* other)
{
  switch(disp_type)
  {
    case SP_TP_REQUEST_CARD:
      do_disp_request_card(p_ctx);
      break;
    case SP_TP_USER_BALANCE:
      do_disp_user_balance(p_card);
      break;
    case SP_TP_CANCEL_CONSUME:
      do_disp_cancel_consume(p_card);
      break;
    case SP_TP_CONSUME_SUCCESS:
      do_disp_consume_success(p_ctx, p_card);
      break;
    case SP_TP_INSUFFICIENT_BALANCE:
      do_disp_insufficient_balance();
      break;
    case SP_TP_REQUEST_CONSUME_CARD:
      do_disp_request_consume_card(p_ctx, p_card);
      break;
    case SP_TP_DISP_MONEY:
      do_disp_money(other);
      break;
    case SP_TP_IS_CANCEL_CONSUME:
      is_cancel_consume(other);
      break;
    case SP_TP_PASSWORD_ERROR:
      show_passwd_error();
      break;
    case SP_TP_REVENUE_QUERY:
      do_disp_revenue(p_ctx, other);
      break;
  }
  p_ctx->disp_type = disp_type;
}

void sp_disp_msg(uint32 nsec, const char* fmt, ...)
{
  char msg[1024];
  int len;
  int key = 0, left_sec = 0;
  char sec_buf[100];
  va_list arg;
  va_start(arg, fmt);
  len = vsnprintf(msg, sizeof(msg) - 1, fmt, arg);
  va_end(arg);
  if(len > 0)
  {
    SP_CLS_FULLSCREEN;
    SP_PRINT(0, 0, msg);
  }
  left_sec = nsec;
  while(left_sec > 0)
  {
    memset(sec_buf, 0 , 100);
    // sprintf(sec_buf, "%ds press=%02x", left_sec--, key);
    sprintf(sec_buf, "%d", left_sec--);
    SP_PRINT(3, 0, sec_buf);
    sp_DelayNS2(5000000);
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
      return;
  }
}


static int messagebox_yes_no(const char* final_msg)
{
  return do_wait_for_retkey(0, final_msg, false);
}
int sp_messagebox(const int box_type, const char* func_name,
                  const int lineno, const char* fmt, ...)
{
  char msg[1024];
  char final_msg[1024];
  int len;
  va_list arg;
  va_start(arg, fmt);
  len = vsnprintf(msg, sizeof(msg) - 1, fmt, arg);
  va_end(arg);

  if(len > 0)
  {
    memset(final_msg, 0, 1024);
    memcpy(final_msg, msg, strlen(msg));
#ifdef SP_DEBUG
    sprintf(final_msg + strlen(msg), " (func=%s,line=%d)", func_name, lineno);
#endif

    switch(box_type)
    {
      case SP_MB_YES_NO:
        return messagebox_yes_no(final_msg);
      case SP_MB_OK:
        break;
    }
  }
  return 0;
}

void sp_wait_for_retkey(const int32 retkey, const char* func_name,
                        const int lineno, const char* fmt, ...)
{
  char msg[1024];
  char final_msg[1024];
  int len;
  va_list arg;
  va_start(arg, fmt);
  len = vsnprintf(msg, sizeof(msg) - 1, fmt, arg);
  va_end(arg);
  if(len > 0)
  {
    memset(final_msg, 0, 1024);
    memcpy(final_msg, msg, strlen(msg));
#ifdef SP_DEBUG
    sprintf(final_msg + strlen(msg), " (func=%s,line=%d)<press 0X%x to return>", func_name, lineno, retkey);
#endif
    sprintf(final_msg + strlen(msg), ",len=%d", strlen(msg));
    do_wait_for_retkey(retkey, final_msg, true);
  }
}

#define SP_ERROR_DEBUG 0
void sp_disp_error(const char* fmt, ...)
{
#ifdef SP_ERROR_DEBUG
  char msg[1024];
  int len;
  int key = 0, left_sec = 0;
  char sec_buf[100];
  va_list arg;
  va_start(arg, fmt);
  len = vsnprintf(msg, sizeof(msg) - 1, fmt, arg);
  va_end(arg);
  if(len > 0)
  {
    SP_CLS_FULLSCREEN;
    SP_PRINT(0, 0, msg);
  }
  left_sec = 999;
  while(left_sec > 0)
  {
    memset(sec_buf, 0 , 100);
    // sprintf(sec_buf, "%ds press=%02x", left_sec--, key);
    sprintf(sec_buf, "%d", left_sec--);
    SP_PRINT(3, 0, sec_buf);
    sp_DelayNS2(5000000);
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
      return;
  }
#endif
}
/*
static void send_debug_log(const char* msg, const int len)
{
  sp_tcp_trans_unit tcp_unt;
  memset(&tcp_unt, 0, sizeof(sp_tcp_trans_unit));
  tcp_unt.header.cmd_code = 0x02;//代表上传的是流水
  memcpy(tcp_unt.data, msg, len);
  tcp_unt.header.data_len = len;
  sp_send_struct(&tcp_unt);
}
*/
//#define SP_DEBUG_MODE 0
void sp_disp_debug(const char* fmt, ...)
{
#ifdef SP_DEBUG_MODE
  char msg[1024];
  int len;
  int key = 0, left_sec = 0;
  char sec_buf[100];

  va_list arg;
  va_start(arg, fmt);
  len = vsnprintf(msg, sizeof(msg) - 1, fmt, arg);
  va_end(arg);

  if(len > 0)
  {
    send_debug_log(msg, len);
    SP_CLS_FULLSCREEN;
    SP_PRINT(0, 0, msg);
  }

  left_sec = 999;
  while(left_sec > 0)
  {
    memset(sec_buf, 0 , 100);
    // sprintf(sec_buf, "%ds press=%02x", left_sec--, key);
    sprintf(sec_buf, "%d", left_sec--);
    SP_PRINT(3, 0, sec_buf);
    sp_DelayNS2(5000000);
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
      return;
  }

#endif
}




