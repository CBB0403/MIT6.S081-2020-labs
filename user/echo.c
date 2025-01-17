#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// int
// main(int argc, char *argv[])
// {
//   int i;

//   for(i = 1; i < argc; i++){
//     write(1, argv[i], strlen(argv[i]));
//     if(i + 1 < argc){
//       write(1, " ", 1);
//     } else {
//       write(1, "\n", 1);
//     }
//   }
//   exit(0);
// }

// 由于这里的echo无法识别\n换行符，所以这里实现了个新的函数，对\n进行转义
int main(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        for (char *p = argv[i]; *p; p++) {
            if (*p == '\\' && *(p + 1) == 'n') {
                write(1, "\n", 1);
                p++;
            } else if (*p != '"') {  // 忽略引号
                write(1, p, 1);
            }
        }
        if (i < argc - 1) {
            write(1, " ", 1);
        }
    }
    write(1, "\n", 1);
    exit(0);
}
