#ifndef BTL_CHIP_COMMON_H
#define BTL_CHIP_COMMON_H
// #include "btl_chip_custom.h"

#define BLM16

#if defined(BLM18P)
#define MAX_CHANNEL_NUM             15
#elif defined(BLM16)
#define MAX_CHANNEL_NUM             16
#elif defined(BLM28N)
#define MAX_CHANNEL_NUM             28
#endif

#define BTL_FLASH_I2C_ADDR		    (0x2C << 1) //8bit addr
#define I2C_WRITE                   0x00
#define I2C_READ                    0x01

#define INT_UPDATE_MODE             0x00
#define I2C_UPDATE_MODE             0x01

#define FLASH_WSIZE                 256 //0x128
#define FLASH_RSIZE                 64  //0x128
#define PROJECT_INFO_LEN            20  // ProjectInfo�ַ�����


#define BTL_FLASH_ID                0x23
#define UPDATE_MODE                 I2C_UPDATE_MODE

#define BTL_ARGUMENT_BASE_OFFSET    0x200
#define	MAX_FLASH_SIZE              0x8000
#define	PJ_ID_OFFSET                0xCB
#define	FW_CHECKSUM_DELAY_TIME      100
#define VERTIFY_START_OFFSET        0x3FC
#define VERTIFY_END_OFFSET          0x3FD

enum MOORE_PLUS_SERIES_flash_cmd {
    ERASE_SECTOR_MAIN_CMD = 0x06,
    ERASE_ALL_MAIN_CMD    = 0x09,
    RW_REGISTER_CMD       = 0x0A,
    READ_MAIN_CMD         = 0x0D,
    WRITE_MAIN_CMD        = 0x0F,
    WRITE_RAM_CMD         = 0x11,
    READ_RAM_CMD          = 0x12,
};

enum fw_reg {
    KeyValueReg         = 0x01,
    KeyModeReg          = 0x02,
    ChannelSelReg       = 0x03,
    KeyStateReg         = 0x06,
    ScanCtrlReg         = 0x08,
    VbiasSelReg         = 0x0C,
    Sc1CfReg            = 0x0D,
    Sc3CfReg            = 0x0E,
    Sc1K1REG            = 0x0F,
    ChOderReg           = 0x13,
    ChNumReg            = 0x14,
    Ch01CfAdjReg        = 0x24,
    Ch02CfAdjReg        = 0x25,
    Ch03CfAdjReg        = 0x26,
    Ch04CfAdjReg        = 0x27,
    Ch05CfAdjReg        = 0x28,
    Ch06CfAdjReg        = 0x29,
    Ch07CfAdjReg        = 0x2A,
    Ch08CfAdjReg        = 0x2B,
    Ch09CfAdjReg        = 0x2C,
    Ch010CfAdjReg       = 0x2D,
    Ch011CfAdjReg       = 0x2E,
    Ch012CfAdjReg       = 0x2F,
    Ch013CfAdjReg       = 0x30,
    Ch014CfAdjReg       = 0x31,
    Ch015CfAdjReg       = 0x32,
    Sc1DeciFactorReg    = 0x39,
    CHECKSUM_REG        = 0x3F,
    FirstThdGroupReg    = 0x43,
    JitterCntReg        = 0x45,
    IoVccLevelReg       = 0x49,
    IntModeSelReg       = 0x4A,
    I2cPullupEnReg      = 0x4B,
    I2cOdEnReg          = 0x4C,
    SResetReg           = 0x52,
    KeyDiffReg          = 0x73,
    WkupJitTimeReg      = 0x85,
    LowPowerEnReg       = 0x86,
    EnterTimeReg        = 0x87,
    IdleTimeReg         = 0x89,
    CHECKSUM_CAL_REG    = 0x8A,
    WakeupThdReg        = 0x8B,
    Sc3DeciFactorReg    = 0x8C,
    MntScanModeReg      = 0x8D,
    BaseChkTimeReg      = 0x8E,
    Sc3K1Reg            = 0x8F,    
    WakeupModeReg       = 0x90,
    ClbEnReg            = 0x9C,
    BareaChkEnReg       = 0xA0,
    BareaThdReg         = 0xA1,
    LongTchTimeReg      = 0xA2,
    MeanFltCoffReg      = 0xA3,
    PModeReg            = 0xA5,
    FwVersionReg        = 0xB6,
    LedCtrlEnReg        = 0xC6,
    LedTkPinMapReg      = 0xC7,
    LedxRatioReg        = 0xD0,
    BTL_ESD_REG         = 0xF9,
};

enum checksum {
    CHECKSUM_READY      = 0x01,
    CHECKSUM_CAL        = 0xAA,
    CHECKSUM_ARG        = 0xBA,
};

enum update_type {
    NONE_UPDATE         = 0x00,
    FW_ARG_UPDATE       = 0x01,
};

/*************Betterlife ic update***********/
// #ifdef BTL_UPDATE_FIRMWARE_ENABLE
#define    BTL_FWVER_MAIN_OFFSET    (0x2A)
#define    BTL_FWVER_ARGU_OFFSET    (0x2B)
#define    BTL_PROJECT_ID_OFFSET    (0x2C)
#define    BTL_FWVER_DEBUG_OFFSET   (0x2D)
#define    BTL_COB_ID_OFFSET        (0x34)
#define    BTL_ARGUMENT_FLASH_SIZE  (1024)
#define    FLASH_PAGE_SIZE          (512)
//Update firmware through driver probe procedure with h and c file
#define 	BTL_AUTO_UPDATE_FARMWARE
#ifdef		BTL_AUTO_UPDATE_FARMWARE

#endif
// #endif

/*************Betterlife ic debug***********/
// #if defined(BTL_DEBUG_SUPPORT)
#define BTL_DEBUG        printf
// #else
// #define BTL_DEBUG
// #endif

#define BTL_DRIVER_VERSION    "betterlife_driver_version_v1.0.0"
#endif


#define BTL_SLAVE_ADDR  0x2C
