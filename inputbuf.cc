#include "inputbuf.h"

#include <cstdio>
#include <iostream>
#include <istream>
#include <string>
#include <vector>

using namespace std;

bool InputBuffer::EndOfInput() {
    if (!input_buffer.empty())
        return false;
    else
        return cin.eof();
}

char InputBuffer::UngetChar(char c) {
    if (c != EOF)
        input_buffer.push_back(c);
    ;
    return c;
}

void InputBuffer::GetChar(char& c) {
    if (!input_buffer.empty()) {
        c = input_buffer.back();
        input_buffer.pop_back();
    } else {
        cin.get(c);
    }
}

string InputBuffer::UngetString(string s) {
    for (int i = 0; i < s.size(); i++)
        input_buffer.push_back(s[s.size() - i - 1]);
    return s;
}