// OrionGS/OrionGS/Utils.h
// Utility classes and functions for OrionGS

#pragma once

#include "CoreMinimal.h"

/**
 * Bit array container for efficient bit storage and manipulation
 */
template<typename Allocator = FDefaultAllocator>
class TBitArray
{
public:
    /**
     * Reference to a single bit within the bit array
     */
    struct FBitReference
    {
        // Non-const constructor for mutable bit reference
        FORCEINLINE FBitReference(uint32& InData, uint32 InMask)
            : Data(InData)
            , Mask(InMask)
        {
        }

        // Const constructor for const bit reference (line 372 - FIXED: removed 'const' before constructor name)
        FORCEINLINE FBitReference(const uint32& InData, const uint32 InMask)
            : Data(const_cast<uint32&>(InData))
            , Mask(InMask)
        {
        }

        FORCEINLINE operator bool() const
        {
            return (Data & Mask) != 0;
        }

        FORCEINLINE FBitReference& operator=(bool Value)
        {
            if (Value)
            {
                Data |= Mask;
            }
            else
            {
                Data &= ~Mask;
            }
            return *this;
        }

    private:
        uint32& Data;
        uint32 Mask;
    };

    /**
     * Iterator for traversing bits in the array
     */
    struct FBitIterator
    {
        // Constructor with start index (line 405 - FIXED: removed 'const' before constructor name)
        FORCEINLINE FBitIterator(const TBitArray& ToIterate, const int32 StartIndex)
            : Array(const_cast<TBitArray&>(ToIterate))
            , Index(StartIndex)
        {
        }

        // Default constructor (line 411 - FIXED: removed 'const' before constructor name)
        FORCEINLINE FBitIterator(const TBitArray& ToIterate)
            : Array(const_cast<TBitArray&>(ToIterate))
            , Index(0)
        {
        }

        FORCEINLINE FBitIterator& operator++()
        {
            ++Index;
            return *this;
        }

        FORCEINLINE bool operator!=(const FBitIterator& Other) const
        {
            return Index != Other.Index;
        }

        FORCEINLINE FBitReference operator*() const
        {
            return Array[Index];
        }

    private:
        TBitArray& Array;
        int32 Index;
    };

public:
    TBitArray()
        : NumBits(0)
        , NumWords(0)
    {
    }

    explicit TBitArray(int32 InNumBits)
    {
        Init(InNumBits);
    }

    void Init(int32 InNumBits)
    {
        NumBits = InNumBits;
        NumWords = (InNumBits + 31) / 32;
        Words.SetNumUninitialized(NumWords);
        FMemory::Memzero(Words.GetData(), NumWords * sizeof(uint32));
    }

    FORCEINLINE FBitReference operator[](int32 Index)
    {
        check(Index >= 0 && Index < NumBits);
        return FBitReference(Words[Index / 32], 1 << (Index % 32));
    }

    FORCEINLINE const FBitReference operator[](int32 Index) const
    {
        check(Index >= 0 && Index < NumBits);
        return FBitReference(Words[Index / 32], 1 << (Index % 32));
    }

    FORCEINLINE int32 Num() const
    {
        return NumBits;
    }

private:
    TArray<uint32, Allocator> Words;
    int32 NumBits;
    int32 NumWords;
};

/**
 * Utility functions for various operations
 */
namespace Utils
{
    FORCEINLINE bool IsPowerOfTwo(uint32 Value)
    {
        return Value && !(Value & (Value - 1));
    }

    FORCEINLINE uint32 Align(uint32 Value, uint32 Alignment)
    {
        check(IsPowerOfTwo(Alignment));
        return (Value + Alignment - 1) & ~(Alignment - 1);
    }

    FORCEINLINE int32 Clamp(int32 Value, int32 Min, int32 Max)
    {
        return Value < Min ? Min : (Value > Max ? Max : Value);
    }
}
