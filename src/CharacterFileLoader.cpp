#include "CharacterFileLoader.h"

#include <cstdio>

CharacterFileLoader::CharacterFileLoader(const char* name, const std::string& pwd) :
    _name(name)
{
    bool success = false;
    rapidjson::Document dom;

    std::string fileName = std::string("../playerfiles/") + name + ".json";
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file)
    {
        char buffer[65536];
        rapidjson::FileReadStream jsonStream(file, buffer, sizeof(buffer));
        dom.ParseStream(jsonStream);
        // Check that password matches: if not, clear the name
        if (!CheckDomValid(dom))
            printf("Invalid DOM for parsed JSON\n");
        else
        {
            std::string password = dom["password"].GetString();
            if (pwd == password)
            {
                printf("Valid password provided\n");
                success = true;
            }
        }
    }

    if (success)
        PopulateCharacterData(dom);
    else
        _name.clear();
}

void CharacterFileLoader::PopulateCharacterData(const rapidjson::Document& dom)
{
    std::string name = dom["name"].GetString();
    _character.SetName(std::move(name));

    _character.SetLocation({
        uint8_t(dom["location"]["world_x"].GetInt()),
        uint8_t(dom["location"]["world_y"].GetInt()),
        uint8_t(dom["location"]["zone_x"].GetInt()),
        uint8_t(dom["location"]["zone_y"].GetInt())
    });
}

bool CharacterFileLoader::CheckDomValid(const rapidjson::Document& dom) const
{
    if (!dom.HasMember("password")) return false;
    if (!dom.HasMember("name")) return false;
    if (!dom.HasMember("location")) return false;
    if (!dom["name"].IsString()) return false;
    if (!dom["password"].IsString()) return false;
    return true;
}
