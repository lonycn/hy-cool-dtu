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

set(COMMON_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -ffunction-sections -fdata-sections -Wall -Wextra -g3 -nostdlib")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=c99")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")

# 链接器标志
set(CMAKE_EXE_LINKER_FLAGS "${COMMON_FLAGS} -T${CMAKE_SOURCE_DIR}/Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/Library/Device/Nuvoton/Nano100Series/Source/GCC/gcc_arm.ld -Wl,--gc-sections -Wl,--print-memory-usage")

# 预处理器定义
add_definitions(
    -DNANO100B
    -DUSE_HAL_DRIVER
    -DHSE_VALUE=12000000
    -DOTA_ENABLE=1
    -DOTA_MAX_FIRMWARE_SIZE=0x38000U
    -DOTA_CHUNK_SIZE=1024
)

# 包含目录
include_directories(
          app
    app/ota/include
    Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/Library/CMSIS/Include
    Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/Library/Device/Nuvoton/Nano100Series/Include
    Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/Library/StdDriver/inc
    Nano/modbus_ver3.4.2(3283)/App
)

# 源文件
file(GLOB_RECURSE SOURCES 
    "app/main.c"
    "app/ota/src/*.c"
    "Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/Library/StdDriver/src/*.c"
    "Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/Library/Device/Nuvoton/Nano100Series/Source/system_Nano100Series.c"
)

# 排除测试文件
list(FILTER SOURCES EXCLUDE REGEX ".*test.*")

# 启动文件
set(STARTUP_FILE "Nano/EC_NANO100B_UART_Rx_Wakeup_PDMA_V1.00/Library/Device/Nuvoton/Nano100Series/Source/GCC/startup_Nano100Series.S")

# 创建可执行文件
add_executable(${PROJECT_NAME}.elf 
    ${SOURCES}
    ${STARTUP_FILE}
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