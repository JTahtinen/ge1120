#pragma once
#include "string.h"

struct TextParser
{
    size_t index;
    String text;
    char separator;
    bool skipSeparator;
    bool skipWhiteSpace;
    bool skipNewLine;
};

bool initParser(TextParser* parser, const String& text);
void setSeparator(char separator, bool skipSeparator, TextParser* parser);
bool getWord(const String& text, size_t& index, char separator,
             bool skipWhiteSpace, bool skipSeparator, bool skipNewLine, String* target);
bool getWord(TextParser* parser, String* target);
bool getLine(TextParser* parser, String* target);

