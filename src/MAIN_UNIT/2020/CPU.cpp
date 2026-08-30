#include <stdexcept>
#include <string>
#include <cmath>
#include <span>

#include "instructions.hpp"
#include "../storage.hpp"
#include "CPU.hpp"
#include "registers.hpp"
#include "../../utils/helpers.hpp"


CPU::CPU(Storage& pass_memory) : memory(pass_memory)
{

}

void CPU::runInstruction(RR instruction)
{
    uint16_t R1_value = readRegister(instruction.R1);
    uint16_t R2_value = readRegister(instruction.R2);

    int16_t OPERAND_1 = static_cast<int16_t>(R1_value);
    int16_t OPERAND_2 = static_cast<int16_t>(R2_value);

    if(instruction.MNEMONIC == "AR")
    {
        int32_t value = OPERAND_1 + OPERAND_2;

        uint16_t set_value = static_cast<uint16_t>(value);
        writeRegister(instruction.R1, set_value);
        setCC(value);
    }
    else if (instruction.MNEMONIC == "SR")
    {
        int32_t value = OPERAND_1 - OPERAND_2;
        uint16_t set_value = static_cast<uint16_t>(value);
        writeRegister(instruction.R1, set_value);     
        setCC(value);
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

    int16_t OPERAND_1 = static_cast<int16_t>(R1_value);
    int16_t OPERAND_2 = static_cast<int16_t>(MEMORY_value);

    if(instruction.MNEMONIC == "LH")
    {
        writeRegister(instruction.R1, MEMORY_value);
    }
    else if(instruction.MNEMONIC == "AH")
    {
        int32_t result = OPERAND_1 + OPERAND_2;
        uint16_t set_value = static_cast<uint16_t>(result);
        writeRegister(instruction.R1, set_value);
        setCC(result);
    }
    else if(instruction.MNEMONIC == "SH")
    {
        int32_t result = OPERAND_1 - OPERAND_2;
        uint16_t set_value = static_cast<uint16_t>(result);
        writeRegister(instruction.R1, set_value);
        setCC(result);
    }
    else if(instruction.MNEMONIC == "CH")
    {
        setCC(OPERAND_1, OPERAND_2);
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
    uint16_t address_1 = addressBuilder(instruction.B1, instruction.D1);
    uint16_t address_2 = addressBuilder(instruction.B2, instruction.D2);

    uint8_t bytes_1_length = instruction.L1 + 1;
    uint8_t bytes_2_length = instruction.L2 + 1;

    auto bytes_1 = std::make_unique_for_overwrite<uint8_t[]>(bytes_1_length);
    auto bytes_2 = std::make_unique_for_overwrite<uint8_t[]>(bytes_1_length);

    for(uint8_t i = 0; i <= instruction.L1; i++)
    {
        bytes_1[i] = memory.read_byte(address_1 + i);
    }

    for(uint8_t i = 0; i <= instruction.L2; i++)
    {
        bytes_2[i] = memory.read_byte(address_2 + i);
    }

    
    // std::pair<uint8_t, std::unique_ptr<uint8_t[]>>
    auto [sign_1, OPERAND_1] = check_packed_decimal({bytes_1.get(), bytes_1_length});
    auto [sign_2, OPERAND_2] = check_packed_decimal({bytes_2.get(), bytes_2_length});

    bool sign_1_positive = !isNegativeSign(sign_1);
    bool sign_2_positive = !isNegativeSign(sign_2);

    uint8_t OPERAND_1_length = bytes_1_length * 2;
    uint8_t OPERAND_2_length = bytes_2_length * 2;

    if(instruction.MNEMONIC == "PACK")
    {
        uint8_t packed_length = 1 + (bytes_2_length) / 2;
        uint8_t packed[packed_length];

        if(instruction.L2 % 2 == 1)
        {
            packed[0] = 0x0 | (bytes_2[0] & 0x0F);

            for(int i = 1; i < instruction.L2; i += 2)
            {
                packed[(i + 1) / 2] = (bytes_2[i] << 4) | (bytes_2[i + 1] & 0x0F);
            }
        }
        else
        {
            for(int i = 0; i < instruction.L2; i += 2)
            {
                packed[i / 2] = (bytes_2[i] << 4) | (bytes_2[i + 1] & 0x0F);
            }
        }

        packed[packed_length - 1] =  (bytes_2[instruction.L2] << 4) | (bytes_2[instruction.L2] >> 4);

        SS_write_zfill(packed, packed_length, instruction);
    }
    else if(instruction.MNEMONIC == "UNPK")
    {
        PSW_STATE state = read_psw_register_state();

        uint8_t zone_format = 0xF0;

        if(state.ASCII_MODE == 1)
        {
            zone_format = 0x50;
        }

        uint8_t zoned_length = 1 + ((instruction.L2 - 1) * 2);
        uint8_t zoned[zoned_length];

        for(uint8_t i = 0; i < zoned_length - 1; i++)
        {
            zoned[i] = zone_format | (bytes_2[static_cast<uint8_t>(floor(i / 2.0))] >> 4);
        }

        zoned[zoned_length - 1] = (bytes_2[instruction.L2] << 4) | (bytes_2[instruction.L2] >> 4);  

        SS_write_zfill(zoned, zoned_length, instruction);
    }
    else if(instruction.MNEMONIC == "MVO")
    {
        uint8_t shifted_length = bytes_2_length;
        uint8_t shifted[shifted_length];
        uint8_t nibble = bytes_1[instruction.L1] & 0x0F;

        shifted[0] = bytes_2[0] >> 4;
        shifted[shifted_length - 1] = (bytes_2[instruction.L2] << 4) | nibble;

        for(uint8_t i = 1; i < shifted_length - 1; i++)
        {
            shifted[i] = (bytes_2[i - 1] << 4) | (bytes_2[i] >> 4);
        }

        SS_write_zfill(shifted, shifted_length, instruction);
    }
    else if(instruction.MNEMONIC == "ZAP")
    {
        uint8_t sign = sign_2;

        bool is_zero = true;
        for (uint8_t i = 0; i < instruction.L2; i++)
        {
            if (bytes_2[i] != 0)
            {
                is_zero = false;
                break;
            }
        }

        if((bytes_2[instruction.L2] >> 4) != 0)
        {
            is_zero = false;
        }

        PSW_STATE state = read_psw_register_state();

        if(is_zero)
        {
            if(state.ASCII_MODE == _ASCII_MODE)
            {
                sign = ASCII_POSITIVE;
            }
            else
            {
                sign = EBCDIC_POSITIVE;
            }
        }
        else if(sign == ASCII_NEGATIVE || sign == EBCDIC_NEGATIVE)
        {
            if(state.ASCII_MODE == _ASCII_MODE)
            {
                sign = ASCII_NEGATIVE;
            }
            else
            {
                sign = EBCDIC_NEGATIVE;
            }
        }
        else
        {
            if(state.ASCII_MODE == _ASCII_MODE)
            {
                sign = ASCII_POSITIVE;
            }
            else
            {
                sign = EBCDIC_POSITIVE;
            }
        }
        
        bytes_2[instruction.L2] &= 0xF0;
        bytes_2[instruction.L2] |= sign;

        uint8_t write_buffer_length = bytes_1_length;

        uint8_t write_buffer[write_buffer_length]{};

        for(uint8_t i = 0; i <= instruction.L1; i++)
        {
            if(i > instruction.L2)
            {
                break;
            }

            write_buffer[instruction.L1 - i] = bytes_2[instruction.L2 - i];
        }

        SS_write_zfill(write_buffer, instruction.L1 + 1, instruction);

        setCC(sign, is_zero);
    }
    else if(instruction.MNEMONIC == "AP")
    {   
        uint8_t write_buffer_length = bytes_1_length;
        uint8_t write_buffer[write_buffer_length]{};
        
        if(OPERAND_2_length > OPERAND_1_length)
        {
            throw std::invalid_argument("OPERAND 2 is longer than OPERAND 1");
        }

        // TODO: Implement this
        if (sign_1_positive != sign_2_positive) 
        {
            throw std::invalid_argument("Different signs (subtraction) in AP not yet implemented");
        }

        uint8_t carry = 0;

        
    }
    else
    {
        throw_ins_err(instruction.MNEMONIC);
    }
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

    if (overflow)
    {
        state.CC = CC_OVERFLOW;
    }
    else if (wrapped == 0)
    {
        state.CC = CC_ZERO;
    }
    else if (wrapped < 0)
    {
        state.CC = CC_NEGATIVE;
    }
    else
    {
        state.CC = CC_POSITIVE;
    }

    set_psw_register(state);
}

/**
 * 
 * @breif Sets the condition code using signs
 * 
 * @param sign The sign nibble
 * @param is_zero If the number is zero or not
 * 
 */
void CPU::setCC(uint8_t sign, bool is_zero)
{
    PSW_STATE state = read_psw_register_state();

    if(is_zero)
    {
        state.CC = CC_ZERO;
    }
    else if(sign == 0xA || sign == 0xC)
    {
        state.CC = CC_POSITIVE;
    }
    else if ((sign & 0xB) == 0xB)
    {
        state.CC = CC_NEGATIVE;
    }
    else
    {
        // Redundant but safer
        throw std::invalid_argument("Invalid sign nibble");
    }

    set_psw_register(state);
}

/**
 * 
 * @brief Sets the condition code after a comparison operation.
 * 
 * @param operand_1 The value to compare.
 * @param operand_2 The value to be compared to.
 * 
 */
void CPU::setCC(int16_t operand_1, int16_t operand_2)
{
    PSW_STATE state = read_psw_register_state();  
    
    if(operand_1 == operand_2)
    {
        state.CC = CC_ZERO;
    }
    else if (operand_1 < operand_2)
    {
        state.CC = CC_NEGATIVE;
    }
    else
    {
        state.CC = CC_POSITIVE;
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
        return static_cast<uint16_t>(effective_address);
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

/**
 * 
 * @brief Check if 2 signs are equal or not
 * 
 * The results are irrespective of the type of the sign. e.g., an ASCII negative is the same as an EBCDIC negative.
 * 
 * @param sign_1 The first sign
 * @param sign_2 The second sign
 * 
 * @return `true` if the signs are equal, and `false` if not
 * 
 * @note This function may be deprecated
 * 
 */
bool CPU::check_sign_equality(uint8_t sign_1, uint8_t sign_2)
{
    bool sign_1_positive = (sign_1 == ASCII_POSITIVE || sign_1 == EBCDIC_POSITIVE || sign_1 == UNDEFINED_POSITIVE_1 || sign_1 == UNDEFINED_POSITIVE_2);
    bool sign_2_positive = (sign_2 == ASCII_POSITIVE || sign_2 == EBCDIC_POSITIVE || sign_2 == UNDEFINED_POSITIVE_1 || sign_2 == UNDEFINED_POSITIVE_2);
    if((sign_1_positive && sign_2_positive) || (!sign_1_positive && !sign_2_positive))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Check if packed decimal is valid, then reutrn the sign along with the packed decimal as an array
 * 
 * @param bytes The array of bytes for the OPERAND
 * 
 */
std::pair<uint8_t, std::unique_ptr<uint8_t[]>> CPU::check_packed_decimal(std::span<const uint8_t> bytes)
{

    uint8_t sign = bytes[bytes.size() - 1] & 0x0F;
    uint8_t last_digit = bytes[bytes.size() - 1] >> 4;

    uint8_t number_as_array_length = bytes.size() * 2;

    std::unique_ptr<uint8_t[]> number_as_array = std::make_unique_for_overwrite<uint8_t[]>(number_as_array_length);

    if(sign < 9)
    {
        throw std::invalid_argument("Invalid packed decimal sign\n");
    }
    else if (last_digit > 9)
    {
        throw std::invalid_argument("Invalid packed decimal digit\n");
    }

    uint8_t sign_offset = 1;
    
    for (uint8_t i = 0; i < bytes.size() - 1; i++)
    {
        uint8_t high_nibble = bytes[i] >> 4;
        uint8_t low_nibble = bytes[i] & 0x0F;

        if (high_nibble > 9 || low_nibble > 9)
        {
            throw std::invalid_argument("Invalid packed decimal digit\n");
        }

        number_as_array[i * 2] = high_nibble;
        number_as_array[i * 2 + 1] = low_nibble;
    }

    number_as_array[number_as_array_length - 2] = last_digit;
    number_as_array[number_as_array_length - 1] = sign;

    return {sign, std::move(number_as_array)};
}

bool CPU::isNegativeSign(uint8_t sign)
{
    return (sign == ASCII_NEGATIVE || sign == EBCDIC_NEGATIVE);
}

std::unique_ptr<uint8_t[]> addNumberAsArray(std::span<const uint8_t> num1, std::span<const uint8_t> num2)
{
    std::unique_ptr<uint8_t[]> ret;

    if(num1.size() > num2.size())
    {
        
        ret = std::make_unique_for_overwrite<uint8_t[]>(num1.size());
    }
    else
    {
        ret = std::make_unique_for_overwrite<uint8_t[]>(num2.size());
    }
    
}

/**
 * 
 * @brief Writes memory and zero fills as needed
 *  TODO: fix this
 * @param bytes The array of bytes to write
 * @param bytes_length The length of the array of bytes
 * @param instruction The instruction that this function is running in
 * 
 * @note The `instruction` argument makes the function easier to implement
 * 
 */
void CPU::SS_write_zfill(uint8_t (&bytes)[], uint8_t bytes_length, SS instruction)
{
    uint16_t address_1 = addressBuilder(instruction.B1, instruction.D1);
    for(uint8_t i = 0; i <= instruction.L1; i++)
    {
        if(i >= bytes_length)
        {
            memory.write_byte(address_1 + instruction.L1 - i, 0);
        }
        else
        {
            memory.write_byte(address_1 + instruction.L1 - i, bytes[bytes_length - 1 - i]);
        }
    }
}

std::unique_ptr<uint8_t[]> zfill_packed(std::span<const uint8_t> num, uint8_t full_length)
{
    auto ret = std::make_unique_for_overwrite<uint8_t[]>(full_length);

    uint8_t zero_length = full_length - num.size();

    for(uint8_t i = 0; i < zero_length; i++)
    {
        
    }

    return nullptr;
}