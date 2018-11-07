#include "CharacterFileLoader.h"

#include <cstdio>
#include <iostream>

CharacterFileLoader::CharacterFileLoader(const char* name, const std::string& pwd) :
    _name(name)
{
    bool success = false;

    std::string fileName = std::string("../playerfiles/") + name + ".json";
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file)
    {
        char buffer[65536];
        rapidjson::FileReadStream jsonStream(file, buffer, sizeof(buffer));
        _dom.ParseStream(jsonStream);
        // Check that password matches: if not, clear the name
        if (!CheckDomValid())
            std::cout << "Invalid DOM for parsed JSON\n";
        else
        {
            std::string password = _dom["password"].GetString();
            if (pwd == password)
            {
                std::cout << "Valid password provided\n";
                success = true;
            }
        }
    }

    if (success)
        PopulateCharacterData();
    else
        _name.clear();
}

void CharacterFileLoader::PopulateCharacterData()
{
    std::string name = _dom["name"].GetString();
    _character.SetName(std::move(name));

    _character.SetLocation({
        uint8_t(_dom["location"]["world_x"].GetInt()),
        uint8_t(_dom["location"]["world_y"].GetInt()),
        uint8_t(_dom["location"]["zone_x"].GetInt()),
        uint8_t(_dom["location"]["zone_y"].GetInt())
    });
}

bool CharacterFileLoader::CheckDomValid() const
{
    if (!_dom.HasMember("password")) return false;
    if (!_dom.HasMember("name")) return false;
    if (!_dom.HasMember("location")) return false;
    if (!_dom["name"].IsString()) return false;
    if (!_dom["password"].IsString()) return false;
    return true;
}
