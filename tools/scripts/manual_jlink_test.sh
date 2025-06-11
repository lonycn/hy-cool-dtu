#!/bin/bash

# 手动J-Link连接测试脚本

echo "🔧 手动J-Link连接测试"
echo "============================================"

# 创建测试脚本
cat > /tmp/manual_test.jlink << 'EOF'
connect
Cortex-M0+
S
4000
h
mem32 0x00000000 4
q
EOF

echo "📝 测试脚本内容:"
cat /tmp/manual_test.jlink
echo ""

echo "🔍 执行J-Link测试..."
echo "如果出现交互提示，请根据提示操作"
echo "============================================"

# 执行J-Link命令
JLinkExe -CommanderScript /tmp/manual_test.jlink

echo ""
echo "============================================"
echo "测试完成"

# 清理
rm -f /tmp/manual_test.jlink 