#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid):m_pid(pid){};

// TODO: Return this process's ID
int Process::Pid() 
{ 
  return m_pid;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() 
{ 
   auto result =
      ((float)LinuxParser::ActiveJiffies(m_pid) / 		                       				LinuxParser::ClkTPS());

   cpu = result / UpTime();
   return cpu;
}

// TODO: Return the command that generated this process
string Process::Command() 
{ 
  return LinuxParser::Command(m_pid);  
}

// TODO: Return this process's memory utilization
string Process::Ram() 
{ 
  return LinuxParser::Ram(m_pid); 
}

// TODO: Return the user (name) that generated this process
string Process::User() 
{ 
  return LinuxParser::User(m_pid);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() 
{ 
   return LinuxParser::UpTime(m_pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const 
{ 
  return (cpu > a.cpu);
}