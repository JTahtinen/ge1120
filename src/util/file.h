#pragma once
#include <iostream>
#include <string>
#include <fstream>

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