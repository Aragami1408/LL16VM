#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "base_types.h"

static const u8 MOV_LIT_REG = 0x10;
static const u8 MOV_REG_REG = 0x11;
static const u8 MOV_REG_MEM = 0x12;
static const u8 MOV_MEM_REG = 0x13;
static const u8 MOV_LIT_MEM = 0x1B;
static const u8 MOV_REG_PTR_REG = 0x1C;
static const u8 MOV_LIT_OFF_REG = 0x1D;

static const u8 ADD_REG_REG = 0x14;
static const u8 ADD_LIT_REG = 0x3F;
static const u8 SUB_LIT_REG = 0x16;
static const u8 SUB_REG_LIT = 0x1E;
static const u8 SUB_REG_REG = 0x1F;
static const u8 INC_REG = 0x35;
static const u8 DEC_REG = 0x36;
static const u8 MUL_LIT_REG = 0x20;
static const u8 MUL_REG_REG = 0x21;

static const u8 LSF_REG_LIT = 0x26;
static const u8 LSF_REG_REG = 0x27;
static const u8 RSF_REG_LIT = 0x2A;
static const u8 RSF_REG_REG = 0x2B;
static const u8 AND_REG_LIT = 0x2E;
static const u8 AND_REG_REG = 0x2F;
static const u8 OR_REG_LIT = 0x30;
static const u8 OR_REG_REG = 0x31;
static const u8 XOR_REG_LIT = 0x32;
static const u8 XOR_REG_REG = 0x33;
static const u8 NOT = 0x34;

static const u8 JNE_LIT = 0x15;
static const u8 JNE_REG = 0x40;
static const u8 JEQ_REG = 0x3E;
static const u8 JEQ_LIT = 0x41;
static const u8 JLT_LIT = 0x42;
static const u8 JLT_REG = 0x43;
static const u8 JGT_LIT = 0x44;
static const u8 JGT_REG = 0x45;
static const u8 JLE_LIT = 0x46;
static const u8 JLE_REG = 0x47;
static const u8 JGE_LIT = 0x48;
static const u8 JGE_REG = 0x49;


static const u8 PSH_LIT = 0x17;
static const u8 PSH_REG = 0x18;
static const u8 POP = 0x1A;

static const u8 CAL_LIT = 0x5E;
static const u8 CAL_REG = 0x5F;
static const u8 RET = 0x60;

static const u8 HLT = 0xFF;
#endif
