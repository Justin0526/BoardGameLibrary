/*********************************************************************************
 * Group         : T01
 * Team Member   : Justin Tang Jia Ze (S10269496B)
 *
 * File Purpose:
 * - Generic singly linked list implemented using C++ templates.
 * - Used as the main in-memory container for records loaded from CSV files
 *   (e.g., Games, Members, Ratings).
 *
 * Key Design Notes:
 * - Returns NodePtr from add() so other components (e.g., HashTable) can store
 *   stable references to list nodes for fast lookup and direct updates.
 * - getItem(NodePtr) returns T& / const T& to allow modifying the stored object
 *   without copying.
 *
 * Constraints / Assumptions:
 * - List nodes are heap-allocated and owned by this List. Destructor/clear()
 *   frees all nodes.
 *********************************************************************************/
#pragma once
#include<string>
#include<iostream>
using namespace std;

// List<T>:
// A reusable singly linked list with basic operations (add, get, remove),
// plus merge sort support. Designed to work with HashTable via NodePtr.
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

	// ---- Merge Sort Helpers (internal) ----
	// mergeSort: Recursively sorts a linked list using merge sort.
	// split    : Splits the list into two halves and returns the head of the second half.
	// merge    : Merges two sorted lists into one sorted list using the comparator.

	// Comparator function used for sorting.
    // cmp(a, b) should return true if 'a' should appear before 'b'.

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

	void printWithIndex();

	// merge sort
	void sort(bool (*cmp)(const T&, const T&));

	template <typename Pred>
	List<T> filter(Pred pred) const;

	void clear();
};

// Constructor
template <typename T>
List<T>::List() {
	firstNode = nullptr;
	size = 0;
}

// Destructor
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

// Check if the list is empty
template <typename T>
bool List<T>::isEmpty() {
	return firstNode == nullptr;
}

/*********************************************************************************
 * Function  : List<T>::add
 * Purpose   : Appends an item to the end of the list and returns the node pointer.
 *             NodePtr is used by HashTable to store direct references for O(1)
 *             average lookup + direct modification via getItem(NodePtr).
 *
 * Params    :
 *   - item (const T&) : Object to be stored in the list (copied into node).
 *
 * Returns   :
 *   - NodePtr : Pointer to the newly created node (stable until removed/cleared).
 *
 * Time Complexity:
 * - O(n) due to traversal to the tail (no tail pointer stored).
 *********************************************************************************/

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

/*********************************************************************************
 * Function  : List<T>::remove
 * Purpose   : Removes a target node from the list and frees its memory.
 *
 * Params    :
 *   - target (NodePtr) : Node to remove (must be a node belonging to this list).
 *
 * Returns   :
 *   - bool : true if removed successfully, false if target not found / invalid.
 *
 * Time Complexity:
 * - O(n) to locate the target node and relink.
 *********************************************************************************/
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

/*********************************************************************************
 * Function  : List<T>::get
 * Purpose   : Retrieves the item stored at a given index (0-based).
 *
 * Params    :
 *   - index (int) : Position in list (0 <= index < size).
 *
 * Returns   :
 *   - T : A COPY of the item at that index.
 *
 * Notes     :
 * - Caller is responsible for ensuring index is valid (no bounds checking here).
 * - Use getNode + getItem if you want a reference (no copying).
 *
 * Time Complexity:
 * - O(n) due to traversal from the head.
 *********************************************************************************/
template <typename T>
T List<T>::get(int index) {
	Node* current = firstNode;

	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->item;
}

template <typename T>
T List<T>::get(int index) const {
	const Node* current = firstNode;

	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->item;
}

/*********************************************************************************
 * Function  : List<T>::getNode
 * Purpose   : Returns the node pointer at a specified index (0-based).
 *
 * Params    :
 *   - index (int) : Position in list (0 <= index < size).
 *
 * Returns   :
 *   - NodePtr : Pointer to the node, or nullptr if index is out of range.
 *
 * Time Complexity:
 * - O(n) due to traversal from the head.
 *********************************************************************************/
template <typename T>
typename List<T>::NodePtr List<T>::getNode(int index) {
	if (index < 0 || index >= size) return nullptr;
	Node* current = firstNode;
	for (int i = 0; i < index; ++i) {
		current = current->next;
	}
	return current;
}

/*********************************************************************************
 * Function  : List<T>::getItem
 * Purpose   : Accesses the item stored inside a node directly (no copying).
 *
 * Params    :
 *   - p (NodePtr) : Node pointer obtained from add()/getNode()/HashTable lookup.
 *
 * Returns   :
 *   - T& / const T& : Reference to the stored item.
 *
 * Notes     :
 * - p must not be nullptr.
 *
 * Time Complexity:
 * - O(1)
 *********************************************************************************/
template<typename T>
T& List<T>::getItem(NodePtr p) {
	return p->item;
}

// Get const item through NodePtr
// Time Complexity: O(1)
template<typename T>
const T& List<T>::getItem(NodePtr p) const {
	return p->item;
}

// Return size of list
// Time Complexity: O(1)
template <typename T>
int List<T>::getLength() {
	return size;
}

// Return const size of list
// Time Complexity: O(1)
template <typename T>
int List<T>::getLength() const {
	return size;
}

// display all the items in the list
// Time Complexity: O(n)
template <typename T>
void List<T>::print() {
	Node* current = firstNode;
	for (int i = 0; i < size; i++) {
		cout << current->item;
		current = current->next;
	}
}

// Display all items in list with index
// Time Complexity: O(n)
template <typename T>
void List<T>::printWithIndex() {
	Node* current = firstNode;
	for (int i = 0; i < size; i++) {
		cout << i + 1 << ". " << current->item;
		current = current->next;
	}
}

/*********************************************************************************
 * Function  : List<T>::sort
 * Purpose   : Sorts the list in-place using merge sort with a custom comparator.
 *
 * Params    :
 *   - cmp : Comparator function (returns true if left item comes before right).
 *
 * Time Complexity:
 * - O(n log n)
 *********************************************************************************/
template <typename T>
void List<T>::sort(bool (*cmp)(const T&, const T&)) {
	firstNode = mergeSort(firstNode, cmp);
}

// Merge sort
// Time Complexity: O(nlogn)
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
// Time Complexity: O(n)
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

// Merge two sorted lists.
// Time Complexity: O(n) where n is total nodes across both lists.
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

// Filter is a template that accepts any callable function/labmbda/functor (Not used anymore)
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

// Function to clear the list for new inputs, used in Recommend Games function. Used to prevent shallow copies, and never learnt deep copies before
// Time Complexity: O(n)
template <typename T>
void List<T>::clear() {
	Node* current = firstNode;
	while (current != nullptr) {
		Node* temp = current;
		current = current->next;
		delete temp;
	}
	firstNode = nullptr;
	size = 0;
}