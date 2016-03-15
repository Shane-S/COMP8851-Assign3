#include <vector>
using std::vector;

template <typename Comparable>
class BinaryHeap
{
public:
    explicit BinaryHeap(int capacity = 100)
        : _array()
    {
        _array.reserve(capacity);
    }

	explicit BinaryHeap(const vector<Comparable> & items)
		: _array(items.size() + 10), _currentSize{ (int)items.size() }
	{
		for (size_t i = 0; i < items.size(); ++i)
			_array[i + 1] = items[i];
		buildHeap();
	}

    bool isEmpty() const
    {
        return _currentSize == 0;
    }

    const Comparable & findMin() const
    {
        return _array[1];
    }

	void insert(const Comparable & x)
	{
        if (_currentSize == _array.size() - 1)
            _array.resize(_array.size() * 2);

        // Percolate up
        int hole = ++_currentSize;

        for (; x < _array[hole / 2]; hole /= 2)
            _array[hole] = std::move(_array[hole / 2]);
        
        _array[0] = _array[hole] = x;
	}

    void insert(Comparable && x)
    {
        if (_currentSize == _array.size() - 1)
            _array.resize(_array.size() * 2);

        // Percolate up
        int hole = ++_currentSize;

        _array[0] = std::move(x);
        for (; x < _array[hole / 2]; hole /= 2)
            _array[hole] = std::move(_array[hole / 2]);
        _array[hole] = _array[0];
    }

	void deleteMin()
	{
		if (isEmpty())
            throw std::underflow_error{ "Can't delete min from empty heap!" };

		_array[1] = std::move(_array[_currentSize--]);
		percolateDown(1);
	}
	void deleteMin(Comparable & minItem)
	{
		if (isEmpty())
			throw UnderflowException{};

		minItem = std::move(_array[1]);
		_array[1] = std::move(_array[currentSize--]);
		percolateDown(1);
	}

    Comparable getLastInserted() const
    {
        return _array[0];
    }

    void makeEmpty()
    {
        _array.clear();
        _currentSize = 0;
    }

    int size() const
    {
        return _currentSize;
    }

private:
	int _currentSize; // Number of elements in heap
	vector<Comparable> _array; // The heap array

	void buildHeap()
	{
		for (int i = _currentSize / 2; i > 0; --i)
			percolateDown(i);
	}

	void percolateDown(int hole)
	{
		int child;
		Comparable tmp = std::move(_array[hole]);

		for (; hole * 2 <= _currentSize; hole = child)
		{
			child = hole * 2;
			if (child != _currentSize && _array[child + 1] < _array[child])
				++child;
			if (_array[child] < tmp)
				_array[hole] = std::move(_array[child]);
			else
				break;
		}
		_array[hole] = std::move(tmp);
	}
};