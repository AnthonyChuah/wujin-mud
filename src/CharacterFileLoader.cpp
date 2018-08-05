#include "CharacterFileLoader.h"

#include <cstdio>

CharacterFileLoader::CharacterFileLoader(const char* name) :
    _name(name)
{
    std::string fileName = std::string("playerfiles/") + name + ".txt";
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file)
    {
        char buffer[65536];
        rapidjson::FileReadStream jsonStream(file, buffer, sizeof(buffer));
        _dom.ParseStream(jsonStream);
    }
    else
        _name.clear();
}

bool CharacterFileLoader::LoadCharacterData(Character& character)
{
    if (_name.empty())
        return false;

    (void) character;

    return true;
}
