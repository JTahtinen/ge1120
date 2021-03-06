#include "font.h"
#include "../util/file.h"
#include "../defs.h"
#include "texture.h"
#include <sstream>
#include <string>
#include "../globals.h"

Font* loadFont(std::string filepath)
{
    Texture* atlas = Texture::loadTexture(filepath + ".png");
    if (!atlas)
    {
        message("[ERROR] Could not load font atlas: %s.png\n", filepath);
        return nullptr;
    }


    std::string fontFile = loadTextFile(filepath + ".fnt");
    if (fontFile == "")
    {
        message("[ERROR] Could not load font file: %s.fnt\n", filepath);
        return nullptr;
    }


    Font* font = (Font*)g_memory.reserve(sizeof(Font));
    font->id = nextIDFont();
    font->atlas = atlas;
    font->filepath = filepath;
    
    unsigned int texWidth = atlas->widthInPixels;
    unsigned int texHeight = atlas->heightInPixels;

    std::istringstream ss(fontFile);
    std::string line;
    Vector<Letter>& letters = font->letters;
    letters.init(100);
    //letters.reserve(100);
    unsigned int i = 0;
    bool firstIdSkipped = false; // The font file contains "id:" that we want to skip
    bool baseFound = false;
    
    while (ss >> line)
    {
        std::istringstream word(line);
        getline(word, line, '=');
        if (!baseFound && line == "base")
        {
            getline(word, line);
            font->base = stof(line) / (float)texHeight;
            
        }
        if (line == "id")
        {
            if (!firstIdSkipped)
            {
                firstIdSkipped = true;
                continue;
            }
         
            getline(word, line);
            letters.push_back(Letter{ 0 });
            letters[i].character = stoi(line);
        }
        else if (line == "x")
        {
            getline(word, line);
            letters[i].x = stof(line) / (float)texWidth;
        }
        else if (line == "y")
        {
            getline(word, line);
            letters[i].y = stof(line) / (float)texHeight;
        }
        else if (line == "width")
		{
			getline(word, line);
			letters[i].width = stof(line) / (float)texWidth;
		}
		else if (line == "height")
		{
			getline(word, line);
			letters[i].height = stof(line) / (float)texHeight;
		}
        else if (line == "xoffset")
		{
			getline(word, line);
			letters[i].xOffset = stof(line) / (float)texWidth;
		}
		else if (line == "yoffset")
		{
			getline(word, line);
			letters[i].yOffset = stof(line) / (float)texHeight;
		}
		else if (line == "xadvance")
		{
			getline(word, line);
			letters[i++].xAdvance = stof(line) / (float)texWidth;
		}
		
    }
        
    return font;
}

Letter* Font::getLetter(char c)
{
    for (unsigned int i = 0; i < letters.size; ++i)
    {
        if (letters[i].character == c)
        {
            return &letters[i];
        }
    }
    return nullptr;
}

unsigned int nextIDFont()
{
    static unsigned int numIds = 0;
    return numIds++;
}

void deleteFont(Font* font)
{
    if (font)
    {
        font->atlas->del();
    }
}
