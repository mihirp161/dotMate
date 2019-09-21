/*
	Author:		Mihir K Patel
	Purpose:	Following is an Abstract parent class of CalendarIO.h, and Menstrual_Guide.h and a Header file for the Period.cpp.
			This class provides important data  members and member functions which are mind of this OOP project.
			Some of the immportant member functions are fix_user_string(string) which removes the numbers and punctuation marks;
			check_choice(), checks the entry of Y or N, distance(int, int, int), a helper-recursive function which calculates the
			minimum for array values passed in until the condition in the did_you_mean_this(string, string, size_t, size_t) function
			return the total numbers of edits. did_you_mean_this(string, string, size_t, size_t) is the edit distace algorithm,
			which finds the minimum distance for strings the user put in, and the string that iterator point to in map in subclass
			Menstrual_guide.h. Lastly there is a pure virtual ostream>> operator which gives each class an ability to
			print their own local ostream>> using Period.h's data members.
*/
#ifndef DOT_MATE_A_GUIDE_TO_MENSTRUATION_PERIOD
#define DOT_MATE_A_GUIDE_TO_MENSTRUATION_PERIOD
/*These are ANSI color schemes*/ /***CLANG_SPECIFIC***/
#define KNRM  "\x1b[0m"
#define KBRED  "\x1b[91m"
#define KBGRN  "\x1b[92m"
#define KBYEL  "\x1b[93m"
#define KBMAG "\x1b[95m"
#define KBBLU "\x1b[94m"
#define KBCYN "\x1b[96m"
#define KBBR  "\x1b[101m"
#define KBBG "\x1b[102m"
#define KBBY "\x1b[103m"
#define KBBB  "\x1b[104m"
#define KBBC "\x1b[106m"
#define KBBW "\x1b[107m"
#define KBBGR "\x1b[100m"
/*These are ANSI text effects*/ /***CLANG_SPECIFIC***/
#define TBOLD "\x1b[1m"
#define TUND "\x1b[4m"
#define TUNDO "\x1b[24m"
#define TOFF "\x1b[0m"
/*Global constants, and a sentinal- exit*/
#define CALCULATOR  "calculator"
#define CALCULATE "calculate"
#define EXIT "exit"
#define CALENDAR "calendar"
#define COMPUTE "compute"
#define HOLIDAYS "holidays"
#define HOLIDAY "holiday"
#define VACATION "vacation"
#define BREAK "break"
#define TRIP "trip"
#define TOUR "tour"
/*Appropriate overhead that supply C++ functions to one or more classes*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <limits>
#include <exception>
#include <ctime>

class Period {
protected:
	short int cycle_length; /* Stores the Menstrual cycle length*/
	short int next_cycle_guess; /*Gives the result after calculations*/
	std::string data_string; /*This stores the value string from the map*/
	std::string key_word_string; /*This stores the key string from the map*/
public:
	std::vector<int> length_container; /*Contains all of the lengths, if user decides to put in*/

	Period();

	int get_next_cycle_guess() const { return next_cycle_guess; } /*Getter for next_cycle_guess which returns the date expected date need to be shifted*/
	
	static std::string fix_user_string(std::string user_string_para); /*Function prototype to set string to letters only*/
	static char check_choice(); /*Input validation prototypes for yes or no*/
	bool operator() (std::string& str_para) { return std::all_of(str_para.begin(), str_para.end(), ::isalpha); } /*Overloded check for letters in string*/

	int distance(size_t upper, size_t diagonal, size_t lower_right); /*Helper function using recursion- a prototype.*/
	int did_you_mean_this(std::string itr_para, std::string to_look_para, size_t itr_len_para, size_t to_look_len_para); /*Edit algorithmn prototype*/

	virtual std::ostream& print_out(std::ostream& out) const = 0;/*Pure virtual function prototype*/
	friend std::ostream& operator<< (std::ostream& left, const Period& right) { /*A friended helper function for this pure virtual ostream&*/
		return right.print_out(left);
	}
	~Period();
};
#endif // !DOT_MATE_A_GUIDE_TO_MENSTRUATION_PERIOD
