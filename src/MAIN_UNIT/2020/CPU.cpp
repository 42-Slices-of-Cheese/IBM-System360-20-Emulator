#include <stdio.h>
#include "instructions.hpp"
#include "../storage.hpp"
#include "CPU.hpp"
#include "registers.hpp"
#include "../../utils/helpers.hpp"
#include <cstring>
#include <stdexcept>
#include <string>

CPU::CPU(Storage& pass_memory) : memory(pass_memory)
{

}

void CPU::runInstruction(RR instruction)
{
    uint16_t R1_value = readRegister(instruction.R1);
    uint16_t R2_value = readRegister(instruction.R2);

    if(instruction.MNEMONIC == "AR")
    {
        int32_t value = static_cast<int16_t>(R1_value) + static_cast<int16_t>(R2_value);
        uint16_t set_value = static_cast<uint16_t>(value);
        writeRegister(instruction.R1, set_value);
        setCC(value);
    }
    else if (instruction.MNEMONIC == "SR")
    {
        int32_t value = static_cast<int16_t>(R1_value) - static_cast<int16_t>(R2_value);
        uint16_t set_value = static_cast<uint16_t>(value);
        writeRegister(instruction.R1, set_value);      
    }
    else
    {
        throw_ins_err(instruction.MNEMONIC);
    }
    
}

void CPU::runInstruction(RX instruction)
{
    uint16_t R1_value = readRegister(instruction.R1);
    uint16_t address = addressBuilder(instruction.B2, instruction.D2);

    if(address % 2 == 1)
    {
        throw std::invalid_argument("Address must be even");
    }

    uint16_t MEMORY_value = memory.read_halfword(address);

    if(instruction.MNEMONIC == "LH")
    {
        writeRegister(instruction.R1, MEMORY_value);
    }
    else if(instruction.MNEMONIC == "AH")
    {
        int32_t result = static_cast<int16_t>(R1_value) + static_cast<int16_t>(MEMORY_value);
        uint16_t set_value = static_cast<uint16_t>(result);
        writeRegister(instruction.R1, set_value);
        setCC(result);
    }
    else if(instruction.MNEMONIC == "SH")
    {
        int32_t result = static_cast<int16_t>(R1_value) - static_cast<int16_t>(MEMORY_value);
        uint16_t set_value = static_cast<uint16_t>(result);
        writeRegister(instruction.R1, set_value);
        setCC(result);
    }
    else if(instruction.MNEMONIC == "CH")
    {
        setCC(static_cast<int16_t>(R1_value), static_cast<int16_t>(MEMORY_value));
    }
    else if (instruction.MNEMONIC == "STH")
    {
        memory.write_halfword(address, R1_value);
    }
    else
    {
        throw_ins_err(instruction.MNEMONIC);
    }
}

void CPU::runInstruction(SI instruction)
{
    
}

void CPU::runInstruction(SS instruction)
{
    
}

/**
 * 
 * @brief Sets the condition code after an arithmetic operation
 * 
 * @param value The value to check
 * 
 */
void CPU::setCC(int32_t value)
{
    PSW_STATE state = read_psw_register_state();

    int16_t wrapped = static_cast<int16_t>(value);

    bool overflow = ((value >> 16) != 0 && (value >> 16) != -1);

    // Set CC according to S/360 rules
    if (overflow)
    {
        state.CC = 3;   // overflow
    }
    else if (wrapped == 0)
    {
        state.CC = 0;   // zero
    }
    else if (wrapped < 0)
    {
        state.CC = 1;   // negative
    }
    else
    {
        state.CC = 2;   // positive
    }

    set_psw_register(state);
}

void CPU::setCC(int16_t operand_1, int16_t operand_2)
{
    PSW_STATE state = read_psw_register_state();  
    
    if(operand_1 == operand_2)
    {
        state.CC = 0;
    }
    else if (operand_1 < operand_2)
    {
        state.CC = 1;
    }
    else
    {
        state.CC = 2;
    }
    

    set_psw_register(state);
}

/**
 * 
 * @brief Constructs an address
 * 
 * @param B The base (register)
 * @param D The displacement
 * 
 * @return A semi-valid address
 * 
 * @exception out_of_range when the effective address is negative
 * 
 */
uint16_t CPU::addressBuilder(uint8_t B, uint16_t D)
{
    if(B >= 8)
    {
        int16_t base = static_cast<int16_t>(readRegister(B));
        int32_t effective_address = static_cast<int32_t>(base) + static_cast<int32_t>(D);

        if(effective_address < 0)
        {
            throw std::out_of_range("Negative addresses are invalid.");
        }

        effective_address &= 0x3FFF;
        uint16_t REA = static_cast<uint16_t>(effective_address);
    }
    else 
    {
        return D |= static_cast<uint16_t>(B & 0b11) << 12;
    }
}

void CPU::throw_ins_err(std::string ins)
{
    std::string error_string = "This instruction does not exist or is not implemented yet: ";
    error_string.append(ins);
    throw std::invalid_argument(error_string);
}
