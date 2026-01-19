#ifndef _2020_H
#define _2020_H

#include "../storage.hpp"
#include "instructions.hpp"
#include "registers.hpp"
#include <optional>


class CPU : public Registers
{
    private:
        Storage& memory;

        void setCC(int32_t value);
        void setCC(int16_t value_1, int16_t value_2);
        uint16_t addressBuilder(uint8_t B, uint16_t D);
        void throw_ins_err(std::string ins);

    public:
        CPU(Storage& pass_memory);
        void runInstruction(RR instruction);
        void runInstruction(RX instruction);
        void runInstruction(SI instruction);
        void runInstruction(SS instruction);    
};

#endif