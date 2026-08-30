#ifndef _REGISTERS_H
#define _REGISTERS_H

#include <cstdint>
#include <string>


class PSW
{
    private:
        uint32_t psw_register;
 

    public:
        static const uint8_t CC_ZERO = 0;
        static const uint8_t CC_NEGATIVE = 1;
        static const uint8_t CC_POSITIVE = 2;
        static const uint8_t CC_OVERFLOW = 3;

        static const uint8_t _ASCII_MODE = 1;
        static const uint8_t _EBCDIC_MODE = 0;

        static const uint8_t ASCII_POSITIVE = 0xA;
        static const uint8_t ASCII_NEGATIVE = 0xB;
        static const uint8_t EBCDIC_POSITIVE = 0xC;
        static const uint8_t EBCDIC_NEGATIVE = 0xD;
        static const uint8_t UNDEFINED_POSITIVE_1 = 0xE;
        static const uint8_t UNDEFINED_POSITIVE_2 = 0xF;

        struct PSW_STATE 
        {
            const unsigned int RESERVED : 2 = 0b00; // Not used
            unsigned int CC : 2;                    // Condition Code (flags)
            const unsigned int NOT_USED : 2 = 0b00; // Not used
            unsigned int ASCII_MODE : 1;           
            unsigned int CHANNEL_MASK : 1;
            unsigned int DEVICE_ADDRESS : 4;
            unsigned int FUNC_SPEC : 4;             // Function specification
            uint16_t INS_ADDRESS;                   // The address of the (previous current next) instruction
        };

        uint16_t read_psw_register_value();
        PSW_STATE read_psw_register_state();
    
    protected:
        void set_psw_register(uint32_t value);
        void set_psw_register(PSW_STATE state);
};

class Registers : public PSW
{
    private:
        
    protected:
        uint16_t R8;
        uint16_t R9;
        uint16_t R10;
        uint16_t R11;
        uint16_t R12;
        uint16_t R13;
        uint16_t R14;
        uint16_t R15;
    public:
        uint16_t readRegister(uint8_t reg);
        void writeRegister(uint8_t reg, uint16_t value);
};

#endif