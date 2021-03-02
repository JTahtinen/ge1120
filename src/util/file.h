#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include "../defs.h"

inline std::string loadTextFile(const std::string &filepath)
{
    std::string text = "";
    std::ifstream file;
    file.open(filepath);
    if (!file)
    {
        std::cout << "Could not open file: " << filepath << std::endl;
        return text;
    }
    std::string line;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            text.append(line + "\n");
        }
        file.close();
    }
    return text;
}

inline void loadBinaryFile(const char* filepath, void* ptr, size_t numBytes)
{
    FILE* fp = fopen(filepath, "rb");
    if (!fp)
    {
        err("Could not open binary file: %s\n", filepath);
        return;
    }
    //Determine size of file
    fseek(fp, 0, SEEK_END); 
    size_t fileSize = ftell(fp); 
    fseek(fp, 0, SEEK_SET);

    size_t bytesToRead;
    if (numBytes > fileSize)
    {
        warn("Tried to read bytes past end of file: %s\n", filepath); 
        bytesToRead = fileSize;
    }
    else
    {
        bytesToRead = numBytes;
    }
    fread(ptr, 1, bytesToRead, fp);
    fclose(fp);
}

inline void saveBinaryFile(const char* filepath, void* data, size_t numBytes)
{
    FILE* fp = fopen(filepath, "wb");
    fwrite(data, numBytes, 1, fp);  
    fclose(fp);
}
