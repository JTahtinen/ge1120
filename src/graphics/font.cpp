#include "font.h"
#include "../util/file.h"
#include "../defs.h"
#include "texture.h"
#include <sstream>
#include <string>

Font* Font::loadFont(std::string filepath)
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

    unsigned int texWidth = atlas->widthInPixels;
    unsigned int texHeight = atlas->heightInPixels;

    std::istringstream ss(fontFile);
    std::string line;
    Vector<Letter> letters;
    int i = 0;
    bool firstIdSkipped = false; // The font file contains "id:" that we want to skip
    bool baseFound = false;
    float base = 0.0f;
    while (ss >> line)
    {
        std::istringstream word(line);
        getline(word, line, '=');
        if (!baseFound && line == "base")
        {
            getline(word, line);
            base = stof(line) / (float)texHeight;
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

    Font* font = new Font(letters, atlas, base, filepath);
    return font;
}

Font::Font(Vector<Letter> letters, Texture* atlas, float base, std::string filepath)
    : _letters(letters)
    , _atlas(atlas)
    , _base(base)
    , _filepath(filepath)
    , _id(nextId())
{
}

Letter* Font::getLetter(char c)
{
    for (unsigned int i = 0; i < _letters.size(); ++i)
    {
        if (_letters[i].character == c)
        {
            return &_letters[i];
        }
    }
    return nullptr;
}

unsigned int Font::nextId()
{
    static unsigned int numIds = 0;
    return numIds++;
}
