# cmake-kernel

用于构建内核的构建系统

构建系统提供的功能：

- [ ] 跨平台

    提供 gcc/clang 的支持

    需要独立的  CMAKE_TOOLCHAIN_FILE 指定编译器等

    组合如下

    |      host      | target  |              gcc              | clang |
    | :------------: | :-----: | :---------------------------: | :---: |
    |   x86_64-osx   | x86_64  |        x86_64_elf-gcc         | TEST  |
    |   x86_64-osx   | aarch64 | aarch64-unknown-linux-gnu-gcc | TEST  |
    |   x86_64-osx   | riscv64 |    riscv64-unknown-elf-gcc    | TEST  |
    |  aarch64-osx   | x86_64  |             TODO              | TODO  |
    |  aarch64-osx   | aarch64 |             TODO              | TODO  |
    |  aarch64-osx   | riscv64 |             TODO              | TODO  |
    | x86_64-ubuntu  | x86_64  |              gcc              | TEST  |
    | x86_64-ubuntu  | aarch64 |     aarch64-linux-gnu-gcc     | TEST  |
    | x86_64-ubuntu  | riscv64 |     riscv64-linux-gnu-gcc     | TEST  |
    | aarch64-ubuntu | x86_64  |             TODO              | TEST  |
    | aarch64-ubuntu | aarch64 |             TODO              | TEST  |
    | aarch64-ubuntu | riscv64 |             TODO              | TEST  |
    | riscv64-ubuntu | x86_64  |             TODO              | TODO  |
    | riscv64-ubuntu | aarch64 |             TODO              | TODO  |
    | riscv64-ubuntu | riscv64 |             TODO              | TODO  |

    host: 编译平台
    target: 内核要运行的平台
    gcc: 使用的 gcc 名称
    clang: 使用的 clang 参数
    TODO: 暂无测试条件
    TEST: 需要进一步验证。

- [ ] 对第三方依赖的支持/构建

    自动下载并编译第三方依赖

    自动生成相关 licence

    预期使用的第三方内容

    |                          第三方内容                          |                     描述                      |     类型     |
    | :----------------------------------------------------------: | :-------------------------------------------: | :----------: |
    |        [CPM](https://github.com/cpm-cmake/CPM.cmake)         |                 cmake 包管理                  | cmake module |
    | [CPMLicences.cmake](https://github.com/TheLartians/CPMLicenses.cmake) |            为第三方包生成 licence             | cmake module |
    |  [google/googletest](https://github.com/google/googletest)   |                     测试                      |      库      |
    |   [easylogingpp](https://github.com/amrayn/easyloggingpp)    |                     日志                      |      库      |
    |           [rttr](https://github.com/rttrorg/rttr)            |         c++ 反射库，在设备驱动部分用          |      库      |
    | [Format.cmake](https://github.com/TheLartians/Format.cmake)  | 代码格式化，支持 clang-format 与 cmake-format | cmake module |
    |        [FreeImage](http://freeimage.sourceforge.net/)        |                   图片渲染                    |      库      |
    |            [Freetype](https://www.freetype.org/)             |                   字体渲染                    |      库      |
    |   [opensbi](https://github.com/riscv-software-src/opensbi)   |                  riscv 引导                   |      库      |
    |     [gnu-efi](https://sourceforge.net/projects/gnu-efi/)     |                 gnu uefi 引导                 |      库      |
    |      [posix-uefi](https://gitlab.com/bztsrc/posix-uefi)      |                posix uefi 引导                |      库      |
    |                [ovmf](SimpleKernel/3rd/ovmf)                 |             qemu 使用的 uefi 固件             |     bin      |
    |          [edk2](https://github.com/tianocore/edk2)           |        构建 qemu 使用的 uefi 固件 ovmf        |      库      |

    

- [ ] 文档生成

    doxygen

    1. 使用 doxygen 从注释生成文档
    2. doxygen 配置文件

- [ ] 构建内核

    生成内核 elf 文件

- [ ] 运行内核

    在 qemu 上运行内核

- [ ] 代码格式化

    格式化全部代码

- [ ] 单元测试

    测试

- [ ] CI

    github action、codecov

- [ ] 定义项目信息

    版本号、自动生成头文件

- [ ] 指定输出目录

    将第三方依赖、内核等编译生成的文件放到指定位置

- [ ] 调试

    使用 make debug 等进入调试模式

- [ ] 工具链的构建

    gcc/qemu/docker

- [ ] IDE 支持

    vscode，Clion
