#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "base_types.h"

const u8 MOV_LIT_REG = 0x10;
const u8 MOV_REG_REG = 0x11;
const u8 MOV_REG_MEM = 0x12;
const u8 MOV_MEM_REG = 0x13;

const u8 ADD_REG_REG = 0x14;

const u8 JNE_LIT = 0x15;
#endif
