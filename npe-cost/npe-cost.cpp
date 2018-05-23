// 10/26/2017 - CS 5460 - Meine, Joel
// Homework #4 - NPE Cost & Simulated Annealing

#include <vector> // std::vector
#include <string> // std::string
#include "npe-cost.h"
Tree tree;

int main()
{
	tree.seed();

	tree.load("test_file.txt");
	std::vector<std::string> NPE_test = { "a", "b", "H", "c", "d", "V", "e", "f", "H", "V", "H" };
	tree.search(NPE_test);

	tree.load("input_file.txt");
	std::vector<std::string> NPE_all_horizontal = { "1", "2", "H", "3", "H", "4", "H", "5", "H", "6", "H", "7", "H", "8", "H", "9", "H", "a", "H", "b", "H", "c", "H", "d", "H", "e", "H", "f", "H", "g", "H", "i", "H", "j", "H", "k", "H", "l", "H" };
	tree.search(NPE_all_horizontal);

	std::vector<std::string> NPE_all_vertical = { "1", "2", "V", "3", "V", "4", "V", "5", "V", "6", "V", "7", "V", "8", "V", "9", "V", "a", "V", "b", "V", "c", "V", "d", "V", "e", "V", "f", "V", "g", "V", "i", "V", "j", "V", "k", "V", "l", "V" };
	tree.search(NPE_all_vertical);
	
	std::vector<std::string> NPE_mixed_operators = { "2", "1", "3", "5", "4", "6", "H", "7", "V", "H", "V", "a", "8", "V", "9", "H", "c", "V", "H", "g", "H", "i", "b", "d", "H", "k", "V", "H", "f", "e", "H", "V", "l", "H", "V", "j", "H", "V", "H" };
	tree.search(NPE_mixed_operators);

	std::cout << "Press ENTER twice to exit the program.";
	std::cin.ignore().get();
	return 0;
}