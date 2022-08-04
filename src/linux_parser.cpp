#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stol;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
    
    filestream.close();
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string value, token;
  float memtotal=0.0, memfree=0.0, memavlble=0.0;
  string line;
  bool bfirst=false;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) 
  {
    while(std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> token >> value;
      if(!bfirst)
          //first line
          memtotal = stoi(value);
      else
      {
          memfree = stoi(value);
          break;
      }
      bfirst = true;
    }
    
    stream.close();
  }
  
  memavlble = (memtotal-memfree)/memtotal;
  return memavlble;
 }

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string value;
  long uptime=0.0;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open())
  {
    std::getline(stream, line);
    std::istringstream ilinestream(line);
    ilinestream>>value;
    uptime = stoi(value);
    stream.close();
  }
  
  return uptime; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return (ActiveJiffies() - IdleJiffies()); 
}

// Read and return the number of active jiffies for a PID

long LinuxParser::ActiveJiffies(int pid) 
{ 
    std::string filename = kProcDirectory + to_string(pid) + 								   kStatFilename;
  
    std::vector<string> vstr;

    std::string line, value;
    std::ifstream stream(filename);

  	if (stream.is_open()) 
    {
    	std::getline(stream, line);

    	std::istringstream linestream(line);
    	while (linestream >> value) 
          vstr.push_back(value);
        stream.close();
  	}
  	
	if (vstr.size() >= 21)
    	return (stol(vstr[13]) + stol(vstr[14]) + stol(vstr[15]) + 					stol(vstr[16]));

  	return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
 
  const vector<string> cpu = LinuxParser::CpuUtilization();
  const long kuser = stol(cpu[LinuxParser::kUser_]);
  const long knice = stol(cpu[LinuxParser::kNice_]);
  const long ksystem = stol(cpu[LinuxParser::kSystem_]);
  const long kidle = stol(cpu[LinuxParser::kIdle_]);
  const long kiowait = stol(cpu[LinuxParser::kIOwait_]);
  const long kirq = stol(cpu[LinuxParser::kIRQ_]);
  const long ksoftirq = stol(cpu[LinuxParser::kSoftIRQ_]);
  const long ksteal = stol(cpu[LinuxParser::kSteal_]);

  return (kuser + knice + ksystem + kidle +
          kiowait + kirq + ksoftirq + ksteal);

}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 

  const vector<string> cpu_stats = LinuxParser::CpuUtilization();
  const long kidle = stol(cpu_stats[LinuxParser::kIdle_]);
  const long kiowait = stol(cpu_stats[LinuxParser::kIOwait_]);

  return (kidle + kiowait);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 

  std::vector<string> result;
  std::string line, val;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) 
  {
    getline(stream, line);

    std::istringstream linestream(line);
    linestream >> val;

    for (int ii = 0; ii < CPUStates::kGuestNice_; ii++) 
    {
      linestream >> val;
      result.push_back(val);
    }
  }

  stream.close();
  return result;

}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string value, token;
  string line;
  int proc_count=0;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      if(line.substr(0, processes.size()) == processes)
      {
        std::istringstream ilinestream(line);
        ilinestream>>token>>value;
        proc_count = stoi(value);
        break;
      }
    }
    stream.close();
  }
  return proc_count; 

}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
 string value, token;
  string line;
  int proc_count=0;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open())
  {
    while(std::getline(stream, line))
    {
      if(line.substr(0, procs_running.size()) == procs_running)
      {
        std::istringstream ilinestream(line);
        ilinestream>>token>>value;
        proc_count = stoi(value);
        break;
      }
    }
    stream.close();
  }
  return proc_count; 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{ 

  std::string line;

  std::ifstream stream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);

  if (stream.is_open()) 
    getline(stream, line);

  stream.close();
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{ 
  
  const long mbMem = (ReadProcPID(pid, filterProcMem) / 1024);
  return to_string(mbMem);
  
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) 
{ 

  const long uid = ReadProcPID(pid, "Uid");
  return to_string(uid);

}

// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{ 
  
  std::string key = LinuxParser::Uid(pid);
  std::string line, uid_str, user;

  std::ifstream stream(kPasswordPath);

  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      while (linestream >> user >> uid_str) 
      {
        if (uid_str == key) 
          return user;
      }
    }
  }

  stream.close();
  return user;
  
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) 
{ 
   std::vector<string> vecTime;
   std::string line, value;
   std::ifstream stream(kProcDirectory + to_string(pid) + 			             kStatFilename);

  if (stream.is_open()) 
  {
    getline(stream, line);

    std::istringstream linestream(line);
    while (linestream >> value) 
      vecTime.push_back(value);
  }
  stream.close();

  if (vecTime.size() >= 21) 
  {
    const long starttime = stol(vecTime[21]);
    return UpTime() - (starttime / ClkTPS());
  }
  
  return 0;
}

int LinuxParser::ClkTPS() 
{ 
  return sysconf(_SC_CLK_TCK); 
}

long LinuxParser::ReadKeyFromFile(const std::string filename, const std::string &key) {
  std::string line, token, value;
  std::ifstream stream(filename);

  if (stream.is_open()) {
    while (getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      while (linestream >> token >> value) {
        if (token == key) return stol(value);
      }
    }
  }
  stream.close();
  return 0;
}

long LinuxParser::ReadProcPID(const int &pid, const std::string &key) {
  std::string name = kProcDirectory + std::to_string(pid) + kStatusFilename;
  return ReadKeyFromFile(name, key);
}