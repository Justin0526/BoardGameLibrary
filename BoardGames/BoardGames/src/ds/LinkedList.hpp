#include "LinkedList.h"

template <typename T>
List<T>::List() {
	firstNode = nullptr;
	size = 0;
}

template <typename T>
List<T>::~List() {
	
	if (firstNode != nullptr) {
		Node* current = firstNode;
		Node* next = firstNode;
		while (current != nullptr) {
			next = current;
			current = current->next;
			delete next;
		}
		firstNode = nullptr;
		size = 0;
	}
}

template <typename T>
bool List<T>::isEmpty() {
	return firstNode == nullptr;
}

template <typename T>
bool List<T>::add(T item) {
	Node* newNode = new Node;
	newNode->item = item;
	newNode->next = nullptr;

	if (isEmpty()) {
		firstNode = newNode;
	}
	else {
		Node* current = firstNode;
		while (current->next != nullptr) {
			current = current->next;
		}
		current->next = newNode;
	}

	size++;
	return true;
}

template <typename T>
void List<T>::remove(int index) {

	if (index >= 0 && index <= size) {

		Node* current;
		if (index == 0) {
			current = firstNode;
			firstNode = firstNode->next;
			delete current;
		}
		else {
			Node* previous = firstNode;

			for (int i = 0; i < index-1; i++) {
				previous = previous->next;
			}
			current = previous->next;
			previous->next = current->next;
			delete current;
		}

		size--;
	}
}

template <typename T>
T List<T>::get(int index) {
	Node* current = firstNode;
	
	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->item;
}

template <typename T>
int List<T>::getLength() {
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