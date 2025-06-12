cmake_minimum_required(VERSION 3.16)

# 工具链配置 - 必须在project()之前
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)

# 跳过编译器测试（嵌入式项目）
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

# 强制设置ARM工具链
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# 项目配置
project(hy-cool-dtu 
    VERSION 3.4.2
    DESCRIPTION "Cold Storage DTU with OTA Support"
    LANGUAGES C ASM
)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size REQUIRED)

# 编译器标志
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ARM Cortex-M0+ 基本标志
set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft")
set(COMMON_FLAGS "${CPU_FLAGS} -ffunction-sections -fdata-sections -Wall -g3")

# 编译器标志 - 使用基本设置，不依赖newlib
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=c99 -nostdlib -fno-builtin")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS}")

# 添加系统包含路径
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -print-file-name=include
    OUTPUT_VARIABLE ARM_GCC_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(EXISTS "${ARM_GCC_INCLUDE_DIR}")
    include_directories(${ARM_GCC_INCLUDE_DIR})
endif()

# 预处理器定义 - 避免重复定义
add_definitions(
    -DNANO100B
    -DUSE_HAL_DRIVER
)

# 防止重复定义HSE_VALUE
if(NOT DEFINED CUSTOM_HSE_VALUE)
    add_definitions(-DHSE_VALUE=12000000UL)
    set(CUSTOM_HSE_VALUE ON CACHE BOOL "HSE_VALUE已定义")
endif()

# 防止重复定义OTA相关宏
if(NOT DEFINED CUSTOM_OTA_SIZE)
    add_definitions(
        -DOTA_ENABLE=1
        -DOTA_MAX_FIRMWARE_SIZE=0x38000U
        -DOTA_CHUNK_SIZE=1024
    )
    set(CUSTOM_OTA_SIZE ON CACHE BOOL "OTA宏已定义")
endif()

# 包含目录
include_directories(
    app
    app/ota/include
    src/include
    src/ota/include
    src/linker
)

# 主要源文件
set(MAIN_SOURCES
    app/main.c
    src/main_loop.c
    src/Modbus.c
    src/sys_flash.c
    src/HistoryDataRead.C
    src/Modbus_LoraModbusMaster.c
    src/RI300CRC.c
    src/BL03D_BH_Para.C
    src/alarm.C
    src/uart0_Modbus_uart3.c
    src/uart3_Modbus.c
    src/uart2_Modbus.c
    src/uart4_dealdata.c
    src/Base_Para.C
    src/HistoryData.C
    src/BL02D_Special.C
    src/switch.c
    src/io.c
    src/Tempalarm.C
    src/datastruct.c
    src/uart0_Modbus.c
    src/uart1_Modbus.c
)

# OTA源文件
file(GLOB_RECURSE OTA_SOURCES 
    "app/ota/src/*.c"
    "src/ota/*.c"
)

# 模块源文件
file(GLOB_RECURSE MODULE_SOURCES
    "src/uart/*.c"
    "src/dido/*.c"
    "src/key/*.c"
    "src/si24r1/*.c"
    "src/bl/*.c"
    "src/sim7600/*.c"
    "src/workled/*.c"
    "src/alarm/*.c"
    "src/lora/*.c"
    "src/switch/*.c"
    "src/lcd/*.c"
    "src/sht2x/*.c"
    "src/adc/*.c"
    "src/spi/*.c"
    "src/i2c/*.c"
    "src/watchdong/*.c"
    "src/led/*.c"
    "src/pwm/*.c"
    "src/alarm_led/*.c"
)

# 合并所有源文件
set(ALL_SOURCES ${MAIN_SOURCES} ${OTA_SOURCES} ${MODULE_SOURCES})

# 排除测试文件
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*test.*")

# 创建可执行文件
add_executable(${PROJECT_NAME}.elf ${ALL_SOURCES})

# 设置链接器脚本和标志
set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    LINK_FLAGS "${CPU_FLAGS} -T${CMAKE_SOURCE_DIR}/src/linker/nano100_512k.ld -Wl,--gc-sections -Wl,--print-memory-usage -nostdlib"
)

# 生成二进制文件
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "生成二进制文件和显示大小信息"
)

# 调试目标
add_custom_target(debug
    COMMAND JLinkGDBServer -select USB -device NANO100B -if SWD -speed 4000
    COMMENT "启动J-Link GDB服务器"
)

# 烧录目标
add_custom_target(flash
    COMMAND JLinkExe -device NANO100B -if SWD -speed 4000 -CommanderScript flash.jlink
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "烧录固件到设备"
)

# 清理目标
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "清理所有构建文件"
) 