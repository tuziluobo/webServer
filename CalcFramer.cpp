#include <assert.h>
#include "CalcFramer.hpp"

using namespace std;

void CalcFramer::append(string chars)
{
	for (uint i = 0; i < chars.size(); i++) {
		buffer.push_back(chars[i]);
	}// PUT YOUR CODE HERE
}

bool CalcFramer::hasMessage() const
{
	for(int i=0;i<int(buffer.size())-1;i++){
		if (buffer[i]=='\r'&&buffer[i+1]=='\n')
			return true;
                //if(buffer[i]=='\n')
                //   return true;
	}
	return false;
	// PUT YOUR CODE HERE
}

string CalcFramer::topMessage() const
{
	string res = "";
	if(hasMessage()){
		for (int i = 0; i < int(buffer.size())-1; i++) {
			if (buffer[i]=='\r'&&buffer[i+1] == '\n') {
                                res = res+'\r'+'\n';
				return res;
			}
                        //if(buffer[i]=='\n')
                        //    return res;
			else
				res = res + buffer[i];
		}
	}
	// PUT YOUR CODE HERE
	return "";
}

void CalcFramer::popMessage()
{
	if (!hasMessage())
		return;
	uint k=0;
	while (buffer.size() >= k) {
		if (buffer[0] == '\r'&&buffer[1]=='\n') {
			buffer.pop_front();
			buffer.pop_front();
			break;
		}
		else {
			buffer.pop_front();
		}
	}
	return;
			// PUT YOUR CODE HERE
}

