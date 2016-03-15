#include <iostream>
#include "CuckooHashTable.hpp"
#include "BinaryHeap.hpp"

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

void BinaryHeapTest()
{
    std::vector<int> ints{ 3, 6, 9, 12, 8, 11 };
    BinaryHeap<int> heap(ints);
    heap.insert(1);
    int sixteen = 16;
    heap.insert(sixteen);
    int size = heap.size();
    for (int i = 0; i < size; i++)
    {
        int elem = heap.findMin();
        heap.deleteMin();
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    std::cout << "Last inserted: " << heap.getLastInserted() << std::endl;
}

int main(void)
{    
    CuckooHashTest();
    BinaryHeapTest();
    std::cin.get();
    return 0;
}