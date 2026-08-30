#ifndef _2020_H
#define _2020_H

#include "../storage.hpp"
#include "instructions.hpp"
#include "registers.hpp"


class CPU : public Registers
{
    private:
        Storage& memory;
        
        void setCC(int32_t value);
        void setCC(uint8_t sign, bool is_zero);
        void setCC(int16_t value_1, int16_t value_2);
        uint16_t addressBuilder(uint8_t B, uint16_t D);
        void throw_ins_err(std::string ins);
        bool check_sign_equality(uint8_t sign_1, uint8_t sign_2);
        std::pair<uint8_t, std::unique_ptr<uint8_t[]>> check_packed_decimal(std::span<const uint8_t> bytes);
        bool isNegativeSign(uint8_t sign);
        void SS_write_zfill(uint8_t (&bytes)[], uint8_t bytes_length, SS instruction);

    public:
        CPU(Storage& pass_memory);
        void runInstruction(RR instruction);
        void runInstruction(RX instruction);
        void runInstruction(SI instruction);
        void runInstruction(SS instruction);    
};

#endif