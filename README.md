# Huffman-compress

C++11 交互式 Huffman 文件压缩、解压程序。使用自定义文件格式，支持输出字符编码表。

## 环境与构建

需要 Git、CMake 3.16+、C++11 编译器；运行自动测试另需 Python 3.8+，只使用标准库。

```powershell
git clone https://github.com/L1ttle-Q/Huffman-compress.git
cd Huffman-compress
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
.\build\huffman.exe
```

上例用于 Windows + MinGW-w64。Linux/macOS 配置时省略 `-G "MinGW Makefiles"`，运行 `./build/huffman`。Visual Studio 可使用独立目录 `build-vs`，构建加 `--config Release`，程序通常在 `build-vs/Release/huffman.exe`。

仅构建程序、不安装 Python 时，配置追加 `-DBUILD_TESTING=OFF`。若 CMake 找不到 Python，可传入 `-DPython3_EXECUTABLE="完整的 Python 路径"`。

## 压缩、解压一个文件

从仓库根目录启动程序后，依次输入：

```text
1
test/1/in.txt
compressed.bin
2
compressed.bin
restored.txt
4
```

菜单 `1` 为压缩，`2` 为解压，`3` 切换字典显示，`4` 退出。所有文件路径都相对于进程工作目录；输出路径会以写入模式打开，可能覆盖已有文件。

压缩文件保存字典、原始数据长度及位编码，可使用任意后缀；它不是 ZIP 格式，不能用 ZIP 工具解压。仓库 `test/5/in.txt.zip` 也不是程序运行所必需。

## 自动测试

```powershell
ctest --test-dir build --output-on-failure
# 只运行原有的连续压缩、解压用例
ctest --test-dir build -R huffman_fixture_3 --output-on-failure
# 不通过 CTest，直接运行全部测试
python tests/run_tests.py --executable build/huffman.exe
```

Visual Studio 构建时，CTest 追加 `-C Release`；直接调用脚本时改为 `build-vs/Release/huffman.exe`。Linux/macOS 的脚本参数改为 `build/huffman`。

测试包含原有 6 组输入（第 3 组在同一进程内连续压缩、解压），以及空文件、单一字节重复、完整 0–255 字节范围三组回归，共 **9 项**。脚本在临时目录运行，重新生成输出并进行逐字节比较；程序异常、超时、缺少输出或内容不同都返回非零。默认每次程序调用超时 60 秒，可用 `--timeout` 修改。

原有 `test.cpp` 作为手动单例入口保留；推荐使用 `tests/run_tests.py` / CTest，它们不会复用源码目录中的历史输出。新增 fixture 时需提供 `test/N/in.txt` 与包含压缩/解压命令的 `test/N/test.txt`，并同步扩展 CMake 和脚本的用例范围。

## 目录与已知限制

- `src/main.cpp`：菜单；`include/huffman.h`：编码；`include/decompress.h`：解码；`include/Util.h`：位流和文件输入输出。
- 输入使用固定长度缓冲区及 `%s`，请使用短于 50 字节且不含空格的相对文件名。
- 文件格式直接保存整数的内存表示，没有版本标记；未保证跨字节序或其他实现的兼容性。
- 对损坏压缩流和错误交互输入的校验仍不完整；测试通过不代表任意文件输入都有完善错误恢复。
- 本次修复了编码器状态未初始化、单字符编码树处理及连续解压时位读取状态未重置，并补齐可移植字节类型定义。

## 验证记录与许可证

2026-10-26：Windows、MinGW-w64 GCC 8.1、CMake 3.31、Python 3.14 下，构建及 9 项 CTest 通过。其他系统尚未在本轮实测。

使用 [Apache License 2.0](LICENSE)。
