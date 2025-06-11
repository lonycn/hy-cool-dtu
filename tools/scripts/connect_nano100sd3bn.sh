#!/bin/bash

# NUVOTON NANO100SD3BN 芯片连接脚本

echo "🎯 NUVOTON NANO100SD3BN 连接脚本"
echo "============================================"

# 芯片信息
echo "📋 目标芯片信息:"
echo "    品牌: NUVOTON"
echo "    型号: NANO100SD3BN"
echo "    内核: ARM Cortex-M0+"
echo "    Flash: 512KB"
echo "    SRAM: 32KB"
echo ""

# 创建专用连接脚本
echo "📝 创建连接配置..."

# 方案1: 使用NANO100B (通用兼容)
cat > /tmp/nano100_test1.jlink << 'EOF'
connect
NANO100B
S
4000
h
mem32 0x00000000 4
q
EOF

# 方案2: 使用通用Cortex-M0+
cat > /tmp/nano100_test2.jlink << 'EOF'
connect
Cortex-M0+
S
4000
h
mem32 0x00000000 4
q
EOF

# 方案3: 手动指定设备参数
cat > /tmp/nano100_test3.jlink << 'EOF'
connect
?
NANO100SD3BN
S
4000
h
mem32 0x00000000 4
q
EOF

echo "🔍 测试方案1: NANO100B (兼容模式)"
echo "============================================"
JLinkExe -CommanderScript /tmp/nano100_test1.jlink
echo ""

echo "🔍 测试方案2: Cortex-M0+ (通用内核)"
echo "============================================"  
JLinkExe -CommanderScript /tmp/nano100_test2.jlink
echo ""

echo "🔍 测试方案3: 手动设备选择"
echo "============================================"
JLinkExe -CommanderScript /tmp/nano100_test3.jlink

# 清理临时文件
rm -f /tmp/nano100_test*.jlink

echo ""
echo "============================================"
echo "✅ 连接测试完成" 