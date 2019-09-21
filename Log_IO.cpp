/*
	Author:		Mihir K Patel
	Purpose:	Following is a implementation file for the LogIO.h. This contains function defintions
			for those in its header file. Main properties of this file are:
				1) To create a log file with users names.
				2) Take user names.
				3) To greet the users.
				4) To make user-specific log file
*/
#include "Log_IO.h"
LogIO::LogIO() { /*Simple constructor*/
	original_user_name = "";
	log_file_name = "";
}
/*Simple setter*/
void LogIO::set_name(std::string name_para) { /*Set the name in correct format*/
	name_para.erase(std::remove_if(name_para.begin(), name_para.end(), ::isspace), name_para.end()); /*Removes the spaces*/
	transform(name_para.begin(), name_para.end(), name_para.begin(), ::tolower); /*Lowers the letters*/
	*user_name = name_para;
	set_file_name(user_name);
}
/*Writes the names to file*/
void LogIO::write_user_names(std::string user_name_para) {
	std::ofstream name_file; ///--------->>>Want to make the users.txt, currently premade one exist
	try {
		/*t makes it refrence to current time, then *tm ptr get the time out of the system time.*/
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);

		name_file.open("users.txt", std::ios::out | std::ios::app);
		if (name_file.fail()) { throw ("Name write failed."); }
		/*Prints the date::time, and then tab then name and then new line in a file*/
		name_file << (now->tm_mon + 1) << '/' << (now->tm_mday) << '/' << (now->tm_year + 1900) << "::" << (now->tm_hour) << ':' << (now->tm_min) << ' ';
		name_file << user_name_para << std::endl;

		name_file.close();
	}
	catch (const std::exception & e) { std::cerr << e.what() << std::endl; } ///------>>Check these since these are not stl
}
/*Writes the question to the file*/
void LogIO::write_search(std::string search_para, std::string log_file_name_para) {
	try {
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		custom_file.open(log_file_name_para, std::ios::out | std::ios::app); //name_missing
		if (custom_file.fail()) { throw ("Search write failed."); }
		custom_file << (now->tm_mon + 1) << '/' << (now->tm_mday) << '/' << (now->tm_year + 1900) << "::" << (now->tm_hour);
		for (int x = 0; x <= 80; ++x) { custom_file << '.'; }
		search_para[0] = std::toupper(search_para[0]);
		custom_file << '\n' << search_para << '?' << std::endl; //put time and dashes in another mini function before calling	
		custom_file.close();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
}
/*Writes the the search result into the user file*/
void LogIO::write_queries(std::string queries_para, std::string log_file_name_para) {
	try {
		custom_file.open(log_file_name_para, std::ios::out | std::ios::app); 
		if (custom_file.fail()) { throw ("Query write failed."); }
		custom_file << ' ' << ' ' << queries_para << std::endl;
		custom_file << std::endl;
		custom_file.close();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl; 
	}
}

/*Sees if user names is in the file, if yes, return 1*/
bool LogIO::check_returned_user(std::string user_name_para) {
	try {
		std::ifstream name_file;
		name_file.open("users.txt", std::ios::in);
		if (name_file.is_open()) {

			std::string line = "";
			std::string dummy = "";
			std::string name = "";
			while (std::getline(name_file, line)) {
				std::istringstream iss(line);
				if (iss >> dummy >> name) { if (name == user_name_para) { return true; } }
			}
			name_file.close();
		}
		else { throw ("Name read failed."); }
	}
	catch (const std::exception & e) { std::cerr << e.what() << std::endl; }
	return false;
}
/*istream take the input*/
std::istream& operator>> (std::istream & in, LogIO & L_IO) {

	std::string local_user_name = "";
	/*Draws/ slows the console output so the bar appears as if it is loading*/
	std::cout << KBBLU << "|||" << KNRM << std::flush; L_IO.sleepcp(1000);
	std::cout << KBBB << "            " << KNRM << std::flush; L_IO.sleepcp(1000);
	std::cout << KBBC << "            " << KNRM << std::flush; L_IO.sleepcp(0100);
	std::cout << KBBG << "            " << KNRM << std::flush; L_IO.sleepcp(0100);
	std::cout << KBBGR << "            " << KNRM << std::flush; L_IO.sleepcp(0100);
	std::cout << KBBR << "            " << KNRM << std::flush; L_IO.sleepcp(0010);
	std::cout << KBBY << "            " << KNRM << std::flush; L_IO.sleepcp(0010);
	std::cout << KBYEL << "|||" << KNRM << KNRM << std::flush << std::endl; L_IO.sleepcp(0010);

	do {
		try { /*Asking name with validation*/
			std::cout << "\nTo begin, please type your name: ";
			std::getline(in, local_user_name);
			if (local_user_name.length() == 0) { throw std::invalid_argument("Type your name first."); }
			else { break; }
		}
		catch (const std::exception & e) { in.clear(); std::cerr << e.what() << std::endl; }
	} while (true);

	L_IO.original_user_name = local_user_name;
	L_IO.set_name(local_user_name); /*Sets the user_name to correct format*/

	if (L_IO.check_returned_user(*(L_IO.user_name)) == true) { /*If the name match say Welcome*/
		std::cout << "Welcome back " << local_user_name << '!' << std::endl;
		L_IO.write_user_names(*(L_IO.user_name));
	}
	else {
		L_IO.write_user_names(*(L_IO.user_name));
		std::cout << "Hello! " << local_user_name << ". Glad you're here.\n";
	}
	return in;
}
/*ostream prints the good-bye message*/
std::ostream& operator<< (std::ostream & out, const LogIO & L_IO) {
	out << KBCYN << "\nThank you, " << L_IO.original_user_name << " for using this program today!\n" << KNRM;
	return out;
}
LogIO::~LogIO() { delete user_name; } //----->Do I need a copy constructor or copy assinment here???
