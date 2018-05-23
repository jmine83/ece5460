#include <algorithm> // std::max
#include <math.h> // sqrt(), log(), exp()
#include <string> // std::string
#include <vector> // std::vector
#include <time.h> // time()
#include <stdlib.h> // free(), srand(), rand()
#include <iostream> // std::cout, std::cin, system()
#include <fstream> // std::ifstream, std::ofstream
#include "print.h" // console and file output
#include "timer.h" // records running time
#include <cfloat> // DLB_MAX

class Tree
{
public:
	Tree()
	{
		root = NULL;
	}
	void seed()
	{
		srand (time(NULL));
	}
	void load(std::string file_name)
	{
		print("Loading input file... ");
		entries.clear();
		modules(file_name);
		print(" DONE\n\n");
	}
	void search(std::vector<std::string> NPE_initial)
	{
		print("Calculating initial topology and cost... ");
		clear(root); root = NULL;
		start(NPE_initial);
		root = insert(root);
		traverse(root->name, root);
		print(" DONE\n\n");
		print("E, Initial: ");
		expression(true);
		print("\n");
		print("Cost, Initial (Area): " + std::to_string(solution()) + "\n");
		print("\n");
		print("Calculating final topology and cost... ");
		search();
		print(" DONE\n\n");
		print("E, Final: ");
		expression(false);
		print("\n");
		print("Cost, Final (Area): " + std::to_string(cost_E_best) + "\n");
		print("\n");
		reset();
	}
private:
	typedef struct {
		std::string name;
		double width;
		double height;
	} module;
	std::vector<int> integer; std::vector<int> fraction;
	bool at_point = false;

	// store digit as either an integer or a fraction
	void buffer(int digit)
	{
		if (!at_point)
			integer.push_back(digit);
		else
			fraction.push_back(digit);
	}

	// convert vectors into floating point number
	double convert()
	{
		double number = 0;
		double position = 1;
		for (int i = integer.size()-1; i >= 0; i--)
		{
			number += integer[i] * position;
			position *= 10;
		}
		position = 0.1;
		for (int i = 0; i < fraction.size(); i++)
		{
			number += fraction[i] * position;
			position *= 0.1;
		}
		integer.clear(); fraction.clear();
		return number;
	}
	double area; double aspect_ratio;
	std::vector<module> entries; // modules data
	double entries_area; double cost_area;

	// load module data from file
	void modules(std::string file_name)
	{
		std::ifstream data(file_name);
		module entry; bool new_entry = true;
		entries_area = 0;
		char symbol;
		while (data.get(symbol))
		{
			if (new_entry)
			{
				if (symbol != '\t') entry.name = symbol;
				else new_entry = false;
			}
			else
			{
				if (symbol == '.') at_point = true;
				else if (symbol == '\t' || symbol == '\n')
				{
					if (symbol == '\t')
						area = convert();
					else if (symbol == '\n')
					{
						aspect_ratio = convert();
						entry.width = sqrt(area * aspect_ratio);
						entry.height = sqrt(area / aspect_ratio);
						entry.width = format(entry.width, 1);
						entry.height = format(entry.height, 1);
						entries_area += entry.width * entry.height;
						entries.push_back(entry);
						new_entry = true;
					}
					at_point = false;
				}
				else if (symbol == '0') buffer(0);
				else if (symbol == '1') buffer(1);
				else if (symbol == '2') buffer(2);
				else if (symbol == '3') buffer(3);
				else if (symbol == '4') buffer(4);
				else if (symbol == '5') buffer(5);
				else if (symbol == '6') buffer(6);
				else if (symbol == '7') buffer(7);
				else if (symbol == '8') buffer(8);
				else if (symbol == '9') buffer(9);
			}
		}
		data.close();
	}

	typedef struct {
		double width;
		double height;
	} size;
	struct node
	{
		std::string name;
		std::vector<size> sizes;
		struct node *left;
		struct node *right;
	} *root;
	std::vector<std::string> nodes;
	std::vector<std::string> NPE; int NPE_position;
	void start(std::vector<std::string> array)
	{
		NPE = array;
		NPE_position = array.size()-1;
	}
	
	// insert operator or operand into tree
	node *insert(node *branch)
	{
		if (branch == NULL)
		{
			// insert new node
			branch = new node();
			branch->name = NPE[NPE_position];
			NPE_position--;
			insert(branch);
			return branch;
		}
		else
		{
			// build tree right to left
			if (NPE_position >= 0)
			{
				if (branch->name.compare("H") != 0 && branch->name.compare("V") != 0)
					return branch;
				else
				{
					branch->right = insert(branch->right);
					branch->left = insert(branch->left);
				}
				return branch;
			}
			else
				return branch;
		}
	}

	// Parameters
	const int n_moves = 10;
	/*
	Number of attempt moves per temperature iteration. Use 10 as the 
	default value. Larger values of n_moves in general produce better 
	results. The total execution time of the program grows linearly 
	with respect to n_moves.
	*/
	const double temperature_ratio = 0.85;
	/*
	Ratio for decreasing temperatures. Use 0.85 as the default value. 
	Larger values of ratios in general produce better results and have 
	much longer execution time.
	*/
	double initial_temperature = -1;
	/*
	Initial temperature. If the input t0 is negative (e.g. -1), the 
	program will compute an initial temperature (in this case you will 
	need to add a negative sign to make the initial temperature positive); 
	otherwise the program will use the input positive t0 as the initial 
	temperature. Default value is -1.
	*/
	const double lambda_tf = 0.005;
	/*
	The program will change ratio to 0.1 when temperature falls below 
	lambda_tf * t0. After ratio is set to 0.1, the program in general will 
	terminate very quickly.
	*/
	const int iseed = 3;
	/*
	Seed for random number generator.
	*/
	int n;
	/*
	Number of modules (this will depend on your input file).
	*/
	double P = 0.99;
	/*
	Initial probability of accepting uphill moves.
	*/
	const double epsilon = 0.001;
	/*
	Final threshold value to be less than to terminate algorithm.
	*/
	std::vector<double> cost_differences;
	/*
	List of cost differences for uphill moves.
	*/
	double cost_E;
	double cost_E_best;
	double cost_E_new;
	/*
	Result of the best expression of E.
	*/
	double time_length;
	double time_duration = 0;
	const double time_max = 30000; // 30 seconds
	/*
	Time duration tracking (in milliseconds).
	*/
	std::vector<std::string> E;
	std::vector<std::string> E_best;
	std::vector<std::string> E_new;
	/*
	Floorplan expresion tracking.
	*/

	void reset()
	{
		initial_temperature = -1; P = 0.99;
		time_length = 0; time_duration = 0;
		E = {}; E_best = {}; E_new = {};
		cost_differences = {};
		cost_E = DBL_MAX; cost_E_best = DBL_MAX; cost_E_new = DBL_MAX;
	}
	double d_avg()
	{
		double sum = 0;
		for (int i = 0; i < cost_differences.size(); i++)
			sum += cost_differences[i];
		return sum / cost_differences.size();
	}
	int N()
	{
		n = entries.size();
		int value = n_moves * n;
		return value;
	}
	int move()
	{
//		int choice = rand() % iseed + 1;
//		return choice;
		return 3;
	}
	double random()
	{
		int integer = rand() % 100 + 1;
		int value = 0.01 * (integer - 1);
		return value;
	}
	void legal()
	{
		// The resulting cost should never be less than the sum of the module areas.
		if (cost_area < entries_area)
			print("ERROR!");
	}
	bool valid()
	{
		int operands = 0;
		int operators = 0;
		for (int i = 0; i < E_new.size(); i++)
		{
			// Has an operand or operator been detected?
			if (E_new[i].compare("H") != 0 && E_new[i].compare("V") != 0)
				operands++;
			else
			{
				operators++;
				if (i < E_new.size()-1)
				{
					// Has a consecutive operator been detected?
					if (E_new[i].compare(E_new[i+1]) == 0)
					{
						E_new = E;
						return false;
					}
				}
			}
			if (operators >= operands)
			{
				E_new = E;
				return false;
			}
			else
			{
				if (i == E_new.size()-1)
					return true;
			}
		}
	}
	void M1()
	{
		// Swap adjacent operands (ignorning chains)
		E_new = E;
		std::vector<int> positions;
		for (int i = 0; i <= E.size()-3; i++)
			positions.push_back(i);
		bool selected = false;
		bool finished = false;

		do {
			// Select a random position that has not been selected before.
			int position;
			selected = false;
			do {
				position = rand() % (E.size()-2) + 1;
				position--;
				for (int i = 0; i < positions.size(); i++)
				{
					if (position == positions[i])
					{
						i = positions.size();
						selected = true;
					}
				}
			} while (!selected);

			// Has an operand been selected?
			if (E[position].compare("H") != 0 && E[position].compare("V") != 0)
			{
				// If yes, then locate the adjacent operand.
				for (int i = position+1; i <= E.size()-2; i++)
				{
					if (E[i].compare("H") != 0 && E[i].compare("V") != 0)
					{
						// Swap the operands.
						E_new[position] = E[i];
						E_new[i] = E[position];
						i = E.size()-1;
						finished = true;
					}
					if (i == E.size()-2)
						goto remove1;
				}
			}
			else
			{
			remove1:
				// Try again if a viable operand was not selected.
				for (int i = 0; i < positions.size(); i++)
				{
					if (position == positions[i])
					{
						positions.erase(positions.begin()+i);
						i = positions.size();
					}
				}
			}
		} while (!finished);

		positions = {};
	}
	void M2()
	{
		// Complement some chain
		E_new = E;
		std::vector<int> positions;
		for (int i = 0; i <= E.size()-2; i++)
			positions.push_back(i);
		bool selected = false;
		bool finished = false;

		do {
			// Select a random position that has not been selected before.
			int position;
			selected = false;
			do {
				position = rand() % (E.size()-1) + 1;
				position--;
				for (int i = 0; i < positions.size(); i++)
				{
					if (position == positions[i])
					{
						i = positions.size();
						selected = true;
					}
				}
			} while (!selected);

			// Has an operator been selected?
			if (E[position].compare("H") == 0 || E[position].compare("V") == 0)
			{
				// Is the selected operator the start of a chain?
				if (E[position-1].compare("H") != 0 && E[position-1].compare("V") != 0)
				{
					// If yes, then invert all operators in selected chain.
					for (int i = position; i <= E.size()-1; i++)
					{
						if (E[i].compare("H") == 0 || E[i].compare("V") == 0)
						{
							if (E[i].compare("H") == 0) E_new[i] = "V";
							else if (E[i].compare("V") == 0) E_new[i] = "H";
							if (i == E.size()-1)
								finished = true;
						}
						else
						{
							i = E.size();
							finished = true;
						}
					}
				}
				else
					goto remove2;
			}
			else
			{
			remove2:
				// Try again if a viable operand was not selected.
				for (int i = 0; i < positions.size(); i++)
				{
					if (position == positions[i])
					{
						positions.erase(positions.begin()+i);
						i = positions.size();
					}
				}
			}
		} while (!finished);

		positions = {};
	}
	void M3()
	{
		// Swap an adjacent operand and an operator
		/*
		(Note that M3 can give you some invalid NPE.
		So checking for validity after M3 is needed.)
		*/
		E_new = E;
		std::vector<int> positions;
		for (int i = 0; i <= E.size()-3; i++)
			positions.push_back(i);
		bool selected = false;
		bool finished = false;

		do {
			// Select a random position that has not been selected before.
			int position;
			selected = false;
			do {
				position = rand() % (E.size()-2) + 1;
				position--;
				for (int i = 0; i < positions.size(); i++)
				{
					if (position == positions[i])
					{
						i = positions.size();
						selected = true;
					}
				}
			} while (!selected);

			// Has an operator been selected?
			if (E[position].compare("H") == 0 || E[position].compare("V") == 0)
			{
				// If yes, is the adjacent node an operand?
				if (E[position+1].compare("H") != 0 && E[position+1].compare("V") != 0)
				{
					// Swap the operands.
					E_new[position] = E[position+1];
					E_new[position+1] = E[position];
				}
				else
					goto remove3;
			}
			// Has an operand been selected?
			else
			{
				// If yes, is the adjacent node an operator?
				if (E[position+1].compare("H") == 0 || E[position+1].compare("V") == 0)
				{
					// Swap the operands.
					E_new[position] = E[position+1];
					E_new[position+1] = E[position];
				}
				else
					goto remove3;
			}
			// Is resulting NPE valid?
			if (valid())
				finished = true;
			else
			{
			remove3:
				// Try again if a viable operand was not selected.
				for (int i = 0; i < positions.size(); i++)
				{
					if (position == positions[i])
					{
						positions.erase(positions.begin()+i);
						i = positions.size();
						if (positions.empty())
						{
							int other_move = rand() % 2 + 1;
							switch (other_move)
							{
							case 1:
								M1();
								break;
							case 2:
								M2();
								break;
							}
							finished = true;
						}
					}
				}
			}
		} while (!finished);

		positions = {};
	}
	double cost(std::vector<std::string> NPE_input)
	{
		clear(root); root = NULL;
		start(NPE_input);
		root = insert(root);
		traverse(root->name, root);
		nodes.clear();
		double result = solution();
		cost_area = result;
		legal();
		return result;
	}

	void printE()
	{
		print("E_old = ");
		for (int i = 0; i < E.size(); i++)
			print(E[i]);
		print("\n");
		print("E_new = ");
		for (int i = 0; i < E_new.size(); i++)
			print(E_new[i]);
		print("\n");
		print("E_old cost = " + std::to_string(cost(E)));
		print("\n");
		print("E_new cost = " + std::to_string(cost(E_new)));
		print("\n");
	}
	void printE_best()
	{
		print("E_bes = ");
		for (int i = 0; i < E_best.size(); i++)
			print(E_best[i]);
		print("\n");
		print("E_bes cost = " + std::to_string(cost(E_best)));
	}

	// find optimal solution
	void search()
	{
		E = NPE;
		E_best = NPE;
		E_new = NPE;
		int uphill = 0; int reject = 0; int MT = 0;
		double temperature = initial_temperature;
		do {
			time_length = timer(true);
			MT = 0; uphill = 0; reject = 0;
			do {
//				print("\n\n");
//				print("Before move...\n"); printE();
				switch (move())
				{
					case 1:
						M1();
//						print("\n");
//						print("M1");
//						print("\n\n");
						break;
					case 2:
						M2();
//						print("\n");
//						print("M2");
//						print("\n\n");
						break;
					case 3:
						M3();
//						print("\n");
//						print("M3");
//						print("\n\n");
						break;
				}
//				print("After move...\n"); printE();
				MT++;
				cost_E_new = cost(E_new); cost_E = cost(E);
				double cost_difference = cost_E_new - cost_E;
				if (cost_difference < 0 || random() < exp(-cost_difference / temperature))
				{
					if (cost_difference > 0)
					{
						uphill++;
						cost_differences.push_back(cost_difference);
						if (temperature < 0)
							temperature = (d_avg() / log(P));
						else
							initial_temperature = temperature;
					}
					E = E_new;
					cost_E_best = cost(E_best);
					if (cost_E_new < cost_E_best)
					{
						E_best = E;
						cost_E_best = cost_E_new;
					}
				}
				else
				{
					reject++;
//					print("Rejected!\n");
				}
//				printE_best();
/*				if (uphill > N())
				{
					print("\n\n");
					print("Uphills exceeded");
					print("\n");
				}
				if (MT > 2*N())
				{
					print("\n\n");
					print("MT exceeded");
					print("\n");
				}
*/			} while (uphill <= N() && MT <= 2*N());
			temperature = temperature_ratio * temperature;
			time_length = timer(false);
			time_duration += time_length;
/*			if (reject / MT > 0.95 || temperature <= epsilon || time_duration >= time_max)
			{
				if (reject / MT > 0.95)
				{
					print("\n");
					print("Rejects exceeded");
					print("\n");
				}
				if (temperature <= epsilon)
				{
					print("\n");
					print("Epsilon exceeded");
					print("\n");
				}
				if (time_duration >= time_max)
				{
					print("\n");
					print("Time exceeded");
					print("\n");
				}
			}
			else
				print("Repeat!");
*/		} while (reject/MT <= 0.95 && temperature > epsilon && time_duration < time_max);
	}

	// clear splicing tree
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

	// remove duplicate and un-needed sizes
	void cleanup(node *branch)
	{
		size selected;
		for (int i = 0; i < branch->sizes.size()-1; i++)
		{
			selected = branch->sizes[i];
			for (int j = i+1; j < branch->sizes.size(); j++)
			{
				if (selected.width == branch->sizes[j].width)
				{
					if (selected.height <= branch->sizes[j].height)
						branch->sizes.erase(branch->sizes.begin() + j);
					else
					{
						branch->sizes.erase(branch->sizes.begin() + i);
						j = branch->sizes.size();
						i--;
					}
				}
				else if (selected.height == branch->sizes[j].height)
				{
					if (selected.width <= branch->sizes[j].width)
						branch->sizes.erase(branch->sizes.begin() + j);
					else
					{
						branch->sizes.erase(branch->sizes.begin() + i);
						j = branch->sizes.size();
						i--;
					}
				}
				else if (selected.width == branch->sizes[j].height)
				{
					if (selected.height == branch->sizes[j].width)
					{
						if (branch->name.compare("H") == 0 || branch->name.compare("V") == 0)
						{
							if (branch->sizes.size() != 2)
							{
								branch->sizes.erase(branch->sizes.begin() + j);
								branch->sizes.erase(branch->sizes.begin() + i);
							}
						}
					}
				}
			}
		}
	}

	// set node with its sizes data
	void set(node *branch)
	{
		size new_size;
		if (branch->name.compare("H") != 0 && branch->name.compare("V") != 0) // operand
		{
			// find correct module from modules data
			for (int i = 0; i < entries.size(); i++)
			{
				if (entries[i].name.compare(branch->name) == 0)
				{
					// creates sizes data for node
					new_size.width = entries[i].width;
					new_size.height = entries[i].height;
					new_size.width = format(new_size.width, 1);
					new_size.height = format(new_size.height, 1);
					branch->sizes.push_back(new_size);
					new_size.width = entries[i].height;
					new_size.height = entries[i].width;
					new_size.width = format(new_size.width, 1);
					new_size.height = format(new_size.height, 1);
					branch->sizes.push_back(new_size);
					i = entries.size();
				}
			}
		}
		else // operator
		{
			if (branch->name.compare("H") == 0)
			{
				for (int i = 0; i < branch->left->sizes.size(); i++)
				{
					for (int j = 0; j < branch->right->sizes.size(); j++)
					{
						// new size will be maximum of the widths and sum of the heights
						new_size.width = std::max(branch->left->sizes[i].width, branch->right->sizes[j].width);
						new_size.height = branch->left->sizes[i].height + branch->right->sizes[j].height;
						new_size.width = format(new_size.width, 1);
						new_size.height = format(new_size.height, 1);
						branch->sizes.push_back(new_size);
					}
				}
			}
			else if (branch->name.compare("V") == 0)
			{
				for (int i = 0; i < branch->left->sizes.size(); i++)
				{
					for (int j = 0; j < branch->right->sizes.size(); j++)
					{
						// new size will be maximum of the heights and sum of the widths
						new_size.height = std::max(branch->left->sizes[i].height, branch->right->sizes[j].height);
						new_size.width = branch->left->sizes[i].width + branch->right->sizes[j].width;
						new_size.width = format(new_size.width, 1);
						new_size.height = format(new_size.height, 1);
						branch->sizes.push_back(new_size);
					}
				}
			}
		}
		cleanup(branch);
	}

	// navigate through tree and propagate up to root
	void traverse(std::string opera, node *branch)
	{
		if (branch != NULL)
		{
			traverse(opera, branch->left);
			traverse(opera, branch->right);
			nodes.push_back(branch->name);
			set(branch);
		}
	}

	// normal polish expression
	void expression(bool initial)
	{
		if (initial)
		{
			for (int i = 0; i < nodes.size(); i++)
				print(nodes[i]);
			nodes.clear();
		}
		else
		{
			for (int i = 0; i < E_best.size(); i++)
				print(E_best[i]);
			E_best = {};
		}
	}

	// format double to a specified decimal precision
	double format(double &value, const uint32_t &to)
	{
		uint32_t places = 1, whole = *(&value);
		for (uint32_t i = 0; i < to; i++) places *= 10;
		value -= whole;
		value *= places;
		value = round(value);
		value /= places;
		value += whole;
		return(value);
	}

	// find smallest area of modules bounding rectangle
	double solution()
	{
		double minimum = DBL_MAX;
		double area;
		for (int i = 0; i < root->sizes.size(); i++)
		{
			area = root->sizes[i].width * root->sizes[i].height;
			if (area < minimum)
				minimum = area;
		}
		return minimum;
	}
};