#pragma once
#include <cstddef>
#include <vector>
#include <utility>
#include <random>
#include <string>
#include <cstdint>
#include "MurmurHash2.h"

#define MAX_LOAD 0.50

// Generic hash function that hashes the contents of x (treating x as an array of bytes)
// Most classes should probably specialise this
template <typename AnyType>
class CuckooHashFamily
{
    public:
        CuckooHashFamily()
        {
            // Could loop infinitely, but probably won't
            do
            {
                seed1_ = rand();
            } while (seed1_ == 0);
                
            do
            {
                seed2_ = rand();
            } while (seed2_ == seed1_ || seed2_ == 0);
        }
        
        // MurmurHash2 (See https://sites.google.com/site/murmurhash/, MurmurHash2_64.cpp)
        std::uint64_t hash(const AnyType & x, int which) const
        {
            const std::uint64_t len = sizeof(x); // Assume 8-bit bytes
            std::uint64_t seed = which == 0 ? seed1_ : seed2_;
            return MurmurHash64((const void*)&x, len, seed);
        }

        void regenerate()
        {
            std::uint64_t prev_seed1 = seed1_;
            std::uint64_t prev_seed2 = seed2_;

            // Could loop infinitely, but probably won't
            do
            {
                seed1_ = rand();
            } while (seed1_ == 0 || seed1_ == prev_seed1);

            do
            {
                seed2_ = rand();
            } while (seed2_ == seed1_ || seed2_ == 0 || seed2_ == prev_seed2);
        }

private:
    std::uint64_t seed1_;
    std::uint64_t seed2_;
};

template<typename AnyType, typename HashFamily = CuckooHashFamily<AnyType>>
class CuckooHashTable
{
public:
    explicit CuckooHashTable(std::size_t size = 101)
        : tableSize_(nextPrime(size)),
          currentSize_(0),
          rehashes_(0),
          entries_(tableSize_ * 2)
    {
        makeEmpty();
    }
    
    void makeEmpty()
    {
        currentSize_ = 0;
        for (auto & entry : entries_)
            entry.isActive = false;
    }

    bool contains(const AnyType & x) const
    {
        return findPos(x) != -1;
    }
    
    // Pre-condition: x is actually in the table
    bool remove(const AnyType & x)
    {
        int currentPos = findPos(x);
        if (!isActive(currentPos))
            return false;
        
        entries_[currentPos].isActive = false;
        --currentSize_;
        return true;
    }

    bool insert(const AnyType & x)
    {
        if (contains(x))
            return false;
        
        if (currentSize_ >= entries_.size() * MAX_LOAD)
            expand();
        
        currentSize_++;
        return insertHelper1(x);
    }

    bool insert(AnyType && x)
    {
        if (contains(x))
            return false;

        if (currentSize_ >= entries_.size() * MAX_LOAD)
            expand();

        currentSize_++;
        return insertHelper1(x);
    }

private:
    struct HashEntry
    {
        AnyType element;
        bool isActive;
  
        HashEntry(const AnyType & e = AnyType(), bool a = false)
            : element{ e }, isActive{ a } { }
        
        HashEntry(AnyType && e, bool a = false)
            : element{ std::move(e) }, isActive{ a } { }
    };

    bool insertHelper1(const AnyType & xx)
    {
        AnyType x = xx;
        return insertHelper1(std::move(x));
    }

    bool insertHelper1(AnyType && xx)
    {
        const int EVICTION_LIMIT = 30; // Number of times we'll try to insert something before rehashing or expanding

        AnyType curElem(std::move(xx));
        int curHash = 0;
        int curTable = 0;
        int insertPos = 0;
        int evictions = 0;
        bool evicted;

        do
        {
            AnyType toInsert = std::move(curElem);
            insertPos = (int)myhash(toInsert, (int)curHash);
            evicted = false;

            if (isActive(insertPos))
            {
                curHash = curHash == 0 ? 1 : 0;
                curTable = curTable == 0 ? 1 : 0;
                curElem = std::move(entries_[insertPos].element);
                evictions++;
                evicted = true;
            }
            entries_[insertPos].element = std::move(toInsert);
            entries_[insertPos].isActive = true;
        } while (evicted && evictions < EVICTION_LIMIT);

        if (evictions == EVICTION_LIMIT)
        {
            if (++rehashes_ < ALLOWED_REHASHES)
            {
                rehash();
            }
            else
            {
                expand();
                rehashes_ = 0;
            }

            // Pretty bad luck if we end up in infinite recursion
            insertHelper1(std::move(curElem));
            return false;
        }
        return true;
    }

    bool isActive(int currentPos) const
    {
        return entries_[currentPos].isActive;
    }

    int findPos(const AnyType & x) const
    {
        std::uint64_t pos0 = myhash(x, 0);
        std::uint64_t pos1 = myhash(x, 1);

        if (isActive((int)pos0) && entries_[pos0].element == x)      return (int)pos0;
        else if (isActive((int)pos1) && entries_[pos1].element == x) return (int)pos1;
        else                                                         return -1;
    }

    void expand()
    {
        rehash(static_cast<int>(tableSize_ / MAX_LOAD));
    }
    
    std::uint64_t myhash(const AnyType& x, int which) const
    {
        if (which == 0)
        {
            return hashFunctions_.hash(x, 0) % tableSize_;
        }
        else
        {
            return hashFunctions_.hash(x, 1) % tableSize_ + tableSize_;
        }
    }

    void rehash()
    {
        hashFunctions_.regenerate();
        rehash(entries_.size());
    }

    void rehash(std::size_t newSize)
    {
        std::vector<HashEntry> oldArray = entries_;
        tableSize_ = nextPrime(newSize);

        entries_.resize(tableSize_ * 2);
        for (auto & entry : entries_)
            entry.isActive = false;
       
        // Copy table over
        currentSize_ = 0;
        for (auto & entry : oldArray)
            if (entry.isActive)
                insert(std::move(entry.element));
    }

    static const int ALLOWED_REHASHES = 2;
    std::size_t tableSize_;
    std::vector<HashEntry> entries_;
    std::size_t currentSize_;
    std::size_t numHashFunctions_;
    std::size_t rehashes_;
    HashFamily hashFunctions_;

    // See http://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime#answer-5694432
    int isPrime(std::size_t x) {
        std::size_t i;

        for (i = 3; 1; i += 2)
        {
            std::size_t q = x / i;
            if (q < i)
                return 1;
            if (x % i == 0)
                return 0;
        }
        return 1;
    }

    std::size_t nextPrime(std::size_t x) {

        if (x <= 2)
            return 2;

        // Start at an odd number
        if (!(x & 1))
            ++x;

        for (; !isPrime(x); x += 2)
            ;
        return x;
    }
};