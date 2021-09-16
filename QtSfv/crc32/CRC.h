/**
    @file CRC.h
    @author Daniel Bahr
    @version 1.1.0.0
    @copyright
    @parblock
        CRC++
        Copyright (c) 2021, Daniel Bahr
        All rights reserved.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions are met:

        * Redistributions of source code must retain the above copyright notice, this
          list of conditions and the following disclaimer.

        * Redistributions in binary form must reproduce the above copyright notice,
          this list of conditions and the following disclaimer in the documentation
          and/or other materials provided with the distribution.

        * Neither the name of CRC++ nor the names of its
          contributors may be used to endorse or promote products derived from
          this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
        AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
        IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
        DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
        FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
        DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
        SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
        CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
        OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    @endparblock
*/

#ifndef CRCPP_CRC_H_
#define CRCPP_CRC_H_

#include <climits>  // Includes CHAR_BIT

#include <stddef.h> // Includes size_t
#include <stdint.h> // Includes uint8_t, uint16_t, uint32_t, uint64_t

#include <limits>   // Includes ::std::numeric_limits
#include <utility>  // Includes ::std::move



/**
    @brief Static class for computing CRCs.
    @note This class supports computation of full and multi-part CRCs, using a bit-by-bit algorithm or a
        byte-by-byte lookup table. The CRCs are calculated using as many optimizations as is reasonable.
        If compiling with C++11, the constexpr keyword is used liberally so that many calculations are
        performed at compile-time instead of at runtime.
*/
class CRC
{
public:
    // Forward declaration
    template <typename CRCType, uint16_t CRCWidth>
    struct Table;

    /**
        @brief CRC parameters.
    */
    template <typename CRCType, uint16_t CRCWidth>
    struct Parameters
    {
        CRCType polynomial;   ///< CRC polynomial
        CRCType initialValue; ///< Initial CRC value
        CRCType finalXOR;     ///< Value to XOR with the final CRC
        bool reflectInput;    ///< true to reflect all input bytes
        bool reflectOutput;   ///< true to reflect the output CRC (reflection occurs before the final XOR)

        Table<CRCType, CRCWidth> MakeTable() const;
    };

    /**
        @brief CRC lookup table. After construction, the CRC parameters are fixed.
        @note A CRC table can be used for multiple CRC calculations.
    */
    template <typename CRCType, uint16_t CRCWidth>
    struct Table
    {
        // Constructors are intentionally NOT marked explicit.
        Table(const Parameters<CRCType, CRCWidth> & parameters);
        Table(Parameters<CRCType, CRCWidth> && parameters);

        const Parameters<CRCType, CRCWidth> & GetParameters() const;

        const CRCType * GetTable() const;

        CRCType operator[](unsigned char index) const;

    private:
        void InitTable();

        Parameters<CRCType, CRCWidth> parameters; ///< CRC parameters used to construct the table
        CRCType table[1 << CHAR_BIT];             ///< CRC lookup table
    };

    // The number of bits in CRCType must be at least as large as CRCWidth.
    // CRCType must be an unsigned integer type or a custom type with operator overloads.
    template <typename CRCType, uint16_t CRCWidth>
    static CRCType Calculate(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType Calculate(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters, CRCType crc);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType Calculate(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType Calculate(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable, CRCType crc);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType CalculateBits(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType CalculateBits(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters, CRCType crc);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType CalculateBits(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType CalculateBits(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable, CRCType crc);

    // Common CRCs up to 64 bits.
    // Note: Check values are the computed CRCs when given an ASCII input of "123456789" (without null terminator)

    static const Parameters<uint32_t, 32> & CRC_32();

    CRC() = delete;
    CRC(const CRC & other) = delete;
    CRC & operator=(const CRC & other) = delete;
    CRC(CRC && other) = delete;
    CRC & operator=(CRC && other) = delete;

private:
    template <typename IntegerType>
    static IntegerType Reflect(IntegerType value, uint16_t numBits);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType Finalize(CRCType remainder, CRCType finalXOR, bool reflectOutput);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType UndoFinalize(CRCType remainder, CRCType finalXOR, bool reflectOutput);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType CalculateRemainder(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters, CRCType remainder);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType CalculateRemainder(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable, CRCType remainder);

    template <typename CRCType, uint16_t CRCWidth>
    static CRCType CalculateRemainderBits(unsigned char byte, size_t numBits, const Parameters<CRCType, CRCWidth> & parameters, CRCType remainder);
};

/**
    @brief Returns a CRC lookup table construct using these CRC parameters.
    @note This function primarily exists to allow use of the auto keyword instead of instantiating
        a table directly, since template parameters are not inferred in constructors.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC lookup table
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRC::Table<CRCType, CRCWidth> CRC::Parameters<CRCType, CRCWidth>::MakeTable() const
{
    // This should take advantage of RVO and optimize out the copy.
    return CRC::Table<CRCType, CRCWidth>(*this);
}

/**
    @brief Constructs a CRC table from a set of CRC parameters
    @param[in] params CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRC::Table<CRCType, CRCWidth>::Table(const Parameters<CRCType, CRCWidth> & params) :
    parameters(params)
{
    InitTable();
}

/**
    @brief Constructs a CRC table from a set of CRC parameters
    @param[in] params CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRC::Table<CRCType, CRCWidth>::Table(Parameters<CRCType, CRCWidth> && params) :
    parameters(::std::move(params))
{
    InitTable();
}

/**
    @brief Gets the CRC parameters used to construct the CRC table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC parameters
*/
template <typename CRCType, uint16_t CRCWidth>
inline const CRC::Parameters<CRCType, CRCWidth> & CRC::Table<CRCType, CRCWidth>::GetParameters() const
{
    return parameters;
}

/**
    @brief Gets the CRC table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC table
*/
template <typename CRCType, uint16_t CRCWidth>
inline const CRCType * CRC::Table<CRCType, CRCWidth>::GetTable() const
{
    return table;
}

/**
    @brief Gets an entry in the CRC table
    @param[in] index Index into the CRC table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC table entry
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::Table<CRCType, CRCWidth>::operator[](unsigned char index) const
{
    return table[index];
}

/**
    @brief Initializes a CRC table.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline void CRC::Table<CRCType, CRCWidth>::InitTable()
{
    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BIT_MASK((CRCType(1) << (CRCWidth - CRCType(1))) |
                                           ((CRCType(1) << (CRCWidth - CRCType(1))) - CRCType(1)));

    // The conditional expression is used to avoid a -Wshift-count-overflow warning.
    static constexpr CRCType SHIFT((CHAR_BIT >= CRCWidth) ? static_cast<CRCType>(CHAR_BIT - CRCWidth) : 0);

    CRCType crc;
    unsigned char byte = 0;

    // Loop over each dividend (each possible number storable in an unsigned char)
    do
    {
        crc = CRC::CalculateRemainder<CRCType, CRCWidth>(&byte, sizeof(byte), parameters, CRCType(0));

        // This mask might not be necessary; all unit tests pass with this line commented out,
        // but that might just be a coincidence based on the CRC parameters used for testing.
        // In any case, this is harmless to leave in and only adds a single machine instruction per loop iteration.
        crc &= BIT_MASK;

        if (!parameters.reflectInput && CRCWidth < CHAR_BIT)
        {
            // Undo the special operation at the end of the CalculateRemainder()
            // function for non-reflected CRCs < CHAR_BIT.
            crc = static_cast<CRCType>(crc << SHIFT);
        }

        table[byte] = crc;
    }
    while (++byte);
}

/**
    @brief Computes a CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::Calculate(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters)
{
    CRCType remainder = CalculateRemainder(data, size, parameters, parameters.initialValue);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}
/**
    @brief Appends additional data to a previous CRC calculation.
    @note This function can be used to compute multi-part CRCs.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] crc CRC from a previous calculation
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::Calculate(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters, CRCType crc)
{
    CRCType remainder = UndoFinalize<CRCType, CRCWidth>(crc, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);

    remainder = CalculateRemainder(data, size, parameters, remainder);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Computes a CRC via a lookup table.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] lookupTable CRC lookup table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::Calculate(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable)
{
    const Parameters<CRCType, CRCWidth> & parameters = lookupTable.GetParameters();

    CRCType remainder = CalculateRemainder(data, size, lookupTable, parameters.initialValue);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Appends additional data to a previous CRC calculation using a lookup table.
    @note This function can be used to compute multi-part CRCs.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bytes
    @param[in] lookupTable CRC lookup table
    @param[in] crc CRC from a previous calculation
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::Calculate(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable, CRCType crc)
{
    const Parameters<CRCType, CRCWidth> & parameters = lookupTable.GetParameters();

    CRCType remainder = UndoFinalize<CRCType, CRCWidth>(crc, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);

    remainder = CalculateRemainder(data, size, lookupTable, remainder);

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Computes a CRC.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bits
    @param[in] parameters CRC parameters
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::CalculateBits(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters)
{
    CRCType remainder = parameters.initialValue;

    // Calculate the remainder on a whole number of bytes first, then call
    // a special-case function for the remaining bits.
    size_t wholeNumberOfBytes = size / CHAR_BIT;
    if (wholeNumberOfBytes > 0)
    {
        remainder = CalculateRemainder(data, wholeNumberOfBytes, parameters, remainder);
    }

    size_t remainingNumberOfBits = size % CHAR_BIT;
    if (remainingNumberOfBits != 0)
    {
        unsigned char lastByte = *(reinterpret_cast<const unsigned char *>(data) + wholeNumberOfBytes);
        remainder = CalculateRemainderBits(lastByte, remainingNumberOfBits, parameters, remainder);
    }

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}
/**
    @brief Appends additional data to a previous CRC calculation.
    @note This function can be used to compute multi-part CRCs.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bits
    @param[in] parameters CRC parameters
    @param[in] crc CRC from a previous calculation
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::CalculateBits(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters, CRCType crc)
{
    CRCType remainder = UndoFinalize<CRCType, CRCWidth>(crc, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);

    // Calculate the remainder on a whole number of bytes first, then call
    // a special-case function for the remaining bits.
    size_t wholeNumberOfBytes = size / CHAR_BIT;
    if (wholeNumberOfBytes > 0)
    {
        remainder = CalculateRemainder(data, wholeNumberOfBytes, parameters, parameters.initialValue);
    }

    size_t remainingNumberOfBits = size % CHAR_BIT;
    if (remainingNumberOfBits != 0)
    {
        unsigned char lastByte = *(reinterpret_cast<const unsigned char *>(data) + wholeNumberOfBytes);
        remainder = CalculateRemainderBits(lastByte, remainingNumberOfBits, parameters, remainder);
    }

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Computes a CRC via a lookup table.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bits
    @param[in] lookupTable CRC lookup table
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::CalculateBits(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable)
{
    const Parameters<CRCType, CRCWidth> & parameters = lookupTable.GetParameters();

    CRCType remainder = parameters.initialValue;

    // Calculate the remainder on a whole number of bytes first, then call
    // a special-case function for the remaining bits.
    size_t wholeNumberOfBytes = size / CHAR_BIT;
    if (wholeNumberOfBytes > 0)
    {
        remainder = CalculateRemainder(data, wholeNumberOfBytes, lookupTable, remainder);
    }

    size_t remainingNumberOfBits = size % CHAR_BIT;
    if (remainingNumberOfBits != 0)
    {
        unsigned char lastByte = *(reinterpret_cast<const unsigned char *>(data) + wholeNumberOfBytes);
        remainder = CalculateRemainderBits(lastByte, remainingNumberOfBits, parameters, remainder);
    }

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Appends additional data to a previous CRC calculation using a lookup table.
    @note This function can be used to compute multi-part CRCs.
    @param[in] data Data over which CRC will be computed
    @param[in] size Size of the data, in bits
    @param[in] lookupTable CRC lookup table
    @param[in] crc CRC from a previous calculation
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::CalculateBits(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable, CRCType crc)
{
    const Parameters<CRCType, CRCWidth> & parameters = lookupTable.GetParameters();

    CRCType remainder = UndoFinalize<CRCType, CRCWidth>(crc, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);

    // Calculate the remainder on a whole number of bytes first, then call
    // a special-case function for the remaining bits.
    size_t wholeNumberOfBytes = size / CHAR_BIT;
    if (wholeNumberOfBytes > 0)
    {
        remainder = CalculateRemainder(data, wholeNumberOfBytes, lookupTable, parameters.initialValue);
    }

    size_t remainingNumberOfBits = size % CHAR_BIT;
    if (remainingNumberOfBits > 0)
    {
        unsigned char lastByte = *(reinterpret_cast<const unsigned char *>(data) + wholeNumberOfBytes);
        remainder = CalculateRemainderBits(lastByte, remainingNumberOfBits, parameters, remainder);
    }

    // No need to mask the remainder here; the mask will be applied in the Finalize() function.

    return Finalize<CRCType, CRCWidth>(remainder, parameters.finalXOR, parameters.reflectInput != parameters.reflectOutput);
}

/**
    @brief Reflects (i.e. reverses the bits within) an integer value.
    @param[in] value Value to reflect
    @param[in] numBits Number of bits in the integer which will be reflected
    @tparam IntegerType Integer type of the value being reflected
    @return Reflected value
*/
template <typename IntegerType>
inline IntegerType CRC::Reflect(IntegerType value, uint16_t numBits)
{
    IntegerType reversedValue(0);

    for (uint16_t i = 0; i < numBits; ++i)
    {
        reversedValue = static_cast<IntegerType>((reversedValue << 1) | (value & 1));
        value = static_cast<IntegerType>(value >> 1);
    }

    return reversedValue;
}

/**
    @brief Computes the final reflection and XOR of a CRC remainder.
    @param[in] remainder CRC remainder to reflect and XOR
    @param[in] finalXOR Final value to XOR with the remainder
    @param[in] reflectOutput true to reflect each byte of the remainder before the XOR
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return Final CRC
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::Finalize(CRCType remainder, CRCType finalXOR, bool reflectOutput)
{
    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BIT_MASK = (CRCType(1) << (CRCWidth - CRCType(1))) |
                                             ((CRCType(1) << (CRCWidth - CRCType(1))) - CRCType(1));

    if (reflectOutput)
    {
        remainder = Reflect(remainder, CRCWidth);
    }

    return (remainder ^ finalXOR) & BIT_MASK;
}

/**
    @brief Undoes the process of computing the final reflection and XOR of a CRC remainder.
    @note This function allows for computation of multi-part CRCs
    @note Calling UndoFinalize() followed by Finalize() (or vice versa) will always return the original remainder value:

        CRCType x = ...;
        CRCType y = Finalize(x, finalXOR, reflectOutput);
        CRCType z = UndoFinalize(y, finalXOR, reflectOutput);
        assert(x == z);

    @param[in] crc Reflected and XORed CRC
    @param[in] finalXOR Final value XORed with the remainder
    @param[in] reflectOutput true if the remainder is to be reflected
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return Un-finalized CRC remainder
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::UndoFinalize(CRCType crc, CRCType finalXOR, bool reflectOutput)
{
    // For masking off the bits for the CRC (in the event that the number of bits in CRCType is larger than CRCWidth)
    static constexpr CRCType BIT_MASK = (CRCType(1) << (CRCWidth - CRCType(1))) |
                                             ((CRCType(1) << (CRCWidth - CRCType(1))) - CRCType(1));

    crc = (crc & BIT_MASK) ^ finalXOR;

    if (reflectOutput)
    {
        crc = Reflect(crc, CRCWidth);
    }

    return crc;
}

/**
    @brief Computes a CRC remainder.
    @param[in] data Data over which the remainder will be computed
    @param[in] size Size of the data, in bytes
    @param[in] parameters CRC parameters
    @param[in] remainder Running CRC remainder. Can be an initial value or the result of a previous CRC remainder calculation.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC remainder
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::CalculateRemainder(const void * data, size_t size, const Parameters<CRCType, CRCWidth> & parameters, CRCType remainder)
{
    // This static_assert is put here because this function will always be compiled in no matter what
    // the template parameters are and whether or not a table lookup or bit-by-bit algorithm is used.
    static_assert(std::numeric_limits<CRCType>::digits >= CRCWidth, "CRCType is too small to contain a CRC of width CRCWidth.");


    const unsigned char * current = reinterpret_cast<const unsigned char *>(data);

    // Slightly different implementations based on the parameters. The current implementations try to eliminate as much
    // computation from the inner loop (looping over each bit) as possible.
    if (parameters.reflectInput)
    {
        CRCType polynomial = CRC::Reflect(parameters.polynomial, CRCWidth);
        while (size--)
        {
            remainder = static_cast<CRCType>(remainder ^ *current++);

            // An optimizing compiler might choose to unroll this loop.
            for (size_t i = 0; i < CHAR_BIT; ++i)
            {
                remainder = static_cast<CRCType>((remainder & 1) ? ((remainder >> 1) ^ polynomial) : (remainder >> 1));
            }
        }
    }
    else if (CRCWidth >= CHAR_BIT)
    {
        static constexpr CRCType CRC_WIDTH_MINUS_ONE(CRCWidth - CRCType(1));
        static constexpr CRCType CRC_HIGHEST_BIT_MASK(CRCType(1) << CRC_WIDTH_MINUS_ONE);
        // The conditional expression is used to avoid a -Wshift-count-overflow warning.
        static constexpr CRCType SHIFT((CRCWidth >= CHAR_BIT) ? static_cast<CRCType>(CRCWidth - CHAR_BIT) : 0);

        while (size--)
        {
            remainder = static_cast<CRCType>(remainder ^ (static_cast<CRCType>(*current++) << SHIFT));

            // An optimizing compiler might choose to unroll this loop.
            for (size_t i = 0; i < CHAR_BIT; ++i)
            {
                remainder = static_cast<CRCType>((remainder & CRC_HIGHEST_BIT_MASK) ? ((remainder << 1) ^ parameters.polynomial) : (remainder << 1));
            }
        }
    }
    else
    {
        static constexpr CRCType CHAR_BIT_MINUS_ONE(CHAR_BIT - 1);
        static constexpr CRCType CHAR_BIT_HIGHEST_BIT_MASK(CRCType(1) << CHAR_BIT_MINUS_ONE);
        // The conditional expression is used to avoid a -Wshift-count-overflow warning.
        static constexpr CRCType SHIFT((CHAR_BIT >= CRCWidth) ? static_cast<CRCType>(CHAR_BIT - CRCWidth) : 0);

        CRCType polynomial = static_cast<CRCType>(parameters.polynomial << SHIFT);
        remainder = static_cast<CRCType>(remainder << SHIFT);

        while (size--)
        {
            remainder = static_cast<CRCType>(remainder ^ *current++);

            // An optimizing compiler might choose to unroll this loop.
            for (size_t i = 0; i < CHAR_BIT; ++i)
            {
                remainder = static_cast<CRCType>((remainder & CHAR_BIT_HIGHEST_BIT_MASK) ? ((remainder << 1) ^ polynomial) : (remainder << 1));
            }
        }

        remainder = static_cast<CRCType>(remainder >> SHIFT);
    }

    return remainder;
}

/**
    @brief Computes a CRC remainder using lookup table.
    @param[in] data Data over which the remainder will be computed
    @param[in] size Size of the data, in bytes
    @param[in] lookupTable CRC lookup table
    @param[in] remainder Running CRC remainder. Can be an initial value or the result of a previous CRC remainder calculation.
    @tparam CRCType Integer type for storing the CRC result
    @tparam CRCWidth Number of bits in the CRC
    @return CRC remainder
*/
template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::CalculateRemainder(const void * data, size_t size, const Table<CRCType, CRCWidth> & lookupTable, CRCType remainder)
{
    const unsigned char * current = reinterpret_cast<const unsigned char *>(data);

    if (lookupTable.GetParameters().reflectInput)
    {
        while (size--)
        {
#if defined(WIN32) || defined(_WIN32) || defined(WINCE)
    // Disable warning about data loss when doing (remainder >> CHAR_BIT) when
    // remainder is one byte long. The algorithm is still correct in this case,
    // though it's possible that one additional machine instruction will be executed.
#   pragma warning (push)
#   pragma warning (disable : 4333)
#endif
            remainder = static_cast<CRCType>((remainder >> CHAR_BIT) ^ lookupTable[static_cast<unsigned char>(remainder ^ *current++)]);
#if defined(WIN32) || defined(_WIN32) || defined(WINCE)
#   pragma warning (pop)
#endif
        }
    }
    else if (CRCWidth >= CHAR_BIT)
    {
        // The conditional expression is used to avoid a -Wshift-count-overflow warning.
        static constexpr CRCType SHIFT((CRCWidth >= CHAR_BIT) ? static_cast<CRCType>(CRCWidth - CHAR_BIT) : 0);

        while (size--)
        {
            remainder = static_cast<CRCType>((remainder << CHAR_BIT) ^ lookupTable[static_cast<unsigned char>((remainder >> SHIFT) ^ *current++)]);
        }
    }
    else
    {
        // The conditional expression is used to avoid a -Wshift-count-overflow warning.
        static constexpr CRCType SHIFT((CHAR_BIT >= CRCWidth) ? static_cast<CRCType>(CHAR_BIT - CRCWidth) : 0);

        remainder = static_cast<CRCType>(remainder << SHIFT);

        while (size--)
        {
            // Note: no need to mask here since remainder is guaranteed to fit in a single byte.
            remainder = lookupTable[static_cast<unsigned char>(remainder ^ *current++)];
        }

        remainder = static_cast<CRCType>(remainder >> SHIFT);
    }

    return remainder;
}

template <typename CRCType, uint16_t CRCWidth>
inline CRCType CRC::CalculateRemainderBits(unsigned char byte, size_t numBits, const Parameters<CRCType, CRCWidth> & parameters, CRCType remainder)
{
    // Slightly different implementations based on the parameters. The current implementations try to eliminate as much
    // computation from the inner loop (looping over each bit) as possible.
    if (parameters.reflectInput)
    {
        CRCType polynomial = CRC::Reflect(parameters.polynomial, CRCWidth);
        remainder = static_cast<CRCType>(remainder ^ byte);

        // An optimizing compiler might choose to unroll this loop.
        for (size_t i = 0; i < numBits; ++i)
        {
            remainder = static_cast<CRCType>((remainder & 1) ? ((remainder >> 1) ^ polynomial) : (remainder >> 1));
        }
    }
    else if (CRCWidth >= CHAR_BIT)
    {
        static constexpr CRCType CRC_WIDTH_MINUS_ONE(CRCWidth - CRCType(1));
        static constexpr CRCType CRC_HIGHEST_BIT_MASK(CRCType(1) << CRC_WIDTH_MINUS_ONE);
        // The conditional expression is used to avoid a -Wshift-count-overflow warning.
        static constexpr CRCType SHIFT((CRCWidth >= CHAR_BIT) ? static_cast<CRCType>(CRCWidth - CHAR_BIT) : 0);

        remainder = static_cast<CRCType>(remainder ^ (static_cast<CRCType>(byte) << SHIFT));

        // An optimizing compiler might choose to unroll this loop.
        for (size_t i = 0; i < numBits; ++i)
        {
            remainder = static_cast<CRCType>((remainder & CRC_HIGHEST_BIT_MASK) ? ((remainder << 1) ^ parameters.polynomial) : (remainder << 1));
        }
    }
    else
    {
        static constexpr CRCType CHAR_BIT_MINUS_ONE(CHAR_BIT - 1);
        static constexpr CRCType CHAR_BIT_HIGHEST_BIT_MASK(CRCType(1) << CHAR_BIT_MINUS_ONE);
        // The conditional expression is used to avoid a -Wshift-count-overflow warning.
        static constexpr CRCType SHIFT((CHAR_BIT >= CRCWidth) ? static_cast<CRCType>(CHAR_BIT - CRCWidth) : 0);

        CRCType polynomial = static_cast<CRCType>(parameters.polynomial << SHIFT);
        remainder = static_cast<CRCType>((remainder << SHIFT) ^ byte);

        // An optimizing compiler might choose to unroll this loop.
        for (size_t i = 0; i < numBits; ++i)
        {
            remainder = static_cast<CRCType>((remainder & CHAR_BIT_HIGHEST_BIT_MASK) ? ((remainder << 1) ^ polynomial) : (remainder << 1));
        }

        remainder = static_cast<CRCType>(remainder >> SHIFT);
    }

    return remainder;
}



/**
    @brief Returns a set of parameters for CRC-32 (aka CRC-32 ADCCP, CRC-32 PKZip).
    @note The parameters are static and are delayed-constructed to reduce memory footprint.
    @note CRC-32 has the following parameters and check value:
        - polynomial     = 0x04C11DB7
        - initial value  = 0xFFFFFFFF
        - final XOR      = 0xFFFFFFFF
        - reflect input  = true
        - reflect output = true
        - check value    = 0xCBF43926
    @return CRC-32 parameters
*/
inline const CRC::Parameters<uint32_t, 32> & CRC::CRC_32()
{
    static const Parameters<uint32_t, 32> parameters = { 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true };
    return parameters;
}

#endif // CRCPP_CRC_H_
