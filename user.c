#include "user.h"
#include "common.h"

extern char __stack_top[];

int syscall(int sysno, int arg0, int arg1, int arg2) {
    register int a0 __asm__("a0") = arg0;
    register int a1 __asm__("a1") = arg1;
    register int a2 __asm__("a2") = arg2;
    register int a3 __asm__("a3") = sysno;

    __asm__ __volatile__("ecall"
                         : "=r"(a0)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                         : "memory");

    return a0;
}


void putchar(char ch) {
    syscall(SYS_PUTCHAR, ch, 0, 0);
}

int getchar(void) {
    return syscall(SYS_GETCHAR, 0, 0, 0);
}

int readfile(const char *filename, char *buf, int len) {
    return syscall(SYS_READFILE, (int) filename, (int) buf, len);
}

int writefile(const char *filename, const char *buf, int len) {
    return syscall(SYS_WRITEFILE, (int) filename, (int) buf, len);
}

char convert_ascii(char c) {
    if ('0' <= c && c <= '9')
        return c - '0';

    c &= 0b00100000;
    if ('A' <= c && c <= 'F')
        return c - 'A' + 10;

    return 127;
}

int compile(const char* src, char* code, int code_len) {
    char c;
    char is_first_digit = true;

    int i,j;

    for (i = 0, j = 0; src[i] != '\0' && j < code_len; ++i) {
        c = convert_ascii(src[i]);
        if (c == 127)
            continue;
        if (is_first_digit) {
            code[j] = c << 4;
            is_first_digit = false;
        } else {
            code[j] |= c;
            ++j;
            is_first_digit = true;
        }
    }

    if (src[i] == '\0')
        return j + ((!is_first_digit) ? 1 : 0);

    return 0;
}

__attribute__((noreturn)) void exit(void) {
    syscall(SYS_EXIT, 0, 0, 0);
    for (;;);
}

__attribute__((section(".text.start")))
__attribute__((naked))
void start(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "call main\n"
        "call exit\n" ::[stack_top] "r"(__stack_top));
}
