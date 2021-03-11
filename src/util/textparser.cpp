#include "textparser.h"

bool initParser(TextParser* parser, const String& text)
{
    if (!parser)
    {
        err("Could not init text parser - parser was NULL!\n");
        return false;
    }
    if (text.size == 0)
    {
        err("Could not init text parser - text was empty!\n");
        return false;
    }
    parser->index = 0;
    parser->text = text;
    parser->separator = '\0';
    parser->skipSeparator = true;
    parser->skipWhiteSpace = true;
    parser->skipNewLine = false;
    return true;
}

void setSeparator(char separator, bool skipSeparator, TextParser* parser)
{
    parser->separator = separator;
    parser->skipSeparator = skipSeparator;
    if (separator == ' ' && skipSeparator)
    {
        parser->skipWhiteSpace = true;
    }
}

bool getWord(const String& text, size_t& index, char separator,
             bool skipWhiteSpace, bool skipSeparator, bool skipNewLine, String* target)
{
    if (separator == '\0')
    {
        warn("Word parsing aborted - separator not set!\n");
        return false;
    }
    target->clear();
    while(text[index] != '\0')
    {
        char c = text[index];
        if (!(c == separator && skipSeparator)
            && !(c == ' ' && skipWhiteSpace)
            && !(c == '\n' && skipNewLine))
        {
            target->append(text[index]);
        }
        ++index;
        if (c == separator)
        {           
            break;
        }       
    }
    bool result = target->size > 0;
    return result;
}
bool getWord(TextParser* parser, String* target)
{
    bool result = getWord(parser->text, parser->index, parser->separator,
                          parser->skipWhiteSpace, parser->skipSeparator,
                          parser->skipNewLine, target);

    return result;
}

bool getLine(TextParser* parser, String* target)
{
    bool result = getWord(parser->text, parser->index, '\n', false, true, true, target);
    return result;
}
