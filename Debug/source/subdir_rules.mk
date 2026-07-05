################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/%.obj: ../source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'C2000 Compiler - building file: "$<"'
	"C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="C:/ti/ccs2100/ccs/tools/compiler/ti-cgt-c2000_25.11.1.LTS/include" --include_path="C:/Users/unkno/workspace_ccstheia/LAUNCHXL_F28379D_setUp_CPU1" --include_path="C:/Users/unkno/workspace_ccstheia/LAUNCHXL_F28379D_setUp_CPU1/include" --include_path="C:/Users/unkno/workspace_ccstheia/LAUNCHXL_F28379D_setUp_CPU1/include/headers" --include_path="C:/Users/unkno/workspace_ccstheia/LAUNCHXL_F28379D_setUp_CPU1/include/common" --include_path="C:/Users/unkno/workspace_ccstheia/LAUNCHXL_F28379D_setUp_CPU1/source/device" --define=_LAUNCHXL_F28379D --define=CPU1 -g --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


