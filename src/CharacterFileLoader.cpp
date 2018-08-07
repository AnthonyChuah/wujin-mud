#include "CharacterFileLoader.h"

#include <cstdio>

CharacterFileLoader::CharacterFileLoader(const char* name, const std::string& pwd) :
    _name(name)
{
    std::string fileName = std::string("playerfiles/") + name + ".txt";
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file)
    {
        char buffer[65536];
        rapidjson::FileReadStream jsonStream(file, buffer, sizeof(buffer));
        _dom.ParseStream(jsonStream);
        // Check that password matches: if not, clear the name
        (void) pwd;
        if (false)
            _name.clear();
    }
    else
        _name.clear();
}

bool CharacterFileLoader::LoadCharacterData(CharacterTemplate& templait)
{
    if (_name.empty())
        return false;

    (void) templait;

    return true;
}
