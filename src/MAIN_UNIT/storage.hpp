#ifndef _STORAGE_H
#define _STORAGE_H

#include <cstdint>
#include <memory>

#define CPU_RESERVED_BYTES 144
#define AMOUNT_OF_OPTIONS 4


/**
 * 
 * @class Storage
 * @brief The storage unit of the 2020 CPU
 * 
 */
class Storage
{
    private:
        uint16_t max_size;
        const uint16_t options[AMOUNT_OF_OPTIONS] = {4096, 8192, 12288, 16384};
        std::unique_ptr<uint8_t[]> memory;
        
    public:
        Storage(uint16_t size);
        uint8_t read_byte(uint16_t address);
        uint16_t read_halfword(uint16_t address);
        void write_byte(uint16_t address, uint8_t value);
        void write_halfword(uint16_t address, uint16_t value);
        void addressCheck(uint16_t address, uint8_t lowerBound, uint16_t upperBound);
};

#endif