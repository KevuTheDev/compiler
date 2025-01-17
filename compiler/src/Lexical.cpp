#include "Lexical.h"

Lexical::Lexical(const std::string& filename) 
{
	if (!this->checkExtension(filename)) {
		return;
	}

	this->_is = std::ifstream(filename);
	if (!this->_is.is_open()) {
		std::cout << "Failed to open file" << std::endl;
		return;
	}
	sanitizeFileName(filename);


	// Initialize attributes
	this->_peek = ' ';
	this->_lineNumber = 1;
	this->_charNumber = 0;
	this->_doubleBufferSwitch = true;
	this->_doubleBufferCounter1 = 0;
	this->_doubleBufferCounter2 = 0;
	this->_doubleBuffer1 = new char[compiler::COMPILER_BUFFER_SIZE];
	this->_doubleBuffer2 = new char[compiler::COMPILER_BUFFER_SIZE];

	memset(this->_doubleBuffer1, 0, compiler::COMPILER_BUFFER_SIZE);
	memset(this->_doubleBuffer2, 0, compiler::COMPILER_BUFFER_SIZE);

	this->_error = false;

	this->_logFileBuffer = nullptr;
	this->_tokenFileBuffer = nullptr;

	this->_reservedWords = nullptr;
	this->_symbolTable = nullptr;
}

void Lexical::linkLogFileBuffer(std::shared_ptr<LogFileBuffer> buffer)
{
	this->_logFileBuffer = buffer;
}

void Lexical::linkTokenFileBuffer(std::shared_ptr<TokenFileBuffer> buffer)
{
	this->_tokenFileBuffer = buffer;
}

void Lexical::linkReservedWords(std::shared_ptr<ReservedWords> table)
{
	this->_reservedWords = table;
}

void Lexical::linkSymbolTable(std::shared_ptr<SymbolTable> table)
{
	this->_symbolTable = table;
}

void Lexical::run()
{
	auto helper1 = std::thread(compiler::readFileToBuffers,
		std::ref(this->_is), this->_doubleBuffer1);
	helper1.join();

	auto helper2 = std::thread(compiler::readFileToBuffers,
		std::ref(this->_is), this->_doubleBuffer2);
	helper2.join();


	bool loop = true;
	while (loop) {
		loop = this->getNextToken();
	}



	this->_logFileBuffer->clearBuffer();
	this->_is.close();
}

bool Lexical::getError()
{
	return this->_error;
}

bool Lexical::checkExtension(const std::string& filepath)
{
	if (filepath.length() < compiler::COMPILER_FILE_EXTENSION_MAIN_LEN + 1) {
		std::cout << "Invalid file string specified: length size" << std::endl;
		return false;
	}

	// check if directory or file

	if (filepath.substr(filepath.length() - compiler::COMPILER_FILE_EXTENSION_MAIN_LEN) 
		!= compiler::COMPILER_FILE_EXTENSION_MAIN) {
		std::cout << "Invalid file string specified: file extension" << std::endl;
		return false;
	}

	return true;
}

void Lexical::readFileToBuffer(std::ifstream& is, char* buf)
{
	is.read(buf, compiler::COMPILER_BUFFER_SIZE_NULL);

	return;
}

void Lexical::sanitizeFileName(const std::string& filename)
{
	// This assumes that the given filename is already valid
	this->_fileName = filename.substr(0, filename.length() - compiler::COMPILER_FILE_EXTENSION_MAIN_LEN);
	size_t char_pos = this->_fileName.find('/');

	
	while (char_pos != std::string::npos) {
		std::cout << this->_fileName << std::endl;
		this->_fileName = this->_fileName.substr(char_pos + 1);
		char_pos = this->_fileName.find('/');
	}
}

void Lexical::appendToLogFileBuffer(int linenumber, int rownumber, const std::string& errorchar)
{
	this->_logFileBuffer->logLexicalError(linenumber, rownumber, errorchar);
}

void Lexical::appendToSymbolTable(compiler::TOKEN token, std::string lexeme)
{
	this->_symbolTable->append(token, lexeme, this->_lineNumber, this->_charNumber);
}

bool Lexical::getNextToken()
{
	// peek into the buffer
	// if whitespace then ignore
	// if newline	 then increase linecount
	// otherwise, move on
	for (;; this->readNextChar()) {

		if (this->_peek == NULL) {
			std::cout << "END OF FILE" << std::endl;
			return false;
		}

		if (this->_peek == ' ' || this->_peek == '\t')
			continue;
		else if (this->_peek == '\n')
		{
			this->_charNumber = 0;
			this->_lineNumber += 1;
		}
		else
			break;
	}

	// special characters involved, go here
	switch (this->_peek) {
	case ';':
		appendToSymbolTable(compiler::SEMICOLON, ";");
		//std::cout << ";" << std::endl;
		this->_peek = ' ';
		return true;
	case '(':
		appendToSymbolTable(compiler::LEFT_PAREN, "(");
		//std::cout << "(" << std::endl;
		this->_peek = ' ';
		return true;
	case ')':
		appendToSymbolTable(compiler::RIGHT_PAREN, ")");
		//std::cout << ")" << std::endl;
		this->_peek = ' ';
		return true;
	case '=':
		if (readNextChar('=')) {
			appendToSymbolTable(compiler::COMP_EQUAL, "==");
			//std::cout << "==" << std::endl;
			this->_peek = ' ';
			return true;
		}
		else {
			appendToSymbolTable(compiler::EQUAL, "=");
			//std::cout << "=" << std::endl;
			this->_peek = ' ';
			return true;
		}
	case '<':
		if (readNextChar('=')) {
			appendToSymbolTable(compiler::COMP_LEQUAL, "<=");
			//std::cout << "<=" << std::endl;
			this->_peek = ' ';
			return true;
		}
		else if (readNextChar('>')) {
			appendToSymbolTable(compiler::COMP_NOT, "<>");
			//std::cout << "<>" << std::endl;
			this->_peek = ' ';
			return true;
		}
		else {
			appendToSymbolTable(compiler::COMP_LTHAN, "<");
			//std::cout << "<" << std::endl;
			this->_peek = ' ';
			return true;
		}
	case '>':
		if (readNextChar('=')) {
			appendToSymbolTable(compiler::COMP_GEQUAL, ">=");
			//std::cout << ">=" << std::endl;
			this->_peek = ' ';
			return true;
		}
		else {
			appendToSymbolTable(compiler::COMP_GTHAN, ">");
			//std::cout << ">" << std::endl;
			this->_peek = ' ';
			return true;
		}
	case ',':
		appendToSymbolTable(compiler::COMMA, ",");
		//std::cout << "," << std::endl;
		this->_peek = ' ';
		return true;
	case '+':
		appendToSymbolTable(compiler::PLUS, "+");
		//std::cout << "+" << std::endl;
		this->_peek = ' ';
		return true;
	case '*':
		appendToSymbolTable(compiler::MULTIPLY, "*");
		//std::cout << "*" << std::endl;
		this->_peek = ' ';
		return true;
	case '-':
		appendToSymbolTable(compiler::MINUS, "-");
		//std::cout << "-" << std::endl;
		this->_peek = ' ';
		return true;
	case '%':
		appendToSymbolTable(compiler::MODULUS, "%");
		//std::cout << "%" << std::endl;
		this->_peek = ' ';
		return true;
	case '/':
		appendToSymbolTable(compiler::DIVIDE, "/");
		//std::cout << "/" << std::endl;
		this->_peek = ' ';
		return true;
	case '[':
		appendToSymbolTable(compiler::LEFT_BRACK, "[");
		//std::cout << "[" << std::endl;
		this->_peek = ' ';
		return true;
	case ']':
		appendToSymbolTable(compiler::RIGHT_BRACK, "]");
		//std::cout << "]" << std::endl;
		this->_peek = ' ';
		return true;
	case '.':
		appendToSymbolTable(compiler::DOT, ".");
		//std::cout << "." << std::endl;
		this->_peek = ' ';
		return true;

	}

	if (std::isdigit(this->_peek)) {
		int v = 0;
		//std::cout << "std:::::" << this->_peek << " " << static_cast<int>(this->_peek) <<std::endl;
		do {
			v = 10 * v + int(this->_peek - '0');
			this->readNextChar();
		} while (std::isdigit(this->_peek));

		if (this->_peek != '.') {
			appendToSymbolTable(compiler::VALUE_INTEGER, std::to_string(v));
			//std::cout << v << std::endl;
			return true;
		}

		float x = static_cast<float>(v);
		float d = 10;

		for (;;) {
			this->readNextChar();
			if (!std::isdigit(this->_peek)) {
				break;
			}
			x = x + float(this->_peek - '0') / 10;
			d = d * 10;
		}
		
		appendToSymbolTable(compiler::VALUE_DOUBLE, std::to_string(x));
		//std::cout << x << std::endl;
		return true;
	}

	if (std::isalpha(this->_peek)) {
		std::string b = "";
		do {
			b += this->_peek;
			this->readNextChar();
		} while (std::isalnum(this->_peek));

		if (this->_reservedWords->findReservedWord(b)) {
			appendToSymbolTable(static_cast<compiler::TOKEN>(this->_reservedWords->findReservedWordIndex(b)), b);
		}
		else {
			appendToSymbolTable(compiler::ID, b);
		}

		//std::cout << b << std::endl;
		return true;
	}

	std::string s(&this->_peek);
	this->appendToLogFileBuffer(this->_lineNumber, this->_charNumber, s);
	this->_error = true;
	//appendToSymbolTable("OTHER", s);
	//appendToTokenFileBuffer("ERROR: " + s);
	//std::cout << this->_peek << std::endl;
	this->_peek = ' ';
	return true;
}

void Lexical::readNextChar()
{
	//std::cout << "::" <<this->_doubleBufferSwitch << "::   Char: " << this->_peek << "    " << std::endl;
	if (this->_doubleBufferSwitch) {
		this->_peek = this->_doubleBuffer1[this->_doubleBufferCounter1];
		this->_doubleBufferCounter1 += 1;
		this->_charNumber += 1;
		
		if (this->_doubleBufferCounter1 == compiler::COMPILER_BUFFER_SIZE) {
			// Get the next char from buffer 2
			this->_peek = this->_doubleBuffer2[this->_doubleBufferCounter2];
			this->_doubleBufferCounter2 += 1;
			this->_charNumber += 1;

			this->_doubleBufferSwitch = !this->_doubleBufferSwitch;

			memset(this->_doubleBuffer1, 0, compiler::COMPILER_BUFFER_SIZE);
			auto helper1 = std::thread(compiler::readFileToBuffers, std::ref(this->_is), std::ref(this->_doubleBuffer1));
			helper1.join();

			this->_doubleBufferCounter1 = 0;
		}
	}
	else {
		this->_peek = this->_doubleBuffer2[this->_doubleBufferCounter2];
		this->_doubleBufferCounter2 += 1;
		this->_charNumber += 1;

		if (this->_doubleBufferCounter2 == compiler::COMPILER_BUFFER_SIZE) {
			// Get the next char from buffer 1
			this->_peek = this->_doubleBuffer1[this->_doubleBufferCounter1];
			this->_doubleBufferCounter1 += 1;
			this->_charNumber += 1;

			this->_doubleBufferSwitch = !this->_doubleBufferSwitch;

			memset(this->_doubleBuffer2, 0, compiler::COMPILER_BUFFER_SIZE);
			auto helper1 = std::thread(compiler::readFileToBuffers, std::ref(this->_is), std::ref(this->_doubleBuffer2));
			helper1.join();

			this->_doubleBufferCounter2 = 0;
		}
	}
}

bool Lexical::readNextChar(char c)
{
	this->readNextChar();

	if (this->_peek != c) {
		if (this->_doubleBufferSwitch) {
			this->_doubleBufferCounter1 -= 1;
			this->_charNumber -= 1;
		}
		else {
			this->_doubleBufferCounter2 -= 1;
			this->_charNumber -= 1;
		}
		return false;
	}

	this->_peek = ' ';
	return true;
}