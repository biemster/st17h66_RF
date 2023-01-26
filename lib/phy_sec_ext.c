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

#include "flash.h"
#include "error.h"
#include "OSAL.h"
#include "mcu.h"
#include "version.h"

#define CHIP_ID_FLASH_ADDRESS           0x11000800
#define CHIP_MADDR_FLASH_ADDRESS        (CHIP_ID_FLASH_ADDRESS+CHIP_ID_LENGTH*4)
#define CHIP_VERSION_L                  SDK_VER_CHIP
#define CHIP_VERSION_H                  (0x0BBB)

#define CHIP_BOOT_AREA_ADDR             0x3000
#define CHIP_BOOT_PART_SEC(n)           ((*(volatile uint32_t*)(SPIF_BASE_ADDR + CHIP_BOOT_AREA_ADDR+0x104+(n)*0x10))&0x80000000)
#define CHIP_SECURE_AREA_ADDR           (0x2900)
#define CHIP_SECURE_AUTH_WORD           (*(volatile uint32_t*)(SPIF_BASE_ADDR + CHIP_SECURE_AREA_ADDR ))
#define CHIP_SECURE_USE_EFUSE           (*(volatile uint32_t*)(SPIF_BASE_ADDR + CHIP_SECURE_AREA_ADDR + 0x4))
#define CHIP_SECURE_KEY_L               (*(volatile uint32_t*)(SPIF_BASE_ADDR + CHIP_SECURE_AREA_ADDR + 0x8))
#define CHIP_SECURE_KEY_H               (*(volatile uint32_t*)(SPIF_BASE_ADDR + CHIP_SECURE_AREA_ADDR + 0xc))
#define pCHIP_SECURE_PLAINTEXT          ((volatile uint8_t*)(SPIF_BASE_ADDR + CHIP_SECURE_AREA_ADDR + 0x10))
#define pCHIP_SECURE_MIC                ((volatile uint8_t*)(SPIF_BASE_ADDR + CHIP_SECURE_AREA_ADDR + 0x20))

#if 0
    chipId_t g_chipId;
    chipMAddr_t  g_chipMAddr;
#endif

const char* s_company_id = "PHY+62XXPLUS0504";

uint8_t MID, JID8, JID0;
enum
{
    OTA_SEC_KEY_NULL = 0,
    OTA_SEC_KEY_CCM = 1,
    OTA_SEC_KEY_FAIL = 0xff,
};

typedef enum
{
    EFUSE_BLOCK_0 = 0,
    EFUSE_BLOCK_1 = 1,
    EFUSE_BLOCK_2 = 2,
    EFUSE_BLOCK_3 = 3,

} EFUSE_block_t;
//extern int efuse_lock(EFUSE_block_t block);
extern int efuse_read(EFUSE_block_t block,uint32_t* buf);
extern int efuse_write(EFUSE_block_t block,uint32_t* buf,uint32_t us);

extern uint8 osal_memcmp( const void GENERIC* src1, const void GENERIC* src2, unsigned int len );

const uint32_t ROM_Base4K[4]= {0x47704800,0x00017D00,0xB082B5F7,0x27009C02};
uint8_t PHY_Varify_Platform()
{
    uint8_t ret = TRUE;

    for(uint8 i=0; i<4; i++)
    {
        if( ROM_Base4K[i] != read_reg( 0x1000 + i*4 ) )
        {
            ret = FALSE;
            break;
        }
    }

    return ret;
}

void efuse_init(void)
{
    write_reg(0x4000f054,0x0);
    write_reg(0x4000f140,0x0);
    write_reg(0x4000f144,0x0);
}
void _rom_sec_boot_init(void)
{
    efuse_init();

    typedef void (*my_function)(void);
    my_function pFunc = (my_function)(0xa2e1);
    pFunc();
    return;
}
