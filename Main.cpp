#include <iostream>
#include "CuckooHashTable.hpp"

struct SimpleStruct
{
    std::uint32_t x;
    std::uint32_t test;
    double test2;

    bool operator==(const SimpleStruct& rhs) const
    {
        return x == rhs.x && test == rhs.test && test2 == rhs.test2;
    }
};

void CuckooHashTest()
{
    CuckooHashTable<SimpleStruct, CuckooHashFamily<SimpleStruct>> hashTable(5);
    SimpleStruct same1{ 32, 64, 1.5 };
    SimpleStruct same2{ 32, 64, 1.5 };

    hashTable.insert(same1);
    std::cout << "Contains same2? " << hashTable.contains(same2) << std::endl;

    hashTable.remove(same1);
    std::cout << "Contains same1? " << hashTable.contains(same1) << std::endl;

    for (int i = 0; i < 6; i++)
    {
        SimpleStruct simple = { i, i * i, 5.0 / (i + 1) };
        hashTable.insert(simple);
    }
}

int main(void)
{    
    CuckooHashTest();
    std::cin.get();
    return 0;
}