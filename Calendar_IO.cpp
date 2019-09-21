/*
	Author:		Mihir K Patel
	Purpose:	Following is a implementation file for the CalendarIO.h. This contains function defintions
			for those in its header file. Main properties of this file are:
				1) To do computation on the Date.
				2) To write a .csv format calendar to the folder, which is more readable than .txt file.
				3) Most importantly, to transfer the control from Main() using istream.
*/
#include "Calendar_IO.h"
/*Counter*/
short int CalendarIO::cycle_counter = 0;
/*Defaults constructor of CalendarIO*/
CalendarIO::CalendarIO() {
	month = 0;
	day = 0;
}
/*Guesses the next period*/
void CalendarIO::guess_the_day() {
	for (std::vector<int>::iterator itr = length_container.begin(); itr != length_container.end(); ++itr) {
		next_cycle_guess = next_cycle_guess + *itr;
		if (itr == length_container.end() - 1) {
			next_cycle_guess = (static_cast<int>(next_cycle_guess / cycle_counter)) - OFFSET_DOS;
		}
	}
}
/*This function shifts the date accordingly to guess_the_day() helper function*/
void CalendarIO::adjust_day_month(int* days_arr, short int remaining_days) {
	try {
		if (days_arr != nullptr) {
			for (int x = 0; x <= remaining_days; ++x) { /*For loop runs until the remaining days are added to the day*/
				if (day < days_arr[month - 1]) {
					++day;
				}
				else if ((day == days_arr[month - 1]) && (month < 12)) {
					day = 1;
					++month;
				}
				else if ((day == days_arr[month - 1]) && (month == 12)) {
					day = 1;
					month = 1;
				}
				else {
					throw ("");
				}
			}
		}
		else { throw (""); }
	}
	catch (...) {
		std::cerr << "Date shifting failed.\n" << std::endl;
	}
}
/*Writes the .csv formatted calendar to a folder*/
void CalendarIO::write_calendar(CalendarIO & C_IO, std::string * months_arr) {
	try {
		srand((unsigned int)time(nullptr));
		int rand_quote = 1 + rand() % 7;
		std::ofstream outputFile("VaccationPlanner.csv");
		if (outputFile.is_open() && (months_arr != nullptr)) {
			/*2D vector creates a grid where months are in column and days in rows*/
			std::vector<std::vector<std::string>> cal_mat(32, std::vector<std::string>(13));
			/*This creates the rows and columns*/
			for (size_t x = 0; x < 32; x++) { /*{Prints the rows*/
				for (size_t y = 0; y < 13; y++) { /*Prints the columns*/
					if ((x == 0) && (y > 0)) cal_mat[x][y] = "  " + months_arr[y - 1]; /*Prints months*/
					else if ((y == 0) && (x > 0)) cal_mat[x][y] = std::to_string(x); /*Prints the numbers*/
					else cal_mat[x][y] = " ";
					cal_mat[C_IO.day][C_IO.month] = "       X,"; /*Marker to give visual representation of expected date*/
					outputFile << cal_mat[x][y] + ","; /*Comma seperates the columns*/
				}
				outputFile << std::endl;
			}
			outputFile << C_IO.quotes_array[0] << std::endl << C_IO.quotes_array[rand_quote]; /*Prints random quote in the end*/
			outputFile.close();
		}
		else { throw (""); }
	}
	catch (...) {
		std::cerr << "File writing failed.\n" << std::endl;
	}

}
/* Output the next date using ostream overridden virtual functions*/
std::ostream& CalendarIO::print_out(std::ostream & out) const {
	out << KBCYN << "\nREMINDER!" << KNRM << std::endl << "->Next Period will be on: " << get_month() << "/" << get_day() << '\n';
	out << "->On average, Menstrual Cycle would last " << (get_next_cycle_guess() + OFFSET_DOS) << " days." << '\n';
	return out;
}
/*This istream takes in all of the necessary input from the user to give his closed-by/her next menstrual date*/
std::istream & operator>> (std::istream & in, CalendarIO & C_IO) {
	char choice = ' ';
	std::string day_s = "";
	std::string cycle_len_s = "";
	for (;;) {
		try {
			std::cout << "Enter first day of your or your relative's last Period (mm/dd) or (mm-dd): ";
			std::getline(in, day_s);
			/*Takes the string of date, do the operation in its substring*/
			C_IO.month = std::stoi(day_s.substr(0, 2));
			C_IO.day = std::stoi(day_s.substr(3, 2));
			/*Input validation for the day and month*/
			if ((C_IO.month >= 1 && C_IO.month <= 31) && !isdigit(C_IO.month)) {
				if ((C_IO.day >= 1 && C_IO.day <= 31) && !isdigit(C_IO.day) && (C_IO.month == 1 || C_IO.month == 3 || C_IO.month == 5
					|| C_IO.month == 7 || C_IO.month == 8 || C_IO.month == 10 || C_IO.month == 12)) {
					break;
				}
				else if ((C_IO.day >= 1 && C_IO.day <= 30) && !isdigit(C_IO.day) && (C_IO.month == 4 || C_IO.month == 6 || C_IO.month == 9
					|| C_IO.month == 11)) {
					break;
				}
				else if ((C_IO.day >= 1 && C_IO.day <= 28) && !isdigit(C_IO.day) && (C_IO.month == 2)) { break; }
				else if (C_IO.day == 29 && C_IO.month == 2 && !isdigit(C_IO.day)) { break; }
				else { throw (""); }
			}
			else { throw (""); }
		}
		catch (...) {
			std::cerr << KBRED << "Invalid Date entry. Kindly try again." << KNRM << std::endl;
			in.clear();
		}
	}
	/*Asks for the Menstrual Cycle days, with its normal input validation*/
	for (;;) {
		try {
			std::cout << "What is the usual duration (in days) between monthly Periods: ";

			while (true) {
				std::getline(in, cycle_len_s);
				C_IO.cycle_length = std::stoi(cycle_len_s.substr(0, 2));
				if (!isdigit(C_IO.cycle_length) && !(C_IO.cycle_length >= 20 && C_IO.cycle_length <= 40)) { throw(""); }

				C_IO.length_container.push_back(C_IO.cycle_length);
				CalendarIO::cycle_counter++;

				std::cout << "Would you like to give any more of Menstrual Cycles (days between periods)?";
				choice = Period::check_choice();
				if (tolower(choice) == 'y') {
					std::cout << "Ok, how many days did the Cycle before previous Cycle lasted?: ";
				}
				if (tolower(choice) == 'n') { break; }
				in.ignore();
			}
			break;
		}
		catch (...) {
			in.clear();
			std::cerr << KBRED << "An abnormal cycle given or entry is invalid. If former, contact your primary doctor; else re-try!" << KNRM << std::endl;
		}
	}
	/*Leap year passes the array container which is used by the adjusted_day_month() function*/
	std::cout << "Is this a Leap year?";
	choice = Period::check_choice();

	if (tolower(choice) == 'y') {
		int leap_array[] = { 31,29,31,30,31,30,31,31,30,31,30,31 };
		C_IO.guess_the_day();
		short int days_adjustment = C_IO.get_next_cycle_guess() % 366; /*Adjust the limiting condition for a for-loop and passes it below*/
		C_IO.adjust_day_month(leap_array, days_adjustment);
	}
	else if (tolower(choice) == 'n') {
		int not_leap_array[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
		C_IO.guess_the_day();
		short int days_adjustment = C_IO.get_next_cycle_guess() % 365;
		C_IO.adjust_day_month(not_leap_array, days_adjustment);
	}
	/*Self explanitpry, check choice, if y, write the calendar*/
	std::cout << "Finally, would you like to get a Calendar with expected Period starting date?";
	choice = Period::check_choice();
	in.ignore();

	if (tolower(choice) == 'y') {
		CalendarIO::write_calendar(C_IO, C_IO.months_array);
		std::cout << KBYEL << "\nA CSV file 'VacationPlanner' has been made in patel_mihir_FP2 folder."
			<< "Don't forget to enable the Gridlines option in Page Layout Option when printing." << KNRM << std::endl;
	}
	return in;
}

CalendarIO::~CalendarIO() { }
