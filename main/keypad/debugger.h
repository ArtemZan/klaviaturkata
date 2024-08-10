#ifndef _BTL_TS_H_ 
#define _BTL_TS_H_
// #include "lv_drv_conf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include "ioi2c.h"
// #include "gpio.h"
// #include "hal_tp.h"
// #include "hal_key.h"
// #include "SmartBand_Window.h"
// #ifdef __GYPSOPHILA_SUPPORT__
// #include "SmartBand_Gypsophila.h"
// #endif
// #include "../../board.h"
// #include "../common/i2c.h"
// #include "../common/lv_pm.h"
// #include "lvgl/lv_core/lv_group.h"
// #include "lvgl/lv_misc/lv_tsrb.h"
// #include "lvgl/lv_hal/lv_hal_indev.h"
// #include "btl_chip_common.h"


#define   MDELAY(n)	    btl_delay_ms(n)	

#ifdef BTL_POWER_CONTROL_SUPPORT
#define	CTP_LDO_PIN		GPIO10
#define CTP_SET_LDO_OUTPUT	    gpio_set_direction(CTP_LDO_PIN,GPIO_OUT)
#define CTP_SET_LDO_HIGH        GPIO_WriteIO(CTP_LDO_PIN,1)
#define CTP_SET_LDO_LOW         GPIO_WriteIO(CTP_LDO_PIN,0)
#endif

#ifdef RESET_PIN_WAKEUP
#define	CTP_RESET_PIN		GPIO19
#define CTP_SET_RESET_PIN_OUTPUT	gpio_set_direction(CTP_RESET_PIN,GPIO_OUT);
#define CTP_SET_RESET_PIN_HIGH		gpio_output_set(CTP_RESET_PIN, 1)
#define CTP_SET_RESET_PIN_LOW     gpio_output_set(CTP_RESET_PIN, 0)
#endif

#ifdef GPIO_EINT
#define  CTP_EINT_PIN			GPIO16
#define CTP_SET_I2C_EINT_OUTPUT		gpio_set_direction(CTP_EINT_PIN,GPIO_OUT)
#define CTP_SET_I2C_EINT_INPUT		gpio_set_direction(CTP_EINT_PIN,GPIO_IN)
#define CTP_SET_I2C_EINT_HIGH     gpio_output_set(CTP_EINT_PIN,1)
#define CTP_SET_I2C_EINT_LOW      gpio_output_set(CTP_EINT_PIN,0)	
#endif

#ifdef BTL_USE_SW_I2C
#define CTP_I2C_DELAY       50
#define CTP_I2C_DATA_PIN		GPIO11 
#define CTP_I2C_CLK_PIN			GPIO10 

#define CTP_WRITE           BTL_SLAVE_ADDR
#define CTP_READ            (BTL_SLAVE_ADDR+1)
#define CTP_ACK_COUNTER     10

#define CTP_SET_I2C_CLK_OUTPUT		gpio_set_direction(CTP_I2C_CLK_PIN,GPIO_OUT)
#define CTP_SET_I2C_CLK_HIGH      gpio_output_set(CTP_I2C_CLK_PIN,1)
#define CTP_SET_I2C_CLK_LOW       gpio_output_set(CTP_I2C_CLK_PIN,0)

#define CTP_SET_I2C_DATA_OUTPUT		gpio_set_direction(CTP_I2C_DATA_PIN,GPIO_OUT)
#define CTP_SET_I2C_DATA_INPUT		gpio_set_direction(CTP_I2C_DATA_PIN,GPIO_IN)
#define CTP_SET_I2C_DATA_HIGH     gpio_output_set(CTP_I2C_DATA_PIN,1)
#define CTP_SET_I2C_DATA_LOW      gpio_output_set(CTP_I2C_DATA_PIN,0)
#define CTP_GET_I2C_DATA_BIT      gpio_input_get(CTP_I2C_DATA_PIN)
h
#define CTP_I2C_START_BIT \
{ \
		volatile unsigned int j; \
		CTP_SET_I2C_CLK_OUTPUT; \
		CTP_SET_I2C_DATA_OUTPUT; \
		CTP_SET_I2C_CLK_HIGH; \
		CTP_SET_I2C_DATA_HIGH; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_DATA_LOW; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_CLK_LOW; \
}

#define CTP_I2C_STOP_BIT \
{ \
		volatile unsigned int j; \
		CTP_SET_I2C_CLK_OUTPUT; \
		CTP_SET_I2C_DATA_OUTPUT; \
		CTP_SET_I2C_CLK_LOW; \
		CTP_SET_I2C_DATA_LOW; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_CLK_HIGH; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
		CTP_SET_I2C_DATA_HIGH; \
		for(j=0;j<CTP_I2C_DELAY;j++);\
}
#endif

int BTL_FLASH_I2C_WRITE(unsigned char i2c_addr, unsigned char *value, unsigned short len);
int BTL_FLASH_I2C_READ(unsigned char i2c_addr, unsigned char *value, unsigned short len);

#if(UPDATE_MODE==I2C_UPDATE_MODE)
void btl_enter_update_with_i2c(void);
void btl_exit_update_with_i2c(void);
#endif

#if(UPDATE_MODE==INT_UPDATE_MODE)
void btl_enter_update_with_int(void);
void btl_exit_update_with_int(void);
#endif

#ifdef  RESET_PIN_WAKEUP
void btl_reset_wakeup(void);
#endif

#if(UPDATE_MODE==I2C_UPDATE_MODE)
#define   SET_WAKEUP_HIGH    btl_exit_update_with_i2c()	 
#define   SET_WAKEUP_LOW	 btl_enter_update_with_i2c()	 
#endif

#if(UPDATE_MODE==INT_UPDATE_MODE)
#define   SET_WAKEUP_HIGH    btl_exit_update_with_int()	 
#define   SET_WAKEUP_LOW	 btl_enter_update_with_int()	 
#endif

int btl_init(void);

#if defined(BTL_CHECK_CHIPID)
int btl_get_chip_id(unsigned char *buf);
#endif

int btl_get_fwArgPrj_id(unsigned char *buf);
void btl_get_key_value(unsigned char *value);
unsigned char btl_get_calibrate_state(void);
#endif



int btl_auto_update_fw(void);


