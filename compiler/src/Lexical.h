#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include "globals.h"
#include "SymbolTable.h"


class Lexical
{
public:
	Lexical(const std::string &filename);

private:
	std::ifstream is;
	char peek;
	uint32_t line;

	char* buffer1;
	char* buffer2;

	uint16_t buffer1_count;
	uint16_t buffer2_count;

	bool buffer_switch; // buffer1 - true, buffer2 - false


	SymbolTable sTable;
	

	// Utilities
	bool checkExtension(const std::string& filepath);
	void readFileToBuffer(std::ifstream& is, char* buf);


	bool nextToken(); // bool: true- continue, false- eof/exit

	void readNextChar();
	bool readNextChar(char c); // this does the peaking for specific grammar


};

