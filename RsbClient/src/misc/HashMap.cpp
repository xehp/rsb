/*
 * HashMap.h
 *
 * http://www.algolist.net/Data_structures/Hash_table/Simple_example
 *
 *
 * www.eit.se/rsb
 * Copyright (C) 2016 Henrik Björkman (www.eit.se/hb)
 * License: www.eit.se/rsb/license
 *
 *
 * The example from above has been modified to allow the table to grow if needed.
 * Henrik Bjorkman 2015-08-29
 */

// This is A hash table that only supports adding to the table not removals.

#include <assert.h>
#include "HashMap.h"


HashMap_64_32::HashMap_64_32() {
	capacity_=TABLE_SIZE;
	table = new HashEntry_64_32*[capacity_];
	for (int i = 0; i < capacity_; i++)
	{
		table[i] = NULL;
	}
	size_=0;
}

HashMap_64_32::~HashMap_64_32()
{
	// Delete the hash entries stored in the table
	for (int i = 0; i < capacity_; i++)
	{
		if (table[i] != NULL)
		{
			delete table[i];
		}
	}
	// Delete the table.
	delete[] table;
}


// Store a key value pair in the hash map
void HashMap_64_32::put(long long key, int value)
{
	assert(value!=-1); // -1 is used for empty slots.
	if ((size_*2)>=capacity_)
	{
		// more than half full, make a bigger table.
		const int newCapacity=capacity_*2;
		HashEntry_64_32 **newTable = new HashEntry_64_32*[newCapacity];
		for (int i = 0; i < newCapacity; i++)
		{
			newTable[i] = NULL;
		}

		// Move all entries over to the new table
		for (int i = 0; i < capacity_; i++)
		{
			HashEntry_64_32* entry=table[i];
			if (entry != NULL)
			{
				const long long keyToMove=table[i]->getKey();
				const int valueToMove=table[i]->getValue();
				int newHash = calculateHash(keyToMove, newCapacity);
				while (newTable[newHash] != NULL)
				{
					assert(!newTable[newHash]->isSameKey(entry->getKey()));
					// This slot/place/position was taken by some other value. Will try next slot.
					//printf("HashMap: not on first choise %ll %d  %d %d", keyToMove, valueToMove, newHash, newCapacity);
					newHash = (newHash + 1) % newCapacity;
				}
				if (newTable[newHash] != NULL)
				{
					delete newTable[newHash];
				}
				newTable[newHash] = new HashEntry_64_32(keyToMove, valueToMove);
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
	table[hash] = new HashEntry_64_32(key, value);

	size_++;
}





// Remove a key (and the pointer) from the hash map
void HashMap_64_32::remove(long long key) {
	int newCapacity=capacity_;
	if (((size_*2)<capacity_) && (capacity_>=4))
	{
		newCapacity=capacity_/2;
	}

	HashEntry_64_32 **newTable = new HashEntry_64_32*[newCapacity];

	// Initialize the new table to NULL.
	for (int i = 0; i < newCapacity; i++)
	{
		newTable[i] = NULL;
	}

	// Move all entries except the one to remove over to the new table
	for (int i = 0; i < capacity_; i++)
	{
		HashEntry_64_32* entry=table[i];
		if (entry != NULL)
		{

			if (entry->isSameKey(key))
			{
				delete entry;
			}
			else
			{

				int newHash = calculateHash(entry->getKey(), newCapacity);

				// Find a free slot in the new table
				while (newTable[newHash] != NULL)
				{
					assert(!newTable[newHash]->isSameKey(entry->getKey()));
					// This slot/place/position was taken by some other value. Will try next slot.
					//printf("HashMap: not on first choise %ll %d  %d %d", keyToMove, valueToMove, newHash, newCapacity);
					newHash = nextHash(newHash, newCapacity);
				}
				if (newTable[newHash] != NULL)
				{
					delete newTable[newHash];
				}
				newTable[newHash] = entry;
			}
		}
	}

	// Delete the old table but not its content since that has been moved (not copied) to the new table.
	/*for (int i = 0; i < capacity_; i++)
	{
		if (table[i] != NULL)
		{
			delete table[i];
		}
	}*/
	delete[] table;

	table = newTable;
	capacity_ = newCapacity;


	size_--;
};
