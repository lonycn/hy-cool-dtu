# ========================================
# ARM None EABI 工具链配置
# 适用于 NANO100B (ARM Cortex-M0+) 芯片
# ========================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 工具链设置
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# 禁用编译器检查（交叉编译）
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# CPU 和架构设置
set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft")

# 编译器标志
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -Wextra -Wpedantic -Os -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp")

# 链接器标志
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")

# 定义宏
add_definitions(
    -DNANO100B
    -DUSE_HAL_DRIVER
    -DHSE_VALUE=12000000
    -DOTA_ENABLE=1
    -DOTA_MAX_FIRMWARE_SIZE=0x38000
    -DOTA_CHUNK_SIZE=1024
)

# 搜索路径设置
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY) 