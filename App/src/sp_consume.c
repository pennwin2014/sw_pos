#include "sp_consume.h"


static int is_blkcard(sp_context* p_ctx, uint32 cardno)
{
  uint32 page_no;
  uint8 blkbuffer[64], t;
  sp_bitmap_offset_t offset;
  uint8 page_offset = 0;
  sp_bitmap_transform(cardno, &offset);
  page_no = (ADDR_BLACKLIST + offset.byte_offset) / FLASH_PAGE_SIZE;
  page_offset = ADDR_BLACKLIST + offset.byte_offset - page_no * FLASH_PAGE_SIZE;
  sp_SF_Read(page_no, page_offset, blkbuffer, 1);
  t = blkbuffer[0];//[offset.byte_offset - 0];
  return ((t & (1 << offset.bit_offset)) == 0) ? 1 : 0;
}
static int do_input_and_check_card_passwd(sp_context* p_ctx, sp_card* card)
{
  char show_password[MAX_SCREEN_ROWS];
  char real_password[MAX_SCREEN_ROWS];
  char temp[MAX_SCREEN_ROWS];
  int start_pos = 0;
  int key = 0;
  uint8 tmp_len = 0;
  uint8 t;
  bool upd_flag = false;
  memset(show_password, 0, MAX_SCREEN_ROWS);
  memset(real_password, 0, MAX_SCREEN_ROWS);
  SP_CLS_FULLSCREEN;
  SP_PRINT(0, 0, "�����뿨����:");
  SP_PRINT(2, 0, "��������밴ȷ��");
  SP_PRINT(3, 0, "��������밴���");
  while(1)
  {
    if(upd_flag == true)
    {
      memset(show_password, 0, MAX_SCREEN_ROWS);
      memset(show_password, '*', tmp_len);
      start_pos = (MAX_SCREEN_ROWS - tmp_len) / 2 - 1;
      SP_CLS(1);
      SP_PRINT(1, start_pos, show_password);
    }
    upd_flag = false;
    key = sp_get_key();
    if(SP_KEY_CLEAR == key)
    {
      if(tmp_len == 0)
        return SP_CARD_PASSWD_CANCEL;
      tmp_len--;
      upd_flag = true;
      continue;
    }
    if(SP_KEY_CONFIRM == key)
    {
      //�Ƚ������Ƿ���ȷ
      //sp_disp_error("real[%02x%02x%02x%02x],card[%02x%02x%02x%02x]", real_password[0], real_password[1],
      //            real_password[2], real_password[3], card->passwd[0], card->passwd[1], card->passwd[2], card->passwd[3]);
      if(tmp_len != strlen(card->passwd))//���볤�ȴ���
        return SP_CARD_PASSWD_ERROR;
      if(memcmp(real_password, card->passwd, strlen(card->passwd)) != 0)
      {
        return SP_CARD_PASSWD_ERROR;
      }
      break;
    }
    if(!sp_is_number_key(key, &t))
    {
      if(t < 10)
      {
        if(tmp_len >= 6)
          continue;
        tmp_len++;
        strcpy(temp, real_password);
        sprintf(real_password, "%s%d", temp, t);
        upd_flag = true;
      }
    }
  }
  return 0;
}
static int do_check_consume_limit(sp_context * p_ctx, sp_card* card)
{
  int ret = 0;
  uint8 cancel_flag = 0;
  card->use_passwd = 0;
  if(p_ctx->syspara.limit_switch == SP_SWITCH_OFF)
    return 0;
  //����
  if(card->once_limit_amt > 0 && card->payamt >= card->once_limit_amt)
  {
    card->use_passwd = 1;
  }
  else if(card->day_sum_limit_amt > 0 && (card->day_sum_amt + card->payamt) >= card->day_sum_limit_amt)
  {
    card->use_passwd = 1;
  }
  //pos����
  else if(p_ctx->syspara.once_limit_amt > 0 && card->payamt >= p_ctx->syspara.once_limit_amt)
  {
    card->use_passwd = 1;
  }
  else if(p_ctx->syspara.day_sum_limit_amt > 0 && (card->day_sum_amt + card->payamt) >= p_ctx->syspara.day_sum_limit_amt)
  {
    card->use_passwd = 1;
  }
  if(card->use_passwd)
  {
    // �������
    ret = sp_read_card(p_ctx, card, SP_READ_FILE19);
    if(ret)
      return ret;
    while(1)
    {
      ret = do_input_and_check_card_passwd(p_ctx, card);
      if(SP_SUCCESS == ret)
        return SP_SUCCESS;
      else if(SP_CARD_PASSWD_CANCEL == ret)
      {
        sp_disp_by_type(SP_TP_IS_CANCEL_CONSUME, p_ctx, card, &cancel_flag);
        if(cancel_flag == SP_SWITCH_OFF)
        {
          continue;
        }
        else
          return ret;
      }
      else if(SP_CARD_PASSWD_ERROR == ret)
      {
        sp_beep(1);
        sp_disp_by_type(SP_TP_PASSWORD_ERROR, p_ctx, card, NULL);
        continue;
      }
      return ret;
    }

  }
  return 0;
}
static int do_check_blkcard(sp_context * p_ctx, sp_card* card)
{
  if(memcmp(card->cardverno, p_ctx->cardverno, 7) > 0)
  {
    return 0;
  }
  return is_blkcard(p_ctx, card->cardno);
}

//��ˮ���������ⲿ��p_ctx���record��ֵ
static int prepare_transdtl(sp_context * p_ctx, sp_card* card, sp_transdtl* transdtl)
{
  int extra_amount = 1;
  uint32 trans_no = 0;
  uint16 record_used_size;
  //  memset(&(p_ctx->record), 0, sizeof(sp_transdtl));
  //�Ƿ���Ҫ�������е���ˮ�ṹ�壬�Ķ�sp_card�ṹ��Ķ���???????
  transdtl->amount = card->amount;
  transdtl->cardbefbal = card->cardbefbal;
  transdtl->cardbefcnt = card->paycnt;
  transdtl->cardno = card->cardno;
  memcpy(transdtl->extra_amt, &extra_amount, 3);
  transdtl->last_amt = card->amount;
  transdtl->last_cnt = card->paycnt;
  memcpy(transdtl->last_limit_amt, (uint8*)(&(card->amount)), 3);
  memset(transdtl->tac, 0 , sizeof transdtl->tac);
  memcpy(transdtl->tac, card->tac, 4);
  memcpy(transdtl->transdatetime, p_ctx->current_datetime + 1, 6);
  memcpy(transdtl->psamno, p_ctx->syspara.samno, 6);
  //file10�õ�
  transdtl->last_transflag = 0x01;
  transdtl->last_amt = card->amount;
  transdtl->last_cnt = card->paycnt;
  memcpy(transdtl->last_termno, card->last_trans_dev, 6);
  memcpy(transdtl->last_datetime, p_ctx->current_datetime + 1, 6);
  //д��ˮ��ʱ����õ������ˮ��
  trans_no = sp_get_transno();
  transdtl->termseqno = trans_no + 1;
  //sp_disp_debug("termseqno=%d", transdtl->termseqno);
  //����crc
  record_used_size = sizeof(sp_transdtl) - 2;//transdtl->crc - &transdtl->termseqno;
  //sp_disp_msg(20, "Record used size=%d", record_used_size);

  sp_protocol_crc((uint8*)transdtl, record_used_size, transdtl->crc);
  return  0;
}



static void do_update_sum_amt(sp_context* ctx, sp_transdtl* record)
{
  uint8 ret = 0;
  int diff_days = 0;
  sp_get_time(ctx);
  diff_days = sp_calc_diff_days(ctx->today, ctx->syspara.today_date);
  if(diff_days > 1)
  {
    ctx->syspara.yesterday_total_cnt = 0;
    ctx->syspara.yesterday_total_amt = 0;
    ctx->syspara.today_total_cnt = 0;
    ctx->syspara.today_total_amt = 0;
    memcpy(ctx->syspara.today_date, ctx->today, 4);
  }
  else if(diff_days == 1)
  {
    ctx->syspara.yesterday_total_cnt = ctx->syspara.today_total_cnt;
    ctx->syspara.yesterday_total_amt = ctx->syspara.today_total_amt;
    ctx->syspara.today_total_cnt = 0;
    ctx->syspara.today_total_amt = 0;
    memcpy(ctx->syspara.today_date, ctx->today, 4);
  }
  else if(diff_days < 0)
  {
    sp_disp_error("��������,ʱ�䵹��,diffdays=%d",diff_days);
		ret = sp_select_card_app(ctx, &ctx->card);
  }
  ctx->syspara.today_total_amt += record->amount;
  ++ctx->syspara.today_total_cnt;
  ret = sp_write_syspara(ctx);
  if(ret)
  {
    //����������ϵ����Ա
    sp_disp_error("������������ϵ����Ա��������%04x", ret);
    sp_sleep(9999999);
  }
}






//////////////////////////////�����ĺ���//////////////////////////////////////////////
void sp_check_record_rom(sp_context* p_ctx)
{
  p_ctx->rec_flash_full = false;
}



uint8 sp_pre_check(sp_context* p_ctx)
{
  sp_check_record_rom(p_ctx);
  if(p_ctx->rec_flash_full)
  {
    sp_disp_msg(30, "��ˮ�洢��������ɼ�");
    return SP_FLASH_TRANSDTL_FULL;
  }
  return SP_SUCCESS;
}

static int do_input_money(sp_context* ctx, uint32* money, const uint8* cardphyid, char* value)
{
  uint8 upd = 1;
  uint32 key;
  int32 ret;
  uint8 req_card_phyid[4];
  char temp[20];
  uint8 t;
  char show_msg[100];
  int i = 0;
  while(1)
  {
    if(upd)
    {
      //����һ�½��
      sp_disp_by_type(SP_TP_DISP_MONEY, ctx, NULL, value);
      upd = 0;
    }
    ret = sp_request_card(req_card_phyid);
    if(ret)
    {
      //sp_disp_error(3, "==Ѱ��ʧ��==");
      return SP_CARD_NOT_FOUND;
    }

    if(memcmp(req_card_phyid, cardphyid, 4) != 0)
    {
      memset(show_msg, 0 , 100);
      memcpy(show_msg, "phyid=", 6);
      for(i = 0; i < 4; i++)
      {
        sprintf(show_msg + 6 + 3 * i, "%02x,", req_card_phyid[i]);
      }
      sp_disp_debug(show_msg);
      return SP_NOT_SAME_CARD;
    }

    key = sp_get_key();
    if(!sp_is_number_key(key, &t))
    {
      if(strlen(value) >= 6)
        continue;
      strcpy(temp, value);
      if(t == SP_KEY_DOT)
      {
        //��һ���������ǵ�
        if(strlen(value) == 0)
        {
          memcpy(value, "0.", 2);
        }
        else
        {
          //�����Ѿ����ڵ��˾Ͳ�����
          if(strchr(temp, '.') != NULL)
            continue;
          sprintf(value, "%s%c", temp, '.');
        }
      }
      else
      {
        sprintf(value, "%s%d", temp, t);
      }
      upd = 1;
    }
    /*
    else if(key == SP_KEY_MODE)
    {
      return -1;
    }
    */
    else if(key == SP_KEY_CONFIRM)
    {
      //*money = atoi(value) * 100;
      *money = sp_atof_mul100(value);
      return 0;
    }
    else if(key == SP_KEY_CLEAR)
    {
      uint8 vl = strlen(value);
      if(vl == 0)
        return 1;
      if(vl > 0)
        value[vl - 1] = 0;
      upd = 1;
    }
  }
}
static int do_input_money_without_card(sp_context* ctx, uint32* money, uint8 t)
{
  uint8 upd = 1;
  uint32 key;
  char temp[20];
  char value[20] = {0};
  //uint8 t;
  //������
  if(t == SP_KEY_DOT)//�������.�ͱ��0.
  {
    memcpy(value, "0.", 2);
  }
  else
  {
    strcpy(temp, value);
    sprintf(value, "%s%d", temp, t);
  }
  while(1)
  {
    if(upd)
    {
      //����һ�½��
      sp_disp_by_type(SP_TP_DISP_MONEY, ctx, NULL, value);
      upd = 0;
    }

    key = sp_get_key();

    if(!sp_is_number_key(key, &t))
    {
      if(strlen(value) >= 6)
        continue;
      strcpy(temp, value);
      if(t == SP_KEY_DOT)
      {
        //��һ���������ǵ�
        if(strlen(value) == 0)
        {
          memcpy(value, "0.", 2);
        }
        else
        {
          //�����Ѿ����ڵ��˾Ͳ�����
          if(strchr(temp, '.') != NULL)
            continue;
          sprintf(value, "%s%c", temp, '.');
        }
      }
      else
      {
        sprintf(value, "%s%d", temp, t);
      }
      upd = 1;
    }
    else if(key == SP_KEY_CONFIRM)
    {
      //*money = atoi(value) * 100;
      *money = sp_atof_mul100(value);
      return 0;
    }
    else if(key == SP_KEY_CLEAR)
    {
      uint8 vl = strlen(value);
      if(vl == 0)
        return 1;
      if(vl > 0)
        value[vl - 1] = 0;
      upd = 1;
    }
  }
}

static int do_transdtl_halfway(sp_context* ctx, sp_card* card)
{
  int ret = 0;
  int rate = 0;
  rate = ctx->feerate_table[card->feetype - 1] - 100;
  if(card->use_passwd)
  {
    if(rate == 0)//��û���ۿ�Ҳû�д���
      ctx->exception_record.transflag = TRANSFLAG_HALFFWAY_NORMAL_WITH_PASSWD;
    else if(rate > 0) //���ۿ�
    {
      ctx->exception_record.transflag = TRANSFLAG_HALFFWAY_DISCOUNT_WITH_PASSWD;
    }
    else//������
    {
      ctx->exception_record.transflag = TRANSFLAG_HALFFWAY_BOARD_WITH_PASSWD;
    }
  }
  else
  {
    if(rate == 0)//��û���ۿ�Ҳû�д���
      ctx->exception_record.transflag = TRANSFLAG_HALFFWAY_NORMAL_WITHOUT_PASSWD;
    else if(rate > 0) //���ۿ�
    {
      ctx->exception_record.transflag = TRANSFLAG_HALFFWAY_DISCOUNT_WITHOUT_PASSWD;
    }
    else//������
    {
      ctx->exception_record.transflag = TRANSFLAG_HALFFWAY_BOARD_WITHOUT_PASSWD;
    }
  }
  ret = prepare_transdtl(ctx, card, &(ctx->exception_record));
  if(ret)
    return ret;
  ret = sp_write_transdtl(&(ctx->exception_record));//��һ�ʺڿ���ˮ
  if(ret)
  {
    sp_disp_msg(30, "������ˮʧ��,ret=%d", ret);
    return ret;
  }
  return 0;
}

static int do_purchase_wirte_card(sp_context* p_ctx, sp_card* card)
{
  int ret;
  ret = sp_payinit(p_ctx, card, card->payamt);
  if(ret)
  {
    sp_disp_error("pay init fail");
    return ret;
  }
  //sp_disp_debug("finish pay init=%d",1);
  // pay debit
  ret = sp_payment(p_ctx, card, card->payamt);
  if(ret)
  {
    //����������д���ױ�־
    p_ctx->record.transflag = TRANSFLAG_UNCONFIRM;
    //��һ����;�ο���ˮ
    card->is_halfway = 1;
    return ret;
  }
  else
  {
    //sp_disp_debug("pay confirm success,tac=%02x%02x%02x%02x", p_card->tac[0], p_card->tac[1], p_card->tac[2], p_card->tac[3]);
    p_ctx->record.transflag = TRANSFLAG_OK;
  }
  return 0;
}

static int do_wait_reload_card(sp_context* p_ctx, sp_card* card)
{
  sp_card this_card;
  int ret = 0;
  while(sp_request_card_poweron(&this_card))
  {
    if(SP_KEY_CLEAR == sp_get_key())
      return SP_CANCEL_CONSUME;
  }
  //Ѱ�������ж����ǲ���ͬһ�ſ�
  if(!SP_IS_SAME_CARDPHYID(this_card.cardphyid, card->cardphyid))
  {
    return SP_NOT_SAME_CARD;
  }
  ret = sp_select_card_app(p_ctx, card);
  if(ret)
  {
    sp_disp_debug("select card app error");
    return ret;
  }
  ret = sp_get_prove(p_ctx, card, ++card->paycnt);
  if(ret)
  {
    return SP_NOT_GET_PROVE;
  }
  return 0;
}
static uint8 send_transdtl_to_server(sp_transdtl* ptransdtl)
{
  sp_tcp_trans_unit tcp_struct;
  // sp_disp_debug("transdtl lasttermno[%02x%02x]", ptransdtl->lasttermno[4], ptransdtl->lasttermno[5]);
  memset(&tcp_struct, 0, sizeof(sp_tcp_trans_unit));
  tcp_struct.header.cmd_code = 0x01;//�����ϴ�������ˮ
  memcpy(tcp_struct.data, ptransdtl, sizeof(sp_transdtl));
  tcp_struct.header.data_len = sizeof(sp_transdtl);
  return sp_send_struct(&tcp_struct);
}

static int do_purchase(sp_context* p_ctx, sp_card* card)
{
  int ret;
  bool has_save_transdtl = false;
  while(1)
  {
    ret = do_purchase_wirte_card(p_ctx, card);
    if(ret)
    {
      //������������Ҫ
      if(E_INSUFFICIENT_BALANCE == ret)
        return ret;
      //��¼һ����;�ο���ˮ
      if(has_save_transdtl == false)
      {
        if(card->is_halfway)
        {
          //��һ����;�ο���ˮ
          ret = do_transdtl_halfway(p_ctx, card);
          if(!ret)
          {
            send_transdtl_to_server(&p_ctx->exception_record);
            has_save_transdtl = true;
          }
        }
      }
      sp_disp_msg(30, "�����·ſ�");
      ret = do_wait_reload_card(p_ctx, card);
      // success,  ��¼�ɹ�������ˮһ��
      if(SP_SUCCESS == ret)
        break;
      sp_disp_debug("do_wait_reload_card,ret=%04x", ret);
      // �û�ȡ���� return
      if(SP_CANCEL_CONSUME == ret)
      {
        sp_disp_msg(30, "�û�ȡ��");
        return ret;
      }
      //��δ�۳ɹ�������
      if(SP_NOT_GET_PROVE == ret)
        continue;
    }
    else
      break;
  }
  //  ��¼�ɹ�������ˮһ��
  return 0;
}
static int check_and_input_money(sp_context* ctx, uint32* money, sp_card* p_card)
{
  int32 key = 0;
  char value[20] = {0};
  char temp[20];
  uint8 req_card_phyid[4];
  uint8 t;
  while(1)
  {
    // Ѱ������ҲҪreturn
    if(sp_request_card(req_card_phyid))
    {
      //sp_disp_error("Ѱ���������˳�");
      return SP_CARD_NOT_FOUND;
    }
    // �ж��ǲ���ͬһ�ſ�
    if(!SP_IS_SAME_CARDPHYID(req_card_phyid, p_card->cardphyid))
    {
      sp_disp_msg(10, "����ͬһ�ſ�");
      return SP_NOT_SAME_CARD;
    }
    key = sp_get_key();
    if(!sp_is_number_key(key, &t))
    {
      if(t == SP_KEY_DOT)
      {
        memcpy(value, "0.", 2);
      }
      else
      {
        strcpy(temp, value);
        sprintf(value, "%s%d", temp, t);
      }
      break;
    }
  }
  //������
  return do_input_money(ctx, money, p_card->cardphyid, value);
}

static int do_transdtl_blackcard(sp_context* ctx, sp_card* the_card)
{
  int ret = 0;
  // black card
  sp_disp_msg(100, "�ÿ��Ǻڿ�");
  sp_set_card_loss(ctx, the_card);
  ctx->exception_record.transflag = TRANSFLAG_LOSTCARD;
  ret = prepare_transdtl(ctx, the_card, &(ctx->exception_record));
  if(ret)
    return ret;
  ret = sp_write_transdtl(&(ctx->exception_record));//��һ�ʺڿ���ˮ
  if(ret)
  {
    sp_disp_error("������ˮʧ��,ret=%d", ret);
    return ret;
  }
  ret = send_transdtl_to_server(&ctx->exception_record);
  if(ret)
  {
    sp_disp_error("�ϴ��ڿ�������ˮʧ��,ret=%d", ret);
    return ret;
  }
  return 0;

}

static int do_prepare_pay_with_card(sp_context* ctx, sp_card* the_card)
{
  int ret;
  uint32 amount;

  ret = sp_read_card(ctx, the_card, SP_READ_FILE10 | SP_READ_FILE12 | SP_READ_FILE15 | SP_READ_FILE16 | SP_READ_CARDBAL);
  if(ret)
  {
    sp_halt(ctx, the_card);
    return ret;
  }
  // TODO : �жϺڿ�ǰ�Ƶ������������Ǻڿ����ϼ�һ�ʺڿ����ѵ���ˮ������
  if(do_check_blkcard(ctx, the_card))
  {
    ret = do_transdtl_blackcard(ctx, the_card);
    if(ret)
    {
      // TODO: ����
    }
    return SP_RETURN_BLACKCARD_CONSUME;
  }
  //��ʾ�����û�������ѧ��
  sp_disp_by_type(SP_TP_USER_BALANCE, ctx, the_card, NULL);
  if(check_and_input_money(ctx, &amount, the_card))
    return SP_CANCEL_CONSUME;
  //sp_disp_debug("%d,%8x", money, money);
  the_card->amount = amount;
  return 0;
}
static int check_number_key(uint8* t)
{
  int key;
  key = sp_get_key();
  if(key == SP_KEY_FUNC)
    return SP_RETURN_FUNC;
  //sp_disp_debug("key=%02x,0=%02x,9=%02x", key, SP_KEY_0, SP_KEY_9);
  //if(sp_is_number_key(key, &t))
  if(sp_is_number_key(key, t))
    return SP_NO_NUMBER_KEY_PRESSED;
  else
    return 0;
}
static int do_prepare_pay_without_card(sp_context* ctx, sp_card* the_card)
{
  int ret = 0;
  uint8 t;
  uint32 amount = 0;
  ret = check_number_key(&t);
  if(ret)
    return ret;
  //������
  if(do_input_money_without_card(ctx, &amount, t))
    return SP_CANCEL_CONSUME;
  the_card->amount = amount;
  sp_disp_by_type(SP_TP_REQUEST_CONSUME_CARD, ctx, the_card, NULL);
  //���ǰ�������������һֱѰ��
  while(sp_request_card_poweron(the_card))
  {
    if(SP_KEY_CLEAR == sp_get_key())
      return SP_CANCEL_CONSUME;
  }
  ret = sp_read_card(ctx, the_card, SP_READ_FILE10 | SP_READ_FILE12 | SP_READ_FILE15 | SP_READ_CARDBAL);
  if(ret)
  {
    sp_halt(ctx, the_card);
    return ret;
  }
  // TODO : �жϺڿ�ǰ�Ƶ������������Ǻڿ����ϼ�һ�ʺڿ����ѵ���ˮ������
  if(do_check_blkcard(ctx, the_card))
  {
    ret = do_transdtl_blackcard(ctx, the_card);
    if(ret)
    {
      // TODO: ����
    }
    return SP_RETURN_BLACKCARD_CONSUME;
  }
  //sp_request_card(the_card->cardphyid);
  return 0;
}

/*
*  ����ִ�в���:
*   1���ȼ�һ�����ˮ
*  2�����ѳ�ʼ���Ѿ�����ȷ�ϣ�����ȷ�ϻ᷵��tac��
*   3����������ȷ�ϳɹ�����¼ʣ�ಿ��(�������9�ֽ�)
*/
static int do_purchase_and_write_transdtl(sp_context * ctx, sp_card* the_card)
{
  int ret = 0;
  int32 last_transdtl_addr = 0;
  ret = prepare_transdtl(ctx, the_card, &ctx->record);
  if(ret)
    return ret;
  ret = sp_write_transdtl(&ctx->record);
  if(ret)
  {
    sp_disp_error("������ˮʧ��,ret=%d", ret);
    return ret;
  }
  last_transdtl_addr = sp_get_last_transaddr();//������ˮ�ĵ�ַ
  the_card->is_halfway = 0;//��;�ο����
  ret = do_purchase(ctx, the_card);
  if(ret)
  {
    sp_beep(CONSUME_FAIL_BEEP);
    return ret;
  }
  //12�������9���ֽ�
  ret = sp_update_left_transdtl_info(last_transdtl_addr, &(ctx->record));
  if(ret)
  {
    sp_disp_error("�������Ÿ��ֽ�ʧ�ܣ�ret=%04x", ret);
    return ret;
  }
  //  sp_disp_debug( "������ˮ�ɹ�,��ˮ��=%d", transdtl.termseqno);
  return 0;
}



static int do_calc_paymoney(sp_context* ctx, sp_card* card)
{
  int rate = 0;
  rate = ctx->feerate_table[card->feetype - 1];
  card->payamt = sp_d2i((card->amount * rate) * 1.0 / 100.0);
  return 0;
}

static void do_check_and_add_day_sum_amt(sp_context* ctx, sp_card* the_card)
{
  //������ڸ�������������һ�ν�������
  //sp_disp_debug("lastdate=[%02x%02x%02x%02x]", the_card->last_trans_date[0], the_card->last_trans_date[1], the_card->last_trans_date[2], the_card->last_trans_date[3]);
  if(memcmp(the_card->last_trans_date, ctx->today, 4) != 0)
  {
    memcpy(the_card->last_trans_date, ctx->today, 4);
    the_card->day_sum_amt = 0;
    sp_disp_debug("today=[%02x%02x%02x%02x],reset date", ctx->today[0], ctx->today[1], ctx->today[2], ctx->today[3]);
  }
  the_card->day_sum_amt += the_card->payamt;
}

static int do_purchase_with_card(sp_context* ctx, sp_card* the_card, uint8 card_reset_flag)
{
  int ret;
  sp_card current_card;
  memset(&current_card, 0, sizeof current_card);
  /*
  //�۷Ѳ���(��Ҫ������һ������)
  ret = sp_request_card_poweron(&current_card);
  if(ret)
  {
    sp_disp_error("����Ѱ��ʧ��");
    return SP_CARD_NOT_FOUND;
  }
  if(!SP_IS_SAME_CARDPHYID(the_card->cardphyid, current_card.cardphyid))
  {
    return SP_NOT_SAME_CARD;
  }
  */
  if(card_reset_flag)
  {
    ret = sp_cpu_reset_card();
    if(ret)
    {
      sp_disp_error("sp_cpu_reset_card error,ret=%04x", ret);
      return ret;
    }
    SP_CLS(0);
    SP_PRINT(0, 0, "���Ѱ����ɹ�!!");
  }

  if(the_card->cardbefbal <= ctx->syspara.min_cardbal)
  {
    //��ʾ�ÿ�����
    sp_disp_by_type(SP_TP_INSUFFICIENT_BALANCE, ctx, the_card, NULL);
    return SP_MIN_CARD_BALANCE;
  }
  ret = sp_select_card_app(ctx, the_card);
  if(ret)
    return ret;
  //�жϿ�״̬
  if(CARD_STATUS_NORMAL != the_card->status)
  {
    return SP_CARD_STATUS_ERROR;
  }
  // DO:�����ۿۺ����ʵ���ѽ��
  ret = do_calc_paymoney(ctx, the_card);
  if(ret)
  {
    sp_disp_error("�����ۿ�ʧ��, ret=%04x", ret);
    return ret;
  }

  // DO:  �ж������޶�
  ret = do_check_consume_limit(ctx, the_card);
  if(ret)
  {
    sp_disp_error("�����޶ret=%04x", ret);
    return ret;
  }

  //���Ѳ�����ˮ
  ret = do_purchase_and_write_transdtl(ctx, the_card);
  if(ret)
  {
    sp_disp_error("write card error,ret= %04x", ret);
    return ret;
  }

  do_check_and_add_day_sum_amt(ctx, the_card);
  // TODO:���¿������Ϣ
  ret = sp_do_write_card(the_card, ctx, &(ctx->record));
  if(ret)
  {
    Beep(100);//����
    sp_disp_msg(30, "������������ˢ��");
    // TODO: �ȴ��û�ˢ����Ѱ���ɹ���д�����0x10�ļ�����Ϣ
  }
  else
  {
    do_update_sum_amt(ctx, &(ctx->record));
  }
  sp_disp_by_type(SP_TP_CONSUME_SUCCESS, ctx, the_card, NULL);

  sp_beep(CONSUME_SUCCESS_BEEP);
  return 0;
}

int sp_consume_loop(sp_context* p_ctx)
{
  sp_card this_card;
  int ret = 0;
  uint8 card_reset_flag = 0;
  memset(&this_card, 0 , sizeof(this_card));
  sp_disp_by_type(SP_TP_REQUEST_CARD, p_ctx, &this_card, NULL);

  //�Ƿ�Ѱ��������������
  ret = sp_request_card_poweron(&this_card);
  if(ret)
  {
    //sp_disp_debug("before type");
    ret = do_prepare_pay_without_card(p_ctx, &this_card);
    card_reset_flag = 0;
  }
  else
  {
    ret = do_prepare_pay_with_card(p_ctx, &this_card);
    card_reset_flag = 1;
  }
  if(ret)
  {
    return ret;
  }
  ret = do_purchase_with_card(p_ctx, &this_card, card_reset_flag);
  //����ret�����;����Ƿ��ϴ���ˮ
  if(SP_SUCCESS == ret)
  {
    ret = send_transdtl_to_server(&p_ctx->record);
    if(ret)
    {
      sp_disp_error("�ϴ���ˮʧ��,ret=%d", ret);
      return ret;
    }
    //�����ڴ��е���ˮ�洢��
    memset(&p_ctx->record, 0, sizeof(sp_transdtl));
    memset(&p_ctx->exception_record, 0, sizeof(sp_transdtl));
  }
  else
  {
    if(E_INSUFFICIENT_BALANCE == ret)
    {
      //��ʾ����
      sp_disp_by_type(SP_TP_INSUFFICIENT_BALANCE, p_ctx, &this_card, NULL);
    }
  }
  return ret;
}
