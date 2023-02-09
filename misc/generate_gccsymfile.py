#!/usr/bin/env python3
syms = [
    'clk_init',
    'drv_disable_irq',
    'drv_enable_irq',
    'drv_irq_init',
    'enableSleep',
    'g_rfPhyClkSel',
    'g_rfPhyFreqOffSet',
    'g_rfPhyPktFmt',
    'g_rfPhyTpCal0',
    'g_rfPhyTpCal0_2Mbps',
    'g_rfPhyTpCal1',
    'g_rfPhyTpCal1_2Mbps',
    'g_rfPhyTxPower',
    'g_rxAdcClkSel',
    'g_system_clk',
    'll_hw_clr_irq',
    'll_hw_get_irq_status',
    'll_hw_get_tr_mode',
    'll_hw_ign_rfifo',
    'll_hw_read_rfifo',
    'll_hw_read_rfifo_pplus',
    'll_hw_rst_rfifo',
    'll_hw_rst_tfifo',
    'll_hw_set_crc_fmt',
    'll_hw_set_pplus_pktfmt',
    'll_hw_set_rx_timeout',
    'll_hw_set_rx_tx_interval',
    'll_hw_set_srx',
    'll_hw_set_stx',
    'll_hw_set_trx',
    'll_hw_set_trx_settle',
    'll_hw_set_tx_rx_interval',
    'll_hw_set_tx_rx_release',
    'll_hw_tx2rx_timing_config',
    'll_hw_write_tfifo',
    'llWaitingIrq',
    'osal_init_system',
    'osal_mem_alloc',
    'osal_memcpy',
    'osal_memset',
    'osal_mem_set_heap',
    'osal_pwrmgr_device',
    'osal_pwrmgr_powerconserve0',
    'osal_set_event',
    'osal_start_system',
    'osal_start_timerEx',
    'read_current_fine_time',
    'set_max_length',
    'setSleepMode',
    'spif_cmd',
    'spif_config',
    'spif_rddata',
    'WaitRTCCount'
]

refs = {}
with open('bb_rom_sym_m0.txt') as symfile:
    for line in symfile:
        if line[0] == '#': continue
        sym = line.split()
        if line[0] == ';': sym = line[1:].split()
        refs[sym[2]] = sym[0]

for s in syms:
    print(f'{s} = {refs[s]};')
