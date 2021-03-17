#include "conversation.h"
#include "../defs.h"
#include "../globals.h"
#include "../graphics/renderer.h"
#include "../util/file.h"
#include "../util/textparser.h"
#include "../util/vector.h"
#include <stdlib.h>

bool initConversation(Conversation* result)
{
    if (!result)
    {
        err("Could not create conversation - target was NULL!\n");
        return false;
    }

    result->nodes.reserve(10);
    result->currentNode = NULL;
    result->nextNodeID = 1;
    return true;
}

bool addNode(const String& text, Conversation* conv, unsigned int* handle)
{
    if (!conv)
    {
        err("Could not add node to conversation - Node was NULL!\n");
        return false;
    }
    
    ConvNode node;
    node.id = conv->nextNodeID++;
    node.text = text;
    conv->nodes.push_back(node);
    if (!conv->currentNode)
    {
        conv->currentNode = &conv->nodes[0];
    }
    *handle = node.id;
    return true;
}

bool addOption(const String& text, unsigned int nodeID, unsigned int nextNodeID, Conversation* target)
{
    if (!target)
    {
        err("Could not add option - Conversation was NULL!\n");
        return false;
    }
    for (unsigned int i = 0; i < target->nodes.size; ++i)
    {
        if (target->nodes[i].id == nodeID)
        {
            target->nodes[i].options.push_back({text, nextNodeID});
            return true;
        }
    }
    err("Could not add option to Conversation - Node ID: %s does not exist!\n", nodeID);
    return false;
}

bool chooseOption(unsigned int optionNum, Conversation* conv)
{
    if (!conv)
    {
        warn("Could not choose option - Conversation was NULL!\n");
        return false;
    }
    if (!conv->currentNode || optionNum == 0 || optionNum > conv->currentNode->options.size)
    {
        return false;
    }
    ConvOption* option = &conv->currentNode->options[optionNum - 1];
    unsigned int nextNode = option->nextNode;
    if (nextNode == EXIT_CONVERSATION)
    {
        conv->currentNode = NULL;
        return true;
    }
    for (unsigned int i = 0; i < conv->nodes.size; ++i)
    {
        if (conv->nodes[i].id == nextNode)
        {
            conv->currentNode = &conv->nodes[i];
            return true;
        }
    }
    //err ("Could not choose option - Next node not found!\n");
    return false;
}

ConvState updateConversation(Conversation* conv)
{
    if (!conv)
    {
        err("Could not update conversation - pointer was NULL!\n");
        return CONV_FAILED;
    }
    if (!conv->currentNode)
    {
        return CONV_INACTIVE;
    }
    
    ConvNode* currentNode = conv->currentNode;
    
    g_uiRenderer->submitText(conv->currentNode->text, Vec2(0, 0.3f));
    unsigned int numOptions = currentNode->options.size;
    unsigned int i;
    for (i = 0; i < numOptions; ++i)
    {
        g_uiRenderer->submitText(toString(i + 1) + " " + currentNode->options[i].text, Vec2(0, 0.27f - i * 0.03f));
    }
    return CONV_ACTIVE;
}

bool reset(Conversation* conv)
{
    if (!conv)
    {
        err("Could not reset conversation - pointer was NULL!\n");
        return false;
    }
    if (conv->nodes.size > 0)
    {
        conv->currentNode = &conv->nodes[0];
    }
    return true;
}

bool loadConversation(const char* filepath, Conversation* target)
{
    String file;
    if (!loadTextFile(filepath, &file))
    {
        err("Could not load conversation - Couldn't open file: %s\n", filepath);
        return false;
    }
    if (!target)
    {
        err("Could not load conversation - Target was NULL!\n");
        return false;
    }
    initConversation(target);
    TextParser parser;
    initParser(&parser, file);
    setSeparator(' ', false, &parser);
    String line;
    Vector<unsigned int> handles;
    handles.reserve(10);
    getLine(&parser, &line);
    if (line != "nodes:")
    {
        err("Could not load conversation from file: %s - Corrupt file!\n", filepath);
        return false;
    }
    while (getLine(&parser, &line) && line != "options:")
    {
    
         
            unsigned int handle;
            addNode(line, target, &handle);
            handles.push_back(handle);
    }
        
        
    while(getLine(&parser, &line))
    {
         
        String optionString;
        String nodeIDString;
        String nextNodeIDString;
         
        optionString.append(line);        
        getWord(&parser, &nodeIDString);
        getWord(&parser, &nextNodeIDString);
        addOption(line, atoi(nodeIDString.c_str()), atoi(nextNodeIDString.c_str()), target);
    }
    return true;
}
