/*
	Author:		Mihir K Patel
	Purpose:	Following is an implementation file for the Period.h. This contains function defintions
				for those in its header file. Main properties of this file are:
					1) To apply prebuilt function of C++ to remove numbers and punctutations in a user string.
					2) To check y-n options with a local try_catch block.
					3) To have a helper function that uses recursion to return the min using C++ built-in function.
					4) Finally, an algorithmn that inserts, delete, or replace a character of two strings to make them equal.
*/
#include "Period.h"
/*Default constructor for Period class*/
Period::Period() {
	cycle_length = 0;
	next_cycle_guess = 0;
	data_string = "";
	key_word_string = "";
}
/*These functions are lambda expressions, the underlying function is built-in C++. And are
	imported using <string> overhead*/
std::string Period::fix_user_string(std::string user_string_para) {

	user_string_para.erase(std::remove_if(user_string_para.begin(),
		user_string_para.end(),
		[](unsigned char c) { return std::ispunct(c); }),
		user_string_para.end());

	user_string_para.erase(std::remove_if(user_string_para.begin(),
		user_string_para.end(),
		[](unsigned int i) { return std::isdigit(i); }),
		user_string_para.end());

	return user_string_para;
}
/*Invalid check for y or n input. Contains the exception to handle any error*/
char Period::check_choice() {
	char y_n_para = ' ';
	for (;;) {//---------->>>Just know that therer is a garbage ifyou put in yfghfhghg or ndfgfdgdfgd
		try {
			std::cout << "\nPlease specify your choice (Y or N): ";
			std::cin >> y_n_para;

			if (!std::cin.fail() && (tolower(y_n_para) != 'y') && (towlower(y_n_para) != 'n')) {
				throw std::invalid_argument("Wrong Choice given. Kindly try again.");
			}
			else { break; }
		}
		catch (const std::invalid_argument & p) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << KBRED << p.what() << KNRM << std::endl;
		}
	}
	return y_n_para;
}
/*Helper function for did_you_mean_this() function that returns the minimum value between the three parameters*/
int Period::distance(size_t upper, size_t diagonal, size_t lower_right) {
	return std::min(std::min(upper, diagonal), lower_right); /*Calls recursively*/
}
/*This is also known as Levenshtein Distance/Edit Distance. The basic functionality of this algorithmn is to
	find the minimum edits (insert, remove, replace a character) in two strings to make them equal*/
int Period::did_you_mean_this(std::string itr_para, std::string to_look_para, size_t itr_len_para, size_t to_look_len_para) {

	/*2-D dynamic container that puts header info on stack but allocates the elements on the heap*/
	std::vector<std::vector<size_t> > edits_grid(itr_len_para + 1, std::vector<size_t>(to_look_len_para + 1));

	/*Nested for loops, locks a character and then compares that chracter to every chracter of the second string*/
	for (size_t x = 0; x <= itr_len_para; ++x) {
		for (size_t y = 0; y <= to_look_len_para; ++y) {

			/*If the length of any string becomes null, set amount of edits made to make two strings equal*/
			if (x == 0) { edits_grid[x][y] = y; }
			else if (y == 0) { edits_grid[x][y] = x; }

			/*If last elements are same, don't do any operation, simply decrement the string*/
			else if (itr_para[x - 1] == to_look_para[y - 1]) {
				edits_grid[x][y] = edits_grid[x - 1][y - 1];
			}

			/*If characters at an index are different use the helper function to either do an Insert, Replace, or Remove so
				the current substring is equal to the other string, and add one, because an operation is performed*/
			else {
				edits_grid[x][y] = 1 + distance(edits_grid[x][y - 1], edits_grid[x - 1][y], edits_grid[x - 1][y - 1]);
			}
		}
	}
	return edits_grid[itr_len_para][to_look_len_para]; /*After recursion is over, simply return the number of edits*/
}

Period::~Period() { }