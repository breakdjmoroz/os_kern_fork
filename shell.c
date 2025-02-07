#include "user.h":

void main(void) {
    while (1) {
prompt:
        printf("> ");
        char cmdline[128];
        int cmdline_len;
        for (int i = 0;; i++) {
            char ch = getchar();
            putchar(ch);
            if (i == sizeof(cmdline) - 1) {
                printf("command line too long\n");
                goto prompt;
            } else if (ch == '\r') {
                printf("\n");
                cmdline[i] = '\0';
                cmdline_len = i;
                break;
            } else {
                cmdline[i] = ch;
            }
        }

        if (strcmp(cmdline, "hello") == 0)
            printf("Hello world from shell!\n");
        else if (strcmp(cmdline, "exit") == 0)
            exit();
        else if (strcmp(cmdline, "compile") == 0) {
            char src[8192];
            int src_len = readfile("full_kernel.hex", src, sizeof(src));
            src[src_len] = '\0';

            char code[8192];
            int code_len = compile(src, code, 8192);
            code[code_len] = '\0';

            writefile("full_kernel.hex", code, code_len); 
        }
        else if (strcmp(cmdline, "writefile") == 0)
            writefile("hello.txt", "Hello from shell!\n", 19);
        else if (strcmp(cmdline, "readfile") == 0) {
            char buf[128];
            int len = readfile("hello.txt", buf, sizeof(buf));
            buf[len] = '\0';
        } else
            printf("unknown command: %s\n", cmdline);
    }
}
