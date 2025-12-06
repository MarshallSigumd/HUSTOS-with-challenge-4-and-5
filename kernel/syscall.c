/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"
#include "elf.h"

#include "spike_interface/spike_utils.h"

extern elf_ctx elfloader;

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char *buf, size_t n)
{
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code)
{
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process).
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}

ssize_t sys_user_print_backtrace(int64 depth)
{
  int i;
  const uint64 margin = 40;
  uint64 current_depth = 0;
  uint64 sp = margin + current->trapframe->regs.sp;
  uint64 fp = current->trapframe->regs.s0;
  while (current_depth < depth)
  {
    sp = fp + 8;
    uint64 ra = *(uint64 *)sp; // sp points to ra
    fp = (*(uint64 *)fp) - 16; // recurse to next fp
    current_depth++;
    // find function symbols then names
    int string_table_index = -1;
    uint64 pre_ra = 0;
    for (i = 0; i < elfloader.symbol_num; i++)
    {
      elf_symbol *func_symbol = &elfloader.symbols[i];
      if (func_symbol->info != STT_FUNC) // not a function
      {
        continue;
      }
      if (func_symbol->value > pre_ra && func_symbol->value < ra) // found a valid function symbol
      {
        string_table_index = i;
        pre_ra = func_symbol->value;
      }
    }
    elf_symbol *func_symbol = &elfloader.symbols[string_table_index];
    sprint("%s\n", &elfloader.string_table[func_symbol->name]);
    if (strcmp("main", &elfloader.string_table[func_symbol->name]) == 0) // this is the end of the user stack
    {
      break;
    }
  }
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7)
{
  switch (a0)
  {
  case SYS_user_print:
    return sys_user_print((const char *)a1, a2);
  case SYS_user_exit:
    return sys_user_exit(a1);
  case SYS_user_print_backtrace:
    return sys_user_print_backtrace(a1);
  default:
    panic("Unknown syscall %ld \n", a0);
  }
}
