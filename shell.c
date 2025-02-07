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
        {
            printf("Hello world from shell!\n");
        }
        else if (strcmp(cmdline, "exit") == 0)
        {
            exit();
        }
        else if (strcmp(cmdline, "readfile") == 0)
        {
            char buf[512 * 4];
            int len = readfile("program.hex0", buf, sizeof(buf));
            buf[len] = '\0';
            printf("%s\n", buf);
        }
        else if (strcmp(cmdline, "writefile") == 0)
        {
            writefile("program.hex0", "Hello from shell!\n", 19);
        }
        else if (strcmp(cmdline, "hex0") == 0)
        {
          compile_hex0("program.hex0", "program.bin", 512 * 16); /* 16 sectors of 512 bytes */
        }
        else
        {
            printf("unknown command: %s\n", cmdline);
        }
    }
}
