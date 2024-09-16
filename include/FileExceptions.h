#pragma once
#include <iostream>

class FileError :public std::exception
{
public:
	FileError() {};
	~FileError() {};
	const char* what() const noexcept {
		return "\nError ";
	}
};

class InvalidPath :public std::exception
{
public:
	InvalidPath() {};
	~InvalidPath() {};
	const char* what() const noexcept {
		return "\nInvalid file path: The path doesn't exist, try again!\n";
	}
};
