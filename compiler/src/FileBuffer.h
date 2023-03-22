#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "globals.h"

class FileBuffer
{
public:
	FileBuffer();
	FileBuffer(const std::string &filename);

	void append(const std::string& value);
	void append(const char* value);
	void finish();


private:
	std::ofstream _os;
	std::string _fileName;


	std::string _buffer = ""; // buffer size 2048
	uint16_t _bufferSizeLimit = 2048;

	// Utilities
	virtual bool checkExtension(const std::string& filepath);

};

