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
		// Constructor
		List();

		// destructor
		~List();

		bool isEmpty();

		// add an item to the back of the list (append)
		bool add(T item);

		// remove an item at a specified position in the list 
		void remove(int index);

		// get an item at a specified position in the list (retrieve)
		T get(int index);

		// Check the size of the list
		int getLength();

		// display all the items in the list
		void print();


};