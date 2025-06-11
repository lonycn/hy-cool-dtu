cmake_minimum_required(VERSION 3.20)

# 项目信息
project(hy_cool_dtu_ota 
    VERSION 1.1.26
    DESCRIPTION "HY Cool DTU with OTA Upgrade Module"
    LANGUAGES C ASM
)

# 设置C标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 交叉编译工具链设置
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 查找ARM工具链
find_program(CMAKE_C_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++ REQUIRED)
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_AR arm-none-eabi-ar REQUIRED)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_OBJDUMP arm-none-eabi-objdump REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size REQUIRED)

# 编译器标志
set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft")
set(OPTIMIZATION_FLAGS "-Os -ffunction-sections -fdata-sections")
set(WARNING_FLAGS "-Wall -Wextra -Wno-unused-parameter")
set(DEBUG_FLAGS "-g3 -gdwarf-2")

# C编译器标志
set(CMAKE_C_FLAGS "${CPU_FLAGS} ${OPTIMIZATION_FLAGS} ${WARNING_FLAGS} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fno-common -fmessage-length=0")

# 汇编器标志
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} ${DEBUG_FLAGS}")

# 链接器标志
set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker/nano100_512k.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -T${LINKER_SCRIPT}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_NAME}.map")

# 预处理器定义
add_definitions(
    -DNANO100SERIES
    -DNANO100B
    -D__CORTEX_M0PLUS
    -DARM_MATH_CM0PLUS
    -DOTA_ENABLE=1
    -DOTA_MAX_FIRMWARE_SIZE=0x38000
    -DOTA_CHUNK_SIZE=1024
)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ota/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 源文件列表
set(COMMON_SOURCES
    # 主程序文件
    main_loop.c
    Modbus.c
    sys_flash.c
    RI300CRC.c
    io.c
    switch.c
    datastruct.c
    Base_Para.C
    
    # 历史数据处理
    HistoryData.C
    HistoryDataRead.C
    HistoryDataWrite.C
    
    # 温度报警
    Tempalarm.C
    alarm.C
    
    # BL02D/BL03D特定功能
    BL02D_Special.C
    BL03D_BH_Para.C
    
    # 各种UART Modbus接口
    uart0_Modbus.c
    uart1_Modbus.c
    uart2_Modbus.c
    uart3_Modbus.c
    uart0_Modbus_uart3.c
    uart2_Modbus_tp.c
    uart2_Modbus_pc.c
    
    # LoRa Modbus主站
    Modbus_LoraModbusMaster.c
    uart1_Modbus_LoraModbusMaste.c
    uart2_Modbus_LoraModbusMaste.c
    uart4_Modbus_LoraModbusMaste.c
    uart4_Modbus_LoraModbusMaste_uart0.c
    
    # 数据处理
    uart4_dealdata.c
)

# UART模块源文件
set(UART_SOURCES
    uart/SerInt-uart0.c
    uart/SerInt-uart0-uart3.c
    uart/SerInt_uart1.c
    uart/SerInt_uart2.c
    uart/SerInt_uart3.c
    uart/SerInt_uart4.c
    uart/Simdata.c
)

# 各种外设模块源文件
set(PERIPHERAL_SOURCES
    # LED模块
    led/led.c
    
    # 报警模块
    alarm/SetAlarmFlag.c
    
    # SIM7600模块（如果存在）
    # SIM7600/sim7600_main.c
    # SIM7600/Sim7600_StatusMachine.c
    # SIM7600/Combine.c
    
    # 其他外设模块
    # adc/adc.c
    # spi/spi.c
    # IIC/iic.c
    # key/key.c
    # lcd/lcd.c
    # PWM/pwm.c
    # sht2x/sht2x.c
    # si24r1/si24r1.c
    # lora/lora.c
    # dido/dido.c
    # workled/workled.c
    # switch/switch.c
    # watchdong/watchdong.c
    # bl/bl.c
    # Alarmled/alarmled.c
    # TEstFunction/TestFunction.c
)

# OTA模块源文件
set(OTA_SOURCES
    ota/src/ota_manager.c
    ota/src/ota_config.c
)

# 检查源文件是否存在
foreach(source ${COMMON_SOURCES} ${UART_SOURCES} ${PERIPHERAL_SOURCES} ${OTA_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        list(APPEND EXISTING_SOURCES ${source})
    else()
        message(WARNING "Source file not found: ${source}")
    endif()
endforeach()

# 创建可执行文件
add_executable(${PROJECT_NAME}.elf ${EXISTING_SOURCES})

# 设置目标属性
set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    SUFFIX ".elf"
    LINK_DEPENDS ${LINKER_SCRIPT}
)

# 创建二进制文件
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Creating binary and hex files"
)

# 创建反汇编文件
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -h -S $<TARGET_FILE:${PROJECT_NAME}.elf> > ${PROJECT_NAME}.lst
    COMMENT "Creating disassembly listing"
)

# 显示构建信息
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "============================================"
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "Target: ${PROJECT_NAME}.elf"
    COMMAND ${CMAKE_COMMAND} -E echo "Binary: ${PROJECT_NAME}.bin"
    COMMAND ${CMAKE_COMMAND} -E echo "Intel Hex: ${PROJECT_NAME}.hex"
    COMMAND ${CMAKE_COMMAND} -E echo "Listing: ${PROJECT_NAME}.lst"
    COMMAND ${CMAKE_COMMAND} -E echo "Map file: ${PROJECT_NAME}.map"
    COMMAND ${CMAKE_COMMAND} -E echo "============================================"
    COMMENT "Build summary"
)

# 添加自定义目标
add_custom_target(flash
    COMMAND echo "Flashing firmware..."
    COMMAND openocd -f interface/jlink.cfg -f target/nano100b.cfg -c "program ${PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing firmware to target"
)

add_custom_target(debug
    COMMAND echo "Starting debug session..."
    COMMAND openocd -f interface/jlink.cfg -f target/nano100b.cfg
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Starting OpenOCD debug server"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} -E remove -f ${PROJECT_NAME}.bin ${PROJECT_NAME}.hex ${PROJECT_NAME}.lst ${PROJECT_NAME}.map
    COMMENT "Cleaning all build artifacts"
)

# 打印配置信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Target CPU: Cortex-M0+")
message(STATUS "C Standard: C${CMAKE_C_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "Linker Flags: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "OTA Module: ENABLED") 