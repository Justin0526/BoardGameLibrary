#pragma once
#include <string>
#include <optional>

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
		void remove(const K& key);
		V get(const K& key);
		bool isEmpty();
		int getLength();
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
	
	return nullptr;
}

template <typename K, typename V>
bool HashTable<K, V>::isEmpty() {
	return size == 0;
}

template <typename K, typename V>
int HashTable<K, V>::getLength() {
	return size;
}