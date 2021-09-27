#include "CRC.h"

uint32_t CRC32::Calculate(const void* data, size_t size)
{
    uint32_t remainder = CalculateRemainder(data, size, 0xFFFFFFFF);
    return Finalize(remainder, 0xFFFFFFFF);
}

uint32_t CRC32::Calculate(const void* data, size_t size, uint32_t crc)
{
    uint32_t remainder = UndoFinalize(crc, 0xFFFFFFFF);
    remainder = CalculateRemainder(data, size, remainder);
    return Finalize(remainder, 0xFFFFFFFF);
}

uint32_t CRC32::Reflect(uint32_t value, uint16_t numBits)
{
    uint32_t reversedValue(0);

    for (uint16_t i = 0; i < numBits; ++i)
    {
        reversedValue = static_cast<uint32_t>((reversedValue << 1) | (value & 1));
        value = static_cast<uint32_t>(value >> 1);
    }

    return reversedValue;
}

uint32_t CRC32::Finalize(uint32_t remainder, uint32_t finalXOR)
{
    return (remainder ^ finalXOR) & BIT_MASK;
}

uint32_t CRC32::UndoFinalize(uint32_t crc, uint32_t finalXOR)
{
    crc = (crc & BIT_MASK) ^ finalXOR;
    return crc;
}

uint32_t CRC32::CalculateRemainder(const void* data, size_t size, uint32_t remainder)
{
    const unsigned char* current = reinterpret_cast<const unsigned char*>(data);
    while (size--)
    {
        remainder = static_cast<uint32_t>((remainder >> 8) ^ LookupTable[static_cast<unsigned char>(remainder ^ *current++)]);
    }
    return remainder;
}