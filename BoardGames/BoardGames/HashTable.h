#pragma once
#include <string>
#include <optional>
#include <functional>

const int MAX_SIZE = 101;

template<typename K, typename V>
struct Node {
	K key;
	V value;
	Node<K, V>* next;
};

// Generic type t
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


template <typename K, typename V>
HashTable<K, V>::HashTable()
{ 
	size = 0;
	for (int i = 0; i < MAX_SIZE; i++) {
		items[i] = nullptr;
	}

};

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
	
	throw runtime_error("Key not found in HashTable");
}

template <typename K, typename V>
bool HashTable<K, V>::isEmpty() {
	return size == 0;
}

template <typename K, typename V>
int HashTable<K, V>::getLength() {
	return size;
}

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

			// Move to the enxt node
			current = current->next;
		}
	}
}