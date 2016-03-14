#pragma once
#include <cstddef>
#include <vector>
#include <utility>
#include <random>
#include <string>

template <typename AnyType>
class CuckooHashFamily
{
    public:
        size_t hash(const AnyType & x, int which) const;
        int getNumberOfFunctions();
        void generateNewFunctions();
};

template<typename AnyType, typename HashFamily>
class CuckooHashTable
{
public:
public:
    explicit CuckooHashTable(std::size_t size = 101)
        : entries_(nextPrime(size))
    {
        numHashFunctions_ = hashFunctions_.getNumberOfFunctions();
        rehashes_ = 0;
        makeEmpty();
    }
    
    void makeEmpty()
    {
        currentSize = 0;
        for (auto & entry : entries_)
            entry.isActive = false;
    }

    bool contains(const AnyType & x) const
    {
        return findPos(x) != -1;
    }
    
    bool remove(const AnyType & x)
    {
        int currentPos = findPos(x);
        if (!isActive(currentPos))
            return false;
        
            entries_[currentPos].isActive = false;
        --currentSize;
        return true;
    }

    bool insert(const AnyType & x)
    {
        if (contains(x))
            return false;
        
        if (currentSize >= array.size() * MAX_LOAD)
            expand();
        
        return insertHelper1(x);
    }

    bool insert(AnyType && x)
    {
        if (contains(x))
            return false;

        if (currentSize >= array.size() * MAX_LOAD)
            expand();

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
        insertHelper1(std::move(x));
    }

    bool insertHelper1(AnyType && xx)
    {
        const int COUNT_LIMIT = 100;
        while (true)
        {
            int lastPos = -1;
            int pos;

            for (int count = 0; count < COUNT_LIMIT; ++count)
            {
                // Look for an available position using all hash functions
                for (int i = 0; i < numHashFunctions_; ++i)
                {
                    pos = myhash(xx, i);

                    if (!isActive(pos))
                    {
                        entries_[pos] = std::move(HashEntry{ std::move(xx), true });
                        ++currentSize;
                        return true;
                    }
                }

                // None of the spots is available. Try to evict a random one
                // (in one of the spots xx can go into)
                int i = 0;
                do
                {
                    pos = myhash(xx, r.nextInt(numHashFunctions));
                } while (pos == lastPos && i++ < 5);

                lastPos = pos;
                std::swap(std::move(xx), entries_[pos].element);
            }

            // Only rehash if we've tried below the number of allowed rehashes
            if (++rehashes > ALLOWED_REHASHES)
            {
                expand(); // Make the table bigger
                rehashes_ = 0; // Reset the # of rehashes
            }
            else
                rehash(); // Same table size, new hash functions
        }
    }
    bool isActive(int currentPos) const
    {
        return entries_[currentPos].isActive;
    }

    // Get the hash value for x using the specified hash function
    std::size_t myhash(const AnyType & x, int which) const
    {
        return hashFunctions_.hash(x, which) % entries_.size();
    }

    int findPos(const AnyType & x) const
    {
        for (int i = 0; i < numHashFunctions; ++i)
        {
            int pos = myhash(x, i);
            
            if (isActive(pos) && entries_[pos].element == x)
                return pos;
        }
        
        return -1;
    }

    void expand()
    {
        // Shouldn't this be currentSize_ / MAX_LOAD?
        // I guess it's more or less the same thing since that's when we'd be testing it
        rehash(static_cast<int>(entries_.size() / MAX_LOAD));
    }
    
    void rehash()
    {
        hashFunctions.generateNewFunctions();
        rehash(elements_.size());
    }

    void rehash(int newSize)
    {
        vector<HashEntry> oldArray = entries_;
        
        // Create new double-sized, empty table
        entries_.resize(nextPrime(newSize));
        for (auto & entry : entries_)
            entry.isActive = false;
       
       // Copy table over
       currentSize_ = 0;
       for (auto & entry : oldArray)
            if (entry.isActive)
                insert(std::move(entry.element));
    }

    static const double MAX_LOAD = 0.40;
    static const int ALLOWED_REHASHES = 5;
    vector<HashEntry> entries_;
    std::size_t currentSize_;
    std::size_t numHashFunctions_;
    std::size_t rehashes_;
    int random_;
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