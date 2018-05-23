#include <algorithm> // std::max
#include <string> // std::string
#include <vector> // std::vector
#include <stdlib.h> // free
#include <iostream> // std::cout, std::cin, system()
#include <fstream> // std::ifstream, std::ofstream
#include "print.h" // console and file output

class Tree
{
public:
	Tree()
	{
		root = NULL;
	}
	void create()
	{
		print("Creating...\n");
		clear(root); root = NULL;
		std::vector<unsigned int> tree_nodes = nodes();
		for (int i = 0; i < tree_nodes.size(); i++)
			root = insert(tree_nodes[i], root);
		print("  Tree (Pre-Order): ");
		traverse("Pre-Order", root);
		print("\n");
	}
	void insert(unsigned int number)
	{
		print("Inserting >> " + std::to_string(number) + "...\n");
		root = insert(number, root);
		print("  Tree (Pre-Order): ");
		traverse("Pre-Order", root);
		print("\n");
	}
	void remove(unsigned int number)
	{
		print("Removing >> " + std::to_string(number) + "...\n");
		if (root != NULL) // tree does exist
			root = remove(number, root);
		else
			print("  Node cannot be removed. Empty Tree!\n");
		print("  Tree (Pre-Order): ");
		traverse("Pre-Order", root);
		print("\n");
	}
	void search(unsigned int number)
	{
		print("Searching >> " + std::to_string(number) + "...\n");
		if (root != NULL) // tree does exist
			root = search(number, root);
		else
			print("  Node cannot be searched. Empty Tree!\n");
	}
	void traverse(std::string method)
	{
		print("Traversing...\n");
		if (method.compare("Pre-Order") == 0)
		{
			print("  Tree (Pre-Order): ");
			traverse("Pre-Order", root);
		}
		else if (method.compare("Post-Order") == 0)
		{
			print("  Tree (Post-Order): ");
			traverse("Post-Order", root);
		}
		else if (method.compare("In-Order") == 0)
		{
			print("  Tree (In-Order): ");
			traverse("In-Order", root);
		}
		print("\n");
	}
	void clear()
	{
		print("Clearing...\n");
		if (root != NULL) // tree does exist
		{
			clear(root); root = NULL;
			print("  Tree has been cleared.\n");
		}
		else
			print("  Tree cannot be cleared. Empty Tree!\n");
	}
	void balance()
	{
		print("Balancing...\n");
		if (root != NULL) // tree does exist
			print("  Factor = " + std::to_string(difference(root)) + "\n");
		else
			print("  Tree cannot be balanced. Empty Tree!\n");
	}
private:
	struct node
	{
		int data;
		struct node *left;
		struct node *right;
	}*root;
	unsigned int height_position = 0;
	unsigned int new_data = NULL;
	std::vector<unsigned int> nodes()
	{
		unsigned int i;
		char *in_name = "input.txt";
		std::ifstream in_file(in_name);
		if (!in_file)
			print("  Read File Error!\n");
		else
		{
			std::vector<unsigned int> tree_nodes;
			while (in_file >> i) {
				tree_nodes.push_back(i);
			}
			return tree_nodes;
		}
	}
	unsigned int height(node *branch)
	{
		unsigned int depth = 0;
		if (branch != NULL)
		{
			unsigned int left_height = height(branch->left);
			unsigned int right_height = height(branch->right);
			unsigned int max_height = std::max(left_height, right_height);
			depth = max_height + 1;
		}
		return depth;
	}
	int difference(node *branch)
	{
		unsigned int left_height = height(branch->left);
		unsigned int right_height = height(branch->right);
		int balance_factor = left_height - right_height;
		return balance_factor;
	}
	node *rotate(std::string direction, node *branch)
	{
		node *temp;
		if (direction.compare("Left-Left") == 0)
		{
			temp = branch->left;
			branch->left = temp->right;
			temp->right = branch;
			return temp;
		}
		else if (direction.compare("Right-Right") == 0)
		{
			temp = branch->right;
			branch->right = temp->left;
			temp->left = branch;
			return temp;
		}
		else if (direction.compare("Left-Right") == 0)
		{
			temp = branch->left;
			branch->left = rotate("Right-Right", temp);
			return rotate("Left-Left", branch);
		}
		else if (direction.compare("Right-Left") == 0)
		{
			temp = branch->right;
			branch->right = rotate("Left-Left", temp);
			return rotate("Right-Right", branch);
		}
	}
	node *balance(node *branch)
	{
		int balance_factor = difference(branch);
		if (balance_factor > 1)
		{
			if (difference(branch->left) > 0)
				branch = rotate("Left-Left", branch);
			else
				branch = rotate("Left-Right", branch);
		}
		else if (balance_factor < -1)
		{
			if (difference(branch->right) > 0)
				branch = rotate("Right-Left", branch);
			else
				branch = rotate("Right-Right", branch);
		}
		return branch;
	}
	node *minimum(node *branch)
	{
		if (branch->left != NULL)
			branch->left = minimum(branch->left);
		else
			new_data = branch->data;
		return branch;
	}
	node *maximum(node *branch)
	{
		if (branch->right != NULL)
			branch->right = maximum(branch->right);
		else
			new_data = branch->data;
		return branch;
	}
	node *insert(unsigned int number, node *branch)
	{
		if (branch == NULL) // terminal node
		{
			// insert new node
			branch = new node();
			branch->data = number;
			return branch;
		}
		else // internal node
		{
			if (number < branch->data) // smaller than internal node
			{
				branch->left = insert(number, branch->left);
				std::cout << "BRANCH DATA, Insert = " << branch->data << std::endl;
				std::cout << "BRANCH LEFT, Insert = " << branch->left->data << std::endl;
				branch = balance(branch);
			}
			else if (number > branch->data) // larger than internal node
			{
				branch->right = insert(number, branch->right);
				std::cout << "BRANCH DATA, Insert = " << branch->data << std::endl;
				std::cout << "BRANCH RIGHT, Insert = " << branch->right->data << std::endl;
				branch = balance(branch);
			}
			else // equal to internal node
				print("  Number cannot be inserted. Duplicate Error! >> " + std::to_string(number) + "\n");
		}
		return branch;
	}
	void replace(std::string path, node *branch)
	{
		if (new_data != NULL)
		{
			if (path.compare("left") == 0)
				branch->left = NULL;
			else if (path.compare("right") == 0)
				branch->right = NULL;
			new_data = NULL;
		}
	}
	node *remove(unsigned int number, node *branch)
	{
		if (branch == NULL) // terminal node
		{
			print("  Node not found. Remove Error!\n");
			return NULL;
		}
		else // internal node
		{
			if (number < branch->data) // smaller than internal node
			{
				branch->left = remove(number, branch->left);
				replace("left", branch);
				std::cout << "BRANCH DATA, Remove = " << branch->data << std::endl;
				try
				{
					std::cout << "BRANCH LEFT, Remove = " << branch->left->data << std::endl;
				}
				catch(std::exception Ex)
				{
					std::cout << "BRANCH LEFT, Remove = " << "NULL" << std::endl;
				}
				branch = balance(branch);
			}
			else if (number > branch->data) // larger than internal node
			{
				branch->right = remove(number, branch->right);
				replace("right", branch);
				std::cout << "BRANCH DATA, Remove = " << branch->data << std::endl;
				try
				{
					std::cout << "BRANCH RIGHT, Remove = " << branch->right->data << std::endl;
				}
				catch (std::exception Ex)
				{
					std::cout << "BRANCH RIGHT, Remove = " << "NULL" << std::endl;
				}
				branch = balance(branch);
			}
			else // equal to internal node
			{
				std::cout << "FOUND! >> " << number << std::endl;
				// node has no branches
				if (branch->left == NULL && branch->right == NULL)
				{
					std::cout << "No branches" << std::endl;
					new_data = branch->data;
				}
				// node has a left branch
				else if (branch->left != NULL)
				{
					std::cout << "Has left branch." << std::endl;
					branch->left = maximum(branch->left);
					std::cout << "BRANCH DATA, ~Remove = " << branch->data << std::endl;
					if (branch->left->data != NULL)
						std::cout << "BRANCH LEFT, ~Remove = " << branch->left->data << std::endl;
					else
						std::cout << "BRANCH LEFT, ~Remove = " << "NULL" << std::endl;
				}
				// node has no left branch
				else
				{
					std::cout << "Has right branch." << std::endl;
					branch->right = minimum(branch->right);
					std::cout << "BRANCH DATA, ~Remove = " << branch->data << std::endl;
					if (branch->right->data != NULL)
						std::cout << "BRANCH RIGHT, ~Remove = " << branch->right->data << std::endl;
					else
						std::cout << "BRANCH RIGHT, ~Remove = " << "NULL" << std::endl;
				}
				print("  Node found and removed.\n");
			}
		}
		return branch;
	}
	node *search(unsigned int number, node *branch)
	{
		if (branch == NULL) // terminal node
		{
			// node not found
			print("  Node not found.\n");
			height_position = 0;
			return branch;
		}
		else // internal node
		{
			if (number < branch->data) // smaller than internal node
			{
				height_position++;
				branch->left = search(number, branch->left);
			}
			else if (number > branch->data) // larger than internal node
			{
				height_position++;
				branch->right = search(number, branch->right);
			}
			else // node is found
			{
				print("  Node is found at height " + std::to_string(height_position) + ".\n");
				height_position = 0;
			}
		}
		return branch;
	}
	void clear(node *branch)
	{
		if (branch != NULL)
		{
			clear(branch->left);
			clear(branch->right);
			branch = NULL;
			free(branch);
		}
	}
	void traverse(std::string method, node *branch)
	{
		if (branch != NULL)
		{
			if (method.compare("Pre-Order") == 0)
			{
				print(std::to_string(branch->data) + " ");
				traverse("Pre-Order", branch->left);
				traverse("Pre-Order", branch->right);
			}
			else if (method.compare("Post-Order") == 0)
			{
				traverse("Post-Order", branch->left);
				traverse("Post-Order", branch->right);
				print(std::to_string(branch->data) + " ");
			}
			else if (method.compare("In-Order") == 0)
			{
				traverse("In-Order", branch->left);
				print(std::to_string(branch->data) + " ");
				traverse("In-Order", branch->right);
			}
		}
	}
};