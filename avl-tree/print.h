#include <iostream> // std::cout, std::cin, system()
#include <fstream> // std::ifstream, std::ofstream
#include <string> // std::string

char *out_name = "meine-A01764207.txt";
std::ofstream out_file;

void print(std::string text)
{
	std::cout << text;
	out_file.open(out_name, std::ios::app);
	out_file << text;
	out_file.close();
}