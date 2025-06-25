/*#ifndef CRYPTION_HPP
#define CRYPTION_HPP

#include <string>

int executeCryption(const std::string& taskData);

#endif*/

#ifndef CRYPTION_HPP
#define CRYPTION_HPP

#include <string>
#include <sodium.h>   // Add this

int executeCryption(const std::string& taskData);

#endif
