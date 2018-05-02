#ifndef CALCFRAMER_HPP
#define CALCFRAMER_HPP

#include <iostream>
#include <deque>

class CalcFramer {
public:
	void append(std::string chars);

	// Does this buffer contain at least one complete message?
	bool hasMessage() const;

	// Copy the first instruction in this buffer into the provided class
	std::string topMessage() const;

	// Pops off the first instruction from the buffer
	void popMessage();

	// prints the string to an ostream (useful for debugging)

public:
	std::deque<char> buffer;// PUT ANY FIELDS/STATE HERE
};

#endif // CALCFRAMER_HPP
