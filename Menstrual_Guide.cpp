/*
	Author:		Mihir K Patel
	Purpose:	Following is a implementation file for the Menstrual_Guide.h. This contains function defintions
			for those in its header file. Main properties of this file are:
				1) To give user a string from map, when he/she asks a question.
				2) To give user an ability to interact with whole program using statements (strings).
				4) To load the text file into map, so search results can occur
				5) Lastly, to display program logo, and few tips in the beginning.
*/
#include "Menstrual_Guide.h"
/*Default constructor and a simple getter*/
Menstrual_Guide::Menstrual_Guide() {
	fuzzy_string = "";
	user_focused_file = "";
}
std::string Menstrual_Guide::get_archive_answer() const {
	return archive_answer;
}
/*Handler for the edit distance algorithmn in Period class*/
std::string Menstrual_Guide::search_archive(std::string to_look_para) {

	load_information(); /*Loads the map at each call.*/
	std::stringstream ss; /*String stream for removing spaces*/
	std::string close_match_key = "";
	std::string close_match_value = "";
	std::string temp_data = "";
	std::string temp_key = "";
	std::string original_string = to_look_para;
	original_string = Period::fix_user_string(original_string);/*Removes the numbers and punctuation marks*/

	ss << to_look_para; /* to_look_para gets loaded into string stream*/
	to_look_para = "";
	temp_data.clear();
	while (!ss.eof()) { /*This loop copies the ss info back to temp_data without any spaces*/
		ss >> temp_data;
		to_look_para = to_look_para + temp_data;
	}
	temp_data.clear();
	ss.clear();

	data_string.clear();
	key_word_string.clear();
	data_string = "";

	int levenshtein_distace = 0;
	/*This for loop iterates through the map, comparing the user string to the information in the map*/
	for (std::map<std::string, std::string>::iterator itr = information_map.begin(); itr != information_map.end();) {

		int* ptr = &levenshtein_distace;

		ss << itr->first; /*Key from map gets loaded into ss, and gets the space removed*/

		key_word_string = "";
		while (!ss.eof()) {
			ss >> temp_key;
			key_word_string = key_word_string + temp_key;
		}
		temp_key.clear();
		ss.clear();
		/*These return the length of the function*/
		int curr_itr_length = strlen(key_word_string.c_str());
		int to_look_length = strlen(to_look_para.c_str());
		/*Pointers then equals to the number of edits made*/
		*ptr = did_you_mean_this(key_word_string, to_look_para, curr_itr_length, to_look_length);
		/*If edits are closed, we print Value associated with its Key*/
		if (*ptr >= 0 && *ptr < 6) {
			temp_data = itr->second;
			L_IO.write_search(itr->first , user_focused_file); /*Writes the history in user specific text files*/
			L_IO.write_queries(temp_data , user_focused_file);
			return temp_data;
		}
		else if (*ptr >= 6 && *ptr <= 10) { /*Gets the second most closest result/s and store them in the other map*/
			close_match_key = itr->first;
			close_match_key[0] = std::toupper(close_match_key[0]);
			close_match_value = itr->second;
			other_possible_results[close_match_key] = close_match_value;
		}
		*ptr = 0;
		++itr;
	}
	temp_data = original_string; /*If no closest match occur, return the original user string*/
	return temp_data;
}
/*This function reads from the text file with a delimiter and loads them into a map*/
void Menstrual_Guide::load_information() {
	std::ifstream input_file("Menstrual_archive.txt", std::ios::in);

	if (input_file.is_open()) {
		std::string local_line;
		while (std::getline(input_file, local_line)) { /*Read line by line, and split them using delimiter*/
			std::istringstream iss(local_line);
			std::getline(iss, key_word_string, '\t');
			std::getline(iss, data_string, '\n');
			information_map[key_word_string] = data_string; /*Store unique Keys and its associated unique map*/
		}
	}
	else {
		std::cout << KBRED << "Archive file is missing, place the text file in the folder.\n" << KNRM << std::flush;
		exit(0);
	}
	input_file.close();
}
/*Prints the title and few tips for the program*/
std::ostream& Menstrual_Guide::print_out(std::ostream& out) const {

	out << KBMAG << "     __  __           _______  ______ \n"
		"    |  \\/  |    /\\   |__   __||  ____|\n"
		"    | \\  / |   /  \\     | |   | |__  \n"
		"    | |\\/| |  / /\\ \\    | |   |  __| \n"
		"    | |  | | / ____ \\   | |   | |____ \n"
		" (O)|_|  |_|/_/    \\_\\  |_|   |______|\n" << KNRM;
	out << std::setw(13); out << TBOLD << TUND << "A GUIDE TO MENSTRUATION" << TUNDO << TOFF;
	out << "!\n" << std::flush;
	out << TBOLD << "\nHello! At any time, you can type \"...exit...\" to end .MATE program.\n" << TOFF
		<< TUND << "\nFor a tailored experience make sure you are doing the following" << TUNDO << TOFF;
	out << ": " << std::flush;
	out << "\n[1] To check next Period, type, for e.g., \"We are planning for a vacation....\".\n"
		<< "[2] To find any Menstrual related information please type a question, for e.g., \"What is....\".\n"
		<< "[3] Use concise and complete sentences.\n"
		<< "[4] Avoid typing multiple questions into one sentence.\n"
		<< "[5] Avoid excessive spelling errors. Double checking will lead to better result.\n"
		<< "[6] Please use only words/letters for your queries.\n"
		<< "[7] Put punctuations only where they are necessary.\n"
		<< "[8] To report crashes, or any errors, please send them at mihirp161@gmail.com\n\n"
		<< TBOLD << "Thanks in advance and I hope you enjoy using .MATE today!\n" << TOFF << std::endl;

	return out;
}
/*This function takes the input, makes program flow in a circular manner */
std::istream& operator>> (std::istream& in, Menstrual_Guide& M_G) {
	char choice = 'y';
	std::string temp = "";
	std::string attempt_this = "";

	while (tolower(choice) == 'y') {
		M_G.archive_answer.clear();
		M_G.key_word_string.clear();
		temp.clear();
		M_G.fuzzy_string.clear();

		std::cout << std::flush << "\nWhat would you like to ask?: " << std::flush;
		std::getline(in, temp);
		attempt_this = temp;

		M_G.fuzzy_string = Period::fix_user_string(temp); /*Remove everything but only keep the letters into a string*/
		transform(M_G.fuzzy_string.begin(), M_G.fuzzy_string.end(), M_G.fuzzy_string.begin(), ::tolower); /*Turns characters to lower case*/
		M_G.archive_answer = M_G.search_archive(M_G.fuzzy_string); /*Stores the Value returned from the function above*/

		temp.clear();
		temp = M_G.archive_answer;
		transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
		/*strstr is a prebuilt C/C++ function that chops the string into word and compares each word with speical sentinals*/
		if ((strstr(temp.c_str(), CALENDAR)) || (strstr(temp.c_str(), CALCULATOR)) || (strstr(temp.c_str(), CALCULATE)) ||
			(strstr(temp.c_str(), EXIT)) || (strstr(temp.c_str(), COMPUTE)) || (strstr(temp.c_str(), VACATION)) ||
			(strstr(temp.c_str(), HOLIDAY)) || (strstr(temp.c_str(), HOLIDAYS)) || (strstr(temp.c_str(), TOUR)) ||
			(strstr(temp.c_str(), BREAK)) || (strstr(temp.c_str(), TRIP)) || (strstr(temp.c_str(), VACATION))) {
			break; /*Sends the control back to main upon catching a keyword*/
		}
		else {
			if (M_G.fuzzy_string == temp || temp.length() <= 100) { /*If the orginal string match with the return, we display this match*/
				std::cout << KBYEL << "\nSorry, no suitable result to show at the moment." << KNRM << std::flush;

				if (M_G.other_possible_results.size() >= 0) {
					if (!M_G(attempt_this)) { /*Overloaded operator/functor sees if there are anything else other than letters in the user string*/
						std::cout << KBCYN << "\nAn attempt was made to fix your input! Your input matters for a better result.\n" << KNRM;
					}
					for (auto& opr : M_G.other_possible_results) { /*Prints the second most matches*/
						std::cout << '\n' << opr.first << '?' << '\n' << opr.second << std::endl;
					}
					M_G.other_possible_results.clear();/*Clears the second buffer*/
				}

				std::cout << KBYEL << "\nWould you like you try again?" << KNRM << std::flush;
				M_G.fuzzy_string.clear();
				choice = Period::check_choice();  /*Gives y or n prompts*/
				in.ignore();

				if (tolower(choice) == 'n') { break; }
			}
			else {
				std::cout << KBGRN << '\n' << M_G.archive_answer << KNRM << std::endl;
			}
		}
	}
	return in;
}

Menstrual_Guide::~Menstrual_Guide() { }
