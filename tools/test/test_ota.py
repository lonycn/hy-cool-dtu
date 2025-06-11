#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
冷库DTU OTA测试工具
用于测试OTA升级功能的各个环节
"""

import os
import sys
import json
import time
import hashlib
import requests
import argparse
from pathlib import Path
from typing import Dict, List, Optional
import logging
from http.server import HTTPServer, SimpleHTTPRequestHandler
import threading

# 设置日志
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)
logger = logging.getLogger(__name__)


class OTATestServer:
    """OTA测试服务器"""

    def __init__(self, port: int = 8080, firmware_dir: str = "./firmware"):
        self.port = port
        self.firmware_dir = Path(firmware_dir)
        self.firmware_dir.mkdir(exist_ok=True)
        self.server = None
        self.server_thread = None

    def start(self):
        """启动测试服务器"""
        os.chdir(self.firmware_dir)
        self.server = HTTPServer(("", self.port), SimpleHTTPRequestHandler)
        self.server_thread = threading.Thread(target=self.server.serve_forever)
        self.server_thread.daemon = True
        self.server_thread.start()
        logger.info(f"🌐 OTA测试服务器已启动: http://localhost:{self.port}")

    def stop(self):
        """停止测试服务器"""
        if self.server:
            self.server.shutdown()
            self.server_thread.join()
            logger.info("🛑 OTA测试服务器已停止")


class FirmwareManager:
    """固件管理器"""

    def __init__(self, firmware_dir: str):
        self.firmware_dir = Path(firmware_dir)
        self.firmware_dir.mkdir(exist_ok=True)

    def generate_test_firmware(self, version: str, size_kb: int = 100) -> Dict:
        """生成测试固件文件"""
        filename = f"dtu_firmware_v{version}.bin"
        filepath = self.firmware_dir / filename

        # 生成随机固件内容
        import random

        firmware_data = bytearray(random.getrandbits(8) for _ in range(size_kb * 1024))

        # 添加版本头信息
        version_header = f"DTU_FW_V{version}".encode("utf-8").ljust(32, b"\x00")
        firmware_data[:32] = version_header

        # 写入文件
        with open(filepath, "wb") as f:
            f.write(firmware_data)

        # 计算校验和
        md5_hash = hashlib.md5(firmware_data).hexdigest()
        sha256_hash = hashlib.sha256(firmware_data).hexdigest()

        firmware_info = {
            "version": version,
            "filename": filename,
            "size": len(firmware_data),
            "md5": md5_hash,
            "sha256": sha256_hash,
            "url": f"http://localhost:8080/{filename}",
        }

        # 保存元数据
        metadata_file = self.firmware_dir / f"{filename}.json"
        with open(metadata_file, "w") as f:
            json.dump(firmware_info, f, indent=2)

        logger.info(f"✅ 生成测试固件: {filename} ({size_kb}KB)")
        return firmware_info

    def list_firmware(self) -> List[Dict]:
        """列出所有固件"""
        firmware_list = []
        for json_file in self.firmware_dir.glob("*.json"):
            try:
                with open(json_file, "r") as f:
                    firmware_info = json.load(f)
                    firmware_list.append(firmware_info)
            except Exception as e:
                logger.warning(f"读取固件信息失败: {json_file} - {e}")
        return firmware_list


class DeviceSimulator:
    """设备模拟器"""

    def __init__(self, device_id: str):
        self.device_id = device_id
        self.current_version = "1.0.0"
        self.server_url = "http://localhost:8080"

    def check_update(self) -> Optional[Dict]:
        """检查更新"""
        logger.info(f"📱 设备 {self.device_id} 检查更新...")

        # 模拟检查更新请求
        check_data = {
            "device_id": self.device_id,
            "current_version": self.current_version,
            "hardware_version": "NANO100SD3BN",
        }

        try:
            # 这里应该向实际的OTA服务器发送请求
            # 现在模拟返回结果
            firmware_manager = FirmwareManager("./firmware")
            firmware_list = firmware_manager.list_firmware()

            if firmware_list:
                latest_firmware = max(firmware_list, key=lambda x: x["version"])
                if latest_firmware["version"] > self.current_version:
                    logger.info(f"🆕 发现新版本: {latest_firmware['version']}")
                    return latest_firmware

            logger.info("✅ 当前已是最新版本")
            return None

        except Exception as e:
            logger.error(f"❌ 检查更新失败: {e}")
            return None

    def download_firmware(self, firmware_info: Dict) -> bool:
        """下载固件"""
        logger.info(f"⬇️ 下载固件: {firmware_info['filename']}")

        try:
            url = firmware_info["url"]
            response = requests.get(url, stream=True)
            response.raise_for_status()

            total_size = int(response.headers.get("content-length", 0))
            downloaded = 0

            download_path = Path(f"./download/{firmware_info['filename']}")
            download_path.parent.mkdir(exist_ok=True)

            with open(download_path, "wb") as f:
                for chunk in response.iter_content(chunk_size=1024):
                    if chunk:
                        f.write(chunk)
                        downloaded += len(chunk)

                        # 显示进度
                        if total_size > 0:
                            progress = (downloaded / total_size) * 100
                            print(
                                f"\r下载进度: {progress:.1f}% ({downloaded}/{total_size})",
                                end="",
                            )

            print()  # 换行

            # 验证校验和
            with open(download_path, "rb") as f:
                file_data = f.read()
                file_md5 = hashlib.md5(file_data).hexdigest()

            if file_md5 == firmware_info["md5"]:
                logger.info("✅ 固件下载并验证成功")
                return True
            else:
                logger.error("❌ 固件校验失败")
                return False

        except Exception as e:
            logger.error(f"❌ 固件下载失败: {e}")
            return False

    def install_firmware(self, firmware_info: Dict) -> bool:
        """安装固件（模拟）"""
        logger.info(f"🔧 安装固件: {firmware_info['filename']}")

        # 模拟安装过程
        for i in range(5):
            time.sleep(0.5)
            print(f"\r安装进度: {(i+1)*20}%", end="")

        print()  # 换行

        # 模拟安装成功
        self.current_version = firmware_info["version"]
        logger.info(f"✅ 固件安装成功，当前版本: {self.current_version}")
        return True


class OTATestSuite:
    """OTA测试套件"""

    def __init__(self):
        self.server = OTATestServer()
        self.firmware_manager = FirmwareManager("./firmware")
        self.device = DeviceSimulator("DTU_TEST_001")

    def setup(self):
        """设置测试环境"""
        logger.info("🚀 设置OTA测试环境...")
        self.server.start()
        time.sleep(1)  # 等待服务器启动

    def teardown(self):
        """清理测试环境"""
        logger.info("🧹 清理测试环境...")
        self.server.stop()

    def test_firmware_generation(self):
        """测试固件生成"""
        logger.info("🧪 测试: 固件生成")

        firmware_info = self.firmware_manager.generate_test_firmware("1.1.0", 128)
        assert firmware_info["version"] == "1.1.0"
        assert firmware_info["size"] == 128 * 1024

        logger.info("✅ 固件生成测试通过")

    def test_update_check(self):
        """测试更新检查"""
        logger.info("🧪 测试: 更新检查")

        # 生成新版本固件
        self.firmware_manager.generate_test_firmware("1.2.0", 128)

        # 检查更新
        update_info = self.device.check_update()
        assert update_info is not None
        assert update_info["version"] == "1.2.0"

        logger.info("✅ 更新检查测试通过")
        return update_info

    def test_firmware_download(self):
        """测试固件下载"""
        logger.info("🧪 测试: 固件下载")

        # 生成测试固件
        firmware_info = self.firmware_manager.generate_test_firmware("1.3.0", 64)

        # 下载固件
        success = self.device.download_firmware(firmware_info)
        assert success

        logger.info("✅ 固件下载测试通过")
        return firmware_info

    def test_firmware_install(self):
        """测试固件安装"""
        logger.info("🧪 测试: 固件安装")

        firmware_info = self.firmware_manager.generate_test_firmware("1.4.0", 64)
        old_version = self.device.current_version

        # 安装固件
        success = self.device.install_firmware(firmware_info)
        assert success
        assert self.device.current_version != old_version

        logger.info("✅ 固件安装测试通过")

    def test_full_upgrade_process(self):
        """测试完整升级流程"""
        logger.info("🧪 测试: 完整升级流程")

        # 1. 生成新固件
        firmware_info = self.firmware_manager.generate_test_firmware("2.0.0", 100)

        # 2. 检查更新
        update_info = self.device.check_update()
        assert update_info is not None

        # 3. 下载固件
        download_success = self.device.download_firmware(update_info)
        assert download_success

        # 4. 安装固件
        install_success = self.device.install_firmware(update_info)
        assert install_success

        # 5. 验证版本
        assert self.device.current_version == "2.0.0"

        logger.info("✅ 完整升级流程测试通过")

    def run_all_tests(self):
        """运行所有测试"""
        logger.info("🎯 开始运行OTA测试套件...")

        try:
            self.setup()

            self.test_firmware_generation()
            self.test_update_check()
            self.test_firmware_download()
            self.test_firmware_install()
            self.test_full_upgrade_process()

            logger.info("🎉 所有测试通过！")

        except Exception as e:
            logger.error(f"❌ 测试失败: {e}")
            raise
        finally:
            self.teardown()


def main():
    """主函数"""
    parser = argparse.ArgumentParser(description="冷库DTU OTA测试工具")
    parser.add_argument(
        "command", choices=["test", "server", "generate", "list"], help="执行的命令"
    )
    parser.add_argument("--version", "-v", default="1.0.0", help="固件版本")
    parser.add_argument("--size", "-s", type=int, default=100, help="固件大小(KB)")
    parser.add_argument("--port", "-p", type=int, default=8080, help="服务器端口")

    args = parser.parse_args()

    if args.command == "test":
        # 运行测试
        test_suite = OTATestSuite()
        test_suite.run_all_tests()

    elif args.command == "server":
        # 启动服务器
        server = OTATestServer(args.port)
        try:
            server.start()
            print(f"服务器运行在 http://localhost:{args.port}")
            print("按 Ctrl+C 停止服务器")
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            server.stop()

    elif args.command == "generate":
        # 生成固件
        manager = FirmwareManager("./firmware")
        firmware_info = manager.generate_test_firmware(args.version, args.size)
        print(f"生成固件: {firmware_info['filename']}")
        print(f"MD5: {firmware_info['md5']}")
        print(f"SHA256: {firmware_info['sha256']}")

    elif args.command == "list":
        # 列出固件
        manager = FirmwareManager("./firmware")
        firmware_list = manager.list_firmware()

        if firmware_list:
            print("可用固件:")
            for fw in firmware_list:
                print(f"  {fw['filename']} (v{fw['version']}, {fw['size']} bytes)")
        else:
            print("没有找到固件文件")


if __name__ == "__main__":
    main()
