#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "base_types.h"

static const u8 MOV_LIT_REG = 0x10;
static const u8 MOV_REG_REG = 0x11;
static const u8 MOV_REG_MEM = 0x12;
static const u8 MOV_MEM_REG = 0x13;

static const u8 ADD_REG_REG = 0x14;

static const u8 JNE_LIT = 0x15;

static const u8 PSH_LIT = 0x17;
static const u8 PSH_REG = 0x18;
static const u8 POP = 0x1A;

static const u8 CAL_LIT = 0x5E;
static const u8 CAL_REG = 0x5F;

static const u8 RET = 0x60;

static const u8 HLT = 0xFF;
#endif
