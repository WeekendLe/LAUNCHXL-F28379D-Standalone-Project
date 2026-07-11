################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./source/SciDriverF28379D.obj" \
"./source/vHardwareSetup.obj" \
"./source/device/F2837xD_Adc.obj" \
"./source/device/F2837xD_CodeStartBranch.obj" \
"./source/device/F2837xD_DefaultISR.obj" \
"./source/device/F2837xD_GlobalVariableDefs.obj" \
"./source/device/F2837xD_Gpio.obj" \
"./source/device/F2837xD_Ipc.obj" \
"./source/device/F2837xD_PieCtrl.obj" \
"./source/device/F2837xD_PieVect.obj" \
"./source/device/F2837xD_SysCtrl.obj" \
"./source/device/F2837xD_usDelay.obj" \
"../2837xD_RAM_lnk_cpu1.cmd" \
"../F2837xD_Headers_nonBIOS_cpu1.cmd" \
$(GEN_CMDS__FLAG) \
-lrts2800_fpu32.lib \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include source/subdir_vars.mk
-include source/device/subdir_vars.mk
-include subdir_rules.mk
-include source/subdir_rules.mk
-include source/device/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
LAUNCHXL_F28379D_setUp_CPU1.out 

EXE_OUTPUTS__QUOTED += \
"LAUNCHXL_F28379D_setUp_CPU1.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "LAUNCHXL_F28379D_setUp_CPU1.out"

# Tool invocations
LAUNCHXL_F28379D_setUp_CPU1.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'C2000 Linker - building target: "$@"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --define=_LAUNCHXL_F28379D --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"LAUNCHXL_F28379D_setUp_CPU1.map" --stack_size=0x200 --warn_sections -i"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/lib" -i"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="LAUNCHXL_F28379D_setUp_CPU1_linkInfo.xml" --entry_point=code_start --rom_model -o "LAUNCHXL_F28379D_setUp_CPU1.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "source\SciDriverF28379D.obj" "source\vHardwareSetup.obj" "source\device\F2837xD_Adc.obj" "source\device\F2837xD_CodeStartBranch.obj" "source\device\F2837xD_DefaultISR.obj" "source\device\F2837xD_GlobalVariableDefs.obj" "source\device\F2837xD_Gpio.obj" "source\device\F2837xD_Ipc.obj" "source\device\F2837xD_PieCtrl.obj" "source\device\F2837xD_PieVect.obj" "source\device\F2837xD_SysCtrl.obj" "source\device\F2837xD_usDelay.obj" 
	-$(RM) "main.d" "source\SciDriverF28379D.d" "source\vHardwareSetup.d" "source\device\F2837xD_Adc.d" "source\device\F2837xD_DefaultISR.d" "source\device\F2837xD_GlobalVariableDefs.d" "source\device\F2837xD_Gpio.d" "source\device\F2837xD_Ipc.d" "source\device\F2837xD_PieCtrl.d" "source\device\F2837xD_PieVect.d" "source\device\F2837xD_SysCtrl.d" 
	-$(RM) "source\device\F2837xD_CodeStartBranch.d" "source\device\F2837xD_usDelay.d" 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

