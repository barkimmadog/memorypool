#include "cx/init.h"
#include "object.h"

#include <iostream>

int main() {
	Object* obj_array[1000];
	for(int ii=0; ii < 1000; ii++) {
		obj_array[ii] = new Object;
		obj_array[ii]->setX(ii);
	}
	for(int ii=0; ii < 1000; ii++) {
		std::cout << obj_array[ii]->getX() << "\n";
	}
}
