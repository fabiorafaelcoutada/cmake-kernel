
/**
 * @file libc.cpp
 * @brief libc c
 * @author Zone.N (Zone.Niuzh@hotmail.com)
 * @version 1.0
 * @date 2023-07-15
 * @copyright MIT LICENSE
 * https://github.com/MRNIU/cmake-kernel
 * @par change log:
 * <table>
 * <tr><th>Date<th>Author<th>Description
 * <tr><td>2023-07-15<td>Zone.N (Zone.Niuzh@hotmail.com)<td>创建文件
 * </table>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "libc.h"

int32_t libc(uint32_t _argc, uint8_t** _argv) {
    (void)_argc;
    (void)_argv;

    // 进入死循环
    while (1) {
        ;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
