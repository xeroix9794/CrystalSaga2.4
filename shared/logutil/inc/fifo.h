#ifndef _FIFO_H_
#define _FIFO_H_

struct fifo_elem {
	fifo_elem() {
		prev = next = NULL;
	}

	~fifo_elem() { 
		prev = next = NULL; 
	}

	fifo_elem* prev;
	fifo_elem* next;
};

template <class T>
class fifo {
public:
	fifo() {
		_head = _tail = NULL; 
	}
	
	~fifo() { 
		_head = _tail = NULL; 
	}

	// Push an element into the tail
	void push(T* t) {
		if (t == NULL)
			return;
		
		// Fifo is empty
		if (_head == NULL) {
			_head = t;
			_head->prev = NULL;
			_head->next = NULL;
			_tail = _head;
		}

		// Fifo is not empty
		else {
			_tail->next = t;
			t->prev = _tail;
			t->next = NULL;
			_tail = t;
		}
	}

	// Take an element from the head
	T* pop() {
		T* tmp;
		if (_head == _tail) {

			// Fifo is empty
			if (_head == NULL) {
				return NULL;
			}

			// Fifo the remaining element
			else {
				tmp = _head;
				_head = _tail = NULL;
				return tmp;
			}
		}

		// fifo remaining more than one element, remove one
		else {
			tmp = _head;
			_head = (T *)_head->next;
			_head->prev = NULL;
			return tmp;
		}
	}

	int size() const {

		// Get the length of fifo
		int sz = 0;
		T* curr = _head;
		
		// Fifo is empty
		if (curr == NULL) {
			return sz;
		}

		// fifo is not empty
		while (curr != _tail) {
			++sz; 
			curr = (T *)curr->next;
		}
		return (sz + 1);
	}

protected:
	T* _head;
	T* _tail;
};

#endif // _FIFO_H_
