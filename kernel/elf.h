#ifndef _ELF_H_
#define _ELF_H_

#include "util/types.h"
#include "process.h"

#define MAX_CMDLINE_ARGS 64

// elf header structure
typedef struct elf_header_t
{
  uint32 magic;
  uint8 elf[12];
  uint16 type;      /* Object file type */
  uint16 machine;   /* Architecture */
  uint32 version;   /* Object file version */
  uint64 entry;     /* Entry point virtual address */
  uint64 phoff;     /* Program header table file offset */
  uint64 shoff;     /* Section header table file offset */
  uint32 flags;     /* Processor-specific flags */
  uint16 ehsize;    /* ELF header size in bytes */
  uint16 phentsize; /* Program header table entry size */
  uint16 phnum;     /* Program header table entry count */
  uint16 shentsize; /* Section header table entry size */
  uint16 shnum;     /* Section header table entry count */
  uint16 shstrndx;  /* Section header string table index */
} elf_header;       // ELF文件头结构体，用于描述ELF文件的文件头（ELF header）。

// Program segment header.
typedef struct elf_prog_header_t
{
  uint32 type;     /* Segment type */
  uint32 flags;    /* Segment flags */
  uint64 off;      /* Segment file offset */
  uint64 vaddr;    /* Segment virtual address */
  uint64 paddr;    /* Segment physical address */
  uint64 filesz;   /* Segment size in file */
  uint64 memsz;    /* Segment size in memory */
  uint64 align;    /* Segment alignment */
} elf_prog_header; // 程序段头结构体，用于描述ELF文件中的程序段（program segment）。

typedef struct elf_section_header_t
{
  uint32 name;        /* Section name (string tbl index) */
  uint32 type;        /* Section type */
  uint64 flags;       /* Section flags */
  uint64 addr;        /* Section virtual addr at execution */
  uint64 offset;      /* Section file offset */
  uint64 size;        /* Section size in bytes */
  uint32 link;        /* Link to another section */
  uint32 info;        /* Additional section information */
  uint64 addralign;   /* Section alignment */
  uint64 entsize;     /* Entry size if section holds table */
} elf_section_header; // 节表头结构体，用于描述ELF文件中的节（section）。

typedef struct elf_symbol_t
{
  uint32 name;  /* Symbol name (string tbl index) */
  uint8 info;   /* Symbol type and binding */
  uint8 other;  /* Symbol visibility */
  uint16 shndx; /* Section index */
  uint64 value; /* Symbol value */
  uint64 size;  /* Symbol size */
} elf_symbol;   // 符号表结构体，用于描述ELF文件中的符号（symbol）。

#define ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian
#define ELF_PROG_LOAD 1
#define SYM_TABLE 2            /* Symbol table */
#define STR_TABLE 3            /* String table */
#define STT_FILE 4             /* Symbol's name is file name */
#define STT_FUNC 18            /* Symbol's name is function name*/
#define STR_TABLE_MAX_LEN 8192 /* max length of string table*/
#define SYMS_MAX_LEN 512       /*max length of symbols*/

typedef enum elf_status_t
{
  EL_OK = 0, // 成功

  EL_EIO,    // 读取文件错误
  EL_ENOMEM, // 内存不足错误
  EL_NOTELF, // 不是ELF文件
  EL_ERR,    // 错误

} elf_status; // 枚举类型，用于表示ELF加载过程中的错误状态。

typedef struct elf_ctx_t // ELF 加载器的上下文结构体，用于在加载过程中保存状态和传递信息。
{
  void *info;                           // 信息
  char string_table[STR_TABLE_MAX_LEN]; // 字符串表
  elf_header ehdr;                      // ELF 文件头
  elf_symbol symbols[SYMS_MAX_LEN];     // 符号表
  uint64 symbol_num;                    // 符号表中的符号数量
} elf_ctx;                              // ELF 加载器的上下文结构体，用于在加载过程中保存状态和传递信息。

elf_status elf_init(elf_ctx *ctx, void *info); // 初始化 ELF 加载器
elf_status elf_load(elf_ctx *ctx);             // 加载 ELF 文件

void load_bincode_from_host_elf(process *p); // 从主机 ELF 文件加载二进制代码

#endif
