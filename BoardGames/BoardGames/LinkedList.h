#pragma once
#include<string>
#include<iostream>
using namespace std;

// Generic type t for List<Games> and List<Members>
template <typename T>

class List
{
private:
	struct Node
	{
		T item;
		Node* next;
	};
	Node* firstNode;
	int size;

public:
	using NodePtr = Node*;

	// Constructor
	List();

	// destructor
	~List();

	bool isEmpty();

	// add an item to the back of the list (append)
	NodePtr add(const T& item);

	// remove an item at a specified position in the list 
	bool remove(NodePtr target);

	// get an item at a specified position in the list (retrieve)
	T get(int index);
	T get(int index) const; // const overload

	// get node pointer at index (new)
	NodePtr getNode(int index);

	T& getItem(NodePtr p);
	const T& getItem(NodePtr p) const;

	// Check the size of the list
	int getLength();
	int getLength() const; // const

	// display all the items in the list
	void print();
	void print() const; // const

};

template <typename T>
List<T>::List() {
	firstNode = nullptr;
	size = 0;
}

template <typename T>
List<T>::~List() {
	Node* cur = firstNode;
	while (cur != nullptr) {
		Node* nxt = cur->next;
		delete cur;
		cur = nxt;
	}
	firstNode = nullptr;
	size = 0;
}

template <typename T>
bool List<T>::isEmpty() {
	return firstNode == nullptr;
}

template <typename T>
typename List<T>::NodePtr List<T>::add(const T& item) {
	Node* newNode = new Node;
	newNode->item = item;
	newNode->next = nullptr;
	
	if (isEmpty()) {
		firstNode = newNode;
	}
	else {
		Node* current = firstNode;
		while (current->next != nullptr)
			current = current->next;

		current->next = newNode;
	}
	size++;
	return newNode;
}

template <typename T>
bool List<T>::remove(NodePtr target) {

	if (firstNode == nullptr || target == nullptr)
		return false;

	if (firstNode == target) {
		firstNode = firstNode->next;
		delete target;
		size--;
		return true;
	}

	Node* current = firstNode;
	while (current->next != nullptr && current->next != target) {
		current = current->next;
	}

	if (current->next == nullptr)
		return false;

	current->next = target->next;
	delete target;
	size--;
	return true;
}

template <typename T>
T List<T>::get(int index) {
	Node* current = firstNode;

	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->item;
}

// const overload
template <typename T>
T List<T>::get(int index) const {
	const Node* current = firstNode;

	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->item;
}

// new: return NodePtr at index (or nullptr if out of range)
template <typename T>
typename List<T>::NodePtr List<T>::getNode(int index) {
	if (index < 0 || index >= size) return nullptr;
	Node* current = firstNode;
	for (int i = 0; i < index; ++i) {
		current = current->next;
	}
	return current;
}

template<typename T>
T& List<T>::getItem(NodePtr p) {
	return p->item;
}

template<typename T>
const T& List<T>::getItem(NodePtr p) const {
	return p->item;
}

template <typename T>
int List<T>::getLength() {
	return size;
}

template <typename T>
int List<T>::getLength() const {
	return size;
}

// display all the items in the list
template <typename T>
void List<T>::print() {
	Node* current = firstNode;
	for (int i = 0; i < size; i++) {
		cout << current->item << endl;
		current = current->next;
	}
}

template <typename T>
void List<T>::print() const {
	const Node* current = firstNode;
	for (int i = 0; i < size; i++) {
		cout << current->item << endl;
		current = current->next;
	}
}