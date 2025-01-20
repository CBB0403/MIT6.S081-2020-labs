// =====================================================================================
//
//       Filename:  test_end.c
//
//    Description:  测试系统是大端还是小端
//
//        Version:  1.0
//        Created:  01/20/2025 11:27:37
//       Revision:  none
//       Compiler:  g++
//
//         Author:  binbin.chen (), binbin.chen0403@gmail.com
//   Organization:
//
// =====================================================================================

#include <stdio.h>
// 返回1，小端
// 返回0，大端
int check_sys() {
    int i = 1;
    return (*(char *)&i);
}

int main() {
    int ret = check_sys();
    if (ret == 1)
        printf("小端\n");
    else
        printf("大端\n");
    return 0;
}
