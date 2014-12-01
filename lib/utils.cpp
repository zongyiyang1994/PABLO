#include <sys/stat.h>
#include <string>
#include "utils.hpp"

// Function: fileExists
/**
	Check if a file exists

@param[in] filename - the namee of the file to check

@return true if the file exists, false otherwise
**/

bool fileExists(const std::string& filename) {
	struct stat buf;

	return stat(filename.c_str(), &buf) != -1; 
}