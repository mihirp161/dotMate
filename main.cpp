/*
	Author:		Mihir K. Patel (COP 3331-s)
	Purpose:	Following program is meant to be serve as an information tool for Menstruation. The user
				has an offline repository where he/she can find information about that. User may also
				calculate her/his-relative's Period and get a calendar for a visual reminder.
				This program is NOT IN ANY WAY exclusively made for particular audience, everyone is more
				than welcome to use it!

				This is a Driver file, it is dependant on CalendarIO.h, and Menstrual_Guide.h
*/
#include "Menstrual_Guide.h" 
#include "Calendar_IO.h"
#include "Log_IO.h"
int main()
{
	CalendarIO cal_io; /*Class objects*/
	Menstrual_Guide menstrual_guide;
	LogIO log_io;

	try
	{
		std::cout << menstrual_guide; /*Virtual function allows to output*/
		std::cin >> log_io;
		menstrual_guide.set_user_focused_file(log_io.get_log_file_name());

	beginning:
		std::string guide_response = "";
		guide_response.clear();
		std::cin >> menstrual_guide; /*Overloaded istream allows the input from the user*/

		guide_response = menstrual_guide.get_archive_answer(); /* Sets the answer here and sees if any unique transfer keys are present*/
		transform(guide_response.begin(), guide_response.end(), guide_response.begin(), ::tolower);

		while (!(strstr(guide_response.c_str(), EXIT))) {
			if ((strstr(guide_response.c_str(), CALENDAR)) || (strstr(guide_response.c_str(), CALCULATOR)) ||
				(strstr(guide_response.c_str(), CALCULATE)) || (strstr(guide_response.c_str(), HOLIDAY)) ||
				(strstr(guide_response.c_str(), COMPUTE)) || (strstr(guide_response.c_str(), HOLIDAYS)) ||
				(strstr(guide_response.c_str(), TOUR)) || (strstr(guide_response.c_str(), BREAK)) ||
				(strstr(guide_response.c_str(), TRIP)) || (strstr(guide_response.c_str(), VACATION))) {
				std::cin >> cal_io; /*If any keywords are present related to CalendarIO class, CalendarIO gets invoke*/
				std::cout << cal_io;  /*Prints the Period date*/
			}
			else {
				std::cout << TBOLD << "\nPlease type \"...exit...\" to end this program." << TOFF << std::endl;
			}
			goto beginning; /*Jumps in the beginning to allow circular interaction*/
		}
		std::cout << log_io;
		std::cout << TBOLD << "\nLeave feedback/comments to mkpatel@mail.usf.edu for any improvements :)\n"
			<< "For collaboration use the email above. If you would like to reuse any code, feel free to do so!\n" << TOFF << std::endl;
	}
	catch (...) { 
		std::cerr << "Please report this crash to mkpatel@mail.usf.edu. In the email do tell what have you done "
			<< "prior to the program crash. Sorry for this inconvenience.\n";
		exit(0);
	}

	return 0;
}