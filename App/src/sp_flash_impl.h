#ifndef __SP_FLASH_IMPL_H__
#define __SP_FLASH_IMPL_H__

#include "sp_info.h"
#include "sp_flash_store.h"

#define TRANSNO_FLAG_MASTER true
#define TRANSNO_FLAG_SLAVE false

//读写流水号
uint8 sp_write_transno_unit(bool flag, sp_transno_unit* punit);
uint8 sp_read_transno_unit(bool flag, sp_transno_unit* punit);
uint32 sp_get_transno(void);
int32 sp_get_last_transaddr(void);


//读写流水
uint8 sp_write_transdtl(sp_transdtl* ptransdtl);
uint8 sp_read_transdtl(sp_transdtl* ptransdtl);
uint8 sp_update_left_transdtl_info(uint32 page_addr, sp_transdtl* ptransdtl);


//读写黑名单
uint8 sp_read_blacklist(sp_context* p_ctx);
uint8 sp_write_blacklist(sp_context* p_ctx);


//读写系统信息
uint8 sp_write_syspara(sp_context* p_ctx);
uint8 sp_read_syspara(sp_context* p_ctx);

//系统信息相关
int sp_init_syspara(sp_context* p_ctx);
uint16 sp_get_transno_lastaddr(uint16 pageno);
int sp_init_transdtl(sp_context* p_ctx);
int sp_init_blacklist(sp_context* p_ctx);
void sp_disp_flash(int32 addr, int32 len);
//检查
int sp_check_date_change(sp_context* ctx);





#endif


