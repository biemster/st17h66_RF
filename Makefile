##############################################################################
BUILD = build
BIN = Lenze_RF
CMSIS_PATH = ../CMSIS_5/
##############################################################################
.PHONY: all directory clean size

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS += -W -Wall --std=gnu99 -O3
CFLAGS += -Wno-parentheses
CFLAGS += -fno-diagnostics-show-caret
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -mcpu=cortex-m0 -mthumb -mthumb-interwork
CFLAGS += -MD -MP -MT $(BUILD)/$(*F).o -MF $(BUILD)/$(@F).d

LDFLAGS += -mcpu=cortex-m0 -mthumb -mthumb-interwork
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--script=./RTE/Device/ARMCM0/st17h66.ld
LDFLAGS += -Wl,--just-symbols=./misc/bb_rom_sym_m0.gcc

INCLUDES += \
	-I./include \
	-I$(CMSIS_PATH)/Device/ARM/ARMCM0/Include \
	-I$(CMSIS_PATH)/CMSIS/Core/Include

SRCS += \
	./main.c \
	./source/Lenze_phy.c \
	./source/OSAL_LenzePhy.c \
	./components/clock.c \
	./components/flash.c \
	./components/fs.c \
	./components/gpio.c \
	./components/pwrmgr.c \
	./components/timer.c \
	./lib/patch.c \
	./lib/phy_sec_ext.c \
	./lib/rf_phy_driver.c \
	./misc/jump_table.c \
	./RTE/Device/ARMCM0/startup_ARMCM0.c \
	./RTE/Device/ARMCM0/system_ARMCM0.c


DEFINES += \
  -DADV_NCONN_CFG=0x01 \
  -DADV_CONN_CFG=0x02 \
  -DSCAN_CFG=0x04 \
  -DINIT_CFG=0x08 \
  -DBROADCASTER_CFG=0x01 \
  -DOBSERVER_CFG=0x02 \
  -DPERIPHERAL_CFG=0x04 \
  -DCENTRAL_CFG=0x08 \
  -DHOST_CONFIG=0x4 \
  -DARMCM0 \
  -D_RTE_ \
  -DCFG_CP \
  -DOSAL_CBTIMER_NUM_TASKS="1" \
  -DMTU_SIZE="247" \
  -DHCI_TL_NONE="1" \
  -DENABLE_LOG_ROM_="0" \
  -D_BUILD_FOR_DTM_="0" \
  -DDEBUG_INFO="1" \
  -DDBG_ROM_MAIN="0" \
  -DAPP_CFG="0" \
  -DOSALMEM_METRICS="0" \
  -DPHY_MCU_TYPE="MCU_BUMBEE_M0" \
  -DCFG_SLEEP_MODE="PWR_MODE_SLEEP" \
  -DDEF_GAPBOND_MGR_ENABLE="0" \
  -DUSE_FS="0" \
  -DMAX_NUM_LL_CONN="1"


CFLAGS += $(INCLUDES) $(DEFINES)

OBJS = $(addprefix $(BUILD)/, $(notdir %/$(subst .c,.o, $(SRCS))))
# $(info List of files : [${OBJS}])

all: directory $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).hex $(BUILD)/$(BIN).bin size

$(BUILD)/$(BIN).elf: $(OBJS)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/$(BIN).hex: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex $^ $@
	@sed -i "130i $$(head -1 $@)" $@ # The flasher needs to know when .text starts after the jump table and global config

$(BUILD)/$(BIN).bin: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O binary $^ $@

%.o:
	@echo CC $@
	@$(CC) $(CFLAGS) $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS)) -c -o $@

directory:
	@mkdir -p $(BUILD)

size: $(BUILD)/$(BIN).elf
	@echo size:
	@$(SIZE) -t $^

clean:
	@echo clean
	@-rm -rf $(BUILD)

-include $(wildcard $(BUILD)/*.d)
