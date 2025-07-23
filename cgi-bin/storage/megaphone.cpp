#include <iostream>

int main(int argc, char **argv)
{
	if (argc == 1)
		std::cout << "* LOUD AND UNBEARABLE FEEDBACK NOISE *";
	for (int i = 1; i < argc; i++)
	{
		std::string s = argv[i];
		for (size_t j = 0; j < s.length(); j++)
			s[j] = std::toupper(s[j]);
		std::cout << s;
		if (i < argc - 1)
			std::cout << " ";
	}
	std::cout << std::endl;
}
