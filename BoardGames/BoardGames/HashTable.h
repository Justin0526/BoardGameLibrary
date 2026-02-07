/*********************************************************************************
 * Group         : T01
 * Team Member   : Justin Tang Jia Ze (S10269496B)
 *
 * File Purpose:
 * - Generic HashTable<K, V> implemented using C++ templates.
 * - Provides fast average-case lookup, insertion, and update by key.
 *
 * Key Design Notes:
 * - Uses separate chaining (linked list per bucket) to handle collisions.
 * - In this project, V is often a List<T>::NodePtr, allowing the HashTable to
 *   store a direct reference to a node inside LinkedList for quick retrieval.
 *   (Caller can then use List::getItem(NodePtr) to access/modify the actual item.)
 *
 * Constraints / Assumptions:
 * - MAX_SIZE is fixed (101). Performance depends on load factor and key distribution.
 * - This hash() implementation assumes K is string-like and iterable by character.
 *********************************************************************************/

#pragma once
#include <string>
#include <optional>
#include <functional>
#include <stdexcept>

const int MAX_SIZE = 101;

// Hash table node for separate chaining (linked list inside each bucket).
// Stores a (key, value) pair and a pointer to the next node in the same bucket.
template<typename K, typename V>
struct Node {
	K key;
	V value;
	Node<K, V>* next;
};

// HashTable<K, V>:
// Fixed-size hash table using separate chaining.
// Designed to support fast key-based lookup for project entities (e.g., members, games).
template<typename K, typename V>
class HashTable {
	private:
		Node<K, V>* items[MAX_SIZE];
		int size;

	public:
		HashTable();
		~HashTable();
		int hash(const K& key);
		bool add(const K& key, const V& value);
		bool addOrUpdate(const K& key, const V& value);
		void remove(const K& key);
		V get(const K& key);
		bool isEmpty();
		int getLength();
		bool containsKey(const K& key);

		// forEach allows the caller to run a function (lambda) on every (key, value)
		// pair stored inside the hash table.
		template<typename Func>
		void forEach(Func fn);
};


/*********************************************************************************
 * Function  : HashTable<K, V>::HashTable
 * Purpose   : Initializes an empty hash table (all buckets set to nullptr).
 *
 * Time Complexity:
 * - O(MAX_SIZE)
 *********************************************************************************/
template <typename K, typename V>
HashTable<K, V>::HashTable()
{ 
	size = 0;
	for (int i = 0; i < MAX_SIZE; i++) {
		items[i] = nullptr;
	}

};

/*********************************************************************************
 * Function  : HashTable<K, V>::~HashTable
 * Purpose   : Frees all nodes allocated in the hash table buckets.
 *
 * Time Complexity:
 * - O(MAX_SIZE + N) where N is total nodes stored in the table.
 *********************************************************************************/
template <typename K, typename V>
HashTable<K, V>::~HashTable() {
	for (int i = 0; i < MAX_SIZE; i++) {
		Node<K, V>* current = items[i];

		while (current != nullptr) {
			Node<K, V>* temp = current;
			current = current->next;
			delete temp;
		}
	}
}

/*********************************************************************************
 * Function  : HashTable<K, V>::hash
 * Purpose   : Computes a bucket index for a key.
 *
 * Notes     :
 * - Uses a rolling hash (multiplier 131) and reduces into [0, MAX_SIZE].
 * - Assumes K behaves like a string (iterable characters).
 *
 * Returns   :
 * - int : bucket index in range [0, MAX_SIZE-1]
 *********************************************************************************/
template <typename K, typename V>
int HashTable<K, V>::hash(const K& key) {
	// h is the "hash value" we are building
	size_t h = 0;

	// Loop through the key character by character
	for (unsigned char c : key) {
		// Multiply the current hash value by 131
		// then add the numeric value of the character
		h = h * 131 + c;
	}

	// Force hash value to be a valid index range for the table
	return static_cast<int>(h % MAX_SIZE);
}

/*********************************************************************************
 * Function  : HashTable<K, V>::add
 * Purpose   : Inserts a new (key, value) pair into the hash table.
 *             Collisions are handled via separate chaining.
 *
 * Notes     :
 * - This function does NOT check for duplicate keys.
 *
 * Returns   :
 * - bool : true when insertion succeeds.
 *
 * Time Complexity:
 * - Average: O(1)
 * - Worst-case: O(n) if many keys collide in the same bucket
 *********************************************************************************/
template <typename K, typename V>
bool HashTable<K, V>::add(const K& newKey, const V& newValue) {
	int index = hash(newKey);
	Node<K, V>* current = items[index];

	Node<K, V>* newNode = new Node <K, V>;
	newNode->key = newKey;
	newNode->value = newValue;
	newNode->next = nullptr;
	
	if (current == nullptr) {
		items[index] = newNode;
	}
	else {
		while (current->next != nullptr) {
			current = current->next;
		}

		current->next = newNode;
	}
	size++;
	return true;
}

/*********************************************************************************
 * Function  : HashTable<K, V>::addOrUpdate
 * Purpose   : Inserts a (key, value) pair if the key does not exist,
 *             otherwise updates the existing value.
 *
 * Project Usage:
 * - Used to maintain the latest score/value for keys such as recommendation
 *   candidate scores.
 *
 * Returns   :
 * - bool : true when operation succeeds.
 *
 * Time Complexity:
 * - Average: O(1)
 * - Worst-case: O(n) due to bucket traversal
 *********************************************************************************/
template <typename K, typename V>
bool HashTable<K, V>::addOrUpdate(const K& key, const V& value) {

	int index = hash(key);
	Node<K, V>* current = items[index];

	Node<K, V>* newNode = new Node <K, V>;
	newNode->key = key;
	newNode->value = value;
	newNode->next = nullptr;

	if (current == nullptr) {
		items[index] = newNode;
		size++;
	}
	else {
		while (current != nullptr) {
			if (current->key == key) {
				current->value = value;
				return true;
			}
			if (current->next == nullptr)
				break;
			current = current->next;
		}
		current->next = newNode;
		size++;
	}	
	return true;
}

/*********************************************************************************
 * Function  : HashTable<K, V>::remove
 * Purpose   : Removes a key from the hash table (if present) by unlinking and
 *             deleting the hash node in its bucket chain.
 *
 * Notes     :
 * - This deletes ONLY the HashTable node. If V points to external memory
 *   (e.g., a LinkedList node pointer), ownership/deletion of that memory is
 *   handled elsewhere.
 *
 * Time Complexity:
 * - Average: O(1)
 * - Worst-case: O(n)
 *********************************************************************************/
template <typename K, typename V>
void HashTable<K, V>::remove(const K& key) {
	int index = hash(key);
	Node<K, V>* current = items[index];
	Node<K, V>* prev = nullptr;

	while (current != nullptr && current->key != key) {
		prev = current;
		current = current->next;
	}

	// Not found
	if (current == nullptr) return;

	// Unlink
	if (prev == nullptr) {
		items[index] = current->next;     // removing head
	}
	else {
		prev->next = current->next;       // removing middle/last
	}

	delete current;   // delete HASH TABLE node only
	size--;
}

/*********************************************************************************
 * Function  : HashTable<K, V>::get
 * Purpose   : Retrieves the value associated with a key.
 *
 * Returns   :
 * - V : The stored value (copied / returned by value).
 *
 * Error Handling:
 * - Throws runtime_error if the key does not exist.
 *
 * Time Complexity:
 * - Average: O(1)
 * - Worst-case: O(n)
 *********************************************************************************/
template <typename K, typename V>
V HashTable<K, V>::get(const K& key) {
	int index = hash(key);
	Node<K, V>* current = items[index];
	while (current != nullptr) {
		if (current->key == key) {
			return current->value;
		}
		current = current->next;
	}
	
	throw std::runtime_error("Key not found in HashTable");
}

// Returns true if no items are stored in the hash table.
// Time Complexity: O(1)
template <typename K, typename V>
bool HashTable<K, V>::isEmpty() {
	return size == 0;
}

// Returns the number of key-value pairs stored in the hash table.
// Time Complexity: O(1)
template <typename K, typename V>
int HashTable<K, V>::getLength() {
	return size;
}

/*********************************************************************************
 * Function  : HashTable<K, V>::containsKey
 * Purpose   : Checks whether a key exists in the hash table.
 *
 * Returns   :
 * - bool : true if key is found, otherwise false.
 *
 * Time Complexity:
 * - Average: O(1)
 * - Worst-case: O(n)
 *********************************************************************************/
template <typename K, typename V>
bool HashTable<K, V>::containsKey(const K& key) {
	int index = hash(key);
	Node<K, V>* current = items[index];

	while (current != nullptr) {
		if (current->key == key)
			return true;
		current = current->next;
	}
	return false;
}

/*********************************************************************************
 * Function  : HashTable<K, V>::forEach
 * Purpose   : Iterates through every stored (key, value) pair and applies a
 *             caller-provided function/lambda.
 *
 * Params    :
 * - fn : Callable that accepts (const K&, const V&) or (K, V) depending on usage.
 *
 * Example Usage:
 * - table.forEach([](const string& key, NodePtr value){ ... });
 *
 * Time Complexity:
 * - O(MAX_SIZE + N) where N is total nodes stored in the table.
 *********************************************************************************/
template <typename K, typename V>
template <typename Func>
void HashTable<K, V>::forEach(Func fn) {
	// Loop through every bucket in the hash table
	for (int i = 0; i < MAX_SIZE; i++) {
		// Start at the head of the linked list in this bucket
		Node<K, V>* current = items[i];

		while (current != nullptr) {
			// Call the lambda function provided by the caller
			// Pass the key and value of the current hash node
			fn(current->key, current->value);

			// Move to the next node
			current = current->next;
		}
	}
}