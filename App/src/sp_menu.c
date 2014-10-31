#include "sp_menu.h"



//////////////////////菜单相关//////////////////////
static void do_menu_set(sp_context* p_ctx)
{
  int32 key = 0;
  sp_disp_msg(2, "设置");
  while(1)
  {
    key = sp_get_key();
    switch(key)
    {
      case SP_KEY_CLEAR:
        return;
      case SP_KEY_0:
        sp_disp_msg(2, "设置0");
        break;
    }
  }

}

static void do_consume_fixed_value(sp_context* ctx)
{
  sp_disp_msg(1000, "使用定值消费");
}

static void do_consume_normal(sp_context * ctx)
{
  int ret = 0;
  int32 key = 0;
  ctx->disp_type = 0;
  SP_CLS_FULLSCREEN;
  while(1)
  {
    //sp_sleep(10);
    key = sp_get_key();
    if(SP_KEY_FUNC == key)
      return;
    //检查是否有存储空间
    ret = sp_pre_check(ctx);
    if(ret)
    {
      sp_disp_msg(10, "存储空间不足");
      continue;
    }
    ret = sp_check_date_change(ctx);
    if(ret)
    {
      sp_disp_error("致命错误, ret=%04x,[%02x%02x%02x%02x]->[%02x%02x%02x%02x%02x%02x%02x]", ret,
                    ctx->syspara.today_date[0], ctx->syspara.today_date[1], ctx->syspara.today_date[2], ctx->syspara.today_date[3],
                    ctx->today[0], ctx->today[1], ctx->today[2], ctx->today[3], ctx->current_datetime[4], ctx->current_datetime[5], ctx->current_datetime[6]);
      //sp_sleep(99999);
      sp_select_card_app(ctx, &ctx->card);
    }
    ret = sp_consume_loop(ctx);
    if(SP_RETURN_FUNC == ret)
    {
      return;
    }
    if(ret)
    {
      if((ret != SP_NO_NUMBER_KEY_PRESSED) && (SP_CANCEL_CONSUME != ret))
      {
        sp_disp_error("消费错误, 错误码=%04x", ret);
        ctx->disp_type = 0;
      }
    }

    Reset_Reader_TimeOut();
    //500ms或1S刷新一次 从Flash读取点阵需要时间可放定时器回调函数处理
    //喂狗定义防止溢出程序复位
    //  KillWatchDog();
  }
}


static void do_menu_sign(sp_context* p_ctx)
{
  int32 ret = 0, key = 0;
  sp_tcp_trans_unit m_unt;
  memset(&m_unt, 0, sizeof(sp_tcp_trans_unit));
  memcpy(m_unt.header.guide_code, "\x00\x00\x01", 3);
  m_unt.header.cmd_code = 0x01;
  m_unt.header.data_len = 300;
  memcpy(m_unt.header.machine_addr, "\x99\x88", 2);
  memcpy(m_unt.data, "\x55\x66\x55\x66\x55", 5);
  m_unt.check_sum = 0x77;
  sp_disp_msg(2, "进入签到，等待按键");
  while(1)
  {
    ret = 0;
    key = sp_get_key();
    switch(key)
    {
      case SP_KEY_CLEAR:
        sp_disp_msg(2, "签到下按键CLAER");
        return;
      case SP_KEY_0:
        sp_disp_msg(2, "签到下按键0");
        break;
      case SP_KEY_1:
        sp_disp_msg(2, "签到下按键1");
        m_unt.header.data_len = 100;
        ret = sp_send_struct(&m_unt);
        break;
      case SP_KEY_2:
        sp_disp_msg(2, "签到下按键2");
        m_unt.header.data_len = 200;
        ret = sp_send_struct(&m_unt);
        break;
      case SP_KEY_3:
        sp_disp_msg(2, "签到下按键3");
        m_unt.header.data_len = 300;
        ret = sp_send_struct(&m_unt);
        break;
    }

    if(ret == 0)
    {
      //sp_disp_msg(2, "发送成功");
    }
    else
    {
      sp_disp_msg(2, "发送失败");
    }
  }
}
static void do_menu_showinfo(sp_context* p_ctx)
{
  sp_disp_flash(ADDR_SYSINFO, sizeof(sp_syspara));

}

static void do_menu_disp_transdtl(sp_context* p_ctx)
{
  uint8 ret = 0;
  int i = 0;
  int start_pos = 0;
  sp_transno_unit transno_unt;
  ret = sp_read_transno_unit(TRANSNO_FLAG_MASTER, &transno_unt);
  if(ret)
  {
    sp_disp_msg(999, "读取主流水号失败");
    return;
  }
  sp_disp_msg(999, "last_addr=%06x,last_no=%d", transno_unt.last_trans_addr, transno_unt.last_trans_no);
  if(transno_unt.last_trans_no > 2)
    start_pos = -2;
  for(i = start_pos; i < start_pos + 5; i++)
    sp_disp_flash(transno_unt.last_trans_addr + i * 64, 64); //打印五笔流水的数据
}

static void do_menu_disp_transno(sp_context* p_ctx)
{
  int i = 0;
  int unit_len = sizeof(sp_transno_unit);
  sp_disp_msg(999, "主流水号存储区数据");
  for(i = 0; i < FLASH_PAGE_SIZE / unit_len; i++)
    sp_disp_flash(ADDR_MASTER_TRANS_SEQNO + i * unit_len, unit_len);
  sp_disp_msg(999, "从流水号存储区数据");
  for(i = 0; i < FLASH_PAGE_SIZE / unit_len; i++)
    sp_disp_flash(ADDR_SLAVE_TRANS_SEQNO + i * unit_len, unit_len);
}



static int do_cancel_last_trans(sp_card* p_card)
{
  sp_disp_msg(20, "撤销上一笔消费");
  return 0;
}

static void do_menu_cancel(sp_context* p_ctx)
{
  int key = 0;
  int ret = 0;
  //char phyid_hex[33] = {0};
  sp_card this_card;
  sp_disp_by_type(SP_TP_REQUEST_CARD, p_ctx, &this_card, NULL);
  while(1)
  {
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
      return;
    ret = sp_request_card(this_card.cardphyid);
    if(ret == 0)
      break;
  }
  //重新寻卡
  ret = sp_request_card_poweron(&this_card);
  if(ret)
  {
    sp_disp_error("sp_request_card_purchase,ret=%04x", ret);
    return;
  }
  ret = sp_read_card(p_ctx, &this_card, SP_READ_FILE10 | SP_READ_FILE15 | SP_READ_FILE16 | SP_READ_CARDBAL);
  if(ret)
  {
    sp_disp_error("sp_read_card,ret=%04x", ret);
    return;
  }
  sp_disp_by_type(SP_TP_CANCEL_CONSUME, p_ctx, &this_card, NULL);
  while(1)
  {
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
      return;
    if(SP_KEY_CONFIRM == key)
      do_cancel_last_trans(&this_card);
  }
}
static void do_menu_query_revenue(sp_context* ctx)
{
  int key = 0;
  uint8 init_disp_flag = 0;
  uint8 upd_flag = 1;
  while(1)
  {
    if(upd_flag)
    {
      SP_CLS_FULLSCREEN;
      SP_PRINT(1, 0, "1.当日营业额查询");
      SP_PRINT(2, 0, "2.昨日营业额查询");
      upd_flag = 0;
    }
    key = sp_get_key();
    switch(key)
    {
      case SP_KEY_1:
        init_disp_flag = 1;
        sp_disp_by_type(SP_TP_REVENUE_QUERY, ctx, NULL, &init_disp_flag);
        upd_flag = 1;
        break;
      case SP_KEY_2:
        init_disp_flag = 2;
        sp_disp_by_type(SP_TP_REVENUE_QUERY, ctx, NULL, &init_disp_flag);
        upd_flag = 1;
        break;
      case SP_KEY_CLEAR:
        return;
    }
  }
}
static void do_menu_keep(sp_context* p_ctx)
{
  int32 key = 0;
  sp_disp_msg(2, "保留");
  while(1)
  {
    key = sp_get_key();
    switch(key)
    {
      case SP_KEY_CLEAR:
        return;
      case SP_KEY_0:
        sp_disp_msg(2, "保留0");
        break;
      case SP_KEY_1:
        sp_disp_msg(2, "保留1");
        break;
    }
  }

}


static void do_show_menu(const menu_info_t* menu_info, int menu_count, void* arg)
{
  int32 index = 0;
  int32 page_index = 0;
  int32 page_cnt = 0;
  int32 key = 0;
  int32 start = 0;
  bool need_upd = true;//是否需要刷新
  int32 i = 0, cnt = 0, last = 0;
  bool no_key_pressed = false;
  bool is_quit = false;
  page_cnt = menu_count / MAX_MENU_SCREEN_CNT + 1;
  while(1)
  {
    if(need_upd)
    {
      SP_CLS_FULLSCREEN;
      start = page_index * MAX_MENU_SCREEN_CNT;
      cnt = 0;
      last = start + MAX_MENU_SCREEN_CNT;
      if(last > menu_count)
        last = menu_count;
      for(i = start; i < last; i++)
      {
        SP_PRINT(cnt++, 0, (char*)menu_info[i].menu_desc);
      }
      SP_PRINT(3, 2, "按+/ - 上下翻页");
      need_upd = false;
    }
    is_quit = false;
    no_key_pressed = true;
    key = sp_get_key();
    switch(key)
    {
      case SP_KEY_CLEAR:
        no_key_pressed = false;
        is_quit = true;
        return;
        // break;
      case SP_KEY_ADD:
        if(page_index < page_cnt - 1)
        {
          page_index ++;
          need_upd = true;
        }
        break;
      case SP_KEY_MINUS:
        if(page_index > 0)
        {
          page_index--;
          need_upd = true;
        }
        break;
      case SP_KEY_1:
        no_key_pressed = false;
        index = 1;
        break;
      case SP_KEY_2:
        no_key_pressed = false;
        index = 2;
        break;
      case SP_KEY_3:
        no_key_pressed = false;
        index = 3;
        break;
      case SP_KEY_4:
        no_key_pressed = false;
        index = 4;
        break;
      case SP_KEY_5:
        no_key_pressed = false;
        index = 5;
        break;
      case SP_KEY_6:
        no_key_pressed = false;
        index = 6;
        break;
      case SP_KEY_7:
        no_key_pressed = false;
        index = 7;
        break;
      case SP_KEY_8:
        no_key_pressed = false;
        index = 8;
        break;
      case SP_KEY_9:
        no_key_pressed = false;
        index = 9;
        break;
      case SP_KEY_0:
        no_key_pressed = false;
        index = 10;
        break;
      default:
        break;
    }
    if(no_key_pressed == false)
    {
      if((index > 0) && (index <= menu_count))
      {
        menu_info[index - 1].menu_func(arg);
        need_upd = true;
      }
      if(is_quit)
        break;
    }
  }
}

static void do_menu_main(sp_context* p_ctx)
{
  menu_info_t menus[] =
  {
    {"1. 消费撤销", (menu_func_t)do_menu_cancel},
    {"2. 营业额查询", (menu_func_t)do_menu_query_revenue},
    {"3. 手工更新黑名单", (menu_func_t)do_menu_keep},
    {"4. 手动上传流水", (menu_func_t)do_menu_keep},
    {"5. 链路检测", (menu_func_t)do_menu_keep},
    {"6. 手工签到", (menu_func_t)do_menu_sign},
    {"7. 终端参数查询", (menu_func_t)do_menu_showinfo},
    {"8. 管理功能", (menu_func_t)do_menu_set},
    {"9. 流水号存储区", (menu_func_t)do_menu_disp_transno},
    {"0. 流水存储区", (menu_func_t)do_menu_disp_transdtl}
  };
  do_show_menu(menus, sizeof(menus) / sizeof(menus[0]), p_ctx);
}

void sp_menu_consume(sp_context* p_ctx)
{
  while(1)
  {
    if(p_ctx->syspara.work_mode == SP_WORK_MODE_NORMAL)
      do_consume_normal(p_ctx);
    else if(p_ctx->syspara.work_mode == SP_WORK_MODE_FIXED_VALUE)
      do_consume_fixed_value(p_ctx);
    do_menu_main(p_ctx);
    //KillWatchDog();
  }
}






