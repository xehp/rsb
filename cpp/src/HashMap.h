/*
 * HashMap.h
 *
 * http://www.algolist.net/Data_structures/Hash_table/Simple_example
 *
 * The example from above has been extended to allow the table to grow if needed.
 * Henrik Bjorkman www.eit.se/hb 2015-08-29
 *
 * Some cleaning, made a template class for storing pointers, added comments.
 * Henrik 2015-10-15
 */

// This is A hash table that only supports adding to the table not removals.


#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <cstddef>


class HashEntry_64_32 {
private:
	long long key;
	int value;
public:
	HashEntry_64_32(long long key, int value) {
		this->key = key;
		this->value = value;
	}

	long long getKey() const {
		return key;
	}

	int getValue() const {
		return value;
	}
private:
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
	HashEntry_64_32();
	HashEntry_64_32(const HashEntry_64_32&);
	HashEntry_64_32& operator=(const HashEntry_64_32&);
};

const int TABLE_SIZE = 32;

// A HashMap using 64 bit keys, storing 32 bit values .
class HashMap_64_32 {
private:
	HashEntry_64_32 **table;
	int capacity_;
	int size_;

public:
	HashMap_64_32();
	~HashMap_64_32();

	int size() const {return size_;};
	int capacity() const {return capacity_;};
	int calculateHash(long long key, int capacity) const {return key >= 0 ? (key % capacity) : (-key % capacity);};
	int nextHash(int hash, int capacity) const {return (hash + 1) % capacity;};

	// This is used when iterating (well looping) the hash map. index is an internal index in the map not same thing as key.
	// It returns -1 if there was no value stored at that location, so this hash map can only be used to store values that are not -1.
	const int getValueByIndex(int index) const {const HashEntry_64_32 *ptr=table[index]; return (ptr==NULL) ? -1 : table[index]->getValue();};

	// Get value for given key
	// It returns -1 if there was no value for the given key.
	int get(long long key) const {
		int hash = calculateHash(key, capacity_);
		while ((table[hash] != NULL) && (table[hash]->getKey() != key))
			hash = nextHash(hash, capacity_);
		if (table[hash] == NULL)
			return -1;
		else
			return table[hash]->getValue();
	}

	// Store a key value pair in the hash map, values should only be >=0 since value -1 is used to tell if there was no value. The key can be negative. If negative values are needed perhaps use the HashMap_64_PTR template class below.
	void put(long long key, int value);

private:
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
	HashMap_64_32(const HashMap_64_32&);
	HashMap_64_32& operator=(const HashMap_64_32&);

};


template <class T>
class HashEntry_64_PTR {
private:
	long long key;
	T* value;
public:
	HashEntry_64_PTR(long long key, T* value) {
		this->key = key;
		this->value = value;
	}

	long long getKey() const {
		return key;
	}

	T* getValue() const {
		return value;
	}

private:
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
	HashEntry_64_PTR();
	HashEntry_64_PTR(const HashEntry_64_PTR&);
	HashEntry_64_PTR& operator=(const HashEntry_64_PTR&);

};


// A HashMap using 64 bit keys, storing 32 bit values .
template <class T>
class HashMap_64_PTR {
private:
	HashEntry_64_PTR<T> **table;
	int capacity_;
	int size_;

public:
	HashMap_64_PTR(){
		capacity_=TABLE_SIZE;
		table = new HashEntry_64_PTR<T>*[capacity_];
		for (int i = 0; i < capacity_; i++)
		{
			table[i] = NULL;
		}
		size_=0;
	};

	~HashMap_64_PTR(){
		// Delete the hash entries stored in the table
		// Note we do not delete the object that value is pointing at since its not known here if that is to be done or not, those objects where not created by HashMap_64_PTR so its not the responsibility of this class.
		for (int i = 0; i < capacity_; i++)
		{
			if (table[i] != NULL)
			{
				delete table[i];
			}
		}
		// Delete the table.
		delete[] table;
	};

	int size() const {return size_;};
	int capacity() const {return capacity_;};
	int calculateHash(long long key, int capacity) const {return key >= 0 ? (key % capacity) : (-key % capacity);};
	int nextHash(int hash, int capacity) const {return (hash + 1) % capacity;};

	// This is used when iterating (well looping) the hash map. index is an internal index in the map not same thing as key.
	T* getValueByIndex(int index) const {return (table[index] == NULL) ?  NULL : table[index]->getValue();};


	// Get value for given key
	T* get(long long key) const {
		int hash = calculateHash(key, capacity_);
		while ((table[hash] != NULL) && (table[hash]->getKey() != key)) {
			hash = nextHash(hash, capacity_);
		}
		return (table[hash] == NULL) ?  NULL : table[hash]->getValue();
	}

	// Add a key value pair to the hash map
	void put(long long key, T* value) {
		if ((size_*2)>capacity_)
		{
			// more than half full, make a bigger table.
			const int newCapacity=capacity_*2;
			HashEntry_64_PTR<T> **newTable = new HashEntry_64_PTR<T>*[newCapacity];
			for (int i = 0; i < newCapacity; i++)
			{
				newTable[i] = NULL;
			}

			// Move all entries over to the new table
			for (int i = 0; i < capacity_; i++)
			{
				if (table[i] != NULL)
				{
					const long long keyToMove=table[i]->getKey();
					T* valueToMove = table[i]->getValue();
					int newHash = calculateHash(keyToMove, newCapacity);
					while ((newTable[newHash] != NULL) && (newTable[newHash]->getKey() != keyToMove))
					{
						// This slot/place/position was taken by some other value. Will try next slot.
						//printf("HashMap: not on first choise %ll %d  %d %d", keyToMove, valueToMove, newHash, newCapacity);
						newHash = nextHash(newHash, newCapacity);
					}
					if (newTable[newHash] != NULL)
					{
						delete newTable[newHash];
					}
					newTable[newHash] = new HashEntry_64_PTR<T>(keyToMove, valueToMove);
				}
			}

			// There will now be space in the table

			// Delete the old table and its content.
			for (int i = 0; i < capacity_; i++)
			{
				if (table[i] != NULL)
				{
					delete table[i];
				}
			}
			delete[] table;

			table = newTable;
			capacity_ = newCapacity;
		}

		int hash = calculateHash(key, capacity_);
		while ((table[hash] != NULL) && (table[hash]->getKey() != key))
		{
			// This slot/place/position was taken by some other value. Will try next slot.
			//printf("HashMap: not on first choice %ll %d  %d %d %d", key, value, hash, capacity_, size_);
			hash = nextHash(hash, capacity_);
		}
		if (table[hash] != NULL)
		{
			delete table[hash];
		}
		table[hash] = new HashEntry_64_PTR<T>(key, value);

		size_++;
	};

private:
	// Prevent unintentional use of copy-constructor and assignment operator by making these private.
	HashMap_64_PTR(const HashMap_64_PTR&);
	HashMap_64_PTR& operator=(const HashMap_64_PTR&);

};

#endif /* HASHMAP_H_ */
