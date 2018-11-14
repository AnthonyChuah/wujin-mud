#include "CharacterFileLoader.h"

#include "Attributes.h"

#include <cstdio>
#include <fstream>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

void CharacterFileLoader::SaveCharacterToFile(const Character& character)
{
    printf("Saving %s to file!", character.GetName().c_str());
    (void) character; // xxx implement
}

CharacterFileLoader::CharacterFileLoader(const char* name, const std::string& pwd) :
    _name(name)
{
    bool success = false;
    rapidjson::Document dom;

    std::string fileName = std::string("../playerfiles/") + name + ".json";
    FILE* file = fopen(fileName.c_str(), "r");
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

CharacterFileLoader::CharacterFileLoader(const char* name, const std::string& pwd, Attributes attr) :
    _name(name)
{
    _character._attr = attr;

    rapidjson::StringBuffer strbuff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuff);

    writer.StartObject();

    writer.Key("name");
    writer.String(name);
    writer.Key("password");
    writer.String(pwd.c_str());

    writer.Key("location");
    writer.StartObject();
    writer.Key("world_x");
    writer.Uint(196);
    writer.Key("world_y");
    writer.Uint(196);
    writer.Key("zone_x");
    writer.Uint(196);
    writer.Key("zone_y");
    writer.Uint(196);
    writer.EndObject();

    writer.Key("attributes");
    writer.StartArray();
    writer.Uint(attr.strength);
    writer.Uint(attr.dexterity);
    writer.Uint(attr.constitution);
    writer.Uint(attr.intelligence);
    writer.Uint(attr.willpower);
    writer.Uint(attr.perception);
    writer.EndArray();

    writer.EndObject();

    std::string fileName = std::string("../playerfiles/") + name + ".json";
    std::ofstream f(fileName.c_str());
    f << strbuff.GetString();

    if (f.good())
        printf("Successfully wrote newly created character file at %s", fileName.c_str());
    else
        printf("Failed to write newly created character file to %s", fileName.c_str());
    f.close();
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

    _character._attr.strength = dom["attributes"][0].GetInt();
    _character._attr.dexterity = dom["attributes"][1].GetInt();
    _character._attr.constitution = dom["attributes"][2].GetInt();
    _character._attr.intelligence = dom["attributes"][3].GetInt();
    _character._attr.willpower = dom["attributes"][4].GetInt();
    _character._attr.perception = dom["attributes"][5].GetInt();

    printf("Loaded character %s from file!", name.c_str());
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
