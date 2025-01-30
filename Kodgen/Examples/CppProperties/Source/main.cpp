#include <iostream>

#include "Generated/DataState.h.hpp"

int main()
{
	DataState dataState;

	dataState.EmplaceBackData<DataState::DataType_SomeClass>();
	dataState.EmplaceBackData<DataState::DataType_SomeOtherClass>();

	//Get the last element of the vector of SomeClass
	SomeNamespace::np1::SomeClass& someClass = std::get<std::vector<SomeNamespace::np1::SomeClass>*>(dataState.GetDatasPtrVar<DataState::DataType_SomeClass>())->back();
	
	//Instantiate a nested class
	SomeNamespace::np1::SomeClass::SomeNestedClass	someNestedClass;

	std::cout << someClass.get_someChar()				<< std::endl;
	std::cout << *someClass.get_someFloat()				<< std::endl;
	std::cout << someClass.get_someInt()				<< std::endl;
	std::cout << someClass.get_someUnsignedLongLong()	<< std::endl;
	std::cout << someClass.get_someString()				<< std::endl;
	someClass.get_someNestedClass();

	someClass.set_someFloat(640.45f);
	someClass.set_someInt(46);
	someClass.set_someUnsignedLongLong(123u);
	someClass.set_someString("This is another test");
	someClass.set_someNestedClass(&someNestedClass);

	std::cout << someClass.get_someChar()				<< std::endl;
	std::cout << someClass.get_someFloat()				<< std::endl;
	std::cout << someClass.get_someInt()				<< std::endl;
	std::cout << someClass.get_someUnsignedLongLong()	<< std::endl;
	std::cout << someClass.get_someString()				<< std::endl;

	//Get the last element of the vector of SomeOtherClass
	SomeNamespace::SomeOtherClass	someOtherClass = std::get<std::vector<SomeNamespace::SomeOtherClass>*>(dataState.GetDatasPtrVar<DataState::DataType_SomeOtherClass>())->back();

	std::cout << someOtherClass.get_someFloat() << std::endl;
	std::cout << someOtherClass.get_someVectorOfInt().data() << std::endl;
	std::cout << someOtherClass.get_someVectorOfSomeClass().data() << std::endl;
	std::cout << someOtherClass.get_someUmapOfSomeClass2().size() << std::endl;

	return EXIT_SUCCESS;
}