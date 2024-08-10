// #include "btl_ts.h"
#include "debugger.h"
#include "common.h"

static int btl_i2c_transfer(unsigned char i2c_addr, unsigned char *buf, int len,unsigned char rw)
{
    int ret;

    switch(rw)
    {
        case I2C_WRITE:
            ret = BTL_FLASH_I2C_WRITE(i2c_addr, buf, len);
            break;
        case I2C_READ:
            ret = BTL_FLASH_I2C_READ(i2c_addr, buf, len);
            break;
    }

    if(ret)
    {
        BTL_DEBUG("btl_i2c_transfer:i2c transfer error___\n");
        return -1;
    }

    return 0;
}

static int btl_read_fw(unsigned char i2c_addr,unsigned char reg_addr, unsigned char *buf, int len)
{
    int ret;

    ret = BTL_FLASH_I2C_WRITE(i2c_addr, &reg_addr, 1);
    if(ret)
    {
        goto IIC_COMM_ERROR;
    }

    ret = BTL_FLASH_I2C_READ(i2c_addr, buf, len);
    if(ret)
    {
        goto IIC_COMM_ERROR;
    }

IIC_COMM_ERROR:
    if(ret)
    {
        BTL_DEBUG("btl_read_fw:i2c transfer error___\n");
        return -1;
    }

    return 0;
}



#ifdef BTL_ESD_PROTECT_SUPPORT
static void btl_esd_recovery(void)
{
#ifdef BTL_POWER_CONTROL_SUPPORT
    btl_reset_powerup();
#else

#ifdef RESET_PIN_WAKEUP
    btl_reset_wakeup();
#endif

#ifdef INT_PIN_WAKEUP
    btl_int_wakeup();
#endif

#endif
}
void btl_esd_check_handler(void)
{
    int ret = 0;
    unsigned char buf[4] = {0x00};
    unsigned char curBuf[4] = {0x00};

    BTL_DEBUG("btl_esd_check_handler start\n");

    ret = btl_read_fw(BTL_SLAVE_ADDR, BTL_ESD_REG, buf,sizeof(buf));
    if(ret < 0)
    {
        BTL_DEBUG("i2c module abnormal need recovery!\n");
        btl_esd_recovery();
    }
    else
    {
        BTL_DEBUG("esd buf value %x %x %x %x!\n",buf[0], buf[1], buf[2], buf[3]);
        MDELAY(50);
        ret = btl_read_fw(BTL_SLAVE_ADDR, BTL_ESD_REG, curBuf,sizeof(curBuf));
        if(ret < 0)
        {
            BTL_DEBUG("i2c module abnormal need recovery!\n");
            btl_esd_recovery();
        }
        else
        {
            BTL_DEBUG("esd curBuf value %x %x %x %x!\n",curBuf[0], curBuf[1], curBuf[2], curBuf[3]);
            if(memcmp(curBuf, buf, sizeof(buf)) == 0)
            {
            	BTL_DEBUG("IC abnormal need recovery!\n");
            	btl_esd_recovery();		
            }
        }
    }

    BTL_DEBUG("btl_esd_check_handler end\n");
}
#endif

static int btl_soft_reset_switch_int_wakemode(void)
{
    unsigned char cmd[4];
    int ret = 0x00;

    cmd[0] = RW_REGISTER_CMD;
    cmd[1] = ~cmd[0];
    cmd[2] = 0xe7;
    cmd[3] = 0xe8;
	
    ret = btl_i2c_transfer(BTL_SLAVE_ADDR, cmd,4,I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("btl_soft_reset_switch_int_wakemode failed:i2c write flash error___\n");
    }

    return ret;
}

#if(UPDATE_MODE == I2C_UPDATE_MODE)
void btl_enter_update_with_i2c(void)
{
    unsigned char buf[4] = {0x63,0x75,0x69,0x33};
    unsigned char cmd[16] = {0};
    int i = 4;
    int ret = 0;

    for(i=0;i<4;i++) 
    {
        cmd[4 * i]     = buf[0];
        cmd[4 * i + 1] = buf[1];
        cmd[4 * i + 2] = buf[2];
        cmd[4 * i + 3] = buf[3];
    }

    ret = btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("btl_enter_update_with_i2c failed:send i2c cmd error___\r\n");
        goto error;
    }
    MDELAY(50);

error:
    return ;
}

void btl_exit_update_with_i2c(void)
{
    int ret = 0;
    unsigned char cmd[2] = {0x5a, 0xa5};

    MDELAY(20);
    ret = btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("btl_exit_update_with_i2c failed:send 5a a5 error___\n");
    }

    MDELAY(20);

#if defined(RESET_PIN_WAKEUP)
    btl_reset_wakeup();
    MDELAY(30);
#endif
}
#endif

#if(UPDATE_MODE == INT_UPDATE_MODE)
void btl_enter_update_with_int(void)
{
    btl_set_int_level(0);

#if defined(RESET_PIN_WAKEUP)
    btl_reset_wakeup();
#endif

    btl_soft_reset_switch_int_wakemode();
    MDELAY(50);
}

void btl_exit_update_with_int(void)
{
    MDELAY(20);
    btl_set_int_level(1);
    MDELAY(20);

#if defined(RESET_PIN_WAKEUP)
    btl_reset_wakeup();
#endif
}
#endif

#if defined(BTL_CHECK_CHIPID)
int btl_get_chip_id(unsigned char *buf)
{
    unsigned char cmd[3];
    int ret = 0x00;
    unsigned char retry = 3;

    BTL_DEBUG("btl_get_chip_id\n");
 
    cmd[0] = RW_REGISTER_CMD;
    cmd[1] = ~cmd[0];
    cmd[2] = 0xe7;

    ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR, cmd,3,I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("btl_get_chip_id:i2c write flash error___\n");
    }

    ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR, buf,1,I2C_READ); 
    if(ret < 0)
    {
        BTL_DEBUG("btl_get_chip_id:i2c read flash error___\n");
    }

    return ret;
}
#endif

static int btl_check_version(unsigned char *pFwData, unsigned char *version)
{
    if((version[0] == pFwData[BTL_ARGUMENT_BASE_OFFSET + BTL_FWVER_MAIN_OFFSET])
	   &&(version[1] == pFwData[BTL_ARGUMENT_BASE_OFFSET + BTL_FWVER_ARGU_OFFSET])
	   &&(version[2] == pFwData[BTL_ARGUMENT_BASE_OFFSET + BTL_PROJECT_ID_OFFSET])
	   &&(version[3] == pFwData[BTL_ARGUMENT_BASE_OFFSET + BTL_FWVER_DEBUG_OFFSET]))
    {
        BTL_DEBUG("btl_check_version check PASS\n"); 
		return 0;
    }
    else
    {
        BTL_DEBUG("btl_check_version check FIAL\n"); 
		return -1;
    }
}

int btl_get_fwArgPrj_id(unsigned char *buf)
{
    BTL_DEBUG("btl_get_fwArgPrj_id\n");
    return btl_read_fw(BTL_SLAVE_ADDR,FwVersionReg, buf, 4);
}

void btl_get_key_value(unsigned char *value)
{
    btl_read_fw(BTL_SLAVE_ADDR, KeyValueReg, value, 1);
}

void btl_set_scan_mode(unsigned char mode)
{
    unsigned char cmd[2] = {0x00};

    cmd[0] = ScanCtrlReg;
    cmd[1] = mode;
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

unsigned char btl_get_scan_mode(void)
{
    unsigned char mode = 0;

    btl_read_fw(BTL_SLAVE_ADDR, ScanCtrlReg, &mode, 1);
    return mode;
}

void btl_set_channel_select(unsigned int Channel_Sel)
{
    unsigned char data[5] = {0x00};
    unsigned char temp[4] = {0x00};

    if(Channel_Sel >= (1 << MAX_CHANNEL_NUM))
    {
        BTL_DEBUG("btl_set_channel_sel:channel select is over MAX_CHANNEL_NUM\n");
        return;
    }

    data[0] = ChannelSelReg;
    data[1] = Channel_Sel & 0xff;
    data[2] = (Channel_Sel >> 8) & 0xff;
    data[3] = (Channel_Sel >> 16) & 0xff;
    data[4] = (Channel_Sel >> 24) & 0xff;

    btl_i2c_transfer(BTL_SLAVE_ADDR, data, sizeof(data), I2C_WRITE);
    btl_read_fw(BTL_SLAVE_ADDR, ChannelSelReg, temp, sizeof(temp));
    if(memcmp(temp, &data[1], sizeof(temp)) != 0)
    {
        BTL_DEBUG("Channel select write Fail\n");
        return;
    }

    BTL_DEBUG("Channel select write Ok\n");
}

void btl_set_work_mode_gain_adj(unsigned char *gain)
{
    unsigned char cmd[MAX_CHANNEL_NUM+1] = {0x00};

    cmd[0] = Ch01CfAdjReg;
    memcpy(&cmd[1],gain,MAX_CHANNEL_NUM);
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

void btl_get_work_mode_gain_adj(unsigned char *gain)
{
    btl_read_fw(BTL_SLAVE_ADDR, Ch01CfAdjReg, gain, MAX_CHANNEL_NUM);
}

void btl_set_low_power_mode_gain(unsigned char *gain)
{
    unsigned char cmd[5] = {0x00};

    cmd[0] = Sc3CfReg;
    cmd[1] = gain[0];
    cmd[2] = gain[1];
    cmd[3] = gain[2];
    cmd[4] = gain[3];
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

void btl_get_low_power_mode_gain(unsigned char *gain)
{
    btl_read_fw(BTL_SLAVE_ADDR, Sc3CfReg, gain, 4);
}

void btl_soft_reset(void)
{
    unsigned char cmd[2] = {SResetReg, SResetReg};

    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

short* btl_get_key_diff_value(unsigned char keyNum)
{
    unsigned char data[MAX_CHANNEL_NUM * 2] = {0};
    short diff[MAX_CHANNEL_NUM] = {0};
    unsigned char i = 0;

    btl_read_fw(BTL_SLAVE_ADDR, KeyDiffReg, data, keyNum * 2);

    for(i=0;i<keyNum;i++)
    {
        diff[i] = (short)((data[2 * i + 1] << 8) | data[2 * i]);
    }

    return diff;
}

void btl_set_low_power_mode(unsigned char mode)
{
    unsigned char cmd[2] = {0x00};

    cmd[0] = LowPowerEnReg;
    cmd[1] = mode;
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

unsigned char btl_get_low_power_mode(void)
{
    unsigned char mode = 0;

    btl_read_fw(BTL_SLAVE_ADDR, LowPowerEnReg, &mode, 1);
    return mode;
}

void btl_set_auto_enter_low_power_mode_time(unsigned char second)
{
    unsigned char cmd[2] = {0x00};

    cmd[0] = EnterTimeReg;
    cmd[1] = second;
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

unsigned char btl_get_auto_enter_low_power_mode_time(void)
{
    unsigned char second = 0;

    btl_read_fw(BTL_SLAVE_ADDR, EnterTimeReg, &second, 1);
    return second;
}

void btl_set_low_power_mode_idle_time(unsigned char timems)
{
    unsigned char cmd[2] = {0x00};

    cmd[0] = IdleTimeReg;
    cmd[1] = timems;

    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

unsigned char btl_get_low_power_mode_idle_time(void)
{
    unsigned char timems = 0;

    btl_read_fw(BTL_SLAVE_ADDR, IdleTimeReg, &timems, 1);
    return timems;
}

void btl_set_lp_scan_mode(unsigned char mode)
{
    unsigned char cmd[2] = {0x00};

    cmd[0] = MntScanModeReg;
    cmd[1] = mode;
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

unsigned char btl_get_lp_scan_mode(void)
{
    unsigned char mode = 0;

    btl_read_fw(BTL_SLAVE_ADDR, MntScanModeReg, &mode, 1);
    return mode;
}

void btl_calibrate(void)
{
    unsigned char cmd[2] = {0x00};
    int retry = 10;
    unsigned char timeout = 50;
    unsigned char flag = 0;

    cmd[0] = ClbEnReg;
    cmd[1] = 0x01;

    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
    while(retry--)
    {

        btl_read_fw(BTL_SLAVE_ADDR, ClbEnReg, &flag, 1);
        if(flag)
        {
            MDELAY(timeout);
        }
        else
        {
            break;
        }
    }

    if(retry < 0)
    {
        BTL_DEBUG("Calibrate IC failed!\n");
    }
    else
    {
        BTL_DEBUG("Calibrate IC Ok:%d!\n", retry);
    }
}

unsigned char btl_get_calibrate_state(void)
{
    unsigned char flag = 0;

    btl_read_fw(BTL_SLAVE_ADDR, &ClbEnReg, &flag, 1);
    return flag;
}

/******************************************
�˳��͹���ģʽ���ж�Դ����:
Bit7~Bit3:��Ч
Bit2:TIMER2����(����Ϊ1)
Bit1:INT����(1:����;0:�ر�)
Bit0:IIC����(1:����;0:�ر�)
******************************************/
void btl_set_wakeup_mode(unsigned char mode)
{
    unsigned char cmd[2] = {0x00};

	cmd[0] = WakeupModeReg;
	cmd[1] = mode;
	btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

void btl_get_wakeup_mode(void)
{
    unsigned char mode = 0x00;
	btl_read_fw(BTL_SLAVE_ADDR, WakeupModeReg, &mode, 1);
	return mode;
}

#ifdef BTL_LED_CTRL_SUPPORT
void btl_set_led_ctrl_en(unsigned char en)
{
    unsigned char cmd[2] = {0x00};

    if (en)
    {
        en = 1;
    }

    cmd[0] = LedCtrlEnReg;
    cmd[1] = en;
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

unsigned char btl_get_led_ctrl_en(void)
{
    unsigned char en = 0;

    btl_read_fw(BTL_SLAVE_ADDR, LedCtrlEnReg, &en, 1);
    return en;
}

void btl_set_led_tk_pin_map(unsigned int ledtkpinmap)
{
    unsigned char cmd[5] = {0x00};

    cmd[0] = LedTkPinMapReg;
    cmd[1] = ledtkpinmap & 0xff;
    cmd[2] = (ledtkpinmap >> 8) & 0xff;
    cmd[3] = (ledtkpinmap >> 16) & 0xff;
    cmd[4] = (ledtkpinmap >> 24) & 0xff;
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

void btl_get_led_tk_pin_map(unsigned char *ledtkpinmap)
{
    btl_read_fw(BTL_SLAVE_ADDR, LedTkPinMapReg, ledtkpinmap, 4);
}

void btl_set_led_ratio(unsigned char *Ratio)
{
    unsigned char cmd[MAX_CHANNEL_NUM-1] = {0x00};

    cmd[0] = LedxRatioReg;
    memcpy(&cmd[1],Ratio,MAX_CHANNEL_NUM-2);
    btl_i2c_transfer(BTL_SLAVE_ADDR, cmd, sizeof(cmd), I2C_WRITE);
}

void btl_get_led_ratio(unsigned char *Ratio)
{
    btl_read_fw(BTL_SLAVE_ADDR, LedxRatioReg, Ratio, MAX_CHANNEL_NUM-2);
}
#endif

// #ifdef BTL_UPDATE_FIRMWARE_ENABLE
#include "fw.h"

static int btl_get_fw_checksum(unsigned short *fw_checksum)
{
    unsigned char buf[3];
    unsigned char checksum_ready = 0;
    int retry = 5;
    int ret = 0x00;

    BTL_DEBUG("btl_get_fw_checksum\n");

    buf[0] = CHECKSUM_CAL_REG;
    buf[1] = CHECKSUM_CAL;
    ret = btl_i2c_transfer(BTL_SLAVE_ADDR, buf,2,I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("btl_get_fw_checksum:write checksum cmd error___\n");
        return -1;
    }
    MDELAY(FW_CHECKSUM_DELAY_TIME);

    ret = btl_read_fw(BTL_SLAVE_ADDR,CHECKSUM_REG, buf, 3);
    if(ret < 0)
    {
        BTL_DEBUG("btl_get_fw_checksum:read checksum error___\n");
        return -1;
    }

    checksum_ready = buf[0];

    while((retry--) && (checksum_ready != CHECKSUM_READY))
    {
        MDELAY(50);
        ret = btl_read_fw(BTL_SLAVE_ADDR,CHECKSUM_REG, buf, 3);
        if(ret < 0)
        {
            BTL_DEBUG("btl_get_fw_checksum:read checksum error___\n");
            return -1;
        }

        checksum_ready = buf[0];
    }

    if(checksum_ready != CHECKSUM_READY)
    {
        BTL_DEBUG("btl_get_fw_checksum:read checksum fail___\n");
        return -1;
    }
    *fw_checksum = (buf[1]<<8)+buf[2];

    return 0;
}

static void btl_get_fw_bin_checksum_for_self_ctp(unsigned char *fw_data,unsigned short *fw_bin_checksum, int fw_size, int specifyArgAddr)
{
    int i = 0;
    int temp_checksum = 0x0;

    for(i = 0; i < BTL_ARGUMENT_BASE_OFFSET; i++)
    {
        temp_checksum += fw_data[i];
    }
    for(i = specifyArgAddr; i < specifyArgAddr + VERTIFY_START_OFFSET; i++)
    {
        temp_checksum += fw_data[i];
    }
    for(i = specifyArgAddr + VERTIFY_START_OFFSET; i < specifyArgAddr + VERTIFY_START_OFFSET + 4; i++)
    {
        temp_checksum += fw_data[i];
    }
    for(i = BTL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET + 4; i < fw_size; i++)
    {
        temp_checksum += fw_data[i];
    }

    for(i = fw_size; i < MAX_FLASH_SIZE; i++)
    {
        temp_checksum += 0xff;
    }
    
    *fw_bin_checksum = temp_checksum & 0xffff;
}

static int btl_erase_flash(void)
{
    unsigned char cmd[2];

    BTL_DEBUG("btl_erase_flash\n");

    cmd[0] = ERASE_ALL_MAIN_CMD; 
    cmd[1] = ~cmd[0];

    return btl_i2c_transfer(BTL_FLASH_I2C_ADDR,cmd, 0x02,I2C_WRITE);	
}

static int btl_write_flash_no_blm18(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    unsigned char cmd_buf[6+FLASH_WSIZE];
    unsigned int flash_end_addr;
    int ret;
		
    BTL_DEBUG("btl_write_flash_no_blm18\n");
	
    if(!len)
    {
        BTL_DEBUG("___write flash len is 0x00,return___\n");
        return -1;	
    }

    flash_end_addr = flash_start_addr + len -1;

    if(flash_end_addr >= MAX_FLASH_SIZE)
    {
        BTL_DEBUG("___write flash end addr is overflow,return___\n");
        return -1;	
    }

    cmd_buf[0] = cmd;
    cmd_buf[1] = ~cmd;
    cmd_buf[2] = flash_start_addr >> 0x08;
    cmd_buf[3] = flash_start_addr & 0xff;
    cmd_buf[4] = flash_end_addr >> 0x08;
    cmd_buf[5] = flash_end_addr & 0xff;

    memcpy(&cmd_buf[6],buf,len);

    ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR,cmd_buf, len+6,I2C_WRITE);	
    if(ret < 0)
    {
        BTL_DEBUG("i2c transfer error___\n");
        return -1;
    }

    return 0;
}

static int btl_write_flash_blm18(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    unsigned char cmd_buf[8+FLASH_WSIZE];
    unsigned int flash_end_addr;
    int ret;
		
    BTL_DEBUG("btl_write_flash_blm18\n");
	
    if(!len)
    {
        BTL_DEBUG("___write flash len is 0x00,return___\n");
        return -1;	
    }

    flash_end_addr = flash_start_addr + len -1;

    if(flash_end_addr >= MAX_FLASH_SIZE)
    {
        BTL_DEBUG("___write flash end addr is overflow,return___\n");
        return -1;	
    }

    cmd_buf[0] = cmd;
    cmd_buf[1] = ~cmd;
    cmd_buf[2] = flash_start_addr >> 16;
    cmd_buf[3] = flash_start_addr >> 8;
    cmd_buf[4] = flash_start_addr & 0xff;
    cmd_buf[5] = flash_end_addr >> 16;
    cmd_buf[6] = flash_end_addr >> 8;
    cmd_buf[7] = flash_end_addr & 0xff;

    memcpy(&cmd_buf[8],buf,len);

    ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR,cmd_buf, len+8,I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("i2c transfer error___\n");
        return -1;
    }

    return 0;
}

static int btl_write_flash(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    return btl_write_flash_no_blm18(cmd, flash_start_addr, buf, len);
}

static int btl_read_flash_no_blm18(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    char ret =0;
    unsigned char cmd_buf[6];
    unsigned int flash_end_addr;

    flash_end_addr = flash_start_addr + len -1;
    cmd_buf[0] = cmd;
    cmd_buf[1] = ~cmd;
    cmd_buf[2] = flash_start_addr >> 0x08;
    cmd_buf[3] = flash_start_addr & 0xff;
    cmd_buf[4] = flash_end_addr >> 0x08;
    cmd_buf[5] = flash_end_addr & 0xff;
    ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR,cmd_buf,6,I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("btl_read_flash_no_blm18:i2c transfer write error\n");
        return -1;
    }
    	ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR,buf,len,I2C_READ);
    if(ret < 0)
    {
        BTL_DEBUG("btl_read_flash_no_blm18:i2c transfer read error\n");
        return -1;
    }

    return 0;
}

static int btl_read_flash_blm18(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    char ret =0;
    unsigned char cmd_buf[6];
    unsigned int flash_end_addr;

    flash_end_addr = flash_start_addr + len -1;
    cmd_buf[0] = cmd;
    cmd_buf[1] = ~cmd;
    cmd_buf[2] = flash_start_addr >> 16;
    cmd_buf[3] = flash_start_addr >> 8;
    cmd_buf[4] = flash_start_addr & 0xff;
    cmd_buf[5] = flash_end_addr >> 16;
    cmd_buf[6] = flash_end_addr >> 8;
    cmd_buf[7] = flash_end_addr & 0xff;
    ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR,cmd_buf,8,I2C_WRITE);
    if(ret < 0)
    {
        BTL_DEBUG("btl_read_flash_blm18:i2c transfer write error\n");
        return -1;
    }
    ret = btl_i2c_transfer(BTL_FLASH_I2C_ADDR,buf,len,I2C_READ);
    if(ret < 0)
    {
        BTL_DEBUG("btl_read_flash_blm18:i2c transfer read error\n");
        return -1;
    }

    return 0;
}

static int btl_read_flash(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    return btl_read_flash_no_blm18(cmd, flash_start_addr, buf, len);
}
static int btl_download_fw_for_self_ctp(unsigned char *pfwbin,int specificArgAddr, int fwsize)
{
    unsigned int i;
    unsigned int size,len;
    unsigned int addr;
    unsigned char verifyBuf[4] = {0xff, 0xff, 0xff, 0xff};
	unsigned char chipID = 0x00;

    BTL_DEBUG("btl_download_fw_for_self_ctp\n");
	
    verifyBuf[2] = pfwbin[BTL_ARGUMENT_BASE_OFFSET+VERTIFY_START_OFFSET+2];
    verifyBuf[3] = pfwbin[BTL_ARGUMENT_BASE_OFFSET+VERTIFY_START_OFFSET+3];	
    BTL_DEBUG("btl_download_fw:verifyBuf = %x %x %x %x\n",verifyBuf[0],verifyBuf[1],verifyBuf[2],verifyBuf[3]);
	
    if(btl_erase_flash())
    {
        BTL_DEBUG("___erase flash fail___\n");
        return -1;
    }

    MDELAY(50);

    //Write data before BTL_ARGUMENT_BASE_OFFSET
    for(i=0;i< BTL_ARGUMENT_BASE_OFFSET;)
    {
        size = BTL_ARGUMENT_BASE_OFFSET - i;
        if(size > FLASH_WSIZE)
        {
            len = FLASH_WSIZE;
        }
        else
        {
            len = size;
        }

        addr = i;
	
        if(btl_write_flash(WRITE_MAIN_CMD,addr, &pfwbin[i],len))
        {
            return -1;
        }
        i += len;
        MDELAY(5);
    }

    //Write the data from BTL_ARGUMENT_BASE_OFFSET to VERTIFY_START_OFFSET
    for(i=BTL_ARGUMENT_BASE_OFFSET;i< (VERTIFY_START_OFFSET+BTL_ARGUMENT_BASE_OFFSET);)
    {
        size = VERTIFY_START_OFFSET + BTL_ARGUMENT_BASE_OFFSET - i;
        if(size > FLASH_WSIZE)
        {
            len = FLASH_WSIZE;
        }
        else
        {
            len = size;
        }
    
        addr = i;
    
        if(btl_write_flash(WRITE_MAIN_CMD,addr, &pfwbin[i+specificArgAddr-BTL_ARGUMENT_BASE_OFFSET],len))
        {
            return -1;
        }
        i += len;
        MDELAY(5);
    }

    //Write the four bytes verifyBuf from VERTIFY_START_OFFSET
    for(i=(VERTIFY_START_OFFSET + BTL_ARGUMENT_BASE_OFFSET);i< (VERTIFY_START_OFFSET + BTL_ARGUMENT_BASE_OFFSET + sizeof(verifyBuf));)
    {
        size = VERTIFY_START_OFFSET + BTL_ARGUMENT_BASE_OFFSET + sizeof(verifyBuf) - i;
        if(size > FLASH_WSIZE)
        {
            len = FLASH_WSIZE;
        }
        else
        {
            len = size;
        }
    
        addr = i;
    
        if(btl_write_flash(WRITE_MAIN_CMD,addr, &verifyBuf[i-VERTIFY_START_OFFSET-BTL_ARGUMENT_BASE_OFFSET],len))
        {
            return -1;
        }
        i += len;
        MDELAY(5);
    }

    //Write data after verityBuf from VERTIFY_START_OFFSET + 4
    for(i=(BTL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET + 4);i< fwsize;)
    {
        size = fwsize - i;
        if(size > FLASH_WSIZE)
        {
            len = FLASH_WSIZE;
        }
        else
        {
            len = size;
        }
    
        addr = i;
    
        if(btl_write_flash(WRITE_MAIN_CMD,addr, &pfwbin[i],len))
        {
            return -1;
        }
        i += len;
        MDELAY(5);
    }

    return 0;
}

static int btl_read_vertify(void)
{
    int ret = 0;
	unsigned char cmd = 0x91;
    unsigned char vertify[2] = {0};
	unsigned char vertify1[2] = {0xaa, 0x55};

	ret = btl_read_fw(BTL_SLAVE_ADDR, cmd, vertify, sizeof(vertify));
    if(ret < 0 || memcmp(vertify, vertify1, sizeof(vertify)))
    {
        BTL_DEBUG("btl_read_vertify: read fail\n");
		return -1;
    }
    return 0;
}

static int btl_read_flash_vertify(unsigned char *pfwbin)
{
    unsigned char cnt = 0;
    int ret = 0;
    unsigned char vertify[2] = {0};
    unsigned char vertify1[2] = {0};
	
    memcpy(vertify,&pfwbin[BTL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET],sizeof(vertify));
    BTL_DEBUG("btl_read_flash_vertify: vertify:%x %x\n",vertify[0],vertify[1]);

    SET_WAKEUP_LOW;
    while(cnt < 3)
    {
        cnt++;
        ret = btl_read_flash(READ_MAIN_CMD, BTL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET, vertify1, sizeof(vertify1));
        if(ret < 0)
        {
            BTL_DEBUG("btl_write_flash_vertify: read fail\n");
            continue;
        }

        if(memcmp(vertify, vertify1, sizeof(vertify)) == 0)
        {
            ret = 0;
            break;
        }
        else
        {
            ret = -1;
        }
    }
    SET_WAKEUP_HIGH;
    return ret;
}

static int btl_write_flash_vertify(unsigned char *pfwbin)
{
    unsigned char cnt = 0;
    int ret = 0;
    unsigned char vertify[2] = {0};
    unsigned char vertify1[2] = {0};

    memcpy(vertify,&pfwbin[BTL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET],sizeof(vertify));
    BTL_DEBUG("btl_write_flash_vertify: vertify:%x %x\n",vertify[0],vertify[1]);

    SET_WAKEUP_LOW;
    while(cnt < 3)
    {
        cnt++;
        ret = btl_write_flash(WRITE_MAIN_CMD, BTL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET, vertify, sizeof(vertify));
        if(ret < 0)
        {
            BTL_DEBUG("btl_write_flash_vertify: write fail\n");
            continue;
        }
		
        MDELAY(10);

        ret = btl_read_flash(READ_MAIN_CMD, BTL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET, vertify1, sizeof(vertify1));
        if(ret < 0)
        {
            BTL_DEBUG("btl_write_flash_vertify: read fail\n");
            continue;
        }

        if(memcmp(vertify, vertify1, sizeof(vertify)) == 0)
        {
            ret = 0;
            break;
        }
    		else
        {
            ret = -1;
        }
    }
    SET_WAKEUP_HIGH;
    return ret;
}

static int btl_update_flash_for_self_ctp(unsigned char update_type, unsigned char *pfwbin,int fwsize, int specificArgAddr)
{
    int retry = 0;
    int ret = 0;
    unsigned short fw_checksum = 0x0;
    unsigned short fw_bin_checksum = 0x0;

    retry =3;
    while(retry--)
    {
        SET_WAKEUP_LOW;

        ret = btl_download_fw_for_self_ctp(pfwbin,specificArgAddr,fwsize);

        if(ret<0)
        {
            BTL_DEBUG("btl_update_flash_for_self_ctp:btl_download_fw_for_self_ctp error retry=%d\n",retry);
            continue;
        }
		
        MDELAY(50);

        SET_WAKEUP_HIGH;
	
        btl_get_fw_bin_checksum_for_self_ctp(pfwbin,&fw_bin_checksum, fwsize,specificArgAddr);
        ret = btl_get_fw_checksum(&fw_checksum);
        fw_checksum -= 0xff;
        BTL_DEBUG("btl_download_fw_for_self_ctp:fw checksum = 0x%x,fw_bin_checksum =0x%x\n",fw_checksum, fw_bin_checksum);

        if((ret < 0) || ((update_type == FW_ARG_UPDATE)&&(fw_checksum != fw_bin_checksum)))
        {
            BTL_DEBUG("btl_download_fw_for_self_ctp:btl_get_fw_checksum error");
            continue;
        }

        if((update_type == FW_ARG_UPDATE)&&(fw_checksum == fw_bin_checksum))
        {
            ret = btl_write_flash_vertify(pfwbin);
            if(ret < 0)
                continue;
        }
        break;
    }

    if(retry < 0)
    {
        SET_WAKEUP_LOW;
		btl_erase_flash();
		MDELAY(50);
		SET_WAKEUP_HIGH;
        BTL_DEBUG("btl_download_fw_for_self_ctp error\n");
        return -1;
    }

    BTL_DEBUG("btl_download_fw_for_self_ctp success___\n");	
    return 0;
}

static unsigned char choose_update_type_for_self_ctp(unsigned char isBlank)
{
    unsigned char update_type = NONE_UPDATE;

    if(isBlank)
    {
        update_type = FW_ARG_UPDATE;
        BTL_DEBUG("Update case 0:FW_ARG_UPDATE\n");
    }
    else
    {
        update_type = NONE_UPDATE;
        BTL_DEBUG("Update case 1:NONE_UPDATE\n");
    }
    return update_type;
}

static int btl_update_fw_for_self_ctp(unsigned char* fw_data, int fw_size)
{
    unsigned char fwArgPrjID[4];    //firmware version/argument version/project identification
    int ret = 0x00;
    unsigned char isBlank = 0x0;    //Indicate the IC have any firmware
    unsigned short fw_checksum = 0x0;  //The checksum for firmware in IC
    unsigned short fw_bin_checksum = 0x0;  //The checksum for firmware in file
    unsigned char update_type = NONE_UPDATE;  
    unsigned char projectFlag = 0;      //protect flag

    BTL_DEBUG("btl_update_fw_for_self_ctp start\n");

    //Step 1:Obtain IC version number
    ret = btl_get_fwArgPrj_id(fwArgPrjID);
	if((btl_check_version(fw_data, fwArgPrjID) < 0) || (btl_read_vertify() < 0))
    {
        isBlank = 1;
        BTL_DEBUG("btl_update_fw_for_self_ctp:This is blank IC ret=%x fwID=%x argID=%x prjID=%x debugVer=%x\n",ret,fwArgPrjID[0],fwArgPrjID[1], fwArgPrjID[2], fwArgPrjID[3]);
    }
    else
    {
    
        isBlank = 0;
        BTL_DEBUG("btl_update_fw_for_self_ctp:ret = %x fwArgPrjID[0]=%x fwArgPrjID[1]=%x fwArgPrjID[2]=%x fwArgPrjID[3]=%x\n",ret,fwArgPrjID[0],fwArgPrjID[1], fwArgPrjID[2], fwArgPrjID[3]);
    }
	
    BTL_DEBUG("isBlank = %d  ver1 = %d ver2 = %d ver3 = %d ver4 = %d binVer1 = %d binVer2 = %d binVer3 = %d binVer4 = %d specificAddr = %x", isBlank,fwArgPrjID[0],fwArgPrjID[1],fwArgPrjID[2],fwArgPrjID[3],fw_data[BTL_ARGUMENT_BASE_OFFSET+BTL_FWVER_MAIN_OFFSET],fw_data[BTL_ARGUMENT_BASE_OFFSET + BTL_FWVER_ARGU_OFFSET],fw_data[BTL_ARGUMENT_BASE_OFFSET + BTL_PROJECT_ID_OFFSET],fw_data[BTL_ARGUMENT_BASE_OFFSET + BTL_FWVER_DEBUG_OFFSET], BTL_ARGUMENT_BASE_OFFSET);

    //Step 2:Judge update or not
    update_type = choose_update_type_for_self_ctp(isBlank);

    //Step 3:Start Update depend condition
    if(update_type != NONE_UPDATE)
    {
        ret = btl_update_flash_for_self_ctp(update_type, fw_data, fw_size,BTL_ARGUMENT_BASE_OFFSET);
        if(ret < 0)
        {
            BTL_DEBUG("btl_update_fw_for_self_ctp:btl_update_flash failed\n");
        }
    }
    BTL_DEBUG("btl_update_fw_for_self_ctp exit\n");

    return ret;
}

static int btl_update_fw(unsigned char* pFwData, unsigned int fwLen)
{
    int ret = 0;

    ret = btl_update_fw_for_self_ctp(pFwData,fwLen);
 

    return ret;
}

// #ifdef BTL_AUTO_UPDATE_FARMWARE
int btl_auto_update_fw(void)
{
    int ret = 0;
    unsigned int fwLen = sizeof(fwbin);

    BTL_DEBUG("btl_auto_update_fw:fwLen = %x\n",fwLen);

    ret = btl_update_fw((unsigned char *)fwbin, fwLen);	
    if(ret < 0)
    {
        BTL_DEBUG("btl_auto_update_fw: btl_update_fw fail\n");  
    }
    else
    {
        BTL_DEBUG("btl_auto_update_fw: btl_update_fw success\n");  
    }
    return ret;
}
// #endif
// #endif
