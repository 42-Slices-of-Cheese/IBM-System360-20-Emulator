#include <stdexcept>

#include "registers.hpp"


/**
 * 
 * @brief Read a 16 bit value from a register.
 * 
 * @param reg The register to read from. Decoded from a binary value.
 * 
 * @returns A 16 bit value from the selected register.
 * 
 * @note Only registers R8 through R15 are valid.
 * 
 */
uint16_t Registers::readRegister(uint8_t reg)
{
    switch (reg)
    {
        case 8:
            return R8;

        case 9:
            return R9;
        
        case 10:
            return R10;
        
        case 11:
            return R11;
        
        case 12:
            return R12;

        case 13:
            return R13;

        case 14:
            return R14;
        
        case 15:
            return R15;
        
        default:
            throw std::invalid_argument("Invalid register. Only R8 through R15 are valid.");
    }
}

/**
 * 
 * @brief Writes a 16 bit the selected register.
 * 
 * @param reg The register to write to. Decoded from a binary value.
 * @param value The value to write to the selected register.
 * 
 * @note Only registers R8 through R15 are valid.
 * 
 */
void Registers::writeRegister(uint8_t reg, uint16_t value)
{
    switch (reg)
    {
        case 8:
            R8 = value;
            return;

        case 9:
            R9 = value;
            return;
        
        case 10:
            R10 = value;
            return;
        
        case 11:
            R11 = value;
            return;
        
        case 12:
            R12 = value;
            return;

        case 13:
            R13 = value;
            return;

        case 14:
            R14 = value;
            return;
        
        case 15:
            R15 = value;
            return;
        
        default:
            throw std::invalid_argument("Invalid register. Only R8 through R15 are valid.");
    }
}


/**
 * 
 * @brief Sets the Program Status Word register using a 32 bit value.
 * 
 * @param value The 32 bit value to be set in the register.
 * 
 */
void PSW::set_psw_register(uint32_t value)
{
    psw_register = value;
}

/**
 * 
 * @brief Sets the Program Status Word Register
 * 
 * @param state A PSW_STATE struct to use.
 * 
 */
void PSW::set_psw_register(PSW_STATE state)
{
    psw_register = 0;

    uint32_t tmp;

    psw_register |= state.INS_ADDRESS;
    psw_register <<= 16;

    psw_register |= state.FUNC_SPEC;
    tmp = psw_register & 0x0000000F;
    tmp <<= 12;
    psw_register &= 0xFFFF0000;
    psw_register |= tmp;

    psw_register |= state.DEVICE_ADDRESS;
    tmp = psw_register & 0x0000000F;
    tmp <<= 8;
    psw_register &= 0xFFFFF000;
    psw_register |= tmp;

    psw_register |= state.CHANNEL_MASK;
    tmp = psw_register & 0x00000001;
    tmp <<= 7;
    psw_register &= 0xFFFFFF00;
    psw_register |= tmp;

    psw_register |= state.ASCII_MODE;
    tmp = psw_register & 0x00000001;
    tmp <<= 6;
    psw_register &= 0xFFFFFF80;
    psw_register |= tmp;

    // Redundent
    psw_register |= state.NOT_USED;
    tmp = psw_register & 0x00000003;
    tmp <<= 4;
    psw_register &= 0xFFFFFFC0;
    psw_register |= tmp;

    psw_register |= state.CC;
    tmp = psw_register & 0x0000003;
    tmp <<= 2;
    psw_register &= 0xFFFFFFF0;
    psw_register |= tmp;

    // Redundent
    psw_register |= state.RESERVED;
}

/**
 * 
 * @brief Read the current state of the Program Status Word register.
 * 
 * @return The current state of the Program Status Word register in the form of a 16 bit value.
 * 
 */
uint16_t PSW::read_psw_register_value()
{
    return psw_register;
}

/**
 * 
 * @brief Read the current state of the Program Status Word register.
 * 
 * @return The current state of the Program Status Word register in the form of a PSW_STATE struct.
 * 
 */
PSW::PSW_STATE PSW::read_psw_register_state()
{
    PSW_STATE state;

    state.CC = (psw_register >> 2) & 0b11;
    state.ASCII_MODE = (psw_register >> 6) & 0b1;
    state.CHANNEL_MASK = (psw_register >> 7) & 0b1;
    state.DEVICE_ADDRESS = (psw_register >> 8) & 0xF;
    state.FUNC_SPEC = (psw_register >> 12) & 0xF;
    state.INS_ADDRESS = (psw_register >> 16) & 0xFFFF;

    return state;
}
