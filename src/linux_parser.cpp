#include <dirent.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <pwd.h>
#include <limits>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  double memtotal{1}, memavailable{};
  std::string key;

  std::ifstream filestream{kProcDirectory + kMeminfoFilename};
  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream{line};
      linestream >> key;
      if (key == "MemTotal:") {
        linestream >> memtotal;
      }

      if (key == "MemAvailable:") {
        linestream >> memavailable;
      }
    }

    filestream.close();
  }

  return (memtotal - memavailable) / memtotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  double uptime{};
  std::ifstream filestream{ kProcDirectory + kUptimeFilename };

  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    filestream.close();
    std::istringstream linestream{line};
    linestream >> uptime;
  }

  return uptime;
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  std::string cpu, usertime, nicetime, systemtime, idletime, iowaittime, irqtime, softirqtime, stealtime;
  std::ifstream filestream{kProcDirectory + kStatFilename};
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    filestream.close();
    std::istringstream linestream{line};
    linestream >> cpu >> usertime >> nicetime >> systemtime >> idletime
               >> iowaittime >> irqtime >> softirqtime >> stealtime;
  }
  return {usertime, nicetime, systemtime, idletime, iowaittime, irqtime, softirqtime, stealtime};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream filestream { kProcDirectory + kStatFilename };

  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream{line};
      std::string key, value;
      linestream >> key >> value;
      if (key.compare("processes") == 0) {
        return std::stoi(value);
      }
    }

    filestream.close();
  }

  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream{ kProcDirectory + kStatFilename };
  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream{line};
      std::string key, value;
      linestream >> key >> value;
      if (key.compare("procs_running") == 0) {
        return std::stoi(value);
      }
    }

    filestream.close();
  }

  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::string cmd{};
  std::ifstream filestream{kProcDirectory + std::to_string(pid) + kCmdlineFilename};
  if (filestream.is_open()) {
    std::getline(filestream, cmd);
    filestream.close();
  }

  return cmd;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::ifstream filestream{ kProcDirectory + std::to_string(pid) + kStatusFilename };
  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream{line};
      std::string key, value;
      linestream >> key >> value;
      if (key == "VmSize:") {
        return std::to_string(int(std::stod(value) / 1024));
      }
    }

    filestream.close();
  }

  return "0";
 }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::ifstream filestream{ kProcDirectory + std::to_string(pid) + kStatusFilename };
  if (filestream.is_open()) {
    std::string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream{line};
      std::string key, value;
      linestream >> key;
      if (key == "Uid:") {
        linestream >> value;
        return value;
      }
    }
    filestream.close();
  }
  
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  uid_t uid{static_cast<uid_t>(std::stoul(LinuxParser::Uid(pid)))};
  passwd* pw = getpwuid(uid);
  if (pw) {
    return std::string{pw->pw_name};
  }
  return std::string{};
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream{kProcDirectory + std::to_string(pid) + kStatFilename};

  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    filestream.close();
    std::istringstream linestream{line};

    std::string ignore;
    for (int i = 0; i < 21; i++) {
      //linestream.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
      linestream >> ignore;
    }

    unsigned long long starttime;
    linestream >> starttime;
    return LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
  }

  return 0;
}
