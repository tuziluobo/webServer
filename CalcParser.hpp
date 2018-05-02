#ifndef CALCPARSER_HPP
#define CALCPARSER_HPP

#include <string>
#include <stdint.h>

using namespace std;

typedef struct CalcInstruction_t {
    string key; // DEFINE YOUR DATA STRUCTURE HERE
    string value;
    string sign;
    string url;
    string version;
    string info;
    string filetype;
} CalcInstruction;

/*
 * Alternatively:
 * class CalcInstruction {
 *   // DEFINE YOUR CLASS HERE
 * };
 *
 */


class CalcParser {
public:
	static CalcInstruction parse(string insstr);
};

#endif // CALCPARSER_HPP
