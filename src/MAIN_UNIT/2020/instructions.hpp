#ifndef _INSTYPES_H
#define _INSTYPES_H

#include <stdint.h>
#include <string>

struct Instruction 
{
    std::string FORMAT;
    std::string MNEMONIC;
};

struct RR : Instruction
{
    unsigned int R1 : 4;
    unsigned int R2 : 4;
};

struct RX : Instruction
{
    unsigned int R1 : 4;
    unsigned int B2 : 4;
    unsigned int D2 : 12;

};

struct SI : Instruction
{
    unsigned int B1 : 4;
    unsigned int D1 : 12;
    uint8_t I2;
};

struct SS : Instruction
{
    unsigned int L1 : 4;
    unsigned int L2 : 4;
    unsigned int B1 : 4;
    unsigned int D1 : 12;
    unsigned int B2 : 4;
    unsigned int D2 : 12;
};

#endif
