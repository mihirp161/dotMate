/*
	Author:		Mihir K Patel
	Purpose:	Following is a Header file for the LogIO.cpp. This is a composition class and has no inheritance.
			Period.h is imported only to supply few stadard libraries, and ANSI global variables. Purpose of
			this file is to get the names of the users, and to greet them back if they return.
*/
#ifndef DOT_MATE_A_GUIDE_TO_MENSTRUATION_LOGIO
#define DOT_MATE_A_GUIDE_TO_MENSTRUATION_LOGIO
#define _CRT_SECURE_NO_WARNINGS /*Suppresses the time warning. It is VS annoying thing*/
#include "Period.h"
#include <time.h>

#ifdef _WIN32
#include <windows.h> /*This gaurd makes the delay in the console cross-platform*/
#else
#include <unistd.h>
#endif // WIN32

class LogIO
{
private:
	std::string original_user_name;
	std::string log_file_name;
	std::string* user_name = new std::string(); /*Used for input*/

	bool check_returned_user(std::string user_name_para); /*Prototype to see if user has returned*/
	
	void set_name(std::string name_para); /*Prototype to set name in good format*/
	void set_file_name(std::string* user_name_para) { log_file_name = *(user_name_para)+"_logs.txt"; } /*Puts the file name*/

	static void write_user_names(std::string user_name_para); /*Prototype to write strings in correct format*/

	friend std::istream& operator>> (std::istream& in, LogIO& L_IO); /*Prototype for input*/
	friend std::ostream& operator<< (std::ostream& out, const LogIO& L_IO); /*Prototype for output*/
protected:
	/*Ostream writeup for user inqueries*/
	std::ofstream custom_file;
public:
	void write_queries(std::string queries_para, std::string log_file_name_para);
	void write_search(std::string search_para, std::string log_file_name_para);

	std::string get_log_file_name() { return log_file_name; } /*Getter for file log*/

	void sleepcp(int millisec) { /*This is a function rather than pre-built function from the header*/
#ifdef _WIN32             
		Sleep(millisec);/*Sleep() is a function of <windows.h> */
#else
		usleep(millisec * 1000); /*This is function of <unistd.h>*/
#endif //WIN32
	}

	LogIO(); /*Constructor, simple*/
	~LogIO();/*Destructor, simple*/
};
#endif // !DOT_MATE_A_GUIDE_TO_MENSTRUATION_LOGIO
