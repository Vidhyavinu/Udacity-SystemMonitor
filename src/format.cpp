#include <string>

#include "format.h"

using std::to_string;
using std::string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) 
{ 
  int hours, minutes;
  hours = seconds / 3600;
  seconds = seconds % 3600;
  minutes = seconds / 60;
  seconds = seconds % 60;
  return to_string(hours) + ":" + to_string(minutes) + ":" +
         to_string(seconds);
}