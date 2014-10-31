#ifndef __SP_DISP_H__
#define __SP_DISP_H__

#include "config.h"
#include "sp_info.h"
#include "sp_pubfunc.h"
#include "sp_communicate.h"

//#define SP_DEBUG_MODE 0

#ifdef SP_DEBUG_MODE
#define OUTPUT_MSG(t, fmt, ...) do{ sp_disp_msg(t, fmt, ...); }while(0)
#else
#define OUTPUT_MSG(t, fmt, ...) do{ }while(0)
#endif
/////////////////////·â×°»ã¶àÆÁÄ»ÏÔÊ¾º¯Êý////////////////////////////
#define SP_PRINT(row, col, str) do{LcdPrint(row, col, str, 0);}while(0)
#define SP_CLS(row) do{LcdCLS(row);}while(0)
#define SP_CLS_FULLSCREEN SP_CLS(0xff)




///////////////////// ²âÊÔº¯Êý///////////////////////////
void sp_disp_press_key(uint8 key_num, uint8 key_cnt);

///////////////////////////////////////////////
void sp_disp_msg(uint32 nsec, const char* fmt, ...);

void sp_disp_by_type(uint8 disp_type, sp_context* p_ctx, sp_card* p_card, void* other);

void sp_wait_for_retkey(const int32 retkey, const char* func_name,
                        const int lineno, const char* fmt, ...);

int sp_messagebox(const int box_type, const char* func_name,
                  const int lineno, const char* fmt, ...);

void sp_disp_error(const char* fmt, ...);

void sp_disp_debug(const char* fmt, ...);

void sp_wait_seconds(int wait_sec);

#endif

