/* This is a platform independent C89 x86 assembler(in development), disassembler(almost done), emulator(in development) and length disassembler library.

Features:
 - Suppport for Intel and AT&T syntax.
 - Support for x86(16/32/64).
 - No dynamic memory allocation, global variables/state/context or runtime initialization.
 - Thread-safe by design.
 - Optimized for speed, size and low memory usage.
 - The only dependencies are <stdbool.h>, <stdint.h> and <stddef.h>. Check out the 'NMD_ASSEMBLY_NO_INCLUDES' macro.

Setup:
Define the 'NMD_ASSEMBLY_IMPLEMENTATION' macro in one source file before the include statement to instantiate the implementation.
#define NMD_ASSEMBLY_IMPLEMENTATION
#include "nmd_assembly.h"

Using absolutely no dependencies(other headers...):
Define the 'NMD_ASSEMBLY_NO_INCLUDES' macro to tell the library not to include any headers. By doing so it will define the required types.
Be aware: This feature uses platform dependent macros.

Interfaces(i.e the functions you call from your application):
 - The assembler is represented by the following function:
    Assembles an instruction from a string. Returns the number of bytes written to the buffer on success, zero otherwise. Instructions can be separated using either the ';' or '\n' character.
    Parameters:
     - string         [in]         A pointer to a string that represents one or more instructions in assembly language.
     - buffer         [out]        A pointer to a buffer that receives the encoded instructions.
     - bufferSize     [in]         The size of the buffer in bytes.
     - runtimeAddress [in]         The instruction's runtime address. You may use 'NMD_X86_INVALID_RUNTIME_ADDRESS'.
     - mode           [in]         The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
     - count          [in/out/opt] A pointer to a variable that on input is the maximum number of instructions that can be parsed(or zero for unlimited instructions), and on output is the number of instructions parsed. This parameter may be zero.
    size_t nmd_x86_assemble(const char* string, void* buffer, size_t bufferSize, uint64_t runtimeAddress, NMD_X86_MODE mode, size_t* const count);

 - The disassembler is composed of a decoder and a formatter represented by these two functions respectively:
	- Decodes an instruction. Returns true if the instruction is valid, false otherwise.
      Parameters:
       - buffer      [in]  A pointer to a buffer containing an encoded instruction.
       - bufferSize  [in]  The size of the buffer in bytes.
       - instruction [out] A pointer to a variable of type 'nmd_x86_instruction' that receives information about the instruction.
       - mode        [in]  The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
       - flags       [in]  A mask of 'NMD_X86_DECODER_FLAGS_XXX' that specifies which features the decoder is allowed to use. If uncertain, use 'NMD_X86_DECODER_FLAGS_MINIMAL'.
      bool nmd_x86_decode_buffer(const void* buffer, size_t bufferSize, nmd_x86_instruction* instruction, NMD_X86_MODE mode, uint32_t flags);

    - Formats an instruction. This function may cause a crash if you modify 'instruction' manually.
      Parameters:
       - instruction    [in]  A pointer to a variable of type 'nmd_x86_instruction' describing the instruction to be formatted.
       - buffer         [out] A pointer to buffer that receives the string. The buffer's recommended size is 128 bytes.
       - runtimeAddress [in]  The instruction's runtime address. You may use 'NMD_X86_INVALID_RUNTIME_ADDRESS'.
       - formatFlags    [in]  A mask of 'NMD_X86_FORMAT_FLAGS_XXX' that specifies how the function should format the instruction. If uncertain, use 'NMD_X86_FORMAT_FLAGS_DEFAULT'.
      void nmd_x86_format_instruction(const nmd_x86_instruction* instruction, char buffer[], uint64_t runtimeAddress, uint32_t formatFlags);

 - The emulator is represented by the following function:
    Emulates x86 code according to the state of the cpu. You MUST initialize the following variables before calling this
    function: 'cpu->mode', 'cpu->physicalMemory', 'cpu->physicalMemorySize', 'cpu->virtualAddress' and 'cpu->rip'.
    You may optionally initialize 'cpu->rsp' if a stack is desirable. Below is a short description of each variable:
     - 'cpu->mode': The emulator's operating architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
     - 'cpu->physicalMemory': A pointer to a buffer used as the emulator's memory.
     - 'cpu->physicalMemorySize': The size of the 'physicalMemory' buffer in bytes.
     - 'cpu->virtualAddress': The starting address of the emulator's virtual address space.
     - 'cpu->rip': The virtual address where emulation starts.
     - 'cpu->rsp': The virtual address of the bottom of the stack.
    Parameters:
     - cpu      [in] A pointer to a variable of type 'nmd_x86_cpu' that holds the state of the cpu.
     - maxCount [in] The maximum number of instructions that can be executed, or zero for unlimited instructions.
    bool nmd_x86_emulate(nmd_x86_cpu* cpu, size_t maxCount);

 - The length disassembler is represented by the following function:
    Returns the length of the instruction if it is valid, zero otherwise.
    Parameters:
     - buffer     [in] A pointer to a buffer containing an encoded instruction.
     - bufferSize [in] The size of the buffer in bytes.
     - mode       [in] The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
    size_t nmd_x86_ldisasm(const void* buffer, size_t bufferSize, NMD_X86_MODE mode);

Enabling and disabling features of the decoder:
To dynamically choose which features are used by the decoder, use the 'flags' parameter of nmd_x86_decode_buffer(). The less features specified in the mask, the
faster the decoder runs. By default all features are available, some can be completely disabled at compile time(thus reducing code size and increasing code speed) by defining
the following macros(in the same place the macro 'NMD_ASSEMBLY_IMPLEMENTATION' is defined):
 - 'NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK': the decoder does not check if the instruction is invalid.
 - 'NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID': the decoder does not fill the 'id' variable.
 - 'NMD_ASSEMBLY_DISABLE_DECODER_CPU_FLAGS': the decoder does not fill the 'cpuFlags' variable.
 - 'NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS': the decoder does not fill the 'numOperands' and 'operands' variable.
 - 'NMD_ASSEMBLY_DISABLE_DECODER_GROUP': the decoder does not fill the 'group' variable.
 - 'NMD_ASSEMBLY_DISABLE_DECODER_VEX': the decoder does not support VEX instructions.
 - 'NMD_ASSEMBLY_DISABLE_DECODER_EVEX': the decoder does not support EVEX instructions.
 - 'NMD_ASSEMBLY_DISABLE_DECODER_3DNOW': the decoder does not support 3DNow! instructions.

Enabling and disabling features of the formatter:
To dynamically choose which features are used by the formatter, use the 'formatFlags' parameter of nmd_x86_format_instruction(). The less features specified in the mask, the
faster the function runs. By default all features are available, some can be completely disabled at compile time(thus reducing code size and increasing code speed) by defining
the following macros(in the same place the macro 'NMD_ASSEMBLY_IMPLEMENTATION' is defined):
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_POINTER_SIZE': the formatter does not support pointer size.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_BYTES: the formatter does not support instruction bytes. You may define the 'NMD_X86_FORMATTER_NUM_PADDING_BYTES' macro to be the number of bytes used as space padding.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_ATT_SYNTAX: the formatter does not support AT&T syntax.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_UPPERCASE: the formatter does not support uppercase.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_COMMA_SPACES: the formatter does not support comma spaces.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_OPERATOR_SPACES: the formatter does not support operator spaces.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_VEX': the formatter does not support VEX instructions.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_EVEX': the formatter does not support EVEX instructions.
 - 'NMD_ASSEMBLY_DISABLE_FORMATTER_3DNOW': the formatter does not support 3DNow! instructions.

Enabling and disabling feature of the length disassembler:
Use the following macros to disable features at compile-time:
 - 'NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK': the length disassembler does not check if the instruction is invalid.
 - 'NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VEX': the length disassembler does not support VEX instructions.
 - 'NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_3DNOW': the length disassembler does not support 3DNow! instructions.

Conventions:
 - Every identifier uses snake case.
 - Enums and macros are uppercase, every other identifier is lowercase.
 - Non-internal identifiers start with the 'NMD_' prefix.
 - Internal identifiers start with the '_NMD_' prefix.

TODO:
 Short-Term
  - implement instruction set extensions to the decoder : VEX, EVEX, MVEX, 3DNOW, XOP.
  - Implement x86 assembler and emulator.
 Long-Term
  - Add support for other architectures(ARM, MIPS and PowerPC ?).

References:
 - Intel 64 and IA-32 Architectures. Software Developer's Manual Volume 2 (2A, 2B, 2C & 2D): Instruction Set Reference, A-Z.
   - Chapter 2 Instruction Format.
   - Chapter 3-5 Instruction set reference.
   - Appendix A Opcode Map.
   - Appendix B.16 Instruction and Formats and Encoding.
 - 3DNow! Technology Manual.
 - AMD Extensions to the 3DNow! and MMX Instruction Sets Manual.
 - Intel Architecture Instruction Set Extensions and Future Features Programming Reference.
 - Capstone Engine.
 - Zydis Disassembler.
 - VIA PadLock Programming Guide.

Contributors(This may not be a complete list):
 - Nomade: Founder and maintainer.
 - Darkratos: Bug reporting and feature suggesting.
*/

#ifndef NMD_ASSEMBLY_H
#define NMD_ASSEMBLY_H

#define NMD_X86_MAXIMUM_INSTRUCTION_STRING_LENGTH 128

#ifdef NMD_ASSEMBLY_NO_INCLUDES

#ifndef __cplusplus

#define bool  _Bool
#define false 0
#define true  1

#endif /* __cplusplus */

typedef signed char        int8_t;
typedef unsigned char      uint8_t;

typedef signed short       int16_t;
typedef unsigned short     uint16_t;

typedef signed int         int32_t;
typedef unsigned int       uint32_t;

typedef signed long long   int64_t;
typedef unsigned long long uint64_t;

#if defined(_WIN64) && defined(_MSC_VER)
	typedef unsigned __int64 size_t;
	typedef __int64          ptrdiff_t;
#elif (defined(_WIN32) || defined(WIN32)) && defined(_MSC_VER)
	typedef unsigned __int32 size_t
	typedef __int32          ptrdiff_t;
#elif defined(__GNUC__) || defined(__clang__)
	#if defined(__x86_64__) || defined(__ppc64__)
		typedef unsigned long size_t
		typedef long          ptrdiff_t
	#else
		typedef unsigned int size_t
		typedef int          ptrdiff_t
	#endif
#else
	typedef unsigned long size_t
	typedef long          ptrdiff_t
#endif

#else

/* Dependencies when 'NMD_ASSEMBLY_NO_INCLUDES' is not defined. */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#endif /* NMD_ASSEMBLY_NO_INCLUDES */

#ifndef NMD_X86_FORMATTER_NUM_PADDING_BYTES
#define NMD_X86_FORMATTER_NUM_PADDING_BYTES 10
#endif /* NMD_X86_FORMATTER_NUM_PADDING_BYTES */

#define NMD_X86_INVALID_RUNTIME_ADDRESS -1
#define NMD_X86_MAXIMUM_INSTRUCTION_LENGTH 15
#define NMD_X86_MAXIMUM_NUM_OPERANDS 4

/* These flags specify how the formatter should work. */
enum NMD_X86_FORMATTER_FLAGS
{
	NMD_X86_FORMAT_FLAGS_HEX                       = (1 << 0),  /* If set, numbers are displayed in hex base, otherwise they are displayed in decimal base. */
	NMD_X86_FORMAT_FLAGS_POINTER_SIZE              = (1 << 1),  /* Pointer sizes(e.g. 'dword ptr', 'byte ptr') are displayed. */
	NMD_X86_FORMAT_FLAGS_ONLY_SEGMENT_OVERRIDE     = (1 << 2),  /* If set, only segment overrides using prefixes(e.g. '2EH', '64H') are displayed, otherwise a segment is always present before a memory operand. */
	NMD_X86_FORMAT_FLAGS_COMMA_SPACES              = (1 << 3),  /* A space is placed after a comma. */
	NMD_X86_FORMAT_FLAGS_OPERATOR_SPACES           = (1 << 4),  /* A space is placed before and after the '+' and '-' characters. */
	NMD_X86_FORMAT_FLAGS_UPPERCASE                 = (1 << 5),  /* The string is uppercase. */
	NMD_X86_FORMAT_FLAGS_0X_PREFIX                 = (1 << 6),  /* Hexadecimal numbers have the '0x'('0X' if uppercase) prefix. */
	NMD_X86_FORMAT_FLAGS_H_SUFFIX                  = (1 << 7),  /* Hexadecimal numbers have the 'h'('H' if uppercase') suffix. */
	NMD_X86_FORMAT_FLAGS_ENFORCE_HEX_ID            = (1 << 8),  /* If the HEX flag is set and either the prefix or suffix flag is also set, numbers less than 10 are displayed with preffix or suffix. */
	NMD_X86_FORMAT_FLAGS_HEX_LOWERCASE             = (1 << 9),  /* If the HEX flag is set and the UPPERCASE flag is not set, hexadecimal numbers are displayed in lowercase. */
	NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_MEMORY_VIEW = (1 << 10), /* If set, signed numbers are displayed as they are represented in memory(e.g. -1 = 0xFFFFFFFF). */
	NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_HINT_HEX    = (1 << 11), /* If set and NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_MEMORY_VIEW is also set, the number's hexadecimal representation is displayed in parenthesis. */
	NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_HINT_DEC    = (1 << 12), /* Same as NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_HINT_HEX, but the number is displayed in decimal base. */
	NMD_X86_FORMAT_FLAGS_SCALE_ONE                 = (1 << 13), /* If set, scale one is displayed. E.g. add byte ptr [eax+eax*1], al. */
	NMD_X86_FORMAT_FLAGS_BYTES                     = (1 << 14), /* The instruction's bytes are displayed before the instructions. */
	NMD_X86_FORMAT_FLAGS_ATT_SYNTAX                = (1 << 15), /* AT&T syntax is used instead of Intel's. */

	/* The formatter's default formatting style. */
	NMD_X86_FORMAT_FLAGS_DEFAULT  = (NMD_X86_FORMAT_FLAGS_HEX | NMD_X86_FORMAT_FLAGS_H_SUFFIX | NMD_X86_FORMAT_FLAGS_ONLY_SEGMENT_OVERRIDE | NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_MEMORY_VIEW | NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_HINT_DEC),
};

enum NMD_X86_DECODER_FLAGS
{
	NMD_X86_DECODER_FLAGS_VALIDITY_CHECK = (1 << 0), /* The decoder checks if the instruction is valid. */
	NMD_X86_DECODER_FLAGS_INSTRUCTION_ID = (1 << 1), /* The decoder fills the 'id' variable. */
	NMD_X86_DECODER_FLAGS_CPU_FLAGS      = (1 << 2), /* The decoder fills the 'cpuFlags' variable. */
	NMD_X86_DECODER_FLAGS_OPERANDS       = (1 << 3), /* The decoder fills the 'numOperands' and 'operands' variable. */
	NMD_X86_DECODER_FLAGS_GROUP          = (1 << 4), /* The decoder fills 'group' variable. */
	NMD_X86_DECODER_FLAGS_VEX            = (1 << 5), /* The decoder parses VEX instructions. */
	NMD_X86_DECODER_FLAGS_EVEX           = (1 << 6), /* The decoder parses EVEX instructions. */
	NMD_X86_DECODER_FLAGS_3DNOW          = (1 << 7), /* The decoder parses 3DNow! instructions. */

	/* These are not actual features, but rather masks of features. */
	NMD_X86_DECODER_FLAGS_NONE    = 0,
	NMD_X86_DECODER_FLAGS_MINIMAL = (NMD_X86_DECODER_FLAGS_VALIDITY_CHECK | NMD_X86_DECODER_FLAGS_VEX | NMD_X86_DECODER_FLAGS_EVEX), /* Mask that specifies minimal features to provide acurate results in any environment. */
	NMD_X86_DECODER_FLAGS_ALL     = (1 << 8) - 1, /* Mask that specifies all features. */
};

enum NMD_X86_PREFIXES
{
	NMD_X86_PREFIXES_NONE                  = 0,
	NMD_X86_PREFIXES_ES_SEGMENT_OVERRIDE   = (1 << 0),
	NMD_X86_PREFIXES_CS_SEGMENT_OVERRIDE   = (1 << 1),
	NMD_X86_PREFIXES_SS_SEGMENT_OVERRIDE   = (1 << 2),
	NMD_X86_PREFIXES_DS_SEGMENT_OVERRIDE   = (1 << 3),
	NMD_X86_PREFIXES_FS_SEGMENT_OVERRIDE   = (1 << 4),
	NMD_X86_PREFIXES_GS_SEGMENT_OVERRIDE   = (1 << 5),
	NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE = (1 << 6),
	NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE = (1 << 7),
	NMD_X86_PREFIXES_LOCK                  = (1 << 8),
	NMD_X86_PREFIXES_REPEAT_NOT_ZERO       = (1 << 9),
	NMD_X86_PREFIXES_REPEAT                = (1 << 10),
	NMD_X86_PREFIXES_REX_W                 = (1 << 11),
	NMD_X86_PREFIXES_REX_R                 = (1 << 12),
	NMD_X86_PREFIXES_REX_X                 = (1 << 13),
	NMD_X86_PREFIXES_REX_B                 = (1 << 14)
};

enum NMD_X86_IMM
{
	NMD_X86_IMM_NONE = 0,
	NMD_X86_IMM8     = 1,
	NMD_X86_IMM16    = 2,
	NMD_X86_IMM32    = 4,
	NMD_X86_IMM48    = 6,
	NMD_X86_IMM64    = 8,
	NMD_X86_IMM_ANY  = (NMD_X86_IMM8 | NMD_X86_IMM16 | NMD_X86_IMM32 | NMD_X86_IMM64)
};

enum NMD_X86_DISP
{
	NMD_X86_DISP_NONE        = 0,
	NMD_X86_DISP8            = 1,
	NMD_X86_DISP16           = 2,
	NMD_X86_DISP32           = 4,
	NMD_X86_DISP64           = 8,
	NMD_X86_DISP_ANY         = (NMD_X86_DISP8 | NMD_X86_DISP16 | NMD_X86_DISP32)
};

typedef union nmd_x86_modrm
{
	struct
	{
		uint8_t rm  : 3;
		uint8_t reg : 3;
		uint8_t mod : 2;
	} fields;
	uint8_t modrm;
} nmd_x86_modrm;

typedef union nmd_x86_sib
{
	struct
	{
		uint8_t base  : 3;
		uint8_t index : 3;
		uint8_t scale : 2;
	} fields;
	uint8_t sib;
} nmd_x86_sib;

typedef enum NMD_X86_MODE
{
	NMD_X86_MODE_NONE = 0, /* Invalid mode. */
	NMD_X86_MODE_16   = 2,
	NMD_X86_MODE_32   = 4,
	NMD_X86_MODE_64   = 8,
} NMD_X86_MODE;

enum NMD_X86_OPCODE_MAP
{
	NMD_X86_OPCODE_MAP_NONE = 0,
	NMD_X86_OPCODE_MAP_DEFAULT,
	NMD_X86_OPCODE_MAP_0F,
	NMD_X86_OPCODE_MAP_0F38,
	NMD_X86_OPCODE_MAP_0F3A,
	NMD_X86_OPCODE_MAP_0F0F
};

enum NMD_X86_ENCODING
{
	NMD_X86_ENCODING_NONE = 0,
	NMD_X86_ENCODING_LEGACY,  /* Legacy encoding. */
	NMD_X86_ENCODING_VEX,     /* Intel's VEX(vector extensions) coding scheme. */
	NMD_X86_ENCODING_EVEX,    /* Intel's EVEX(Enhanced vector extension) coding scheme. */
	NMD_X86_ENCODING_3DNOW,   /* AMD's 3DNow! extension. */
	NMD_X86_ENCODING_XOP,     /* AMD's XOP(eXtended Operations) instruction set. */
	/* NMD_X86_ENCODING_MVEX,     MVEX used by Intel's "Xeon Phi" ISA. */
};

typedef struct nmd_x86_vex
{
	bool R : 1;
	bool X : 1;
	bool B : 1;
	bool L : 1;
	bool W : 1;
	uint8_t pp : 2;
	uint8_t m_mmmm : 5;
	uint8_t vvvv : 4;
	uint8_t vex[3]; /* The full vex prefix. vex[0] is either C4h(3-byte VEX) or C5h(2-byte VEX).*/
} nmd_x86_vex;

typedef enum NMD_X86_REG
{
	NMD_X86_REG_NONE = 0,

	NMD_X86_REG_AL,
	NMD_X86_REG_CL,
	NMD_X86_REG_DL,
	NMD_X86_REG_BL,
	NMD_X86_REG_AH,
	NMD_X86_REG_CH,
	NMD_X86_REG_DH,
	NMD_X86_REG_BH,

	NMD_X86_REG_AX,
	NMD_X86_REG_CX,
	NMD_X86_REG_DX,
	NMD_X86_REG_BX,
	NMD_X86_REG_SP,
	NMD_X86_REG_BP,
	NMD_X86_REG_SI,
	NMD_X86_REG_DI,

	NMD_X86_REG_EAX,
	NMD_X86_REG_ECX,
	NMD_X86_REG_EDX,
	NMD_X86_REG_EBX,
	NMD_X86_REG_ESP,
	NMD_X86_REG_EBP,
	NMD_X86_REG_ESI,
	NMD_X86_REG_EDI,

	NMD_X86_REG_RAX,
	NMD_X86_REG_RBX,
	NMD_X86_REG_RCX,
	NMD_X86_REG_RDX,
	NMD_X86_REG_RBP,
	NMD_X86_REG_RSP,
	NMD_X86_REG_RSI,
	NMD_X86_REG_RDI,

	NMD_X86_REG_R8,
	NMD_X86_REG_R9,
	NMD_X86_REG_R10,
	NMD_X86_REG_R11,
	NMD_X86_REG_R12,
	NMD_X86_REG_R13,
	NMD_X86_REG_R14,
	NMD_X86_REG_R15,

	NMD_X86_REG_R8B,
	NMD_X86_REG_R9B,
	NMD_X86_REG_R10B,
	NMD_X86_REG_R11B,
	NMD_X86_REG_R12B,
	NMD_X86_REG_R13B,
	NMD_X86_REG_R14B,
	NMD_X86_REG_R15B,

	NMD_X86_REG_R8D,
	NMD_X86_REG_R9D,
	NMD_X86_REG_R10D,
	NMD_X86_REG_R11D,
	NMD_X86_REG_R12D,
	NMD_X86_REG_R13D,
	NMD_X86_REG_R14D,
	NMD_X86_REG_R15D,

	NMD_X86_REG_ES,
	NMD_X86_REG_CS,
	NMD_X86_REG_SS,
	NMD_X86_REG_DS,
	NMD_X86_REG_FS,
	NMD_X86_REG_GS,

	NMD_X86_REG_CR0,
	NMD_X86_REG_CR1,
	NMD_X86_REG_CR2,
	NMD_X86_REG_CR3,
	NMD_X86_REG_CR4,
	NMD_X86_REG_CR5,
	NMD_X86_REG_CR6,
	NMD_X86_REG_CR7,
	NMD_X86_REG_CR8,
	NMD_X86_REG_CR9,
	NMD_X86_REG_CR10,
	NMD_X86_REG_CR11,
	NMD_X86_REG_CR12,
	NMD_X86_REG_CR13,
	NMD_X86_REG_CR14,
	NMD_X86_REG_CR15,

	NMD_X86_REG_DR0,
	NMD_X86_REG_DR1,
	NMD_X86_REG_DR2,
	NMD_X86_REG_DR3,
	NMD_X86_REG_DR4,
	NMD_X86_REG_DR5,
	NMD_X86_REG_DR6,
	NMD_X86_REG_DR7,
	NMD_X86_REG_DR8,
	NMD_X86_REG_DR9,
	NMD_X86_REG_DR10,
	NMD_X86_REG_DR11,
	NMD_X86_REG_DR12,
	NMD_X86_REG_DR13,
	NMD_X86_REG_DR14,
	NMD_X86_REG_DR15,

	NMD_X86_REG_K0,
	NMD_X86_REG_K1,
	NMD_X86_REG_K2,
	NMD_X86_REG_K3,
	NMD_X86_REG_K4,
	NMD_X86_REG_K5,
	NMD_X86_REG_K6,
	NMD_X86_REG_K7,

	NMD_X86_REG_ST0,
	NMD_X86_REG_ST1,
	NMD_X86_REG_ST2,
	NMD_X86_REG_ST3,
	NMD_X86_REG_ST4,
	NMD_X86_REG_ST5,
	NMD_X86_REG_ST6,
	NMD_X86_REG_ST7,

	NMD_X86_REG_MM0,
	NMD_X86_REG_MM1,
	NMD_X86_REG_MM2,
	NMD_X86_REG_MM3,
	NMD_X86_REG_MM4,
	NMD_X86_REG_MM5,
	NMD_X86_REG_MM6,
	NMD_X86_REG_MM7,

	NMD_X86_REG_XMM0,
	NMD_X86_REG_XMM1,
	NMD_X86_REG_XMM2,
	NMD_X86_REG_XMM3,
	NMD_X86_REG_XMM4,
	NMD_X86_REG_XMM5,
	NMD_X86_REG_XMM6,
	NMD_X86_REG_XMM7,
	NMD_X86_REG_XMM8,
	NMD_X86_REG_XMM9,
	NMD_X86_REG_XMM10,
	NMD_X86_REG_XMM11,
	NMD_X86_REG_XMM12,
	NMD_X86_REG_XMM13,
	NMD_X86_REG_XMM14,
	NMD_X86_REG_XMM15,
	NMD_X86_REG_XMM16,
	NMD_X86_REG_XMM17,
	NMD_X86_REG_XMM18,
	NMD_X86_REG_XMM19,
	NMD_X86_REG_XMM20,
	NMD_X86_REG_XMM21,
	NMD_X86_REG_XMM22,
	NMD_X86_REG_XMM23,
	NMD_X86_REG_XMM24,
	NMD_X86_REG_XMM25,
	NMD_X86_REG_XMM26,
	NMD_X86_REG_XMM27,
	NMD_X86_REG_XMM28,
	NMD_X86_REG_XMM29,
	NMD_X86_REG_XMM30,
	NMD_X86_REG_XMM31,

	NMD_X86_REG_YMM0,
	NMD_X86_REG_YMM1,
	NMD_X86_REG_YMM2,
	NMD_X86_REG_YMM3,
	NMD_X86_REG_YMM4,
	NMD_X86_REG_YMM5,
	NMD_X86_REG_YMM6,
	NMD_X86_REG_YMM7,
	NMD_X86_REG_YMM8,
	NMD_X86_REG_YMM9,
	NMD_X86_REG_YMM10,
	NMD_X86_REG_YMM11,
	NMD_X86_REG_YMM12,
	NMD_X86_REG_YMM13,
	NMD_X86_REG_YMM14,
	NMD_X86_REG_YMM15,
	NMD_X86_REG_YMM16,
	NMD_X86_REG_YMM17,
	NMD_X86_REG_YMM18,
	NMD_X86_REG_YMM19,
	NMD_X86_REG_YMM20,
	NMD_X86_REG_YMM21,
	NMD_X86_REG_YMM22,
	NMD_X86_REG_YMM23,
	NMD_X86_REG_YMM24,
	NMD_X86_REG_YMM25,
	NMD_X86_REG_YMM26,
	NMD_X86_REG_YMM27,
	NMD_X86_REG_YMM28,
	NMD_X86_REG_YMM29,
	NMD_X86_REG_YMM30,
	NMD_X86_REG_YMM31,

	NMD_X86_REG_ZMM0,
	NMD_X86_REG_ZMM1,
	NMD_X86_REG_ZMM2,
	NMD_X86_REG_ZMM3,
	NMD_X86_REG_ZMM4,
	NMD_X86_REG_ZMM5,
	NMD_X86_REG_ZMM6,
	NMD_X86_REG_ZMM7,
	NMD_X86_REG_ZMM8,
	NMD_X86_REG_ZMM9,
	NMD_X86_REG_ZMM10,
	NMD_X86_REG_ZMM11,
	NMD_X86_REG_ZMM12,
	NMD_X86_REG_ZMM13,
	NMD_X86_REG_ZMM14,
	NMD_X86_REG_ZMM15,
	NMD_X86_REG_ZMM16,
	NMD_X86_REG_ZMM17,
	NMD_X86_REG_ZMM18,
	NMD_X86_REG_ZMM19,
	NMD_X86_REG_ZMM20,
	NMD_X86_REG_ZMM21,
	NMD_X86_REG_ZMM22,
	NMD_X86_REG_ZMM23,
	NMD_X86_REG_ZMM24,
	NMD_X86_REG_ZMM25,
	NMD_X86_REG_ZMM26,
	NMD_X86_REG_ZMM27,
	NMD_X86_REG_ZMM28,
	NMD_X86_REG_ZMM29,
	NMD_X86_REG_ZMM30,
	NMD_X86_REG_ZMM31,
} NMD_X86_REG;

enum NMD_GROUP {
	NMD_GROUP_NONE = 0, /* The instruction is not part of any group. */

	NMD_GROUP_JUMP                 = (1 << 0), /* All jump instructions. */
	NMD_GROUP_CALL                 = (1 << 1), /* Call instruction. */
	NMD_GROUP_RET                  = (1 << 2), /* Return instruction. */
	NMD_GROUP_INT                  = (1 << 3), /* Interrupt instruction. */
	NMD_GROUP_PRIVILEGE            = (1 << 4), /* Privileged instruction. */
	NMD_GROUP_CONDITIONAL_BRANCH   = (1 << 5), /* Conditional branch instruction. */
	NMD_GROUP_UNCONDITIONAL_BRANCH = (1 << 6), /* Unconditional branch instruction. */
	NMD_GROUP_RELATIVE_ADDRESSING  = (1 << 7), /* Relative addressing instruction. */

	/* These are not actual groups, but rather masks of groups. */
	NMD_GROUP_BRANCH = (NMD_GROUP_CONDITIONAL_BRANCH | NMD_GROUP_UNCONDITIONAL_BRANCH), /* Mask used to check if the instruction is a branch instruction. */
	NMD_GROUP_ANY    = (1 << 8) - 1, /* Mask used to check if the instruction is part of any group. */
};

/*
Credits to the capstone engine:
Some members of the enum are organized in such a way because the instruction's id parsing component of the decoder can take advantage of it.
If an instruction as marked as 'padding', it means that it's being used to fill holes between instructions organized in a special way for optimization reasons.
*/
enum NMD_X86_INSTRUCTION
{
	NMD_X86_INSTRUCTION_INVALID = 0,

	/* Optimized for opcode extension group 1. */
	NMD_X86_INSTRUCTION_ADD,
	NMD_X86_INSTRUCTION_OR,
	NMD_X86_INSTRUCTION_ADC,
	NMD_X86_INSTRUCTION_SBB,
	NMD_X86_INSTRUCTION_AND,
	NMD_X86_INSTRUCTION_SUB,
	NMD_X86_INSTRUCTION_XOR,
	NMD_X86_INSTRUCTION_CMP,

	/* Optimized for opcode extension group 2. */
	NMD_X86_INSTRUCTION_ROL,
	NMD_X86_INSTRUCTION_ROR,
	NMD_X86_INSTRUCTION_RCL,
	NMD_X86_INSTRUCTION_RCR,
	NMD_X86_INSTRUCTION_SHL,
	NMD_X86_INSTRUCTION_SHR,
	NMD_X86_INSTRUCTION_AAA, /* padding */
	NMD_X86_INSTRUCTION_SAR,

	/* Optimized for opcode extension group 3. */
	NMD_X86_INSTRUCTION_TEST,
	NMD_X86_INSTRUCTION_BLSFILL, /* pading */
	NMD_X86_INSTRUCTION_NOT,
	NMD_X86_INSTRUCTION_NEG,
	NMD_X86_INSTRUCTION_MUL,
	NMD_X86_INSTRUCTION_IMUL,
	NMD_X86_INSTRUCTION_DIV,
	NMD_X86_INSTRUCTION_IDIV,

	/* Optimized for opcode extension group 5. */
	NMD_X86_INSTRUCTION_INC,
	NMD_X86_INSTRUCTION_DEC,
	NMD_X86_INSTRUCTION_CALL,
	NMD_X86_INSTRUCTION_LCALL,
	NMD_X86_INSTRUCTION_JMP,
	NMD_X86_INSTRUCTION_LJMP,
	NMD_X86_INSTRUCTION_PUSH,

	/* Optimized for the 7th row of the 1 byte opcode map and the 8th row of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_JO,
	NMD_X86_INSTRUCTION_JNO,
	NMD_X86_INSTRUCTION_JB,
	NMD_X86_INSTRUCTION_JAE,
	NMD_X86_INSTRUCTION_JE,
	NMD_X86_INSTRUCTION_JNE,
	NMD_X86_INSTRUCTION_JBE,
	NMD_X86_INSTRUCTION_JA,
	NMD_X86_INSTRUCTION_JS,
	NMD_X86_INSTRUCTION_JNS,
	NMD_X86_INSTRUCTION_JP,
	NMD_X86_INSTRUCTION_JNP,
	NMD_X86_INSTRUCTION_JL,
	NMD_X86_INSTRUCTION_JGE,
	NMD_X86_INSTRUCTION_JLE,
	NMD_X86_INSTRUCTION_JG,

	/* Optimized for escape opcodes with D8 as first byte. */
	NMD_X86_INSTRUCTION_FADD,
	NMD_X86_INSTRUCTION_FMUL,
	NMD_X86_INSTRUCTION_FCOM,
	NMD_X86_INSTRUCTION_FCOMP,
	NMD_X86_INSTRUCTION_FSUB,
	NMD_X86_INSTRUCTION_FSUBR,
	NMD_X86_INSTRUCTION_FDIV,
	NMD_X86_INSTRUCTION_FDIVR,

	/* Optimized for escape opcodes with D9 as first byte. */
	NMD_X86_INSTRUCTION_FLD,
	NMD_X86_INSTRUCTION_ADOX, /* padding */
	NMD_X86_INSTRUCTION_FST,
	NMD_X86_INSTRUCTION_FSTP,
	NMD_X86_INSTRUCTION_FLDENV,
	NMD_X86_INSTRUCTION_FLDCW,
	NMD_X86_INSTRUCTION_FNSTENV,
	NMD_X86_INSTRUCTION_FNSTCW,

	NMD_X86_INSTRUCTION_FCHS,
	NMD_X86_INSTRUCTION_FABS,
	NMD_X86_INSTRUCTION_AAS, /* padding */
	NMD_X86_INSTRUCTION_ADCX, /* padding */
	NMD_X86_INSTRUCTION_FTST,
	NMD_X86_INSTRUCTION_FXAM,
	NMD_X86_INSTRUCTION_RET, /* padding */
	NMD_X86_INSTRUCTION_ENTER, /* padding */
	NMD_X86_INSTRUCTION_FLD1,
	NMD_X86_INSTRUCTION_FLDL2T,
	NMD_X86_INSTRUCTION_FLDL2E,
	NMD_X86_INSTRUCTION_FLDPI,
	NMD_X86_INSTRUCTION_FLDLG2,
	NMD_X86_INSTRUCTION_FLDLN2,
	NMD_X86_INSTRUCTION_FLDZ,
	NMD_X86_INSTRUCTION_FNOP, /* padding */
	NMD_X86_INSTRUCTION_F2XM1,
	NMD_X86_INSTRUCTION_FYL2X,
	NMD_X86_INSTRUCTION_FPTAN,
	NMD_X86_INSTRUCTION_FPATAN,
	NMD_X86_INSTRUCTION_FXTRACT,
	NMD_X86_INSTRUCTION_FPREM1,
	NMD_X86_INSTRUCTION_FDECSTP,
	NMD_X86_INSTRUCTION_FINCSTP,
	NMD_X86_INSTRUCTION_FPREM,
	NMD_X86_INSTRUCTION_FYL2XP1,
	NMD_X86_INSTRUCTION_FSQRT,
	NMD_X86_INSTRUCTION_FSINCOS,
	NMD_X86_INSTRUCTION_FRNDINT,
	NMD_X86_INSTRUCTION_FSCALE,
	NMD_X86_INSTRUCTION_FSIN,
	NMD_X86_INSTRUCTION_FCOS,

	/* Optimized for escape opcodes with DA as first byte. */
	NMD_X86_INSTRUCTION_FIADD,
	NMD_X86_INSTRUCTION_FIMUL,
	NMD_X86_INSTRUCTION_FICOM,
	NMD_X86_INSTRUCTION_FICOMP,
	NMD_X86_INSTRUCTION_FISUB,
	NMD_X86_INSTRUCTION_FISUBR,
	NMD_X86_INSTRUCTION_FIDIV,
	NMD_X86_INSTRUCTION_FIDIVR,

	NMD_X86_INSTRUCTION_FCMOVB,
	NMD_X86_INSTRUCTION_FCMOVE,
	NMD_X86_INSTRUCTION_FCMOVBE,
	NMD_X86_INSTRUCTION_FCMOVU,

	/* Optimized for escape opcodes with DB/DF as first byte. */
	NMD_X86_INSTRUCTION_FILD,
	NMD_X86_INSTRUCTION_FISTTP,
	NMD_X86_INSTRUCTION_FIST,
	NMD_X86_INSTRUCTION_FISTP,
	NMD_X86_INSTRUCTION_FBLD,
	NMD_X86_INSTRUCTION_AESKEYGENASSIST, /* padding */
	NMD_X86_INSTRUCTION_FBSTP,
	NMD_X86_INSTRUCTION_ANDN, /* padding */

	NMD_X86_INSTRUCTION_FCMOVNB,
	NMD_X86_INSTRUCTION_FCMOVNE,
	NMD_X86_INSTRUCTION_FCMOVNBE,
	NMD_X86_INSTRUCTION_FCMOVNU,
	NMD_X86_INSTRUCTION_FNCLEX,
	NMD_X86_INSTRUCTION_FUCOMI,
	NMD_X86_INSTRUCTION_FCOMI,

	/* Optimized for escape opcodes with DE as first byte. */
	NMD_X86_INSTRUCTION_FADDP,
	NMD_X86_INSTRUCTION_FMULP,
	NMD_X86_INSTRUCTION_MOVAPD, /* padding */
	NMD_X86_INSTRUCTION_BNDCN, /* padding */
	NMD_X86_INSTRUCTION_FSUBRP,
	NMD_X86_INSTRUCTION_FSUBP,
	NMD_X86_INSTRUCTION_FDIVRP,
	NMD_X86_INSTRUCTION_FDIVP,

	/* Optimized for the 15th row of the 1 byte opcode map. */
	NMD_X86_INSTRUCTION_INT1,
	NMD_X86_INSTRUCTION_BSR, /* padding */
	NMD_X86_INSTRUCTION_ADDSUBPD, /* padding */
	NMD_X86_INSTRUCTION_HLT,
	NMD_X86_INSTRUCTION_CMC,
	NMD_X86_INSTRUCTION_ADDSUBPS, /* padding */
	NMD_X86_INSTRUCTION_BLENDVPD, /* padding*/
	NMD_X86_INSTRUCTION_CLC,
	NMD_X86_INSTRUCTION_STC,
	NMD_X86_INSTRUCTION_CLI,
	NMD_X86_INSTRUCTION_STI,
	NMD_X86_INSTRUCTION_CLD,
	NMD_X86_INSTRUCTION_STD,

	/* Optimized for the 13th row of the 1 byte opcode map. */
	NMD_X86_INSTRUCTION_AAM,
	NMD_X86_INSTRUCTION_AAD,
	NMD_X86_INSTRUCTION_BLENDVPS, /* padding*/
	NMD_X86_INSTRUCTION_XLAT,

	/* Optimized for the 14th row of the 1 byte opcode map. */
	NMD_X86_INSTRUCTION_LOOPNE,
	NMD_X86_INSTRUCTION_LOOPE,
	NMD_X86_INSTRUCTION_LOOP,
	NMD_X86_INSTRUCTION_JRCXZ,

	/* Optimized for opcode extension group 6. */
	NMD_X86_INSTRUCTION_SLDT,
	NMD_X86_INSTRUCTION_STR,
	NMD_X86_INSTRUCTION_LLDT,
	NMD_X86_INSTRUCTION_LTR,
	NMD_X86_INSTRUCTION_VERR,
	NMD_X86_INSTRUCTION_VERW,

	/* Optimized for opcode extension group 7. */
	NMD_X86_INSTRUCTION_SGDT,
	NMD_X86_INSTRUCTION_SIDT,
	NMD_X86_INSTRUCTION_LGDT,
	NMD_X86_INSTRUCTION_LIDT,
	NMD_X86_INSTRUCTION_SMSW,
	NMD_X86_INSTRUCTION_CLWB, /* padding */
	NMD_X86_INSTRUCTION_LMSW,
	NMD_X86_INSTRUCTION_INVLPG,

	NMD_X86_INSTRUCTION_VMCALL,
	NMD_X86_INSTRUCTION_VMLAUNCH,
	NMD_X86_INSTRUCTION_VMRESUME,
	NMD_X86_INSTRUCTION_VMXOFF,

	NMD_X86_INSTRUCTION_MONITOR,
	NMD_X86_INSTRUCTION_MWAIT,
	NMD_X86_INSTRUCTION_CLAC,
	NMD_X86_INSTRUCTION_STAC,
	NMD_X86_INSTRUCTION_CBW, /* padding */
	NMD_X86_INSTRUCTION_CMPSB, /* padding */
	NMD_X86_INSTRUCTION_CMPSQ, /* padding */
	NMD_X86_INSTRUCTION_ENCLS,

	NMD_X86_INSTRUCTION_XGETBV,
	NMD_X86_INSTRUCTION_XSETBV,
	NMD_X86_INSTRUCTION_ARPL, /* padding */
	NMD_X86_INSTRUCTION_BEXTR, /* padding */
	NMD_X86_INSTRUCTION_VMFUNC,
	NMD_X86_INSTRUCTION_XEND,
	NMD_X86_INSTRUCTION_XTEST,
	NMD_X86_INSTRUCTION_ENCLU,

	NMD_X86_INSTRUCTION_VMRUN,
	NMD_X86_INSTRUCTION_VMMCALL,
	NMD_X86_INSTRUCTION_VMLOAD,
	NMD_X86_INSTRUCTION_VMSAVE,
	NMD_X86_INSTRUCTION_STGI,
	NMD_X86_INSTRUCTION_CLGI,
	NMD_X86_INSTRUCTION_SKINIT,
	NMD_X86_INSTRUCTION_INVLPGA,

	/* Optimized for the row 0x0 of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_LAR,
	NMD_X86_INSTRUCTION_LSL,
	NMD_X86_INSTRUCTION_BLCFILL, /* padding */
	NMD_X86_INSTRUCTION_SYSCALL,
	NMD_X86_INSTRUCTION_CLTS,
	NMD_X86_INSTRUCTION_SYSRET,
	NMD_X86_INSTRUCTION_INVD,
	NMD_X86_INSTRUCTION_WBINVD,
	NMD_X86_INSTRUCTION_BLCI, /* padding */
	NMD_X86_INSTRUCTION_UD2,
	NMD_X86_INSTRUCTION_PREFETCHW,
	NMD_X86_INSTRUCTION_FEMMS,

	/* Optimized for the row 0x3 of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_WRMSR,
	NMD_X86_INSTRUCTION_RDTSC,
	NMD_X86_INSTRUCTION_RDMSR,
	NMD_X86_INSTRUCTION_RDPMC,
	NMD_X86_INSTRUCTION_SYSENTER,
	NMD_X86_INSTRUCTION_SYSEXIT,
	NMD_X86_INSTRUCTION_BLCIC, /* padding */
	NMD_X86_INSTRUCTION_GETSEC,

	/* Optimized for the row 0x4 of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_CMOVO,
	NMD_X86_INSTRUCTION_CMOVNO,
	NMD_X86_INSTRUCTION_CMOVB,
	NMD_X86_INSTRUCTION_CMOVAE,
	NMD_X86_INSTRUCTION_CMOVE,
	NMD_X86_INSTRUCTION_CMOVNE,
	NMD_X86_INSTRUCTION_CMOVBE,
	NMD_X86_INSTRUCTION_CMOVA,
	NMD_X86_INSTRUCTION_CMOVS,
	NMD_X86_INSTRUCTION_CMOVNS,
	NMD_X86_INSTRUCTION_CMOVP,
	NMD_X86_INSTRUCTION_CMOVNP,
	NMD_X86_INSTRUCTION_CMOVL,
	NMD_X86_INSTRUCTION_CMOVGE,
	NMD_X86_INSTRUCTION_CMOVLE,
	NMD_X86_INSTRUCTION_CMOVG,

	/* Optimized for the row 0x9 of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_SETO,
	NMD_X86_INSTRUCTION_SETNO,
	NMD_X86_INSTRUCTION_SETB,
	NMD_X86_INSTRUCTION_SETAE,
	NMD_X86_INSTRUCTION_SETE,
	NMD_X86_INSTRUCTION_SETNE,
	NMD_X86_INSTRUCTION_SETBE,
	NMD_X86_INSTRUCTION_SETA,
	NMD_X86_INSTRUCTION_SETS,
	NMD_X86_INSTRUCTION_SETNS,
	NMD_X86_INSTRUCTION_SETP,
	NMD_X86_INSTRUCTION_SETNP,
	NMD_X86_INSTRUCTION_SETL,
	NMD_X86_INSTRUCTION_SETGE,
	NMD_X86_INSTRUCTION_SETLE,
	NMD_X86_INSTRUCTION_SETG,

	/* Optimized for the row 0xb of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_LSS,
	NMD_X86_INSTRUCTION_BTR,
	NMD_X86_INSTRUCTION_LFS,
	NMD_X86_INSTRUCTION_LGS,

	NMD_X86_INSTRUCTION_BT,
	NMD_X86_INSTRUCTION_BTC,
	NMD_X86_INSTRUCTION_BTS,

	/* Optimized for the row 0x0 of the 3 byte opcode map(38h). */
	NMD_X86_INSTRUCTION_PSHUFB,
	NMD_X86_INSTRUCTION_PHADDW,
	NMD_X86_INSTRUCTION_PHADDD,
	NMD_X86_INSTRUCTION_PHADDSW,
	NMD_X86_INSTRUCTION_PMADDUBSW,
	NMD_X86_INSTRUCTION_PHSUBW,
	NMD_X86_INSTRUCTION_PHSUBD,
	NMD_X86_INSTRUCTION_PHSUBSW,
	NMD_X86_INSTRUCTION_PSIGNB,
	NMD_X86_INSTRUCTION_PSIGNW,
	NMD_X86_INSTRUCTION_PSIGND,
	NMD_X86_INSTRUCTION_PMULHRSW,

	/* Optimized for the row 0x1 of the 3 byte opcode map(38h). */
	NMD_X86_INSTRUCTION_PABSB,
	NMD_X86_INSTRUCTION_PABSW,
	NMD_X86_INSTRUCTION_PABSD,

	/* Optimized for the row 0x2 of the 3 byte opcode map(38). */
	NMD_X86_INSTRUCTION_PMOVSXBW,
	NMD_X86_INSTRUCTION_PMOVSXBD,
	NMD_X86_INSTRUCTION_PMOVSXBQ,
	NMD_X86_INSTRUCTION_PMOVSXWD,
	NMD_X86_INSTRUCTION_PMOVSXWQ,
	NMD_X86_INSTRUCTION_PMOVZXDQ,
	NMD_X86_INSTRUCTION_CPUID, /* padding */
	NMD_X86_INSTRUCTION_BLCMSK, /* padding */
	NMD_X86_INSTRUCTION_PMULDQ,
	NMD_X86_INSTRUCTION_PCMPEQQ,
	NMD_X86_INSTRUCTION_MOVNTDQA,
	NMD_X86_INSTRUCTION_PACKUSDW,

	/* Optimized for the row 0x3 of the 3 byte opcode map(38h). */
	NMD_X86_INSTRUCTION_PMOVZXBW,
	NMD_X86_INSTRUCTION_PMOVZXBD,
	NMD_X86_INSTRUCTION_PMOVZXBQ,
	NMD_X86_INSTRUCTION_PMOVZXWD,
	NMD_X86_INSTRUCTION_PMOVZXWQ,
	NMD_X86_INSTRUCTION_PMOVSXDQ,
	NMD_X86_INSTRUCTION_BLCS, /* padding */
	NMD_X86_INSTRUCTION_PCMPGTQ,
	NMD_X86_INSTRUCTION_PMINSB,
	NMD_X86_INSTRUCTION_PMINSD,
	NMD_X86_INSTRUCTION_PMINUW,
	NMD_X86_INSTRUCTION_PMINUD,
	NMD_X86_INSTRUCTION_PMAXSB,
	NMD_X86_INSTRUCTION_PMAXSD,
	NMD_X86_INSTRUCTION_PMAXUW,
	NMD_X86_INSTRUCTION_PMAXUD,

	/* Optimized for the row 0x8 of the 3 byte opcode map(38h). */
	NMD_X86_INSTRUCTION_INVEPT,
	NMD_X86_INSTRUCTION_INVVPID,
	NMD_X86_INSTRUCTION_INVPCID,

	/* Optimized for the row 0xc of the 3 byte opcode map(38h). */
	NMD_X86_INSTRUCTION_SHA1NEXTE,
	NMD_X86_INSTRUCTION_SHA1MSG1,
	NMD_X86_INSTRUCTION_SHA1MSG2,
	NMD_X86_INSTRUCTION_SHA256RNDS2,
	NMD_X86_INSTRUCTION_SHA256MSG1,
	NMD_X86_INSTRUCTION_SHA256MSG2,

	/* Optimized for the row 0xd of the 3 byte opcode map(38h). */
	NMD_X86_INSTRUCTION_AESIMC,
	NMD_X86_INSTRUCTION_AESENC,
	NMD_X86_INSTRUCTION_AESENCLAST,
	NMD_X86_INSTRUCTION_AESDEC,
	NMD_X86_INSTRUCTION_AESDECLAST,

	/* Optimized for the row 0x0 of the 3 byte opcode map(3Ah). */
	NMD_X86_INSTRUCTION_ROUNDPS,
	NMD_X86_INSTRUCTION_ROUNDPD,
	NMD_X86_INSTRUCTION_ROUNDSS,
	NMD_X86_INSTRUCTION_ROUNDSD,
	NMD_X86_INSTRUCTION_BLENDPS,
	NMD_X86_INSTRUCTION_BLENDPD,
	NMD_X86_INSTRUCTION_PBLENDW,
	NMD_X86_INSTRUCTION_PALIGNR,

	/* Optimized for the row 0x4 of the 3 byte opcode map(3A). */
	NMD_X86_INSTRUCTION_DPPS,
	NMD_X86_INSTRUCTION_DPPD,
	NMD_X86_INSTRUCTION_MPSADBW,
	NMD_X86_INSTRUCTION_VPCMPGTQ, /* padding */
	NMD_X86_INSTRUCTION_PCLMULQDQ,

	/* Optimized for the row 0x6 of the 3 byte opcode map(3A). */
	NMD_X86_INSTRUCTION_PCMPESTRM,
	NMD_X86_INSTRUCTION_PCMPESTRI,
	NMD_X86_INSTRUCTION_PCMPISTRM,
	NMD_X86_INSTRUCTION_PCMPISTRI,

	/* Optimized for the rows 0xd, 0xe and 0xf of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_PSRLW,
	NMD_X86_INSTRUCTION_PSRLD,
	NMD_X86_INSTRUCTION_PSRLQ,
	NMD_X86_INSTRUCTION_PADDQ,
	NMD_X86_INSTRUCTION_PMULLW,
	NMD_X86_INSTRUCTION_BOUND, /* padding */
	NMD_X86_INSTRUCTION_PMOVMSKB,
	NMD_X86_INSTRUCTION_PSUBUSB,
	NMD_X86_INSTRUCTION_PSUBUSW,
	NMD_X86_INSTRUCTION_PMINUB,
	NMD_X86_INSTRUCTION_PAND,
	NMD_X86_INSTRUCTION_PADDUSB,
	NMD_X86_INSTRUCTION_PADDUSW,
	NMD_X86_INSTRUCTION_PMAXUB,
	NMD_X86_INSTRUCTION_PANDN,
	NMD_X86_INSTRUCTION_PAVGB,
	NMD_X86_INSTRUCTION_PSRAW,
	NMD_X86_INSTRUCTION_PSRAD,
	NMD_X86_INSTRUCTION_PAVGW,
	NMD_X86_INSTRUCTION_PMULHUW,
	NMD_X86_INSTRUCTION_PMULHW,
	NMD_X86_INSTRUCTION_CQO, /* padding */
	NMD_X86_INSTRUCTION_CRC32, /* padding */
	NMD_X86_INSTRUCTION_PSUBSB,
	NMD_X86_INSTRUCTION_PSUBSW,
	NMD_X86_INSTRUCTION_PMINSW,
	NMD_X86_INSTRUCTION_POR,
	NMD_X86_INSTRUCTION_PADDSB,
	NMD_X86_INSTRUCTION_PADDSW,
	NMD_X86_INSTRUCTION_PMAXSW,
	NMD_X86_INSTRUCTION_PXOR,
	NMD_X86_INSTRUCTION_LDDQU,
	NMD_X86_INSTRUCTION_PSLLW,
	NMD_X86_INSTRUCTION_PSLLD,
	NMD_X86_INSTRUCTION_PSLLQ,
	NMD_X86_INSTRUCTION_PMULUDQ,
	NMD_X86_INSTRUCTION_PMADDWD,
	NMD_X86_INSTRUCTION_PSADBW,
	NMD_X86_INSTRUCTION_BSWAP, /* padding */
	NMD_X86_INSTRUCTION_PSUBB,
	NMD_X86_INSTRUCTION_PSUBW,
	NMD_X86_INSTRUCTION_PSUBD,
	NMD_X86_INSTRUCTION_PSUBQ,
	NMD_X86_INSTRUCTION_PADDB,
	NMD_X86_INSTRUCTION_PADDW,
	NMD_X86_INSTRUCTION_PADDD,

	/* Optimized for the row 0xc of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_MOVNTI,
	NMD_X86_INSTRUCTION_PINSRW,
	NMD_X86_INSTRUCTION_PEXTRW,

	/* Optimized for opcode extension group 15. */
	NMD_X86_INSTRUCTION_FXSAVE,
	NMD_X86_INSTRUCTION_FXRSTOR,
	NMD_X86_INSTRUCTION_LDMXCSR,
	NMD_X86_INSTRUCTION_STMXCSR,
	NMD_X86_INSTRUCTION_XSAVE,
	NMD_X86_INSTRUCTION_XRSTOR,
	NMD_X86_INSTRUCTION_XSAVEOPT,
	NMD_X86_INSTRUCTION_CLFLUSH,

	NMD_X86_INSTRUCTION_RDFSBASE,
	NMD_X86_INSTRUCTION_RDGSBASE,
	NMD_X86_INSTRUCTION_WRFSBASE,
	NMD_X86_INSTRUCTION_WRGSBASE,
	NMD_X86_INSTRUCTION_CMPXCHG, /* padding */
	NMD_X86_INSTRUCTION_LFENCE,
	NMD_X86_INSTRUCTION_MFENCE,
	NMD_X86_INSTRUCTION_SFENCE,
	
	NMD_X86_INSTRUCTION_PCMPEQB,
	NMD_X86_INSTRUCTION_PCMPEQW,
	NMD_X86_INSTRUCTION_PCMPEQD,
	
	/* Optimized for the row 0x5 of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_MOVMSKPS,
	NMD_X86_INSTRUCTION_SQRTPS,
	NMD_X86_INSTRUCTION_RSQRTPS,
	NMD_X86_INSTRUCTION_RCPPS,
	NMD_X86_INSTRUCTION_ANDPS,
	NMD_X86_INSTRUCTION_ANDNPS,
	NMD_X86_INSTRUCTION_ORPS,
	NMD_X86_INSTRUCTION_XORPS,
	NMD_X86_INSTRUCTION_ADDPS,
	NMD_X86_INSTRUCTION_MULPS,
	NMD_X86_INSTRUCTION_CVTPS2PD,
	NMD_X86_INSTRUCTION_CVTDQ2PS,
	NMD_X86_INSTRUCTION_SUBPS,
	NMD_X86_INSTRUCTION_MINPS,
	NMD_X86_INSTRUCTION_DIVPS,
	NMD_X86_INSTRUCTION_MAXPS,
	
	NMD_X86_INSTRUCTION_MOVMSKPD,
	NMD_X86_INSTRUCTION_SQRTPD,
	NMD_X86_INSTRUCTION_BNDLDX, /* padding */
	NMD_X86_INSTRUCTION_BNDSTX, /* padding */
	NMD_X86_INSTRUCTION_ANDPD,
	NMD_X86_INSTRUCTION_ANDNPD,
	NMD_X86_INSTRUCTION_ORPD,
	NMD_X86_INSTRUCTION_XORPD,
	NMD_X86_INSTRUCTION_ADDPD,
	NMD_X86_INSTRUCTION_MULPD,
	NMD_X86_INSTRUCTION_CVTPD2PS,
	NMD_X86_INSTRUCTION_CVTPS2DQ,
	NMD_X86_INSTRUCTION_SUBPD,
	NMD_X86_INSTRUCTION_MINPD,
	NMD_X86_INSTRUCTION_DIVPD,
	NMD_X86_INSTRUCTION_MAXPD,
	
	NMD_X86_INSTRUCTION_BNDMOV,  /* padding */
	NMD_X86_INSTRUCTION_SQRTSS,
	NMD_X86_INSTRUCTION_RSQRTSS,
	NMD_X86_INSTRUCTION_RCPSS,
	NMD_X86_INSTRUCTION_CMPXCHG16B, /* padding */
	NMD_X86_INSTRUCTION_DAA, /* padding */
	NMD_X86_INSTRUCTION_CWD, /* padding */
	NMD_X86_INSTRUCTION_INSD, /* padding */
	NMD_X86_INSTRUCTION_ADDSS,
	NMD_X86_INSTRUCTION_MULSS,
	NMD_X86_INSTRUCTION_CVTSS2SD,
	NMD_X86_INSTRUCTION_CVTTPS2DQ,
	NMD_X86_INSTRUCTION_SUBSS,
	NMD_X86_INSTRUCTION_MINSS,
	NMD_X86_INSTRUCTION_DIVSS,
	NMD_X86_INSTRUCTION_MAXSS,
	
	NMD_X86_INSTRUCTION_BNDCL, /* padding */
	NMD_X86_INSTRUCTION_SQRTSD,
	NMD_X86_INSTRUCTION_BNDCU, /* padding */
	NMD_X86_INSTRUCTION_BNDMK, /* padding */
	NMD_X86_INSTRUCTION_CMPXCHG8B, /* padding */
	NMD_X86_INSTRUCTION_DAS, /* padding */
	NMD_X86_INSTRUCTION_CWDE, /* padding */
	NMD_X86_INSTRUCTION_INSW, /* padding */
	NMD_X86_INSTRUCTION_ADDSD,
	NMD_X86_INSTRUCTION_MULSD,
	NMD_X86_INSTRUCTION_CVTSD2SS,
	NMD_X86_INSTRUCTION_FCOMIP, /* padding */
	NMD_X86_INSTRUCTION_SUBSD,
	NMD_X86_INSTRUCTION_MINSD,
	NMD_X86_INSTRUCTION_DIVSD,
	NMD_X86_INSTRUCTION_MAXSD,
	
	/* Optimized for the row 0x6 of the 2 byte opcode map. */
	NMD_X86_INSTRUCTION_PUNPCKLBW,
	NMD_X86_INSTRUCTION_PUNPCKLWD,
	NMD_X86_INSTRUCTION_PUNPCKLDQ,
	NMD_X86_INSTRUCTION_PACKSSWB,
	NMD_X86_INSTRUCTION_PCMPGTB,
	NMD_X86_INSTRUCTION_PCMPGTW,
	NMD_X86_INSTRUCTION_PCMPGTD,
	NMD_X86_INSTRUCTION_PACKUSWB,
	NMD_X86_INSTRUCTION_PUNPCKHBW,
	NMD_X86_INSTRUCTION_PUNPCKHWD,
	NMD_X86_INSTRUCTION_PUNPCKHDQ,
	NMD_X86_INSTRUCTION_PACKSSDW,
	NMD_X86_INSTRUCTION_PUNPCKLQDQ,
	NMD_X86_INSTRUCTION_PUNPCKHQDQ,
	
	/* Optimized for AVX instructions. */
	NMD_X86_INSTRUCTION_VPSHUFB,    /* 00 */
	NMD_X86_INSTRUCTION_VPHADDW,    /* 01 */
	NMD_X86_INSTRUCTION_VPHADDD,    /* 02 */
	NMD_X86_INSTRUCTION_VPHADDSW,   /* 03 */
	NMD_X86_INSTRUCTION_VPMADDUBSW, /* 04 */
	NMD_X86_INSTRUCTION_VPHSUBW,    /* 05 */
	NMD_X86_INSTRUCTION_VPHSUBD,    /* 06 */
	NMD_X86_INSTRUCTION_VPHSUBSW,   /* 07 */
	NMD_X86_INSTRUCTION_VPSIGNB,    /* 08 */
	NMD_X86_INSTRUCTION_VPSIGNW,    /* 09 */
	NMD_X86_INSTRUCTION_VPSIGND,    /* 0A dup */
	NMD_X86_INSTRUCTION_VPMULHRSW,  /* 0B dup */

	NMD_X86_INSTRUCTION_VPHADDWQ,
	NMD_X86_INSTRUCTION_VPHADDDQ,
	NMD_X86_INSTRUCTION_BLSI,
	NMD_X86_INSTRUCTION_BLSIC,
	NMD_X86_INSTRUCTION_BLSMSK,
	NMD_X86_INSTRUCTION_BLSR,
	NMD_X86_INSTRUCTION_BSF,
	NMD_X86_INSTRUCTION_BZHI,
	NMD_X86_INSTRUCTION_CDQ,
	NMD_X86_INSTRUCTION_CDQE,
	NMD_X86_INSTRUCTION_CLFLUSHOPT,
	NMD_X86_INSTRUCTION_CMPSW,
	NMD_X86_INSTRUCTION_COMISD,
	NMD_X86_INSTRUCTION_COMISS,
	NMD_X86_INSTRUCTION_CVTDQ2PD,
	NMD_X86_INSTRUCTION_CVTPD2DQ,
	NMD_X86_INSTRUCTION_CVTSD2SI,
	NMD_X86_INSTRUCTION_CVTSI2SD,
	NMD_X86_INSTRUCTION_CVTSI2SS,
	NMD_X86_INSTRUCTION_CVTSS2SI,
	NMD_X86_INSTRUCTION_CVTTPD2DQ,
	NMD_X86_INSTRUCTION_CVTTSD2SI,
	NMD_X86_INSTRUCTION_CVTTSS2SI,
	NMD_X86_INSTRUCTION_DATA16,
	NMD_X86_INSTRUCTION_EXTRACTPS,
	NMD_X86_INSTRUCTION_EXTRQ,
	NMD_X86_INSTRUCTION_FCOMPP,
	NMD_X86_INSTRUCTION_FFREE,
	NMD_X86_INSTRUCTION_FNINIT,
	NMD_X86_INSTRUCTION_FNSTSW,
	NMD_X86_INSTRUCTION_FFREEP,
	NMD_X86_INSTRUCTION_FRSTOR,
	NMD_X86_INSTRUCTION_FNSAVE,
	NMD_X86_INSTRUCTION_FSETPM,
	NMD_X86_INSTRUCTION_FXRSTOR64,
	NMD_X86_INSTRUCTION_FXSAVE64,
	NMD_X86_INSTRUCTION_MOVAPS,
	NMD_X86_INSTRUCTION_VMOVAPD,
	NMD_X86_INSTRUCTION_VMOVAPS,
	NMD_X86_INSTRUCTION_HADDPD,
	NMD_X86_INSTRUCTION_HADDPS,
	NMD_X86_INSTRUCTION_HSUBPD,
	NMD_X86_INSTRUCTION_HSUBPS,
	NMD_X86_INSTRUCTION_IN,
	NMD_X86_INSTRUCTION_INSB,
	NMD_X86_INSTRUCTION_INSERTPS,
	NMD_X86_INSTRUCTION_INSERTQ,
	NMD_X86_INSTRUCTION_INT,
	NMD_X86_INSTRUCTION_INT3,
	NMD_X86_INSTRUCTION_INTO,
	NMD_X86_INSTRUCTION_IRET,
	NMD_X86_INSTRUCTION_IRETD,
	NMD_X86_INSTRUCTION_IRETQ,
	NMD_X86_INSTRUCTION_UCOMISD,
	NMD_X86_INSTRUCTION_UCOMISS,
	NMD_X86_INSTRUCTION_VCOMISD,
	NMD_X86_INSTRUCTION_VCOMISS,
	NMD_X86_INSTRUCTION_VCVTSD2SS,
	NMD_X86_INSTRUCTION_VCVTSI2SD,
	NMD_X86_INSTRUCTION_VCVTSI2SS,
	NMD_X86_INSTRUCTION_VCVTSS2SD,
	NMD_X86_INSTRUCTION_VCVTTSD2SI,
	NMD_X86_INSTRUCTION_VCVTTSD2USI,
	NMD_X86_INSTRUCTION_VCVTTSS2SI,
	NMD_X86_INSTRUCTION_VCVTTSS2USI,
	NMD_X86_INSTRUCTION_VCVTUSI2SD,
	NMD_X86_INSTRUCTION_VCVTUSI2SS,
	NMD_X86_INSTRUCTION_VUCOMISD,
	NMD_X86_INSTRUCTION_VUCOMISS,
	NMD_X86_INSTRUCTION_JCXZ,
	NMD_X86_INSTRUCTION_JECXZ,
	NMD_X86_INSTRUCTION_KANDB,
	NMD_X86_INSTRUCTION_KANDD,
	NMD_X86_INSTRUCTION_KANDNB,
	NMD_X86_INSTRUCTION_KANDND,
	NMD_X86_INSTRUCTION_KANDNQ,
	NMD_X86_INSTRUCTION_KANDNW,
	NMD_X86_INSTRUCTION_KANDQ,
	NMD_X86_INSTRUCTION_KANDW,
	NMD_X86_INSTRUCTION_KMOVB,
	NMD_X86_INSTRUCTION_KMOVD,
	NMD_X86_INSTRUCTION_KMOVQ,
	NMD_X86_INSTRUCTION_KMOVW,
	NMD_X86_INSTRUCTION_KNOTB,
	NMD_X86_INSTRUCTION_KNOTD,
	NMD_X86_INSTRUCTION_KNOTQ,
	NMD_X86_INSTRUCTION_KNOTW,
	NMD_X86_INSTRUCTION_KORB,
	NMD_X86_INSTRUCTION_KORD,
	NMD_X86_INSTRUCTION_KORQ,
	NMD_X86_INSTRUCTION_KORTESTB,
	NMD_X86_INSTRUCTION_KORTESTD,
	NMD_X86_INSTRUCTION_KORTESTQ,
	NMD_X86_INSTRUCTION_KORTESTW,
	NMD_X86_INSTRUCTION_KORW,
	NMD_X86_INSTRUCTION_KSHIFTLB,
	NMD_X86_INSTRUCTION_KSHIFTLD,
	NMD_X86_INSTRUCTION_KSHIFTLQ,
	NMD_X86_INSTRUCTION_KSHIFTLW,
	NMD_X86_INSTRUCTION_KSHIFTRB,
	NMD_X86_INSTRUCTION_KSHIFTRD,
	NMD_X86_INSTRUCTION_KSHIFTRQ,
	NMD_X86_INSTRUCTION_KSHIFTRW,
	NMD_X86_INSTRUCTION_KUNPCKBW,
	NMD_X86_INSTRUCTION_KXNORB,
	NMD_X86_INSTRUCTION_KXNORD,
	NMD_X86_INSTRUCTION_KXNORQ,
	NMD_X86_INSTRUCTION_KXNORW,
	NMD_X86_INSTRUCTION_KXORB,
	NMD_X86_INSTRUCTION_KXORD,
	NMD_X86_INSTRUCTION_KXORQ,
	NMD_X86_INSTRUCTION_KXORW,
	NMD_X86_INSTRUCTION_LAHF,
	NMD_X86_INSTRUCTION_LDS,
	NMD_X86_INSTRUCTION_LEA,
	NMD_X86_INSTRUCTION_LEAVE,
	NMD_X86_INSTRUCTION_LES,
	NMD_X86_INSTRUCTION_LODSB,
	NMD_X86_INSTRUCTION_LODSD,
	NMD_X86_INSTRUCTION_LODSQ,
	NMD_X86_INSTRUCTION_LODSW,
	NMD_X86_INSTRUCTION_RETF,
	NMD_X86_INSTRUCTION_XADD,
	NMD_X86_INSTRUCTION_LZCNT,
	NMD_X86_INSTRUCTION_MASKMOVDQU,
	NMD_X86_INSTRUCTION_CVTPD2PI,
	NMD_X86_INSTRUCTION_CVTPI2PD,
	NMD_X86_INSTRUCTION_CVTPI2PS,
	NMD_X86_INSTRUCTION_CVTPS2PI,
	NMD_X86_INSTRUCTION_CVTTPD2PI,
	NMD_X86_INSTRUCTION_CVTTPS2PI,
	NMD_X86_INSTRUCTION_EMMS,
	NMD_X86_INSTRUCTION_MASKMOVQ,
	NMD_X86_INSTRUCTION_MOVD,
	NMD_X86_INSTRUCTION_MOVDQ2Q,
	NMD_X86_INSTRUCTION_MOVNTQ,
	NMD_X86_INSTRUCTION_MOVQ2DQ,
	NMD_X86_INSTRUCTION_MOVQ,
	NMD_X86_INSTRUCTION_PSHUFW,
	NMD_X86_INSTRUCTION_MONTMUL,
	NMD_X86_INSTRUCTION_MOV,
	NMD_X86_INSTRUCTION_MOVABS,
	NMD_X86_INSTRUCTION_MOVBE,
	NMD_X86_INSTRUCTION_MOVDDUP,
	NMD_X86_INSTRUCTION_MOVDQA,
	NMD_X86_INSTRUCTION_MOVDQU,
	NMD_X86_INSTRUCTION_MOVHLPS,
	NMD_X86_INSTRUCTION_MOVHPD,
	NMD_X86_INSTRUCTION_MOVHPS,
	NMD_X86_INSTRUCTION_MOVLHPS,
	NMD_X86_INSTRUCTION_MOVLPD,
	NMD_X86_INSTRUCTION_MOVLPS,
	NMD_X86_INSTRUCTION_MOVNTDQ,
	NMD_X86_INSTRUCTION_MOVNTPD,
	NMD_X86_INSTRUCTION_MOVNTPS,
	NMD_X86_INSTRUCTION_MOVNTSD,
	NMD_X86_INSTRUCTION_MOVNTSS,
	NMD_X86_INSTRUCTION_MOVSB,
	NMD_X86_INSTRUCTION_MOVSD,
	NMD_X86_INSTRUCTION_MOVSHDUP,
	NMD_X86_INSTRUCTION_MOVSLDUP,
	NMD_X86_INSTRUCTION_MOVSQ,
	NMD_X86_INSTRUCTION_MOVSS,
	NMD_X86_INSTRUCTION_MOVSW,
	NMD_X86_INSTRUCTION_MOVSX,
	NMD_X86_INSTRUCTION_MOVSXD,
	NMD_X86_INSTRUCTION_MOVUPD,
	NMD_X86_INSTRUCTION_MOVUPS,
	NMD_X86_INSTRUCTION_MOVZX,
	NMD_X86_INSTRUCTION_MULX,
	NMD_X86_INSTRUCTION_NOP,
	NMD_X86_INSTRUCTION_OUT,
	NMD_X86_INSTRUCTION_OUTSB,
	NMD_X86_INSTRUCTION_OUTSD,
	NMD_X86_INSTRUCTION_OUTSW,
	NMD_X86_INSTRUCTION_PAUSE,
	NMD_X86_INSTRUCTION_PAVGUSB,
	NMD_X86_INSTRUCTION_PBLENDVB,
	NMD_X86_INSTRUCTION_PCOMMIT,
	NMD_X86_INSTRUCTION_PDEP,
	NMD_X86_INSTRUCTION_PEXT,
	NMD_X86_INSTRUCTION_PEXTRB,
	NMD_X86_INSTRUCTION_PEXTRD,
	NMD_X86_INSTRUCTION_PEXTRQ,
	NMD_X86_INSTRUCTION_PF2ID,
	NMD_X86_INSTRUCTION_PF2IW,
	NMD_X86_INSTRUCTION_PFACC,
	NMD_X86_INSTRUCTION_PFADD,
	NMD_X86_INSTRUCTION_PFCMPEQ,
	NMD_X86_INSTRUCTION_PFCMPGE,
	NMD_X86_INSTRUCTION_PFCMPGT,
	NMD_X86_INSTRUCTION_PFMAX,
	NMD_X86_INSTRUCTION_PFMIN,
	NMD_X86_INSTRUCTION_PFMUL,
	NMD_X86_INSTRUCTION_PFNACC,
	NMD_X86_INSTRUCTION_PFPNACC,
	NMD_X86_INSTRUCTION_PFRCPIT1,
	NMD_X86_INSTRUCTION_PFRCPIT2,
	NMD_X86_INSTRUCTION_PFRCP,
	NMD_X86_INSTRUCTION_PFRSQIT1,
	NMD_X86_INSTRUCTION_PFRSQRT,
	NMD_X86_INSTRUCTION_PFSUBR,
	NMD_X86_INSTRUCTION_PFSUB,
	NMD_X86_INSTRUCTION_PHMINPOSUW,
	NMD_X86_INSTRUCTION_PI2FD,
	NMD_X86_INSTRUCTION_PI2FW,
	NMD_X86_INSTRUCTION_PINSRB,
	NMD_X86_INSTRUCTION_PINSRD,
	NMD_X86_INSTRUCTION_PINSRQ,
	NMD_X86_INSTRUCTION_PMULHRW,
	NMD_X86_INSTRUCTION_PMULLD,
	NMD_X86_INSTRUCTION_POP,
	NMD_X86_INSTRUCTION_POPA,
	NMD_X86_INSTRUCTION_POPAD,
	NMD_X86_INSTRUCTION_POPCNT,
	NMD_X86_INSTRUCTION_POPF,
	NMD_X86_INSTRUCTION_POPFD,
	NMD_X86_INSTRUCTION_POPFQ,
	NMD_X86_INSTRUCTION_PREFETCH,
	NMD_X86_INSTRUCTION_PREFETCHNTA,
	NMD_X86_INSTRUCTION_PREFETCHT0,
	NMD_X86_INSTRUCTION_PREFETCHT1,
	NMD_X86_INSTRUCTION_PREFETCHT2,
	NMD_X86_INSTRUCTION_PSHUFD,
	NMD_X86_INSTRUCTION_PSHUFHW,
	NMD_X86_INSTRUCTION_PSHUFLW,
	NMD_X86_INSTRUCTION_PSLLDQ,
	NMD_X86_INSTRUCTION_PSRLDQ,
	NMD_X86_INSTRUCTION_PSWAPD,
	NMD_X86_INSTRUCTION_PTEST,
	NMD_X86_INSTRUCTION_PUSHA,
	NMD_X86_INSTRUCTION_PUSHAD,
	NMD_X86_INSTRUCTION_PUSHF,
	NMD_X86_INSTRUCTION_PUSHFD,
	NMD_X86_INSTRUCTION_PUSHFQ,
	NMD_X86_INSTRUCTION_RDRAND,
	NMD_X86_INSTRUCTION_RDPID,
	NMD_X86_INSTRUCTION_RDSEED,
	NMD_X86_INSTRUCTION_RDTSCP,
	NMD_X86_INSTRUCTION_RORX,
	NMD_X86_INSTRUCTION_RSM,
	NMD_X86_INSTRUCTION_SAHF,
	NMD_X86_INSTRUCTION_SAL,
	NMD_X86_INSTRUCTION_SALC,
	NMD_X86_INSTRUCTION_SARX,
	NMD_X86_INSTRUCTION_SCASB,
	NMD_X86_INSTRUCTION_SCASD,
	NMD_X86_INSTRUCTION_SCASQ,
	NMD_X86_INSTRUCTION_SCASW,
	NMD_X86_INSTRUCTION_SHA1RNDS4,
	NMD_X86_INSTRUCTION_SHLD,
	NMD_X86_INSTRUCTION_SHLX,
	NMD_X86_INSTRUCTION_SHRD,
	NMD_X86_INSTRUCTION_SHRX,
	NMD_X86_INSTRUCTION_SHUFPD,
	NMD_X86_INSTRUCTION_SHUFPS,
	NMD_X86_INSTRUCTION_STOSB,
	NMD_X86_INSTRUCTION_STOSD,
	NMD_X86_INSTRUCTION_STOSQ,
	NMD_X86_INSTRUCTION_STOSW,
	NMD_X86_INSTRUCTION_FSTPNCE,
	NMD_X86_INSTRUCTION_FXCH,
	NMD_X86_INSTRUCTION_SWAPGS,
	NMD_X86_INSTRUCTION_T1MSKC,
	NMD_X86_INSTRUCTION_TZCNT,
	NMD_X86_INSTRUCTION_TZMSK,
	NMD_X86_INSTRUCTION_FUCOMIP,
	NMD_X86_INSTRUCTION_FUCOMPP,
	NMD_X86_INSTRUCTION_FUCOMP,
	NMD_X86_INSTRUCTION_FUCOM,
	NMD_X86_INSTRUCTION_UD1,
	NMD_X86_INSTRUCTION_UNPCKHPD,
	NMD_X86_INSTRUCTION_UNPCKHPS,
	NMD_X86_INSTRUCTION_UNPCKLPD,
	NMD_X86_INSTRUCTION_UNPCKLPS,
	NMD_X86_INSTRUCTION_VADDPD,
	NMD_X86_INSTRUCTION_VADDPS,
	NMD_X86_INSTRUCTION_VADDSD,
	NMD_X86_INSTRUCTION_VADDSS,
	NMD_X86_INSTRUCTION_VADDSUBPD,
	NMD_X86_INSTRUCTION_VADDSUBPS,
	NMD_X86_INSTRUCTION_VAESDECLAST,
	NMD_X86_INSTRUCTION_VAESDEC,
	NMD_X86_INSTRUCTION_VAESENCLAST,
	NMD_X86_INSTRUCTION_VAESENC,
	NMD_X86_INSTRUCTION_VAESIMC,
	NMD_X86_INSTRUCTION_VAESKEYGENASSIST,
	NMD_X86_INSTRUCTION_VALIGND,
	NMD_X86_INSTRUCTION_VALIGNQ,
	NMD_X86_INSTRUCTION_VANDNPD,
	NMD_X86_INSTRUCTION_VANDNPS,
	NMD_X86_INSTRUCTION_VANDPD,
	NMD_X86_INSTRUCTION_VANDPS,
	NMD_X86_INSTRUCTION_VBLENDMPD,
	NMD_X86_INSTRUCTION_VBLENDMPS,
	NMD_X86_INSTRUCTION_VBLENDPD,
	NMD_X86_INSTRUCTION_VBLENDPS,
	NMD_X86_INSTRUCTION_VBLENDVPD,
	NMD_X86_INSTRUCTION_VBLENDVPS,
	NMD_X86_INSTRUCTION_VBROADCASTF128,
	NMD_X86_INSTRUCTION_VBROADCASTI32X4,
	NMD_X86_INSTRUCTION_VBROADCASTI64X4,
	NMD_X86_INSTRUCTION_VBROADCASTSD,
	NMD_X86_INSTRUCTION_VBROADCASTSS,
	NMD_X86_INSTRUCTION_VCOMPRESSPD,
	NMD_X86_INSTRUCTION_VCOMPRESSPS,
	NMD_X86_INSTRUCTION_VCVTDQ2PD,
	NMD_X86_INSTRUCTION_VCVTDQ2PS,
	NMD_X86_INSTRUCTION_VCVTPD2DQX,
	NMD_X86_INSTRUCTION_VCVTPD2DQ,
	NMD_X86_INSTRUCTION_VCVTPD2PSX,
	NMD_X86_INSTRUCTION_VCVTPD2PS,
	NMD_X86_INSTRUCTION_VCVTPD2UDQ,
	NMD_X86_INSTRUCTION_VCVTPH2PS,
	NMD_X86_INSTRUCTION_VCVTPS2DQ,
	NMD_X86_INSTRUCTION_VCVTPS2PD,
	NMD_X86_INSTRUCTION_VCVTPS2PH,
	NMD_X86_INSTRUCTION_VCVTPS2UDQ,
	NMD_X86_INSTRUCTION_VCVTSD2SI,
	NMD_X86_INSTRUCTION_VCVTSD2USI,
	NMD_X86_INSTRUCTION_VCVTSS2SI,
	NMD_X86_INSTRUCTION_VCVTSS2USI,
	NMD_X86_INSTRUCTION_VCVTTPD2DQX,
	NMD_X86_INSTRUCTION_VCVTTPD2DQ,
	NMD_X86_INSTRUCTION_VCVTTPD2UDQ,
	NMD_X86_INSTRUCTION_VCVTTPS2DQ,
	NMD_X86_INSTRUCTION_VCVTTPS2UDQ,
	NMD_X86_INSTRUCTION_VCVTUDQ2PD,
	NMD_X86_INSTRUCTION_VCVTUDQ2PS,
	NMD_X86_INSTRUCTION_VDIVPD,
	NMD_X86_INSTRUCTION_VDIVPS,
	NMD_X86_INSTRUCTION_VDIVSD,
	NMD_X86_INSTRUCTION_VDIVSS,
	NMD_X86_INSTRUCTION_VDPPD,
	NMD_X86_INSTRUCTION_VDPPS,
	NMD_X86_INSTRUCTION_VEXP2PD,
	NMD_X86_INSTRUCTION_VEXP2PS,
	NMD_X86_INSTRUCTION_VEXPANDPD,
	NMD_X86_INSTRUCTION_VEXPANDPS,
	NMD_X86_INSTRUCTION_VEXTRACTF128,
	NMD_X86_INSTRUCTION_VEXTRACTF32X4,
	NMD_X86_INSTRUCTION_VEXTRACTF64X4,
	NMD_X86_INSTRUCTION_VEXTRACTI128,
	NMD_X86_INSTRUCTION_VEXTRACTI32X4,
	NMD_X86_INSTRUCTION_VEXTRACTI64X4,
	NMD_X86_INSTRUCTION_VEXTRACTPS,
	NMD_X86_INSTRUCTION_VFMADD132PD,
	NMD_X86_INSTRUCTION_VFMADD132PS,
	NMD_X86_INSTRUCTION_VFMADDPD,
	NMD_X86_INSTRUCTION_VFMADD213PD,
	NMD_X86_INSTRUCTION_VFMADD231PD,
	NMD_X86_INSTRUCTION_VFMADDPS,
	NMD_X86_INSTRUCTION_VFMADD213PS,
	NMD_X86_INSTRUCTION_VFMADD231PS,
	NMD_X86_INSTRUCTION_VFMADDSD,
	NMD_X86_INSTRUCTION_VFMADD213SD,
	NMD_X86_INSTRUCTION_VFMADD132SD,
	NMD_X86_INSTRUCTION_VFMADD231SD,
	NMD_X86_INSTRUCTION_VFMADDSS,
	NMD_X86_INSTRUCTION_VFMADD213SS,
	NMD_X86_INSTRUCTION_VFMADD132SS,
	NMD_X86_INSTRUCTION_VFMADD231SS,
	NMD_X86_INSTRUCTION_VFMADDSUB132PD,
	NMD_X86_INSTRUCTION_VFMADDSUB132PS,
	NMD_X86_INSTRUCTION_VFMADDSUBPD,
	NMD_X86_INSTRUCTION_VFMADDSUB213PD,
	NMD_X86_INSTRUCTION_VFMADDSUB231PD,
	NMD_X86_INSTRUCTION_VFMADDSUBPS,
	NMD_X86_INSTRUCTION_VFMADDSUB213PS,
	NMD_X86_INSTRUCTION_VFMADDSUB231PS,
	NMD_X86_INSTRUCTION_VFMSUB132PD,
	NMD_X86_INSTRUCTION_VFMSUB132PS,
	NMD_X86_INSTRUCTION_VFMSUBADD132PD,
	NMD_X86_INSTRUCTION_VFMSUBADD132PS,
	NMD_X86_INSTRUCTION_VFMSUBADDPD,
	NMD_X86_INSTRUCTION_VFMSUBADD213PD,
	NMD_X86_INSTRUCTION_VFMSUBADD231PD,
	NMD_X86_INSTRUCTION_VFMSUBADDPS,
	NMD_X86_INSTRUCTION_VFMSUBADD213PS,
	NMD_X86_INSTRUCTION_VFMSUBADD231PS,
	NMD_X86_INSTRUCTION_VFMSUBPD,
	NMD_X86_INSTRUCTION_VFMSUB213PD,
	NMD_X86_INSTRUCTION_VFMSUB231PD,
	NMD_X86_INSTRUCTION_VFMSUBPS,
	NMD_X86_INSTRUCTION_VFMSUB213PS,
	NMD_X86_INSTRUCTION_VFMSUB231PS,
	NMD_X86_INSTRUCTION_VFMSUBSD,
	NMD_X86_INSTRUCTION_VFMSUB213SD,
	NMD_X86_INSTRUCTION_VFMSUB132SD,
	NMD_X86_INSTRUCTION_VFMSUB231SD,
	NMD_X86_INSTRUCTION_VFMSUBSS,
	NMD_X86_INSTRUCTION_VFMSUB213SS,
	NMD_X86_INSTRUCTION_VFMSUB132SS,
	NMD_X86_INSTRUCTION_VFMSUB231SS,
	NMD_X86_INSTRUCTION_VFNMADD132PD,
	NMD_X86_INSTRUCTION_VFNMADD132PS,
	NMD_X86_INSTRUCTION_VFNMADDPD,
	NMD_X86_INSTRUCTION_VFNMADD213PD,
	NMD_X86_INSTRUCTION_VFNMADD231PD,
	NMD_X86_INSTRUCTION_VFNMADDPS,
	NMD_X86_INSTRUCTION_VFNMADD213PS,
	NMD_X86_INSTRUCTION_VFNMADD231PS,
	NMD_X86_INSTRUCTION_VFNMADDSD,
	NMD_X86_INSTRUCTION_VFNMADD213SD,
	NMD_X86_INSTRUCTION_VFNMADD132SD,
	NMD_X86_INSTRUCTION_VFNMADD231SD,
	NMD_X86_INSTRUCTION_VFNMADDSS,
	NMD_X86_INSTRUCTION_VFNMADD213SS,
	NMD_X86_INSTRUCTION_VFNMADD132SS,
	NMD_X86_INSTRUCTION_VFNMADD231SS,
	NMD_X86_INSTRUCTION_VFNMSUB132PD,
	NMD_X86_INSTRUCTION_VFNMSUB132PS,
	NMD_X86_INSTRUCTION_VFNMSUBPD,
	NMD_X86_INSTRUCTION_VFNMSUB213PD,
	NMD_X86_INSTRUCTION_VFNMSUB231PD,
	NMD_X86_INSTRUCTION_VFNMSUBPS,
	NMD_X86_INSTRUCTION_VFNMSUB213PS,
	NMD_X86_INSTRUCTION_VFNMSUB231PS,
	NMD_X86_INSTRUCTION_VFNMSUBSD,
	NMD_X86_INSTRUCTION_VFNMSUB213SD,
	NMD_X86_INSTRUCTION_VFNMSUB132SD,
	NMD_X86_INSTRUCTION_VFNMSUB231SD,
	NMD_X86_INSTRUCTION_VFNMSUBSS,
	NMD_X86_INSTRUCTION_VFNMSUB213SS,
	NMD_X86_INSTRUCTION_VFNMSUB132SS,
	NMD_X86_INSTRUCTION_VFNMSUB231SS,
	NMD_X86_INSTRUCTION_VFRCZPD,
	NMD_X86_INSTRUCTION_VFRCZPS,
	NMD_X86_INSTRUCTION_VFRCZSD,
	NMD_X86_INSTRUCTION_VFRCZSS,
	NMD_X86_INSTRUCTION_VORPD,
	NMD_X86_INSTRUCTION_VORPS,
	NMD_X86_INSTRUCTION_VXORPD,
	NMD_X86_INSTRUCTION_VXORPS,
	NMD_X86_INSTRUCTION_VGATHERDPD,
	NMD_X86_INSTRUCTION_VGATHERDPS,
	NMD_X86_INSTRUCTION_VGATHERPF0DPD,
	NMD_X86_INSTRUCTION_VGATHERPF0DPS,
	NMD_X86_INSTRUCTION_VGATHERPF0QPD,
	NMD_X86_INSTRUCTION_VGATHERPF0QPS,
	NMD_X86_INSTRUCTION_VGATHERPF1DPD,
	NMD_X86_INSTRUCTION_VGATHERPF1DPS,
	NMD_X86_INSTRUCTION_VGATHERPF1QPD,
	NMD_X86_INSTRUCTION_VGATHERPF1QPS,
	NMD_X86_INSTRUCTION_VGATHERQPD,
	NMD_X86_INSTRUCTION_VGATHERQPS,
	NMD_X86_INSTRUCTION_VHADDPD,
	NMD_X86_INSTRUCTION_VHADDPS,
	NMD_X86_INSTRUCTION_VHSUBPD,
	NMD_X86_INSTRUCTION_VHSUBPS,
	NMD_X86_INSTRUCTION_VINSERTF128,
	NMD_X86_INSTRUCTION_VINSERTF32X4,
	NMD_X86_INSTRUCTION_VINSERTF32X8,
	NMD_X86_INSTRUCTION_VINSERTF64X2,
	NMD_X86_INSTRUCTION_VINSERTF64X4,
	NMD_X86_INSTRUCTION_VINSERTI128,
	NMD_X86_INSTRUCTION_VINSERTI32X4,
	NMD_X86_INSTRUCTION_VINSERTI32X8,
	NMD_X86_INSTRUCTION_VINSERTI64X2,
	NMD_X86_INSTRUCTION_VINSERTI64X4,
	NMD_X86_INSTRUCTION_VINSERTPS,
	NMD_X86_INSTRUCTION_VLDDQU,
	NMD_X86_INSTRUCTION_VLDMXCSR,
	NMD_X86_INSTRUCTION_VMASKMOVDQU,
	NMD_X86_INSTRUCTION_VMASKMOVPD,
	NMD_X86_INSTRUCTION_VMASKMOVPS,
	NMD_X86_INSTRUCTION_VMAXPD,
	NMD_X86_INSTRUCTION_VMAXPS,
	NMD_X86_INSTRUCTION_VMAXSD,
	NMD_X86_INSTRUCTION_VMAXSS,
	NMD_X86_INSTRUCTION_VMCLEAR,
	NMD_X86_INSTRUCTION_VMINPD,
	NMD_X86_INSTRUCTION_VMINPS,
	NMD_X86_INSTRUCTION_VMINSD,
	NMD_X86_INSTRUCTION_VMINSS,
	NMD_X86_INSTRUCTION_VMOVQ,
	NMD_X86_INSTRUCTION_VMOVDDUP,
	NMD_X86_INSTRUCTION_VMOVD,
	NMD_X86_INSTRUCTION_VMOVDQA32,
	NMD_X86_INSTRUCTION_VMOVDQA64,
	NMD_X86_INSTRUCTION_VMOVDQA,
	NMD_X86_INSTRUCTION_VMOVDQU16,
	NMD_X86_INSTRUCTION_VMOVDQU32,
	NMD_X86_INSTRUCTION_VMOVDQU64,
	NMD_X86_INSTRUCTION_VMOVDQU8,
	NMD_X86_INSTRUCTION_VMOVDQU,
	NMD_X86_INSTRUCTION_VMOVHLPS,
	NMD_X86_INSTRUCTION_VMOVHPD,
	NMD_X86_INSTRUCTION_VMOVHPS,
	NMD_X86_INSTRUCTION_VMOVLHPS,
	NMD_X86_INSTRUCTION_VMOVLPD,
	NMD_X86_INSTRUCTION_VMOVLPS,
	NMD_X86_INSTRUCTION_VMOVMSKPD,
	NMD_X86_INSTRUCTION_VMOVMSKPS,
	NMD_X86_INSTRUCTION_VMOVNTDQA,
	NMD_X86_INSTRUCTION_VMOVNTDQ,
	NMD_X86_INSTRUCTION_VMOVNTPD,
	NMD_X86_INSTRUCTION_VMOVNTPS,
	NMD_X86_INSTRUCTION_VMOVSD,
	NMD_X86_INSTRUCTION_VMOVSHDUP,
	NMD_X86_INSTRUCTION_VMOVSLDUP,
	NMD_X86_INSTRUCTION_VMOVSS,
	NMD_X86_INSTRUCTION_VMOVUPD,
	NMD_X86_INSTRUCTION_VMOVUPS,
	NMD_X86_INSTRUCTION_VMPSADBW,
	NMD_X86_INSTRUCTION_VMPTRLD,
	NMD_X86_INSTRUCTION_VMPTRST,
	NMD_X86_INSTRUCTION_VMREAD,
	NMD_X86_INSTRUCTION_VMULPD,
	NMD_X86_INSTRUCTION_VMULPS,
	NMD_X86_INSTRUCTION_VMULSD,
	NMD_X86_INSTRUCTION_VMULSS,
	NMD_X86_INSTRUCTION_VMWRITE,
	NMD_X86_INSTRUCTION_VMXON,
	NMD_X86_INSTRUCTION_VPABSB,
	NMD_X86_INSTRUCTION_VPABSD,
	NMD_X86_INSTRUCTION_VPABSQ,
	NMD_X86_INSTRUCTION_VPABSW,
	NMD_X86_INSTRUCTION_VPACKSSDW,
	NMD_X86_INSTRUCTION_VPACKSSWB,
	NMD_X86_INSTRUCTION_VPACKUSDW,
	NMD_X86_INSTRUCTION_VPACKUSWB,
	NMD_X86_INSTRUCTION_VPADDB,
	NMD_X86_INSTRUCTION_VPADDD,
	NMD_X86_INSTRUCTION_VPADDQ,
	NMD_X86_INSTRUCTION_VPADDSB,
	NMD_X86_INSTRUCTION_VPADDSW,
	NMD_X86_INSTRUCTION_VPADDUSB,
	NMD_X86_INSTRUCTION_VPADDUSW,
	NMD_X86_INSTRUCTION_VPADDW,
	NMD_X86_INSTRUCTION_VPALIGNR,
	NMD_X86_INSTRUCTION_VPANDD,
	NMD_X86_INSTRUCTION_VPANDND,
	NMD_X86_INSTRUCTION_VPANDNQ,
	NMD_X86_INSTRUCTION_VPANDN,
	NMD_X86_INSTRUCTION_VPANDQ,
	NMD_X86_INSTRUCTION_VPAND,
	NMD_X86_INSTRUCTION_VPAVGB,
	NMD_X86_INSTRUCTION_VPAVGW,
	NMD_X86_INSTRUCTION_VPBLENDD,
	NMD_X86_INSTRUCTION_VPBLENDMB,
	NMD_X86_INSTRUCTION_VPBLENDMD,
	NMD_X86_INSTRUCTION_VPBLENDMQ,
	NMD_X86_INSTRUCTION_VPBLENDMW,
	NMD_X86_INSTRUCTION_VPBLENDVB,
	NMD_X86_INSTRUCTION_VPBLENDW,
	NMD_X86_INSTRUCTION_VPBROADCASTB,
	NMD_X86_INSTRUCTION_VPBROADCASTD,
	NMD_X86_INSTRUCTION_VPBROADCASTMB2Q,
	NMD_X86_INSTRUCTION_VPBROADCASTMW2D,
	NMD_X86_INSTRUCTION_VPBROADCASTQ,
	NMD_X86_INSTRUCTION_VPBROADCASTW,
	NMD_X86_INSTRUCTION_VPCLMULQDQ,
	NMD_X86_INSTRUCTION_VPCMOV,
	NMD_X86_INSTRUCTION_VPCMPB,
	NMD_X86_INSTRUCTION_VPCMPD,
	NMD_X86_INSTRUCTION_VPCMPEQB,
	NMD_X86_INSTRUCTION_VPCMPEQD,
	NMD_X86_INSTRUCTION_VPCMPEQQ,
	NMD_X86_INSTRUCTION_VPCMPEQW,
	NMD_X86_INSTRUCTION_VPCMPESTRI,
	NMD_X86_INSTRUCTION_VPCMPESTRM,
	NMD_X86_INSTRUCTION_VPCMPGTB,
	NMD_X86_INSTRUCTION_VPCMPGTD,
	NMD_X86_INSTRUCTION_VPCMPGTW,
	NMD_X86_INSTRUCTION_VPCMPISTRI,
	NMD_X86_INSTRUCTION_VPCMPISTRM,
	NMD_X86_INSTRUCTION_VPCMPQ,
	NMD_X86_INSTRUCTION_VPCMPUB,
	NMD_X86_INSTRUCTION_VPCMPUD,
	NMD_X86_INSTRUCTION_VPCMPUQ,
	NMD_X86_INSTRUCTION_VPCMPUW,
	NMD_X86_INSTRUCTION_VPCMPW,
	NMD_X86_INSTRUCTION_VPCOMB,
	NMD_X86_INSTRUCTION_VPCOMD,
	NMD_X86_INSTRUCTION_VPCOMPRESSD,
	NMD_X86_INSTRUCTION_VPCOMPRESSQ,
	NMD_X86_INSTRUCTION_VPCOMQ,
	NMD_X86_INSTRUCTION_VPCOMUB,
	NMD_X86_INSTRUCTION_VPCOMUD,
	NMD_X86_INSTRUCTION_VPCOMUQ,
	NMD_X86_INSTRUCTION_VPCOMUW,
	NMD_X86_INSTRUCTION_VPCOMW,
	NMD_X86_INSTRUCTION_VPCONFLICTD,
	NMD_X86_INSTRUCTION_VPCONFLICTQ,
	NMD_X86_INSTRUCTION_VPERM2F128,
	NMD_X86_INSTRUCTION_VPERM2I128,
	NMD_X86_INSTRUCTION_VPERMD,
	NMD_X86_INSTRUCTION_VPERMI2D,
	NMD_X86_INSTRUCTION_VPERMI2PD,
	NMD_X86_INSTRUCTION_VPERMI2PS,
	NMD_X86_INSTRUCTION_VPERMI2Q,
	NMD_X86_INSTRUCTION_VPERMIL2PD,
	NMD_X86_INSTRUCTION_VPERMIL2PS,
	NMD_X86_INSTRUCTION_VPERMILPD,
	NMD_X86_INSTRUCTION_VPERMILPS,
	NMD_X86_INSTRUCTION_VPERMPD,
	NMD_X86_INSTRUCTION_VPERMPS,
	NMD_X86_INSTRUCTION_VPERMQ,
	NMD_X86_INSTRUCTION_VPERMT2D,
	NMD_X86_INSTRUCTION_VPERMT2PD,
	NMD_X86_INSTRUCTION_VPERMT2PS,
	NMD_X86_INSTRUCTION_VPERMT2Q,
	NMD_X86_INSTRUCTION_VPEXPANDD,
	NMD_X86_INSTRUCTION_VPEXPANDQ,
	NMD_X86_INSTRUCTION_VPEXTRB,
	NMD_X86_INSTRUCTION_VPEXTRD,
	NMD_X86_INSTRUCTION_VPEXTRQ,
	NMD_X86_INSTRUCTION_VPEXTRW,
	NMD_X86_INSTRUCTION_VPGATHERDD,
	NMD_X86_INSTRUCTION_VPGATHERDQ,
	NMD_X86_INSTRUCTION_VPGATHERQD,
	NMD_X86_INSTRUCTION_VPGATHERQQ,
	NMD_X86_INSTRUCTION_VPHADDBD,
	NMD_X86_INSTRUCTION_VPHADDBQ,
	NMD_X86_INSTRUCTION_VPHADDBW,
	NMD_X86_INSTRUCTION_VPHADDUBD,
	NMD_X86_INSTRUCTION_VPHADDUBQ,
	NMD_X86_INSTRUCTION_VPHADDUBW,
	NMD_X86_INSTRUCTION_VPHADDUDQ,
	NMD_X86_INSTRUCTION_VPHADDUWD,
	NMD_X86_INSTRUCTION_VPHADDUWQ,
	NMD_X86_INSTRUCTION_VPHADDWD,
	NMD_X86_INSTRUCTION_VPHMINPOSUW,
	NMD_X86_INSTRUCTION_VPHSUBBW,
	NMD_X86_INSTRUCTION_VPHSUBDQ,
	
	NMD_X86_INSTRUCTION_VPHSUBWD,
	
	NMD_X86_INSTRUCTION_VPINSRB,
	NMD_X86_INSTRUCTION_VPINSRD,
	NMD_X86_INSTRUCTION_VPINSRQ,
	NMD_X86_INSTRUCTION_VPINSRW,
	NMD_X86_INSTRUCTION_VPLZCNTD,
	NMD_X86_INSTRUCTION_VPLZCNTQ,
	NMD_X86_INSTRUCTION_VPMACSDD,
	NMD_X86_INSTRUCTION_VPMACSDQH,
	NMD_X86_INSTRUCTION_VPMACSDQL,
	NMD_X86_INSTRUCTION_VPMACSSDD,
	NMD_X86_INSTRUCTION_VPMACSSDQH,
	NMD_X86_INSTRUCTION_VPMACSSDQL,
	NMD_X86_INSTRUCTION_VPMACSSWD,
	NMD_X86_INSTRUCTION_VPMACSSWW,
	NMD_X86_INSTRUCTION_VPMACSWD,
	NMD_X86_INSTRUCTION_VPMACSWW,
	NMD_X86_INSTRUCTION_VPMADCSSWD,
	NMD_X86_INSTRUCTION_VPMADCSWD,
	NMD_X86_INSTRUCTION_VPMADDWD,
	NMD_X86_INSTRUCTION_VPMASKMOVD,
	NMD_X86_INSTRUCTION_VPMASKMOVQ,
	NMD_X86_INSTRUCTION_VPMAXSB,
	NMD_X86_INSTRUCTION_VPMAXSD,
	NMD_X86_INSTRUCTION_VPMAXSQ,
	NMD_X86_INSTRUCTION_VPMAXSW,
	NMD_X86_INSTRUCTION_VPMAXUB,
	NMD_X86_INSTRUCTION_VPMAXUD,
	NMD_X86_INSTRUCTION_VPMAXUQ,
	NMD_X86_INSTRUCTION_VPMAXUW,
	NMD_X86_INSTRUCTION_VPMINSB,
	NMD_X86_INSTRUCTION_VPMINSD,
	NMD_X86_INSTRUCTION_VPMINSQ,
	NMD_X86_INSTRUCTION_VPMINSW,
	NMD_X86_INSTRUCTION_VPMINUB,
	NMD_X86_INSTRUCTION_VPMINUD,
	NMD_X86_INSTRUCTION_VPMINUQ,
	NMD_X86_INSTRUCTION_VPMINUW,
	NMD_X86_INSTRUCTION_VPMOVDB,
	NMD_X86_INSTRUCTION_VPMOVDW,
	NMD_X86_INSTRUCTION_VPMOVM2B,
	NMD_X86_INSTRUCTION_VPMOVM2D,
	NMD_X86_INSTRUCTION_VPMOVM2Q,
	NMD_X86_INSTRUCTION_VPMOVM2W,
	NMD_X86_INSTRUCTION_VPMOVMSKB,
	NMD_X86_INSTRUCTION_VPMOVQB,
	NMD_X86_INSTRUCTION_VPMOVQD,
	NMD_X86_INSTRUCTION_VPMOVQW,
	NMD_X86_INSTRUCTION_VPMOVSDB,
	NMD_X86_INSTRUCTION_VPMOVSDW,
	NMD_X86_INSTRUCTION_VPMOVSQB,
	NMD_X86_INSTRUCTION_VPMOVSQD,
	NMD_X86_INSTRUCTION_VPMOVSQW,
	NMD_X86_INSTRUCTION_VPMOVSXBD,
	NMD_X86_INSTRUCTION_VPMOVSXBQ,
	NMD_X86_INSTRUCTION_VPMOVSXBW,
	NMD_X86_INSTRUCTION_VPMOVSXDQ,
	NMD_X86_INSTRUCTION_VPMOVSXWD,
	NMD_X86_INSTRUCTION_VPMOVSXWQ,
	NMD_X86_INSTRUCTION_VPMOVUSDB,
	NMD_X86_INSTRUCTION_VPMOVUSDW,
	NMD_X86_INSTRUCTION_VPMOVUSQB,
	NMD_X86_INSTRUCTION_VPMOVUSQD,
	NMD_X86_INSTRUCTION_VPMOVUSQW,
	NMD_X86_INSTRUCTION_VPMOVZXBD,
	NMD_X86_INSTRUCTION_VPMOVZXBQ,
	NMD_X86_INSTRUCTION_VPMOVZXBW,
	NMD_X86_INSTRUCTION_VPMOVZXDQ,
	NMD_X86_INSTRUCTION_VPMOVZXWD,
	NMD_X86_INSTRUCTION_VPMOVZXWQ,
	NMD_X86_INSTRUCTION_VPMULDQ,
	NMD_X86_INSTRUCTION_VPMULHUW,
	NMD_X86_INSTRUCTION_VPMULHW,
	NMD_X86_INSTRUCTION_VPMULLD,
	NMD_X86_INSTRUCTION_VPMULLQ,
	NMD_X86_INSTRUCTION_VPMULLW,
	NMD_X86_INSTRUCTION_VPMULUDQ,
	NMD_X86_INSTRUCTION_VPORD,
	NMD_X86_INSTRUCTION_VPORQ,
	NMD_X86_INSTRUCTION_VPOR,
	NMD_X86_INSTRUCTION_VPPERM,
	NMD_X86_INSTRUCTION_VPROTB,
	NMD_X86_INSTRUCTION_VPROTD,
	NMD_X86_INSTRUCTION_VPROTQ,
	NMD_X86_INSTRUCTION_VPROTW,
	NMD_X86_INSTRUCTION_VPSADBW,
	NMD_X86_INSTRUCTION_VPSCATTERDD,
	NMD_X86_INSTRUCTION_VPSCATTERDQ,
	NMD_X86_INSTRUCTION_VPSCATTERQD,
	NMD_X86_INSTRUCTION_VPSCATTERQQ,
	NMD_X86_INSTRUCTION_VPSHAB,
	NMD_X86_INSTRUCTION_VPSHAD,
	NMD_X86_INSTRUCTION_VPSHAQ,
	NMD_X86_INSTRUCTION_VPSHAW,
	NMD_X86_INSTRUCTION_VPSHLB,
	NMD_X86_INSTRUCTION_VPSHLD,
	NMD_X86_INSTRUCTION_VPSHLQ,
	NMD_X86_INSTRUCTION_VPSHLW,
	NMD_X86_INSTRUCTION_VPSHUFD,
	NMD_X86_INSTRUCTION_VPSHUFHW,
	NMD_X86_INSTRUCTION_VPSHUFLW,
	NMD_X86_INSTRUCTION_VPSLLDQ,
	NMD_X86_INSTRUCTION_VPSLLD,
	NMD_X86_INSTRUCTION_VPSLLQ,
	NMD_X86_INSTRUCTION_VPSLLVD,
	NMD_X86_INSTRUCTION_VPSLLVQ,
	NMD_X86_INSTRUCTION_VPSLLW,
	NMD_X86_INSTRUCTION_VPSRAD,
	NMD_X86_INSTRUCTION_VPSRAQ,
	NMD_X86_INSTRUCTION_VPSRAVD,
	NMD_X86_INSTRUCTION_VPSRAVQ,
	NMD_X86_INSTRUCTION_VPSRAW,
	NMD_X86_INSTRUCTION_VPSRLDQ,
	NMD_X86_INSTRUCTION_VPSRLD,
	NMD_X86_INSTRUCTION_VPSRLQ,
	NMD_X86_INSTRUCTION_VPSRLVD,
	NMD_X86_INSTRUCTION_VPSRLVQ,
	NMD_X86_INSTRUCTION_VPSRLW,
	NMD_X86_INSTRUCTION_VPSUBB,
	NMD_X86_INSTRUCTION_VPSUBD,
	NMD_X86_INSTRUCTION_VPSUBQ,
	NMD_X86_INSTRUCTION_VPSUBSB,
	NMD_X86_INSTRUCTION_VPSUBSW,
	NMD_X86_INSTRUCTION_VPSUBUSB,
	NMD_X86_INSTRUCTION_VPSUBUSW,
	NMD_X86_INSTRUCTION_VPSUBW,
	NMD_X86_INSTRUCTION_VPTESTMD,
	NMD_X86_INSTRUCTION_VPTESTMQ,
	NMD_X86_INSTRUCTION_VPTESTNMD,
	NMD_X86_INSTRUCTION_VPTESTNMQ,
	NMD_X86_INSTRUCTION_VPTEST,
	NMD_X86_INSTRUCTION_VPUNPCKHBW,
	NMD_X86_INSTRUCTION_VPUNPCKHDQ,
	NMD_X86_INSTRUCTION_VPUNPCKHQDQ,
	NMD_X86_INSTRUCTION_VPUNPCKHWD,
	NMD_X86_INSTRUCTION_VPUNPCKLBW,
	NMD_X86_INSTRUCTION_VPUNPCKLDQ,
	NMD_X86_INSTRUCTION_VPUNPCKLQDQ,
	NMD_X86_INSTRUCTION_VPUNPCKLWD,
	NMD_X86_INSTRUCTION_VPXORD,
	NMD_X86_INSTRUCTION_VPXORQ,
	NMD_X86_INSTRUCTION_VPXOR,
	NMD_X86_INSTRUCTION_VRCP14PD,
	NMD_X86_INSTRUCTION_VRCP14PS,
	NMD_X86_INSTRUCTION_VRCP14SD,
	NMD_X86_INSTRUCTION_VRCP14SS,
	NMD_X86_INSTRUCTION_VRCP28PD,
	NMD_X86_INSTRUCTION_VRCP28PS,
	NMD_X86_INSTRUCTION_VRCP28SD,
	NMD_X86_INSTRUCTION_VRCP28SS,
	NMD_X86_INSTRUCTION_VRCPPS,
	NMD_X86_INSTRUCTION_VRCPSS,
	NMD_X86_INSTRUCTION_VRNDSCALEPD,
	NMD_X86_INSTRUCTION_VRNDSCALEPS,
	NMD_X86_INSTRUCTION_VRNDSCALESD,
	NMD_X86_INSTRUCTION_VRNDSCALESS,
	NMD_X86_INSTRUCTION_VROUNDPD,
	NMD_X86_INSTRUCTION_VROUNDPS,
	NMD_X86_INSTRUCTION_VROUNDSD,
	NMD_X86_INSTRUCTION_VROUNDSS,
	NMD_X86_INSTRUCTION_VRSQRT14PD,
	NMD_X86_INSTRUCTION_VRSQRT14PS,
	NMD_X86_INSTRUCTION_VRSQRT14SD,
	NMD_X86_INSTRUCTION_VRSQRT14SS,
	NMD_X86_INSTRUCTION_VRSQRT28PD,
	NMD_X86_INSTRUCTION_VRSQRT28PS,
	NMD_X86_INSTRUCTION_VRSQRT28SD,
	NMD_X86_INSTRUCTION_VRSQRT28SS,
	NMD_X86_INSTRUCTION_VRSQRTPS,
	NMD_X86_INSTRUCTION_VRSQRTSS,
	NMD_X86_INSTRUCTION_VSCATTERDPD,
	NMD_X86_INSTRUCTION_VSCATTERDPS,
	NMD_X86_INSTRUCTION_VSCATTERPF0DPD,
	NMD_X86_INSTRUCTION_VSCATTERPF0DPS,
	NMD_X86_INSTRUCTION_VSCATTERPF0QPD,
	NMD_X86_INSTRUCTION_VSCATTERPF0QPS,
	NMD_X86_INSTRUCTION_VSCATTERPF1DPD,
	NMD_X86_INSTRUCTION_VSCATTERPF1DPS,
	NMD_X86_INSTRUCTION_VSCATTERPF1QPD,
	NMD_X86_INSTRUCTION_VSCATTERPF1QPS,
	NMD_X86_INSTRUCTION_VSCATTERQPD,
	NMD_X86_INSTRUCTION_VSCATTERQPS,
	NMD_X86_INSTRUCTION_VSHUFPD,
	NMD_X86_INSTRUCTION_VSHUFPS,
	NMD_X86_INSTRUCTION_VSQRTPD,
	NMD_X86_INSTRUCTION_VSQRTPS,
	NMD_X86_INSTRUCTION_VSQRTSD,
	NMD_X86_INSTRUCTION_VSQRTSS,
	NMD_X86_INSTRUCTION_VSTMXCSR,
	NMD_X86_INSTRUCTION_VSUBPD,
	NMD_X86_INSTRUCTION_VSUBPS,
	NMD_X86_INSTRUCTION_VSUBSD,
	NMD_X86_INSTRUCTION_VSUBSS,
	NMD_X86_INSTRUCTION_VTESTPD,
	NMD_X86_INSTRUCTION_VTESTPS,
	NMD_X86_INSTRUCTION_VUNPCKHPD,
	NMD_X86_INSTRUCTION_VUNPCKHPS,
	NMD_X86_INSTRUCTION_VUNPCKLPD,
	NMD_X86_INSTRUCTION_VUNPCKLPS,
	NMD_X86_INSTRUCTION_VZEROALL,
	NMD_X86_INSTRUCTION_VZEROUPPER,
	NMD_X86_INSTRUCTION_FWAIT,
	NMD_X86_INSTRUCTION_XABORT,
	NMD_X86_INSTRUCTION_XACQUIRE,
	NMD_X86_INSTRUCTION_XBEGIN,
	NMD_X86_INSTRUCTION_XCHG,
	NMD_X86_INSTRUCTION_XCRYPTCBC,
	NMD_X86_INSTRUCTION_XCRYPTCFB,
	NMD_X86_INSTRUCTION_XCRYPTCTR,
	NMD_X86_INSTRUCTION_XCRYPTECB,
	NMD_X86_INSTRUCTION_XCRYPTOFB,
	NMD_X86_INSTRUCTION_XRELEASE,
	NMD_X86_INSTRUCTION_XRSTOR64,
	NMD_X86_INSTRUCTION_XRSTORS,
	NMD_X86_INSTRUCTION_XRSTORS64,
	NMD_X86_INSTRUCTION_XSAVE64,
	NMD_X86_INSTRUCTION_XSAVEC,
	NMD_X86_INSTRUCTION_XSAVEC64,
	NMD_X86_INSTRUCTION_XSAVEOPT64,
	NMD_X86_INSTRUCTION_XSAVES,
	NMD_X86_INSTRUCTION_XSAVES64,
	NMD_X86_INSTRUCTION_XSHA1,
	NMD_X86_INSTRUCTION_XSHA256,
	NMD_X86_INSTRUCTION_XSTORE,
	NMD_X86_INSTRUCTION_FDISI8087_NOP,
	NMD_X86_INSTRUCTION_FENI8087_NOP,

	/* pseudo instructions */
	NMD_X86_INSTRUCTION_CMPSS,
	NMD_X86_INSTRUCTION_CMPEQSS,
	NMD_X86_INSTRUCTION_CMPLTSS,
	NMD_X86_INSTRUCTION_CMPLESS,
	NMD_X86_INSTRUCTION_CMPUNORDSS,
	NMD_X86_INSTRUCTION_CMPNEQSS,
	NMD_X86_INSTRUCTION_CMPNLTSS,
	NMD_X86_INSTRUCTION_CMPNLESS,
	NMD_X86_INSTRUCTION_CMPORDSS,

	NMD_X86_INSTRUCTION_CMPSD,
	NMD_X86_INSTRUCTION_CMPEQSD,
	NMD_X86_INSTRUCTION_CMPLTSD,
	NMD_X86_INSTRUCTION_CMPLESD,
	NMD_X86_INSTRUCTION_CMPUNORDSD,
	NMD_X86_INSTRUCTION_CMPNEQSD,
	NMD_X86_INSTRUCTION_CMPNLTSD,
	NMD_X86_INSTRUCTION_CMPNLESD,
	NMD_X86_INSTRUCTION_CMPORDSD,

	NMD_X86_INSTRUCTION_CMPPS,
	NMD_X86_INSTRUCTION_CMPEQPS,
	NMD_X86_INSTRUCTION_CMPLTPS,
	NMD_X86_INSTRUCTION_CMPLEPS,
	NMD_X86_INSTRUCTION_CMPUNORDPS,
	NMD_X86_INSTRUCTION_CMPNEQPS,
	NMD_X86_INSTRUCTION_CMPNLTPS,
	NMD_X86_INSTRUCTION_CMPNLEPS,
	NMD_X86_INSTRUCTION_CMPORDPS,

	NMD_X86_INSTRUCTION_CMPPD,
	NMD_X86_INSTRUCTION_CMPEQPD,
	NMD_X86_INSTRUCTION_CMPLTPD,
	NMD_X86_INSTRUCTION_CMPLEPD,
	NMD_X86_INSTRUCTION_CMPUNORDPD,
	NMD_X86_INSTRUCTION_CMPNEQPD,
	NMD_X86_INSTRUCTION_CMPNLTPD,
	NMD_X86_INSTRUCTION_CMPNLEPD,
	NMD_X86_INSTRUCTION_CMPORDPD,

	NMD_X86_INSTRUCTION_VCMPSS,
	NMD_X86_INSTRUCTION_VCMPEQSS,
	NMD_X86_INSTRUCTION_VCMPLTSS,
	NMD_X86_INSTRUCTION_VCMPLESS,
	NMD_X86_INSTRUCTION_VCMPUNORDSS,
	NMD_X86_INSTRUCTION_VCMPNEQSS,
	NMD_X86_INSTRUCTION_VCMPNLTSS,
	NMD_X86_INSTRUCTION_VCMPNLESS,
	NMD_X86_INSTRUCTION_VCMPORDSS,
	NMD_X86_INSTRUCTION_VCMPEQ_UQSS,
	NMD_X86_INSTRUCTION_VCMPNGESS,
	NMD_X86_INSTRUCTION_VCMPNGTSS,
	NMD_X86_INSTRUCTION_VCMPFALSESS,
	NMD_X86_INSTRUCTION_VCMPNEQ_OQSS,
	NMD_X86_INSTRUCTION_VCMPGESS,
	NMD_X86_INSTRUCTION_VCMPGTSS,
	NMD_X86_INSTRUCTION_VCMPTRUESS,
	NMD_X86_INSTRUCTION_VCMPEQ_OSSS,
	NMD_X86_INSTRUCTION_VCMPLT_OQSS,
	NMD_X86_INSTRUCTION_VCMPLE_OQSS,
	NMD_X86_INSTRUCTION_VCMPUNORD_SSS,
	NMD_X86_INSTRUCTION_VCMPNEQ_USSS,
	NMD_X86_INSTRUCTION_VCMPNLT_UQSS,
	NMD_X86_INSTRUCTION_VCMPNLE_UQSS,
	NMD_X86_INSTRUCTION_VCMPORD_SSS,
	NMD_X86_INSTRUCTION_VCMPEQ_USSS,
	NMD_X86_INSTRUCTION_VCMPNGE_UQSS,
	NMD_X86_INSTRUCTION_VCMPNGT_UQSS,
	NMD_X86_INSTRUCTION_VCMPFALSE_OSSS,
	NMD_X86_INSTRUCTION_VCMPNEQ_OSSS,
	NMD_X86_INSTRUCTION_VCMPGE_OQSS,
	NMD_X86_INSTRUCTION_VCMPGT_OQSS,
	NMD_X86_INSTRUCTION_VCMPTRUE_USSS,

	NMD_X86_INSTRUCTION_VCMPSD,
	NMD_X86_INSTRUCTION_VCMPEQSD,
	NMD_X86_INSTRUCTION_VCMPLTSD,
	NMD_X86_INSTRUCTION_VCMPLESD,
	NMD_X86_INSTRUCTION_VCMPUNORDSD,
	NMD_X86_INSTRUCTION_VCMPNEQSD,
	NMD_X86_INSTRUCTION_VCMPNLTSD,
	NMD_X86_INSTRUCTION_VCMPNLESD,
	NMD_X86_INSTRUCTION_VCMPORDSD,
	NMD_X86_INSTRUCTION_VCMPEQ_UQSD,
	NMD_X86_INSTRUCTION_VCMPNGESD,
	NMD_X86_INSTRUCTION_VCMPNGTSD,
	NMD_X86_INSTRUCTION_VCMPFALSESD,
	NMD_X86_INSTRUCTION_VCMPNEQ_OQSD,
	NMD_X86_INSTRUCTION_VCMPGESD,
	NMD_X86_INSTRUCTION_VCMPGTSD,
	NMD_X86_INSTRUCTION_VCMPTRUESD,
	NMD_X86_INSTRUCTION_VCMPEQ_OSSD,
	NMD_X86_INSTRUCTION_VCMPLT_OQSD,
	NMD_X86_INSTRUCTION_VCMPLE_OQSD,
	NMD_X86_INSTRUCTION_VCMPUNORD_SSD,
	NMD_X86_INSTRUCTION_VCMPNEQ_USSD,
	NMD_X86_INSTRUCTION_VCMPNLT_UQSD,
	NMD_X86_INSTRUCTION_VCMPNLE_UQSD,
	NMD_X86_INSTRUCTION_VCMPORD_SSD,
	NMD_X86_INSTRUCTION_VCMPEQ_USSD,
	NMD_X86_INSTRUCTION_VCMPNGE_UQSD,
	NMD_X86_INSTRUCTION_VCMPNGT_UQSD,
	NMD_X86_INSTRUCTION_VCMPFALSE_OSSD,
	NMD_X86_INSTRUCTION_VCMPNEQ_OSSD,
	NMD_X86_INSTRUCTION_VCMPGE_OQSD,
	NMD_X86_INSTRUCTION_VCMPGT_OQSD,
	NMD_X86_INSTRUCTION_VCMPTRUE_USSD,

	NMD_X86_INSTRUCTION_VCMPPS,
	NMD_X86_INSTRUCTION_VCMPEQPS,
	NMD_X86_INSTRUCTION_VCMPLTPS,
	NMD_X86_INSTRUCTION_VCMPLEPS,
	NMD_X86_INSTRUCTION_VCMPUNORDPS,
	NMD_X86_INSTRUCTION_VCMPNEQPS,
	NMD_X86_INSTRUCTION_VCMPNLTPS,
	NMD_X86_INSTRUCTION_VCMPNLEPS,
	NMD_X86_INSTRUCTION_VCMPORDPS,
	NMD_X86_INSTRUCTION_VCMPEQ_UQPS,
	NMD_X86_INSTRUCTION_VCMPNGEPS,
	NMD_X86_INSTRUCTION_VCMPNGTPS,
	NMD_X86_INSTRUCTION_VCMPFALSEPS,
	NMD_X86_INSTRUCTION_VCMPNEQ_OQPS,
	NMD_X86_INSTRUCTION_VCMPGEPS,
	NMD_X86_INSTRUCTION_VCMPGTPS,
	NMD_X86_INSTRUCTION_VCMPTRUEPS,
	NMD_X86_INSTRUCTION_VCMPEQ_OSPS,
	NMD_X86_INSTRUCTION_VCMPLT_OQPS,
	NMD_X86_INSTRUCTION_VCMPLE_OQPS,
	NMD_X86_INSTRUCTION_VCMPUNORD_SPS,
	NMD_X86_INSTRUCTION_VCMPNEQ_USPS,
	NMD_X86_INSTRUCTION_VCMPNLT_UQPS,
	NMD_X86_INSTRUCTION_VCMPNLE_UQPS,
	NMD_X86_INSTRUCTION_VCMPORD_SPS,
	NMD_X86_INSTRUCTION_VCMPEQ_USPS,
	NMD_X86_INSTRUCTION_VCMPNGE_UQPS,
	NMD_X86_INSTRUCTION_VCMPNGT_UQPS,
	NMD_X86_INSTRUCTION_VCMPFALSE_OSPS,
	NMD_X86_INSTRUCTION_VCMPNEQ_OSPS,
	NMD_X86_INSTRUCTION_VCMPGE_OQPS,
	NMD_X86_INSTRUCTION_VCMPGT_OQPS,
	NMD_X86_INSTRUCTION_VCMPTRUE_USPS,

	NMD_X86_INSTRUCTION_VCMPPD,
	NMD_X86_INSTRUCTION_VCMPEQPD,
	NMD_X86_INSTRUCTION_VCMPLTPD,
	NMD_X86_INSTRUCTION_VCMPLEPD,
	NMD_X86_INSTRUCTION_VCMPUNORDPD,
	NMD_X86_INSTRUCTION_VCMPNEQPD,
	NMD_X86_INSTRUCTION_VCMPNLTPD,
	NMD_X86_INSTRUCTION_VCMPNLEPD,
	NMD_X86_INSTRUCTION_VCMPORDPD,
	NMD_X86_INSTRUCTION_VCMPEQ_UQPD,
	NMD_X86_INSTRUCTION_VCMPNGEPD,
	NMD_X86_INSTRUCTION_VCMPNGTPD,
	NMD_X86_INSTRUCTION_VCMPFALSEPD,
	NMD_X86_INSTRUCTION_VCMPNEQ_OQPD,
	NMD_X86_INSTRUCTION_VCMPGEPD,
	NMD_X86_INSTRUCTION_VCMPGTPD,
	NMD_X86_INSTRUCTION_VCMPTRUEPD,
	NMD_X86_INSTRUCTION_VCMPEQ_OSPD,
	NMD_X86_INSTRUCTION_VCMPLT_OQPD,
	NMD_X86_INSTRUCTION_VCMPLE_OQPD,
	NMD_X86_INSTRUCTION_VCMPUNORD_SPD,
	NMD_X86_INSTRUCTION_VCMPNEQ_USPD,
	NMD_X86_INSTRUCTION_VCMPNLT_UQPD,
	NMD_X86_INSTRUCTION_VCMPNLE_UQPD,
	NMD_X86_INSTRUCTION_VCMPORD_SPD,
	NMD_X86_INSTRUCTION_VCMPEQ_USPD,
	NMD_X86_INSTRUCTION_VCMPNGE_UQPD,
	NMD_X86_INSTRUCTION_VCMPNGT_UQPD,
	NMD_X86_INSTRUCTION_VCMPFALSE_OSPD,
	NMD_X86_INSTRUCTION_VCMPNEQ_OSPD,
	NMD_X86_INSTRUCTION_VCMPGE_OQPD,
	NMD_X86_INSTRUCTION_VCMPGT_OQPD,
	NMD_X86_INSTRUCTION_VCMPTRUE_USPD,

	NMD_X86_INSTRUCTION_UD0,
	NMD_X86_INSTRUCTION_ENDBR32,
	NMD_X86_INSTRUCTION_ENDBR64,
};

enum NMD_X86_OPERAND_TYPE
{
	NMD_X86_OPERAND_TYPE_NONE = 0,
	NMD_X86_OPERAND_TYPE_REGISTER,
	NMD_X86_OPERAND_TYPE_MEMORY,
	NMD_X86_OPERAND_TYPE_IMMEDIATE,
};

typedef struct nmd_x86_memory_operand
{
	uint8_t segment;     /* The segment register. A member of 'NMD_X86_REG'. */
	uint8_t base;        /* The base register. A member of 'NMD_X86_REG'. */
	uint8_t index;       /* The index register. A member of 'NMD_X86_REG'. */
	uint8_t scale;       /* Scale(1, 2, 4 or 8). */
	int64_t disp;        /* Displacement. */
} nmd_x86_memory_operand;

enum NMD_X86_OPERAND_ACTION
{
	NMD_X86_OPERAND_ACTION_NONE = 0, /* The operand is neither read from nor written to. */

	NMD_X86_OPERAND_ACTION_READ              = (1 << 0), /* The operand is read. */
	NMD_X86_OPERAND_ACTION_WRITE             = (1 << 1), /* The operand is modified. */
	NMD_X86_OPERAND_ACTION_CONDITIONAL_READ  = (1 << 2), /* The operand may be read depending on some condition. */
	NMD_X86_OPERAND_ACTION_CONDITIONAL_WRITE = (1 << 3), /* The operand may be modified depending on some condition. */

	/* These are not actual actions, but rather masks of actions. */
	NMD_X86_OPERAND_ACTION_READ_WRITE = (NMD_X86_OPERAND_ACTION_READ | NMD_X86_OPERAND_ACTION_WRITE),
	NMD_X86_OPERAND_ACTION_ANY_READ   = (NMD_X86_OPERAND_ACTION_READ | NMD_X86_OPERAND_ACTION_CONDITIONAL_READ),
	NMD_X86_OPERAND_ACTION_ANY_WRITE  = (NMD_X86_OPERAND_ACTION_WRITE | NMD_X86_OPERAND_ACTION_CONDITIONAL_WRITE)
};

typedef struct nmd_x86_operand
{
	uint8_t type;                  /* The operand's type. A member of 'NMD_X86_OPERAND_TYPE'. */
	uint8_t size;                  /* The operand's size in bytes. */
	bool isImplicit;               /* If true, the operand does not appear on the intruction's formatted form. */
	uint8_t action;                /* The action on the operand. A member of 'NMD_X86_OPERAND_ACTION'. */
	union {                        /* The operand's "raw" data. */
		uint8_t reg;               /* Register operand. A variable of type 'NMD_X86_REG' */
		int64_t imm;               /* Immediate operand. */
		nmd_x86_memory_operand mem;  /* Memory operand. */
	} fields;
} nmd_x86_operand;

typedef union nmd_x86_cpu_flags
{
	struct
	{
		uint8_t CF   : 1; /* Bit  0.    Carry Flag (CF) */
		uint8_t b1   : 1; /* Bit  1.    Reserved */
		uint8_t PF   : 1; /* Bit  2.    Parity Flag (PF) */
		uint8_t B3   : 1; /* Bit  3.    Reserved */
		uint8_t AF   : 1; /* Bit  4.    Auxiliary Carry Flag (AF) */
		uint8_t B5   : 1; /* Bit  5.    Reserved */
		uint8_t ZF   : 1; /* Bit  6.    Zero flag(ZF) */
		uint8_t SF   : 1; /* Bit  7.    Sign flag(SF) */
		uint8_t TF   : 1; /* Bit  8.    Trap flag(TF) */
		uint8_t IF   : 1; /* Bit  9.    Interrupt Enable Flag (IF) */
		uint8_t DF   : 1; /* Bit 10.    Direction Flag (DF) */
		uint8_t OF   : 1; /* Bit 11.    Overflow Flag (OF) */
		uint8_t IOPL : 2; /* Bit 12,13. I/O Privilege Level (IOPL) */
		uint8_t NT   : 1; /* Bit 14.    Nested Task (NT) */
		uint8_t B15  : 1; /* Bit 15.    Reserved */
		uint8_t RF   : 1; /* Bit 16.    Resume Flag (RF) */
		uint8_t VM   : 1; /* Bit 17.    Virtual-8086 Mode (VM) */
		uint8_t AC   : 1; /* Bit 18.    Alignment Check / Access Control (AC) */
		uint8_t VIF  : 1; /* Bit 19.    Virtual Interrupt Flag (VIF) */
		uint8_t VIP  : 1; /* Bit 20.    Virtual Interrupt Pending (VIP) */
		uint8_t ID   : 1; /* Bit 21.    ID Flag(ID) */
		uint8_t B22  : 1; /* Bit 22.    Reserved */
		uint8_t B23  : 1; /* Bit 23.    Reserved */
		uint8_t B24  : 1; /* Bit 24.    Reserved */
		uint8_t B25  : 1; /* Bit 25.    Reserved */
		uint8_t B26  : 1; /* Bit 26.    Reserved */
		uint8_t B27  : 1; /* Bit 27.    Reserved */
		uint8_t B28  : 1; /* Bit 28.    Reserved */
		uint8_t B29  : 1; /* Bit 29.    Reserved */
		uint8_t B30  : 1; /* Bit 30.    Reserved */
		uint8_t B31  : 1; /* Bit 31.    Reserved */
	} fields;
	struct
	{
		uint8_t IE  : 1; /* Bit  0.    Invalid Operation (IE) */
		uint8_t DE  : 1; /* Bit  1.    Denormalized Operand (DE) */
		uint8_t ZE  : 1; /* Bit  2.    Zero Divide (ZE) */
		uint8_t OE  : 1; /* Bit  3.    Overflow (OE) */
		uint8_t UE  : 1; /* Bit  4.    Underflow (UE) */
		uint8_t PE  : 1; /* Bit  5.    Precision (PE) */
		uint8_t SF  : 1; /* Bit  6.    Stack Fault (SF) */
		uint8_t ES  : 1; /* Bit  7.    Exception Summary Status (ES) */
		uint8_t C0  : 1; /* Bit  8.    Condition code 0 (C0) */
		uint8_t C1  : 1; /* Bit  9.    Condition code 1 (C1) */
		uint8_t C2  : 1; /* Bit 10.    Condition code 2 (C2) */
		uint8_t TOP : 3; /* Bit 11-13. Top of Stack Pointer (TOP) */
		uint8_t C3  : 1; /* Bit 14.    Condition code 3 (C3) */
		uint8_t B   : 1; /* Bit 15.    FPU Busy (B) */
	} fpuFields;
	uint8_t l8;
	uint32_t eflags;
	uint16_t fpuFlags;
} nmd_x86_cpu_flags;

enum NMD_X86_EFLAGS
{
	NMD_X86_EFLAGS_ID   = (1 << 21),
	NMD_X86_EFLAGS_VIP  = (1 << 20),
	NMD_X86_EFLAGS_VIF  = (1 << 19),
	NMD_X86_EFLAGS_AC   = (1 << 18),
	NMD_X86_EFLAGS_VM   = (1 << 17),
	NMD_X86_EFLAGS_RF   = (1 << 16),
	NMD_X86_EFLAGS_NT   = (1 << 14),
	NMD_X86_EFLAGS_IOPL = (1 << 12) | (1 << 13),
	NMD_X86_EFLAGS_OF   = (1 << 11),
	NMD_X86_EFLAGS_DF   = (1 << 10),
	NMD_X86_EFLAGS_IF   = (1 << 9),
	NMD_X86_EFLAGS_TF   = (1 << 8),
	NMD_X86_EFLAGS_SF   = (1 << 7),
	NMD_X86_EFLAGS_ZF   = (1 << 6),
	NMD_X86_EFLAGS_AF   = (1 << 4),
	NMD_X86_EFLAGS_PF   = (1 << 2),
	NMD_X86_EFLAGS_CF   = (1 << 0)
};

enum NMD_X86_FPU_FLAGS
{
	NMD_X86_FPU_FLAGS_C0 = (1 << 8),
	NMD_X86_FPU_FLAGS_C1 = (1 << 9),
	NMD_X86_FPU_FLAGS_C2 = (1 << 10),
	NMD_X86_FPU_FLAGS_C3 = (1 << 14)
};

typedef struct nmd_x86_instruction
{
	bool valid : 1;                                        /* If true, the instruction is valid. */
	bool hasModrm : 1;                                     /* If true, the instruction has a modrm byte. */
	bool hasSIB : 1;                                       /* If true, the instruction has an SIB byte. */
	bool hasRex : 1;                                       /* If true, the instruction has a REX prefix */
	bool operandSize64 : 1;                                /* If true, a REX.W prefix is closer to the opcode than a operand size override prefix. */
	bool repeatPrefix : 1;                                 /* If true, a 'repeat'(F3h) prefix is closer to the opcode than a 'repeat not zero'(F2h) prefix. */
	uint8_t mode;                                          /* The decoding mode. A member of 'NMD_X86_MODE'. */
	uint8_t length;                                        /* The instruction's length in bytes. */
	uint8_t opcode;                                        /* Opcode byte. */
	uint8_t opcodeSize;                                    /* The opcode's size in bytes. */
	uint16_t id;                                           /* The instruction's identifier. A member of 'NMD_X86_INSTRUCTION'. */
	uint16_t prefixes;                                     /* A mask of prefixes. See 'NMD_X86_PREFIXES'. */
	uint8_t numPrefixes;                                   /* Number of prefixes. */
	uint8_t numOperands;                                   /* The number of operands. */
	uint8_t group;                                         /* The instruction's group(e.g. jmp, prvileged...). A member of 'NMD_X86_GROUP'. */
	uint8_t buffer[NMD_X86_MAXIMUM_INSTRUCTION_LENGTH];    /* A buffer containing the full instruction. */
	nmd_x86_operand operands[NMD_X86_MAXIMUM_NUM_OPERANDS]; /* Operands. */
	nmd_x86_modrm modrm;                                       /* The Mod/RM byte. Check 'flags.fields.hasModrm'. */
	nmd_x86_sib sib;                                           /* The SIB byte. Check 'flags.fields.hasSIB'. */
	uint8_t immMask;                                       /* A mask of one or more members of 'NMD_X86_IMM'. */
	uint8_t dispMask;                                      /* A mask of one or more members of 'NMD_X86_DISP'. */
	uint64_t immediate;                                    /* Immediate. Check 'immMask'. */
	uint32_t displacement;                                 /* Displacement. Check 'dispMask'. */
	uint8_t opcodeMap;                                     /* The instruction's opcode map. A member of 'NMD_X86_OPCODE_MAP'. */
	uint8_t encoding;                                      /* The instruction's encoding. A member of 'NMD_X86_INSTRUCTION_ENCODING'. */
	nmd_x86_vex vex;                                        /* VEX prefix. */
	nmd_x86_cpu_flags modifiedFlags;                         /* Cpu flags modified by the instruction. */
	nmd_x86_cpu_flags testedFlags;                           /* Cpu flags tested by the instruction. */
	nmd_x86_cpu_flags setFlags;                              /* Cpu flags set by the instruction. */
	nmd_x86_cpu_flags clearedFlags;                          /* Cpu flags cleared by the instruction. */
	nmd_x86_cpu_flags undefinedFlags;                        /* Cpu flags whose state is undefined. */
	uint8_t rex;                                           /* REX prefix. */
	uint8_t segmentOverride;                               /* The segment override prefix closest to the opcode. A member of 'NMD_X86_PREFIXES'. */
	uint16_t simdPrefix;                                   /* Either one of these prefixes that is the closest to the opcode: NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE, NMD_X86_PREFIXES_LOCK, NMD_X86_PREFIXES_REPEAT_NOT_ZERO, NMD_X86_PREFIXES_REPEAT, or NMD_X86_PREFIXES_NONE. The prefixes are specified as members of the 'NMD_X86_PREFIXES' enum. */
} nmd_x86_instruction;

typedef enum NMD_X86_EMULATOR_EXCEPTION
{
	NMD_X86_EMULATOR_EXCEPTION_NONE = 0,
	NMD_X86_EMULATOR_EXCEPTION_BREAKPOINT, /* #BP generated by int3 */
	NMD_X86_EMULATOR_EXCEPTION_DEBUG, /* #DB generated by int1 */
	NMD_X86_EMULATOR_EXCEPTION_OVERFLOW, /* #OF generated by into */
	NMD_X86_EMULATOR_EXCEPTION_GENERAL_PROTECTION,
	NMD_X86_EMULATOR_EXCEPTION_BAD_INSTRUCTION,
	NMD_X86_EMULATOR_EXCEPTION_BAD_MEMORY,
	NMD_X86_EMULATOR_EXCEPTION_STEP
} NMD_X86_EMULATOR_EXCEPTION;

typedef union nmd_x86_register
{
	int8_t  h8;
	int8_t  l8;
	int16_t l16;
	int32_t l32;
	int64_t l64;
} nmd_x86_register;

typedef union nmd_x86_register_512
{
	uint64_t xmm0[2];
	uint64_t ymm0[4];
	uint64_t zmm0[8];
} nmd_x86_register_512;

typedef struct nmd_x86_cpu
{
	bool running; /* If true, the emulator is running, false otherwise. */

	uint8_t mode; /* The emulator's architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'. */

	void* physicalMemory; /* A pointer to a buffer used as the emulator's memory. */
	size_t physicalMemorySize; /* The size of the buffer pointer by 'physicalMemory' in bytes. */

	uint64_t virtualAddress; /* The starting address of the emulator's virtual address space. This address can be any value. */

	void (*callback)(struct nmd_x86_cpu* cpu, const nmd_x86_instruction* instruction, NMD_X86_EMULATOR_EXCEPTION exception);

	void* userdata;

	size_t count; /* Internal counter used by the emulator.*/

	uint64_t rip; /* The address of the next instruction to be executed(emulated). */

	nmd_x86_cpu_flags flags;

	nmd_x86_register rax;
	nmd_x86_register rcx;
	nmd_x86_register rdx;
	nmd_x86_register rbx;
	nmd_x86_register rsp;
	nmd_x86_register rbp;
	nmd_x86_register rsi;
	nmd_x86_register rdi;

	nmd_x86_register r8;
	nmd_x86_register r9;
	nmd_x86_register r10;
	nmd_x86_register r11;
	nmd_x86_register r12;
	nmd_x86_register r13;
	nmd_x86_register r14;
	nmd_x86_register r15;

	uint16_t es;
	uint16_t ss;
	uint16_t cs;
	uint16_t ds;
	uint16_t fs;
	uint16_t gs;

	nmd_x86_register mm0;
	nmd_x86_register mm1;
	nmd_x86_register mm2;
	nmd_x86_register mm3;
	nmd_x86_register mm4;
	nmd_x86_register mm5;
	nmd_x86_register mm6;
	nmd_x86_register mm7;

	nmd_x86_register_512 zmm0;
	nmd_x86_register_512 zmm1;
	nmd_x86_register_512 zmm2;
	nmd_x86_register_512 zmm3;
	nmd_x86_register_512 zmm4;
	nmd_x86_register_512 zmm5;
	nmd_x86_register_512 zmm6;
	nmd_x86_register_512 zmm7;
	nmd_x86_register_512 zmm8;
	nmd_x86_register_512 zmm9;
	nmd_x86_register_512 zmm10;
	nmd_x86_register_512 zmm11;
	nmd_x86_register_512 zmm12;
	nmd_x86_register_512 zmm13;
	nmd_x86_register_512 zmm14;
	nmd_x86_register_512 zmm15;
	nmd_x86_register_512 zmm16;
	nmd_x86_register_512 zmm17;
	nmd_x86_register_512 zmm18;
	nmd_x86_register_512 zmm19;
	nmd_x86_register_512 zmm20;
	nmd_x86_register_512 zmm21;
	nmd_x86_register_512 zmm22;
	nmd_x86_register_512 zmm23;
	nmd_x86_register_512 zmm24;
	nmd_x86_register_512 zmm25;
	nmd_x86_register_512 zmm26;
	nmd_x86_register_512 zmm27;
	nmd_x86_register_512 zmm28;
	nmd_x86_register_512 zmm29;
	nmd_x86_register_512 zmm30;
	nmd_x86_register_512 zmm31;

	nmd_x86_register dr0;
	nmd_x86_register dr1;
	nmd_x86_register dr2;
	nmd_x86_register dr3;
	nmd_x86_register dr4;
	nmd_x86_register dr5;
	nmd_x86_register dr6;
	nmd_x86_register dr7;
} nmd_x86_cpu;

/*
Assembles an instruction from a string. Returns the number of bytes written to the buffer on success, zero otherwise. Instructions can be separated using either the ';' or '\n' character.
Parameters:
 - string         [in]         A pointer to a string that represents one or more instructions in assembly language.
 - buffer         [out]        A pointer to a buffer that receives the encoded instructions.
 - bufferSize     [in]         The size of the buffer in bytes.
 - runtimeAddress [in]         The instruction's runtime address. You may use 'NMD_X86_INVALID_RUNTIME_ADDRESS'.
 - mode           [in]         The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
 - count          [in/out/opt] A pointer to a variable that on input is the maximum number of instructions that can be parsed(or zero for unlimited instructions), and on output is the number of instructions parsed. This parameter may be zero.
*/
size_t nmd_x86_assemble(const char* string, void* buffer, size_t bufferSize, uint64_t runtimeAddress, NMD_X86_MODE mode, size_t* count);

/*
Decodes an instruction. Returns true if the instruction is valid, false otherwise.
Parameters:
 - buffer      [in]  A pointer to a buffer containing a encoded instruction.
 - bufferSize  [in]  The buffer's size in bytes.
 - instruction [out] A pointer to a variable of type 'nmd_x86_instruction' that receives information about the instruction.
 - mode        [in]  The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
 - flags       [in]  A mask of 'NMD_X86_DECODER_FLAGS_XXX' that specifies which features the decoder is allowed to use. If uncertain, use 'NMD_X86_DECODER_FLAGS_MINIMAL'.
*/
bool nmd_x86_decode_buffer(const void* buffer, size_t bufferSize, nmd_x86_instruction* instruction, NMD_X86_MODE mode, uint32_t flags);

/*
Formats an instruction. This function may cause a crash if you modify 'instruction' manually.
Parameters:
 - instruction    [in]  A pointer to a variable of type 'nmd_x86_instruction' describing the instruction to be formatted.
 - buffer         [out] A pointer to buffer that receives the string. The buffer's recommended size is 128 bytes.
 - runtimeAddress [in]  The instruction's runtime address. You may use 'NMD_X86_INVALID_RUNTIME_ADDRESS'.
 - formatFlags    [in]  A mask of 'NMD_X86_FORMAT_FLAGS_XXX' that specifies how the function should format the instruction. If uncertain, use 'NMD_X86_FORMAT_FLAGS_DEFAULT'.
*/
void nmd_x86_format_instruction(const nmd_x86_instruction* instruction, char* buffer, uint64_t runtimeAddress, uint32_t formatFlags);

/*
Emulates x86 code according to the cpu's state. You MUST initialize the following variables before calling this
function: 'cpu->mode', 'cpu->physicalMemory', 'cpu->physicalMemorySize', 'cpu->virtualAddress' and 'cpu->rip'.
You may optionally initialize 'cpu->rsp' if a stack is desirable. Below is a short description of each variable:
 - 'cpu->mode': The emulator's operating architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
 - 'cpu->physicalMemory': A pointer to a buffer used as the emulator's memory.
 - 'cpu->physicalMemorySize': The size of the buffer pointer by 'physicalMemory' in bytes.
 - 'cpu->virtualAddress': The starting address of the emulator's virtual address space.
 - 'cpu->rip': The virtual address where emulation starts.
 - 'cpu->rsp': The virtual address of the bottom of the stack.
Parameters:
 - cpu      [in] A pointer to a variable of type 'nmd_x86_cpu' that holds the state of the cpu.
 - maxCount [in] The maximum number of instructions that can be executed, or zero for unlimited instructions.
*/
bool nmd_x86_emulate(nmd_x86_cpu* cpu, size_t maxCount);

/*
Returns the instruction's length if it's valid, zero otherwise.
Parameters:
 - buffer     [in] A pointer to a buffer containing a encoded instruction.
 - bufferSize [in] The buffer's size in bytes.
 - mode       [in] The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
*/
size_t nmd_x86_ldisasm(const void* buffer, size_t bufferSize, NMD_X86_MODE mode);

#endif /* NMD_ASSEMBLY_H */


#ifdef NMD_ASSEMBLY_IMPLEMENTATION

/* Four high-order bits of an opcode to index a row of the opcode table */
#define NMD_R(b) ((b) >> 4)

/* Four low-order bits to index a column of the table */
#define NMD_C(b) ((b) & 0xF)

#define _NMD_NUM_ELEMENTS(arr) (sizeof(arr) / sizeof((arr)[0]))

#define _NMD_IS_UPPERCASE(c) (c >= 'A' && c <= 'Z')
#define _NMD_IS_LOWERCASE(c) (c >= 'a' && c <= 'z')
#define _NMD_TOLOWER(c) (_NMD_IS_UPPERCASE(c) ? c + 0x20 : c)
#define _NMD_IS_DECIMAL_NUMBER(c) (c >= '0' && c <= '9')

const char* const _nmd_reg8[] = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh" };
const char* const _nmd_reg8_x64[] = { "al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil" };
const char* const _nmd_reg16[] = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di" };
const char* const _nmd_reg32[] = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi" };
const char* const _nmd_reg64[] = { "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi" };
const char* const _nmd_regrx[] = { "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" };
const char* const _nmd_regrxd[] = { "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d" };
const char* const _nmd_regrxw[] = { "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w" };
const char* const _nmd_regrxb[] = { "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b" };
const char* const _nmd_segmentReg[] = { "es", "cs", "ss", "ds", "fs", "gs" };

const char* const _nmd_conditionSuffixes[] = { "o", "no", "b", "ae", "e", "ne", "be", "a", "s", "ns", "p", "np", "l", "ge", "le", "g" };

const char* const _nmd_op1OpcodeMapMnemonics[] = { "add", "adc", "and", "xor", "or", "sbb", "sub", "cmp" };
const char* const _nmd_opcodeExtensionsGrp1[] = { "add", "or", "adc", "sbb", "and", "sub", "xor", "cmp" };
const char* const _nmd_opcodeExtensionsGrp2[] = { "rol", "ror", "rcl", "rcr", "shl", "shr", "shl", "sar" };
const char* const _nmd_opcodeExtensionsGrp3[] = { "test", "test", "not", "neg", "mul", "imul", "div", "idiv" };
const char* const _nmd_opcodeExtensionsGrp5[] = { "inc", "dec", "call", "call far", "jmp", "jmp far", "push" };
const char* const _nmd_opcodeExtensionsGrp6[] = { "sldt", "str", "lldt", "ltr", "verr", "verw" };
const char* const _nmd_opcodeExtensionsGrp7[] = { "sgdt", "sidt", "lgdt", "lidt", "smsw", 0, "lmsw", "invlpg" };
const char* const _nmd_opcodeExtensionsGrp7reg0[] = { "enclv", "vmcall", "vmlaunch", "vmresume", "vmxoff", "pconfig" };
const char* const _nmd_opcodeExtensionsGrp7reg1[] = { "monitor", "mwait", "clac", "stac", 0, 0, 0, "encls" };
const char* const _nmd_opcodeExtensionsGrp7reg2[] = { "xgetbv", "xsetbv", 0, 0, "vmfunc", "xend", "xtest", "enclu" };
const char* const _nmd_opcodeExtensionsGrp7reg3[] = { "vmrun ", "vmmcall", "vmload ", "vmsave", "stgi", "clgi", "skinit eax", "invlpga " };
const char* const _nmd_opcodeExtensionsGrp7reg7[] = { "swapgs", "rdtscp", "monitorx", "mwaitx", "clzero ", "rdpru" };

const char* const _nmd_escapeOpcodesD8[] = { "add", "mul", "com", "comp", "sub", "subr", "div", "divr" };
const char* const _nmd_escapeOpcodesD9[] = { "ld", 0, "st", "stp", "ldenv", "ldcw", "nstenv", "nstcw" };
const char* const _nmd_escapeOpcodesDA_DE[] = { "iadd", "imul", "icom", "icomp", "isub", "isubr", "idiv", "idivr" };
const char* const _nmd_escapeOpcodesDB[] = { "ild", "isttp", "ist", "istp", 0, "ld", 0, "stp" };
const char* const _nmd_escapeOpcodesDC[] = { "add", "mul", "com", "comp", "sub", "subr", "div", "divr" };
const char* const _nmd_escapeOpcodesDD[] = { "ld", "isttp", "st", "stp", "rstor", 0, "nsave", "nstsw" };
const char* const _nmd_escapeOpcodesDF[] = { "ild", "isttp", "ist", "istp", "bld", "ild", "bstp", "istp" };
const char* const* _nmd_escapeOpcodes[] = { _nmd_escapeOpcodesD8, _nmd_escapeOpcodesD9, _nmd_escapeOpcodesDA_DE, _nmd_escapeOpcodesDB, _nmd_escapeOpcodesDC, _nmd_escapeOpcodesDD, _nmd_escapeOpcodesDA_DE, _nmd_escapeOpcodesDF };

const uint8_t _nmd_op1modrm[] = { 0xFF, 0x63, 0x69, 0x6B, 0xC0, 0xC1, 0xC6, 0xC7, 0xD0, 0xD1, 0xD2, 0xD3, 0xF6, 0xF7, 0xFE };
const uint8_t _nmd_op1imm8[] = { 0x6A, 0x6B, 0x80, 0x82, 0x83, 0xA8, 0xC0, 0xC1, 0xC6, 0xCD, 0xD4, 0xD5, 0xEB };
const uint8_t _nmd_op1imm32[] = { 0xE8, 0xE9, 0x68, 0x81, 0x69, 0xA9, 0xC7 };
const uint8_t _nmd_invalid2op[] = { 0x04, 0x0a, 0x0c, 0x7a, 0x7b, 0x36, 0x39 };
const uint8_t _nmd_twoOpcodes[] = { 0xb0, 0xb1, 0xb3, 0xbb, 0xc0, 0xc1 };
const uint8_t _nmd_valid3DNowOpcodes[] = { 0x0c, 0x0d, 0x1c, 0x1d, 0x8a, 0x8e, 0x90, 0x94, 0x96, 0x97, 0x9a, 0x9e, 0xa0, 0xa4, 0xa6, 0xa7, 0xaa, 0xae, 0xb0, 0xb4, 0xb6, 0xb7, 0xbb, 0xbf };

bool _nmd_findByte(const uint8_t* arr, const size_t N, const uint8_t x)
{
	size_t i = 0;
	for (; i < N; i++)
	{
		if (arr[i] == x)
			return true;
	}; 
	
	return false;
}

/* Returns a pointer to the first occurrence of 'c' in 's', or a null pointer if 'c' is not present. */
const char* _nmd_strchr(const char* s, char c)
{
	for (; *s; s++)
	{
		if (*s == c)
			return s;
	}

	return 0;
}

/* Returns a pointer to the last occurrence of 'c' in 's', or a null pointer if 'c' is not present. */
const char* _nmd_reverse_strchr(const char* s, char c)
{
	const char* end = s;
	while (*end)
		end++;

	for (; end > s; end--)
	{
		if (*end == c)
			return end;
	}

	return 0;
}

/* Returns a pointer to the first occurrence of 's2' in 's', or a null pointer if 's2' is not present. */
const char* _nmd_strstr(const char* s, const char* s2)
{
	size_t i = 0;
	for (; *s; s++)
	{
		if (s2[i] == '\0')
			return s - i;

		if (*s != s2[i])
			i = 0;

		if (*s == s2[i])
			i++;
	}

	return 0;
}

/* Returns a pointer to the first occurrence of 's2' in 's', or a null pointer if 's2' is not present. If 's3_opt' is not null it receives the address of the next byte in 's'. */
const char* _nmd_strstr_ex(const char* s, const char* s2, const char** s3_opt)
{
	size_t i = 0;
	for (; *s; s++)
	{
		if (s2[i] == '\0')
		{
			if (s3_opt)
				*s3_opt = s;
			return s - i;
		}

		if (*s != s2[i])
			i = 0;

		if (*s == s2[i])
			i++;
	}

	return 0;
}

/* Inserts 'c' at 's'. */
void _nmd_insert_char(const char* s, char c)
{
	char* end = (char*)s;
	while (*end)
		end++;

	*(end + 1) = '\0';

	for (; end > s; end--)
		*end = *(end - 1);

	*end = c;
}

/* Returns true if there is only a number between 's1' and 's2', false otherwise. */
bool _nmd_is_number(const char* s1, const char* s2)
{
	const char* s = s1;
	for (; s < s2; s++)
	{
		if (!(*s >= '0' && *s <= '9') && !(*s >= 'a' && *s <= 'f') && !(*s >= 'A' && *s <= 'F'))
		{
			if ((s == s1 + 1 && *s1 == '0' && (*s == 'x' || *s == 'X')) || (s == s2 - 1 && (*s == 'h' || *s == 'H')))
				continue;

			return false;
		}
	}

	return true;
}

/* Returns a pointer to the first occurence of a number between 's1' and 's2', zero otherwise. */
const char* _nmd_find_number(const char* s1, const char* s2)
{
	const char* s = s1;
	for (; s < s2; s++)
	{
		if ((*s >= '0' && *s <= '9') || (*s >= 'a' && *s <= 'f') || (*s >= 'A' && *s <= 'F'))
			return s;
	}

	return 0;
}

/* Returns true if s1 matches s2 exactly. */
bool _nmd_strcmp(const char* s1, const char* s2)
{
	for (; *s1 && *s2; s1++, s2++)
	{
		if (*s1 != *s2)
			return false;
	}

	return !*s1 && !*s2;
}

size_t _nmd_get_bit_index(uint32_t mask)
{
	size_t i = 0;
	while (!(mask & (1 << i)))
		i++;

	return i;
}

typedef struct _nmd_assemble_info
{
	const char* s; /* string */
	uint8_t* b; /* buffer */
	NMD_X86_MODE mode;
	uint64_t runtimeAddress;
} _nmd_assemble_info;

enum _NMD_NUMBER_BASE
{
	_NMD_NUMBER_BASE_NONE = 0,
	_NMD_NUMBER_BASE_DECIMAL = 10,
	_NMD_NUMBER_BASE_HEXADECIMAL = 16,
	_NMD_NUMBER_BASE_BINARY = 2
};

size_t _nmd_assemble_reg(_nmd_assemble_info* ai, uint8_t baseByte)
{
	uint8_t i = 0;
	if (ai->mode == NMD_X86_MODE_64)
	{
		for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg64); i++)
		{
			if (_nmd_strcmp(ai->s, _nmd_reg64[i]))
			{
				ai->b[0] = baseByte + i;
				return 1;
			}
		}

		for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_regrx); i++)
		{
			if (_nmd_strcmp(ai->s, _nmd_regrx[i]))
			{
				ai->b[0] = 0x41;
				ai->b[1] = baseByte + i;
				return 2;
			}
		}
	}
	else if (ai->mode == NMD_X86_MODE_32)
	{
		for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg32); i++)
		{
			if (_nmd_strcmp(ai->s, _nmd_reg32[i]))
			{
				ai->b[0] = baseByte + i;
				return 1;
			}
		}
	}	

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg16); i++)
	{
		if (_nmd_strcmp(ai->s, _nmd_reg16[i]))
		{
			ai->b[0] = 0x66;
			ai->b[1] = baseByte + i;
			return 2;
		}
	}

	return 0;
}

uint8_t _nmd_encode_segment_reg(NMD_X86_REG segmentReg)
{
	switch (segmentReg)
	{
	case NMD_X86_REG_ES: return 0x26;
	case NMD_X86_REG_CS: return 0x2e;
	case NMD_X86_REG_SS: return 0x36;
	case NMD_X86_REG_DS: return 0x3e;
	case NMD_X86_REG_FS: return 0x64;
	case NMD_X86_REG_GS: return 0x65;
	default: return 0;
	}
}

bool _nmd_parse_number(const char* string, int64_t* pNum, size_t* pNumDigits)
{
	if (*string == '\0')
		return false;

	/* Assume decimal base. */
	uint8_t base = _NMD_NUMBER_BASE_DECIMAL;
	size_t i;
	const char* s = string;
	bool isNegative = false;

	if (s[0] == '-')
	{
		isNegative = true;
		s++;
	}

	if (s[0] == '0')
	{
		if (s[1] == 'x')
		{
			s += 2;
			base = _NMD_NUMBER_BASE_HEXADECIMAL;
		}
		else if (s[1] == 'b')
		{
			s += 2;
			base = _NMD_NUMBER_BASE_BINARY;
		}
	}

	for (i = 0; s[i]; i++)
	{
		const char c = s[i];

		if (base == _NMD_NUMBER_BASE_DECIMAL)
		{
			if (c >= 'a' && c <= 'f')
			{
				base = _NMD_NUMBER_BASE_HEXADECIMAL;
				continue;
			}
			else if(!(c >= '0' && c <= '9'))
				break;
		}
		else if (base == _NMD_NUMBER_BASE_HEXADECIMAL)
		{
			if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
				break;
		}
		else if (c != '0' && c != '1') /* _NMD_NUMBER_BASE_BINARY */
			break;
	}

	size_t numDigits = i;

	int64_t num = 0;
	for (i = 0; i < numDigits; i++)
	{
		const char c = s[i];
		num += (c <= '9') ? (c - '0') : (10 + c - 'a');
		if (i < numDigits - 1)
		{
			/* Return false if number is greater than 2^64-1 */
			if ( numDigits > 16 && i >= 15)
			{
				if ((base == _NMD_NUMBER_BASE_DECIMAL && (uint64_t)num >= (uint64_t)1844674407370955162) || /* ceiling((2^64-1) / 10) */
					(base == _NMD_NUMBER_BASE_HEXADECIMAL && (uint64_t)num >= (uint64_t)1152921504606846976) || /* *ceiling((2^64-1) / 16) */
					(base == _NMD_NUMBER_BASE_BINARY && (uint64_t)num >= (uint64_t)9223372036854775808)) /* ceiling((2^64-1) / 2) */
				{
					return false;
				}

			}
			num *= base;
		}
	}

	if (s != string) /* There's either a "0x" or "0b" prefix. */
		numDigits = (size_t)((ptrdiff_t)(s + i) - (ptrdiff_t)string);
	
	if (isNegative)
		num *= -1;

	*pNum = num;

	if(pNumDigits)
		*pNumDigits = numDigits;

	return true;
}

size_t _nmd_append_prefix_by_reg_size(uint8_t* b, const char* s, size_t* numPrefixes, size_t* index)
{
	size_t i;
	
	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg32); i++)
	{
		if (_nmd_strcmp(s, _nmd_reg32[i]))
		{
			*numPrefixes = 0;
			*index = i;
			return 4;
		}
	}

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg8); i++)
	{
		if (_nmd_strcmp(s, _nmd_reg8[i]))
		{
			*numPrefixes = 0;
			*index = i;
			return 1;
		}
	}

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg16); i++)
	{
		if (_nmd_strcmp(s, _nmd_reg16[i]))
		{
			b[0] = 0x66;
			*numPrefixes = 1;
			*index = i;
			return 2;
		}
	}	

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg64); i++)
	{
		if (_nmd_strcmp(s, _nmd_reg64[i]))
		{
			b[0] = 0x48;
			*numPrefixes = 1;
			*index = i;
			return 8;
		}
	}

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_regrx); i++)
	{
		if (_nmd_strcmp(s, _nmd_regrx[i]))
		{
			b[0] = 0x49;
			*numPrefixes = 1;
			*index = i;
			return 8;
		}
	}

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_regrxd); i++)
	{
		if (_nmd_strcmp(s, _nmd_regrxd[i]))
		{
			b[0] = 0x41;
			*numPrefixes = 1;
			*index = i;
			return 4;
		}
	}

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_regrxw); i++)
	{
		if (_nmd_strcmp(s, _nmd_regrxw[i]))
		{
			b[0] = 0x66;
			b[1] = 0x41;
			*numPrefixes = 2;
			*index = i;
			return 2;
		}
	}

	for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_regrxb); i++)
	{
		if (_nmd_strcmp(s, _nmd_regrxb[i]))
		{
			b[0] = 0x41;
			*numPrefixes = 1;
			*index = i;
			return 1;
		}
	}

	return 0;
}

/* 
Parses a memory operand in the format: '[exp]'
string: a pointer to the string that represents the memory operand. The string is modified to point to the character after the memory operand.
operand[out]: Describes the memory operand.
size[out]: The size of the pointer. byte ptr:1, dword ptr:4, etc.. or zero if unknown.
Return value: True if success, false otherwise.
*/
bool _nmd_parse_memory_operand(const char** string, nmd_x86_memory_operand* operand, size_t* size)
{
	const char* s = *string;
	size_t numBytes = 0;
	if (_nmd_strstr(s, "byte") == s)
		numBytes = 1;
	else if (_nmd_strstr(s, "word") == s)
		numBytes = 2;
	else if (_nmd_strstr(s, "dword") == s)
		numBytes = 4;
	else if (_nmd_strstr(s, "qword") == s)
		numBytes = 8;
	else
		numBytes = 0;
	*size = numBytes;

	if (numBytes > 0)
	{
		s += numBytes >= 4 ? 5 : 4;

		if (s[0] == ' ' && s[1] == 'p' && s[2] == 't' && s[3] == 'r')
			s += 4;

		if (s[0] == ' ')
			s++;
		else if (s[0] != '[')
			return false;
	}

	size_t i = 0;
	operand->segment = NMD_X86_REG_NONE;
	for (; i < _NMD_NUM_ELEMENTS(_nmd_segmentReg); i++)
	{
		if (_nmd_strstr(s, _nmd_segmentReg[i]) == s)
		{
			if (s[2] != ':')
				return false;

			s += 3;
			operand->segment = (uint8_t)(NMD_X86_REG_ES + i);
			break;
		}
	}

	if (s[0] == '[')
		s++;
	else
		return false;
	
	operand->base = 0;
	operand->index = 0;
	operand->scale = 0;
	operand->disp = 0;
	bool add = false;
	bool sub = false;
	bool multiply = false;
	bool isRegister = false;
	while (true)
	{
		bool parsedElement = false;
		if (!sub && !multiply)
		{
			for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_reg32); i++)
			{
				const char* tmp;
				if (_nmd_strstr_ex(s, _nmd_reg32[i], &tmp) == s)
				{
					s = tmp;
					if (add)
					{
						operand->index = (uint8_t)(NMD_X86_REG_EAX + i);
						operand->scale = 1;
						add = false;
					}
					else
						operand->base = (uint8_t)(NMD_X86_REG_EAX + i);
					parsedElement = true;
					isRegister = true;
					break;
				}
			}
		}

		int64_t num;
		size_t numDigits;
		if (!parsedElement && _nmd_parse_number(s, &num, &numDigits))
		{
			s += numDigits;

			if (add)
			{
				operand->disp += num;
				add = false;
			}
			else if (sub)
			{
				operand->disp -= num;
				sub = false;
			}
			else if (multiply)
			{
				if (!isRegister || (num != 1 && num != 2 && num != 4 && num != 8))
					return false;

				operand->scale = (uint8_t)num;
			}
			else
				operand->disp = num;

			parsedElement = true;
		}

		if (!parsedElement)
			return false;

		if (s[0] == '+')
			add = true;
		else if (s[0] == '-')
			sub = true;
		else if (s[0] == '*')
		{
			/* There cannot be more than one '*' operator. */
			if (multiply)
				return false;

			multiply = true;
		}
		else if (s[0] == ']')
		{
			break;
		}
		else
			return false;

		s++;
	}

	*string = s + 1;
	return true;
}

size_t _nmd_assemble_single(_nmd_assemble_info* ai)
{
	size_t i = 0;

	/* Parse prefixes */
	bool lockPrefix = false, repeatPrefix = false, repeatZeroPrefix = false, repeatNotZeroPrefix = false;
	if (_nmd_strstr(ai->s, "lock ") == ai->s)
		lockPrefix = true, ai->s += 5;
	else if (_nmd_strstr(ai->s, "rep ") == ai->s)
		repeatPrefix = true, ai->s += 4;
	else if (_nmd_strstr(ai->s, "repe ") == ai->s || _nmd_strstr(ai->s, "repz ") == ai->s)
		repeatZeroPrefix = true, ai->s += 5;
	else if (_nmd_strstr(ai->s, "repne ") == ai->s || _nmd_strstr(ai->s, "repnz ") == ai->s)
		repeatNotZeroPrefix = true, ai->s += 6;
	
	if (_nmd_strstr(ai->s, "xacquire ") == ai->s)
	{

	}
	else if (_nmd_strstr(ai->s, "xrelease ") == ai->s)
	{

	}

	/* Parse opcodes */
	if (ai->mode == NMD_X86_MODE_64) /* Only x86-64. */
	{
		if (_nmd_strcmp(ai->s, "xchg r8,rax") || _nmd_strcmp(ai->s, "xchg rax,r8"))
		{
			ai->b[0] = 0x49;
			ai->b[1] = 0x90;
			return 2;
		}
		else if (_nmd_strcmp(ai->s, "xchg r8d,eax") || _nmd_strcmp(ai->s, "xchg eax,r8d"))
		{
			ai->b[0] = 0x41;
			ai->b[1] = 0x90;
			return 2;
		}
		else if (_nmd_strcmp(ai->s, "pushfq"))
		{
			ai->b[0] = 0x9c;
			return 1;
		}
		else if (_nmd_strcmp(ai->s, "popfq"))
		{
			ai->b[0] = 0x9d;
			return 1;
		}
		else if (_nmd_strcmp(ai->s, "iretq"))
		{
			ai->b[0] = 0x48;
			ai->b[1] = 0xcf;
			return 2;
		}
		else if (_nmd_strcmp(ai->s, "cdqe"))
		{
			ai->b[0] = 0x48;
			ai->b[1] = 0x98;
			return 2;
		}
		else if (_nmd_strcmp(ai->s, "cqo"))
		{
			ai->b[0] = 0x48;
			ai->b[1] = 0x99;
			return 2;
		}
	}
	else /* x86-16 / x86-32 */
	{
		if (_nmd_strcmp(ai->s, "pushad"))
		{
			ai->b[0] = 0x60;
			return 1;
		}
		else if (_nmd_strcmp(ai->s, "pusha"))
		{
			ai->b[0] = 0x66;
			ai->b[1] = 0x60;
			return 2;
		}
		else if (_nmd_strcmp(ai->s, "popad"))
		{
			ai->b[0] = 0x61;
			return 1;
		}
		else if (_nmd_strcmp(ai->s, "popa"))
		{
			ai->b[0] = 0x66;
			ai->b[1] = 0x62;
			return 2;
		}
		else if (_nmd_strcmp(ai->s, "pushfd"))
		{
			if (ai->mode == NMD_X86_MODE_16)
			{
				ai->b[0] = 0x66;
				ai->b[1] = 0x9c;
				return 2;
			}
			else
			{
				ai->b[0] = 0x9c;
				return 1;
			}
		}
		else if (_nmd_strcmp(ai->s, "popfd"))
		{
			if (ai->mode == NMD_X86_MODE_16)
			{
				ai->b[0] = 0x66;
				ai->b[1] = 0x9d;
				return 2;
			}
			else
			{
				ai->b[0] = 0x9d;
				return 1;
			}
		}
	}

	typedef struct NMD_StringBytePair { const char* s; uint8_t b; } NMD_StringBytePair;

	const NMD_StringBytePair op1SingleByte[] = {
		{ "int3",    0xcc },
		{ "nop",     0x90 },
		{ "ret",     0xc3 },
		{ "retf",    0xcb },
		{ "ret far", 0xcb },
		{ "leave",   0xc9 },
		{ "int1",    0xf1 },
		{ "push es", 0x06 },
		{ "push ss", 0x16 },
		{ "push ds", 0x1e },
		{ "push cs", 0x0e },
		{ "pop es",  0x07 },
		{ "pop ss",  0x17 },
		{ "pop ds",  0x1f },
		{ "daa",     0x27 },
		{ "aaa",     0x37 },
		{ "das",     0x2f },
		{ "aas",     0x3f },
		{ "xlat",    0xd7 },
		{ "fwait",   0x9b },
		{ "hlt",     0xf4 },
		{ "cmc",     0xf5 },
		{ "sahf",    0x9e },
		{ "lahf",    0x9f },
		{ "into",    0xce },
		{ "cwde",    0x98 },
		{ "cdq",     0x99 },
		{ "salc",    0xd6 },
		{ "slc",     0xf8 },
		{ "stc",     0xf9 },
		{ "cli",     0xfa },
		{ "sti",     0xfb },
		{ "cld",     0xfc },
		{ "std",     0xfd },
	};
	for (i = 0; i < _NMD_NUM_ELEMENTS(op1SingleByte); i++)
	{
		if (_nmd_strcmp(ai->s, op1SingleByte[i].s))
		{
			ai->b[0] = op1SingleByte[i].b;
			return 1;
		}
	}

	const NMD_StringBytePair op2SingleByte[] = {
		{ "syscall",  0x05 },
		{ "clts",     0x06 },
		{ "sysret",   0x07 },
		{ "invd",     0x08 },
		{ "wbinvd",   0x09 },
		{ "ud2",      0x0b },
		{ "femms",    0x0e },
		{ "wrmsr",    0x30 },
		{ "rdtsc",    0x31 },
		{ "rdmsr",    0x32 },
		{ "rdpmc",    0x33 },
		{ "sysenter", 0x34 },
		{ "sysexit",  0x35 },
		{ "getsec",   0x37 },
		{ "emms",     0x77 },
		{ "push fs",  0xa0 },
		{ "pop fs",   0xa1 },
		{ "cpuid",    0xa2 },
		{ "push gs",  0xa8 },
		{ "pop gs",   0xa9 },
		{ "rsm",      0xaa }
	};
	for (i = 0; i < _NMD_NUM_ELEMENTS(op2SingleByte); i++)
	{
		if (_nmd_strcmp(ai->s, op2SingleByte[i].s))
		{
			ai->b[0] = 0x0f;
			ai->b[1] = op2SingleByte[i].b;
			return 2;
		}
	}
	
	if (ai->s[0] == 'j')
	{
		const char* s = 0;
		for (i = 0; i < _NMD_NUM_ELEMENTS(_nmd_conditionSuffixes); i++)
		{
			if (_nmd_strstr_ex(ai->s + 1, _nmd_conditionSuffixes[i], &s) == ai->s + 1)
			{
				if (s[0] != ' ')
					return 0;


				int64_t num;
				size_t numDigits;
				if (!_nmd_parse_number(s + 1, &num, &numDigits))
					return 0;

				const int64_t delta = num - ai->runtimeAddress;
				if (delta >= -(1 << 7) + 2 && delta <= (1 << 7) - 1 + 2)
				{
					ai->b[0] = 0x70 + (uint8_t)i;
					*(int8_t*)(ai->b + 1) = (int8_t)(delta - 2);
					return 2;
				}
				else if (delta >= -(1 << 31) + 6 && delta <= ((size_t)(1) << 31) - 1 + 6)
				{
					ai->b[0] = 0x0f;
					ai->b[1] = 0x80 + (uint8_t)i;
					*(int32_t*)(ai->b + 2) = (int32_t)(delta - 6);
					return 6;
				}
				else
					return 0;
			}
		}
	}
	else if (_nmd_strstr(ai->s, "inc ") == ai->s || _nmd_strstr(ai->s, "dec ") == ai->s)
	{
		const char* tmp = ai->s + 4;
		nmd_x86_memory_operand memoryOperand;
		size_t size;
		if (_nmd_parse_memory_operand(&tmp, &memoryOperand, &size))
		{
			size_t offset = 0;
			if (memoryOperand.segment && memoryOperand.segment != ((memoryOperand.base == NMD_X86_REG_ESP || memoryOperand.index == NMD_X86_REG_ESP) ? NMD_X86_REG_SS : NMD_X86_REG_DS))
				ai->b[offset++] = _nmd_encode_segment_reg((NMD_X86_REG)memoryOperand.segment);

			ai->b[offset++] = size == 1 ? 0xfe : 0xff;

			nmd_x86_modrm modrm;
			modrm.fields.reg = ai->s[0] == 'i' ? 0 : 8;
			modrm.fields.mod = 0;

			if (memoryOperand.index != NMD_X86_REG_NONE && memoryOperand.base != NMD_X86_REG_NONE)
			{
				modrm.fields.rm = 0b100;
				nmd_x86_sib sib;
				sib.fields.scale = (uint8_t)_nmd_get_bit_index(memoryOperand.scale);
				sib.fields.base = memoryOperand.base - NMD_X86_REG_EAX;
				sib.fields.index = memoryOperand.index - NMD_X86_REG_EAX;

				const size_t nextOffset = offset;
				if (memoryOperand.disp != 0)
				{
					if (memoryOperand.disp >= -128 && memoryOperand.disp <= 127)
					{
						modrm.fields.mod = 1;
						*(int8_t*)(ai->b + offset + 2) = (int8_t)memoryOperand.disp;
						offset++;
					}
					else
					{
						modrm.fields.mod = 2;
						*(int32_t*)(ai->b + offset + 2) = (int32_t)memoryOperand.disp;
						offset += 4;
					}
				}

				ai->b[nextOffset] = modrm.modrm;
				ai->b[nextOffset + 1] = sib.sib;
				offset += 2;

				return offset;
			}
			else if (memoryOperand.base != NMD_X86_REG_NONE)
			{
				modrm.fields.rm = memoryOperand.base - NMD_X86_REG_EAX;
				const size_t nextOffset = offset;
				if (memoryOperand.disp != 0)
				{
					if (memoryOperand.disp >= -128 && memoryOperand.disp <= 127)
					{
						modrm.fields.mod = 1;
						*(int8_t*)(ai->b + offset + 1) = (int8_t)memoryOperand.disp;
						offset++;
					}
					else
					{
						modrm.fields.mod = 2;
						*(int32_t*)(ai->b + offset + 1) = (int32_t)memoryOperand.disp;
						offset += 4;
					}
				}
				ai->b[nextOffset] = modrm.modrm;
				offset++;
			}
			else
			{
				modrm.fields.rm = 0b101;
				ai->b[offset++] = modrm.modrm;
				*(int32_t*)(ai->b + offset) = (int32_t)memoryOperand.disp;
				offset += 4;
			}

			return offset;
		}

		size_t numPrefixes, index;
		size = _nmd_append_prefix_by_reg_size(ai->b, ai->s + 4, &numPrefixes, &index);
		if (size > 0)
		{
			if (ai->mode == NMD_X86_MODE_64)
			{
				ai->b[numPrefixes + 0] = size == 1 ? 0xfe : 0xff;
				ai->b[numPrefixes + 1] = 0xc0 + (ai->s[0] == 'i' ? 0 : 8) + (uint8_t)index;
				return numPrefixes + 2;
			}
			else
			{
				if (size == 1)
				{
					ai->b[0] = 0xfe;
					ai->b[1] = 0xc0 + (ai->s[0] == 'i' ? 0 : 8) + (uint8_t)index;
					return 2;
				}
				else
				{
					ai->b[numPrefixes + 0] = (ai->s[0] == 'i' ? 0x40 : 0x48) + (uint8_t)index;
					return numPrefixes + 1;
				}
			}
		}
	}
	else if (_nmd_strstr(ai->s, "push ") == ai->s)
	{
		size_t numDigits = 0;
		int64_t num = 0;
		if (_nmd_parse_number(ai->s + 5, &num, &numDigits))
		{
			if (*(ai->s + numDigits) != '\0' || !(num >= -(1 << 31) && num <= ((int64_t)1 << 31) - 1))
				return 0;

			if (num >= -(1 << 7) && num <= (1 << 7) - 1)
			{
				ai->b[0] = 0x6a;
				*(int8_t*)(ai->b + 1) = (int8_t)num;
				return 2;
			}
			else
			{
				ai->b[0] = 0x68;
				*(int32_t*)(ai->b + 1) = (int32_t)num;
				return 5;
			}
		}

		size_t n = _nmd_assemble_reg(ai, 0x50);
		if (n > 0)
			return n;

	}
	else if (_nmd_strstr(ai->s, "pop ") == ai->s)
	{
		ai->s += 3;
		return _nmd_assemble_reg(ai, 0x58);
	}
	else if (_nmd_strstr(ai->s, "emit ") == ai->s)
	{
		int64_t num = 0;
		size_t numDigits = 0;
		size_t offset = 5;
		while (_nmd_parse_number(ai->s + offset, &num, &numDigits))
		{
			if (num < 0 || num > 0xff)
				return 0;

			ai->b[i++] = (uint8_t)num;

			offset += numDigits;
			if (ai->s[offset] == ' ')
				offset++;
		}
		return i;
	}
	else if (_nmd_strcmp(ai->s, "pushf"))
	{
		if (ai->mode == NMD_X86_MODE_16)
		{
			ai->b[0] = 0x9c;
			return 1;
		}
		else
		{
			ai->b[0] = 0x66;
			ai->b[1] = 0x9c;
			return 2;
		}
	}
	else if (_nmd_strcmp(ai->s, "popf"))
	{
		if (ai->mode == NMD_X86_MODE_16)
		{
			ai->b[0] = 0x9d;
			return 1;
		}
		else
		{
			ai->b[0] = 0x66;
			ai->b[1] = 0x9d;
			return 2;
		}
	}
	else if (_nmd_strcmp(ai->s, "pause"))
	{
		ai->b[0] = 0xf3;
		ai->b[1] = 0x90;
		return 2;
	}
	else if (_nmd_strcmp(ai->s, "iret"))
	{
		if (ai->mode == NMD_X86_MODE_16)
		{
			ai->b[0] = 0xcf;
			return 1;
		}
		else
		{
			ai->b[0] = 0x66;
			ai->b[1] = 0xcf;
			return 2;
		}
	}
	else if (_nmd_strcmp(ai->s, "iretd"))
	{
		if (ai->mode == NMD_X86_MODE_16)
		{
			ai->b[0] = 0x66;
			ai->b[1] = 0xcf;
			return 2;
		}
		else
		{
			ai->b[0] = 0xcf;
			return 1;
		}
	}
	else if (_nmd_strcmp(ai->s, "cbw"))
	{
		ai->b[0] = 0x66;
		ai->b[1] = 0x98;
		return 2;
	}
	else if (_nmd_strcmp(ai->s, "cwd"))
	{
		ai->b[0] = 0x66;
		ai->b[1] = 0x99;
		return 2;
	}
	else if (_nmd_strcmp(ai->s, "pushf"))
	{
		ai->b[0] = 0x66;
		ai->b[1] = 0x9c;
		return 2;
	}
	else if (_nmd_strcmp(ai->s, "popf"))
	{
		ai->b[0] = 0x66;
		ai->b[1] = 0x9d;
		return 2;
	}
	
	return 0;
}

/*
Assembles an instruction from a string. Returns the number of bytes written to the buffer on success, zero otherwise. Instructions can be separated using either the ';' or '\n' character.
Parameters:
 - string         [in]         A pointer to a string that represents one or more instructions in assembly language.
 - buffer         [out]        A pointer to a buffer that receives the encoded instructions.
 - bufferSize     [in]         The size of the buffer in bytes.
 - runtimeAddress [in]         The instruction's runtime address. You may use 'NMD_X86_INVALID_RUNTIME_ADDRESS'.
 - mode           [in]         The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
 - count          [in/out/opt] A pointer to a variable that on input is the maximum number of instructions that can be parsed(or zero for unlimited instructions), and on output is the number of instructions parsed. This parameter may be 0(zero).
*/
size_t nmd_x86_assemble(const char* string, void* buffer, size_t bufferSize, uint64_t runtimeAddress, NMD_X86_MODE mode, size_t* count)
{
	char parsedString[128];
	const uint8_t* const bufferEnd = (uint8_t*)buffer + bufferSize;
	uint8_t* b = (uint8_t*)buffer;
	size_t remainingSize;

	uint8_t tempBuffer[NMD_X86_MAXIMUM_INSTRUCTION_LENGTH];

	_nmd_assemble_info ai;
	ai.s = parsedString;
	ai.mode = mode;
	ai.runtimeAddress = runtimeAddress;
	ai.b = tempBuffer;

	size_t numInstructions = 0;
	const size_t numMaxInstructions = (count && *count != 0) ? *count : (size_t)(-1);

	while (string[0] != '\0' && numInstructions < numMaxInstructions)
	{
		remainingSize = bufferEnd - b;

		/* Copy 'string' to 'buffer' converting it to lowercase and removing unwanted spaces. If the instruction separator character ';' and '\n' is found, stop. */
		size_t length = 0;
		bool allowSpace = false;
		for (; *string; string++)
		{
			char c = *string;
			if (c == ';' || c == '\n')
				break;
			else if (c == ' ' && !allowSpace)
				continue;

			if (length >= 128)
				return 0;

			c = _NMD_TOLOWER(c);
			parsedString[length++] = c;
			allowSpace = (_NMD_IS_LOWERCASE(c) || _NMD_IS_DECIMAL_NUMBER(c)) && (_NMD_IS_LOWERCASE(string[2]) || _NMD_IS_DECIMAL_NUMBER(string[2]));
		}

		if (*string != '\0')
			string++;

		/* If the last character is a ' '(space), remove it. */
		if (length > 0 && parsedString[length - 1] == ' ')
			length--;

		/* After all of the string manipulation, place the null character. */
		parsedString[length] = '\0';

		const size_t numBytes = _nmd_assemble_single(&ai);
		if (numBytes == 0 || numBytes > remainingSize)
			return 0;

		/* Copy bytes from 'tempBuffer' to the buffer provided by the user. */
		size_t i = 0;
		for (; i < numBytes; i++)
			b[i] = tempBuffer[i];

		b += numBytes;

		numInstructions++;
	}

	if (count)
		*count = numInstructions;

	return (size_t)(b - (uint8_t*)buffer);
}

void _nmd_decode_operand_segment_reg(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	if (instruction->segmentOverride)
		operand->fields.reg = (uint8_t)(NMD_X86_REG_ES + _nmd_get_bit_index(instruction->segmentOverride));
	else
		operand->fields.reg = (uint8_t)(!(instruction->prefixes & NMD_X86_PREFIXES_REX_B) && (instruction->modrm.fields.rm == 0b100 || instruction->modrm.fields.rm == 0b101) ? NMD_X86_REG_SS : NMD_X86_REG_DS);
}

void _nmd_decode_modrm_upper32(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_MEMORY;

	if (instruction->hasSIB)
	{
		operand->size++;

		if (instruction->sib.fields.base == 0b101)
		{
			if (instruction->modrm.fields.mod != 0b00)
				operand->fields.mem.base = (uint8_t)(instruction->mode == NMD_X86_MODE_64 && !(instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? NMD_X86_REG_R13 : NMD_X86_REG_RBP) : NMD_X86_REG_EBP);
		}
		else
			operand->fields.mem.base = (uint8_t)((instruction->mode == NMD_X86_MODE_64 && !(instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? NMD_X86_REG_R8 : NMD_X86_REG_RAX) : NMD_X86_REG_EAX) + instruction->sib.fields.base);

		if (instruction->sib.fields.index != 0b100)
			operand->fields.mem.index = (uint8_t)((instruction->mode == NMD_X86_MODE_64 && !(instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (instruction->prefixes & NMD_X86_PREFIXES_REX_X ? NMD_X86_REG_R8 : NMD_X86_REG_RAX) : NMD_X86_REG_EAX) + instruction->sib.fields.index);

		if (instruction->prefixes & NMD_X86_PREFIXES_REX_X && instruction->sib.fields.index == 0b100)
		{
			operand->fields.mem.index = (uint8_t)NMD_X86_REG_R12;
			operand->fields.mem.scale = instruction->sib.fields.scale;
		}
	}
	else if (!(instruction->modrm.fields.mod == 0b00 && instruction->modrm.fields.rm == 0b101))
	{
		if ((instruction->prefixes & (NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE | NMD_X86_PREFIXES_REX_B)) == (NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE | NMD_X86_PREFIXES_REX_B) && instruction->mode == NMD_X86_MODE_64)
			operand->fields.mem.base = (uint8_t)(NMD_X86_REG_R8D + instruction->modrm.fields.rm);
		else
			operand->fields.mem.base = (uint8_t)((instruction->mode == NMD_X86_MODE_64 && !(instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? NMD_X86_REG_R8 : NMD_X86_REG_RAX) : NMD_X86_REG_EAX) + instruction->modrm.fields.rm);
	}

	_nmd_decode_operand_segment_reg(instruction, operand);

	operand->fields.mem.disp = instruction->displacement;
	operand->size += (uint8_t)(instruction->dispMask);
}

void _nmd_decode_memory_operand(const nmd_x86_instruction* instruction, nmd_x86_operand* operand, uint8_t mod11baseReg)
{
	/* At least one byte is used for ModR/M. */
	operand->size = 1;

	if (instruction->modrm.fields.mod == 0b11)
	{
		operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
		operand->fields.reg = mod11baseReg + instruction->modrm.fields.rm;
	}
	else
		_nmd_decode_modrm_upper32(instruction, operand);
}

void _nmd_decode_operand_Eb(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	_nmd_decode_memory_operand(instruction, operand, NMD_X86_REG_AL);
}

void _nmd_decode_operand_Ew(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	_nmd_decode_memory_operand(instruction, operand, NMD_X86_REG_AX);
}

void _nmd_decode_operand_Ev(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	_nmd_decode_memory_operand(instruction, operand, (uint8_t)(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : NMD_X86_REG_EAX));
}

void _nmd_decode_operand_Ey(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	_nmd_decode_memory_operand(instruction, operand, (uint8_t)(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : NMD_X86_REG_EAX));
}

void _nmd_decode_operand_Qq(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	_nmd_decode_memory_operand(instruction, operand, NMD_X86_REG_MM0);
}

void _nmd_decode_operand_Wdq(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	_nmd_decode_memory_operand(instruction, operand, NMD_X86_REG_XMM0);
}

void _nmd_decode_operand_Gb(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = NMD_X86_REG_AL + instruction->modrm.fields.reg;
	operand->size = 1;
}

void _nmd_decode_operand_Gd(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = NMD_X86_REG_EAX + instruction->modrm.fields.reg;
	operand->size = 1;
}

void _nmd_decode_operand_Gw(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = NMD_X86_REG_AX + instruction->modrm.fields.reg;
	operand->size = 1;
}

void _nmd_decode_operand_Gv(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	if (instruction->prefixes & NMD_X86_PREFIXES_REX_B)
		operand->fields.reg = (uint8_t)((!(instruction->prefixes & NMD_X86_PREFIXES_REX_W) ? NMD_X86_REG_R8D : NMD_X86_REG_R8) + instruction->modrm.fields.reg);
	else
		operand->fields.reg = (uint8_t)((instruction->operandSize64 ? NMD_X86_REG_RAX : (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && instruction->mode != NMD_X86_MODE_16 ? NMD_X86_REG_AX : NMD_X86_REG_EAX)) + instruction->modrm.fields.reg);
	operand->size = 1;
}

void _nmd_decode_operand_Rv(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	if (instruction->prefixes & NMD_X86_PREFIXES_REX_R)
		operand->fields.reg = (uint8_t)((!(instruction->prefixes & NMD_X86_PREFIXES_REX_W) ? NMD_X86_REG_R8D : NMD_X86_REG_R8) + instruction->modrm.fields.rm);
	else
		operand->fields.reg = (uint8_t)((instruction->operandSize64 ? NMD_X86_REG_RAX : ((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && instruction->mode != NMD_X86_MODE_16) || (instruction->mode == NMD_X86_MODE_16 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? NMD_X86_REG_AX : NMD_X86_REG_EAX)) + instruction->modrm.fields.rm);
	operand->size = 1;
}

void _nmd_decode_operand_Gy(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = (uint8_t)((instruction->mode == NMD_X86_MODE_64 ? NMD_X86_REG_RAX : NMD_X86_REG_EAX) + instruction->modrm.fields.reg);
	operand->size = 1;
}

void _nmd_decode_operand_Pq(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = NMD_X86_REG_MM0 + instruction->modrm.fields.reg;
	operand->size = 1;
}

void _nmd_decode_operand_Nq(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = NMD_X86_REG_MM0 + instruction->modrm.fields.rm;
	operand->size = 1;
}

void _nmd_decode_operand_Vdq(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = NMD_X86_REG_XMM0 + instruction->modrm.fields.reg;
	operand->size = 1;
}

void _nmd_decode_operand_Udq(const nmd_x86_instruction* instruction, nmd_x86_operand* operand)
{
	operand->type = NMD_X86_OPERAND_TYPE_REGISTER;
	operand->fields.reg = NMD_X86_REG_XMM0 + instruction->modrm.fields.rm;
	operand->size = 1;
}

void _nmd_decode_conditional_flag(nmd_x86_instruction* instruction, const uint8_t condition)
{
	switch (condition)
	{
		case 0x0: instruction->testedFlags.fields.OF = 1; break;                                                                           /* Jump if overflow (OF=1) */
		case 0x1: instruction->testedFlags.fields.OF = 1; break;                                                                           /* Jump if not overflow (OF=0) */
		case 0x2: instruction->testedFlags.fields.CF = 1; break;                                                                           /* Jump if not above or equal (CF=1) */
		case 0x3: instruction->testedFlags.fields.CF = 1; break;                                                                           /* Jump if not below (CF=0) */
		case 0x4: instruction->testedFlags.fields.ZF = 1; break;                                                                           /* Jump if equal (ZF=1) */
		case 0x5: instruction->testedFlags.fields.ZF = 1; break;                                                                           /* Jump if not equal (ZF=0) */
		case 0x6: instruction->testedFlags.fields.CF = instruction->testedFlags.fields.ZF = 1; break;                                      /* Jump if not above (CF=1 or ZF=1) */
		case 0x7: instruction->testedFlags.fields.CF = instruction->testedFlags.fields.ZF = 1; break;                                      /* Jump if not below or equal (CF=0 and ZF=0) */
		case 0x8: instruction->testedFlags.fields.SF = 1; break;                                                                           /* Jump if sign (SF=1) */
		case 0x9: instruction->testedFlags.fields.SF = 1; break;                                                                           /* Jump if not sign (SF=0) */
		case 0xa: instruction->testedFlags.fields.PF = 1; break;                                                                           /* Jump if parity/parity even (PF=1) */
		case 0xb: instruction->testedFlags.fields.PF = 1; break;                                                                           /* Jump if parity odd (PF=0) */
		case 0xc: instruction->testedFlags.fields.SF = instruction->testedFlags.fields.OF = 1; break;                                      /* Jump if not greater or equal (SF != OF) */
		case 0xd: instruction->testedFlags.fields.SF = instruction->testedFlags.fields.OF = 1; break;                                      /* Jump if not less (SF=OF) */
		case 0xe: instruction->testedFlags.fields.ZF = instruction->testedFlags.fields.SF = instruction->testedFlags.fields.OF = 1; break; /* Jump if not greater (ZF=1 or SF != OF) */
		case 0xf: instruction->testedFlags.fields.ZF = instruction->testedFlags.fields.SF = instruction->testedFlags.fields.OF = 1; break; /* Jump if not less or equal (ZF=0 and SF=OF) */
	}
}

/* 'remaningSize' in the context of this function is the number of bytes the instruction takes not counting prefixes and opcode. */
bool _nmd_decode_modrm(const uint8_t** b, nmd_x86_instruction* const instruction, const size_t remainingSize)
{
	if (remainingSize == 0)
		return false;

	instruction->hasModrm = true;
	instruction->modrm.modrm = *++*b;
	const bool addressPrefix = (bool)(instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE);

	if (instruction->mode == NMD_X86_MODE_16)
	{
		if (instruction->modrm.fields.mod != 0b11)
		{
			if (instruction->modrm.fields.mod == 0b00)
			{
				if (instruction->modrm.fields.rm == 0b110)
					instruction->dispMask = NMD_X86_DISP16;
			}
			else
				instruction->dispMask = (uint8_t)(instruction->modrm.fields.mod == 0b01 ? NMD_X86_DISP8 : NMD_X86_DISP16);
		}
	}
	else
	{
		if (addressPrefix && instruction->mode == NMD_X86_MODE_32)
		{
			if ((instruction->modrm.fields.mod == 0b00 && instruction->modrm.fields.rm == 0b110) || instruction->modrm.fields.mod == 0b10)
				instruction->dispMask = NMD_X86_DISP16;
			else if (instruction->modrm.fields.mod == 0b01)
				instruction->dispMask = NMD_X86_DISP8;
		}
		else /*if (!addressPrefix || (addressPrefix && **b >= 0x40) || (addressPrefix && instruction->mode == NMD_X86_MODE_64)) */
		{
			/* Check for SIB byte */
			if (instruction->modrm.modrm < 0xC0 && instruction->modrm.fields.rm == 0b100 && (!addressPrefix || (addressPrefix && instruction->mode == NMD_X86_MODE_64)))
			{
				if (remainingSize < 2)
					return false;

				instruction->hasSIB = true;
				instruction->sib.sib = *++*b;
			}

			if (instruction->modrm.fields.mod == 0b01) /* disp8 (ModR/M) */
				instruction->dispMask = NMD_X86_DISP8;
			else if ((instruction->modrm.fields.mod == 0b00 && instruction->modrm.fields.rm == 0b101) || instruction->modrm.fields.mod == 0b10) /* disp16,32 (ModR/M) */
				instruction->dispMask = (uint8_t)(addressPrefix && !(instruction->mode == NMD_X86_MODE_64 && instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? NMD_X86_DISP16 : NMD_X86_DISP32);
			else if (instruction->hasSIB && instruction->sib.fields.base == 0b101) /* disp8,32 (SIB) */
				instruction->dispMask = (uint8_t)(instruction->modrm.fields.mod == 0b01 ? NMD_X86_DISP8 : NMD_X86_DISP32);
		}
	}

	if (remainingSize - (instruction->hasSIB ? 2 : 1) < instruction->dispMask)
		return false;

	size_t i = 0;
	for (; i < (size_t)instruction->dispMask; i++, (*b)++)
		((uint8_t*)(&instruction->displacement))[i] = *(*b + 1);

	return true;
}


/*
Decodes an instruction. Returns true if the instruction is valid, false otherwise.
Parameters:
 - buffer      [in]  A pointer to a buffer containing an encoded instruction.
 - bufferSize  [in]  The size of the buffer in bytes.
 - instruction [out] A pointer to a variable of type 'nmd_x86_instruction' that receives information about the instruction.
 - mode        [in]  The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
 - flags       [in]  A mask of 'NMD_X86_DECODER_FLAGS_XXX' that specifies which features the decoder is allowed to use. If uncertain, use 'NMD_X86_DECODER_FLAGS_MINIMAL'.
*/
bool nmd_x86_decode_buffer(const void* buffer, size_t bufferSize, nmd_x86_instruction* instruction, NMD_X86_MODE mode, uint32_t flags)
{
	if (bufferSize == 0)
		return false;

	/* Clear 'instruction'. */
	size_t i = 0;
	for (; i < sizeof(nmd_x86_instruction); i++)
		((uint8_t*)(instruction))[i] = 0x00;

	instruction->mode = (uint8_t)mode;

	const uint8_t* b = (const uint8_t*)(buffer);

	/* Parse legacy prefixes & REX prefixes. */
	i = 0;
	const size_t numMaxBytes = bufferSize < NMD_X86_MAXIMUM_INSTRUCTION_LENGTH ? bufferSize : NMD_X86_MAXIMUM_INSTRUCTION_LENGTH;
	for (; i < numMaxBytes; i++, b++)
	{
		switch (*b)
		{
		case 0xF0: instruction->prefixes = (instruction->prefixes | (instruction->simdPrefix = NMD_X86_PREFIXES_LOCK)); continue;
		case 0xF2: instruction->prefixes = (instruction->prefixes | (instruction->simdPrefix = NMD_X86_PREFIXES_REPEAT_NOT_ZERO)), instruction->repeatPrefix = false; continue;
		case 0xF3: instruction->prefixes = (instruction->prefixes | (instruction->simdPrefix = NMD_X86_PREFIXES_REPEAT)), instruction->repeatPrefix = true; continue;
		case 0x2E: instruction->prefixes = (instruction->prefixes | (instruction->segmentOverride = NMD_X86_PREFIXES_CS_SEGMENT_OVERRIDE)); continue;
		case 0x36: instruction->prefixes = (instruction->prefixes | (instruction->segmentOverride = NMD_X86_PREFIXES_SS_SEGMENT_OVERRIDE)); continue;
		case 0x3E: instruction->prefixes = (instruction->prefixes | (instruction->segmentOverride = NMD_X86_PREFIXES_DS_SEGMENT_OVERRIDE)); continue;
		case 0x26: instruction->prefixes = (instruction->prefixes | (instruction->segmentOverride = NMD_X86_PREFIXES_ES_SEGMENT_OVERRIDE)); continue;
		case 0x64: instruction->prefixes = (instruction->prefixes | (instruction->segmentOverride = NMD_X86_PREFIXES_FS_SEGMENT_OVERRIDE)); continue;
		case 0x65: instruction->prefixes = (instruction->prefixes | (instruction->segmentOverride = NMD_X86_PREFIXES_GS_SEGMENT_OVERRIDE)); continue;
		case 0x66: instruction->prefixes = (instruction->prefixes | (instruction->simdPrefix = NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)), instruction->operandSize64 = false; continue;
		case 0x67: instruction->prefixes = (instruction->prefixes | NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE); continue;
		default:
			if ((mode == NMD_X86_MODE_64) && NMD_R(*b) == 4) /* 0x40 */
			{
				instruction->hasRex = true;
				instruction->rex = *b;
				instruction->prefixes = (instruction->prefixes & ~(NMD_X86_PREFIXES_REX_B | NMD_X86_PREFIXES_REX_X | NMD_X86_PREFIXES_REX_R | NMD_X86_PREFIXES_REX_W));

				if (*b & 0b0001) /* Bit position 0. */
					instruction->prefixes = instruction->prefixes | NMD_X86_PREFIXES_REX_B;
				if (*b & 0b0010) /* Bit position 1. */
					instruction->prefixes = instruction->prefixes | NMD_X86_PREFIXES_REX_X;
				if (*b & 0b0100) /* Bit position 2. */
					instruction->prefixes = instruction->prefixes | NMD_X86_PREFIXES_REX_R;
				if (*b & 0b1000) /* Bit position 3. */
				{
					instruction->prefixes = instruction->prefixes | NMD_X86_PREFIXES_REX_W;
					instruction->operandSize64 = true;
				}

				continue;
			}
		}

		break;
	}

	instruction->numPrefixes = (uint8_t)((ptrdiff_t)(b)-(ptrdiff_t)(buffer));

	const size_t remainingValidBytes = (NMD_X86_MAXIMUM_INSTRUCTION_LENGTH - instruction->numPrefixes);
	if (remainingValidBytes == 0)
		return false;

	const size_t remainingBufferSize = bufferSize - instruction->numPrefixes;
	if (remainingBufferSize == 0)
		return false;

	const size_t remainingSize = remainingValidBytes < remainingBufferSize ? remainingValidBytes : remainingBufferSize;

	/* Assume NMD_X86_INSTRUCTION_ENCODING_LEGACY. */
	instruction->encoding = NMD_X86_ENCODING_LEGACY;

	/* Opcode byte. This variable is used because it's easier to write 'op' than 'instruction->opcode'. */
	uint8_t op = 0;

	/* Parse opcode. */
	if (*b == 0x0F) /* 2 or 3 byte opcode. */
	{
		if (remainingSize < 2)
			return false;

		b++;

		if (*b == 0x38 || *b == 0x3A) /* 3 byte opcode. */
		{
			if (remainingSize < 4)
				return false;

			instruction->opcodeMap = (uint8_t)(*b == 0x38 ? NMD_X86_OPCODE_MAP_0F38 : NMD_X86_OPCODE_MAP_0F3A);
			instruction->opcodeSize = 3;
			instruction->opcode = *++b;

			op = instruction->opcode;

			if (!_nmd_decode_modrm(&b, instruction, remainingSize - 3))
				return false;

			const nmd_x86_modrm modrm = instruction->modrm;
			if (instruction->opcodeMap == NMD_X86_OPCODE_MAP_0F38)
			{
#ifndef NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK
				if (flags & NMD_X86_DECODER_FLAGS_VALIDITY_CHECK)
				{
					/* Check if the instruction is invalid. */
					if (op == 0x36)
					{
						return false;
					}
					else if (op <= 0xb || (op >= 0x1c && op <= 0x1e))
					{
						if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
							return false;
					}
					else if (op >= 0xc8 && op <= 0xcd)
					{
						if (instruction->simdPrefix)
							return false;
					}
					else if (op == 0x10 || op == 0x14 || op == 0x15 || op == 0x17 || (op >= 0x20 && op <= 0x25) || op == 0x28 || op == 0x29 || op == 0x2b || NMD_R(op) == 3 || op == 0x40 || op == 0x41 || op == 0xcf || (op >= 0xdb && op <= 0xdf))
					{
						if (instruction->simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
							return false;
					}
					else if (op == 0x2a || (op >= 0x80 && op <= 0x82))
					{
						if (modrm.fields.mod == 0b11 || instruction->simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
							return false;
					}
					else if (op == 0xf0 || op == 0xf1)
					{
						if (modrm.fields.mod == 0b11 && (instruction->simdPrefix == NMD_X86_PREFIXES_NONE || instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE))
							return false;
						else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
							return false;
					}
					else if (op == 0xf5 || op == 0xf8)
					{
						if (instruction->simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || modrm.fields.mod == 0b11)
							return false;
					}
					else if (op == 0xf6)
					{
						if (instruction->simdPrefix == NMD_X86_PREFIXES_NONE && modrm.fields.mod == 0b11)
							return false;
						else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
							return false;
					}
					else if (op == 0xf9)
					{
						if (instruction->simdPrefix != NMD_X86_PREFIXES_NONE || modrm.fields.mod == 0b11)
							return false;
					}
					else
						return false;
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID
				if (flags & NMD_X86_DECODER_FLAGS_INSTRUCTION_ID)
				{
					if (NMD_R(op) == 0x00)
						instruction->id = NMD_X86_INSTRUCTION_PSHUFB + op;
					else if (op >= 0x1c && op <= 0x1e)
						instruction->id = NMD_X86_INSTRUCTION_PABSB + (op - 0x1c);
					else if (NMD_R(op) == 2)
						instruction->id = NMD_X86_INSTRUCTION_PMOVSXBW + NMD_C(op);
					else if (NMD_R(op) == 3)
						instruction->id = NMD_X86_INSTRUCTION_PMOVZXBW + NMD_C(op);
					else if (NMD_R(op) == 8)
						instruction->id = NMD_X86_INSTRUCTION_INVEPT + NMD_C(op);
					else if (NMD_R(op) == 0xc)
						instruction->id = NMD_X86_INSTRUCTION_SHA1NEXTE + (NMD_C(op) - 8);
					else if (NMD_R(op) == 0xd)
						instruction->id = NMD_X86_INSTRUCTION_AESIMC + (NMD_C(op) - 0xb);
					else
					{
						switch (op)
						{
						case 0x10: instruction->id = NMD_X86_INSTRUCTION_PBLENDVB; break;
						case 0x14: instruction->id = NMD_X86_INSTRUCTION_BLENDVPS; break;
						case 0x15: instruction->id = NMD_X86_INSTRUCTION_BLENDVPD; break;
						case 0x17: instruction->id = NMD_X86_INSTRUCTION_PTEST; break;
						case 0x40: instruction->id = NMD_X86_INSTRUCTION_PMULLD; break;
						case 0x41: instruction->id = NMD_X86_INSTRUCTION_PHMINPOSUW; break;
						case 0xf0: case 0xf1: instruction->id = (uint16_t)((instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || instruction->simdPrefix == 0x00) ? NMD_X86_INSTRUCTION_MOVBE : NMD_X86_INSTRUCTION_CRC32); break;
						case 0xf6: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_ADCX : NMD_X86_INSTRUCTION_ADOX); break;
						}
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID */
				
#ifndef NMD_ASSEMBLY_DISABLE_DECODER_CPU_FLAGS
				if (flags & NMD_X86_DECODER_FLAGS_CPU_FLAGS)
				{
					if (op == 0x80 || op == 0x81) /* invept,invvpid */
					{
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_ZF;
						instruction->clearedFlags.eflags = NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_OF;
					}
					else if (op == 0xf6)
					{
						if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE) /* adcx */
							instruction->modifiedFlags.eflags = instruction->testedFlags.eflags = NMD_X86_EFLAGS_CF;
						if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT) /* adox */
							instruction->modifiedFlags.eflags = instruction->testedFlags.eflags = NMD_X86_EFLAGS_OF;
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_CPU_FLAGS */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS
				if (flags & NMD_X86_DECODER_FLAGS_OPERANDS)
				{
					instruction->numOperands = 2;
					instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
					instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;

					if (NMD_R(op) == 0 || (op >= 0x1c && op <= 0x1e))
					{
						_nmd_decode_operand_Pq(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Qq(instruction, &instruction->operands[1]);
					}
					else if (NMD_R(op) == 8)
					{
						_nmd_decode_operand_Gy(instruction, &instruction->operands[0]);
						_nmd_decode_modrm_upper32(instruction, &instruction->operands[1]);
					}
					else if (NMD_R(op) >= 1 && NMD_R(op) <= 0xe)
					{
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Wdq(instruction, &instruction->operands[1]);
					}
					else if (op == 0xf6)
					{
						_nmd_decode_operand_Gy(instruction, &instruction->operands[!instruction->simdPrefix ? 1 : 0]);
						_nmd_decode_operand_Ey(instruction, &instruction->operands[!instruction->simdPrefix ? 0 : 1]);
					}
					else if (op == 0xf0 || op == 0xf1)
					{
						if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO || (instruction->prefixes & (NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE | NMD_X86_PREFIXES_REPEAT_NOT_ZERO)) == (NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE | NMD_X86_PREFIXES_REPEAT_NOT_ZERO))
						{
							_nmd_decode_operand_Gd(instruction, &instruction->operands[0]);
							if (op == 0xf0)
								_nmd_decode_operand_Eb(instruction, &instruction->operands[1]);
							else if (instruction->prefixes == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
								_nmd_decode_operand_Ey(instruction, &instruction->operands[1]);
							else
								_nmd_decode_operand_Ew(instruction, &instruction->operands[1]);
						}
						else
						{
							if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
								_nmd_decode_operand_Gw(instruction, &instruction->operands[op == 0xf0 ? 0 : 1]);
							else
								_nmd_decode_operand_Gy(instruction, &instruction->operands[op == 0xf0 ? 0 : 1]);

							_nmd_decode_memory_operand(instruction, &instruction->operands[op == 0xf0 ? 1 : 0], (uint8_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : (instruction->operandSize64 ? NMD_X86_REG_RAX : NMD_X86_REG_EAX)));
						}
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS */
			}
			else /* 0x3a */
			{
				if (remainingSize < 5)
					return false;

				instruction->immMask = NMD_X86_IMM8;

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK
				if (flags & NMD_X86_DECODER_FLAGS_VALIDITY_CHECK)
				{
					/* Check if the instruction is invalid. */
					if ((op >= 0x8 && op <= 0xe) || (op >= 0x14 && op <= 0x17) || (op >= 0x20 && op <= 0x22) || (op >= 0x40 && op <= 0x42) || op == 0x44 || (op >= 0x60 && op <= 0x63) || op == 0xdf || op == 0xce || op == 0xcf)
					{
						if (instruction->simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
							return false;
					}
					else if (op == 0x0f || op == 0xcc)
					{
						if (instruction->simdPrefix)
							return false;
					}
					else
						return false;
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID
				if (flags & NMD_X86_DECODER_FLAGS_INSTRUCTION_ID)
				{
					if (NMD_R(op) == 0)
						instruction->id = NMD_X86_INSTRUCTION_ROUNDPS + (op - 8);
					else if (NMD_R(op) == 4)
						instruction->id = NMD_X86_INSTRUCTION_DPPS + NMD_C(op);
					else if (NMD_R(op) == 6)
						instruction->id = NMD_X86_INSTRUCTION_PCMPESTRM + NMD_C(op);
					else
					{
						switch (op)
						{
						case 0x14: instruction->id = NMD_X86_INSTRUCTION_PEXTRB; break;
						case 0x15: instruction->id = NMD_X86_INSTRUCTION_PEXTRW; break;
						case 0x16: instruction->id = (uint16_t)(instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_INSTRUCTION_PEXTRQ : NMD_X86_INSTRUCTION_PEXTRD); break;
						case 0x17: instruction->id = NMD_X86_INSTRUCTION_EXTRACTPS; break;
						case 0x20: instruction->id = NMD_X86_INSTRUCTION_PINSRB; break;
						case 0x21: instruction->id = NMD_X86_INSTRUCTION_INSERTPS; break;
						case 0x22: instruction->id = (uint16_t)(instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_INSTRUCTION_PINSRQ : NMD_X86_INSTRUCTION_PINSRD); break;
						case 0xcc: instruction->id = NMD_X86_INSTRUCTION_SHA1RNDS4; break;
						case 0xdf: instruction->id = NMD_X86_INSTRUCTION_AESKEYGENASSIST; break;
						}
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS
				if (flags & NMD_X86_DECODER_FLAGS_OPERANDS)
				{
					instruction->numOperands = 3;
					instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
					instruction->operands[1].action = instruction->operands[2].action = NMD_X86_OPERAND_ACTION_READ;
					instruction->operands[2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;

					if (op == 0x0f && !instruction->simdPrefix)
					{
						_nmd_decode_operand_Pq(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Qq(instruction, &instruction->operands[1]);
					}
					else if (NMD_R(op) == 1)
					{
						_nmd_decode_memory_operand(instruction, &instruction->operands[0], NMD_X86_REG_EAX);
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[1]);
					}
					else if (NMD_R(op) == 2)
					{
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
						_nmd_decode_memory_operand(instruction, &instruction->operands[1], (uint8_t)(NMD_C(op) == 1 ? NMD_X86_REG_XMM0 : NMD_X86_REG_EAX));
					}
					else if (op == 0xcc || op == 0xdf || NMD_R(op) == 4 || NMD_R(op) == 6 || NMD_R(op) == 0)
					{
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Wdq(instruction, &instruction->operands[1]);
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS */
			}
		}
		else if (*b == 0x0f) /* 3DNow! opcode map*/
		{
#ifndef NMD_ASSEMBLY_DISABLE_DECODER_3DNOW
			if (flags & NMD_X86_DECODER_FLAGS_3DNOW)
			{
				if (remainingSize < 5)
					return false;
			
				if (!_nmd_decode_modrm(&b, instruction, remainingSize - 2))
					return false;

				instruction->encoding = NMD_X86_ENCODING_3DNOW;
				instruction->opcode = 0x0f;
				instruction->immMask = NMD_X86_IMM8; /* The real opcode is encoded as the immediate byte. */
				instruction->immediate = *(b + 1);

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK
				if (!_nmd_findByte(_nmd_valid3DNowOpcodes, sizeof(_nmd_valid3DNowOpcodes), (uint8_t)instruction->immediate))
					return false;
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK */
			}
			else
				return false;
#else /* NMD_ASSEMBLY_DISABLE_DECODER_3DNOW */
		return false;
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_3DNOW */
		}
		else /* 2 byte opcode. */
		{
			instruction->opcodeSize = 2;
			instruction->opcode = *b;
			instruction->opcodeMap = NMD_X86_OPCODE_MAP_0F;

			op = instruction->opcode;

			/* Check for ModR/M, SIB and displacement. */
			if (op >= 0x20 && op <= 0x23 && remainingSize == 2)
				instruction->hasModrm = true, instruction->modrm.modrm = *++b;
			else if (op < 4 || (NMD_R(op) != 3 && NMD_R(op) > 0 && NMD_R(op) < 7) || (op >= 0xD0 && op != 0xFF) || (NMD_R(op) == 7 && NMD_C(op) != 7) || NMD_R(op) == 9 || NMD_R(op) == 0xB || (NMD_R(op) == 0xC && NMD_C(op) < 8) || (NMD_R(op) == 0xA && (op % 8) >= 3) || op == 0x0ff || op == 0x00 || op == 0x0d)
			{
				if (!_nmd_decode_modrm(&b, instruction, remainingSize - 2))
					return false;
			}

			const nmd_x86_modrm modrm = instruction->modrm;
#ifndef NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK
			if (flags & NMD_X86_DECODER_FLAGS_VALIDITY_CHECK)
			{
				/* Check if the instruction is invalid. */
				if (_nmd_findByte(_nmd_invalid2op, sizeof(_nmd_invalid2op), op))
					return false;
				else if (op == 0xc7)
				{
					if ((!instruction->simdPrefix && (modrm.fields.mod == 0b11 ? modrm.fields.reg <= 0b101 : modrm.fields.reg == 0b000 || modrm.fields.reg == 0b010)) || (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO && (modrm.fields.mod == 0b11 || modrm.fields.reg != 0b001)) || ((instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT) && (modrm.fields.mod == 0b11 ? modrm.fields.reg <= (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? 0b110 : 0b101) : (modrm.fields.reg != 0b001 && modrm.fields.reg != 0b110))))
						return false;
				}
				else if (op == 0x00)
				{
					if (modrm.fields.reg >= 0b110)
						return false;
				}
				else if (op == 0x01)
				{
					if ((modrm.fields.mod == 0b11 ? ((instruction->prefixes & (NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE | NMD_X86_PREFIXES_REPEAT_NOT_ZERO | NMD_X86_PREFIXES_REPEAT) && ((modrm.modrm >= 0xc0 && modrm.modrm <= 0xc5) || (modrm.modrm >= 0xc8 && modrm.modrm <= 0xcb) || (modrm.modrm >= 0xcf && modrm.modrm <= 0xd1) || (modrm.modrm >= 0xd4 && modrm.modrm <= 0xd7) || modrm.modrm == 0xee || modrm.modrm == 0xef || modrm.modrm == 0xfa || modrm.modrm == 0xfb)) || (modrm.fields.reg == 0b000 && modrm.fields.rm >= 0b110) || (modrm.fields.reg == 0b001 && modrm.fields.rm >= 0b100 && modrm.fields.rm <= 0b110) || (modrm.fields.reg == 0b010 && (modrm.fields.rm == 0b010 || modrm.fields.rm == 0b011)) || (modrm.fields.reg == 0b101 && modrm.fields.rm < 0b110 && (!(instruction->prefixes & NMD_X86_PREFIXES_REPEAT) || (instruction->prefixes & NMD_X86_PREFIXES_REPEAT && (modrm.fields.rm != 0b000 && modrm.fields.rm != 0b010)))) || (modrm.fields.reg == 0b111 && (modrm.fields.rm > 0b101 || (mode != NMD_X86_MODE_64 && modrm.fields.rm == 0b000)))) : (!(instruction->prefixes & NMD_X86_PREFIXES_REPEAT) && modrm.fields.reg == 0b101)))
						return false;
				}
				else if (op == 0x1A || op == 0x1B)
				{
					if (modrm.fields.mod == 0b11)
						return false;
				}
				else if (op == 0x20 || op == 0x22)
				{
					if (modrm.fields.reg == 0b001 || modrm.fields.reg >= 0b101)
						return false;
				}
				else if (op >= 0x24 && op <= 0x27)
					return false;
				else if (op >= 0x3b && op <= 0x3f)
					return false;
				else if (NMD_R(op) == 5)
				{
					if ((op == 0x50 && modrm.fields.mod != 0b11) || (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && (op == 0x52 || op == 0x53)) || (instruction->prefixes & NMD_X86_PREFIXES_REPEAT && (op == 0x50 || (op >= 0x54 && op <= 0x57))) || (instruction->prefixes & NMD_X86_PREFIXES_REPEAT_NOT_ZERO && (op == 0x50 || (op >= 0x52 && op <= 0x57) || op == 0x5b)))
						return false;
				}
				else if (NMD_R(op) == 6)
				{
					if ((!(instruction->prefixes & (NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE | NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO)) && (op == 0x6c || op == 0x6d)) || (instruction->prefixes & NMD_X86_PREFIXES_REPEAT && op != 0x6f) || instruction->prefixes & NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
						return false;
				}
				else if (op == 0x78 || op == 0x79)
				{
					if ((((instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && op == 0x78) && !(modrm.fields.mod == 0b11 && modrm.fields.reg == 0b000)) || ((instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) && modrm.fields.mod != 0b11)) || (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT))
						return false;
				}
				else if (op == 0x7c || op == 0x7d)
				{
					if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT || !(instruction->prefixes & (NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE | NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO)))
						return false;
				}
				else if (op == 0x7e || op == 0x7f)
				{
					if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
						return false;
				}
				else if (op >= 0x71 && op <= 0x73)
				{
					if (instruction->prefixes & (NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO) || modrm.modrm <= 0xcf || (modrm.modrm >= 0xe8 && modrm.modrm <= 0xef))
						return false;
				}
				else if (op == 0x73)
				{
					if (modrm.modrm >= 0xe0 && modrm.modrm <= 0xe8)
						return false;
				}
				else if (op == 0xa6)
				{
					if (modrm.modrm != 0xc0 && modrm.modrm != 0xc8 && modrm.modrm != 0xd0)
						return false;
				}
				else if (op == 0xa7)
				{
					if (!(modrm.fields.mod == 0b11 && modrm.fields.reg <= 0b101 && modrm.fields.rm == 0b000))
						return false;
				}
				else if (op == 0xae)
				{
					if (((!instruction->simdPrefix && modrm.fields.mod == 0b11 && modrm.fields.reg <= 0b100) || (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO && !(modrm.fields.mod == 0b11 && modrm.fields.reg == 0b110)) || (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && (modrm.fields.reg < 0b110 || (modrm.fields.mod == 0b11 && modrm.fields.reg == 0b111))) || (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT && (modrm.fields.reg != 0b100 && modrm.fields.reg != 0b110) && !(modrm.fields.mod == 0b11 && modrm.fields.reg == 0b101))))
						return false;
				}
				else if (op == 0xb8)
				{
					if (!(instruction->prefixes & NMD_X86_PREFIXES_REPEAT))
						return false;
				}
				else if (op == 0xba)
				{
					if (modrm.fields.reg <= 0b011)
						return false;
				}
				else if (op == 0xd0)
				{
					if (!instruction->simdPrefix || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
						return false;
				}
				else if (op == 0xe0)
				{
					if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
						return false;
				}
				else if (op == 0xf0)
				{
					if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? modrm.fields.mod == 0b11 : true)
						return false;
				}
				else if (instruction->prefixes & (NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO))
				{
					if ((op >= 0x13 && op <= 0x17 && !(op == 0x16 && instruction->prefixes & NMD_X86_PREFIXES_REPEAT)) || op == 0x28 || op == 0x29 || op == 0x2e || op == 0x2f || (op <= 0x76 && op >= 0x74))
						return false;
				}
				else if (op == 0x71 || op == 0x72 || (op == 0x73 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)))
				{
					if ((modrm.modrm >= 0xd8 && modrm.modrm <= 0xdf) || modrm.modrm >= 0xf8)
						return false;
				}
				else if (op >= 0xc3 && op <= 0xc6)
				{
					if ((op == 0xc5 && modrm.fields.mod != 0b11) || (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) || (op == 0xc3 && instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE))
						return false;
				}
				else if (NMD_R(op) >= 0xd && NMD_C(op) != 0 && op != 0xff && ((NMD_C(op) == 6 && NMD_R(op) != 0xf) ? (!instruction->simdPrefix || (NMD_R(op) == 0xD && (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) ? modrm.fields.mod != 0b11 : false)) : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO || ((NMD_C(op) == 7 && NMD_R(op) != 0xe) ? modrm.fields.mod != 0b11 : false))))
					return false;
				else if (modrm.fields.mod == 0b11)
				{
					if (op == 0xb2 || op == 0xb4 || op == 0xb5 || op == 0xc3 || op == 0xe7 || op == 0x2b || (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && (op == 0x12 || op == 0x16)) || (!(instruction->prefixes & (NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO)) && (op == 0x13 || op == 0x17)))
						return false;
				}
			}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID
			if (flags & NMD_X86_DECODER_FLAGS_INSTRUCTION_ID)
			{
				if (NMD_R(op) == 8)
					instruction->id = NMD_X86_INSTRUCTION_JO + NMD_C(op);
				else if (op >= 0xa2 && op <= 0xa5)
					instruction->id = NMD_X86_INSTRUCTION_CPUID + (op - 0xa2);
				else if (op == 0x05)
					instruction->id = NMD_X86_INSTRUCTION_SYSCALL;
				else if (NMD_R(op) == 4)
					instruction->id = NMD_X86_INSTRUCTION_CMOVO + NMD_C(op);
				else if (op == 0x00)
					instruction->id = NMD_X86_INSTRUCTION_SLDT + modrm.fields.reg;
				else if (op == 0x01)
				{
					if (modrm.fields.mod == 0b11)
					{
						switch (modrm.fields.reg)
						{
						case 0b000: instruction->id = NMD_X86_INSTRUCTION_VMCALL + modrm.fields.rm; break;
						case 0b001: instruction->id = NMD_X86_INSTRUCTION_MONITOR + modrm.fields.rm; break;
						case 0b010: instruction->id = NMD_X86_INSTRUCTION_XGETBV + modrm.fields.rm; break;
						case 0b011: instruction->id = NMD_X86_INSTRUCTION_VMRUN + modrm.fields.rm; break;
						case 0b100: instruction->id = NMD_X86_INSTRUCTION_SMSW; break;
						case 0b110: instruction->id = NMD_X86_INSTRUCTION_LMSW; break;
						case 0b111: instruction->id = (uint16_t)(modrm.fields.rm == 0b000 ? NMD_X86_INSTRUCTION_SWAPGS : NMD_X86_INSTRUCTION_RDTSCP); break;
						}
					}
					else
						instruction->id = NMD_X86_INSTRUCTION_SGDT + modrm.fields.reg;
				}
				else if (op <= 0x0b)
					instruction->id = NMD_X86_INSTRUCTION_LAR + (op - 2);
				else if (op == 0x19 || (op >= 0x1c && op <= 0x1f))
				{
					if (op == 0x1e && modrm.modrm == 0xfa)
						instruction->id = NMD_X86_INSTRUCTION_ENDBR64;
					else if (op == 0x1e && modrm.modrm == 0xfb)
						instruction->id = NMD_X86_INSTRUCTION_ENDBR32;
					else
						instruction->id = NMD_X86_INSTRUCTION_NOP;
				}
				else if (op >= 0x10 && op <= 0x17)
				{
					switch (instruction->simdPrefix)
					{
					case NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE: instruction->id = NMD_X86_INSTRUCTION_VMOVUPS + NMD_C(op); break;
					case NMD_X86_PREFIXES_REPEAT: instruction->id = NMD_X86_INSTRUCTION_VMOVUPS + NMD_C(op); break;
					case NMD_X86_PREFIXES_REPEAT_NOT_ZERO: instruction->id = NMD_X86_INSTRUCTION_VMOVUPS + NMD_C(op); break;
					default: instruction->id = NMD_X86_INSTRUCTION_VMOVUPS + NMD_C(op); break;
					}
				}
				else if (op >= 0x20 && op <= 0x23)
					instruction->id = NMD_X86_INSTRUCTION_MOV;
				else if (NMD_R(op) == 3)
					instruction->id = NMD_X86_INSTRUCTION_WRMSR + NMD_C(op);
				else if (NMD_R(op) == 5)
				{
					switch (instruction->simdPrefix)
					{
					case NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE: instruction->id = NMD_X86_INSTRUCTION_MOVMSKPD + NMD_C(op); break;
					case NMD_X86_PREFIXES_REPEAT: instruction->id = NMD_X86_INSTRUCTION_BNDMOV + NMD_C(op); break;
					case NMD_X86_PREFIXES_REPEAT_NOT_ZERO: instruction->id = NMD_X86_INSTRUCTION_BNDCL + NMD_C(op); break;
					default:   instruction->id = NMD_X86_INSTRUCTION_MOVMSKPS + NMD_C(op); break;
					}
				}
				else if (op >= 0x60 && op <= 0x6d)
					instruction->id = NMD_X86_INSTRUCTION_PUNPCKLBW + NMD_C(op);
				else if (op >= 0x74 && op <= 0x76)
					instruction->id = NMD_X86_INSTRUCTION_PCMPEQB + (op - 0x74);
				else if (op >= 0xb2 && op <= 0xb5)
					instruction->id = NMD_X86_INSTRUCTION_LSS + (op - 0xb2);
				else if (op >= 0xc3 && op <= 0xc5)
					instruction->id = NMD_X86_INSTRUCTION_MOVNTI + (op - 0xc3);
				else if (op == 0xc7)
				{
					if (modrm.fields.reg == 0b001)
						instruction->id = (uint16_t)(instruction->operandSize64 ? NMD_X86_INSTRUCTION_CMPXCHG16B : NMD_X86_INSTRUCTION_CMPXCHG8B);
					else if (modrm.fields.reg == 0b111)
						instruction->id = (uint16_t)(modrm.fields.mod == 0b11 ? (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_RDPID : NMD_X86_INSTRUCTION_RDSEED) : NMD_X86_INSTRUCTION_VMPTRST);
					else
						instruction->id = (uint16_t)(modrm.fields.mod == 0b11 ? NMD_X86_INSTRUCTION_RDRAND : (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_VMCLEAR : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_VMXON : NMD_X86_INSTRUCTION_VMPTRLD)));
				}
				else if (op >= 0xc8 && op <= 0xcf)
					instruction->id = NMD_X86_INSTRUCTION_BSWAP;
				else if (op == 0xa3)
					instruction->id = (uint16_t)((modrm.fields.mod == 0b11 ? NMD_X86_INSTRUCTION_RDFSBASE : NMD_X86_INSTRUCTION_FXSAVE) + modrm.fields.reg);
				else if (op >= 0xd1 && op <= 0xfe)
				{
					if (op == 0xd6)
						instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVQ : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_MOVQ2DQ : NMD_X86_INSTRUCTION_MOVDQ2Q));
					else if (op == 0xe6)
						instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_CVTTPD2DQ : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_CVTDQ2PD : NMD_X86_INSTRUCTION_CVTPD2DQ));
					else if (op == 0xe7)
						instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVNTDQ : NMD_X86_INSTRUCTION_MOVNTQ);
					else if (op == 0xf7)
						instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MASKMOVDQU : NMD_X86_INSTRUCTION_MASKMOVQ);
					else
						instruction->id = NMD_X86_INSTRUCTION_PSRLW + (op - 0xd1);
				}
				else
				{
					switch (op)
					{
					case 0xa0: case 0xa8: instruction->id = NMD_X86_INSTRUCTION_PUSH; break;
					case 0xa1: case 0xa9: instruction->id = NMD_X86_INSTRUCTION_POP; break;
					case 0xaf: instruction->id = NMD_X86_INSTRUCTION_IMUL; break;
					case 0xb0: case 0xb1: instruction->id = NMD_X86_INSTRUCTION_CMPXCHG; break;
					case 0x10: case 0x11: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVUPD : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_MOVSS : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_MOVSD : NMD_X86_INSTRUCTION_MOVUPD))); break;
					case 0x12: case 0x13: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVLPD : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_MOVSLDUP : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_MOVDDUP : NMD_X86_INSTRUCTION_MOVLPS))); break;
					case 0x14: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_UNPCKLPD : NMD_X86_INSTRUCTION_UNPCKLPS); break;
					case 0x15: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_UNPCKHPD : NMD_X86_INSTRUCTION_UNPCKHPS); break;
					case 0x16: case 0x17: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVHPD : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_MOVSHDUP : NMD_X86_INSTRUCTION_MOVHPS)); break;
					case 0x18: instruction->id = (uint16_t)(modrm.fields.reg >= 0b100 ? NMD_X86_INSTRUCTION_NOP : (modrm.fields.reg == 0b000 ? NMD_X86_INSTRUCTION_PREFETCHNTA : (modrm.fields.reg == 0b001 ? NMD_X86_INSTRUCTION_PREFETCHT0 : (modrm.fields.reg == 0b010 ? NMD_X86_INSTRUCTION_PREFETCHT1 : NMD_X86_INSTRUCTION_PREFETCHT2)))); break;
					case 0x1a: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_BNDMOV : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_BNDCL : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_BNDCU : NMD_X86_INSTRUCTION_BNDLDX))); break;
					case 0x1b: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_BNDMOV : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_BNDMK : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_BNDCN : NMD_X86_INSTRUCTION_BNDSTX))); break;
					case 0x28: case 0x29: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVAPD : NMD_X86_INSTRUCTION_MOVAPS); break;
					case 0x2a: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_CVTPI2PD : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_CVTSI2SS : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_CVTSI2SD : NMD_X86_INSTRUCTION_CVTPI2PS))); break;
					case 0x2b: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVNTPD : NMD_X86_INSTRUCTION_MOVNTPS); break;
					case 0x2c: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_CVTTPD2PI : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_CVTTSS2SI : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_CVTTSS2SI : NMD_X86_INSTRUCTION_CVTTPS2PI))); break;
					case 0x2d: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_CVTPD2PI : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_CVTSS2SI : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_CVTSS2SI : NMD_X86_INSTRUCTION_CVTPS2PI))); break;
					case 0x2e: case 0x2f: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_UCOMISD : NMD_X86_INSTRUCTION_UCOMISS); break;
					case 0x6e: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && !instruction->operandSize64 && (instruction->prefixes & NMD_X86_PREFIXES_REX_W) ? NMD_X86_INSTRUCTION_MOVQ : NMD_X86_INSTRUCTION_MOVD); break;
					case 0x6f: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVDQA : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_MOVDQU : NMD_X86_INSTRUCTION_MOVQ)); break;
					case 0x70: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_PSHUFD : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_PSHUFHW : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_PSHUFLW : NMD_X86_INSTRUCTION_PSHUFW))); break;
					case 0x71: instruction->id = (uint16_t)(modrm.fields.reg == 0b101 ? NMD_X86_INSTRUCTION_PSRLQ : (modrm.fields.reg == 0b100 ? NMD_X86_INSTRUCTION_PSRAW : NMD_X86_INSTRUCTION_PSLLW)); break;
					case 0x72: instruction->id = (uint16_t)(modrm.fields.reg == 0b101 ? NMD_X86_INSTRUCTION_PSRLD : (modrm.fields.reg == 0b100 ? NMD_X86_INSTRUCTION_PSRAD : NMD_X86_INSTRUCTION_PSLLD)); break;
					case 0x73: instruction->id = (uint16_t)(modrm.fields.reg == 0b010 ? NMD_X86_INSTRUCTION_PSRLQ : (modrm.fields.reg == 0b011 ? NMD_X86_INSTRUCTION_PSRLDQ : (modrm.fields.reg == 0b110 ? NMD_X86_INSTRUCTION_PSLLQ : NMD_X86_INSTRUCTION_PSLLDQ))); break;
					case 0x77: instruction->id = NMD_X86_INSTRUCTION_EMMS; break;
					case 0x78: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_EXTRQ : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_INSERTQ : NMD_X86_INSTRUCTION_VMREAD)); break;
					case 0x79: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_EXTRQ : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_INSERTQ : NMD_X86_INSTRUCTION_VMWRITE)); break;
					case 0x7c: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_HADDPD : NMD_X86_INSTRUCTION_HADDPS); break;
					case 0x7d: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_HSUBPD : NMD_X86_INSTRUCTION_HSUBPS); break;
					case 0x7e: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || (instruction->operandSize64 && instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE) ? NMD_X86_INSTRUCTION_MOVQ : NMD_X86_INSTRUCTION_MOVD); break;
					case 0x7f: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_MOVDQA : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_MOVDQU : NMD_X86_INSTRUCTION_MOVQ)); break;
					case 0xa3: instruction->id = NMD_X86_INSTRUCTION_BT; break;
					case 0xa4: case 0xa5: instruction->id = NMD_X86_INSTRUCTION_SHLD; break;
					case 0xaa: instruction->id = NMD_X86_INSTRUCTION_RSM; break;
					case 0xab: instruction->id = NMD_X86_INSTRUCTION_BTS; break;
					case 0xac: case 0xad: instruction->id = NMD_X86_INSTRUCTION_SHRD; break;
					case 0xb6: case 0xb7: instruction->id = NMD_X86_INSTRUCTION_MOVZX; break;
					case 0xb8: instruction->id = NMD_X86_INSTRUCTION_POPCNT; break;
					case 0xb9: instruction->id = NMD_X86_INSTRUCTION_UD1; break;
					case 0xba: instruction->id = (uint16_t)(modrm.fields.reg == 0b100 ? NMD_X86_INSTRUCTION_BT : (modrm.fields.reg == 0b101 ? NMD_X86_INSTRUCTION_BTS : (modrm.fields.reg == 0b110 ? NMD_X86_INSTRUCTION_BTR : NMD_X86_INSTRUCTION_BTC))); break;
					case 0xbb: instruction->id = NMD_X86_INSTRUCTION_BTC; break;
					case 0xbc: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_BSF : NMD_X86_INSTRUCTION_TZCNT); break;
					case 0xbd: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_BSR : NMD_X86_INSTRUCTION_LZCNT); break;
					case 0xbe: case 0xbf: instruction->id = NMD_X86_INSTRUCTION_MOVSX; break;
					case 0xc0: case 0xc1: instruction->id = NMD_X86_INSTRUCTION_XADD; break;
					case 0xc2: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_CMPPD : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? NMD_X86_INSTRUCTION_CMPSS : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? NMD_X86_INSTRUCTION_CMPSD : NMD_X86_INSTRUCTION_CMPPS))); break;
					case 0xd0: instruction->id = (uint16_t)(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_INSTRUCTION_ADDSUBPD : NMD_X86_INSTRUCTION_ADDSUBPS); break;
					case 0xff: instruction->id = NMD_X86_INSTRUCTION_UD0; break;
					}
				}
			}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_CPU_FLAGS
			if (flags & NMD_X86_DECODER_FLAGS_CPU_FLAGS)
			{
				if (NMD_R(op) == 4 || NMD_R(op) == 8 || NMD_R(op) == 9) /* Conditional Move (CMOVcc),Conditional jump(Jcc),Byte set on condition(SETcc) */
					_nmd_decode_conditional_flag(instruction, NMD_C(op));
				else if (op == 0x05 || op == 0x07) /* syscall,sysret */
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_TF | NMD_X86_EFLAGS_IF | NMD_X86_EFLAGS_DF | NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_IOPL | NMD_X86_EFLAGS_AC | NMD_X86_EFLAGS_VIF | NMD_X86_EFLAGS_VIP | NMD_X86_EFLAGS_ID;
				else if (op == 0xaf) /* mul */
				{
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_OF;
					instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF;
				}
				else if (op == 0xb0 || op == 0xb1) /* cmpxchg */
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_OF;
				else if (op == 0xc0 || op == 0xc1) /* xadd */
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_OF;
				else if (op == 0x00 && (modrm.fields.reg == 0b100 || modrm.fields.reg == 0b101)) /* verr,verw*/
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_OF;
				else if (op == 0x01 && modrm.fields.mod == 0b11)
				{
					if (modrm.fields.reg == 0b000)
					{
						if (modrm.fields.rm == 0b001 || modrm.fields.rm == 0b010 || modrm.fields.rm == 0b011) /* vmcall,vmlaunch,vmresume */
						{
							instruction->testedFlags.eflags = NMD_X86_EFLAGS_IOPL | NMD_X86_EFLAGS_VM;
							instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_TF | NMD_X86_EFLAGS_IF | NMD_X86_EFLAGS_DF | NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_IOPL | NMD_X86_EFLAGS_NT | NMD_X86_EFLAGS_RF | NMD_X86_EFLAGS_VM | NMD_X86_EFLAGS_AC | NMD_X86_EFLAGS_VIF | NMD_X86_EFLAGS_VIP | NMD_X86_EFLAGS_ID;
						}
					}
				}
				else if (op == 0x34)
					instruction->clearedFlags.eflags = NMD_X86_EFLAGS_VM | NMD_X86_EFLAGS_IF;
				else if (op == 0x78 || op == 0x79) /* vmread,vmwrite */
				{
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_ZF;
					instruction->clearedFlags.eflags = NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_OF;
				}
				else if (op == 0x02 || op == 0x03) /* lar,lsl */
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_ZF;
				else if (op == 0xa3 || op == 0xab || op == 0xb3 || op == 0xba || op == 0xbb) /* bt,bts,btc */
				{
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF;
					instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_PF;
				}
				else if (op == 0xa4 || op == 0xa5 || op == 0xac || op == 0xad || op == 0xbc) /* shld,shrd */
				{
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF;
					instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_OF;
				}
				else if (op == 0xaa) /* rsm */
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_TF | NMD_X86_EFLAGS_IF | NMD_X86_EFLAGS_DF | NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_IOPL | NMD_X86_EFLAGS_NT | NMD_X86_EFLAGS_RF | NMD_X86_EFLAGS_VM | NMD_X86_EFLAGS_AC | NMD_X86_EFLAGS_VIF | NMD_X86_EFLAGS_VIP | NMD_X86_EFLAGS_ID;
				else if ((op == 0xbc || op == 0xbd) && instruction->prefixes & NMD_X86_PREFIXES_REPEAT) /* tzcnt */
				{
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_ZF;
					instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_OF;
				}
				else if (op == 0xbc || op == 0xbd) /* bsf */
				{
					instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_ZF;
					instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_OF;
				}
			}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_CPU_FLAGS */

			if (NMD_R(op) == 8) /* imm32 */
				instruction->immMask = (uint8_t)(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_IMM16 : NMD_X86_IMM32);
			else if ((NMD_R(op) == 7 && NMD_C(op) < 4) || op == 0xA4 || op == 0xC2 || (op > 0xC3 && op <= 0xC6) || op == 0xBA || op == 0xAC) /* imm8 */
				instruction->immMask = NMD_X86_IMM8;
			else if (op == 0x78 && (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO || instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) /* imm8 + imm8 = "imm16" */
				instruction->immMask = NMD_X86_IMM16;

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_GROUP
			/* Parse the instruction's group. */
			if (flags & NMD_X86_DECODER_FLAGS_GROUP)
			{
				if (NMD_R(op) == 8)
					instruction->group = NMD_GROUP_JUMP | NMD_GROUP_CONDITIONAL_BRANCH | NMD_GROUP_RELATIVE_ADDRESSING;
				else if ((op == 0x01 && modrm.fields.rm == 0b111 && (modrm.fields.mod == 0b00 || modrm.modrm == 0xf8)) || op == 0x06 || op == 0x08 || op == 0x09)
					instruction->group = NMD_GROUP_PRIVILEGE;
				else if (op == 0x05)
					instruction->group = NMD_GROUP_INT;
			}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_GROUP */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS
			if (flags & NMD_X86_DECODER_FLAGS_OPERANDS)
			{
				if (op == 0x2 || op == 0x3 || (op >= 0x10 && op <= 0x17) || NMD_R(op) == 2 || (NMD_R(op) >= 4 && NMD_R(op) <= 7) || op == 0xa3 || op == 0xab || op == 0xaf || (NMD_R(op) >= 0xc && op != 0xc7 && op != 0xff))
					instruction->numOperands = 2;
				else if (NMD_R(op) == 8 || NMD_R(op) == 9 || (NMD_R(op) == 0xa && op % 8 < 2) || op == 0xc7)
					instruction->numOperands = 1;
				else if (op == 0xa4 || op == 0xa5 || op == 0xc2 || (op >= 0xc4 && op <= 0xc6))
					instruction->numOperands = 3;

				if (instruction->numOperands > 0)
				{
					if (op == 0x00)
					{
						if (instruction->modrm.fields.reg >= 0b010)
							_nmd_decode_operand_Ew(instruction, &instruction->operands[0]);
						else
							_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);

						instruction->operands[0].action = (uint8_t)(instruction->modrm.fields.reg >= 0b010 ? NMD_X86_OPERAND_ACTION_READ : NMD_X86_OPERAND_ACTION_WRITE);
					}
					else if (op == 0x01)
					{
						if (instruction->modrm.fields.mod != 0b11)
						{
							_nmd_decode_modrm_upper32(instruction, &instruction->operands[0]);
							instruction->operands[0].action = (uint8_t)(instruction->modrm.fields.reg >= 0b010 ? NMD_X86_OPERAND_ACTION_READ : NMD_X86_OPERAND_ACTION_WRITE);
						}
						else if (instruction->modrm.fields.reg == 0b100)
							_nmd_decode_operand_Rv(instruction, &instruction->operands[0]);
						else if (instruction->modrm.fields.reg == 0b110)
						{
							_nmd_decode_operand_Ew(instruction, &instruction->operands[0]);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ;
						}

						if (instruction->modrm.fields.reg == 0b100)
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
					}
					else if (op == 0x02 || op == 0x03)
					{
						_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Ew(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0x0d)
					{
						_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (NMD_R(op) == 0x8)
					{
						instruction->operands[0].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
					}
					else if (NMD_R(op) == 9)
					{
						_nmd_decode_operand_Eb(instruction, &instruction->operands[0]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
					}
					else if (op == 0x17)
					{
						_nmd_decode_modrm_upper32(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op >= 0x20 && op <= 0x23)
					{
						instruction->operands[0].type = instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
						instruction->operands[op < 0x22 ? 0 : 1].fields.reg = NMD_X86_REG_EAX + instruction->modrm.fields.rm;
						instruction->operands[op < 0x22 ? 1 : 0].fields.reg = (uint8_t)((op % 2 == 0 ? NMD_X86_REG_CR0 : NMD_X86_REG_DR0) + instruction->modrm.fields.reg);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0x29 || op == 0x2b || (op == 0x7f && instruction->simdPrefix))
					{
						_nmd_decode_operand_Wdq(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0x2a || op == 0x2c || op == 0x2d)
					{
						if (op == 0x2a)
							_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
						else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
							_nmd_decode_operand_Gy(instruction, &instruction->operands[0]);
						else if (op == 0x2d && instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
							_nmd_decode_operand_Qq(instruction, &instruction->operands[0]);
						else
							_nmd_decode_operand_Pq(instruction, &instruction->operands[0]);

						if (op == 0x2a)
						{
							if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT || instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
								_nmd_decode_operand_Ey(instruction, &instruction->operands[1]);
							else
								_nmd_decode_operand_Qq(instruction, &instruction->operands[1]);
						}
						else
							_nmd_decode_operand_Wdq(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0x50)
					{
						_nmd_decode_operand_Gy(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Udq(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (NMD_R(op) == 5 || (op >= 0x10 && op <= 0x16) || op == 0x28 || op == 0x2e || op == 0x2f || (op == 0x7e && instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT))
					{
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[op == 0x11 || op == 0x13 ? 1 : 0]);
						_nmd_decode_operand_Wdq(instruction, &instruction->operands[op == 0x11 || op == 0x13 ? 0 : 1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0x7e)
					{
						_nmd_decode_operand_Ey(instruction, &instruction->operands[0]);
						instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
						instruction->operands[1].size = 1;
						instruction->operands[1].fields.reg = (uint8_t)((instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_XMM0 : NMD_X86_REG_MM0) + instruction->modrm.fields.reg);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (NMD_R(op) == 6 || op == 0x70 || (op >= 0x74 && op <= 0x76) || (op >= 0x7c && op <= 0x7f))
					{
						if (!instruction->simdPrefix)
						{
							_nmd_decode_operand_Pq(instruction, &instruction->operands[op == 0x7f ? 1 : 0]);

							if (op == 0x6e)
								_nmd_decode_operand_Ey(instruction, &instruction->operands[1]);
							else
								_nmd_decode_operand_Qq(instruction, &instruction->operands[op == 0x7f ? 0 : 1]);
						}
						else
						{
							_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);

							if (op == 0x6e)
								_nmd_decode_operand_Ey(instruction, &instruction->operands[1]);
							else
								_nmd_decode_operand_Wdq(instruction, &instruction->operands[1]);
						}

						if (op == 0x70)
							instruction->operands[2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;

						instruction->operands[0].action = (uint8_t)(((op >= 0x60 && op <= 0x6d) || (op >= 0x74 && op <= 0x76)) ? NMD_X86_OPERAND_ACTION_READ_WRITE : NMD_X86_OPERAND_ACTION_WRITE);
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op >= 0x71 && op <= 0x73)
					{
						if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
							_nmd_decode_operand_Udq(instruction, &instruction->operands[0]);
						else
							_nmd_decode_operand_Qq(instruction, &instruction->operands[0]);
						instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0x78 || op == 0x79)
					{
						if (instruction->simdPrefix)
						{
							if (op == 0x78)
							{
								i = 0;
								if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
									_nmd_decode_operand_Vdq(instruction, &instruction->operands[i++]);
								_nmd_decode_operand_Udq(instruction, &instruction->operands[i + 0]);
								instruction->operands[i + 1].type = instruction->operands[i + 2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
								instruction->operands[i + 1].size = instruction->operands[i + 2].size = 1;
								instruction->operands[i + 1].fields.imm = b[1];
								instruction->operands[i + 2].fields.imm = b[2];
							}
							else
							{
								_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
								_nmd_decode_operand_Wdq(instruction, &instruction->operands[1]);
							}
						}
						else
						{
							_nmd_decode_operand_Ey(instruction, &instruction->operands[op == 0x78 ? 0 : 1]);
							_nmd_decode_operand_Gy(instruction, &instruction->operands[op == 0x78 ? 1 : 0]);
						}
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (NMD_R(op) == 0xa && (op % 8) < 2)
					{
						instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
						instruction->operands[0].fields.reg = (uint8_t)(op > 0xa8 ? NMD_X86_REG_GS : NMD_X86_REG_FS);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if ((NMD_R(op) == 0xa && ((op % 8) >= 3 && (op % 8) <= 5)) || op == 0xb3 || op == 0xbb)
					{
						_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Gv(instruction, &instruction->operands[1]);

						if (NMD_R(op) == 0xa)
						{
							if ((op % 8) == 4)
								instruction->operands[2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
							else if ((op % 8) == 5)
							{
								instruction->operands[2].type = NMD_X86_OPERAND_TYPE_REGISTER;
								instruction->operands[2].fields.reg = NMD_X86_REG_CL;
							}
						}

						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0xaf || op == 0xb8)
					{
						_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Ev(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0xba)
					{
						_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
						instruction->operands[0].action = (uint8_t)(instruction->modrm.fields.reg <= 0b101 ? NMD_X86_OPERAND_ACTION_READ : NMD_X86_OPERAND_ACTION_READ_WRITE);
						instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (NMD_R(op) == 0xb && (op % 8) >= 6)
					{
						_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
						if ((op % 8) == 6)
							_nmd_decode_operand_Eb(instruction, &instruction->operands[1]);
						else
							_nmd_decode_operand_Ew(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (NMD_R(op) == 0x4 || (NMD_R(op) == 0xb && ((op % 8) == 0x4 || (op % 8) == 0x5)))
					{
						_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Ev(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if ((NMD_R(op) == 0xb || NMD_R(op) == 0xc) && NMD_C(op) < 2)
					{
						if (NMD_C(op) == 0)
						{
							_nmd_decode_operand_Eb(instruction, &instruction->operands[0]);
							_nmd_decode_operand_Gb(instruction, &instruction->operands[1]);
						}
						else
						{
							_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
							_nmd_decode_operand_Gv(instruction, &instruction->operands[1]);
						}

						if (NMD_R(op) == 0xb)
						{
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ | NMD_X86_OPERAND_ACTION_CONDITIONAL_WRITE;
							instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
						}
						else
							instruction->operands[0].action = instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
					}
					else if (op == 0xb2)
					{
						_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
						_nmd_decode_modrm_upper32(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0xc3)
					{
						_nmd_decode_modrm_upper32(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Gy(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
					else if (op == 0xc2 || op == 0xc6)
					{
						_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Wdq(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
						instruction->operands[2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
					}
					else if (op == 0xc4)
					{
						if (instruction->prefixes == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
							_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
						else
							_nmd_decode_operand_Pq(instruction, &instruction->operands[0]);
						_nmd_decode_operand_Ey(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
						instruction->operands[2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
					}
					else if (op == 0xc5)
					{
						_nmd_decode_operand_Gd(instruction, &instruction->operands[0]);
						if (instruction->prefixes == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
							_nmd_decode_operand_Udq(instruction, &instruction->operands[1]);
						else
							_nmd_decode_operand_Nq(instruction, &instruction->operands[1]);
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
						instruction->operands[2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
					}
					else if (op == 0xc7)
					{
						if (instruction->modrm.fields.mod == 0b11)
							_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
						else
							_nmd_decode_modrm_upper32(instruction, &instruction->operands[0]);
						instruction->operands[0].action = (uint8_t)(instruction->modrm.fields.reg == 0b001 ? (NMD_X86_OPERAND_ACTION_READ | NMD_X86_OPERAND_ACTION_CONDITIONAL_WRITE) : (instruction->modrm.fields.mod == 0b11 || !instruction->simdPrefix ? NMD_X86_OPERAND_ACTION_WRITE : NMD_X86_OPERAND_ACTION_READ));
					}
					else if (op >= 0xc8 && op <= 0xcf)
					{
						instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
						instruction->operands[0].fields.reg = (uint8_t)((instruction->prefixes & (NMD_X86_PREFIXES_REX_W | NMD_X86_PREFIXES_REX_B)) == (NMD_X86_PREFIXES_REX_W | NMD_X86_PREFIXES_REX_B) ? NMD_X86_REG_R8 : (instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_REG_RAX : (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? NMD_X86_REG_R8D : NMD_X86_REG_EAX)) + (op % 8));
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
					}
					else if (NMD_R(op) >= 0xd)
					{
						if (op == 0xff)
						{
							_nmd_decode_operand_Gd(instruction, &instruction->operands[0]);
							_nmd_decode_memory_operand(instruction, &instruction->operands[1], NMD_X86_REG_EAX);
						}
						else if (op == 0xd6 && instruction->simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
						{
							if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
							{
								_nmd_decode_operand_Vdq(instruction, &instruction->operands[0]);
								_nmd_decode_operand_Qq(instruction, &instruction->operands[1]);
							}
							else
							{
								_nmd_decode_operand_Pq(instruction, &instruction->operands[0]);
								_nmd_decode_operand_Wdq(instruction, &instruction->operands[1]);
							}
						}
						else
						{
							const size_t opIndex1 = op == 0xe7 || op == 0xd6 ? 1 : 0;
							const size_t opIndex2 = op == 0xe7 || op == 0xd6 ? 0 : 1;

							if (!instruction->simdPrefix)
							{
								if (op == 0xd7)
									_nmd_decode_operand_Gd(instruction, &instruction->operands[0]);
								else
									_nmd_decode_operand_Pq(instruction, &instruction->operands[opIndex1]);
								_nmd_decode_operand_Qq(instruction, &instruction->operands[opIndex2]);
							}
							else
							{
								if (op == 0xd7)
									_nmd_decode_operand_Gd(instruction, &instruction->operands[0]);
								else
									_nmd_decode_operand_Vdq(instruction, &instruction->operands[opIndex1]);
								_nmd_decode_operand_Wdq(instruction, &instruction->operands[opIndex2]);
							}
						}
						instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
					}
				}
			}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS */
		}
	}
	else /* 1 byte opcode */
	{
		instruction->opcodeSize = 1;
		instruction->opcode = *b;
		instruction->opcodeMap = NMD_X86_OPCODE_MAP_DEFAULT;

		op = instruction->opcode;

		/* Check for ModR/M, SIB and displacement. */
		if (NMD_R(op) == 8 || _nmd_findByte(_nmd_op1modrm, sizeof(_nmd_op1modrm), op) || (NMD_R(op) < 4 && (NMD_C(op) < 4 || (NMD_C(op) >= 8 && NMD_C(op) < 0xC))) || (NMD_R(op) == 0xD && NMD_C(op) >= 8) || (remainingSize > 1 && ((nmd_x86_modrm*)(b + 1))->fields.mod != 0b11 && (op == 0xc4 || op == 0xc5 || op == 0x62)))
		{
			if (!_nmd_decode_modrm(&b, instruction, remainingSize - 1))
				return false;
		}

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_EVEX
		/* Check if instruction is EVEX. */
		if (flags & NMD_X86_DECODER_FLAGS_EVEX && op == 0x62 && !instruction->hasModrm)
		{
			instruction->encoding = NMD_X86_ENCODING_EVEX;
		}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_EVEX */
#if !defined(NMD_ASSEMBLY_DISABLE_DECODER_EVEX) && !defined(NMD_ASSEMBLY_DISABLE_DECODER_VEX)
		else
#endif
#ifndef NMD_ASSEMBLY_DISABLE_DECODER_VEX
			/* Check if instruction is VEX. */
			if (flags & NMD_X86_DECODER_FLAGS_VEX && (op == 0xc4 || op == 0xc5) && !instruction->hasModrm)
			{
				instruction->encoding = NMD_X86_ENCODING_VEX;

				instruction->vex.vex[0] = op;
				if (remainingSize < 4)
					return false;

				const uint8_t byte1 = *++b;

				instruction->vex.R = byte1 & 0b10000000;
				if (instruction->vex.vex[0] == 0xc4)
				{
					instruction->vex.X = (byte1 & 0b01000000) == 0b01000000;
					instruction->vex.B = (byte1 & 0b00100000) == 0b00100000;
					instruction->vex.m_mmmm = (uint8_t)(byte1 & 0b00011111);

					const uint8_t byte2 = *++b;
					instruction->vex.W = (byte2 & 0b10000000) == 0b10000000;
					instruction->vex.vvvv = (uint8_t)((byte2 & 0b01111000) >> 3);
					instruction->vex.L = (byte2 & 0b00000100) == 0b00000100;
					instruction->vex.pp = (uint8_t)(byte2 & 0b00000011);

					instruction->opcode = *++b;
					op = instruction->opcode;

					if (op == 0x0c || op == 0x0d || op == 0x40 || op == 0x41 || op == 0x17 || op == 0x21 || op == 0x42)
						instruction->immMask = NMD_X86_IMM8;

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK
					/* Check if the instruction is invalid. */
					if (op == 0x0c && instruction->vex.m_mmmm != 3)
						return false;
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK */


#ifndef NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID
					/*if(op == 0x0c)
						instruction->id = NMD_X86_INSTR
						*/
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID */
				}
				else /* 0xc5 */
				{
					instruction->vex.vvvv = (uint8_t)(byte1 & 0b01111000);
					instruction->vex.L = byte1 & 0b00000100;
					instruction->vex.pp = (uint8_t)(byte1 & 0b00000011);

					b++;
					instruction->opcode = *b;
					op = instruction->opcode;
				}

				if (!_nmd_decode_modrm(&b, instruction, remainingSize - (instruction->vex.vex[0] == 0xc4 ? 4 : 3)))
					return false;
			}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_VEX */
#if !(defined(NMD_ASSEMBLY_DISABLE_DECODER_EVEX) && defined(NMD_ASSEMBLY_DISABLE_DECODER_VEX))
			else
#endif
			{
				const nmd_x86_modrm modrm = instruction->modrm;
#ifndef NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK
				/* Check if the instruction is invalid. */
				if (flags & NMD_X86_DECODER_FLAGS_VALIDITY_CHECK)
				{
					if (op == 0xC6 || op == 0xC7)
					{
						if ((modrm.fields.reg != 0b000 && modrm.fields.reg != 0b111) || (modrm.fields.reg == 0b111 && (modrm.fields.mod != 0b11 || modrm.fields.rm != 0b000)))
							return false;
					}
					else if (op == 0x8f)
					{
						if (modrm.fields.reg != 0b000)
							return false;
					}
					else if (op == 0xfe)
					{
						if (modrm.fields.reg >= 0b010)
							return false;
					}
					else if (op == 0xff)
					{
						if (modrm.fields.reg == 0b111 || (modrm.fields.mod == 0b11 && (modrm.fields.reg == 0b011 || modrm.fields.reg == 0b101)))
							return false;
					}
					else if (op == 0x8c)
					{
						if (modrm.fields.reg >= 0b110)
							return false;
					}
					else if (op == 0x8e)
					{
						if (modrm.fields.reg == 0b001 || modrm.fields.reg >= 0b110)
							return false;
					}
					else if (op == 0x62)
					{
						if (mode == NMD_X86_MODE_64)
							return false;
					}
					else if (op == 0x8d)
					{
						if (modrm.fields.mod == 0b11)
							return false;
					}
					else if (op == 0xc4 || op == 0xc5)
					{
						if (mode == NMD_X86_MODE_64 && instruction->hasModrm && modrm.fields.mod != 0b11)
							return false;
					}
					else if (op >= 0xd8 && op <= 0xdf)
					{
						switch (op)
						{
						case 0xd9:
							if ((modrm.fields.reg == 0b001 && modrm.fields.mod != 0b11) || (modrm.modrm > 0xd0 && modrm.modrm < 0xd8) || modrm.modrm == 0xe2 || modrm.modrm == 0xe3 || modrm.modrm == 0xe6 || modrm.modrm == 0xe7 || modrm.modrm == 0xef)
								return false;
							break;
						case 0xda:
							if (modrm.modrm >= 0xe0 && modrm.modrm != 0xe9)
								return false;
							break;
						case 0xdb:
							if (((modrm.fields.reg == 0b100 || modrm.fields.reg == 0b110) && modrm.fields.mod != 0b11) || (modrm.modrm >= 0xe5 && modrm.modrm <= 0xe7) || modrm.modrm >= 0xf8)
								return false;
							break;
						case 0xdd:
							if ((modrm.fields.reg == 0b101 && modrm.fields.mod != 0b11) || NMD_R(modrm.modrm) == 0xf)
								return false;
							break;
						case 0xde:
							if (modrm.modrm == 0xd8 || (modrm.modrm >= 0xda && modrm.modrm <= 0xdf))
								return false;
							break;
						case 0xdf:
							if ((modrm.modrm >= 0xe1 && modrm.modrm <= 0xe7) || modrm.modrm >= 0xf8)
								return false;
							break;
						}
					}
					else if (mode == NMD_X86_MODE_64)
					{
						if (op == 0x6 || op == 0x7 || op == 0xe || op == 0x16 || op == 0x17 || op == 0x1e || op == 0x1f || op == 0x27 || op == 0x2f || op == 0x37 || op == 0x3f || (op >= 0x60 && op <= 0x62) || op == 0x82 || op == 0xce || (op >= 0xd4 && op <= 0xd6))
							return false;
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_VALIDITY_CHECK */

				/* Check for immediate */		
				if (_nmd_findByte(_nmd_op1imm32, sizeof(_nmd_op1imm32), op) || (NMD_R(op) < 4 && (NMD_C(op) == 5 || NMD_C(op) == 0xD)) || (NMD_R(op) == 0xB && NMD_C(op) >= 8) || (op == 0xF7 && modrm.fields.reg == 0b000)) /* imm32,16 */
				{
					if (NMD_R(op) == 0xB && NMD_C(op) >= 8)
						instruction->immMask = (uint8_t)(instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_IMM64 : (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || (mode == NMD_X86_MODE_16 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? NMD_X86_IMM16 : NMD_X86_IMM32));
					else
					{
						if (mode == NMD_X86_MODE_16 && instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || mode != NMD_X86_MODE_16 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE))
							instruction->immMask = NMD_X86_IMM32;
						else
							instruction->immMask = NMD_X86_IMM16;
					}
				}
				else if (NMD_R(op) == 7 || (NMD_R(op) == 0xE && NMD_C(op) < 8) || (NMD_R(op) == 0xB && NMD_C(op) < 8) || (NMD_R(op) < 4 && (NMD_C(op) == 4 || NMD_C(op) == 0xC)) || (op == 0xF6 && modrm.fields.reg <= 0b001) || _nmd_findByte(_nmd_op1imm8, sizeof(_nmd_op1imm8), op)) /* imm8 */
					instruction->immMask = NMD_X86_IMM8;
				else if (NMD_R(op) == 0xA && NMD_C(op) < 4)
					instruction->immMask = (uint8_t)(mode == NMD_X86_MODE_64 ? (instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE ? NMD_X86_IMM32 : NMD_X86_IMM64) : (instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE ? NMD_X86_IMM16 : NMD_X86_IMM32));
				else if (op == 0xEA || op == 0x9A) /* imm32,48 */
				{
					if (mode == NMD_X86_MODE_64)
						return false;
					instruction->immMask = (uint8_t)(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_IMM32 : NMD_X86_IMM48);
				}
				else if (op == 0xC2 || op == 0xCA) /* imm16 */
					instruction->immMask = NMD_X86_IMM16;
				else if (op == 0xC8) /* imm16 + imm8 */
					instruction->immMask = NMD_X86_IMM16 | NMD_X86_IMM8;
							

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID
				if (flags & NMD_X86_DECODER_FLAGS_INSTRUCTION_ID)
				{
					const bool operandSize = instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE;
					if ((op >= 0x88 && op <= 0x8c) || (op >= 0xa0 && op <= 0xa3) || NMD_R(op) == 0xb || op == 0x8e)
						instruction->id = NMD_X86_INSTRUCTION_MOV;
					else if (NMD_R(op) == 5)
						instruction->id = (uint16_t)((NMD_C(op) < 8) ? NMD_X86_INSTRUCTION_PUSH : NMD_X86_INSTRUCTION_POP);
					else if (NMD_R(op) < 4 && (op % 8 < 6))
						instruction->id = (NMD_X86_INSTRUCTION_ADD + (NMD_R(op) << 1) + (NMD_C(op) >= 8 ? 1 : 0));
					else if (op >= 0x80 && op <= 0x84)
						instruction->id = NMD_X86_INSTRUCTION_ADD + modrm.fields.reg;
					else if (op == 0xe8)
						instruction->id = NMD_X86_INSTRUCTION_CALL;
					else if (op == 0xcc)
						instruction->id = NMD_X86_INSTRUCTION_INT3;
					else if (op == 0x8d)
						instruction->id = NMD_X86_INSTRUCTION_LEA;
					else if (NMD_R(op) == 4)
						instruction->id = (uint16_t)((NMD_C(op) < 8) ? NMD_X86_INSTRUCTION_INC : NMD_X86_INSTRUCTION_DEC);
					else if (NMD_R(op) == 7)
						instruction->id = NMD_X86_INSTRUCTION_JO + NMD_C(op);
					else if (op == 0xff)
						instruction->id = NMD_X86_INSTRUCTION_INC + modrm.fields.reg;
					else if (op == 0xeb || op == 0xe9)
						instruction->id = NMD_X86_INSTRUCTION_JMP;
					else if (op == 0x90)
					{
						if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT)
							instruction->id = NMD_X86_INSTRUCTION_PAUSE;
						else if (instruction->prefixes & NMD_X86_PREFIXES_REX_B)
							instruction->id = NMD_X86_INSTRUCTION_XCHG;
						else
							instruction->id = NMD_X86_INSTRUCTION_NOP;
					}
					else if (op == 0xc3 || op == 0xc2)
						instruction->id = NMD_X86_INSTRUCTION_RET;
					else if ((op >= 0x91 && op <= 0x97) || op == 0x86 || op == 0x87)
						instruction->id = NMD_X86_INSTRUCTION_XCHG;
					else if (op == 0xc0 || op == 0xc1 || (op >= 0xd0 && op <= 0xd3))
						instruction->id = NMD_X86_INSTRUCTION_ROL + modrm.fields.reg;
					else if (NMD_R(op) == 0x0f && (op % 8 < 6))
						instruction->id = NMD_X86_INSTRUCTION_INT1 + (op - 0xf1);
					else if (op >= 0xd4 && op <= 0xd7)
						instruction->id = NMD_X86_INSTRUCTION_AAM + (op - 0xd4);
					else if (op >= 0xe0 && op <= 0xe3)
						instruction->id = NMD_X86_INSTRUCTION_LOOPNE + (op - 0xe0);
					else /* case 0x: instruction->id = NMD_X86_INSTRUCTION_; break; */
					{
						switch (op)
						{
						case 0x8f: instruction->id = NMD_X86_INSTRUCTION_POP; break;
						case 0xfe: instruction->id = (uint16_t)(modrm.fields.reg == 0b000 ? NMD_X86_INSTRUCTION_INC : NMD_X86_INSTRUCTION_DEC); break;
						case 0x84: case 0x85: case 0xa8: case 0xa9: instruction->id = NMD_X86_INSTRUCTION_TEST; break;
						case 0xf6: case 0xf7: instruction->id = NMD_X86_INSTRUCTION_TEST + modrm.fields.reg; break;
						case 0x69: case 0x6b: instruction->id = NMD_X86_INSTRUCTION_IMUL; break;
						case 0x9a: instruction->id = NMD_X86_INSTRUCTION_CALL; break;
						case 0x62: instruction->id = NMD_X86_INSTRUCTION_BOUND; break;
						case 0x63: instruction->id = (uint16_t)(mode == NMD_X86_MODE_64 ? NMD_X86_INSTRUCTION_MOVSXD : NMD_X86_INSTRUCTION_ARPL); break;
						case 0x68: case 0x6a: case 0x06: case 0x16: case 0x1e: case 0x0e: instruction->id = NMD_X86_INSTRUCTION_PUSH; break;
						case 0x6c: instruction->id = NMD_X86_INSTRUCTION_INSB; break;
						case 0x6d: instruction->id = (uint16_t)(operandSize ? NMD_X86_INSTRUCTION_INSW : NMD_X86_INSTRUCTION_INSD); break;
						case 0x6e: instruction->id = NMD_X86_INSTRUCTION_OUTSB; break;
						case 0x6f: instruction->id = (uint16_t)(operandSize ? NMD_X86_INSTRUCTION_OUTSW : NMD_X86_INSTRUCTION_OUTSD); break;
						case 0xc2: case 0xc3:; break;
						case 0xc4: instruction->id = NMD_X86_INSTRUCTION_LES; break;
						case 0xc5: instruction->id = NMD_X86_INSTRUCTION_LDS; break;
						case 0xc6: case 0xc7: instruction->id = (uint16_t)(modrm.fields.reg == 0b000 ? NMD_X86_INSTRUCTION_MOV : (instruction->opcode == 0xc6 ? NMD_X86_INSTRUCTION_XABORT : NMD_X86_INSTRUCTION_XBEGIN)); break;
						case 0xc8: instruction->id = NMD_X86_INSTRUCTION_ENTER; break;
						case 0xc9: instruction->id = NMD_X86_INSTRUCTION_LEAVE; break;
						case 0xca: case 0xcb: instruction->id = NMD_X86_INSTRUCTION_RETF; break;
						case 0xcd: instruction->id = NMD_X86_INSTRUCTION_INT; break;
						case 0xce: instruction->id = NMD_X86_INSTRUCTION_INTO; break;
						case 0xcf: 
							if (instruction->operandSize64)
								instruction->id = NMD_X86_INSTRUCTION_IRETQ;
							else if (mode == NMD_X86_MODE_16)
								instruction->id = (uint16_t)(operandSize ? NMD_X86_INSTRUCTION_IRETD : NMD_X86_INSTRUCTION_IRET);
							else
								instruction->id = (uint16_t)(operandSize ? NMD_X86_INSTRUCTION_IRET : NMD_X86_INSTRUCTION_IRETD);
							break;
						case 0xe4: case 0xe5: case 0xec: case 0xed: instruction->id = NMD_X86_INSTRUCTION_IN; break;
						case 0xe6: case 0xe7: case 0xee: case 0xef: instruction->id = NMD_X86_INSTRUCTION_OUT; break;
						case 0xea: instruction->id = NMD_X86_INSTRUCTION_LJMP; break;
						case 0x9c:
							if (operandSize)
								instruction->id = (uint16_t)(mode == NMD_X86_MODE_16 ? NMD_X86_INSTRUCTION_PUSHFD : NMD_X86_INSTRUCTION_PUSHF);
							else
								instruction->id = (uint16_t)(mode == NMD_X86_MODE_16 ? NMD_X86_INSTRUCTION_PUSHF : (mode == NMD_X86_MODE_32 ? NMD_X86_INSTRUCTION_PUSHFD : NMD_X86_INSTRUCTION_PUSHFQ));
							break;
						case 0x9d:
							if (operandSize)
								instruction->id = (uint16_t)(mode == NMD_X86_MODE_16 ? NMD_X86_INSTRUCTION_POPFD : NMD_X86_INSTRUCTION_POPF);
							else
								instruction->id = (uint16_t)(mode == NMD_X86_MODE_16 ? NMD_X86_INSTRUCTION_POPF : (mode == NMD_X86_MODE_32 ? NMD_X86_INSTRUCTION_POPFD : NMD_X86_INSTRUCTION_POPFQ));
							break;
						case 0x60:
						case 0x61:
							instruction->id = (uint16_t)(operandSize ? (instruction->opcode == 0x60 ? NMD_X86_INSTRUCTION_PUSHA : NMD_X86_INSTRUCTION_POPA) : (instruction->opcode == 0x60 ? NMD_X86_INSTRUCTION_PUSHAD : NMD_X86_INSTRUCTION_POPAD));
							break;
						case 0x07: case 0x17: case 0x1f: instruction->id = NMD_X86_INSTRUCTION_POP; break;
						case 0x27: instruction->id = NMD_X86_INSTRUCTION_DAA; break;
						case 0x37: instruction->id = NMD_X86_INSTRUCTION_AAA; break;
						case 0x2f: instruction->id = NMD_X86_INSTRUCTION_DAS; break;
						case 0x3f: instruction->id = NMD_X86_INSTRUCTION_AAS; break;
						case 0x9b: instruction->id = NMD_X86_INSTRUCTION_FWAIT; break;
						case 0x9e: instruction->id = NMD_X86_INSTRUCTION_SAHF; break;
						case 0x9f: instruction->id = NMD_X86_INSTRUCTION_LAHF; break;
						case 0xA4: instruction->id = NMD_X86_INSTRUCTION_MOVSB; break;
						case 0xA5: instruction->id = (uint16_t)(instruction->operandSize64 ? NMD_X86_INSTRUCTION_MOVSQ : (operandSize ? NMD_X86_INSTRUCTION_MOVSW : NMD_X86_INSTRUCTION_MOVSD)); break;
						case 0xA6: instruction->id = NMD_X86_INSTRUCTION_CMPSB; break;
						case 0xA7: instruction->id = (uint16_t)(instruction->operandSize64 ? NMD_X86_INSTRUCTION_CMPSQ : (operandSize ? NMD_X86_INSTRUCTION_CMPSW : NMD_X86_INSTRUCTION_CMPSD)); break;
						case 0xAA: instruction->id = NMD_X86_INSTRUCTION_STOSB; break;
						case 0xAB: instruction->id = (uint16_t)(instruction->operandSize64 ? NMD_X86_INSTRUCTION_STOSQ : (operandSize ? NMD_X86_INSTRUCTION_STOSW : NMD_X86_INSTRUCTION_STOSD)); break;
						case 0xAC: instruction->id = NMD_X86_INSTRUCTION_LODSB; break;
						case 0xAD: instruction->id = (uint16_t)(instruction->operandSize64 ? NMD_X86_INSTRUCTION_LODSQ : (operandSize ? NMD_X86_INSTRUCTION_LODSW : NMD_X86_INSTRUCTION_LODSD)); break;
						case 0xAE: instruction->id = NMD_X86_INSTRUCTION_SCASB; break;
						case 0xAF: instruction->id = (uint16_t)(instruction->operandSize64 ? NMD_X86_INSTRUCTION_SCASQ : (operandSize ? NMD_X86_INSTRUCTION_SCASW : NMD_X86_INSTRUCTION_SCASD)); break;
						case 0x98: instruction->id = (uint16_t)(instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_INSTRUCTION_CDQE : (operandSize ? NMD_X86_INSTRUCTION_CBW : NMD_X86_INSTRUCTION_CWDE)); break;
						case 0x99: instruction->id = (uint16_t)(instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_INSTRUCTION_CQO : (operandSize ? NMD_X86_INSTRUCTION_CWD : NMD_X86_INSTRUCTION_CDQ)); break;
						case 0xd6: instruction->id = NMD_X86_INSTRUCTION_SALC; break;

							/* Floating-point opcodes. */
#define _NMD_F_OP_GET_OFFSET() ((NMD_R(modrm.modrm) - 0xc) << 1) + (NMD_C(op) >= 8 ? 1 : 0)
						case 0xd8: instruction->id = (NMD_X86_INSTRUCTION_FADD + (modrm.fields.mod == 0b11 ? _NMD_F_OP_GET_OFFSET() : modrm.fields.reg)); break;
						case 0xd9:
							if (modrm.fields.mod == 0b11)
							{
								if (modrm.modrm <= 0xcf)
									instruction->id = (uint16_t)(modrm.modrm <= 0xc7 ? NMD_X86_INSTRUCTION_FLD : NMD_X86_INSTRUCTION_FXCH);
								else if (modrm.modrm >= 0xd8 && modrm.modrm <= 0xdf)
									instruction->id = NMD_X86_INSTRUCTION_FSTPNCE;
								else if (modrm.modrm == 0xd0)
									instruction->id = NMD_X86_INSTRUCTION_FNOP;
								else
									instruction->id = NMD_X86_INSTRUCTION_FCHS + (modrm.modrm - 0xe0);
							}
							else
								instruction->id = NMD_X86_INSTRUCTION_FLD + modrm.fields.reg;
							break;
						case 0xda:
							if (modrm.fields.mod == 0b11)
								instruction->id = ((modrm.modrm == 0xe9) ? NMD_X86_INSTRUCTION_FUCOMPP : NMD_X86_INSTRUCTION_FCMOVB + _NMD_F_OP_GET_OFFSET());
							else
								instruction->id = NMD_X86_INSTRUCTION_FIADD + modrm.fields.reg;
							break;
						case 0xdb:
							if (modrm.fields.mod == 0b11)
								instruction->id = (modrm.modrm == 0xe2 ? NMD_X86_INSTRUCTION_FNCLEX : (modrm.modrm == 0xe2 ? NMD_X86_INSTRUCTION_FNINIT : NMD_X86_INSTRUCTION_FCMOVNB + _NMD_F_OP_GET_OFFSET()));
							else
								instruction->id = (modrm.fields.reg == 0b101 ? NMD_X86_INSTRUCTION_FLD : (modrm.fields.reg == 0b111 ? NMD_X86_INSTRUCTION_FSTP : NMD_X86_INSTRUCTION_FILD + modrm.fields.reg));
							break;
						case 0xdc:
							if (modrm.fields.mod == 0b11)
								instruction->id = (NMD_X86_INSTRUCTION_FADD + ((NMD_R(modrm.modrm) - 0xc) << 1) + ((NMD_C(modrm.modrm) >= 8 && NMD_R(modrm.modrm) <= 0xd) ? 1 : 0));
							else
								instruction->id = NMD_X86_INSTRUCTION_FADD + modrm.fields.reg;
							break;
						case 0xdd:
							if (modrm.fields.mod == 0b11)
							{
								switch ((modrm.modrm - 0xc0) >> 3)
								{
								case 0b000: instruction->id = NMD_X86_INSTRUCTION_FFREE; break;
								case 0b001: instruction->id = NMD_X86_INSTRUCTION_FXCH; break;
								case 0b010: instruction->id = NMD_X86_INSTRUCTION_FST; break;
								case 0b011: instruction->id = NMD_X86_INSTRUCTION_FSTP; break;
								case 0b100: instruction->id = NMD_X86_INSTRUCTION_FUCOM; break;
								case 0b101: instruction->id = NMD_X86_INSTRUCTION_FUCOMP; break;
								}
							}
							else
							{
								switch (modrm.fields.reg)
								{
								case 0b000: instruction->id = NMD_X86_INSTRUCTION_FLD; break;
								case 0b001: instruction->id = NMD_X86_INSTRUCTION_FISTTP; break;
								case 0b010: instruction->id = NMD_X86_INSTRUCTION_FST; break;
								case 0b011: instruction->id = NMD_X86_INSTRUCTION_FSTP; break;
								case 0b100: instruction->id = NMD_X86_INSTRUCTION_FRSTOR; break;
								case 0b110: instruction->id = NMD_X86_INSTRUCTION_FNSAVE; break;
								case 0b111: instruction->id = NMD_X86_INSTRUCTION_FNSTSW; break;
								}
							}
							break;
						case 0xde:
							if (modrm.fields.mod == 0b11)
								instruction->id = (modrm.modrm == 0xd9 ? NMD_X86_INSTRUCTION_FCOMPP : ((modrm.modrm >= 0xd0 && modrm.modrm <= 0xd7) ? NMD_X86_INSTRUCTION_FCOMP : NMD_X86_INSTRUCTION_FADDP + _NMD_F_OP_GET_OFFSET()));
							else
								instruction->id = NMD_X86_INSTRUCTION_FIADD + modrm.fields.reg;
							break;
						case 0xdf:
							if (modrm.fields.mod == 0b11)
							{
								if (modrm.fields.reg == 0b000)
									instruction->id = NMD_X86_INSTRUCTION_FFREEP;
								else if (modrm.fields.reg == 0b001)
									instruction->id = NMD_X86_INSTRUCTION_FXCH;
								else if (modrm.fields.reg <= 3)
									instruction->id = NMD_X86_INSTRUCTION_FSTP;
								else if (modrm.modrm == 0xe0)
									instruction->id = NMD_X86_INSTRUCTION_FNSTSW;
								else if (modrm.fields.reg == 0b110)
									instruction->id = NMD_X86_INSTRUCTION_FCOMIP;
								else
									instruction->id = NMD_X86_INSTRUCTION_FUCOMIP;
							}
							else
								instruction->id = (modrm.fields.reg == 0b101 ? NMD_X86_INSTRUCTION_FILD : (modrm.fields.reg == 0b111 ? NMD_X86_INSTRUCTION_FISTP : (NMD_X86_INSTRUCTION_FILD + modrm.fields.reg)));
							break;
						}
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_INSTRUCTION_ID */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_CPU_FLAGS
				if (flags & NMD_X86_DECODER_FLAGS_CPU_FLAGS)
				{
					if (op == 0xcc || op == 0xcd || op == 0xce) /* int3,int,into */
					{
						instruction->clearedFlags.eflags = NMD_X86_EFLAGS_TF | NMD_X86_EFLAGS_RF;
						instruction->testedFlags.eflags = NMD_X86_EFLAGS_NT | NMD_X86_EFLAGS_VM;
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_IF | NMD_X86_EFLAGS_NT | NMD_X86_EFLAGS_VM | NMD_X86_EFLAGS_AC | NMD_X86_EFLAGS_VIF;
					}
					else if (NMD_R(op) == 7) /* conditional jump */
						_nmd_decode_conditional_flag(instruction, NMD_C(op));
					else if (NMD_R(op) == 4 || ((op == 0xfe || op == 0xff) && modrm.fields.reg <= 0b001)) /* inc,dec */
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_PF;
					else if (op <= 0x05 || (op >= 0x10 && op <= 0x15) || ((NMD_R(op) == 1 || NMD_R(op) == 2 || NMD_R(op) == 3) && (NMD_C(op) >= 0x8 && NMD_C(op) <= 0x0d)) || ((op >= 0x80 && op <= 0x83) && (modrm.fields.reg == 0b000 || modrm.fields.reg == 0b010 || modrm.fields.reg == 0b011 || modrm.fields.reg == 0b010 || modrm.fields.reg == 0b101 || modrm.fields.reg == 0b111)) || (op == 0xa6 || op == 0xa7) || (op == 0xae || op == 0xaf)) /* add,adc,sbb,sub,cmp, cmps,cmpsb,cmpsw,cmpsd,cmpsq, scas,scasb,scasw,scasd */
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF;
					else if ((op >= 0x08 && op <= 0x0d) || ((NMD_R(op) == 2 || NMD_R(op) == 3) && NMD_C(op) <= 5) || ((op >= 0x80 && op <= 0x83) && (modrm.fields.reg == 0b001 || modrm.fields.reg == 0b100 || modrm.fields.reg == 0b110)) || (op == 0x84 || op == 0x85 || op == 0xa8 || op == 0xa9) || ((op == 0xf6 || op == 0xf7) && modrm.fields.reg == 0b000)) /* or,and,xor, test */
					{
						instruction->clearedFlags.eflags = NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_CF;
						instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_AF;
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_PF;
					}
					else if (op == 0x69 || op == 0x6b || ((op == 0xf6 || op == 0xf7) && (modrm.fields.reg == 0b100 || modrm.fields.reg == 0b101))) /* mul,imul */
					{
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_OF;
						instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_PF;
					}
					else if (op == 0xf6 || op == 0xf7) /* Group 3 */
					{
						if (modrm.fields.reg == 0b011) /* neg */
							instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_OF;
						else if (modrm.fields.reg >= 0b110) /* div,idiv */
							instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_PF;
					}
					else if (op == 0xc0 || op == 0xc1 || (op >= 0xd0 && op <= 0xd3))
					{
						if (modrm.fields.reg <= 0b011) /* rol,ror,rcl,rcr */
						{
							instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF;
							instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_OF;
						}
						else /* shl,shr,sar */
						{
							instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_OF;
							instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_AF;
						}
					}
					else if (op == 0x27 || op == 0x2f) /* daa,das */
					{
						instruction->testedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_AF;
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_PF;
						instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_OF;
					}
					else if (op == 0x37 || op == 0x3f) /* aaa,aas */
					{
						instruction->testedFlags.eflags = NMD_X86_EFLAGS_AF;
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_CF;
						instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_PF;
					}
					else if (op == 0x63 && mode != NMD_X86_MODE_64) /* arpl */
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_ZF;
					else if (op == 0x9b) /* fwait,wait */
						instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C1 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
					else if (op == 0x9e) /* sahf */
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_PF | NMD_X86_EFLAGS_CF;
					else if (op == 0xd4 || op == 0xd5) /* aam,aad */
					{
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_SF | NMD_X86_EFLAGS_ZF | NMD_X86_EFLAGS_PF;
						instruction->undefinedFlags.eflags = NMD_X86_EFLAGS_OF | NMD_X86_EFLAGS_AF | NMD_X86_EFLAGS_CF;
					}
					else if (op >= 0xd8 && op <= 0xdf) /* escape opcodes */
					{
						if (op == 0xd8 || op == 0xdc)
						{
							if (modrm.fields.reg == 0b000 || modrm.fields.reg == 0b001 || modrm.fields.reg == 0b100 || modrm.fields.reg == 0b101 || modrm.fields.reg == 0b110 || modrm.fields.reg == 0b111) /* fadd,fmul,fsub,fsubr,fdiv,fdivr */
							{
								instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
								instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
							}
							else if (modrm.fields.reg == 0b010 || modrm.fields.reg == 0b011) /* fcom,fcomp */
							{
								instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								instruction->clearedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
							}
						}
						else if (op == 0xd9)
						{
							if (modrm.fields.mod != 0b11)
							{
								if (modrm.fields.reg == 0b000 || modrm.fields.reg == 0b010 || modrm.fields.reg == 0b011) /* fld,fst,fstp */
								{
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
								else if (modrm.fields.reg == 0b100) /* fldenv */
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C1 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								else if (modrm.fields.reg == 0b101 || modrm.fields.reg == 0b110 || modrm.fields.reg == 0b111) /* fldcw,fstenv,fstcw */
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C1 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
							}
							else
							{
								if (modrm.modrm < 0xc8) /* fld */
								{
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
								else /*if (modrm.modrm <= 0xcf)*/ /* fxch */
								{
									instruction->clearedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
							}
						}
						else if (op == 0xda || op == 0xde)
						{
							if (modrm.fields.mod != 0b11)
							{
								if (modrm.fields.reg == 0b000 || modrm.fields.reg == 0b001 || modrm.fields.reg == 0b100 || modrm.fields.reg == 0b101 || modrm.fields.reg == 0b110 || modrm.fields.reg == 0b111) /* fiadd,fimul,fisub,fisubr,fidiv,fidivr */
								{
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
								else /*if (modrm.fields.reg == 0b010 || modrm.fields.reg == 0b011)*/ /* ficom,ficomp */
								{
									instruction->clearedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
							}
							else
							{

								if ((op == 0xda && modrm.modrm == 0xe9) || (op == 0xde && modrm.modrm == 0xd9))
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C1 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								else
								{
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
							}
						}
						else if (op == 0xdb || op == 0xdd || op == 0xdf)
						{
							if (modrm.fields.mod != 0b11)
							{
								if (modrm.fields.reg == 0b000 || modrm.fields.reg == 0b010 || modrm.fields.reg == 0b011 || modrm.fields.reg == 0b101 || modrm.fields.reg == 0b111) /* fild,fist,fistp,fld,fstp */
								{
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
								else if (modrm.fields.reg == 0b001) /* fisttp */
								{
									instruction->clearedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
							}
							else
							{
								if (modrm.fields.reg <= 0b011) /* fcmovnb,fcmovne,fcmovnbe,fcmovnu */
								{
									instruction->modifiedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
								else if (modrm.modrm == 0xe0 || modrm.modrm == 0xe2) /* fstsw,fclex */
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C1 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								else if (modrm.modrm == 0xe3) /* finit */
									instruction->clearedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C1 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								else /* fucomi,fcomi */
								{
									instruction->clearedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C1;
									instruction->undefinedFlags.fpuFlags = NMD_X86_FPU_FLAGS_C0 | NMD_X86_FPU_FLAGS_C2 | NMD_X86_FPU_FLAGS_C3;
								}
							}
						}
					}
					else if (op == 0xf5) /* cmc */
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_CF;
					else if (op == 0xf8) /* clc */
						instruction->clearedFlags.eflags = NMD_X86_EFLAGS_CF;
					else if (op == 0xf9) /* stc */
						instruction->setFlags.eflags = NMD_X86_EFLAGS_CF;
					else if (op == 0xfa || op == 0xfb) /* cli,sti */
					{
						instruction->modifiedFlags.eflags = NMD_X86_EFLAGS_IF | NMD_X86_EFLAGS_VIF;
						instruction->testedFlags.eflags = NMD_X86_EFLAGS_IOPL;
					}
					else if (op == 0xfc) /* cld */
						instruction->clearedFlags.eflags = NMD_X86_EFLAGS_DF;
					else if (op == 0xfd) /* std */
						instruction->setFlags.eflags = NMD_X86_EFLAGS_DF;
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_CPU_FLAGS */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_GROUP
				/* Parse the instruction's group. */
				if (flags & NMD_X86_DECODER_FLAGS_GROUP)
				{
					if (NMD_R(op) == 7 || op == 0xe3)
						instruction->group = NMD_GROUP_JUMP | NMD_GROUP_CONDITIONAL_BRANCH | NMD_GROUP_RELATIVE_ADDRESSING;
					else if (op == 0xe9 || op == 0xea || op == 0xeb || (op == 0xff && (modrm.fields.reg == 0b100 || modrm.fields.reg == 0b101)))
						instruction->group = NMD_GROUP_JUMP | NMD_GROUP_UNCONDITIONAL_BRANCH | (op == 0xe9 || op == 0xeb ? NMD_GROUP_RELATIVE_ADDRESSING : 0);
					else if (op == 0x9a || op == 0xe8 || (op == 0xff && (modrm.fields.reg == 0b010 || modrm.fields.reg == 0b011)))
						instruction->group = NMD_GROUP_CALL | NMD_GROUP_UNCONDITIONAL_BRANCH | (op == 0xe8 ? NMD_GROUP_RELATIVE_ADDRESSING : 0);
					else if (op == 0xc2 || op == 0xc3 || op == 0xca || op == 0xcb)
						instruction->group = NMD_GROUP_RET;
					else if ((op >= 0xcc && op <= 0xce) || op == 0xf1)
						instruction->group = NMD_GROUP_INT;
					else if (op == 0xf4)
						instruction->group = NMD_GROUP_PRIVILEGE;
					else if (op == 0xc7 && modrm.modrm == 0xf8)
						instruction->group = NMD_GROUP_UNCONDITIONAL_BRANCH | NMD_GROUP_RELATIVE_ADDRESSING;
					else if (op >= 0xe0 && op <= 0xe2)
						instruction->group = NMD_GROUP_CONDITIONAL_BRANCH | NMD_GROUP_RELATIVE_ADDRESSING;
					else if (op == 0x8d && mode == NMD_X86_MODE_64)
						instruction->group = NMD_GROUP_RELATIVE_ADDRESSING;
					else if(op == 0xcf)
						instruction->group = NMD_GROUP_RET | NMD_GROUP_INT;
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_GROUP */

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS
				if (flags & NMD_X86_DECODER_FLAGS_OPERANDS)
				{
					if (op >= 0xd8 && op <= 0xdf)
					{
						if (modrm.fields.mod == 0b11)
						{
							if ((op == 0xd9 && (NMD_R(modrm.modrm) == 0xc || (op >= 0xc8 && op <= 0xcf))) ||
								(op == 0xda && NMD_R(modrm.modrm) <= 0xd) ||
								(op == 0xdb && (NMD_R(modrm.modrm) <= 0xd || modrm.modrm >= 0xe8)) ||
								(op == 0xde && modrm.modrm != 0xd9) ||
								(op == 0xdf && modrm.modrm != 0xe0))
								instruction->numOperands = 2;
						}
						else
							instruction->numOperands = 1;
					}
					else if ((NMD_R(op) < 4 && op % 8 < 6) || (NMD_R(op) >= 8 && NMD_R(op) <= 0xb && op != 0x8f && op != 0x90 && !(op >= 0x98 && op <= 0x9f)) || op == 0x62 || op == 0x63 || (op >= 0x6c && op <= 0x6f) || op == 0xc0 || op == 0xc1 || (op >= 0xc4 && op <= 0xc8) || (op >= 0xd0 && op <= 0xd3) || (NMD_R(op) == 0xe && op % 8 >= 4))
						instruction->numOperands = 2;
					else if (NMD_R(op) == 4 || NMD_R(op) == 5 || NMD_R(op) == 7 || (op == 0x68 || op == 0x6a) || op == 0x8f || op == 0x9a || op == 0xc2 || op == 0xca || op == 0xcd || op == 0xd4 || op == 0xd5 || (NMD_R(op) == 0xe && op % 8 <= 3) || (NMD_R(op) == 0xf && op % 8 >= 6))
						instruction->numOperands = 1;
					else if (op == 0x69 || op == 0x6b)
						instruction->numOperands = 3;

					if (instruction->numOperands > 0)
					{
						if (op >= 0x84 && op <= 0x8b)
						{
							if (op % 2 == 0)
							{
								_nmd_decode_operand_Eb(instruction, &instruction->operands[op == 0x8a ? 1 : 0]);
								_nmd_decode_operand_Gb(instruction, &instruction->operands[op == 0x8a ? 0 : 1]);
							}
							else
							{
								_nmd_decode_operand_Ev(instruction, &instruction->operands[op == 0x8b ? 1 : 0]);
								_nmd_decode_operand_Gv(instruction, &instruction->operands[op == 0x8b ? 0 : 1]);
							}

							if (op >= 0x88)
							{
								instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
								instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
							}
							else if (op >= 0x86)
								instruction->operands[0].action = instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						}
						else if (op >= 0x80 && op <= 0x83)
						{
							if (op % 2 == 0)
								_nmd_decode_operand_Eb(instruction, &instruction->operands[0]);
							else
								_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
							instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
						}
						else if (op == 0x68 || NMD_R(op) == 7 || op == 0x6a || op == 0x9a || op == 0xc2 || op == 0xca || op == 0xcd || op == 0xd4 || op == 0xd5)
							instruction->operands[0].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
						else if (op == 0x90 && instruction->prefixes & NMD_X86_PREFIXES_REX_B)
						{
							instruction->operands[0].type = instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[0].fields.reg = (uint8_t)(instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_REG_R8 : NMD_X86_REG_R8D);
							instruction->operands[1].fields.reg = (uint8_t)(instruction->prefixes & NMD_X86_PREFIXES_REX_W ? NMD_X86_REG_RAX : NMD_X86_REG_EAX);
						}
						else if (NMD_R(op) < 4)
						{
							const size_t opMod8 = (size_t)(op % 8);
							if (opMod8 == 0 || opMod8 == 2)
							{
								_nmd_decode_operand_Eb(instruction, &instruction->operands[opMod8 == 0 ? 0 : 1]);
								_nmd_decode_operand_Gb(instruction, &instruction->operands[opMod8 == 0 ? 1 : 0]);
							}
							else if (opMod8 == 1 || opMod8 == 3)
							{
								_nmd_decode_operand_Ev(instruction, &instruction->operands[opMod8 == 1 ? 0 : 1]);
								_nmd_decode_operand_Gv(instruction, &instruction->operands[opMod8 == 1 ? 1 : 0]);
							}
							else if (opMod8 == 4 || opMod8 == 5)
							{
								instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
								if (opMod8 == 4)
									instruction->operands[0].fields.reg = NMD_X86_REG_AL;
								else
									instruction->operands[0].fields.reg = (uint8_t)(instruction->operandSize64 ? NMD_X86_REG_RAX : (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : NMD_X86_REG_EAX));

								instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
							}

							instruction->operands[0].action = instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
							if (!(NMD_R(op) == 3 && NMD_C(op) >= 8))
								instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						}
						else if (NMD_R(op) == 4)
						{
							instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[0].fields.reg = (uint8_t)((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : NMD_X86_REG_EAX) + (op % 8));
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						}
						else if (NMD_R(op) == 5)
						{
							instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[0].fields.reg = (uint8_t)((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : (mode == NMD_X86_MODE_64 ? NMD_X86_REG_RAX : NMD_X86_REG_EAX)) + (op % 8));
							instruction->operands[0].action = (uint8_t)(NMD_C(op) < 8 ? NMD_X86_OPERAND_ACTION_READ : NMD_X86_OPERAND_ACTION_WRITE);
						}
						else if (op == 0x62)
						{
							_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
							_nmd_decode_modrm_upper32(instruction, &instruction->operands[1]);
							instruction->operands[0].action = instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
						}
						else if (op == 0x63)
						{
							if (mode == NMD_X86_MODE_64)
							{
								_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
								_nmd_decode_operand_Ev(instruction, &instruction->operands[1]);
								instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
								instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
							}
							else
							{
								if (instruction->modrm.fields.mod == 0b11)
								{
									instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
									instruction->operands[0].fields.reg = NMD_X86_REG_AX + instruction->modrm.fields.rm;
								}
								else
									_nmd_decode_modrm_upper32(instruction, &instruction->operands[0]);

								instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
								instruction->operands[1].fields.reg = NMD_X86_REG_AX + instruction->modrm.fields.reg;
								instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
								instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
							}
						}
						else if (op == 0x69 || op == 0x6b)
						{
							_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
							_nmd_decode_operand_Ev(instruction, &instruction->operands[1]);
							instruction->operands[2].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
							instruction->operands[2].fields.imm = (int64_t)(instruction->immediate);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
							instruction->operands[1].action = instruction->operands[2].action = NMD_X86_OPERAND_ACTION_READ;
						}
						else if (op == 0x8c)
						{
							_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
							instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
							instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[1].fields.reg = NMD_X86_REG_ES + instruction->modrm.fields.reg;
						}
						else if (op == 0x8d)
						{
							_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
							instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
							_nmd_decode_modrm_upper32(instruction, &instruction->operands[1]);
						}
						else if (op == 0x8e)
						{
							instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[0].fields.reg = NMD_X86_REG_ES + instruction->modrm.fields.reg;
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
							instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
							_nmd_decode_operand_Ew(instruction, &instruction->operands[1]);
						}
						else if (op == 0x8f)
						{
							_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						}
						else if (op >= 0x91 && op <= 0x97)
						{
							_nmd_decode_operand_Gv(instruction, &instruction->operands[0]);
							instruction->operands[0].fields.reg = instruction->operands[0].fields.reg + NMD_C(op);
							instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[1].fields.reg = (uint8_t)(instruction->operandSize64 ? NMD_X86_REG_RAX : (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && mode != NMD_X86_MODE_16 ? NMD_X86_REG_AX : NMD_X86_REG_EAX));
							instruction->operands[0].action = instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						}
						else if (op >= 0xa0 && op <= 0xa3)
						{
							instruction->operands[op < 0xa2 ? 0 : 1].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[op < 0xa2 ? 0 : 1].fields.reg = (uint8_t)(op % 2 == 0 ? NMD_X86_REG_AL : (instruction->operandSize64 ? NMD_X86_REG_RAX : ((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && mode != NMD_X86_MODE_16) || (mode == NMD_X86_MODE_16 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? NMD_X86_REG_AX : NMD_X86_REG_EAX)));
							instruction->operands[op < 0xa2 ? 1 : 0].type = NMD_X86_OPERAND_TYPE_MEMORY;
							instruction->operands[op < 0xa2 ? 1 : 0].fields.mem.disp = (mode == NMD_X86_MODE_64) ? *(uint64_t*)(b + 1) : *(uint32_t*)(b + 1);
							_nmd_decode_operand_segment_reg(instruction, &instruction->operands[op < 0xa2 ? 1 : 0]);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
							instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
						}
						else if (op == 0xa8 || op == 0xa9)
						{
							instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[0].fields.reg = (uint8_t)(op == 0xa8 ? NMD_X86_REG_AL : (instruction->operandSize64 ? NMD_X86_REG_RAX : ((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && mode != NMD_X86_MODE_16) || (mode == NMD_X86_MODE_16 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? NMD_X86_REG_AX : NMD_X86_REG_EAX)));
							instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
						}
						else if (NMD_R(op) == 0xb)
						{
							instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[0].fields.reg = (uint8_t)((op < 0xb8 ? (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? NMD_X86_REG_R8B : NMD_X86_REG_AL) : (instruction->prefixes & NMD_X86_PREFIXES_REX_W ? (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? NMD_X86_REG_R8 : NMD_X86_REG_RAX) : (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? NMD_X86_REG_R8D : NMD_X86_REG_EAX))) + op % 8);
							instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
						}
						else if (op == 0xc0 || op == 0xc1 || op == 0xc6 || op == 0xc7)
						{
							if (!(op >= 0xc6 && instruction->modrm.fields.reg))
							{
								if (op % 2 == 0)
									_nmd_decode_operand_Eb(instruction, &instruction->operands[0]);
								else
									_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
							}
							instruction->operands[op >= 0xc6 && instruction->modrm.fields.reg ? 0 : 1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
							instruction->operands[0].action = (uint8_t)(op <= 0xc1 ? NMD_X86_OPERAND_ACTION_READ_WRITE : NMD_X86_OPERAND_ACTION_WRITE);
						}
						else if (op == 0xc4 || op == 0xc5)
						{
							instruction->operands[0].type = NMD_X86_OPERAND_TYPE_REGISTER;
							instruction->operands[0].fields.reg = (uint8_t)((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : NMD_X86_REG_EAX) + instruction->modrm.fields.reg);
							_nmd_decode_modrm_upper32(instruction, &instruction->operands[1]);
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_WRITE;
							instruction->operands[1].action = NMD_X86_OPERAND_ACTION_READ;
						}
						else if (op == 0xc8)
						{
							instruction->operands[0].type = instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
							instruction->operands[0].size = 2;
							instruction->operands[0].fields.imm = *(uint16_t*)(b + 1);
							instruction->operands[1].size = 1;
							instruction->operands[1].fields.imm = b[3];
						}
						else if (op >= 0xd0 && op <= 0xd3)
						{
							if (op % 2 == 0)
								_nmd_decode_operand_Eb(instruction, &instruction->operands[0]);
							else
								_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);

							if (op < 0xd2)
							{
								instruction->operands[1].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
								instruction->operands[1].fields.imm = 1;
							}
							else
							{
								instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
								instruction->operands[1].fields.reg = NMD_X86_REG_CL;
							}
							instruction->operands[0].action = NMD_X86_OPERAND_ACTION_READ_WRITE;
						}
						else if (op >= 0xd8 && op <= 0xdf)
						{
							if (instruction->modrm.fields.mod != 0b11 ||
								op == 0xd8 ||
								(op == 0xd9 && NMD_C(instruction->modrm.modrm) == 0xc) ||
								(op == 0xda && NMD_C(instruction->modrm.modrm) <= 0xd) ||
								(op == 0xdb && (NMD_C(instruction->modrm.modrm) <= 0xd || instruction->modrm.modrm >= 0xe8)) ||
								op == 0xdc ||
								op == 0xdd ||
								(op == 0xde && instruction->modrm.modrm != 0xd9) ||
								(op == 0xdf && instruction->modrm.modrm != 0xe0))
							{
								instruction->operands[0].type = instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
								instruction->operands[0].isImplicit = true;
								instruction->operands[0].fields.reg = NMD_X86_REG_ST0;
								instruction->operands[1].fields.reg = NMD_X86_REG_ST0 + instruction->modrm.fields.reg;
							}
						}
						else if (NMD_R(op) == 0xe)
						{
							if (op % 8 < 4)
							{
								instruction->operands[0].type = NMD_X86_OPERAND_TYPE_IMMEDIATE;
								instruction->operands[0].fields.imm = (int64_t)(instruction->immediate);
							}
							else
							{
								if (op < 0xe8)
								{
									instruction->operands[0].type = (uint8_t)(NMD_C(op) < 6 ? NMD_X86_OPERAND_TYPE_REGISTER : NMD_X86_OPERAND_TYPE_IMMEDIATE);
									instruction->operands[1].type = (uint8_t)(NMD_C(op) < 6 ? NMD_X86_OPERAND_TYPE_IMMEDIATE : NMD_X86_OPERAND_TYPE_REGISTER);
									instruction->operands[0].fields.imm = instruction->operands[1].fields.imm = (int64_t)(instruction->immediate);
								}
								else
								{
									instruction->operands[0].type = instruction->operands[1].type = NMD_X86_OPERAND_TYPE_REGISTER;
									instruction->operands[0].fields.reg = instruction->operands[1].fields.reg = NMD_X86_REG_DX;
								}

								if (op % 2 == 0)
									instruction->operands[op % 8 == 4 ? 0 : 1].fields.reg = NMD_X86_REG_AL;
								else
									instruction->operands[op % 8 == 5 ? 0 : 1].fields.reg = (uint8_t)((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? NMD_X86_REG_AX : NMD_X86_REG_EAX) + instruction->modrm.fields.reg);

								instruction->operands[op % 8 <= 5 ? 0 : 1].action = NMD_X86_OPERAND_ACTION_WRITE;
								instruction->operands[op % 8 <= 5 ? 1 : 0].action = NMD_X86_OPERAND_ACTION_READ;
							}
						}
						else if (op == 0xf6 || op == 0xfe)
						{
							_nmd_decode_operand_Eb(instruction, &instruction->operands[0]);
							instruction->operands[0].action = (uint8_t)(op == 0xfe && instruction->modrm.fields.reg >= 0b010 ? NMD_X86_OPERAND_ACTION_READ : NMD_X86_OPERAND_ACTION_READ_WRITE);
						}
						else if (op == 0xf7 || op == 0xff)
						{
							_nmd_decode_operand_Ev(instruction, &instruction->operands[0]);
							instruction->operands[0].action = (uint8_t)(op == 0xff && instruction->modrm.fields.reg >= 0b010 ? NMD_X86_OPERAND_ACTION_READ : NMD_X86_OPERAND_ACTION_READ_WRITE);
						}
					}
				}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS */
			}
	}

	if (instruction->prefixes & NMD_X86_PREFIXES_LOCK)
	{
		if (!(instruction->hasModrm && instruction->modrm.fields.mod != 0b11 &&
			((instruction->opcodeSize == 1 && (op == 0x86 || op == 0x87 || (NMD_R(op) < 4 && (op % 8) < 2 && op < 0x38) || ((op >= 0x80 && op <= 0x83) && instruction->modrm.fields.reg != 0b111) || (op >= 0xfe && instruction->modrm.fields.reg < 2) || ((op == 0xf6 || op == 0xf7) && (instruction->modrm.fields.reg == 0b010 || instruction->modrm.fields.reg == 0b011)))) ||
				(instruction->opcodeSize == 2 && (_nmd_findByte(_nmd_twoOpcodes, sizeof(_nmd_twoOpcodes), op) || op == 0xab || (op == 0xba && instruction->modrm.fields.reg != 0b100) || (op == 0xc7 && instruction->modrm.fields.reg == 0b001))))))
			return false;
	}

	instruction->length = (uint8_t)((ptrdiff_t)(++b + (size_t)instruction->immMask) - (ptrdiff_t)(buffer));
	for (i = 0; i < instruction->length; i++)
		instruction->buffer[i] = ((const uint8_t*)(buffer))[i];

	for (i = 0; i < (size_t)instruction->immMask; i++)
		((uint8_t*)(&instruction->immediate))[i] = b[i];

#ifndef NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS
	for (i = 0; i < instruction->numOperands; i++)
	{
		if (instruction->operands[i].type == NMD_X86_OPERAND_TYPE_IMMEDIATE)
		{
			if (instruction->operands[i].action == NMD_X86_OPERAND_ACTION_NONE)
				instruction->operands[i].action = NMD_X86_OPERAND_ACTION_READ;

			if (instruction->operands[i].size == 0)
			{
				instruction->operands[i].size = (uint8_t)instruction->immMask;
				instruction->operands[i].fields.imm = instruction->immediate;
			}
		}
	}
#endif /* NMD_ASSEMBLY_DISABLE_DECODER_OPERANDS */

	instruction->valid = true;

	return true;
}

bool _nmd_ldisasm_parse_modrm(const uint8_t** b, bool addressPrefix, NMD_X86_MODE mode, nmd_x86_modrm* const pModrm, size_t remainingSize)
{
	if (remainingSize == 0)
		return false;

	const nmd_x86_modrm modrm = *(nmd_x86_modrm*)(++*b);
	*pModrm = modrm;
	bool hasSIB = false;
	size_t dispSize = 0;

	if (mode == NMD_X86_MODE_16)
	{
		if (modrm.fields.mod != 0b11)
		{
			if (modrm.fields.mod == 0b00)
			{
				if (modrm.fields.rm == 0b110)
					dispSize = 2;
			}
			else
				dispSize = modrm.fields.mod == 0b01 ? 1 : 2;
		}
	}
	else
	{
		if (addressPrefix && mode == NMD_X86_MODE_32)
		{
			if ((modrm.fields.mod == 0b00 && modrm.fields.rm == 0b110) || modrm.fields.mod == 0b10)
				dispSize = 2;
			else if (modrm.fields.mod == 0b01)
				dispSize = 1;
		}
		else
		{
			/* Check for SIB byte */
			uint8_t sib = 0;
			if (modrm.modrm < 0xC0 && modrm.fields.rm == 0b100 && (!addressPrefix || (addressPrefix && mode == NMD_X86_MODE_64)))
			{
				if (remainingSize < 2)
					return false;

				hasSIB = true;
				sib = *++*b;
			}

			if (modrm.fields.mod == 0b01) /* disp8 (ModR/M) */
				dispSize = 1;
			else if ((modrm.fields.mod == 0b00 && modrm.fields.rm == 0b101) || modrm.fields.mod == 0b10) /* disp16,32 (ModR/M) */
				dispSize = (addressPrefix && !(mode == NMD_X86_MODE_64 && addressPrefix) ? 2 : 4);
			else if (hasSIB && (sib & 0b111) == 0b101) /* disp8,32 (SIB) */
				dispSize = (modrm.fields.mod == 0b01 ? 1 : 4);
		}
	}

	*b += dispSize;

	return remainingSize - (hasSIB ? 2 : 1) >= dispSize;
}

/*
Returns the length of the instruction if it is valid, zero otherwise.
Parameters:
 - buffer     [in] A pointer to a buffer containing an encoded instruction.
 - bufferSize [in] The size of the buffer in bytes.
 - mode       [in] The architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
*/
size_t nmd_x86_ldisasm(const void* buffer, size_t bufferSize, NMD_X86_MODE mode)
{
	const uint8_t* b = (const uint8_t*)(buffer);

	bool operandPrefix = false;
	bool addressPrefix = false;
	bool repeatPrefix = false;
	bool repeatNotZeroPrefix = false;
	bool rexW = false;
	bool lockPrefix = false;
	uint16_t simdPrefix = NMD_X86_PREFIXES_NONE;
	uint8_t op = 0;
	uint8_t opcodeSize = 0;

	bool hasModrm = false;
	nmd_x86_modrm modrm = { 0,0,0 };

	size_t offset = 0;

	/* Parse legacy prefixes & REX prefixes. */
	size_t i = 0;
	for (; i < NMD_X86_MAXIMUM_INSTRUCTION_LENGTH; i++, b++)
	{
		switch (*b)
		{
		case 0xF0: lockPrefix = true; continue;
		case 0xF2: repeatNotZeroPrefix = true, simdPrefix = NMD_X86_PREFIXES_REPEAT_NOT_ZERO; continue;
		case 0xF3: repeatPrefix = true, simdPrefix = NMD_X86_PREFIXES_REPEAT; continue;
		case 0x2E: continue;
		case 0x36: continue;
		case 0x3E: continue;
		case 0x26: continue;
		case 0x64: continue;
		case 0x65: continue;
		case 0x66: operandPrefix = true, simdPrefix = NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE; continue;
		case 0x67: addressPrefix = true; continue;
		default:
			if (mode == NMD_X86_MODE_64 && NMD_R(*b) == 4) /* [40,4f[ */
			{
				if(NMD_C(*b) & 0b1000)
					rexW = true;
				continue;
			}
		}

		break;
	}

	const size_t numPrefixes = (uint8_t)((ptrdiff_t)(b)-(ptrdiff_t)(buffer));

	const size_t remainingValidBytes = (NMD_X86_MAXIMUM_INSTRUCTION_LENGTH - numPrefixes);
	if (remainingValidBytes == 0)
		return 0;

	const size_t remainingBufferSize = bufferSize - numPrefixes;
	if (remainingBufferSize == 0)
		return 0;

	const size_t remainingSize = remainingValidBytes < remainingBufferSize ? remainingValidBytes : remainingBufferSize;

	/* Parse opcode. */
	if (*b == 0x0F) /* 2 or 3 byte opcode. */
	{
		if (remainingSize == 1)
			return false;

		b++;

		if (*b == 0x38 || *b == 0x3A) /* 3 byte opcode. */
		{

			if (remainingSize < 4)
				return false;

			const bool isOpcodeMap38 = *b == 0x38;
			op = *++b;
			modrm = *(nmd_x86_modrm*)(b + 1);
			opcodeSize = 3;
			hasModrm = true;
			if (!_nmd_ldisasm_parse_modrm(&b, addressPrefix, mode, &modrm, remainingSize - 3))
				return 0;

			if (isOpcodeMap38)
			{
#ifndef NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK
				/* Check if the instruction is invalid. */
				if (op == 0x36)
				{
					return 0;
				}
				else if (op <= 0xb || (op >= 0x1c && op <= 0x1e))
				{
					if (simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
						return 0;
				}
				else if (op >= 0xc8 && op <= 0xcd)
				{
					if (simdPrefix)
						return 0;
				}
				else if (op == 0x10 || op == 0x14 || op == 0x15 || op == 0x17 || (op >= 0x20 && op <= 0x25) || op == 0x28 || op == 0x29 || op == 0x2b || NMD_R(op) == 3 || op == 0x40 || op == 0x41 || op == 0xcf || (op >= 0xdb && op <= 0xdf))
				{
					if (simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
						return 0;
				}
				else if (op == 0x2a || (op >= 0x80 && op <= 0x82))
				{
					if (modrm.fields.mod == 0b11 || simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
						return 0;
				}
				else if (op == 0xf0 || op == 0xf1)
				{
					if (modrm.fields.mod == 0b11 && (simdPrefix == NMD_X86_PREFIXES_NONE || simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE))
						return 0;
					else if (simdPrefix == NMD_X86_PREFIXES_REPEAT)
						return 0;
				}
				else if (op == 0xf5 || op == 0xf8)
				{
					if (simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || modrm.fields.mod == 0b11)
						return 0;
				}
				else if (op == 0xf6)
				{
					if (simdPrefix == NMD_X86_PREFIXES_NONE && modrm.fields.mod == 0b11)
						return 0;
					else if (simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
						return 0;
				}
				else if (op == 0xf9)
				{
					if (simdPrefix != NMD_X86_PREFIXES_NONE || modrm.fields.mod == 0b11)
						return 0;
				}
				else
					return 0;
#endif /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK */
			}
			else /* 0x3a */
			{
				if (remainingSize < 5)
					return false;

				offset++;

#ifndef NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK
				/* Check if the instruction is invalid. */
				if ((op >= 0x8 && op <= 0xe) || (op >= 0x14 && op <= 0x17) || (op >= 0x20 && op <= 0x22) || (op >= 0x40 && op <= 0x42) || op == 0x44 || (op >= 0x60 && op <= 0x63) || op == 0xdf || op == 0xce || op == 0xcf)
				{
					if (simdPrefix != NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
						return 0;
				}
				else if (op == 0x0f || op == 0xcc)
				{
					if (simdPrefix)
						return 0;
				}
				else
					return 0;
#endif /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK */
			}
		}
		else if (*b == 0x0f) /* 3DNow! opcode map*/
		{
#ifndef NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_3DNOW
				if (remainingSize < 5)
					return false;

				/*
				if (!_nmd_decode_modrm(&b, instruction, remainingSize - 2))
					return false;
				
				instruction->encoding = NMD_X86_ENCODING_3DNOW;
				instruction->opcode = 0x0f;
				instruction->immMask = NMD_X86_IMM8; 
				instruction->immediate = *(b + 1);
				*/

#ifndef NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK
				/*if (!_nmd_findByte(_nmd_valid3DNowOpcodes, sizeof(_nmd_valid3DNowOpcodes), (uint8_t)instruction->immediate))
					return false;*/
#endif /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK */
#else /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_3DNOW */
			return false;
#endif /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_3DNOW */
		}
		else /* 2 byte opcode. */
		{
			op = *b;
			opcodeSize = 2;

			/* Check for ModR/M, SIB and displacement. */
			if (op >= 0x20 && op <= 0x23 && remainingSize == 2)
				hasModrm = true, modrm.modrm = *++b;
			else if (op < 4 || (NMD_R(op) != 3 && NMD_R(op) > 0 && NMD_R(op) < 7) || (op >= 0xD0 && op != 0xFF) || (NMD_R(op) == 7 && NMD_C(op) != 7) || NMD_R(op) == 9 || NMD_R(op) == 0xB || (NMD_R(op) == 0xC && NMD_C(op) < 8) || (NMD_R(op) == 0xA && (op % 8) >= 3) || op == 0x0ff || op == 0x00 || op == 0x0d)
			{
				if (!_nmd_ldisasm_parse_modrm(&b, addressPrefix, mode, &modrm, remainingSize - 2))
					return 0;
				hasModrm = true;
			}

#ifndef NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK
			/* Check if the instruction is invalid. */
			if (_nmd_findByte(_nmd_invalid2op, sizeof(_nmd_invalid2op), op))
				return 0;
			else if (op == 0xc7)
			{
				if ((!simdPrefix && (modrm.fields.mod == 0b11 ? modrm.fields.reg <= 0b101 : modrm.fields.reg == 0b000 || modrm.fields.reg == 0b010)) || (simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO && (modrm.fields.mod == 0b11 || modrm.fields.reg != 0b001)) || ((simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || simdPrefix == NMD_X86_PREFIXES_REPEAT) && (modrm.fields.mod == 0b11 ? modrm.fields.reg <= (simdPrefix == NMD_X86_PREFIXES_REPEAT ? 0b110 : 0b101) : (modrm.fields.reg != 0b001 && modrm.fields.reg != 0b110))))
					return 0;
			}
			else if (op == 0x00)
			{
				if (modrm.fields.reg >= 0b110)
					return 0;
			}
			else if (op == 0x01)
			{
				if ((modrm.fields.mod == 0b11 ? (( (simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO || simdPrefix == NMD_X86_PREFIXES_REPEAT) && ((modrm.modrm >= 0xc0 && modrm.modrm <= 0xc5) || (modrm.modrm >= 0xc8 && modrm.modrm <= 0xcb) || (modrm.modrm >= 0xcf && modrm.modrm <= 0xd1) || (modrm.modrm >= 0xd4 && modrm.modrm <= 0xd7) || modrm.modrm == 0xee || modrm.modrm == 0xef || modrm.modrm == 0xfa || modrm.modrm == 0xfb)) || (modrm.fields.reg == 0b000 && modrm.fields.rm >= 0b110) || (modrm.fields.reg == 0b001 && modrm.fields.rm >= 0b100 && modrm.fields.rm <= 0b110) || (modrm.fields.reg == 0b010 && (modrm.fields.rm == 0b010 || modrm.fields.rm == 0b011)) || (modrm.fields.reg == 0b101 && modrm.fields.rm < 0b110 && (!repeatPrefix || (simdPrefix == NMD_X86_PREFIXES_REPEAT && (modrm.fields.rm != 0b000 && modrm.fields.rm != 0b010)))) || (modrm.fields.reg == 0b111 && (modrm.fields.rm > 0b101 || (mode != NMD_X86_MODE_64 && modrm.fields.rm == 0b000)))) : (!repeatPrefix && modrm.fields.reg == 0b101)))
					return 0;
			}
			else if (op == 0x1A || op == 0x1B)
			{
				if (modrm.fields.mod == 0b11)
					return 0;
			}
			else if (op == 0x20 || op == 0x22)
			{
				if (modrm.fields.reg == 0b001 || modrm.fields.reg >= 0b101)
					return 0;
			}
			else if (op >= 0x24 && op <= 0x27)
				return 0;
			else if (op >= 0x3b && op <= 0x3f)
				return 0;
			else if (NMD_R(op) == 5)
			{
				if ((op == 0x50 && modrm.fields.mod != 0b11) || (simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && (op == 0x52 || op == 0x53)) || (simdPrefix == NMD_X86_PREFIXES_REPEAT && (op == 0x50 || (op >= 0x54 && op <= 0x57))) || (repeatNotZeroPrefix && (op == 0x50 || (op >= 0x52 && op <= 0x57) || op == 0x5b)))
					return 0;
			}
			else if (NMD_R(op) == 6)
			{
				if ((!(simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) && (op == 0x6c || op == 0x6d)) || (simdPrefix == NMD_X86_PREFIXES_REPEAT && op != 0x6f) || repeatNotZeroPrefix)
					return 0;
			}
			else if (op == 0x78 || op == 0x79)
			{
				if ((((simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && op == 0x78) && !(modrm.fields.mod == 0b11 && modrm.fields.reg == 0b000)) || ((simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) && modrm.fields.mod != 0b11)) || (simdPrefix == NMD_X86_PREFIXES_REPEAT))
					return 0;
			}
			else if (op == 0x7c || op == 0x7d)
			{
				if (simdPrefix == NMD_X86_PREFIXES_REPEAT || !(simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO))
					return 0;
			}
			else if (op == 0x7e || op == 0x7f)
			{
				if (repeatNotZeroPrefix)
					return 0;
			}
			else if (op >= 0x71 && op <= 0x73)
			{
				if ((simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) || modrm.modrm <= 0xcf || (modrm.modrm >= 0xe8 && modrm.modrm <= 0xef))
					return 0;
			}
			else if (op == 0x73)
			{
				if (modrm.modrm >= 0xe0 && modrm.modrm <= 0xe8)
					return 0;
			}
			else if (op == 0xa6)
			{
				if (modrm.modrm != 0xc0 && modrm.modrm != 0xc8 && modrm.modrm != 0xd0)
					return 0;
			}
			else if (op == 0xa7)
			{
				if (!(modrm.fields.mod == 0b11 && modrm.fields.reg <= 0b101 && modrm.fields.rm == 0b000))
					return 0;
			}
			else if (op == 0xae)
			{
				if (((!simdPrefix && modrm.fields.mod == 0b11 && modrm.fields.reg <= 0b100) || (simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO && !(modrm.fields.mod == 0b11 && modrm.fields.reg == 0b110)) || (simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && (modrm.fields.reg < 0b110 || (modrm.fields.mod == 0b11 && modrm.fields.reg == 0b111))) || (simdPrefix == NMD_X86_PREFIXES_REPEAT && (modrm.fields.reg != 0b100 && modrm.fields.reg != 0b110) && !(modrm.fields.mod == 0b11 && modrm.fields.reg == 0b101))))
					return 0;
			}
			else if (op == 0xb8)
			{
				if (!repeatPrefix)
					return 0;
			}
			else if (op == 0xba)
			{
				if (modrm.fields.reg <= 0b011)
					return 0;
			}
			else if (op == 0xd0)
			{
				if (!simdPrefix || simdPrefix == NMD_X86_PREFIXES_REPEAT)
					return 0;
			}
			else if (op == 0xe0)
			{
				if (simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
					return 0;
			}
			else if (op == 0xf0)
			{
				if (simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? modrm.fields.mod == 0b11 : true)
					return 0;
			}
			else if (simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
			{
				if ((op >= 0x13 && op <= 0x17 && !(op == 0x16 && simdPrefix == NMD_X86_PREFIXES_REPEAT)) || op == 0x28 || op == 0x29 || op == 0x2e || op == 0x2f || (op <= 0x76 && op >= 0x74))
					return 0;
			}
			else if (op == 0x71 || op == 0x72 || (op == 0x73 && !(simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)))
			{
				if ((modrm.modrm >= 0xd8 && modrm.modrm <= 0xdf) || modrm.modrm >= 0xf8)
					return 0;
			}
			else if (op >= 0xc3 && op <= 0xc6)
			{
				if ((op == 0xc5 && modrm.fields.mod != 0b11) || (simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) || (op == 0xc3 && simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE))
					return 0;
			}
			else if (NMD_R(op) >= 0xd && NMD_C(op) != 0 && op != 0xff && ((NMD_C(op) == 6 && NMD_R(op) != 0xf) ? (!simdPrefix || (NMD_R(op) == 0xD && (simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) ? modrm.fields.mod != 0b11 : false)) : (simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO || ((NMD_C(op) == 7 && NMD_R(op) != 0xe) ? modrm.fields.mod != 0b11 : false))))
				return 0;
			else if (hasModrm && modrm.fields.mod == 0b11)
			{
				if (op == 0xb2 || op == 0xb4 || op == 0xb5 || op == 0xc3 || op == 0xe7 || op == 0x2b || (simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && (op == 0x12 || op == 0x16)) || (!(simdPrefix == NMD_X86_PREFIXES_REPEAT || simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO) && (op == 0x13 || op == 0x17)))
					return 0;
			}
#endif /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK */

			if (NMD_R(op) == 8) /* imm32 */
				offset += (operandPrefix ? 2 : 4);
			else if ((NMD_R(op) == 7 && NMD_C(op) < 4) || op == 0xA4 || op == 0xC2 || (op > 0xC3 && op <= 0xC6) || op == 0xBA || op == 0xAC) /* imm8 */
				offset++;
			else if (op == 0x78 && (repeatNotZeroPrefix || operandPrefix)) /* imm8 + imm8 = "imm16" */
				offset += 2;
		}
	}
	else /* 1 byte opcode */
	{
		op = *b;
		opcodeSize = 1;

		/* Check for ModR/M, SIB and displacement. */
		if (NMD_R(op) == 8 || _nmd_findByte(_nmd_op1modrm, sizeof(_nmd_op1modrm), op) || (NMD_R(op) < 4 && (NMD_C(op) < 4 || (NMD_C(op) >= 8 && NMD_C(op) < 0xC))) || (NMD_R(op) == 0xD && NMD_C(op) >= 8) || ((op == 0xc4 || op == 0xc5) && remainingSize > 1 && ((nmd_x86_modrm*)(b + 1))->fields.mod != 0b11))
		{
			if (!_nmd_ldisasm_parse_modrm(&b, addressPrefix, mode, &modrm, remainingSize - 1))
				return 0;
			hasModrm = true;
		}

#ifndef NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK
		if (op == 0xC6 || op == 0xC7)
		{
			if ((modrm.fields.reg != 0b000 && modrm.fields.reg != 0b111) || (modrm.fields.reg == 0b111 && (modrm.fields.mod != 0b11 || modrm.fields.rm != 0b000)))
				return 0;
		}
		else if (op == 0x8f)
		{
			if (modrm.fields.reg != 0b000)
				return 0;
		}
		else if (op == 0xfe)
		{
			if (modrm.fields.reg >= 0b010)
				return 0;
		}
		else if (op == 0xff)
		{
			if (modrm.fields.reg == 0b111 || (modrm.fields.mod == 0b11 && (modrm.fields.reg == 0b011 || modrm.fields.reg == 0b101)))
				return 0;
		}
		else if (op == 0x8c)
		{
			if (modrm.fields.reg >= 0b110)
				return 0;
		}
		else if (op == 0x8e)
		{
			if (modrm.fields.reg == 0b001 || modrm.fields.reg >= 0b110)
				return 0;
		}
		else if (op == 0x62)
		{
			if (mode == NMD_X86_MODE_64)
				return 0;
		}
		else if (op == 0x8d)
		{
			if (modrm.fields.mod == 0b11)
				return 0;
		}
		else if (op == 0xc4 || op == 0xc5)
		{
			if (mode == NMD_X86_MODE_64 && hasModrm && modrm.fields.mod != 0b11)
				return 0;
		}
		else if (op >= 0xd8 && op <= 0xdf)
		{
			switch (op)
			{
			case 0xd9:
				if ((modrm.fields.reg == 0b001 && modrm.fields.mod != 0b11) || (modrm.modrm > 0xd0 && modrm.modrm < 0xd8) || modrm.modrm == 0xe2 || modrm.modrm == 0xe3 || modrm.modrm == 0xe6 || modrm.modrm == 0xe7 || modrm.modrm == 0xef)
					return 0;
				break;
			case 0xda:
				if (modrm.modrm >= 0xe0 && modrm.modrm != 0xe9)
					return 0;
				break;
			case 0xdb:
				if (((modrm.fields.reg == 0b100 || modrm.fields.reg == 0b110) && modrm.fields.mod != 0b11) || (modrm.modrm >= 0xe5 && modrm.modrm <= 0xe7) || modrm.modrm >= 0xf8)
					return 0;
				break;
			case 0xdd:
				if ((modrm.fields.reg == 0b101 && modrm.fields.mod != 0b11) || NMD_R(modrm.modrm) == 0xf)
					return 0;
				break;
			case 0xde:
				if (modrm.modrm == 0xd8 || (modrm.modrm >= 0xda && modrm.modrm <= 0xdf))
					return 0;
				break;
			case 0xdf:
				if ((modrm.modrm >= 0xe1 && modrm.modrm <= 0xe7) || modrm.modrm >= 0xf8)
					return 0;
				break;
			}
		}
		else if (mode == NMD_X86_MODE_64)
		{
			if (op == 0x6 || op == 0x7 || op == 0xe || op == 0x16 || op == 0x17 || op == 0x1e || op == 0x1f || op == 0x27 || op == 0x2f || op == 0x37 || op == 0x3f || (op >= 0x60 && op <= 0x62) || op == 0x82 || op == 0xce || (op >= 0xd4 && op <= 0xd6))
				return 0;
		}
#endif /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VALIDITY_CHECK */

#ifndef NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VEX
		/* Check if instruction is VEX. */
		if ((op == 0xc4 || op == 0xc5) && !hasModrm)
		{
			const uint8_t byte0 = op;
			if (remainingSize < 4)
				return 0;

			if (byte0 == 0xc4)
			{
				b += 3;
				op = *b;

				if (op == 0x0c || op == 0x0d || op == 0x40 || op == 0x41 || op == 0x17 || op == 0x21 || op == 0x42)
					offset++;
			}
			else /* 0xc5 */
			{
				b += 2;
				op = *b;
			}

			if (!_nmd_ldisasm_parse_modrm(&b, addressPrefix, mode, &modrm, remainingSize - (byte0 == 0xc4 ? 4 : 3)))
				return false;
			hasModrm = true;
		}
		else
#endif /* NMD_ASSEMBLY_DISABLE_LENGTH_DISASSEMBLER_VEX */

		{
			/* Check for immediate */
			if (_nmd_findByte(_nmd_op1imm32, sizeof(_nmd_op1imm32), op) || (NMD_R(op) < 4 && (NMD_C(op) == 5 || NMD_C(op) == 0xD)) || (NMD_R(op) == 0xB && NMD_C(op) >= 8) || (op == 0xF7 && modrm.fields.reg == 0b000)) /* imm32,16 */
			{
				if (NMD_R(op) == 0xB && NMD_C(op) >= 8)
					offset += rexW ? 8 : (operandPrefix || (mode == NMD_X86_MODE_16 && !operandPrefix) ? 2 : 4);
				else
				{
					if ((mode == NMD_X86_MODE_16 && operandPrefix) || (mode != NMD_X86_MODE_16 && !operandPrefix))
						offset += NMD_X86_IMM32;
					else
						offset += NMD_X86_IMM16;
				}
			}
			else if (NMD_R(op) == 7 || (NMD_R(op) == 0xE && NMD_C(op) < 8) || (NMD_R(op) == 0xB && NMD_C(op) < 8) || (NMD_R(op) < 4 && (NMD_C(op) == 4 || NMD_C(op) == 0xC)) || (op == 0xF6 && modrm.fields.reg <= 0b001) || _nmd_findByte(_nmd_op1imm8, sizeof(_nmd_op1imm8), op)) /* imm8 */
				offset++;
			else if (NMD_R(op) == 0xA && NMD_C(op) < 4)
				offset += (mode == NMD_X86_MODE_64) ? (addressPrefix ? 4 : 8) : (addressPrefix ? 2 : 4);
			else if (op == 0xEA || op == 0x9A) /* imm32,48 */
			{
				if (mode == NMD_X86_MODE_64)
					return 0;
				offset += (operandPrefix ? 4 : 6);
			}
			else if (op == 0xC2 || op == 0xCA) /* imm16 */
				offset += 2;
			else if (op == 0xC8) /* imm16 + imm8 */
				offset += 3;
		}
	}

	if (lockPrefix)
	{
		if (!(hasModrm && modrm.fields.mod != 0b11 &&
			((opcodeSize == 1 && (op == 0x86 || op == 0x87 || (NMD_R(op) < 4 && (op % 8) < 2 && op < 0x38) || ((op >= 0x80 && op <= 0x83) && modrm.fields.reg != 0b111) || (op >= 0xfe && modrm.fields.reg < 2) || ((op == 0xf6 || op == 0xf7) && (modrm.fields.reg == 0b010 || modrm.fields.reg == 0b011)))) ||
				(opcodeSize == 2 && (_nmd_findByte(_nmd_twoOpcodes, sizeof(_nmd_twoOpcodes), op) || op == 0xab || (op == 0xba && modrm.fields.reg != 0b100) || (op == 0xc7 && modrm.fields.reg == 0b001))))))
			return 0;
	}

	return (size_t)((ptrdiff_t)(++b + offset) - (ptrdiff_t)(buffer));
}

typedef struct
{
	char* buffer;
	const nmd_x86_instruction* instruction;
	uint64_t runtimeAddress;
	uint32_t formatFlags;
} _nmd_string_info;

void _nmd_append_string(_nmd_string_info* const si, const char* source)
{
	while (*source)
		*si->buffer++ = *source++;
}

size_t _nmd_get_num_digits(uint64_t n, bool hex)
{
	size_t numDigits = 0;
	while ((n /= (hex ? 16 : 10)) > 0)
		numDigits++;

	return numDigits;
}

void _nmd_append_number(_nmd_string_info* const si, uint64_t n)
{
	size_t numDigits = _nmd_get_num_digits(n, si->formatFlags & NMD_X86_FORMAT_FLAGS_HEX);
	size_t bufferOffset = numDigits + 1;

	if (si->formatFlags & NMD_X86_FORMAT_FLAGS_HEX)
	{
		const bool condition = n > 9 || si->formatFlags & NMD_X86_FORMAT_FLAGS_ENFORCE_HEX_ID;
		if (si->formatFlags & NMD_X86_FORMAT_FLAGS_0X_PREFIX && condition)
			*si->buffer++ = '0', *si->buffer++ = 'x';

		const uint8_t baseChar = (uint8_t)(si->formatFlags & NMD_X86_FORMAT_FLAGS_HEX_LOWERCASE ? 0x57 : 0x37);
		do {
			size_t num = n % 16;
			*(si->buffer + numDigits--) = (char)((num > 9 ? baseChar : '0') + num);
		} while ((n /= 16) > 0);

		if (si->formatFlags & NMD_X86_FORMAT_FLAGS_H_SUFFIX && condition)
			*(si->buffer + bufferOffset++) = 'h';
	}
	else
	{
		do {
			*(si->buffer + numDigits--) = (char)('0' + n % 10);
		} while ((n /= 10) > 0);
	}

	si->buffer += bufferOffset;
}

void _nmd_append_signed_number(_nmd_string_info* const si, int64_t n, bool showPositiveSign)
{
	if (n >= 0)
	{
		if (showPositiveSign)
			*si->buffer++ = '+';

		_nmd_append_number(si, (uint64_t)n);
	}
	else
	{
		*si->buffer++ = '-';
		_nmd_append_number(si, (uint64_t)(~n + 1));
	}
}

void _nmd_append_signed_number_memory_view(_nmd_string_info* const si)
{
	_nmd_append_number(si, (si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? 0xFF00 : (si->instruction->mode == NMD_X86_MODE_64 ? 0xFFFFFFFFFFFFFF00 : 0xFFFFFF00)) | si->instruction->immediate);
	if (si->formatFlags & NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_HINT_HEX)
	{
		*si->buffer++ = '(';
		_nmd_append_signed_number(si, (int8_t)(si->instruction->immediate), false);
		*si->buffer++ = ')';
	}
	else if (si->formatFlags & NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_HINT_DEC)
	{
		*si->buffer++ = '(';
		const uint32_t previousMask = si->formatFlags;
		si->formatFlags &= ~NMD_X86_FORMAT_FLAGS_HEX;
		_nmd_append_signed_number(si, (int8_t)(si->instruction->immediate), false);
		si->formatFlags = previousMask;
		*si->buffer++ = ')';
	}
}

void _nmd_append_relative_address8(_nmd_string_info* const si)
{
	if (si->runtimeAddress == (uint64_t)NMD_X86_INVALID_RUNTIME_ADDRESS)
	{
		/* *si->buffer++ = '$'; */
		_nmd_append_signed_number(si, (int64_t)((int8_t)(si->instruction->immediate) + (int8_t)(si->instruction->length)), true);
	}
	else
	{
		uint64_t n;
		if (si->instruction->mode == NMD_X86_MODE_64)
			n = (uint64_t)((int64_t)(si->runtimeAddress + si->instruction->length) + (int8_t)(si->instruction->immediate));
		else if (si->instruction->mode == NMD_X86_MODE_16)
			n = (uint16_t)((int16_t)(si->runtimeAddress + si->instruction->length) + (int8_t)(si->instruction->immediate));
		else
			n = (uint32_t)((int32_t)(si->runtimeAddress + si->instruction->length) + (int8_t)(si->instruction->immediate));
		_nmd_append_number(si, n);
	}
}

void _nmd_append_relative_address16_32(_nmd_string_info* const si)
{
	if (si->runtimeAddress == (uint64_t)NMD_X86_INVALID_RUNTIME_ADDRESS)
	{
		/* *si->buffer++ = '$'; */
		_nmd_append_signed_number(si, (int64_t)((int32_t)(si->instruction->immediate) + (int32_t)(si->instruction->length)), true);
	}
	else
		_nmd_append_number(si, ((si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && si->instruction->mode == NMD_X86_MODE_32) || (si->instruction->mode == NMD_X86_MODE_16 && !(si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? 0xFFFF : 0xFFFFFFFFFFFFFFFF) & (si->instruction->mode == NMD_X86_MODE_64 ?
			(uint64_t)((uint64_t)((int64_t)(si->runtimeAddress + si->instruction->length) + (int32_t)(si->instruction->immediate))) :
			(uint64_t)((uint32_t)((int32_t)(si->runtimeAddress + si->instruction->length) + (int32_t)(si->instruction->immediate)))
		));
}

void _nmd_append_modrm_memory_prefix(_nmd_string_info* const si, const char* addrSpecifierReg)
{
#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_POINTER_SIZE
	if (si->formatFlags & NMD_X86_FORMAT_FLAGS_POINTER_SIZE)
	{
		_nmd_append_string(si, addrSpecifierReg);
		_nmd_append_string(si, " ptr ");
	}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_POINTER_SIZE */

	if (!(si->formatFlags & NMD_X86_FORMAT_FLAGS_ONLY_SEGMENT_OVERRIDE && !si->instruction->segmentOverride))
	{
		size_t i = 0;
		if (si->instruction->segmentOverride)
			i = _nmd_get_bit_index(si->instruction->segmentOverride);

		_nmd_append_string(si, si->instruction->segmentOverride ? _nmd_segmentReg[i] : (!(si->instruction->prefixes & NMD_X86_PREFIXES_REX_B) && (si->instruction->modrm.fields.rm == 0b100 || si->instruction->modrm.fields.rm == 0b101) ? "ss" : "ds"));
		*si->buffer++ = ':';
	}
}

void _nmd_append_modrm16_upper(_nmd_string_info* const si)
{
	*si->buffer++ = '[';

	if (!(si->instruction->modrm.fields.mod == 0b00 && si->instruction->modrm.fields.rm == 0b110))
	{
		const char* addresses[] = { "bx+si", "bx+di", "bp+si", "bp+di", "si", "di", "bp", "bx" };
		_nmd_append_string(si, addresses[si->instruction->modrm.fields.rm]);
	}

	if (si->instruction->dispMask != NMD_X86_DISP_NONE && (si->instruction->displacement != 0 || *(si->buffer - 1) == '['))
	{
		if (si->instruction->modrm.fields.mod == 0b00 && si->instruction->modrm.fields.rm == 0b110)
			_nmd_append_number(si, si->instruction->displacement);
		else
		{
			const bool isNegative = si->instruction->displacement & (1 << (si->instruction->dispMask * 8 - 1));
			if (*(si->buffer - 1) != '[')
				*si->buffer++ = isNegative ? '-' : '+';

			if (isNegative)
			{
				const uint16_t mask = (uint16_t)(si->instruction->dispMask == 2 ? 0xFFFF : 0xFF);
				_nmd_append_number(si, (uint64_t)(~si->instruction->displacement & mask) + 1);
			}
			else
				_nmd_append_number(si, si->instruction->displacement);
		}
	}

	*si->buffer++ = ']';
}

void _nmd_append_modrm32_upper(_nmd_string_info* const si)
{
	*si->buffer++ = '[';

	if (si->instruction->hasSIB)
	{
		if (si->instruction->sib.fields.base == 0b101)
		{
			if (si->instruction->modrm.fields.mod != 0b00)
				_nmd_append_string(si, si->instruction->mode == NMD_X86_MODE_64 && !(si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (si->instruction->prefixes & NMD_X86_PREFIXES_REX_B ? "r13" : "rbp") : "ebp");
		}
		else
			_nmd_append_string(si, (si->instruction->mode == NMD_X86_MODE_64 && !(si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (si->instruction->prefixes & NMD_X86_PREFIXES_REX_B ? _nmd_regrx : _nmd_reg64) : _nmd_reg32)[si->instruction->sib.fields.base]);

		if (si->instruction->sib.fields.index != 0b100)
		{
			if (!(si->instruction->sib.fields.base == 0b101 && si->instruction->modrm.fields.mod == 0b00))
				*si->buffer++ = '+';
			_nmd_append_string(si, (si->instruction->mode == NMD_X86_MODE_64 && !(si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (si->instruction->prefixes & NMD_X86_PREFIXES_REX_X ? _nmd_regrx : _nmd_reg64) : _nmd_reg32)[si->instruction->sib.fields.index]);
			if (!(si->instruction->sib.fields.scale == 0b00 && !(si->formatFlags & NMD_X86_FORMAT_FLAGS_SCALE_ONE)))
				*si->buffer++ = '*', *si->buffer++ = (char)('0' + (1 << si->instruction->sib.fields.scale));
		}

		if (si->instruction->prefixes & NMD_X86_PREFIXES_REX_X && si->instruction->sib.fields.index == 0b100)
		{
			if (*(si->buffer - 1) != '[')
				*si->buffer++ = '+';
			_nmd_append_string(si, "r12");
			if (!(si->instruction->sib.fields.scale == 0b00 && !(si->formatFlags & NMD_X86_FORMAT_FLAGS_SCALE_ONE)))
				*si->buffer++ = '*', *si->buffer++ = (char)('0' + (1 << si->instruction->sib.fields.scale));
		}
	}
	else if (!(si->instruction->modrm.fields.mod == 0b00 && si->instruction->modrm.fields.rm == 0b101))
	{
		if ((si->instruction->prefixes & (NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE | NMD_X86_PREFIXES_REX_B)) == (NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE | NMD_X86_PREFIXES_REX_B) && si->instruction->mode == NMD_X86_MODE_64)
			_nmd_append_string(si, _nmd_regrx[si->instruction->modrm.fields.rm]), *si->buffer++ = 'd';
		else
			_nmd_append_string(si, (si->instruction->mode == NMD_X86_MODE_64 && !(si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE) ? (si->instruction->prefixes & NMD_X86_PREFIXES_REX_B ? _nmd_regrx : _nmd_reg64) : _nmd_reg32)[si->instruction->modrm.fields.rm]);
	}

	/* Handle displacement. */
	if (si->instruction->dispMask != NMD_X86_DISP_NONE && (si->instruction->displacement != 0 || *(si->buffer - 1) == '['))
	{
		/* Relative address. */
		if (si->instruction->modrm.fields.rm == 0b101 && si->instruction->mode == NMD_X86_MODE_64 && si->instruction->modrm.fields.mod == 0b00 && si->runtimeAddress != NMD_X86_INVALID_RUNTIME_ADDRESS)
		{
			if (si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE)
				_nmd_append_number(si, (uint32_t)((int32_t)(si->runtimeAddress + si->instruction->length) + (int32_t)si->instruction->displacement));
			else
				_nmd_append_number(si, (uint64_t)((int64_t)(si->runtimeAddress + si->instruction->length) + (int64_t)((int32_t)si->instruction->displacement)));
		}
		else if (si->instruction->modrm.fields.mod == 0b00 && ((si->instruction->sib.fields.base == 0b101 && si->instruction->sib.fields.index == 0b100) || si->instruction->modrm.fields.rm == 0b101) && *(si->buffer - 1) == '[')
			_nmd_append_number(si, si->instruction->mode == NMD_X86_MODE_64 ? 0xFFFFFFFF00000000 | si->instruction->displacement : si->instruction->displacement);
		else
		{
			if (si->instruction->modrm.fields.rm == 0b101 && si->instruction->mode == NMD_X86_MODE_64 && si->instruction->modrm.fields.mod == 0b00)
				_nmd_append_string(si, si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE ? "eip" : "rip");

			const bool isNegative = si->instruction->displacement & (1 << (si->instruction->dispMask * 8 - 1));
			if (*(si->buffer - 1) != '[')
				*si->buffer++ = isNegative ? '-' : '+';

			if (isNegative)
			{
				const uint32_t mask = (uint32_t)(si->instruction->dispMask == 4 ? -1 : (1 << (si->instruction->dispMask * 8)) - 1);
				_nmd_append_number(si, (uint64_t)(~si->instruction->displacement & mask) + 1);
			}
			else
				_nmd_append_number(si, si->instruction->displacement);
		}
	}

	*si->buffer++ = ']';
}

void _nmd_append_modrm_upper(_nmd_string_info* const si, const char* addrSpecifierReg)
{
	_nmd_append_modrm_memory_prefix(si, addrSpecifierReg);

	if ((si->instruction->mode == NMD_X86_MODE_16 && !(si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE)) || (si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE && si->instruction->mode == NMD_X86_MODE_32))
		_nmd_append_modrm16_upper(si);
	else
		_nmd_append_modrm32_upper(si);
}

void _nmd_append_modrm_upper_without_address_specifier(_nmd_string_info* const si)
{
	if ((si->instruction->mode == NMD_X86_MODE_16 && !(si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE)) || (si->instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE && si->instruction->mode == NMD_X86_MODE_32))
		_nmd_append_modrm16_upper(si);
	else
		_nmd_append_modrm32_upper(si);
}

void _nmd_append_Nq(_nmd_string_info* const si)
{
	*si->buffer++ = 'm', *si->buffer++ = 'm';
	*si->buffer++ = (char)('0' + si->instruction->modrm.fields.rm);
}

void _nmd_append_Pq(_nmd_string_info* const si)
{
	*si->buffer++ = 'm', *si->buffer++ = 'm';
	*si->buffer++ = (char)('0' + si->instruction->modrm.fields.reg);
}

void _nmd_append_avx_register_reg(_nmd_string_info* const si)
{
	*si->buffer++ = si->instruction->vex.L ? 'y' : 'x';
	_nmd_append_Pq(si);
}

void _nmd_append_avx_vvvv_register(_nmd_string_info* const si)
{
	*si->buffer++ = si->instruction->vex.L ? 'y' : 'x';
	*si->buffer++ = 'm', *si->buffer++ = 'm';
	if ((15 - si->instruction->vex.vvvv) > 9)
		*si->buffer++ = '1', *si->buffer++ = (char)(0x26 + (15 - si->instruction->vex.vvvv));
	else
		*si->buffer++ = (char)('0' + (15 - si->instruction->vex.vvvv));
}

void _nmd_append_Vdq(_nmd_string_info* const si)
{
	*si->buffer++ = 'x';
	_nmd_append_Pq(si);
}

void _nmd_append_Vqq(_nmd_string_info* const si)
{
	*si->buffer++ = 'y';
	_nmd_append_Pq(si);
}

void _nmd_append_Vx(_nmd_string_info* const si)
{
	if (si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		_nmd_append_Vdq(si);
	else
		_nmd_append_Vqq(si);
}

void _nmd_append_Udq(_nmd_string_info* const si)
{
	*si->buffer++ = 'x';
	_nmd_append_Nq(si);
}

void _nmd_append_Uqq(_nmd_string_info* const si)
{
	*si->buffer++ = 'y';
	_nmd_append_Nq(si);
}

void _nmd_append_Ux(_nmd_string_info* const si)
{
	if (si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		_nmd_append_Udq(si);
	else
		_nmd_append_Uqq(si);
}

void _nmd_append_Qq(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
		_nmd_append_Nq(si);
	else
		_nmd_append_modrm_upper(si, "qword");
}

void _nmd_append_Ev(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
	{
		if (si->instruction->prefixes & NMD_X86_PREFIXES_REX_B)
		{
			_nmd_append_string(si, _nmd_regrx[si->instruction->modrm.fields.rm]);
			if (!(si->instruction->prefixes & NMD_X86_PREFIXES_REX_W))
				*si->buffer++ = 'd';
		}
		else
			_nmd_append_string(si, ((si->instruction->operandSize64 ? _nmd_reg64 : (si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && si->instruction->mode != NMD_X86_MODE_16) || (si->instruction->mode == NMD_X86_MODE_16 && !(si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? _nmd_reg16 : _nmd_reg32))[si->instruction->modrm.fields.rm]);
	}
	else
		_nmd_append_modrm_upper(si, (si->instruction->operandSize64) ? "qword" : ((si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && si->instruction->mode != NMD_X86_MODE_16) || (si->instruction->mode == NMD_X86_MODE_16 && !(si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? "word" : "dword"));
}

void _nmd_append_Ey(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
		_nmd_append_string(si, (si->instruction->operandSize64 ? _nmd_reg64 : _nmd_reg32)[si->instruction->modrm.fields.rm]);
	else
		_nmd_append_modrm_upper(si, si->instruction->operandSize64 ? "qword" : "dword");
}

void _nmd_append_Eb(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
	{
		if (si->instruction->prefixes & NMD_X86_PREFIXES_REX_B)
			_nmd_append_string(si, _nmd_regrx[si->instruction->modrm.fields.rm]), *si->buffer++ = 'b';
		else
			_nmd_append_string(si, (si->instruction->hasRex ? _nmd_reg8_x64 : _nmd_reg8)[si->instruction->modrm.fields.rm]);
	}
	else
		_nmd_append_modrm_upper(si, "byte");
}

void _nmd_append_Ew(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
		_nmd_append_string(si, _nmd_reg16[si->instruction->modrm.fields.rm]);
	else
		_nmd_append_modrm_upper(si, "word");
}

void _nmd_append_Ed(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
		_nmd_append_string(si, _nmd_reg32[si->instruction->modrm.fields.rm]);
	else
		_nmd_append_modrm_upper(si, "dword");
}

void _nmd_append_Eq(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
		_nmd_append_string(si, _nmd_reg64[si->instruction->modrm.fields.rm]);
	else
		_nmd_append_modrm_upper(si, "qword");
}

void _nmd_append_Rv(_nmd_string_info* const si)
{
	_nmd_append_string(si, (si->instruction->operandSize64 ? _nmd_reg64 : (si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? _nmd_reg16 : _nmd_reg32))[si->instruction->modrm.fields.rm]);
}

void _nmd_append_Gv(_nmd_string_info* const si)
{
	if (si->instruction->prefixes & NMD_X86_PREFIXES_REX_R)
	{
		_nmd_append_string(si, _nmd_regrx[si->instruction->modrm.fields.reg]);
		if (!(si->instruction->prefixes & NMD_X86_PREFIXES_REX_W))
			*si->buffer++ = 'd';
	}
	else
		_nmd_append_string(si, ((si->instruction->operandSize64) ? _nmd_reg64 : ((si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && si->instruction->mode != NMD_X86_MODE_16) || (si->instruction->mode == NMD_X86_MODE_16 && !(si->instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? _nmd_reg16 : _nmd_reg32))[si->instruction->modrm.fields.reg]);
}

void _nmd_append_Gy(_nmd_string_info* const si)
{
	_nmd_append_string(si, (si->instruction->operandSize64 ? _nmd_reg64 : _nmd_reg32)[si->instruction->modrm.fields.reg]);
}

void _nmd_append_Gb(_nmd_string_info* const si)
{
	if (si->instruction->prefixes & NMD_X86_PREFIXES_REX_R)
		_nmd_append_string(si, _nmd_regrx[si->instruction->modrm.fields.reg]), *si->buffer++ = 'b';
	else
		_nmd_append_string(si, (si->instruction->hasRex ? _nmd_reg8_x64 : _nmd_reg8)[si->instruction->modrm.fields.reg]);
}

void _nmd_append_Gw(_nmd_string_info* const si)
{
	_nmd_append_string(si, _nmd_reg16[si->instruction->modrm.fields.reg]);
}

void _nmd_append_W(_nmd_string_info* const si)
{
	if (si->instruction->modrm.fields.mod == 0b11)
		_nmd_append_string(si, "xmm"), *si->buffer++ = (char)('0' + si->instruction->modrm.fields.rm);
	else
		_nmd_append_modrm_upper(si, "xmmword");
}

#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_ATT_SYNTAX
char* _nmd_format_operand_to_att(char* operand, _nmd_string_info* si)
{
	char* nextOperand = (char*)_nmd_strchr(operand, ',');
	const char* operandEnd = nextOperand ? nextOperand : si->buffer;

	/* Memory operand. */
	const char* memoryOperand = _nmd_strchr(operand, '[');
	if (memoryOperand && memoryOperand < operandEnd)
	{
		memoryOperand++;
		const char* segReg = _nmd_strchr(operand, ':');
		if (segReg)
		{
			if (segReg == operand + 2)
				_nmd_insert_char(operand, '%'), si->buffer++, operand += 4;
			else
			{
				*operand++ = '%';
				*operand++ = *(segReg - 2);
				*operand++ = 's';
				*operand++ = ':';
			}
		}

		/* Handle displacement. */
		char* displacement = operand;
		do
		{
			displacement++;
			displacement = (char*)_nmd_find_number(displacement, operandEnd);
		} while (displacement && ((*(displacement - 1) != '+' && *(displacement - 1) != '-' && *(displacement - 1) != '[') || !_nmd_is_number(displacement, operandEnd - 2)));

		bool isThereBaseOrIndex = true;
		char memoryOperandBuffer[96];

		if (displacement)
		{
			if (*(displacement - 1) != '[')
				displacement--;
			else
				isThereBaseOrIndex = false;

			char* i = (char*)memoryOperand;
			char* j = memoryOperandBuffer;
			for (; i < displacement; i++, j++)
				*j = *i;
			*j = '\0';

			if (*displacement == '+')
				displacement++;

			for (; *displacement != ']'; displacement++, operand++)
				*operand = *displacement;
		}

		/* Handle base, index and scale. */
		if (isThereBaseOrIndex)
		{
			*operand++ = '(';

			char* baseOrIndex = operand;
			if (displacement)
			{
				char* s = memoryOperandBuffer;
				for (; *s; s++, operand++)
					*operand = *s;
			}
			else
			{
				for (; *memoryOperand != ']'; operand++, memoryOperand++)
					*operand = *memoryOperand;
			}

			_nmd_insert_char(baseOrIndex, '%');
			operand++;
			*operand++ = ')';

			for (; *baseOrIndex != ')'; baseOrIndex++)
			{
				if (*baseOrIndex == '+' || *baseOrIndex == '*')
				{
					if (*baseOrIndex == '+')
						_nmd_insert_char(baseOrIndex + 1, '%'), operand++;
					*baseOrIndex = ',';
				}
			}

			operand = baseOrIndex;
			operand++;
		}

		if (nextOperand)
		{
			/* Move second operand to the left until the comma. */
			operandEnd = _nmd_strchr(operand, ',');
			for (; *operandEnd != '\0'; operand++, operandEnd++)
				*operand = *operandEnd;

			*operand = '\0';

			operandEnd = operand;
			while (*operandEnd != ',')
				operandEnd--;
		}
		else
			*operand = '\0', operandEnd = operand;

		si->buffer = operand;

		return (char*)operandEnd;
	}
	else /* Immediate or register operand. */
	{
		_nmd_insert_char(operand, _nmd_is_number(operand, operandEnd) ? '$' : '%');
		si->buffer++;
		return (char*)operandEnd + 1;
	}
}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_ATT_SYNTAX */

/*
Formats an instruction. This function may cause a crash if you modify 'instruction' manually.
Parameters:
 - instruction    [in]  A pointer to a variable of type 'nmd_x86_instruction' describing the instruction to be formatted.
 - buffer         [out] A pointer to buffer that receives the string. The buffer's recommended size is 128 bytes.
 - runtimeAddress [in]  The instruction's runtime address. You may use 'NMD_X86_INVALID_RUNTIME_ADDRESS'.
 - formatFlags    [in]  A mask of 'NMD_X86_FORMAT_FLAGS_XXX' that specifies how the function should format the instruction. If uncertain, use 'NMD_X86_FORMAT_FLAGS_DEFAULT'.
*/
void nmd_x86_format_instruction(const nmd_x86_instruction* instruction, char* buffer, uint64_t runtimeAddress, uint32_t formatFlags)
{
	if (!instruction->valid)
		return;

	_nmd_string_info si;
	si.buffer = buffer;
	si.instruction = instruction;
	si.runtimeAddress = runtimeAddress;
	si.formatFlags = formatFlags;

#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_BYTES
	if (formatFlags & NMD_X86_FORMAT_FLAGS_BYTES)
	{
		size_t i = 0;
		for (; i < instruction->length; i++)
		{
			uint8_t num = instruction->buffer[i] >> 4;
			*si.buffer++ = (char)((num > 9 ? 0x37 : '0') + num);
			num = instruction->buffer[i] & 0xf;
			*si.buffer++ = (char)((num > 9 ? 0x37 : '0') + num);
			*si.buffer++ = ' ';
		}

		const size_t numPaddingBytes = instruction->length < NMD_X86_FORMATTER_NUM_PADDING_BYTES ? (NMD_X86_FORMATTER_NUM_PADDING_BYTES - instruction->length) : 0;
		for (i = 0; i < numPaddingBytes * 3; i++)
			*si.buffer++ = ' ';
	}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_BYTES */

	const uint8_t op = instruction->opcode;

	if (instruction->prefixes & (NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO) && (instruction->prefixes & NMD_X86_PREFIXES_LOCK || ((op == 0x86 || op == 0x87) && instruction->modrm.fields.mod != 0b11)))
		_nmd_append_string(&si, instruction->repeatPrefix ? "xrelease " : "xacquire ");
	else if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT_NOT_ZERO && (instruction->opcodeSize == 1 && (op == 0xc2 || op == 0xc3 || op == 0xe8 || op == 0xe9 || NMD_R(op) == 7 || (op == 0xff && (instruction->modrm.fields.reg == 0b010 || instruction->modrm.fields.reg == 0b100)))))
		_nmd_append_string(&si, "bnd ");

	if (instruction->prefixes & NMD_X86_PREFIXES_LOCK)
		_nmd_append_string(&si, "lock ");

	const bool operandSize = instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE;

	if (instruction->opcodeMap == NMD_X86_OPCODE_MAP_DEFAULT)
	{
#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_EVEX
		if (instruction->encoding == NMD_X86_ENCODING_EVEX)
		{

		}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_EVEX */

#if !defined(NMD_ASSEMBLY_DISABLE_FORMATTER_EVEX) && !defined(NMD_ASSEMBLY_DISABLE_FORMATTER_VEX)
		else
#endif
#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_VEX
			if (instruction->encoding == NMD_X86_ENCODING_VEX)
			{
				if (instruction->vex.vex[0] == 0xc4)
				{
					if (instruction->opcode == 0x0c || instruction->opcode == 0x0d || instruction->opcode == 0x4a || instruction->opcode == 0x4b)
					{
						_nmd_append_string(&si, instruction->opcode == 0x0c ? "vblendps" : (instruction->opcode == 0x0c ? "vblendpd" : (instruction->opcode == 0x4a ? "vblendvps" : "vblendvpd")));
						*si.buffer++ = ' ';

						_nmd_append_avx_register_reg(&si);
						*si.buffer++ = ',';

						_nmd_append_avx_vvvv_register(&si);
						*si.buffer++ = ',';

						_nmd_append_W(&si);
						*si.buffer++ = ',';

						if(instruction->opcode <= 0x0d)
							_nmd_append_number(&si, instruction->immediate);
						else
						{
							_nmd_append_string(&si, "xmm");
							*si.buffer++ = (char)('0' + ((instruction->immediate & 0xf0) >> 4) % 8);
						}
					}
					else if (instruction->opcode == 0x40 || instruction->opcode == 0x41)
					{
						_nmd_append_string(&si, instruction->opcode == 0x40 ? "vdpps" : "vdppd");
						*si.buffer++ = ' ';

						_nmd_append_avx_register_reg(&si);
						*si.buffer++ = ',';

						_nmd_append_avx_vvvv_register(&si);
						*si.buffer++ = ',';

						_nmd_append_W(&si);
						*si.buffer++ = ',';

						_nmd_append_number(&si, instruction->immediate);
					}
					else if (instruction->opcode == 0x17)
					{
						_nmd_append_string(&si, "vextractps ");

						_nmd_append_Ev(&si);
						*si.buffer++ = ',';

						_nmd_append_Vdq(&si);
						*si.buffer++ = ',';

						_nmd_append_number(&si, instruction->immediate);
					}
					else if (instruction->opcode == 0x21)
					{
						_nmd_append_string(&si, "vinsertps ");

						_nmd_append_Vdq(&si);
						*si.buffer++ = ',';

						_nmd_append_avx_vvvv_register(&si);
						*si.buffer++ = ',';

						_nmd_append_W(&si);
						*si.buffer++ = ',';

						_nmd_append_number(&si, instruction->immediate);
					}
					else if (instruction->opcode == 0x2a)
					{
						_nmd_append_string(&si, "vmovntdqa ");

						_nmd_append_Vdq(&si);
						*si.buffer++ = ',';

						_nmd_append_modrm_upper_without_address_specifier(&si);
					}
					else if (instruction->opcode == 0x42)
					{
						_nmd_append_string(&si, "vmpsadbw ");

						_nmd_append_Vdq(&si);
						*si.buffer++ = ',';

						_nmd_append_avx_vvvv_register(&si);
						*si.buffer++ = ',';

						if (si.instruction->modrm.fields.mod == 0b11)
							_nmd_append_string(&si, "xmm"), *si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
						else
							_nmd_append_modrm_upper_without_address_specifier(&si);
						*si.buffer++ = ',';

						_nmd_append_number(&si, instruction->immediate);
					}
				}
			}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_VEX */
			
#if (!defined(NMD_ASSEMBLY_DISABLE_FORMATTER_EVEX) || !defined(NMD_ASSEMBLY_DISABLE_FORMATTER_VEX)) && !defined(NMD_ASSEMBLY_DISABLE_FORMATTER_3DNOW)
		else
#endif
#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_3DNOW
		if (instruction->encoding == NMD_X86_ENCODING_3DNOW)
		{
			const char* mnemonic = 0;
			switch (instruction->opcode)
			{
			case 0x0c: mnemonic = "pi2fw"; break;
			case 0x0d: mnemonic = "pi2fd"; break;
			case 0x1c: mnemonic = "pf2iw"; break;
			case 0x1d: mnemonic = "pf2id"; break;
			case 0x8a: mnemonic = "pfnacc"; break;
			case 0x8e: mnemonic = "pfpnacc"; break;
			case 0x90: mnemonic = "pfcmpge"; break;
			case 0x94: mnemonic = "pfmin"; break;
			case 0x96: mnemonic = "pfrcp"; break;
			case 0x97: mnemonic = "pfrsqrt"; break;
			case 0x9a: mnemonic = "pfsub"; break;
			case 0x9e: mnemonic = "pfadd"; break;
			case 0xa0: mnemonic = "pfcmpgt"; break;
			case 0xa4: mnemonic = "pfmax"; break;
			case 0xa6: mnemonic = "pfrcpit1"; break;
			case 0xa7: mnemonic = "pfrsqit1"; break;
			case 0xaa: mnemonic = "pfsubr"; break;
			case 0xae: mnemonic = "pfacc"; break;
			case 0xb0: mnemonic = "pfcmpeq"; break;
			case 0xb4: mnemonic = "pfmul"; break;
			case 0xb6: mnemonic = "pfrcpit2"; break;
			case 0xb7: mnemonic = "pmulhrw"; break;
			case 0xbb: mnemonic = "pswapd"; break;
			case 0xbf: mnemonic = "pavgusb"; break;
			}

			_nmd_append_string(&si, mnemonic);
			*si.buffer++ = ' ';

			_nmd_append_Pq(&si);
			*si.buffer++ = ',';
			_nmd_append_Qq(&si);
		}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_3DNOW */

#if !defined(NMD_ASSEMBLY_DISABLE_FORMATTER_EVEX) || !defined(NMD_ASSEMBLY_DISABLE_FORMATTER_VEX) || !defined(NMD_ASSEMBLY_DISABLE_FORMATTER_3DNOW)
			else /*if (instruction->encoding == INSTRUCTION_ENCODING_LEGACY) */
#endif
			{
				if (op >= 0x88 && op <= 0x8c) /* mov [88,8c] */
				{
					_nmd_append_string(&si, "mov ");
					if (op == 0x8b)
					{
						_nmd_append_Gv(&si);
						*si.buffer++ = ',';
						_nmd_append_Ev(&si);
					}
					else if (op == 0x89)
					{
						_nmd_append_Ev(&si);
						*si.buffer++ = ',';
						_nmd_append_Gv(&si);
					}
					else if (op == 0x88)
					{
						_nmd_append_Eb(&si);
						*si.buffer++ = ',';
						_nmd_append_Gb(&si);
					}
					else if (op == 0x8a)
					{
						_nmd_append_Gb(&si);
						*si.buffer++ = ',';
						_nmd_append_Eb(&si);
					}
					else if (op == 0x8c)
					{
						if (si.instruction->modrm.fields.mod == 0b11)
							_nmd_append_string(&si, (si.instruction->operandSize64 ? _nmd_reg64 : (si.instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || instruction->mode == NMD_X86_MODE_16 ? _nmd_reg16 : _nmd_reg32))[si.instruction->modrm.fields.rm]);
						else
							_nmd_append_modrm_upper(&si, "word");

						*si.buffer++ = ',';
						_nmd_append_string(&si, _nmd_segmentReg[instruction->modrm.fields.reg]);
					}
				}
				else if (op == 0x68 || op == 0x6A) /* push */
				{
					_nmd_append_string(&si, "push ");
					if (op == 0x6a)
					{
						if (formatFlags & NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_MEMORY_VIEW && instruction->immediate >= 0x80)
							_nmd_append_signed_number_memory_view(&si);
						else
							_nmd_append_signed_number(&si, (int8_t)instruction->immediate, false);
					}
					else
						_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0xff) /* Opcode extensions Group 5 */
				{
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp5[instruction->modrm.fields.reg]);
					*si.buffer++ = ' ';
					if (instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, (si.instruction->operandSize64 ? _nmd_reg64 : (operandSize ? _nmd_reg16 : _nmd_reg32))[si.instruction->modrm.fields.rm]);
					else
						_nmd_append_modrm_upper(&si, (instruction->modrm.fields.reg == 0b011 || instruction->modrm.fields.reg == 0b101) ? "fword" : (instruction->mode == NMD_X86_MODE_64 && ((instruction->modrm.fields.reg >= 0b010 && instruction->modrm.fields.reg <= 0b110) || (instruction->prefixes & NMD_X86_PREFIXES_REX_W && instruction->modrm.fields.reg <= 0b010)) ? "qword" : (operandSize ? "word" : "dword")));
				}
				else if (NMD_R(op) < 4 && (NMD_C(op) < 6 || (NMD_C(op) >= 8 && NMD_C(op) < 0xE))) /* add,adc,and,xor,or,sbb,sub,cmp */
				{
					_nmd_append_string(&si, _nmd_op1OpcodeMapMnemonics[NMD_R((NMD_C(op) > 6 ? op + 0x40 : op))]);
					*si.buffer++ = ' ';

					switch (op % 8)
					{
					case 0:
						_nmd_append_Eb(&si);
						*si.buffer++ = ',';
						_nmd_append_Gb(&si);
						break;
					case 1:
						_nmd_append_Ev(&si);
						*si.buffer++ = ',';
						_nmd_append_Gv(&si);
						break;
					case 2:
						_nmd_append_Gb(&si);
						*si.buffer++ = ',';
						_nmd_append_Eb(&si);
						break;
					case 3:
						_nmd_append_Gv(&si);
						*si.buffer++ = ',';
						_nmd_append_Ev(&si);
						break;
					case 4:
						_nmd_append_string(&si, "al,");
						_nmd_append_number(&si, instruction->immediate);
						break;
					case 5:
						_nmd_append_string(&si, instruction->operandSize64 ? "rax" : (operandSize ? "ax" : "eax"));
						*si.buffer++ = ',';
						_nmd_append_number(&si, instruction->immediate);
						break;
					}
				}
				else if (NMD_R(op) == 4 || NMD_R(op) == 5) /* inc,dec,push,pop [0x40, 0x5f] */
				{
					_nmd_append_string(&si, NMD_C(op) < 8 ? (NMD_R(op) == 4 ? "inc " : "push ") : (NMD_R(op) == 4 ? "dec " : "pop "));
					_nmd_append_string(&si, (instruction->prefixes & NMD_X86_PREFIXES_REX_B ? _nmd_regrx : (instruction->mode == NMD_X86_MODE_64 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE) ? _nmd_reg64 : (operandSize ? _nmd_reg16 : _nmd_reg32)))[op % 8]);
				}
				else if (op >= 0x80 && op < 0x84) /* add,adc,and,xor,or,sbb,sub,cmp [80,83] */
				{
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp1[instruction->modrm.fields.reg]);
					*si.buffer++ = ' ';
					if (op == 0x80 || op == 0x82)
						_nmd_append_Eb(&si);
					else
						_nmd_append_Ev(&si);
					*si.buffer++ = ',';
					if (op == 0x83)
					{
						if ((instruction->modrm.fields.reg == 0b001 || instruction->modrm.fields.reg == 0b100 || instruction->modrm.fields.reg == 0b110) && instruction->immediate >= 0x80)
							_nmd_append_number(&si, (instruction->prefixes & NMD_X86_PREFIXES_REX_W ? 0xFFFFFFFFFFFFFF00 : (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE || instruction->mode == NMD_X86_MODE_16 ? 0xFF00 : 0xFFFFFF00)) | instruction->immediate);
						else
							_nmd_append_signed_number(&si, (int8_t)(instruction->immediate), false);
					}
					else
						_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0xe8 || op == 0xe9 || op == 0xeb) /* call,jmp */
				{
					_nmd_append_string(&si, op == 0xe8 ? "call " : "jmp ");
					if (op == 0xeb)
						_nmd_append_relative_address8(&si);
					else
						_nmd_append_relative_address16_32(&si);
				}
				else if (op >= 0xA0 && op < 0xA4) /* mov [a0, a4] */
				{
					_nmd_append_string(&si, "mov ");
					if (op == 0xa0)
					{
						_nmd_append_string(&si, "al,");
						_nmd_append_modrm_memory_prefix(&si, "byte");
						*si.buffer++ = '[';
						_nmd_append_number(&si, (instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE || instruction->mode == NMD_X86_MODE_16 ? 0xFFFF : 0xFFFFFFFFFFFFFFFF) & instruction->immediate);
						*si.buffer++ = ']';
					}
					else if (op == 0xa1)
					{
						_nmd_append_string(&si, instruction->operandSize64 ? "rax," : (operandSize ? "ax," : "eax,"));
						_nmd_append_modrm_memory_prefix(&si, instruction->operandSize64 ? "qword" : (operandSize ? "word" : "dword"));
						*si.buffer++ = '[';
						_nmd_append_number(&si, (instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE || instruction->mode == NMD_X86_MODE_16 ? 0xFFFF : 0xFFFFFFFFFFFFFFFF) & instruction->immediate);
						*si.buffer++ = ']';
					}
					else if (op == 0xa2)
					{
						_nmd_append_modrm_memory_prefix(&si, "byte");
						*si.buffer++ = '[';
						_nmd_append_number(&si, (instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE || instruction->mode == NMD_X86_MODE_16 ? 0xFFFF : 0xFFFFFFFFFFFFFFFF) & instruction->immediate);
						_nmd_append_string(&si, "],al");
					}
					else if (op == 0xa3)
					{
						_nmd_append_modrm_memory_prefix(&si, instruction->operandSize64 ? "qword" : (operandSize ? "word" : "dword"));
						*si.buffer++ = '[';
						_nmd_append_number(&si, (instruction->prefixes & NMD_X86_PREFIXES_ADDRESS_SIZE_OVERRIDE || instruction->mode == NMD_X86_MODE_16 ? 0xFFFF : 0xFFFFFFFFFFFFFFFF) & instruction->immediate);
						_nmd_append_string(&si, "],");
						_nmd_append_string(&si, instruction->operandSize64 ? "rax" : (operandSize ? "ax" : "eax"));
					}
				}
				else if(op == 0xcc) /* int3 */
					_nmd_append_string(&si, "int3");
				else if (op == 0x8d) /* lea */
				{
					_nmd_append_string(&si, "lea ");
					_nmd_append_Gv(&si);
					*si.buffer++ = ',';
					_nmd_append_modrm_upper_without_address_specifier(&si);
				}
				else if (op == 0x8f) /* pop */
				{
					_nmd_append_string(&si, "pop ");
					if (instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, (operandSize ? _nmd_reg16 : _nmd_reg32)[instruction->modrm.fields.rm]);
					else
						_nmd_append_modrm_upper(&si, instruction->mode == NMD_X86_MODE_64 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE) ? "qword" : (operandSize ? "word" : "dword"));
				}
				else if (NMD_R(op) == 7) /* conditional jump [70,7f]*/
				{
					*si.buffer++ = 'j';
					_nmd_append_string(&si, _nmd_conditionSuffixes[NMD_C(op)]);
					*si.buffer++ = ' ';
					_nmd_append_relative_address8(&si);
				}
				else if (op == 0xa8) /* test */
				{
					_nmd_append_string(&si, "test al,");
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0xa9) /* test */
				{
					_nmd_append_string(&si, instruction->operandSize64 ? "test rax" : (operandSize ? "test ax" : "test eax"));
					*si.buffer++ = ',';
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0x90)
				{
					if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT)
						_nmd_append_string(&si, "pause");
					else if (instruction->prefixes & NMD_X86_PREFIXES_REX_B)
						_nmd_append_string(&si, instruction->prefixes & NMD_X86_PREFIXES_REX_W ? "xchg r8,rax" : "xchg r8d,eax");
					else
						_nmd_append_string(&si, "nop");
				}
				else if(op == 0xc3)
					_nmd_append_string(&si, "ret");
				else if (NMD_R(op) == 0xb) /* mov [b0, bf] */
				{
					_nmd_append_string(&si, "mov ");
					if (instruction->prefixes & NMD_X86_PREFIXES_REX_B)
						_nmd_append_string(&si, _nmd_regrx[op % 8]), * si.buffer++ = NMD_C(op) < 8 ? 'b' : 'd';
					else
						_nmd_append_string(&si, (NMD_C(op) < 8 ? (instruction->hasRex ? _nmd_reg8_x64 : _nmd_reg8) : (instruction->operandSize64 ? _nmd_reg64 : (operandSize ? _nmd_reg16 : _nmd_reg32)))[op % 8]);
					*si.buffer++ = ',';
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0xfe) /* inc,dec */
				{
					_nmd_append_string(&si, instruction->modrm.fields.reg == 0b000 ? "inc " : "dec ");
					_nmd_append_Eb(&si);
				}
				else if (op == 0xf6 || op == 0xf7) /* test,test,not,neg,mul,imul,div,idiv */
				{
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp3[instruction->modrm.fields.reg]);
					*si.buffer++ = ' ';
					if (op == 0xf6)
						_nmd_append_Eb(&si);
					else
						_nmd_append_Ev(&si);

					if (instruction->modrm.fields.reg <= 0b001)
					{
						*si.buffer++ = ',';
						_nmd_append_number(&si, instruction->immediate);
					}
				}				
				else if (op == 0x69 || op == 0x6B)
				{
					_nmd_append_string(&si, "imul ");
					_nmd_append_Gv(&si);
					*si.buffer++ = ',';
					_nmd_append_Ev(&si);
					*si.buffer++ = ',';
					if (op == 0x6b)
					{
						if (si.formatFlags & NMD_X86_FORMAT_FLAGS_SIGNED_NUMBER_MEMORY_VIEW && instruction->immediate >= 0x80)
							_nmd_append_signed_number_memory_view(&si);
						else
							_nmd_append_signed_number(&si, (int8_t)instruction->immediate, false);
					}
					else
						_nmd_append_number(&si, instruction->immediate);
				}
				else if (op >= 0x84 && op <= 0x87)
				{
					_nmd_append_string(&si, op > 0x85 ? "xchg " : "test ");
					if (op % 2 == 0)
					{
						_nmd_append_Eb(&si);
						*si.buffer++ = ',';
						_nmd_append_Gb(&si);
					}
					else
					{
						_nmd_append_Ev(&si);
						*si.buffer++ = ',';
						_nmd_append_Gv(&si);
					}
				}
				else if (op == 0x8e)
				{
					_nmd_append_string(&si, "mov ");
					_nmd_append_string(&si, _nmd_segmentReg[instruction->modrm.fields.reg]);
					*si.buffer++ = ',';
					_nmd_append_Ew(&si);
				}
				else if (op >= 0x91 && op <= 0x97)
				{
					_nmd_append_string(&si, "xchg ");
					if (instruction->prefixes & NMD_X86_PREFIXES_REX_B)
					{
						_nmd_append_string(&si, _nmd_regrx[NMD_C(op)]);
						if (!(instruction->prefixes & NMD_X86_PREFIXES_REX_W))
							*si.buffer++ = 'd';
					}
					else
						_nmd_append_string(&si, (instruction->prefixes & NMD_X86_PREFIXES_REX_W ? _nmd_reg64 : (operandSize ? _nmd_reg16 : _nmd_reg32))[NMD_C(op)]);
					_nmd_append_string(&si, (instruction->prefixes & NMD_X86_PREFIXES_REX_W ? ",rax" : (operandSize ? ",ax" : ",eax")));
				}
				else if (op == 0x9A)
				{
					_nmd_append_string(&si, "call far ");
					_nmd_append_number(&si, (uint64_t)(*(uint16_t*)((char*)(&instruction->immediate) + (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? 2 : 4))));
					*si.buffer++ = ':';
					_nmd_append_number(&si, (uint64_t)(operandSize ? *((uint16_t*)(&instruction->immediate)) : *((uint32_t*)(&instruction->immediate))));
				}
				else if ((op >= 0x6c && op <= 0x6f) || (op >= 0xa4 && op <= 0xa7) || (op >= 0xaa && op <= 0xaf))
				{
					if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT)
						_nmd_append_string(&si, "rep ");
					else if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
						_nmd_append_string(&si, "repne ");

					const char* str = 0;
					switch (op)
					{
					case 0x6c: case 0x6d: str = "ins"; break;
					case 0x6e: case 0x6f: str = "outs"; break;
					case 0xa4: case 0xa5: str = "movs"; break;
					case 0xa6: case 0xa7: str = "cmps"; break;
					case 0xaa: case 0xab: str = "stos"; break;
					case 0xac: case 0xad: str = "lods"; break;
					case 0xae: case 0xaf: str = "scas"; break;
					}
					_nmd_append_string(&si, str);
					*si.buffer++ = (op % 2 == 0) ? 'b' : (operandSize ? 'w' : 'd');
				}
				else if (op == 0xC0 || op == 0xC1 || (NMD_R(op) == 0xd && NMD_C(op) < 4))
				{
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp2[instruction->modrm.fields.reg]);
					*si.buffer++ = ' ';
					if (op % 2 == 0)
						_nmd_append_Eb(&si);
					else
						_nmd_append_Ev(&si);
					*si.buffer++ = ',';
					if (NMD_R(op) == 0xc)
						_nmd_append_number(&si, instruction->immediate);
					else if (NMD_C(op) < 2)
						_nmd_append_number(&si, 1);
					else
						_nmd_append_string(&si, "cl");
				}
				else if (op == 0xc2)
				{
					_nmd_append_string(&si, "ret ");
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op >= 0xe0 && op <= 0xe3)
				{
					const char* mnemonics[] = { "loopne", "loope", "loop" };
					_nmd_append_string(&si, op == 0xe3 ? (instruction->mode == NMD_X86_MODE_64 ? "jrcxz" : "jecxz") : mnemonics[NMD_C(op)]);
					*si.buffer++ = ' ';
					_nmd_append_relative_address8(&si);
				}
				else if (op == 0xea)
				{
					_nmd_append_string(&si, "jmp far ");
					_nmd_append_number(&si, (uint64_t)(*(uint16_t*)(((uint8_t*)(&instruction->immediate) + 4))));
					*si.buffer++ = ':';
					_nmd_append_number(&si, (uint64_t)(*(uint32_t*)(&instruction->immediate)));
				}
				else if (op == 0xca)
				{
					_nmd_append_string(&si, "ret far");
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0xcd)
				{
					_nmd_append_string(&si, "int ");
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0x63)
				{
					if (instruction->mode == NMD_X86_MODE_64)
					{
						_nmd_append_string(&si, "movsxd ");
						_nmd_append_string(&si, (instruction->mode == NMD_X86_MODE_64 ? (instruction->prefixes & NMD_X86_PREFIXES_REX_R ? _nmd_regrx : _nmd_reg64) : (operandSize ? _nmd_reg16 : _nmd_reg32))[instruction->modrm.fields.reg]);
						*si.buffer++ = ',';
						if (instruction->modrm.fields.mod == 0b11)
						{
							if (instruction->prefixes & NMD_X86_PREFIXES_REX_B)
								_nmd_append_string(&si, _nmd_regrx[instruction->modrm.fields.rm]), * si.buffer++ = 'd';
							else
								_nmd_append_string(&si, ((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && instruction->mode == NMD_X86_MODE_32) || (instruction->mode == NMD_X86_MODE_16 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? _nmd_reg16 : _nmd_reg32)[instruction->modrm.fields.rm]);
						}
						else
							_nmd_append_modrm_upper(&si, (instruction->operandSize64 && !(instruction->prefixes & NMD_X86_PREFIXES_REX_W)) ? "qword" : ((instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE && instruction->mode == NMD_X86_MODE_32) || (instruction->mode == NMD_X86_MODE_16 && !(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)) ? "word" : "dword"));
					}
					else
					{
						_nmd_append_string(&si, "arpl ");
						_nmd_append_Ew(&si);
						*si.buffer++ = ',';
						_nmd_append_Gw(&si);
					}
				}
				else if (op == 0xc4 || op == 0xc5)
				{
					_nmd_append_string(&si, op == 0xc4 ? "les" : "lds");
					*si.buffer++ = ' ';
					_nmd_append_Gv(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, (si.instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? _nmd_reg16 : _nmd_reg32)[si.instruction->modrm.fields.rm]);
					else
						_nmd_append_modrm_upper(&si, si.instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "dword" : "fword");
				}
				else if (op == 0xc6 || op == 0xc7)
				{
					_nmd_append_string(&si, instruction->modrm.fields.reg == 0b000 ? "mov " : (op == 0xc6 ? "xabort " : "xbegin "));
					if (instruction->modrm.fields.reg == 0b111)
					{
						if (op == 0xc6)
							_nmd_append_number(&si, instruction->immediate);
						else
							_nmd_append_relative_address16_32(&si);
					}
					else
					{
						if (op == 0xc6)
							_nmd_append_Eb(&si);
						else
							_nmd_append_Ev(&si);
						*si.buffer++ = ',';
						_nmd_append_number(&si, instruction->immediate);
					}
				}
				else if (op == 0xc8)
				{
					_nmd_append_string(&si, "enter ");
					_nmd_append_number(&si, (uint64_t)(*(uint16_t*)(&instruction->immediate)));
					*si.buffer++ = ',';
					_nmd_append_number(&si, (uint64_t)(*((uint8_t*)(&instruction->immediate) + 2)));
				}				
				else if (op == 0xd4)
				{
					_nmd_append_string(&si, "aam ");
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0xd5)
				{
					_nmd_append_string(&si, "aad ");
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op >= 0xd8 && op <= 0xdf)
				{
					*si.buffer++ = 'f';

					if (instruction->modrm.modrm < 0xc0)
					{
						_nmd_append_string(&si, _nmd_escapeOpcodes[NMD_C(op) - 8][instruction->modrm.fields.reg]);
						*si.buffer++ = ' ';
						switch (op)
						{
						case 0xd8: case 0xda: _nmd_append_modrm_upper(&si, "dword"); break;
						case 0xd9: _nmd_append_modrm_upper(&si, instruction->modrm.fields.reg & 0b100 ? (instruction->modrm.fields.reg & 0b001 ? "word" : (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "m14" : "m28")) : "dword"); break;
						case 0xdb: _nmd_append_modrm_upper(&si, instruction->modrm.fields.reg & 0b100 ? "tbyte" : "dword"); break;
						case 0xdc: _nmd_append_modrm_upper(&si, "qword"); break;
						case 0xdd: _nmd_append_modrm_upper(&si, instruction->modrm.fields.reg & 0b100 ? ((instruction->modrm.fields.reg & 0b111) == 0b111 ? "word" : "byte") : "qword"); break;
						case 0xde: _nmd_append_modrm_upper(&si, "word"); break;
						case 0xdf: _nmd_append_modrm_upper(&si, instruction->modrm.fields.reg & 0b100 ? (instruction->modrm.fields.reg & 0b001 ? "qword" : "tbyte") : "word"); break;
						}
					}
					else
					{
						switch (op)
						{
						case 0xd8:
							_nmd_append_string(&si, _nmd_escapeOpcodesD8[(NMD_R(instruction->modrm.modrm) - 0xc) * 2 + (NMD_C(instruction->modrm.modrm) > 7 ? 1 : 0)]);
							_nmd_append_string(&si, " st(0),st(");
							*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8), * si.buffer++ = ')';
							break;
						case 0xd9:
							if (NMD_R(instruction->modrm.modrm) == 0xc)
							{
								_nmd_append_string(&si, NMD_C(instruction->modrm.modrm) < 8 ? "ld" : "xch");
								_nmd_append_string(&si, " st(0),st(");
								*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8), * si.buffer++ = ')';
							}
							else if (instruction->modrm.modrm >= 0xd8 && instruction->modrm.modrm <= 0xdf)
							{
								_nmd_append_string(&si, "stpnce st(");
								*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
								_nmd_append_string(&si, "),st(0)");
							}
							else
							{
								const char* str = 0;
								switch (instruction->modrm.modrm)
								{
								case 0xd0: str = "nop"; break;
								case 0xe0: str = "chs"; break;
								case 0xe1: str = "abs"; break;
								case 0xe4: str = "tst"; break;
								case 0xe5: str = "xam"; break;
								case 0xe8: str = "ld1"; break;
								case 0xe9: str = "ldl2t"; break;
								case 0xea: str = "ldl2e"; break;
								case 0xeb: str = "ldpi"; break;
								case 0xec: str = "ldlg2"; break;
								case 0xed: str = "ldln2"; break;
								case 0xee: str = "ldz"; break;
								case 0xf0: str = "2xm1"; break;
								case 0xf1: str = "yl2x"; break;
								case 0xf2: str = "ptan"; break;
								case 0xf3: str = "patan"; break;
								case 0xf4: str = "xtract"; break;
								case 0xf5: str = "prem1"; break;
								case 0xf6: str = "decstp"; break;
								case 0xf7: str = "incstp"; break;
								case 0xf8: str = "prem"; break;
								case 0xf9: str = "yl2xp1"; break;
								case 0xfa: str = "sqrt"; break;
								case 0xfb: str = "sincos"; break;
								case 0xfc: str = "rndint"; break;
								case 0xfd: str = "scale"; break;
								case 0xfe: str = "sin"; break;
								case 0xff: str = "cos"; break;
								}
								_nmd_append_string(&si, str);
							}
							break;
						case 0xda:
							if (instruction->modrm.modrm == 0xe9)
								_nmd_append_string(&si, "ucompp");
							else
							{
								const char* mnemonics[4] = { "cmovb", "cmovbe", "cmove", "cmovu" };
								_nmd_append_string(&si, mnemonics[(NMD_R(instruction->modrm.modrm) - 0xc) + (NMD_C(instruction->modrm.modrm) > 7 ? 2 : 0)]);
								_nmd_append_string(&si, " st(0),st(");
								*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
								*si.buffer++ = ')';
							}
							break;
						case 0xdb:
							if (NMD_R(instruction->modrm.modrm) == 0xe && NMD_C(instruction->modrm.modrm) < 8)
							{
								const char* mnemonics[] = { "eni8087_nop", "disi8087_nop", "nclex", "ninit", "setpm287_nop" };
								_nmd_append_string(&si, mnemonics[NMD_C(instruction->modrm.modrm)]);
							}
							else
							{
								if (instruction->modrm.modrm >= 0xe0)
									_nmd_append_string(&si, instruction->modrm.modrm < 0xf0 ? "ucomi" : "comi");
								else
								{
									_nmd_append_string(&si, "cmovn");
									if (instruction->modrm.modrm < 0xc8)
										*si.buffer++ = 'b';
									else if (instruction->modrm.modrm < 0xd0)
										*si.buffer++ = 'e';
									else if (instruction->modrm.modrm >= 0xd8)
										*si.buffer++ = 'u';
									else
										_nmd_append_string(&si, "be");
								}
								_nmd_append_string(&si, " st(0),st(");
								*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
								*si.buffer++ = ')';
							}
							break;
						case 0xdc:
							if (NMD_R(instruction->modrm.modrm) == 0xc)
								_nmd_append_string(&si, NMD_C(instruction->modrm.modrm) > 7 ? "mul" : "add");
							else
							{
								_nmd_append_string(&si, NMD_R(instruction->modrm.modrm) == 0xd ? "com" : (NMD_R(instruction->modrm.modrm) == 0xe ? "subr" : "div"));
								if (NMD_R(instruction->modrm.modrm) == 0xd && NMD_C(instruction->modrm.modrm) >= 8)
								{
									if (NMD_R(instruction->modrm.modrm) >= 8)
										*si.buffer++ = 'p';
								}
								else
								{
									if (NMD_R(instruction->modrm.modrm) < 8)
										*si.buffer++ = 'r';
								}
							}

							if (NMD_R(instruction->modrm.modrm) == 0xd)
							{
								_nmd_append_string(&si, " st(0),st(");
								*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
								*si.buffer++ = ')';
							}
							else
							{
								_nmd_append_string(&si, " st(");
								*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
								_nmd_append_string(&si, "),st(0)");
							}
							break;
						case 0xdd:
							if (NMD_R(instruction->modrm.modrm) == 0xc)
								_nmd_append_string(&si, NMD_C(instruction->modrm.modrm) < 8 ? "free" : "xch");
							else
							{
								_nmd_append_string(&si, instruction->modrm.modrm < 0xe0 ? "st" : "ucom");
								if (NMD_C(instruction->modrm.modrm) >= 8)
									*si.buffer++ = 'p';
							}

							_nmd_append_string(&si, " st(");
							*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
							*si.buffer++ = ')';

							break;
						case 0xde:
							if (instruction->modrm.modrm == 0xd9)
								_nmd_append_string(&si, "compp");
							else
							{
								if (instruction->modrm.modrm >= 0xd0 && instruction->modrm.modrm <= 0xd7)
								{
									_nmd_append_string(&si, "comp st(0),st(");
									*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
									*si.buffer++ = ')';
								}
								else
								{
									if (NMD_R(instruction->modrm.modrm) == 0xc)
										_nmd_append_string(&si, NMD_C(instruction->modrm.modrm) < 8 ? "add" : "mul");
									else
									{
										_nmd_append_string(&si, instruction->modrm.modrm < 0xf0 ? "sub" : "div");
										if (NMD_R(instruction->modrm.modrm) < 8 || (NMD_R(instruction->modrm.modrm) >= 0xe && NMD_C(instruction->modrm.modrm) < 8))
											*si.buffer++ = 'r';
									}
									_nmd_append_string(&si, "p st(");
									*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
									_nmd_append_string(&si, "),st(0)");
								}
							}
							break;
						case 0xdf:
							if (instruction->modrm.modrm == 0xe0)
								_nmd_append_string(&si, "nstsw ax");
							else
							{
								if (instruction->modrm.modrm >= 0xe8)
								{
									if (instruction->modrm.modrm < 0xf0)
										*si.buffer++ = 'u';
									_nmd_append_string(&si, "comip");
									_nmd_append_string(&si, " st(0),st(");
									*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
									*si.buffer++ = ')';
								}
								else
								{
									_nmd_append_string(&si, instruction->modrm.modrm < 0xc8 ? "freep" : (instruction->modrm.modrm >= 0xd0 ? "stp" : "xch"));
									_nmd_append_string(&si, " st(");
									*si.buffer++ = (char)('0' + instruction->modrm.modrm % 8);
									*si.buffer++ = ')';
								}
							}

							break;
						}
					}
				}
				else if (op == 0xe4 || op == 0xe5)
				{
					_nmd_append_string(&si, "in ");
					_nmd_append_string(&si, op == 0xe4 ? "al" : (operandSize ? "ax" : "eax"));
					*si.buffer++ = ',';
					_nmd_append_number(&si, instruction->immediate);
				}
				else if (op == 0xe6 || op == 0xe7)
				{
					_nmd_append_string(&si, "out ");
					_nmd_append_number(&si, instruction->immediate);
					*si.buffer++ = ',';
					_nmd_append_string(&si, op == 0xe6 ? "al" : (operandSize ? "ax" : "eax"));
				}				
				else if (op == 0xec || op == 0xed)
				{
					_nmd_append_string(&si, "in ");
					_nmd_append_string(&si, op == 0xec ? "al" : (operandSize ? "ax" : "eax"));
					_nmd_append_string(&si, ",dx");
				}
				else if (op == 0xee || op == 0xef)
				{
					_nmd_append_string(&si, "out dx,");
					_nmd_append_string(&si, op == 0xee ? "al" : (operandSize ? "ax" : "eax"));
				}
				else if (op == 0x62)
				{
					_nmd_append_string(&si, "bound ");
					_nmd_append_Gv(&si);
					*si.buffer++ = ',';
					_nmd_append_modrm_upper(&si, operandSize ? "dword" : "qword");
				}
				else /* Try to parse all opcodes not parsed by the checks above. */
				{
					const char* str = 0;
					switch (instruction->opcode)
					{
					case 0x9c: 
					{
						if (operandSize)
							str = (instruction->mode == NMD_X86_MODE_16) ? "pushfd" : "pushf";
						else
							str = (instruction->mode == NMD_X86_MODE_16) ? "pushf" : ((instruction->mode == NMD_X86_MODE_32) ? "pushfd" : "pushfq");
						break;
					}
					case 0x9d:
					{
						if (operandSize)
							str = (instruction->mode == NMD_X86_MODE_16) ? "popfd" : "popf";
						else
							str = (instruction->mode == NMD_X86_MODE_16) ? "popf" : ((instruction->mode == NMD_X86_MODE_32) ? "popfd" : "popfq");
						break;
					}
					case 0x60:
					case 0x61:
						str = operandSize ? (instruction->opcode == 0x60 ? "pusha" : "popa") : (instruction->opcode == 0x60 ? "pushad" : "popad");
						break;
					case 0xcb: str = "retf"; break;
					case 0xc9: str = "leave"; break;
					case 0xf1: str = "int1"; break;
					case 0x06: str = "push es"; break;
					case 0x16: str = "push ss"; break;
					case 0x1e: str = "push ds"; break;
					case 0x0e: str = "push cs"; break;
					case 0x07: str = "pop es"; break;
					case 0x17: str = "pop ss"; break;
					case 0x1f: str = "pop ds"; break;
					case 0x27: str = "daa"; break;
					case 0x37: str = "aaa"; break;
					case 0x2f: str = "das"; break;
					case 0x3f: str = "aas"; break;
					case 0xd7: str = "xlat"; break;
					case 0x9b: str = "fwait"; break;
					case 0xf4: str = "hlt"; break;
					case 0xf5: str = "cmc"; break;
					case 0x9e: str = "sahf"; break;
					case 0x9f: str = "lahf"; break;
					case 0xce: str = "into"; break;
					case 0xcf:
						if (instruction->operandSize64)
							str = "iretq";
						else if (instruction->mode == NMD_X86_MODE_16)
							str = operandSize ? "iretd" : "iret";
						else
							str = operandSize ? "iret" : "iretd";
						break;
					case 0x98: str = (instruction->prefixes & NMD_X86_PREFIXES_REX_W ? "cdqe" : (operandSize ? "cbw" : "cwde")); break;
					case 0x99: str = (instruction->prefixes & NMD_X86_PREFIXES_REX_W ? "cqo" : (operandSize ? "cwd" : "cdq")); break;
					case 0xd6: str = "salc"; break;
					case 0xf8: str = "clc"; break;
					case 0xf9: str = "stc"; break;
					case 0xfa: str = "cli"; break;
					case 0xfb: str = "sti"; break;
					case 0xfc: str = "cld"; break;
					case 0xfd: str = "std"; break;
					default: return;
					}
					_nmd_append_string(&si, str);
				}
			}
	}
	else if (instruction->opcodeMap == NMD_X86_OPCODE_MAP_0F)
	{
		if (NMD_R(op) == 8)
		{
			*si.buffer++ = 'j';
			_nmd_append_string(&si, _nmd_conditionSuffixes[NMD_C(op)]);
			*si.buffer++ = ' ';
			_nmd_append_relative_address16_32(&si);
		}
		else if(op == 0x05)
			_nmd_append_string(&si, "syscall");
		else if(op == 0xa2)
			_nmd_append_string(&si, "cpuid");
		else if (NMD_R(op) == 4)
		{
			_nmd_append_string(&si, "cmov");
			_nmd_append_string(&si, _nmd_conditionSuffixes[NMD_C(op)]);
			*si.buffer++ = ' ';
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			_nmd_append_Ev(&si);
		}
		else if (op >= 0x10 && op <= 0x17)
		{
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
			{
				const char* prefix66Mnemonics[] = { "movupd", "movupd", "movlpd", "movlpd", "unpcklpd", "unpckhpd", "movhpd", "movhpd" };

				_nmd_append_string(&si, prefix66Mnemonics[NMD_C(op)]);
				*si.buffer++ = ' ';

				switch (NMD_C(op))
				{
				case 0:
					_nmd_append_Vx(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
					break;
				case 1:
					_nmd_append_W(&si);
					*si.buffer++ = ',';
					_nmd_append_Vx(&si);
					break;
				case 2:
				case 6:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
					break;
				default:
					break;
				}
			}
			else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
			{
				const char* prefixF3Mnemonics[] = { "movss", "movss", "movsldup", 0, 0, 0, "movshdup" };

				_nmd_append_string(&si, prefixF3Mnemonics[NMD_C(op)]);
				*si.buffer++ = ' ';

				switch (NMD_C(op))
				{
				case 0:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "dword");
					break;
				case 1:
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "dword");
					*si.buffer++ = ',';
					_nmd_append_Vdq(&si);
					break;
				case 2:
				case 6:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
					break;
				}
			}
			else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
			{
				const char* prefixF2Mnemonics[] = { "movsd", "movsd", "movddup" };

				_nmd_append_string(&si, prefixF2Mnemonics[NMD_C(op)]);
				*si.buffer++ = ' ';

				switch (NMD_C(op))
				{
				case 0:
				case 2:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
					break;
				case 1:
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
					*si.buffer++ = ',';
					_nmd_append_Vdq(&si);
					break;
				}
			}
			else
			{
				const char* noPrefixMnemonics[] = { "movups", "movups", "movlps", "movlps", "unpcklps", "unpckhps", "movhps", "movhps" };

				if (op == 0x12 && instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, "movhlps");
				else if (op == 0x16 && instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, "movlhps");
				else
					_nmd_append_string(&si, noPrefixMnemonics[NMD_C(op)]);
				*si.buffer++ = ' ';

				switch (NMD_C(op))
				{
				case 0:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
					break;
				case 1:
					_nmd_append_W(&si);
					*si.buffer++ = ',';
					_nmd_append_Vdq(&si);
					break;
				case 2:
				case 6:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
					break;
				default:
					break;
				};

			}

			switch (NMD_C(op))
			{
			case 3:
			case 7:
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
				else
					_nmd_append_modrm_upper(&si, "qword");
				*si.buffer++ = ',';
				_nmd_append_Vdq(&si);
				break;
			case 4:
			case 5:
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_W(&si);
				break;
			};
		}
		else if (NMD_R(op) == 6 || (op >= 0x74 && op <= 0x76))
		{
			if (op == 0x6e)
			{
				_nmd_append_string(&si, "movd ");
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
					*si.buffer++ = 'x';
				_nmd_append_Pq(&si);
				*si.buffer++ = ',';
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, _nmd_reg32[si.instruction->modrm.fields.rm]);
				else
					_nmd_append_modrm_upper(&si, "dword");
			}
			else
			{
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
				{
					const char* prefix66Mnemonics[] = { "punpcklbw", "punpcklwd", "punpckldq", "packsswb", "pcmpgtb", "pcmpgtw", "pcmpgtd", "packuswb", "punpckhbw", "punpckhwd", "punpckhdq", "packssdw", "punpcklqdq", "punpckhqdq", "movd", "movdqa" };

					_nmd_append_string(&si, op == 0x74 ? "pcmpeqb" : (op == 0x75 ? "pcmpeqw" : (op == 0x76 ? "pcmpeqd" : prefix66Mnemonics[op % 0x10])));
					*si.buffer++ = ' ';
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
				}
				else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
				{
					_nmd_append_string(&si, "movdqu ");
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
				}
				else
				{
					const char* noPrefixMnemonics[] = { "punpcklbw", "punpcklwd", "punpckldq", "packsswb", "pcmpgtb", "pcmpgtw", "pcmpgtd", "packuswb", "punpckhbw", "punpckhwd", "punpckhdq", "packssdw", 0, 0, "movd", "movq" };

					_nmd_append_string(&si, op == 0x74 ? "pcmpeqb" : (op == 0x75 ? "pcmpeqw" : (op == 0x76 ? "pcmpeqd" : noPrefixMnemonics[op % 0x10])));
					*si.buffer++ = ' ';
					_nmd_append_Pq(&si);
					*si.buffer++ = ',';
					_nmd_append_Qq(&si);
				}
			}
		}
		else if (op == 0x00)
		{
			_nmd_append_string(&si, _nmd_opcodeExtensionsGrp6[instruction->modrm.fields.reg]);
			*si.buffer++ = ' ';
			if (NMD_R(instruction->modrm.modrm) == 0xc)
				_nmd_append_Ev(&si);
			else
				_nmd_append_Ew(&si);
		}
		else if (op == 0x01)
		{
			if (instruction->modrm.fields.mod == 0b11)
			{
				if (instruction->modrm.fields.reg == 0b000)
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp7reg0[instruction->modrm.fields.rm]);
				else if (instruction->modrm.fields.reg == 0b001)
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp7reg1[instruction->modrm.fields.rm]);
				else if (instruction->modrm.fields.reg == 0b010)
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp7reg2[instruction->modrm.fields.rm]);
				else if (instruction->modrm.fields.reg == 0b011)
				{
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp7reg3[instruction->modrm.fields.rm]);
					if (instruction->modrm.fields.rm == 0b000 || instruction->modrm.fields.rm == 0b010 || instruction->modrm.fields.rm == 0b111)
						_nmd_append_string(&si, instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "ax" : "eax");

					if (instruction->modrm.fields.rm == 0b111)
						_nmd_append_string(&si, ",ecx");
				}
				else if (instruction->modrm.fields.reg == 0b100)
					_nmd_append_string(&si, "smsw "), _nmd_append_string(&si, (instruction->operandSize64 ? _nmd_reg64 : (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? _nmd_reg16 : _nmd_reg32))[instruction->modrm.fields.rm]);
				else if (instruction->modrm.fields.reg == 0b101)
				{
					if (instruction->prefixes & NMD_X86_PREFIXES_REPEAT)
						_nmd_append_string(&si, instruction->modrm.fields.rm == 0b000 ? "setssbsy" : "saveprevssp");
					else
						_nmd_append_string(&si, instruction->modrm.fields.rm == 0b111 ? "wrpkru" : "rdpkru");
				}
				else if (instruction->modrm.fields.reg == 0b110)
					_nmd_append_string(&si, "lmsw "), _nmd_append_string(&si, _nmd_reg16[instruction->modrm.fields.rm]);
				else if (instruction->modrm.fields.reg == 0b111)
				{
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp7reg7[instruction->modrm.fields.rm]);
					if (instruction->modrm.fields.rm == 0b100)
						_nmd_append_string(&si, instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "ax" : "eax");
				}
			}
			else
			{
				if (instruction->modrm.fields.reg == 0b101)
				{
					_nmd_append_string(&si, "rstorssp ");
					_nmd_append_modrm_upper(&si, "qword");
				}
				else
				{
					_nmd_append_string(&si, _nmd_opcodeExtensionsGrp7[instruction->modrm.fields.reg]);
					*si.buffer++ = ' ';
					if (si.instruction->modrm.fields.reg == 0b110)
						_nmd_append_Ew(&si);
					else
						_nmd_append_modrm_upper(&si, si.instruction->modrm.fields.reg == 0b111 ? "byte" : si.instruction->modrm.fields.reg == 0b100 ? "word" : "fword");
				}
			}
		}
		else if (op == 0x02 || op == 0x03)
		{
			_nmd_append_string(&si, op == 0x02 ? "lar" : "lsl");
			*si.buffer++ = ' ';
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			if (si.instruction->modrm.fields.mod == 0b11)
				_nmd_append_string(&si, (operandSize ? _nmd_reg16 : _nmd_reg32)[si.instruction->modrm.fields.rm]);
			else
				_nmd_append_modrm_upper(&si, "word");
		}
		else if (op == 0x0d)
		{
			if (instruction->modrm.fields.mod == 0b11)
			{
				_nmd_append_string(&si, "nop ");
				_nmd_append_string(&si, (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? _nmd_reg16 : _nmd_reg32)[instruction->modrm.fields.rm]);
				*si.buffer++ = ',';
				_nmd_append_string(&si, (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? _nmd_reg16 : _nmd_reg32)[instruction->modrm.fields.reg]);
			}
			else
			{
				_nmd_append_string(&si, "prefetch");
				if (instruction->modrm.fields.reg == 0b001)
					*si.buffer++ = 'w';
				else if (instruction->modrm.fields.reg == 0b010)
					_nmd_append_string(&si, "wt1");

				*si.buffer++ = ' ';

				_nmd_append_modrm_upper(&si, "byte");
			}
		}	
		else if (op == 0x18)
		{
			if (instruction->modrm.fields.mod == 0b11 || instruction->modrm.fields.reg >= 0b100)
			{
				_nmd_append_string(&si, "nop ");
				_nmd_append_Ev(&si);
			}
			else
			{
				if (instruction->modrm.fields.reg == 0b000)
					_nmd_append_string(&si, "prefetchnta");
				else
				{
					_nmd_append_string(&si, "prefetcht");
					*si.buffer++ = (char)('0' + (instruction->modrm.fields.reg - 1));
				}
				*si.buffer++ = ' ';

				_nmd_append_Eb(&si);
			}
		}
		else if (op == 0x19)
		{
			_nmd_append_string(&si, "nop ");
			_nmd_append_Ev(&si);
			*si.buffer++ = ',';
			_nmd_append_Gv(&si);
		}
		else if (op == 0x1A)
		{
			if (instruction->modrm.fields.mod == 0b11)
			{
				_nmd_append_string(&si, "nop ");
				_nmd_append_Ev(&si);
				*si.buffer++ = ',';
				_nmd_append_Gv(&si);
			}
			else
			{
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
					_nmd_append_string(&si, "bndmov");
				else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
					_nmd_append_string(&si, "bndcl");
				else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
					_nmd_append_string(&si, "bndcu");
				else
					_nmd_append_string(&si, "bndldx");

				_nmd_append_string(&si, " bnd");
				*si.buffer++ = (char)('0' + instruction->modrm.fields.reg);
				*si.buffer++ = ',';
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
					*si.buffer++ = 'q';
				_nmd_append_Ev(&si);
			}
		}
		else if (op == 0x1B)
		{
			if (instruction->modrm.fields.mod == 0b11)
			{
				_nmd_append_string(&si, "nop ");
				_nmd_append_Ev(&si);
				*si.buffer++ = ',';
				_nmd_append_Gv(&si);
			}
			else
			{
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
					_nmd_append_string(&si, "bndmov");
				else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
					_nmd_append_string(&si, "bndmk");
				else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
					_nmd_append_string(&si, "bndcn");
				else
					_nmd_append_string(&si, "bndstx");

				*si.buffer++ = ' ';
				_nmd_append_Ev(&si);
				*si.buffer++ = ',';
				_nmd_append_string(&si, "bnd");
				*si.buffer++ = (char)('0' + instruction->modrm.fields.reg);
			}
		}
		else if (op >= 0x1c && op <= 0x1f)
		{
			if (op == 0x1e && instruction->modrm.modrm == 0xfa)
				_nmd_append_string(&si, "endbr64");
			else if (op == 0x1e && instruction->modrm.modrm == 0xfb)
				_nmd_append_string(&si, "endbr32");
			else
			{
				_nmd_append_string(&si, "nop ");
				_nmd_append_Ev(&si);
				*si.buffer++ = ',';
				_nmd_append_Gv(&si);
			}
		}
		else if (op >= 0x20 && op <= 0x23)
		{
			_nmd_append_string(&si, "mov ");
			if (op < 0x22)
			{
				_nmd_append_string(&si, (instruction->mode == NMD_X86_MODE_64 ? _nmd_reg64 : _nmd_reg32)[instruction->modrm.fields.rm]);
				_nmd_append_string(&si, op == 0x20 ? ",cr" : ",dr");
				*si.buffer++ = (char)('0' + instruction->modrm.fields.reg);
			}
			else
			{
				_nmd_append_string(&si, op == 0x22 ? "cr" : "dr");
				*si.buffer++ = (char)('0' + instruction->modrm.fields.reg);
				*si.buffer++ = ',';
				_nmd_append_string(&si, (instruction->mode == NMD_X86_MODE_64 ? _nmd_reg64 : _nmd_reg32)[instruction->modrm.fields.rm]);
			}
		}
		else if (op >= 0x28 && op <= 0x2f)
		{
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
			{
				const char* prefix66Mnemonics[] = { "movapd", "movapd", "cvtpi2pd", "movntpd", "cvttpd2pi", "cvtpd2pi", "ucomisd", "comisd" };

				_nmd_append_string(&si, prefix66Mnemonics[op % 8]);
				*si.buffer++ = ' ';

				switch (op % 8)
				{
				case 0:
					_nmd_append_Vx(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
					break;
				case 1:
					_nmd_append_W(&si);
					*si.buffer++ = ',';
					_nmd_append_Vx(&si);
					break;
				case 2:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_Qq(&si);
					break;
				case 4:
				case 5:
					_nmd_append_Pq(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
					break;
				case 6:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
					break;
				case 7:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
				default:
					break;
				}
			}
			else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
			{
				const char* prefixF3Mnemonics[] = { 0, 0, "cvtsi2ss", "movntss", "cvttss2si", "cvtss2si", 0, 0 };

				_nmd_append_string(&si, prefixF3Mnemonics[op % 8]);
				*si.buffer++ = ' ';

				switch (op % 8)
				{
				case 3:
					_nmd_append_modrm_upper(&si, "dword");
					*si.buffer++ = ',';
					_nmd_append_Vdq(&si);
					break;
				case 4:
				case 5:
					_nmd_append_Gv(&si);
					*si.buffer++ = ',';
					if (instruction->modrm.fields.mod == 0b11)
						_nmd_append_Udq(&si);
					else
						_nmd_append_modrm_upper(&si, "dword");
					break;
				case 2:
				case 6:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_Ev(&si);
					break;
				}
			}
			else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
			{
				const char* prefixF2Mnemonics[] = { 0, 0, "cvtsi2sd", "movntsd", "cvttsd2si", "cvtsd2si", 0, 0 };

				_nmd_append_string(&si, prefixF2Mnemonics[op % 8]);
				*si.buffer++ = ' ';

				switch (op % 8)
				{
				case 2:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_Ev(&si);
					break;
				case 3:
					_nmd_append_modrm_upper(&si, "qword");
					*si.buffer++ = ',';
					_nmd_append_Vdq(&si);
					break;
				case 4:
				case 5:
					_nmd_append_Gv(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
					break;
				}
			}
			else
			{
				const char* noPrefixMnemonics[] = { "movaps", "movaps", "cvtpi2ps", "movntps", "cvttps2pi", "cvtps2pi", "ucomiss", "comiss" };

				_nmd_append_string(&si, noPrefixMnemonics[op % 8]);
				*si.buffer++ = ' ';

				switch (op % 8)
				{
				case 0:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_W(&si);
					break;
				case 1:
					_nmd_append_W(&si);
					*si.buffer++ = ',';
					_nmd_append_Vdq(&si);
					break;
				case 4:
				case 5:
					_nmd_append_Pq(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "qword");
					break;
				case 2:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					_nmd_append_Qq(&si);
					break;
				case 6:
				case 7:
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, "dword");
					break;
				default:
					break;
				};

			}

			if (!(instruction->prefixes & (NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO)) && (op % 8) == 3)
			{
				_nmd_append_modrm_upper(&si, "xmmword");
				*si.buffer++ = ',';
				_nmd_append_Vdq(&si);
			}
		}
		else if (NMD_R(op) == 5)
		{
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
			{
				const char* prefix66Mnemonics[] = { "movmskpd", "sqrtpd", 0, 0, "andpd", "andnpd", "orpd", "xorpd", "addpd", "mulpd", "cvtpd2ps",  "cvtps2dq", "subpd", "minpd", "divpd", "maxpd" };

				_nmd_append_string(&si, prefix66Mnemonics[op % 0x10]);
				*si.buffer++ = ' ';
				if (op == 0x50)
					_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.reg]);
				else
					_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_W(&si);
			}
			else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
			{
				const char* prefixF3Mnemonics[] = { 0, "sqrtss", "rsqrtss", "rcpss", 0, 0, 0, 0, "addss", "mulss", "cvtss2sd", "cvttps2dq", "subss", "minss", "divss", "maxss" };

				_nmd_append_string(&si, prefixF3Mnemonics[op % 0x10]);
				*si.buffer++ = ' ';
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
				else
					_nmd_append_modrm_upper(&si, op == 0x5b ? "xmmword" : "dword");
			}
			else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
			{
				const char* prefixF2Mnemonics[] = { 0, "sqrtsd", 0, 0, 0, 0, 0, 0, "addsd", "mulsd", "cvtsd2ss", 0, "subsd", "minsd", "divsd", "maxsd" };

				_nmd_append_string(&si, prefixF2Mnemonics[op % 0x10]);
				*si.buffer++ = ' ';
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
				else
					_nmd_append_modrm_upper(&si, "qword");
			}
			else
			{
				const char* noPrefixMnemonics[] = { "movmskps", "sqrtps", "rsqrtps", "rcpps", "andps", "andnps", "orps", "xorps", "addps", "mulps", "cvtps2pd",  "cvtdq2ps", "subps", "minps", "divps", "maxps" };

				_nmd_append_string(&si, noPrefixMnemonics[op % 0x10]);
				*si.buffer++ = ' ';
				if (op == 0x50)
				{
					_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.reg]);
					*si.buffer++ = ',';
					_nmd_append_Udq(&si);
				}
				else
				{
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
					if (si.instruction->modrm.fields.mod == 0b11)
						_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + si.instruction->modrm.fields.rm);
					else
						_nmd_append_modrm_upper(&si, op == 0x5a ? "qword" : "xmmword");
				}
			}
		}		
		else if (op == 0x70)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "pshufd" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "pshufhw" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? "pshuflw" : "pshufw")));
			*si.buffer++ = ' ';
			if (!(instruction->prefixes & (NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE | NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO)))
			{
				_nmd_append_Pq(&si);
				*si.buffer++ = ',';
				_nmd_append_Qq(&si);
			}
			else
			{
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_W(&si);
			}

			*si.buffer++ = ',';
			_nmd_append_number(&si, instruction->immediate);
		}
		else if (op >= 0x71 && op <= 0x73)
		{
			if (instruction->modrm.fields.reg % 2 == 1)
				_nmd_append_string(&si, instruction->modrm.fields.reg == 0b111 ? "pslldq" : "psrldq");
			else
			{
				const char* mnemonics[] = { "psrl", "psra", "psll" };
				_nmd_append_string(&si, mnemonics[(instruction->modrm.fields.reg >> 1) - 1]);
				*si.buffer++ = op == 0x71 ? 'w' : (op == 0x72 ? 'd' : 'q');
			}

			*si.buffer++ = ' ';
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
				_nmd_append_Udq(&si);
			else
				_nmd_append_Nq(&si);
			*si.buffer++ = ',';
			_nmd_append_number(&si, instruction->immediate);
		}
		else if (op == 0x78)
		{
			if (!instruction->simdPrefix)
			{
				_nmd_append_string(&si, "vmread ");
				_nmd_append_Ey(&si);
				*si.buffer++ = ',';
				_nmd_append_Gy(&si);
			}
			else
			{
				if(instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
					_nmd_append_string(&si, "extrq ");
				else
				{ 
					_nmd_append_string(&si, "insertq ");
					_nmd_append_Vdq(&si);
					*si.buffer++ = ',';
				}
				_nmd_append_Udq(&si);
				*si.buffer++ = ',';
				_nmd_append_number(&si, instruction->immediate & 0x00FF);
				*si.buffer++ = ',';
				_nmd_append_number(&si, (instruction->immediate & 0xFF00) >> 8);
			}
		}
		else if (op == 0x79)
		{
			if (!instruction->simdPrefix)
			{
				_nmd_append_string(&si, "vmwrite ");
				_nmd_append_Gy(&si);
				*si.buffer++ = ',';
				_nmd_append_Ey(&si);
			}
			else
			{
				_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "extrq " : "insertq ");
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_Udq(&si);
			}

		}
		else if (op == 0x7c || op == 0x7d)
		{
			_nmd_append_string(&si, op == 0x7c ? "haddp" : "hsubp");
			*si.buffer++ = instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? 'd' : 's';
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			_nmd_append_W(&si);
		}
		else if (op == 0x7e)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "movq " : "movd ");
			if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
			{
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_Udq(&si);
				else
					_nmd_append_modrm_upper(&si, "qword");
			}
			else
			{
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.rm]);
				else
					_nmd_append_modrm_upper(&si, "dword");
				*si.buffer++ = ',';
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
					_nmd_append_Vdq(&si);
				else
					_nmd_append_Pq(&si);
			}
		}
		else if (op == 0x7f)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "movdqu" : (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "movdqa" : "movq"));
			*si.buffer++ = ' ';
			if (instruction->prefixes & (NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE))
			{
				_nmd_append_W(&si);
				*si.buffer++ = ',';
				_nmd_append_Vdq(&si);
			}
			else
			{
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_Nq(&si);
				else
					_nmd_append_modrm_upper(&si, "qword");
				*si.buffer++ = ',';
				_nmd_append_Pq(&si);
			}
		}		
		else if (NMD_R(op) == 9)
		{
			_nmd_append_string(&si, "set");
			_nmd_append_string(&si, _nmd_conditionSuffixes[NMD_C(op)]);
			*si.buffer++ = ' ';
			_nmd_append_Eb(&si);
		}
		else if ((NMD_R(op) == 0xA || NMD_R(op) == 0xB) && op % 8 == 3)
		{
			_nmd_append_string(&si, op == 0xa3 ? "bt" : (op == 0xb3 ? "btr" : (op == 0xab ? "bts" : "btc")));
			*si.buffer++ = ' ';
			_nmd_append_Ev(&si);
			*si.buffer++ = ',';
			_nmd_append_Gv(&si);
		}
		else if (NMD_R(op) == 0xA && (op % 8 == 4 || op % 8 == 5))
		{
			_nmd_append_string(&si, op > 0xA8 ? "shrd" : "shld");
			*si.buffer++ = ' ';
			_nmd_append_Ev(&si);
			*si.buffer++ = ',';
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			if (op % 8 == 4)
				_nmd_append_number(&si, instruction->immediate);
			else
				_nmd_append_string(&si, "cl");
		}
		else if (op == 0xb4 || op == 0xb5)
		{
			_nmd_append_string(&si, op == 0xb4 ? "lfs " : "lgs ");
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			_nmd_append_modrm_upper(&si, "fword");
		}
		else if (op == 0xbc || op == 0xbd)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? (op == 0xbc ? "tzcnt" : "lzcnt") : (op == 0xbc ? "bsf" : "bsr"));
			*si.buffer++ = ' ';
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			_nmd_append_Ev(&si);
		}
		else if (op == 0xa6)
		{
			const char* mnemonics[] = { "montmul", "xsha1", "xsha256" };
			_nmd_append_string(&si, mnemonics[instruction->modrm.fields.reg]);
		}
		else if (op == 0xa7)
		{
			const char* mnemonics[] = { "xstorerng", "xcryptecb", "xcryptcbc", "xcryptctr", "xcryptcfb", "xcryptofb" };
			_nmd_append_string(&si, mnemonics[instruction->modrm.fields.reg]);
		}
		else if (op == 0xae)
		{
			if (instruction->modrm.fields.mod == 0b11)
			{
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
					_nmd_append_string(&si, "pcommit");
				else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
				{
					_nmd_append_string(&si, "incsspd ");
					_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.rm]);
				}
				else
				{
					const char* mnemonics[] = { "rdfsbase", "rdgsbase", "wrfsbase", "wrgsbase", 0, "lfence", "mfence", "sfence" };
					_nmd_append_string(&si, mnemonics[instruction->modrm.fields.reg]);
				}
			}
			else
			{
				if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
				{
					_nmd_append_string(&si, instruction->modrm.fields.reg == 0b110 ? "clwb " : "clflushopt ");
					_nmd_append_modrm_upper(&si, "byte");
				}
				else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
				{
					_nmd_append_string(&si, instruction->modrm.fields.reg == 0b100 ? "ptwrite " : "clrssbsy ");
					_nmd_append_modrm_upper(&si, instruction->modrm.fields.reg == 0b100 ? "dword" : "qword");
				}
				else
				{
					const char* mnemonics[] = { "fxsave", "fxrstor", "ldmxcsr", "stmxcsr", "xsave", "xrstor", "xsaveopt", "clflush" };
					_nmd_append_string(&si, mnemonics[instruction->modrm.fields.reg]);
					*si.buffer++ = ' ';
					_nmd_append_modrm_upper(&si, "dword");
				}
			}
		}
		else if (op == 0xaf)
		{
			_nmd_append_string(&si, "imul ");
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			_nmd_append_Ev(&si);
		}
		else if (op == 0xb0 || op == 0xb1)
		{
			_nmd_append_string(&si, "cmpxchg ");
			if (op == 0xb0)
			{
				_nmd_append_Eb(&si);
				*si.buffer++ = ',';
				_nmd_append_Gb(&si);
			}
			else
			{
				_nmd_append_Ev(&si);
				*si.buffer++ = ',';
				_nmd_append_Gv(&si);
			}
		}
		else if (op == 0xb2)
		{
			_nmd_append_string(&si, "lss ");
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			_nmd_append_modrm_upper(&si, "fword");
		}
		else if (NMD_R(op) == 0xb && (op % 8) >= 6)
		{
			_nmd_append_string(&si, op > 0xb8 ? "movsx " : "movzx ");
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			if ((op % 8) == 6)
				_nmd_append_Eb(&si);
			else
				_nmd_append_Ew(&si);
		}
		else if (op == 0xb8)
		{
			_nmd_append_string(&si, "popcnt ");
			_nmd_append_Gv(&si);
			*si.buffer++ = ',';
			_nmd_append_Ev(&si);
		}
		else if (op == 0xba)
		{
			const char* mnemonics[] = { "bt","bts","btr","btc" };
			_nmd_append_string(&si, mnemonics[instruction->modrm.fields.reg - 4]);
			*si.buffer++ = ' ';
			_nmd_append_Ev(&si);
			*si.buffer++ = ',';
			_nmd_append_number(&si, instruction->immediate);
		}
		else if (op == 0xc0 || op == 0xc1)
		{
			_nmd_append_string(&si, "xadd ");
			if (op == 0xc0)
			{
				_nmd_append_Eb(&si);
				*si.buffer++ = ',';
				_nmd_append_Gb(&si);
			}
			else
			{
				_nmd_append_Ev(&si);
				*si.buffer++ = ',';
				_nmd_append_Gv(&si);
			}
		}
		else if (op == 0xc2)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "cmppd" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "cmpss" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? "cmpsd" : "cmpps")));
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			if (si.instruction->modrm.fields.mod == 0b11)
				_nmd_append_Udq(&si);
			else
				_nmd_append_modrm_upper(&si, instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "dword" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? "qword" : "xmmword"));
			*si.buffer++ = ',';
			_nmd_append_number(&si, instruction->immediate);
		}
		else if (op == 0xc3)
		{
			_nmd_append_string(&si, "movnti ");
			_nmd_append_modrm_upper(&si, "dword");
			*si.buffer++ = ',';
			_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.reg]);
		}
		else if (op == 0xc4)
		{
			_nmd_append_string(&si, "pinsrw ");
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
				_nmd_append_Vdq(&si);
			else
				_nmd_append_Pq(&si);
			*si.buffer++ = ',';
			if (si.instruction->modrm.fields.mod == 0b11)
				_nmd_append_string(&si, _nmd_reg32[si.instruction->modrm.fields.rm]);
			else
				_nmd_append_modrm_upper(&si, "word");
			*si.buffer++ = ',';
			_nmd_append_number(&si, instruction->immediate);
		}
		else if (op == 0xc5)
		{
			_nmd_append_string(&si, "pextrw ");
			_nmd_append_string(&si, _nmd_reg32[si.instruction->modrm.fields.reg]);
			*si.buffer++ = ',';
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
				_nmd_append_Udq(&si);
			else
				_nmd_append_Nq(&si);
			*si.buffer++ = ',';
			_nmd_append_number(&si, instruction->immediate);
		}
		else if (op == 0xc6)
		{
			_nmd_append_string(&si, "shufp");
			*si.buffer++ = instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? 'd' : 's';
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			_nmd_append_W(&si);
			*si.buffer++ = ',';
			_nmd_append_number(&si, instruction->immediate);
		}
		else if (op == 0xC7)
		{
			if (instruction->modrm.fields.reg == 0b001)
			{
				_nmd_append_string(&si, "cmpxchg8b ");
				_nmd_append_modrm_upper(&si, "qword");
			}
			else if (instruction->modrm.fields.reg <= 0b101)
			{
				const char* mnemonics[] = { "xrstors", "xsavec", "xsaves" };
				_nmd_append_string(&si, mnemonics[instruction->modrm.fields.reg - 3]);
				*si.buffer++ = ' ';
				_nmd_append_Eb(&si);
			}
			else if (instruction->modrm.fields.reg == 0b110)
			{
				if (instruction->modrm.fields.mod == 0b11)
				{
					_nmd_append_string(&si, "rdrand ");
					_nmd_append_Rv(&si);
				}
				else
				{
					_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "vmclear" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "vmxon" : "vmptrld"));
					*si.buffer++ = ' ';
					_nmd_append_modrm_upper(&si, "qword");
				}
			}
			else /* reg == 0b111 */
			{
				if (instruction->modrm.fields.mod == 0b11)
				{
					_nmd_append_string(&si, "rdseed ");
					_nmd_append_Rv(&si);
				}
				else
				{
					_nmd_append_string(&si, "vmptrst ");
					_nmd_append_modrm_upper(&si, "qword");
				}
			}
		}
		else if (op >= 0xc8 && op <= 0xcf)
		{
			_nmd_append_string(&si, "bswap ");
			_nmd_append_string(&si, (operandSize ? _nmd_reg16 : _nmd_reg32)[op % 8]);
		}
		else if (op == 0xd0)
		{
			_nmd_append_string(&si, "addsubp");
			*si.buffer++ = instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? 'd' : 's';
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			_nmd_append_W(&si);
		}
		else if (op == 0xd6)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "movq" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "movq2dq" : "movdq2q"));
			*si.buffer++ = ' ';
			if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT)
			{
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_Nq(&si);
			}
			else if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
			{
				_nmd_append_Pq(&si);
				*si.buffer++ = ',';
				_nmd_append_Udq(&si);
			}
			else
			{
				if (si.instruction->modrm.fields.mod == 0b11)
					_nmd_append_Udq(&si);
				else
					_nmd_append_modrm_upper(&si, "qword");
				*si.buffer++ = ',';
				_nmd_append_Vdq(&si);
			}
		}
		else if (op == 0xd7)
		{
			_nmd_append_string(&si, "pmovmskb ");
			_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.reg]);
			*si.buffer++ = ',';
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
				_nmd_append_Udq(&si);
			else
				_nmd_append_Nq(&si);
		}
		else if (op == 0xe6)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "cvttpd2dq" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "cvtdq2pd" : "cvtpd2dq"));
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			if (si.instruction->modrm.fields.mod == 0b11)
				_nmd_append_Udq(&si);
			else
				_nmd_append_modrm_upper(&si, instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "qword" : "xmmword");
		}
		else if (op == 0xe7)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "movntdq" : "movntq");
			*si.buffer++ = ' ';
			_nmd_append_modrm_upper(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "xmmword" : "qword");
			*si.buffer++ = ',';
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
				_nmd_append_Vdq(&si);
			else
				_nmd_append_Pq(&si);
		}
		else if (op == 0xf0)
		{
			_nmd_append_string(&si, "lddqu ");
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			_nmd_append_modrm_upper(&si, "xmmword");
		}
		else if (op == 0xf7)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "maskmovdqu " : "maskmovq ");
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
			{
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_Udq(&si);
			}
			else
			{
				_nmd_append_Pq(&si);
				*si.buffer++ = ',';
				_nmd_append_Nq(&si);
			}
		}
		else if (op >= 0xd1 && op <= 0xfe)
		{
			const char* mnemonics[] = { "srlw", "srld", "srlq", "addq", "mullw", 0, 0, "subusb", "subusw", "minub", "and", "addusb", "addusw", "maxub", "andn", "avgb", "sraw", "srad", "avgw", "mulhuw", "mulhw", 0, 0, "subsb", "subsw", "minsw", "or", "addsb", "addsw", "maxsw", "xor", 0, "sllw", "slld", "sllq", "muludq", "maddwd", "sadbw", 0, "subb", "subw", "subd", "subq", "addb", "addw", "addd" };
			*si.buffer++ = 'p';
			_nmd_append_string(&si, mnemonics[op - 0xd1]);
			*si.buffer++ = ' ';
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
			{
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_W(&si);
			}
			else
			{
				_nmd_append_Pq(&si);
				*si.buffer++ = ',';
				_nmd_append_Qq(&si);
			}
		}
		else if (op == 0xb9 || op == 0xff)
		{
			_nmd_append_string(&si, op == 0xb9 ? "ud1 " : "ud0 ");
			_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.reg]);
			*si.buffer++ = ',';
			if (instruction->modrm.fields.mod == 0b11)
				_nmd_append_string(&si, (instruction->mode == NMD_X86_MODE_64 ? _nmd_reg64 : _nmd_reg32)[instruction->modrm.fields.rm]);
			else
				_nmd_append_modrm_upper(&si, "dword");
		}
		else
		{
			const char* str = 0;
			switch (op)
			{
			case 0x31: str = "rdtsc"; break;
			case 0x07: str = "sysret"; break;
			case 0x06: str = "clts"; break;
			case 0x08: str = "invd"; break;
			case 0x09: str = "wbinvd"; break;
			case 0x0b: str = "ud2"; break;
			case 0x0e: str = "femms"; break;
			case 0x30: str = "wrmsr"; break;
			case 0x32: str = "rdmsr"; break;
			case 0x33: str = "rdpmc"; break;
			case 0x34: str = "sysenter"; break;
			case 0x35: str = "sysexit"; break;
			case 0x37: str = "getsec"; break;
			case 0x77: str = "emms"; break;
			case 0xa0: str = "push fs"; break;
			case 0xa1: str = "pop fs"; break;
			case 0xa8: str = "push gs"; break;
			case 0xa9: str = "pop gs"; break;
			case 0xaa: str = "rsm"; break;
			default: return;
			}
			_nmd_append_string(&si, str);
		}
	}
	else if (instruction->opcodeMap == NMD_X86_OPCODE_MAP_0F38)
	{
		if ((NMD_R(op) == 2 || NMD_R(op) == 3) && NMD_C(op) <= 5)
		{
			const char* mnemonics[] = { "pmovsxbw", "pmovsxbd", "pmovsxbq", "pmovsxwd", "pmovsxwq", "pmovsxdq" };
			_nmd_append_string(&si, mnemonics[NMD_C(op)]);
			if (NMD_R(op) == 3)
				*(si.buffer - 4) = 'z';
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			if (instruction->modrm.fields.mod == 0b11)
				_nmd_append_Udq(&si);
			else
				_nmd_append_modrm_upper(&si, NMD_C(op) == 5 ? "qword" : (NMD_C(op) % 3 == 0 ? "qword" : (NMD_C(op) % 3 == 1 ? "dword" : "word")));
		}
		else if (op >= 0x80 && op <= 0x83)
		{
			_nmd_append_string(&si, op == 0x80 ? "invept" : (op == 0x81 ? "invvpid" : "invpcid"));
			*si.buffer++ = ' ';
			_nmd_append_Gy(&si);
			*si.buffer++ = ',';
			_nmd_append_modrm_upper(&si, "xmmword");
		}
		else if (op >= 0xc8 && op <= 0xcd)
		{
			const char* mnemonics[] = { "sha1nexte", "sha1msg1", "sha1msg2", "sha256rnds2", "sha256msg1", "sha256msg2" };
			_nmd_append_string(&si, mnemonics[op - 0xc8]);
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			_nmd_append_W(&si);
		}
		else if (op == 0xcf)
		{
			_nmd_append_string(&si, "gf2p8mulb ");
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			_nmd_append_W(&si);
		}
		else if (op == 0xf0 || op == 0xf1)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO ? "crc32" : "movbe");
			*si.buffer++ = ' ';
			if (op == 0xf0)
			{
				if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
				{
					_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.reg]);
					*si.buffer++ = ',';
					_nmd_append_Eb(&si);
				}
				else
				{
					_nmd_append_Gv(&si);
					*si.buffer++ = ',';
					_nmd_append_Ev(&si);
				}
			}
			else
			{
				if (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT_NOT_ZERO)
				{
					_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.reg]);
					*si.buffer++ = ',';
					if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
						_nmd_append_Ew(&si);
					else
						_nmd_append_Ey(&si);
				}
				else
				{
					_nmd_append_Ev(&si);
					*si.buffer++ = ',';
					_nmd_append_Gv(&si);
				}
			}
		}
		else if (op == 0xf6)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "adcx" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "adox" : (instruction->operandSize64 ? "wrssq" : "wrssd")));
			*si.buffer++ = ' ';
			if (!instruction->simdPrefix)
			{
				_nmd_append_Ey(&si);
				*si.buffer++ = ',';
				_nmd_append_Gy(&si);
			}
			else
			{
				_nmd_append_Gy(&si);
				*si.buffer++ = ',';
				_nmd_append_Ey(&si);
			}
		}
		else if (op == 0xf5)
		{
			_nmd_append_string(&si, instruction->operandSize64 ? "wrussq " : "wrussd ");
			_nmd_append_modrm_upper(&si, instruction->operandSize64 ? "qword" : "dword");
			*si.buffer++ = ',';
			_nmd_append_string(&si, (instruction->operandSize64 ? _nmd_reg64 : _nmd_reg32)[instruction->modrm.fields.reg]);
		}
		else if (op == 0xf8)
		{
			_nmd_append_string(&si, instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE ? "movdir64b" : (instruction->simdPrefix == NMD_X86_PREFIXES_REPEAT ? "enqcmd" : "enqcmds"));
			*si.buffer++ = ' ';
			_nmd_append_string(&si, (instruction->mode == NMD_X86_MODE_64 ? _nmd_reg64 : (instruction->mode == NMD_X86_MODE_16 ? _nmd_reg16 : _nmd_reg32))[instruction->modrm.fields.rm]);
			*si.buffer++ = ',';
			_nmd_append_modrm_upper(&si, "zmmword");
		}
		else if (op == 0xf9)
		{
			_nmd_append_string(&si, "movdiri ");
			_nmd_append_modrm_upper_without_address_specifier(&si);
			*si.buffer++ = ',';
			_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.rm]);
		}
		else
		{
			if (op == 0x40)
				_nmd_append_string(&si, "pmulld");
			else if (op == 0x41)
				_nmd_append_string(&si, "phminposuw");
			else if (op >= 0xdb && op <= 0xdf)
			{
				const char* mnemonics[] = { "aesimc", "aesenc", "aesenclast", "aesdec", "aesdeclast" };
				_nmd_append_string(&si, mnemonics[op - 0xdb]);
			}
			else if (op == 0x37)
				_nmd_append_string(&si, "pcmpgtq");
			else if (NMD_R(op) == 2)
			{
				const char* mnemonics[] = { "pmuldq", "pcmpeqq", "movntdqa", "packusdw" };
				_nmd_append_string(&si, mnemonics[NMD_C(op) - 8]);
			}
			else if (NMD_R(op) == 3)
			{
				const char* mnemonics[] = { "pminsb", "pminsd", "pminuw", "pminud", "pmaxsb", "pmaxsd", "pmaxuw", "pmaxud" };
				_nmd_append_string(&si, mnemonics[NMD_C(op) - 8]);
			}
			else if (op < 0x10)
			{
				const char* mnemonics[] = { "pshufb", "phaddw", "phaddd", "phaddsw", "pmaddubsw", "phsubw", "phsubd", "phsubsw", "psignb", "psignw", "psignd", "pmulhrsw", "permilpsv", "permilpdv", "testpsv", "testpdv" };
				_nmd_append_string(&si, mnemonics[op]);
			}
			else if (op < 0x18)
				_nmd_append_string(&si, op == 0x10 ? "pblendvb" : (op == 0x14 ? "blendvps" : (op == 0x15 ? "blendvpd" : "ptest")));
			else
			{
				_nmd_append_string(&si, "pabs");
				*si.buffer++ = op == 0x1c ? 'b' : (op == 0x1d ? 'w' : 'd');
			}
			*si.buffer++ = ' ';
			if (instruction->simdPrefix == NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
			{
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				_nmd_append_W(&si);
			}
			else
			{
				_nmd_append_Pq(&si);
				*si.buffer++ = ',';
				_nmd_append_Qq(&si);
			}
		}

	}
	else if (instruction->opcodeMap == NMD_X86_OPCODE_MAP_0F3A)
	{
		if (NMD_R(op) == 1)
		{
			const char* mnemonics[] = { "pextrb", "pextrw", "pextrd", "extractps" };
			_nmd_append_string(&si, mnemonics[op - 0x14]);
			*si.buffer++ = ' ';
			if (instruction->modrm.fields.mod == 0b11)
				_nmd_append_string(&si, (si.instruction->operandSize64 ? _nmd_reg64 : _nmd_reg32)[instruction->modrm.fields.rm]);
			else
			{
				if (op == 0x14)
					_nmd_append_modrm_upper(&si, "byte");
				else if (op == 0x15)
					_nmd_append_modrm_upper(&si, "word");
				else if (op == 0x16)
					_nmd_append_Ey(&si);
				else
					_nmd_append_modrm_upper(&si, "dword");
			}
			*si.buffer++ = ',';
			_nmd_append_Vdq(&si);
		}
		else if (NMD_R(op) == 2)
		{
			_nmd_append_string(&si, op == 0x20 ? "pinsrb" : (op == 0x21 ? "insertps" : "pinsrd"));
			*si.buffer++ = ' ';
			_nmd_append_Vdq(&si);
			*si.buffer++ = ',';
			if (op == 0x20)
			{
				if (instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, _nmd_reg32[instruction->modrm.fields.rm]);
				else
					_nmd_append_modrm_upper(&si, "byte");
			}
			else if (op == 0x21)
			{
				if (instruction->modrm.fields.mod == 0b11)
					_nmd_append_Udq(&si);
				else
					_nmd_append_modrm_upper(&si, "dword");
			}
			else
				_nmd_append_Ey(&si);
		}
		else
		{
			if (op < 0x10)
			{
				const char* mnemonics[] = { "roundps", "roundpd", "roundss", "roundsd", "blendps", "blendpd", "pblendw", "palignr" };
				_nmd_append_string(&si, mnemonics[op - 8]);
			}
			else if (NMD_R(op) == 4)
			{
				const char* mnemonics[] = { "dpps", "dppd", "mpsadbw", 0, "pclmulqdq" };
				_nmd_append_string(&si, mnemonics[NMD_C(op)]);
			}
			else if (NMD_R(op) == 6)
			{
				const char* mnemonics[] = { "pcmpestrm", "pcmpestri", "pcmpistrm", "pcmpistri" };
				_nmd_append_string(&si, mnemonics[NMD_C(op)]);
			}
			else if (op > 0x80)
				_nmd_append_string(&si, op == 0xcc ? "sha1rnds4" : (op == 0xce ? "gf2p8affineqb" : (op == 0xcf ? "gf2p8affineinvqb" : "aeskeygenassist")));
			*si.buffer++ = ' ';
			if (op == 0xf && !(instruction->prefixes & (NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE | NMD_X86_PREFIXES_REPEAT | NMD_X86_PREFIXES_REPEAT_NOT_ZERO)))
			{
				_nmd_append_Pq(&si);
				*si.buffer++ = ',';
				_nmd_append_Qq(&si);
			}
			else
			{
				_nmd_append_Vdq(&si);
				*si.buffer++ = ',';
				if (instruction->modrm.fields.mod == 0b11)
					_nmd_append_string(&si, "xmm"), * si.buffer++ = (char)('0' + instruction->modrm.fields.rm);
				else
					_nmd_append_modrm_upper(&si, op == 0xa ? "dword" : (op == 0xb ? "qword" : "xmmword"));
			}
		}
		*si.buffer++ = ',';
		_nmd_append_number(&si, instruction->immediate);
	}

#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_ATT_SYNTAX
	if (formatFlags & NMD_X86_FORMAT_FLAGS_ATT_SYNTAX)
	{
		*si.buffer = '\0';
		char* operand = (char*)_nmd_reverse_strchr(buffer, ' ');
		if (operand && *(operand - 1) != ' ') /* If the instruction has a ' '(space character) and the left character of 'operand' is not ' '(space) the instruction has operands. */
		{
			/* If there is a memory operand. */
			const char* memoryOperand = _nmd_strchr(buffer, '[');
			if (memoryOperand)
			{
				/* If the memory operand has pointer size. */
				char* tmp2 = (char*)memoryOperand - (*(memoryOperand - 1) == ':' ? 7 : 4);
				if (_nmd_strstr(tmp2, "ptr") == tmp2)
				{
					/* Find the ' '(space) that is after two ' '(spaces). */
					tmp2 -= 2;
					while (*tmp2 != ' ')
						tmp2--;
					operand = tmp2;
				}
			}

			const char* const firstOperandConst = operand;
			char* firstOperand = operand + 1;
			char* secondOperand = 0;
			/* Convert each operand to AT&T syntax. */
			do
			{
				operand++;
				operand = _nmd_format_operand_to_att(operand, &si);
				if (*operand == ',')
					secondOperand = operand;
			} while (*operand);

			/* Swap operands. */
			if (secondOperand) /* At least two operands. */
			{
				/* Copy first operand to 'tmpBuffer'. */
				char tmpBuffer[64];
				char* i = tmpBuffer;
				char* j = firstOperand;
				for (; j < secondOperand; i++, j++)
					*i = *j;

				*i = '\0';

				/* Copy second operand to first operand. */
				for (i = secondOperand + 1; *i; firstOperand++, i++)
					*firstOperand = *i;

				*firstOperand++ = ',';

				/* 'firstOperand' is now the second operand. */
				/* Copy 'tmpBuffer' to second operand. */
				for (i = tmpBuffer; *firstOperand; i++, firstOperand++)
					*firstOperand = *i;
			}

			/* Memory operands change the mnemonic string(e.g. 'mov eax, dword ptr [ebx]' -> 'movl (%ebx), %eax'). */
			if (memoryOperand && !_nmd_strstr(firstOperandConst - 4, "lea"))
			{
				const char* r_char = _nmd_strchr(firstOperandConst, 'r');
				const char* e_char = _nmd_strchr(firstOperandConst, 'e');
				const char* call_str = _nmd_strstr(firstOperandConst - 5, "call");
				const char* jmp_str = _nmd_strstr(firstOperandConst - 4, "jmp");
				_nmd_insert_char(firstOperandConst, (instruction->mode == NMD_X86_MODE_64 && ((r_char && *(r_char - 1) == '%') || call_str || jmp_str)) ? 'q' : (instruction->mode == NMD_X86_MODE_32 && ((e_char && *(e_char - 1) == '%') || call_str || jmp_str) ? 'l' : 'b'));
				si.buffer++;
			}
		}
	}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_ATT_SYNTAX */

	size_t stringLength = si.buffer - buffer;
#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_UPPERCASE
	if (formatFlags & NMD_X86_FORMAT_FLAGS_UPPERCASE)
	{
		size_t i = 0;
		for (; i < stringLength; i++)
		{
			if (_NMD_IS_LOWERCASE(buffer[i]))
				buffer[i] -= 0x20; /* Capitalize letter. */
		}
	}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_UPPERCASE */

#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_COMMA_SPACES
	if (formatFlags & NMD_X86_FORMAT_FLAGS_COMMA_SPACES)
	{
		size_t i = 0;
		for (; i < stringLength; i++)
		{
			if (buffer[i] == ',')
			{
				/* Move all characters after the comma one position to the right. */
				size_t j = stringLength;
				for (; j > i; j--)
					buffer[j] = buffer[j - 1];

				buffer[i + 1] = ' ';
				si.buffer++, stringLength++;
			}
		}
	}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_COMMA_SPACES */

#ifndef NMD_ASSEMBLY_DISABLE_FORMATTER_OPERATOR_SPACES
	if (formatFlags & NMD_X86_FORMAT_FLAGS_OPERATOR_SPACES)
	{
		size_t i = 0;
		for (; i < stringLength; i++)
		{
			if (buffer[i] == '+' || (buffer[i] == '-' && buffer[i - 1] != ' ' && buffer[i - 1] != '('))
			{
				/* Move all characters after the operator two positions to the right. */
				size_t j = stringLength + 1;
				for (; j > i; j--)
					buffer[j] = buffer[j - 2];

				buffer[i + 1] = buffer[i];
				buffer[i] = ' ';
				buffer[i + 2] = ' ';
				si.buffer += 2, stringLength += 2;
				i++;
			}
		}
	}
#endif /* NMD_ASSEMBLY_DISABLE_FORMATTER_OPERATOR_SPACES */

	*si.buffer = '\0';
}

#define NMD_EMULATOR_RESOLVE_VA(va) ((void*)((uint64_t)cpu.physicalMemory + (va - cpu.virtualAddress)))

bool _nmd_check_jump_condition(nmd_x86_cpu* const cpu, uint8_t opcodeCondition)
{
	switch (opcodeCondition)
	{
	case 0x0: return cpu->flags.fields.OF == 1;                                                 /* Jump if overflow (OF=1) */
	case 0x1: return cpu->flags.fields.OF == 0;                                                 /* Jump if not overflow (OF=0) */
	case 0x2: return cpu->flags.fields.CF == 1;                                                 /* Jump if not above or equal (CF=1) */
	case 0x3: return cpu->flags.fields.CF == 0;                                                 /* Jump if not below (CF=0) */
	case 0x4: return cpu->flags.fields.ZF == 1;                                                 /* Jump if equal (ZF=1) */
	case 0x5: return cpu->flags.fields.ZF == 0;                                                 /* Jump if not equal (ZF=0) */
	case 0x6: return cpu->flags.fields.CF == 1 || cpu->flags.fields.ZF == 1;                    /* Jump if not above (CF=1 or ZF=1) */
	case 0x7: return cpu->flags.fields.CF == 0 && cpu->flags.fields.ZF == 0;                    /* Jump if not below or equal (CF=0 and ZF=0) */
	case 0x8: return cpu->flags.fields.SF == 1;                                                 /* Jump if sign (SF=1) */
	case 0x9: return cpu->flags.fields.SF == 0;                                                 /* Jump if not sign (SF=0) */
	case 0xa: return cpu->flags.fields.PF == 1;                                                 /* Jump if parity/parity even (PF=1) */
	case 0xb: return cpu->flags.fields.PF == 0;                                                 /* Jump if parity odd (PF=0) */
	case 0xc: return cpu->flags.fields.SF != cpu->flags.fields.OF;                              /* Jump if not greater or equal (SF != OF) */
	case 0xd: return cpu->flags.fields.SF == cpu->flags.fields.OF;                              /* Jump if not less (SF=OF) */
	case 0xe: return cpu->flags.fields.ZF == 1 || cpu->flags.fields.SF != cpu->flags.fields.OF; /* Jump if not greater (ZF=1 or SF != OF) */
	case 0xf: return cpu->flags.fields.ZF == 0 && cpu->flags.fields.SF == cpu->flags.fields.OF; /* Jump if not less or equal (ZF=0 and SF=OF) */
	default: return false;
	}
}

/* 
Checks if the number of set bits in an 8-bit number is even.
Credits: https://stackoverflow.com/questions/21617970/how-to-check-if-value-has-even-parity-of-bits-or-odd
*/
bool _nmd_is_parity_even8(uint8_t x)
{
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return !(x & 1);
}

void _nmd_copy_by_mode(void* dst, void* src, NMD_X86_MODE mode)
{
	if (mode == NMD_X86_MODE_32)
		*(int32_t*)(dst) = *(int32_t*)(src);
	else if (mode == NMD_X86_MODE_64)
		*(int64_t*)(dst) = *(int64_t*)(src);
	else /* (mode == NMD_X86_MODE_16) */
		*(int16_t*)(dst) = *(int16_t*)(src);
}

void _nmd_copy_by_operand_size(void* dst, void* src, nmd_x86_instruction* instruction)
{
	if(instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) = *(int16_t*)(src);
	else
		*(int32_t*)(dst) = *(int32_t*)(src);
}

void _nmd_add_by_operand_size(void* dst, void* src, nmd_x86_instruction* instruction)
{
	if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) += *(int16_t*)(src);
	else
		*(int32_t*)(dst) += *(int32_t*)(src);
}

void _nmd_or_by_operand_size(void* dst, void* src, nmd_x86_instruction* instruction)
{
	if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) |= *(int16_t*)(src);
	else
		*(int32_t*)(dst) |= *(int32_t*)(src);
} 

void _nmd_adc_by_operand_size(void* dst, void* src, nmd_x86_cpu* cpu, nmd_x86_instruction* instruction)
{
	if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) += *(int16_t*)(src) + cpu->flags.fields.CF;
	else
		*(int32_t*)(dst) += *(int32_t*)(src) + cpu->flags.fields.CF;
}

void _nmd_sbb_by_operand_size(void* dst, void* src, nmd_x86_cpu* cpu, nmd_x86_instruction* instruction)
{
	if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) -= *(int16_t*)(src) + cpu->flags.fields.CF;
	else
		*(int32_t*)(dst) -= *(int32_t*)(src) + cpu->flags.fields.CF;
}

void _nmd_and_by_operand_size(void* dst, void* src, nmd_x86_instruction* instruction)
{
	if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) &= *(int16_t*)(src);
	else
		*(int32_t*)(dst) &= *(int32_t*)(src);
}

void _nmd_sub_by_operand_size(void* dst, void* src, nmd_x86_instruction* instruction)
{
	if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) -= *(int16_t*)(src);
	else
		*(int32_t*)(dst) -= *(int32_t*)(src);
}

void _nmd_xor_by_operand_size(void* dst, void* src, nmd_x86_instruction* instruction)
{
	if (instruction->prefixes & NMD_X86_PREFIXES_OPERAND_SIZE_OVERRIDE)
		*(int16_t*)(dst) &= *(int16_t*)(src);
	else
		*(int32_t*)(dst) &= *(int32_t*)(src);
}

#define _NMD_GET_GREG(index) (&cpu->rax + (index)) /* general register */
#define _NMD_GET_RREG(index) (&cpu->r8 + (index)) /* r8,r9...r15 */
#define _NMD_GET_PHYSICAL_ADDRESS(address) (uint8_t*)((uint64_t)(cpu->physicalMemory)+((address)-cpu->virtualAddress))
#define _NMD_IN_BOUNDARIES(address) (address >= cpu->physicalMemory && address < endPhysicalMemory)
/* #define NMD_TEST(value, bit) ((value&(1<<bit))==(1<<bit)) */

void* _nmd_resolve_memory_operand(nmd_x86_cpu* cpu, nmd_x86_instruction* instruction)
{
	if (instruction->modrm.fields.mod == 0b11)
		return &_NMD_GET_GREG(instruction->modrm.fields.rm)->l64;
	else
	{
		int64_t va_expr; /* virtual address expression */

		if (instruction->hasSIB)
			va_expr = _NMD_GET_GREG(instruction->sib.fields.base)->l64 + _NMD_GET_GREG(instruction->sib.fields.index)->l64;
		else
			va_expr = _NMD_GET_GREG(instruction->modrm.fields.rm)->l64;

		va_expr += ((instruction->dispMask == NMD_X86_DISP8) ? (int8_t)instruction->displacement : (int32_t)instruction->displacement);

		return _NMD_GET_PHYSICAL_ADDRESS(va_expr);
	}
}

int64_t _nmd_resolve_memory_operand_va(nmd_x86_cpu* cpu, nmd_x86_instruction* instruction)
{
	int64_t va_expr; /* virtual address expression */

	if (instruction->hasSIB)
		va_expr = _NMD_GET_GREG(instruction->sib.fields.base)->l64 + _NMD_GET_GREG(instruction->sib.fields.index)->l64;
	else
		va_expr = _NMD_GET_GREG(instruction->modrm.fields.rm)->l64;

	return va_expr + ((instruction->dispMask == NMD_X86_DISP8) ? (int8_t)instruction->displacement : (int32_t)instruction->displacement);
}

/*
Emulates x86 code according to the state of the cpu. You MUST initialize the following variables before calling this
function: 'cpu->mode', 'cpu->physicalMemory', 'cpu->physicalMemorySize', 'cpu->virtualAddress' and 'cpu->rip'.
You may optionally initialize 'cpu->rsp' if a stack is desirable. Below is a short description of each variable:
 - 'cpu->mode': The emulator's operating architecture mode. 'NMD_X86_MODE_32', 'NMD_X86_MODE_64' or 'NMD_X86_MODE_16'.
 - 'cpu->physicalMemory': A pointer to a buffer used as the emulator's memory.
 - 'cpu->physicalMemorySize': The size of the 'physicalMemory' buffer in bytes.
 - 'cpu->virtualAddress': The starting address of the emulator's virtual address space.
 - 'cpu->rip': The virtual address where emulation starts.
 - 'cpu->rsp': The virtual address of the bottom of the stack.
Parameters:
 - cpu      [in] A pointer to a variable of type 'nmd_x86_cpu' that holds the state of the cpu.
 - maxCount [in] The maximum number of instructions that can be executed, or zero for unlimited instructions.
*/
bool nmd_x86_emulate(nmd_x86_cpu* cpu, size_t maxCount)
{
	const uint64_t endVirtualAddress = cpu->virtualAddress + cpu->physicalMemorySize;
	const void* endPhysicalMemory = (uint8_t*)cpu->physicalMemory + cpu->physicalMemorySize;

	cpu->count = 0;
	cpu->running = true;

	while (cpu->running)
	{
		nmd_x86_instruction instruction;
		const void* buffer = _NMD_GET_PHYSICAL_ADDRESS(cpu->rip);
		const bool validBuffer = _NMD_IN_BOUNDARIES(buffer);
		if (!validBuffer || !nmd_x86_decode_buffer(buffer, (size_t)(endVirtualAddress - cpu->rip), &instruction, (NMD_X86_MODE)cpu->mode, NMD_X86_DECODER_FLAGS_MINIMAL))
		{
			if (cpu->callback)
				cpu->callback(cpu, &instruction, validBuffer ? NMD_X86_EMULATOR_EXCEPTION_BAD_INSTRUCTION : NMD_X86_EMULATOR_EXCEPTION_BAD_MEMORY);
			cpu->running = false;
			return false;
		}

		if (instruction.opcodeMap == NMD_X86_OPCODE_MAP_DEFAULT)
		{
			if (instruction.opcode >= 0x88 && instruction.opcode <= 0x8b) /* mov [88,8b] */
			{
				nmd_x86_register* r0 = _NMD_GET_GREG(instruction.modrm.fields.reg);
				void* addr;
				if (instruction.modrm.fields.mod == 0b11)
					addr = _NMD_GET_GREG(instruction.modrm.fields.rm);
				else
				{
					int64_t x = _NMD_GET_GREG(instruction.modrm.fields.rm)->l64 + instruction.displacement;
					addr = _NMD_GET_PHYSICAL_ADDRESS(x);
					if (!_NMD_IN_BOUNDARIES(addr))
					{
						if (cpu->callback)
							cpu->callback(cpu, &instruction, NMD_X86_EMULATOR_EXCEPTION_BAD_MEMORY);
						cpu->running = false;
						return false;
					}
				}

				if (instruction.opcode == 0x88)
					*(int8_t*)(addr) = r0->l8;
				else if (instruction.opcode == 0x89)
				{
					_nmd_copy_by_mode(addr, r0, (NMD_X86_MODE)cpu->mode);
				}
				else if (instruction.opcode == 0x8a)
					r0->l8 = *(int8_t*)(addr);
				else /* if (instruction.opcode == 0x8b) */
				{
					_nmd_copy_by_mode(r0, addr, (NMD_X86_MODE)cpu->mode);
				}
			}
			else if (NMD_R(instruction.opcode) == 5) /* push,pop [50,5f] */
			{
				nmd_x86_register* r0 = _NMD_GET_GREG(instruction.opcode % 8);
				void* dst, * src;

				if (instruction.opcode < 0x58) /* push */
				{
					cpu->rsp.l64 -= (int8_t)cpu->mode;
					dst = _NMD_GET_PHYSICAL_ADDRESS(cpu->rsp.l64);
					src = r0;
				}
				else /* pop */
				{
					src = _NMD_GET_PHYSICAL_ADDRESS(cpu->rsp.l64);
					cpu->rsp.l64 += (int8_t)cpu->mode;
					dst = r0;
				}

				_nmd_copy_by_mode(dst, src, (NMD_X86_MODE)cpu->mode);
			}
			else if (instruction.opcode == 0xe8) /* call */
			{
				/* push the instruction pointer onto the stack. */
				cpu->rsp.l64 -= (int8_t)cpu->mode;
				_nmd_copy_by_mode(_NMD_GET_PHYSICAL_ADDRESS(cpu->rsp.l64), &cpu->rip, (NMD_X86_MODE)cpu->mode);

				/* jump */
				cpu->rip += (int32_t)instruction.immediate;
			}
			else if (instruction.opcode == 0xc3) /* ret */
			{
				/* pop rip */
				_nmd_copy_by_mode(&cpu->rip, _NMD_GET_PHYSICAL_ADDRESS(cpu->rsp.l64), (NMD_X86_MODE)cpu->mode);
				cpu->rsp.l64 += (int8_t)cpu->mode;
			}
			else if (instruction.opcode == 0xc2) /* ret imm8 */
			{
				/* pop rip */
				_nmd_copy_by_mode(&cpu->rip, _NMD_GET_PHYSICAL_ADDRESS(cpu->rsp.l64), (NMD_X86_MODE)cpu->mode);
				cpu->rsp.l64 += (int8_t)(cpu->mode + instruction.immediate);
			}
			else if (instruction.opcode == 0x8d) /* lea */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l64 = _nmd_resolve_memory_operand_va(cpu, &instruction);
			else if (instruction.opcode == 0xe9) /* jmp r32 */
				cpu->rip += (int32_t)instruction.immediate;
			else if (instruction.opcode == 0xeb) /* jmp r8 */
				cpu->rip += (int8_t)instruction.immediate;

			else if (instruction.opcode == 0x00) /* add Eb, Gb */
				*(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) += _NMD_GET_GREG(instruction.modrm.fields.reg)->l8;
			else if (instruction.opcode == 0x01) /* add Ev, Gv */
				_nmd_add_by_operand_size(_nmd_resolve_memory_operand(cpu, &instruction), _NMD_GET_GREG(instruction.modrm.fields.reg), &instruction);
			else if (instruction.opcode == 0x02) /* add Gb, Eb */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l8 += *(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction);
			else if (instruction.opcode == 0x03) /* add Gv, Ev */
				_nmd_add_by_operand_size(_NMD_GET_GREG(instruction.modrm.fields.reg), _nmd_resolve_memory_operand(cpu, &instruction), &instruction);
			else if (instruction.opcode == 0x04) /* add al, bl */
				cpu->rax.l8 += (int8_t)instruction.immediate;
			else if (instruction.opcode == 0x05) /* add rAX, lz */
				_nmd_add_by_operand_size(&cpu->rax, &instruction.immediate, &instruction);

			else if (instruction.opcode == 0x08) /* or Eb, Gb */
				*(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) |= _NMD_GET_GREG(instruction.modrm.fields.reg)->l8;
			else if (instruction.opcode == 0x09) /* or Ev, Gv */
				_nmd_or_by_operand_size(_nmd_resolve_memory_operand(cpu, &instruction), _NMD_GET_GREG(instruction.modrm.fields.reg), &instruction);
			else if (instruction.opcode == 0x0a) /* or Gb, Eb */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l8 |= *(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction);
			else if (instruction.opcode == 0x0b) /* or Gv, Ev */
				_nmd_or_by_operand_size(_NMD_GET_GREG(instruction.modrm.fields.reg), _nmd_resolve_memory_operand(cpu, &instruction), &instruction);
			else if (instruction.opcode == 0x0c) /* or al, bl */
				cpu->rax.l8 |= (int8_t)instruction.immediate;
			else if (instruction.opcode == 0x0d) /* or rAX, lz */
				_nmd_or_by_operand_size(&cpu->rax, &instruction.immediate, &instruction);

			else if (instruction.opcode == 0x10) /* adc Eb, Gb */
				*(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) += _NMD_GET_GREG(instruction.modrm.fields.reg)->l8 + cpu->flags.fields.CF;
			else if (instruction.opcode == 0x11) /* adc Ev, Gv */
				_nmd_adc_by_operand_size(_nmd_resolve_memory_operand(cpu, &instruction), _NMD_GET_GREG(instruction.modrm.fields.reg), cpu, &instruction);
			else if (instruction.opcode == 0x12) /* adc Gb, Eb */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l8 += *(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) + cpu->flags.fields.CF;
			else if (instruction.opcode == 0x13) /* adc Gv, Ev */
				_nmd_adc_by_operand_size(_NMD_GET_GREG(instruction.modrm.fields.reg), _nmd_resolve_memory_operand(cpu, &instruction), cpu, &instruction);
			else if (instruction.opcode == 0x14) /* adc al, bl */
				cpu->rax.l8 += (int8_t)instruction.immediate + cpu->flags.fields.CF;
			else if (instruction.opcode == 0x15) /* adc rAX, lz */
				_nmd_adc_by_operand_size(&cpu->rax, &instruction.immediate, cpu, &instruction);

			else if (instruction.opcode == 0x18) /* sbb Eb, Gb */
				*(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) -= _NMD_GET_GREG(instruction.modrm.fields.reg)->l8 + cpu->flags.fields.CF;
			else if (instruction.opcode == 0x19) /* sbb Ev, Gv */
				_nmd_sbb_by_operand_size(_nmd_resolve_memory_operand(cpu, &instruction), _NMD_GET_GREG(instruction.modrm.fields.reg), cpu, &instruction);
			else if (instruction.opcode == 0x1a) /* sbb Gb, Eb */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l8 -= *(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) + cpu->flags.fields.CF;
			else if (instruction.opcode == 0x1b) /* sbb Gv, Ev */
				_nmd_sbb_by_operand_size(_NMD_GET_GREG(instruction.modrm.fields.reg), _nmd_resolve_memory_operand(cpu, &instruction), cpu, &instruction);
			else if (instruction.opcode == 0x1c) /* sbb al, bl */
				cpu->rax.l8 -= (int8_t)instruction.immediate + cpu->flags.fields.CF;
			else if (instruction.opcode == 0x1d) /* sbb rAX, lz */
				_nmd_sbb_by_operand_size(&cpu->rax, &instruction.immediate, cpu, &instruction);

			else if (instruction.opcode == 0x20) /* and Eb, Gb */
				*(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) &= _NMD_GET_GREG(instruction.modrm.fields.reg)->l8;
			else if (instruction.opcode == 0x21) /* and Ev, Gv */
				_nmd_and_by_operand_size(_nmd_resolve_memory_operand(cpu, &instruction), _NMD_GET_GREG(instruction.modrm.fields.reg), &instruction);
			else if (instruction.opcode == 0x22) /* and Gb, Eb */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l8 &= *(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction);
			else if (instruction.opcode == 0x23) /* and Gv, Ev */
				_nmd_and_by_operand_size(_NMD_GET_GREG(instruction.modrm.fields.reg), _nmd_resolve_memory_operand(cpu, &instruction), &instruction);
			else if (instruction.opcode == 0x24) /* and al, bl */
				cpu->rax.l8 &= (int8_t)instruction.immediate;
			else if (instruction.opcode == 0x25) /* and rAX, lz */
				_nmd_and_by_operand_size(&cpu->rax, &instruction.immediate, &instruction);

			else if (instruction.opcode == 0x28) /* sub Eb, Gb */
				*(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) -= _NMD_GET_GREG(instruction.modrm.fields.reg)->l8;
			else if (instruction.opcode == 0x29) /* sub Ev, Gv */
				_nmd_sub_by_operand_size(_nmd_resolve_memory_operand(cpu, &instruction), _NMD_GET_GREG(instruction.modrm.fields.reg), &instruction);
			else if (instruction.opcode == 0x2a) /* sub Gb, Eb */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l8 -= *(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction);
			else if (instruction.opcode == 0x2b) /* sub Gv, Ev */
				_nmd_sub_by_operand_size(_NMD_GET_GREG(instruction.modrm.fields.reg), _nmd_resolve_memory_operand(cpu, &instruction), &instruction);
			else if (instruction.opcode == 0x2c) /* sub al, bl */
				cpu->rax.l8 -= (int8_t)instruction.immediate;
			else if (instruction.opcode == 0x2d) /* sub rAX, lz */
				_nmd_sub_by_operand_size(&cpu->rax, &instruction.immediate, &instruction);

			else if (instruction.opcode == 0x08) /* xor Eb, Gb */
				*(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction) ^= _NMD_GET_GREG(instruction.modrm.fields.reg)->l8;
			else if (instruction.opcode == 0x09) /* xor Ev, Gv */
				_nmd_xor_by_operand_size(_nmd_resolve_memory_operand(cpu, &instruction), _NMD_GET_GREG(instruction.modrm.fields.reg), &instruction);
			else if (instruction.opcode == 0x0a) /* xor Gb, Eb */
				_NMD_GET_GREG(instruction.modrm.fields.reg)->l8 ^= *(int8_t*)_nmd_resolve_memory_operand(cpu, &instruction);
			else if (instruction.opcode == 0x0b) /* xor Gv, Ev */
				_nmd_xor_by_operand_size(_NMD_GET_GREG(instruction.modrm.fields.reg), _nmd_resolve_memory_operand(cpu, &instruction), &instruction);
			else if (instruction.opcode == 0x0c) /* xor al, bl */
				cpu->rax.l8 ^= (int8_t)instruction.immediate;
			else if (instruction.opcode == 0x0d) /* xor rAX, lz */
				_nmd_xor_by_operand_size(&cpu->rax, &instruction.immediate, &instruction);

			else if (NMD_R(instruction.opcode) == 4) /* inc/dec [40,4f] */
			{
				nmd_x86_register* r0 = _NMD_GET_GREG(instruction.opcode % 8);
				instruction.opcode < 0x48 ? r0->l64++ : r0->l64--;
			}
			else if (NMD_R(instruction.opcode) == 7 && _nmd_check_jump_condition(cpu, NMD_C(instruction.opcode))) /* conditional jump r8 */
				cpu->rip += (int8_t)(instruction.immediate);
			else if (instruction.opcode >= 0x91 && instruction.opcode <= 0x97) /* xchg rax, ... */
			{
				const nmd_x86_register tmp = cpu->rax;
				nmd_x86_register* r0 = _NMD_GET_GREG(instruction.opcode - 0x91);
				cpu->rax = *r0;
				*r0 = tmp;
			}
			else if (instruction.opcode == 0x60) /* pusha,pushad */
			{
				void* stack = _NMD_GET_PHYSICAL_ADDRESS(cpu->rsp.l32);
				cpu->rsp.l32 -= cpu->mode * 8;
				if (instruction.mode == NMD_X86_MODE_32) /* pushad */
				{
					((uint32_t*)(stack))[0] = cpu->rax.l32;
					((uint32_t*)(stack))[1] = cpu->rcx.l32;
					((uint32_t*)(stack))[2] = cpu->rdx.l32;
					((uint32_t*)(stack))[3] = cpu->rbx.l32;
					((uint32_t*)(stack))[4] = cpu->rsp.l32;
					((uint32_t*)(stack))[5] = cpu->rbp.l32;
					((uint32_t*)(stack))[6] = cpu->rsi.l32;
					((uint32_t*)(stack))[7] = cpu->rdi.l32;
				}
				else /* if (instruction.mode == NMD_X86_MODE_16) pusha */
				{
					((uint16_t*)(stack))[0] = cpu->rax.l16;
					((uint16_t*)(stack))[1] = cpu->rcx.l16;
					((uint16_t*)(stack))[2] = cpu->rdx.l16;
					((uint16_t*)(stack))[3] = cpu->rbx.l16;
					((uint16_t*)(stack))[4] = cpu->rsp.l16;
					((uint16_t*)(stack))[5] = cpu->rbp.l16;
					((uint16_t*)(stack))[6] = cpu->rsi.l16;
					((uint16_t*)(stack))[7] = cpu->rdi.l16;
				}
			}
			else if (instruction.opcode == 0x61) /* popa,popad */
			{
				void* stack = _NMD_GET_PHYSICAL_ADDRESS(cpu->rsp.l32);
				if (instruction.mode == NMD_X86_MODE_32) /* popad */
				{
					cpu->rax.l32 = ((uint32_t*)(stack))[0];
					cpu->rcx.l32 = ((uint32_t*)(stack))[1];
					cpu->rdx.l32 = ((uint32_t*)(stack))[2];
					cpu->rbx.l32 = ((uint32_t*)(stack))[3];
					cpu->rsp.l32 = ((uint32_t*)(stack))[4];
					cpu->rbp.l32 = ((uint32_t*)(stack))[5];
					cpu->rsi.l32 = ((uint32_t*)(stack))[6];
					cpu->rdi.l32 = ((uint32_t*)(stack))[7];
				}
				else /* if (instruction.mode == NMD_X86_MODE_16) popa */
				{
					cpu->rax.l16 = ((uint16_t*)(stack))[0];
					cpu->rcx.l16 = ((uint16_t*)(stack))[1];
					cpu->rdx.l16 = ((uint16_t*)(stack))[2];
					cpu->rbx.l16 = ((uint16_t*)(stack))[3];
					cpu->rsp.l16 = ((uint16_t*)(stack))[4];
					cpu->rbp.l16 = ((uint16_t*)(stack))[5];
					cpu->rsi.l16 = ((uint16_t*)(stack))[6];
					cpu->rdi.l16 = ((uint16_t*)(stack))[7];
				}
				cpu->rsp.l32 += cpu->mode * 8;
			}
			else if (NMD_R(instruction.opcode) == 0xb) /* mov reg, imm */
			{
				const uint8_t width = (instruction.prefixes & NMD_X86_PREFIXES_REX_W && instruction.opcode >= 0xb8) ? 8 : instruction.mode;
				nmd_x86_register* r0 = instruction.prefixes & NMD_X86_PREFIXES_REX_B ? _NMD_GET_RREG(NMD_C(instruction.opcode)) : _NMD_GET_GREG(NMD_C(instruction.opcode));
				_nmd_copy_by_mode(r0, &instruction.immediate, (NMD_X86_MODE)width);
			}
			else if (instruction.opcode == 0x90)
			{
				if (instruction.simdPrefix == NMD_X86_PREFIXES_REPEAT) /* pause */
				{
					/* spin-wait loop ahead? */
				}
			}
			else if (instruction.opcode == 0x9e) /* sahf */
				cpu->flags.l8 = cpu->rax.l8;
			else if (instruction.opcode == 0x9f) /* lahf */
				cpu->rax.l8 = cpu->flags.l8;
			else if (instruction.opcode == 0xcc) /* int3 */
			{
				if (cpu->callback)
					cpu->callback(cpu, &instruction, NMD_X86_EMULATOR_EXCEPTION_BREAKPOINT);
			}
			else if (instruction.opcode == 0xf1) /* int1 */
			{
				if (cpu->callback)
					cpu->callback(cpu, &instruction, NMD_X86_EMULATOR_EXCEPTION_DEBUG);
			}
			else if (instruction.opcode == 0xce) /* into */
			{
				if (cpu->callback)
					cpu->callback(cpu, &instruction, NMD_X86_EMULATOR_EXCEPTION_OVERFLOW);
			}
			else if (instruction.opcode == 0xcd) /* int n */
			{
				if (cpu->callback)
					cpu->callback(cpu, &instruction, NMD_X86_EMULATOR_EXCEPTION_GENERAL_PROTECTION);
			}
			else if (instruction.opcode == 0xf4) /* hlt */
			{
				cpu->running = false;
				return true;
			}
			else if (instruction.opcode == 0xf5) /* cmc */
				cpu->flags.fields.CF = ~cpu->flags.fields.CF;
			else if(instruction.opcode == 0xf8) /* clc */
				cpu->flags.fields.CF = 0;
			else if (instruction.opcode == 0xf9) /* stc */
				cpu->flags.fields.CF = 1;
			else if (instruction.opcode == 0xfa) /* cli */
				cpu->flags.fields.IF = 0;
			else if (instruction.opcode == 0xfb) /* sti */
				cpu->flags.fields.IF = 1;
			else if (instruction.opcode == 0xfc) /* cld */
				cpu->flags.fields.DF = 0;
			else if (instruction.opcode == 0xfd) /* std */
				cpu->flags.fields.DF = 1;

			/* Push/Pop segment registers */
			else if (instruction.opcode == 0x06) /* push es*/
			{
				cpu->rsp.l64 -= cpu->mode;
				*(uint16_t*)cpu->rsp.l64 = cpu->es;
			}
			else if (instruction.opcode == 0x07) /* pop es */
			{
				cpu->es = *(uint16_t*)cpu->rsp.l64;
				cpu->rsp.l64 += cpu->mode;
			}
			else if (instruction.opcode == 0x16) /* push ss */
			{
				cpu->rsp.l64 -= cpu->mode;
				*(uint16_t*)cpu->rsp.l64 = cpu->ss;
			}
			else if (instruction.opcode == 0x17) /* pop ss */
			{
				cpu->ss = *(uint16_t*)cpu->rsp.l64;
				cpu->rsp.l64 += cpu->mode;
			}
			else if (instruction.opcode == 0x0e) /* push cs */
			{
				cpu->rsp.l64 -= cpu->mode;
				*(uint16_t*)cpu->rsp.l64 = cpu->cs;
			}
			else if (instruction.opcode == 0x1e) /* push ds */
			{
				cpu->rsp.l64 -= cpu->mode;
				*(uint16_t*)cpu->rsp.l64 = cpu->ds;
			}
			else if (instruction.opcode == 0x1f) /* pop ds */
			{
				cpu->ds = *(uint16_t*)cpu->rsp.l64;
				cpu->rsp.l64 += cpu->mode;
			}
		}
		else if (instruction.opcodeMap == NMD_X86_OPCODE_MAP_0F)
		{
			if (NMD_R(instruction.opcode) == 8 && _nmd_check_jump_condition(cpu, NMD_C(instruction.opcode))) /* conditional jump r32 */
				cpu->rip += (int32_t)(instruction.immediate);

			else if (instruction.opcode == 0xa0) /* push fs */
			{
				cpu->rsp.l64 -= cpu->mode;
				*(uint16_t*)cpu->rsp.l64 = cpu->fs;
			}
			else if (instruction.opcode == 0xa1) /* pop fs */
			{
				cpu->fs = *(uint16_t*)cpu->rsp.l64;
				cpu->rsp.l64 += cpu->mode;
			}
			else if (instruction.opcode == 0xa8) /* push gs */
			{
				cpu->rsp.l64 -= cpu->mode;
				*(uint16_t*)cpu->rsp.l64 = cpu->gs;
			}
			else if (instruction.opcode == 0xa9) /* pop gs */
			{
				cpu->gs = *(uint16_t*)cpu->rsp.l64;
				cpu->rsp.l64 += cpu->mode;
			}
		}
		else if (instruction.opcodeMap == NMD_X86_OPCODE_MAP_0F38)
		{

		}
		else /* if (instruction.opcodeMap == NMD_X86_OPCODE_MAP_0F_38) */
		{

		}

		/*
		if (r0)
		{
			cpu->flags.fields.ZF = (r0->l64 == 0);
			cpu->flags.fields.PF = _nmd_is_parity_even8(r0->l8);
			
		}
		*//* OF,SF,CF*/


		if (cpu->flags.fields.TF && cpu->callback)
			cpu->callback(cpu, &instruction, NMD_X86_EMULATOR_EXCEPTION_STEP);

		cpu->rip += instruction.length;

		if (maxCount > 0 && ++cpu->count >= maxCount)
			return true;
	}

	return true;
}

#endif /* NMD_ASSEMBLY_IMPLEMENTATION */
