#pragma once
#include "../util/vector.h"
#include "../util/string.h"

#define EXIT_CONVERSATION 0

enum ConvState
{
    CONV_FAILED,
    CONV_ACTIVE,
    CONV_INACTIVE
};

struct ConvOption
{
    String text;
    unsigned int nextNode;
};

struct ConvNode
{
    unsigned int id;
    String text;
    Vector<ConvOption> options;
};

struct Conversation
{
    Vector<ConvNode> nodes;
    ConvNode* currentNode;
    unsigned int nextNodeID;
};

bool initConversation(Conversation* result);
bool addNode(const String& text, Conversation* conv, unsigned int* handle);
bool addOption(const String& text, unsigned int nodeID, unsigned int nextNodeID, Conversation* target);
bool chooseOption(unsigned int optionNum, Conversation* conv);
ConvState updateConversation(Conversation* conv);
bool reset(Conversation* conv);
bool loadConversation(const char* filepath, Conversation* target);
