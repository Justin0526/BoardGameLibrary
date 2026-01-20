#include <iostream>
#include <string>
#include "../src/ds/LinkedList.h"

using namespace std;

int main() {
	List<int> numList = List();
	numList.add(1);
	numList.add(2);

	numList.print();
}