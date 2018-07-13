#include <iostream>
#include "promisepp.hpp"

int main()
{
	promisepp::helpers::Variant<int, short> t_variant;
	std::cout << std::boolalpha;

	std::cout << (promisepp::helpers::IsOneOf<int, int>::value == true) << std::endl;
	std::cout << (promisepp::helpers::IsOneOf<int, short, int>::value == true) << std::endl;
	std::cout << (promisepp::helpers::IsOneOf<int, short>::value == false) << std::endl;
	std::cout << (promisepp::helpers::IsOneOf<int, bool, short>::value == false) << std::endl;

	std::cout << (promisepp::helpers::IndexOf<int, int>::value == 0) << std::endl;
	std::cout << (promisepp::helpers::IndexOf<int, short, std::string, int>::value == 2) << std::endl;

	promisepp::helpers::Variant<bool, int> variant1{true};

	std::cout << (variant1.to<bool>() == true) << std::endl;

	variant1 = false;

	std::cout << (variant1.to<bool>() == false) << std::endl;

	variant1 = 7;

	std::cout << (variant1.to<int>() == 7) << std::endl;
}
