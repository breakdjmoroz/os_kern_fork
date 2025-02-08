#include "user.h"

#define SECTOR_SIZE (512)
#define blk_capacity (1024)

void compile(char*, unsigned);

char convert_ascii(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    c &= 0b11011111;
    if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    return 127;
}


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

/* hex0 shell program */
void compile_hex0(const char* input_filename, const char* output_filename, unsigned file_size)
{
  char buf[file_size] = {};

  readfile(input_filename, buf, sizeof(buf));

  compile(buf, file_size / SECTOR_SIZE);

  writefile(output_filename, buf, sizeof(buf));
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

void compile(char* disk, unsigned disk_size_in_sectors)
{
    char buf[SECTOR_SIZE]         = {};
    char code[SECTOR_SIZE]        = {}; 

    unsigned current_sector       = 0;
    unsigned current_code_sector  = 0;

    char current_char             = '\0';
    char is_first_digit           = true;

    unsigned j = 0; /* code[] index */

    while (current_sector < disk_size_in_sectors)
    {
      memcpy(buf, &disk[current_sector * SECTOR_SIZE], SECTOR_SIZE); /* read sector in buf*/

      for (unsigned i = 0; i < SECTOR_SIZE; ++i)
      {
        current_char = convert_ascii(buf[i]);

        if (current_char != 127) /* 127 - convert_ascii error value*/
        {
          if (is_first_digit)
          {
            code[j] = current_char << 4; /* upper 4 bits of the byte */

            is_first_digit = false;
          }
          else
          {
            code[j] |= current_char; /* lower 4 bits of the byte */

            ++j;

            if (j == SECTOR_SIZE)
            {
              memcpy(&disk[current_code_sector * SECTOR_SIZE], code, SECTOR_SIZE); /* write code sector */
              ++current_code_sector;
              j = 0;
            }

            is_first_digit = true;
          }
        }
      }

      ++current_sector;
    }

    char clear[512] = {};
    unsigned current_clear_sector = current_code_sector + 1;

    while (j < SECTOR_SIZE)
    {
      code[j] = 0;
      ++j;
    }
    memcpy(&disk[current_code_sector * SECTOR_SIZE], code, SECTOR_SIZE); /* fill last code sector with zeros */

    while (current_clear_sector * SECTOR_SIZE < blk_capacity)
    {
      memcpy(&disk[current_clear_sector * SECTOR_SIZE], clear, SECTOR_SIZE);
      ++current_clear_sector;
    } 
}
