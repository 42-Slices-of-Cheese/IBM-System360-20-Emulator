#include <vector>
#include <stdexcept>
#include "storage.hpp"
#include <cstdint>
#include "../utils/helpers.hpp"

/**
 * 
 * @brief Constructor for Storage
 * 
 * @param size The amount of memory of the storage unit.
 * 
 * @note The options are 4096, 8192, 12288, or 16384.
 * 
 */
Storage::Storage(uint16_t size)
{
    for(int i = 0; i < AMOUNT_OF_OPTIONS; i++)
    {
        if(size == options[i])
        {
            max_size = size;
            return;
        }
    }

    std::string full_options;
    for(int i = 0; i < AMOUNT_OF_OPTIONS; i++)
    {            
        if(i != AMOUNT_OF_OPTIONS - 1 && i != 0)
            full_options += ", ";
        else if(i != 0)
            full_options += ", or ";

        full_options += std::to_string(options[i]);
    }
    throw std::invalid_argument("\nInvalid storage size.\nYou chose: " + std::to_string(size) +  "\nOptions are: " + full_options + ".");
}

/**
 * 
 * Reads a single byte from memory.
 * @param address The address to read the byte from.
 * 
 */
uint8_t Storage::read_byte(uint16_t address)
{
    addressCheck(address, CPU_RESERVED_BYTES, max_size);

    return memory[address];
}

/**
 * 
 * Reads a 2 bytes (a halfword) from memory.
 * @param address The address to read the byte from.
 * 
 */
uint16_t Storage::read_halfword(uint16_t address)
{
    return (static_cast<uint16_t>(read_byte(address)) << 8) | static_cast<uint16_t>(read_byte(address + 1));
}

/**
 * 
 * Writes a single byte to memeory.
 * @param address The address to write with.
 * @param value The single byte value to write to memory.
 * 
 */
void Storage::write_byte(uint16_t address, uint8_t value)
{
    addressCheck(address, CPU_RESERVED_BYTES, max_size);

    memory[address] = value;
}

/**
 * 
 * Writes 2 bytes (a halfword) to memeory.
 * @param address The address to write with.
 * @param value The 2 byte value (halfword) to write to memory.
 * 
 */
void Storage::write_halfword(uint16_t address, uint16_t value)
{
    addressCheck(address, CPU_RESERVED_BYTES, max_size);
    addressCheck(address + 1, CPU_RESERVED_BYTES, max_size);

    uint8_t VH = static_cast<uint8_t>(value >> 8);
    uint8_t VL = static_cast<uint8_t>(value);

    memory[address] = VH;
    memory[address + 1] = VL;
}


/**
 * @param address The address that is attempted to be accessed.
 * @param lowerBound Any address lower than this inaccessable (Exclusive).
 * @param upperBound Any address higher than this inaccessable (Inclusive).
 */
void Storage::addressCheck(uint16_t address, uint8_t lowerBound, uint16_t upperBound)
{
    if(address > upperBound)
        throw std::out_of_range("Memory address too high.");

    if(address < lowerBound)
        throw std::out_of_range("CPU reserved bytes.");
}