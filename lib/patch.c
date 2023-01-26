/**************************************************************************************************

    Phyplus Microelectronics Limited confidential and proprietary.
    All rights reserved.

    IMPORTANT: All rights of this software belong to Phyplus Microelectronics
    Limited ("Phyplus"). Your use of this Software is limited to those
    specific rights granted under  the terms of the business contract, the
    confidential agreement, the non-disclosure agreement and any other forms
    of agreements as a customer or a partner of Phyplus. You may not use this
    Software unless you agree to abide by the terms of these agreements.
    You acknowledge that the Software may not be modified, copied,
    distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy
    (BLE) integrated circuit, either as a product or is integrated into your
    products.  Other than for the aforementioned purposes, you may not use,
    reproduce, copy, prepare derivative works of, modify, distribute, perform,
    display or sell this Software and/or its documentation for any purposes.

    YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
    PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
    INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
    NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
    PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
    NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
    LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
    OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
    OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

**************************************************************************************************/
#include "global_config.h"
#include "rf_phy_driver.h"
#include "clock.h"
#include "flash.h"

//-----------------------------------------------------------------------------------
//extern rom  variable
//
extern uint32_t  __initial_sp;
uint32* pGlobal_config = NULL;
extern volatile uint8 g_clk32K_config;

// global configuration in SRAM, it could be change by application
// TODO: when integrate, the global_config should be set by APP project
__ATTR_SECTION_XIP__ void init_config(void)
{
    pGlobal_config = (uint32*)(CONFIG_BASE_ADDR);
    int i;

    for (i = 0; i < 256; i ++)
        pGlobal_config[i] = 0;

    //save the app initial_sp  which will be used in wakeupProcess 20180706 by ZQ
    pGlobal_config[INITIAL_STACK_PTR] = (uint32_t)&__initial_sp;
    // LL switch setting
    pGlobal_config[LL_SWITCH] =  LL_DEBUG_ALLOW | SLAVE_LATENCY_ALLOW | LL_WHITELIST_ALLOW
                                 | SIMUL_CONN_ADV_ALLOW | SIMUL_CONN_SCAN_ALLOW; //RC32_TRACKINK_ALLOW

    if(g_clk32K_config==CLK_32K_XTAL)
        pGlobal_config[LL_SWITCH] &= 0xffffffee;
    else
        pGlobal_config[LL_SWITCH] |= RC32_TRACKINK_ALLOW | LL_RC32K_SEL;

    // sleep delay
    pGlobal_config[MIN_TIME_TO_STABLE_32KHZ_XOSC] = 10;      // 10ms, temporary set
    // system clock setting
    pGlobal_config[CLOCK_SETTING] = g_system_clk;//CLOCK_32MHZ;
    //------------------------------------------------------------------------
    // wakeup time cose
    // t1. HW_Wakeup->MCU relase 62.5us
    // t2. wakeup_process in waitRTCCounter 30.5us*[WAKEUP_DELAY] about 500us
    // t3. dll_en -> hclk_sel in hal_system_ini 100us in run as RC32M
    // t4. sw prepare cal sleep tick initial rf_ini about 300us @16M this part depends on HCLK
    // WAKEUP_ADVANCE should be larger than t1+t2+t3+t4
    //------------------------------------------------------------------------
    // wakeup advance time, in us
    pGlobal_config[WAKEUP_ADVANCE] = 1850;//650;//600;//310;

    if(g_system_clk==SYS_CLK_XTAL_16M)
    {
        pGlobal_config[WAKEUP_DELAY] = 16;
    }
    else if(g_system_clk==SYS_CLK_DBL_32M)
    {
        pGlobal_config[WAKEUP_DELAY] = 16;
    }
    else if(g_system_clk==SYS_CLK_DLL_48M)
    {
        pGlobal_config[WAKEUP_DELAY] = 16;
    }
    else if(g_system_clk==SYS_CLK_DLL_64M)
    {
        pGlobal_config[WAKEUP_DELAY] = 16;
    }

    // sleep time, in us
    pGlobal_config[MAX_SLEEP_TIME] = 30000000;
    pGlobal_config[MIN_SLEEP_TIME] = 1600;
    pGlobal_config[ALLOW_TO_SLEEP_TICK_RC32K] = 55;// 30.5 per tick
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // LL engine settle time
    pGlobal_config[LL_HW_BB_DELAY] = 54;//54-8;
    pGlobal_config[LL_HW_AFE_DELAY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY] = 40;//45;//52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV] = 62-RF_PHY_EXT_PREAMBLE_US;
    pGlobal_config[LL_HW_Tx_TO_RX_INTV] = 50;//65
    //------------------------------------------------2MPHY
    // LL engine settle time
    pGlobal_config[LL_HW_BB_DELAY_2MPHY] = 59;
    pGlobal_config[LL_HW_AFE_DELAY_2MPHY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_2MPHY] = 40;//45;//52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV_2MPHY] = 73-RF_PHY_EXT_PREAMBLE_US;//20200822 ZQ
    pGlobal_config[LL_HW_Tx_TO_RX_INTV_2MPHY] = 57;//72
    //------------------------------------------------CODEPHY 500K
    // LL engine settle time CODEPHY 500K
    pGlobal_config[LL_HW_BB_DELAY_500KPHY] = 50;//54-8;
    pGlobal_config[LL_HW_AFE_DELAY_500KPHY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_500KPHY] = 40;//45;//52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV_500KPHY] =  2;
    pGlobal_config[LL_HW_Tx_TO_RX_INTV_500KPHY] = 66;//72
    //------------------------------------------------CODEPHY 125K
    // LL engine settle time CODEPHY 125K
    pGlobal_config[LL_HW_BB_DELAY_125KPHY] = 30;//54-8;
    pGlobal_config[LL_HW_AFE_DELAY_125KPHY] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_125KPHY] = 40;//45;//52;
    // Tx2Rx and Rx2Tx interval
    //Tx2Rx could be advanced a little
    //Rx2Tx should be ensure T_IFS within150us+-2us
    pGlobal_config[LL_HW_Rx_TO_TX_INTV_125KPHY] = 5;
    pGlobal_config[LL_HW_Tx_TO_RX_INTV_125KPHY] = 66;//72
    // LL engine settle time, for advertisement
    pGlobal_config[LL_HW_BB_DELAY_ADV] = 90;
    pGlobal_config[LL_HW_AFE_DELAY_ADV] = 8;
    pGlobal_config[LL_HW_PLL_DELAY_ADV] = 60;
    // adv channel interval
    pGlobal_config[ADV_CHANNEL_INTERVAL] = 1400;//6250;
    pGlobal_config[NON_ADV_CHANNEL_INTERVAL] = 666;//6250;

    //20201207 Jie modify
    if(g_system_clk==SYS_CLK_XTAL_16M)
    {
        // scan req -> scan rsp timing
        pGlobal_config[SCAN_RSP_DELAY] = 13+RF_PHY_EXT_PREAMBLE_US;//23;
    }
    else if(g_system_clk==SYS_CLK_DBL_32M)
    {
        pGlobal_config[SCAN_RSP_DELAY] = 8+RF_PHY_EXT_PREAMBLE_US;//23;
    }
    else if(g_system_clk==SYS_CLK_DLL_48M)
    {
        // scan req -> scan rsp timing
        pGlobal_config[SCAN_RSP_DELAY] = 6+RF_PHY_EXT_PREAMBLE_US;//20201207 set           //4;        // 12    //  2019/3/19 A2: 12 --> 9
    }
    else if(g_system_clk == SYS_CLK_DLL_64M)        //  2019/3/26 add
    {
        pGlobal_config[SCAN_RSP_DELAY] = 4+RF_PHY_EXT_PREAMBLE_US;//2020.12.07 set         //3;
    }

    // conn_req -> slave connection event calibration time, will advance the receive window
    pGlobal_config[CONN_REQ_TO_SLAVE_DELAY] = 300;//192;//500;//192;
    // calibration time for 2 connection event, will advance the next conn event receive window
    // SLAVE_CONN_DELAY for sync catch, SLAVE_CONN_DELAY_BEFORE_SYNC for sync not catch
    pGlobal_config[SLAVE_CONN_DELAY] = 300;//0;//1500;//0;//3000;//0;          ---> update 11-20
    pGlobal_config[SLAVE_CONN_DELAY_BEFORE_SYNC] = 500;//160 NG//500 OK
    // RTLP timeout
    pGlobal_config[LL_HW_RTLP_LOOP_TIMEOUT] = 50000;
    pGlobal_config[LL_HW_RTLP_TO_GAP]       = 1000;
    pGlobal_config[LL_HW_RTLP_1ST_TIMEOUT]  = 2000 + pGlobal_config[SLAVE_CONN_DELAY] * 2;//500;
    // direct adv interval configuration
    pGlobal_config[HDC_DIRECT_ADV_INTERVAL] = 1000;
    pGlobal_config[LDC_DIRECT_ADV_INTERVAL] = 6250;
    // A1 ROM metal change for HDC direct adv,
    pGlobal_config[DIR_ADV_DELAY] = 115;   // in us, consider both direct adv broadcast time & SW delay, ... etc.
    // A1 ROM metal change
    pGlobal_config[LL_TX_PKTS_PER_CONN_EVT] = 6;//8;
    pGlobal_config[LL_RX_PKTS_PER_CONN_EVT] = 6;//8;
    pGlobal_config[LL_TRX_NUM_ADAPTIVE_CONFIG] = 8;     //0:        disable adaptive
    //other:    adaptive max limitation
//    pGlobal_config[LL_TX_PWR_TO_REG_BIAS]   = 0x15;   // assume when g_rfPhyTxPower = 0x1f, tx power = 10dBm
    //smart window configuration
    pGlobal_config[LL_SMART_WINDOW_COEF_ALPHA]      = 2;
    pGlobal_config[LL_SMART_WINDOW_TARGET]          = 600;
    pGlobal_config[LL_SMART_WINDOW_INCREMENT]       = 9;
    pGlobal_config[LL_SMART_WINDOW_LIMIT]           = 20000;
    pGlobal_config[LL_SMART_WINDOW_ACTIVE_THD]      = 8;
    pGlobal_config[LL_SMART_WINDOW_ACTIVE_RANGE]    = 0;//300
    pGlobal_config[LL_SMART_WINDOW_FIRST_WINDOW]    = 5000;

    //====== A2 metal change add, for scanner & initiator
    if(g_system_clk==SYS_CLK_XTAL_16M)
    {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 18+RF_PHY_EXT_PREAMBLE_US;//20;      //  2019/3/19 A2: 20 --> 18
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 25+RF_PHY_EXT_PREAMBLE_US;//27;      //  2019/3/19 A2: 27 --> 25
    }
    else if(g_system_clk==SYS_CLK_DBL_32M)
    {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 12+RF_PHY_EXT_PREAMBLE_US;                //  2019/3/26 add
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 16+RF_PHY_EXT_PREAMBLE_US;
    }
    else if(g_system_clk==SYS_CLK_DLL_48M)
    {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 8+RF_PHY_EXT_PREAMBLE_US;//12;       //  2019/3/19 A2: 12 --> 10
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 11+RF_PHY_EXT_PREAMBLE_US;
    }
    else if(g_system_clk==SYS_CLK_DLL_64M)
    {
        pGlobal_config[LL_ADV_TO_SCAN_REQ_DELAY]    = 6+RF_PHY_EXT_PREAMBLE_US;                //  2019/3/26 add
        pGlobal_config[LL_ADV_TO_CONN_REQ_DELAY]    = 8+RF_PHY_EXT_PREAMBLE_US;
    }

    // TRLP timeout
    pGlobal_config[LL_HW_TRLP_LOOP_TIMEOUT] = 50000;    // enough for 8Tx + 8Rx : (41 * 8 + 150) * 16 - 150 = 7498us
    pGlobal_config[LL_HW_TRLP_TO_GAP]       = 1000;
    pGlobal_config[LL_MOVE_TO_MASTER_DELAY] = 100;
    pGlobal_config[LL_CONN_REQ_WIN_SIZE] = 5;
    pGlobal_config[LL_CONN_REQ_WIN_OFFSET] = 2;
    pGlobal_config[LL_MASTER_PROCESS_TARGET] = 200;   // reserve time for preparing master conn event, delay should be insert if needn't so long time
    pGlobal_config[LL_MASTER_TIRQ_DELAY] = 0;         // timer IRQ -> timer ISR delay
    pGlobal_config[OSAL_SYS_TICK_WAKEUP_TRIM] = 56;  // 0.125us
    pGlobal_config[MAC_ADDRESS_LOC] = 0x11004000;
    // for simultaneous conn & adv/scan
    pGlobal_config[LL_NOCONN_ADV_EST_TIME] = 1400*3;
    pGlobal_config[LL_NOCONN_ADV_MARGIN] = 600;
    pGlobal_config[LL_SEC_SCAN_MARGIN] = 2500;//1400;  to avoid mesh proxy llTrigErr 0x15
    pGlobal_config[LL_MIN_SCAN_TIME] = 2000;
    //  BBB new
    pGlobal_config[TIMER_ISR_ENTRY_TIME] = 30;//15;
    pGlobal_config[LL_MULTICONN_MASTER_PREEMP] = 0;
    pGlobal_config[LL_MULTICONN_SLAVE_PREEMP] = 0;
    pGlobal_config[LL_EXT_ADV_TASK_DURATION] = 20000;
    pGlobal_config[LL_PRD_ADV_TASK_DURATION] = 20000;
    pGlobal_config[LL_CONN_TASK_DURATION] = 5000;
    pGlobal_config[LL_EXT_ADV_INTER_PRI_CHN_INT] = 5000;
    pGlobal_config[LL_EXT_ADV_INTER_SEC_CHN_INT] = 5000;
    pGlobal_config[LL_EXT_ADV_PRI_2_SEC_CHN_INT] = 1500;
    pGlobal_config[LL_EXT_ADV_RSC_PERIOD] = 1000000;
    pGlobal_config[LL_EXT_ADV_RSC_SLOT_DURATION] = 10000;
    pGlobal_config[LL_PRD_ADV_RSC_PERIOD] = 1000000;
    pGlobal_config[LL_PRD_ADV_RSC_SLOT_DURATION] = 10000;
    pGlobal_config[LL_EXT_ADV_PROCESS_TARGET] = 500;
    pGlobal_config[LL_PRD_ADV_PROCESS_TARGET] = 500;

    enableSleep();
    setSleepMode(SYSTEM_SLEEP_MODE);
}

void hal_rom_boot_init(void)
{
    extern void _rom_sec_boot_init();
    _rom_sec_boot_init();
}

extern inline uint32_t __psr(void)
{
    uint32_t i;
    __asm
    {
        MRS i, psr
    }
    return i;
}

CHIP_ID_STATUS_e chip_id_one_bit_hot_convter(uint8_t* b,uint32_t w)
{
    uint16 dh = w>>16;
    uint16 dl = w&0xffff;
    uint16 h1,h0,l1,l0;
    h0=l0=0xff;
    h1=l1=0;

    for(int i=0; i<16; i++)
    {
        l1+=((dl&(1<<i))>>i);

        if(l0==0xff && l1==1)
        {
            l0=i;
        }

        h1+=((dh&(1<<i))>>i);

        if(h0==0xff && h1==1)
        {
            h0=i;
        }
    }

    if(l1==1 && h1==1)
    {
        *b=((h0<<4)+l0);
        return CHIP_ID_VALID;
    }
    else if(l1==16 && h1==16)
    {
        return CHIP_ID_EMPTY;
    }
    else
    {
        return CHIP_ID_INVALID;
    }
}
