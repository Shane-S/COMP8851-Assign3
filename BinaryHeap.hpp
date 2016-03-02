template <typename Comparable>

class BinaryHeap
{
public:
	explicit BinaryHeap(int capacity = 100);
	explicit BinaryHeap(const vector<Comparable> & items)
		: _array(items.size() + 10), currentSize{ items.size() }
	{
		for (int i = 0; i < items.size(); ++i)
			_array[i + 1] = items[i];
		buildHeap();
	}

	bool isEmpty() const;
	const Comparable & findMin() const;

	void insert(const Comparable & x)
	{
		if (currentSize == array.size() - 1)
			array.resize(array.size() * 2);

		// Percolate up
		int hole = ++currentSize;
		Comparable copy = x;

		_array[0] = std::move(copy);
		for (; x < _array[hole / 2]; hole /= 2)
			_array[hole] = std::move(_array[hole / 2]);
		_array[hole] = std::move(_array[0]);

	}
	void insert(Comparable && x);
	void deleteMin()
	{
		if (isEmpty())
			throw UnderflowException{};

		_array[1] = std::move(_array[currentSize--]);
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
	void makeEmpty();

private:
	int _currentSize; // Number of elements in heap
	vector<Comparable> _array; // The heap array

	void buildHeap()
	{
		for (int i = currentSize / 2; i > 0; --i)
			percolateDown(i);
	}

	void percolateDown(int hole)
	{
		int child;
		Comparable tmp = std::move(_array[hole]);

		for (; hole * 2 <= currentSize; hole = child)
		{
			child = hole * 2;
			if (child != currentSize && _array[child + 1] < _array[child])
				++child;
			if (_array[child] < tmp)
				_array[hole] = std::move(_array[child]);
			else
				break;
		}
		_array[hole] = std::move(tmp);
	}
};