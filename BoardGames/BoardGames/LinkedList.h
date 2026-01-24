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

	// cmp is a fuunction that compares two T objects and returns true if first should come before the second

	Node* mergeSort(Node* head, bool (*cmp)(const T&, const T&));
	Node* split(Node* head);
	Node* merge(Node* left, Node* right, bool (*cmp)(const T&, const T&));

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

	T& getItem(NodePtr p);
	const T& getItem(NodePtr p) const;

	// Check the size of the list
	int getLength();

	// display all the items in the list
	void print();

	// merge sort
	void sort(bool (*cmp)(const T&, const T&));

	template <typename Pred>
	List<T> filter(Pred pred) const;
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

// display all the items in the list
template <typename T>
void List<T>::print() {
	Node* current = firstNode;
	for (int i = 0; i < size; i++) {
		cout << i+1 << ". " << current->item;
		current = current->next;
	}
}

template <typename T>
void List<T>::sort(bool (*cmp)(const T&, const T&)) {
	firstNode = mergeSort(firstNode, cmp);
}

// Merge sort
template <typename T>
typename List<T>::Node*
List<T>::mergeSort(Node* head, bool (*cmp)(const T&, const T&)) {
	// Base case: If empty list or single node then it is sorted
	if (head == nullptr || head->next == nullptr) {
		return head;
	}

	// Split list into two halves
	Node* mid = split(head);
	
	// Sort left half
	Node* left = mergeSort(head, cmp);
	
	// Sort right half
	Node* right = mergeSort(mid, cmp);

	// merge sorted halves
	return merge(left, right, cmp);

}

// Split 
template <typename T>
typename List<T>::Node* 
List<T>::split(Node* head) {
	Node* slow = head;
	Node* fast = head->next;

	while (fast != nullptr && fast->next != nullptr) {
		slow = slow->next;
		fast = fast->next->next;
	}

	Node* secondHalf = slow->next;
	slow->next = nullptr; // cut the list
	return secondHalf;
}

// Merge
template <typename T>
typename List<T>::Node*
List<T>::merge(Node* left, Node* right, bool (*cmp)(const T&, const T&)) {
	if (left == nullptr) return right;
	if (right == nullptr) return left;

	if (cmp(left->item, right->item)) {
		left->next = merge(left->next, right, cmp);
		return left;
	}
	else {
		right->next = merge(left, right->next, cmp);
		return right;
	}
}

// Filter is a template that accepts any callable function/labmbda/functor
template <typename T>
template <typename Pred>
List<T> List<T>::filter(Pred pred) const {
	List<T> result;

	// Start from first node
	Node* current = firstNode;

	while (current != nullptr) {
		// If predicate returns true, keep the item
		if (pred(current->item)) {
			result.add(current->item); // add a COPY of the item
		}
		current = current->next; // move forward
	}

	return result;
	
}