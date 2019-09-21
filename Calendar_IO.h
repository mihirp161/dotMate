/*
	Author:		Mihir K Patel
	Purpose:	Following is a Header file for the CalendarIO.cpp. This inherits a parent class named Period.h
			This contains function protypes. Few of the main prototypes are adjust_day_month(array, int), which
			shifts the date to give an accurate result for the next Period. A static function
			write_function(CalendarIO, array) which makes a CSV format calendar that reflects next Period with
			a marker. A virtual function of overridden ostream<< that prints the results displaying date change and
			expected date. Lastly, a friend function of istream>> that allows interaction with this class.
*/
#ifndef DOT_MATE_A_GUIDE_TO_MENSTRUATION_CALENDERIO
#define DOT_MATE_A_GUIDE_TO_MENSTRUATION_CALENDERIO
#define OFFSET_DOS 2
/*Class specific overhead*/
#include "Period.h"
#include <cstdlib>

class CalendarIO :public Period
{
private:
	static short int cycle_counter; /*Counter for cycle input*/
	short int month = 0; /*Holds the month*/
	short int day = 0; /*Holds the day*/

	std::string months_array[12]{ "January","February", "March", "April", "May", "June", "July",
								"August", "September", "October", "November", "December" };/*Months container*/
	/*Prints quote in the VacationPlanner.csv (comma-delimited)*/
	std::string quotes_array[8]{ "Today's, quotes:,",
								"Last time, one man, said to, a woman, that she, can't fly, a plane, then she, flew one, around, the world.,",
								"Men are, without, a  vagina, therefore, there's, a $20B, market, for the, Sanitary, Pads.,",
								"If you, want to, compare, genders, then let, me tell, you:  Wo,men are, not only ,competin,g they're, beating, you!,",
								"So you, were told, to shut u,p. To clos,e your eye,s & to po,p your ear,drums....,my questi,on is: Wi,ll you?,",
								"My body,can be, put on, chains., But my, soul will, never be, able to, comply.,",
								"God gave, us hands, to love, and to, work and, most im,portantly-, to slap, when the, need arise.,",
								"Duty for, my count,ry is first, but duty, for you, comes later.," };

	/*Pure function that guesses the next period days which would get added into date*/
	void guess_the_day();
	void adjust_day_month(int* days_arr, short int remaining_days); /*Offsets the date with next_cycle_guess*/

	static void write_calendar(CalendarIO&, std::string* months_arr); /*Prototype for writing a calendar to folder*/
public:
	CalendarIO();

	/*Simple getters to get the day and month*/
	int get_day() const { return day; }
	int get_month() const { return month; }

	virtual std::ostream& print_out(std::ostream& out) const override; /*Prototype for ostream virtual function*/
	friend std::istream& operator>> (std::istream&, CalendarIO&); /*Prototype for istream friend function*/
	~CalendarIO();
};
#endif // !DOT_MATE_A_GUIDE_TO_MENSTRUATION_CALENDERIO
