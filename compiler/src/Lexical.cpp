#include "Lexical.h"

Lexical::Lexical()
{
	this->is = std::ifstream("res/Test1.cp");

	if (!this->is.is_open()) {
		std::cout << "Failed to open file" << std::endl;
		return;
	}



	this->peek = ' ';
	this->line = 0;

	bool loop = true;

	while (loop) {
		loop = this->nextToken();


	}

}

Lexical::Lexical(const std::string& filename)
{
	this->is = std::ifstream(filename);

	if (!this->is.is_open()) {
		std::cout << "Failed to open file" << std::endl;
		return;
	}



	this->peek = ' ';
	this->line = 0;

	bool loop = true;

	while (loop) {
		loop = this->nextToken();
	}
}

void Lexical::readChar()
{
	this->is.get(this->peek);
}

bool Lexical::readChar(char c)
{	
	this->readChar();
	if (this->peek != c)
		return false;

	this->peek = ' ';
	return true;
}

bool Lexical::nextToken()
{
	// peek into the buffer
	// if whitespace then ignore
	// if newline	 then increase linecount
	// otherwise, move on
	for (;; this->readChar()) {

		if (this->is.eof()) {
			// checks for eof, and exits scan
			// TODO: 
			std::cout << "END OF FILE" << std::endl;
			return false;
		}

		if (this->peek == ' ' || this->peek == '\t')
			continue;
		else if (this->peek == '\n')
			this->line += 1;
		else
			break;
	}

	// special characters involved, go here
	switch (this->peek) {
	case ';':
		std::cout << ";" << std::endl;
		this->peek = ' ';
		return true;
	case '(':
		std::cout << "(" << std::endl;
		this->peek = ' ';
		return true;
	case ')':
		std::cout << ")" << std::endl;
		this->peek = ' ';
		return true;
	case '&':
		if (readChar('&')) {
			std::cout << "&&" << std::endl;
			this->peek = ' ';
			return true;
		}
		else {
			std::cout << "&" << std::endl;
			this->peek = ' ';
			return true;
		}
	case '|':
		if (readChar('|')) {
			std::cout << "||" << std::endl;
			this->peek = ' ';
			return true;
		}
		else {
			std::cout << "|" << std::endl;
			this->peek = ' ';
			return true;
		}
	case '=':
		if (readChar('=')) {
			std::cout << "==" << std::endl;
			this->peek = ' ';
			return true;
		}
		else {
			std::cout << "=" << std::endl;
			this->peek = ' ';
			return true;
		}
	case '<':
		if (readChar('=')) {
			std::cout << "<=" << std::endl;
			this->peek = ' ';
			return true;
		}
		else if (readChar('>')) {
			std::cout << "<>" << std::endl;
			this->peek = ' ';
			return true;
		}
		else {
			std::cout << "<" << std::endl;
			this->peek = ' ';
			return true;
		}
	case '>':
		if (readChar('=')) {
			std::cout << ">=" << std::endl;
			this->peek = ' ';
			return true;
		}
		else {
			std::cout << ">" << std::endl;
			this->peek = ' ';
			return true;
		}
	case ',':
		std::cout << "," << std::endl;
		this->peek = ' ';
		return true;
	case '+':
		std::cout << "+" << std::endl;
		this->peek = ' ';
		return true;
	case '*':
		std::cout << "*" << std::endl;
		this->peek = ' ';
		return true;
	case '-':
		std::cout << "-" << std::endl;
		this->peek = ' ';
		return true;
	case '%':
		std::cout << "%" << std::endl;
		this->peek = ' ';
		return true;
	case '/':
		std::cout << "/" << std::endl;
		this->peek = ' ';
		return true;
	case '[':
		std::cout << "[" << std::endl;
		this->peek = ' ';
		return true;
	case ']':
		std::cout << "]" << std::endl;
		this->peek = ' ';
		return true;
	case '.':
		std::cout << "." << std::endl;
		this->peek = ' ';
		return true;

	}

	if (std::isdigit(this->peek)) {
		int v = 0;
		do {
			v = 10 * v + (int)this->peek;
			this->readChar();
		} while (std::isdigit(this->peek));

		if (this->peek != '.') {
			std::cout << v << std::endl;
			return true;
		}

		float x = v;
		float d = 10;

		for (;;) {
			this->readChar();
			if (!std::isdigit(this->peek)) {
				break;
			}
			x = x + (int)this->peek / 10;
			d = d * 10;
		}

		std::cout << x << std::endl;
		return true;
	}

	if (std::isalpha(this->peek)) {
		std::string b = "";
		do {
			b += this->peek;
			this->readChar();
		} while (std::isalnum(this->peek));

		std::cout << b << std::endl;
		return true;
	}

	std::cout << this->peek << std::endl;
	this->peek = ' ';
	return true;
}