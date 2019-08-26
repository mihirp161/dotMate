/*
	Author:		Mihir K Patel
	Purpose:	Following is a Header file for the Menstrual_Guide.cpp. This inherits a parent class named Period.h
				This contains function protypes. Few of the main prototypes are load_information() which reads the
				text containing info, and loads them into a map data member. Then there is, search_arhive(string) which is a
				part 2 of the main algorithm that gets a user his search results back. Lastly, we have a friend
				function using an overloaded istream& that allows input for data members of this class, and then a
				pure virtual overriden ostream << that prints aesthetics of this problem.
*/
#ifndef DOT_MATE_A_GUIDE_TO_MENSTRUATION_MENSTRUAL_GUIDE
#define DOT_MATE_A_GUIDE_TO_MENSTRUATION_MENSTRUAL_GUIDE
#define OFFSET_UNO 1
/*Class specific headers*/
#include "Period.h"
#include <iomanip>
#include <cctype>
#include <map>
#include <cstring>
#include "Log_IO.h"
class Menstrual_Guide :public Period
{
private:

	std::map<std::string, std::string> information_map; /*Stores the file data*/
	std::map<std::string, std::string> other_possible_results; /*Stores closest results that weren't found*/

	LogIO L_IO; /*Composition object to give writing functions*/
	std::string fuzzy_string; /*Holds the user search queries*/
	std::string archive_answer; /*Stores the value from Key-Value map*/
	std::string user_focused_file;  /*Custom file for each user*/

	std::string search_archive(std::string to_look_para); /*Part 2 of the searching function. This uses the did_you_mean_this() function*/
	void load_information();/*Prototypes for function that loads everything into a map*/
public:
	Menstrual_Guide();

	void set_user_focused_file(std::string focused_file_para) { user_focused_file = focused_file_para; }
	std::string get_archive_answer() const;	/*Simple getter to get the value*/

	friend std::istream& operator>> (std::istream&, Menstrual_Guide&);/*Friend overloaded istream operator prototype*/
	virtual std::ostream& print_out(std::ostream& out) const override; /*Overridden ostream virtual function prototype*/
	~Menstrual_Guide();
};
#endif // !DOT_MATE_A_GUIDE_TO_MENSTRUATION_MENSTRUAL_GUIDE