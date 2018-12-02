#include "CharacterFileLoader.h"

#include "Attributes.h"

#include <cstdio>
#include <fstream>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

CharacterFileLoader CharacterFileLoader::SaveCharacterToFile(const Character& character)
{
    printf("Saving player file for %s!", character.GetName().c_str());
    auto loader = CharacterFileLoader(character); // ctor saves to file
    return loader;
}

CharacterFileLoader CharacterFileLoader::CreateNewCharacterFile(const char* name, const std::string& pwd,
                                                                Attributes attr)
{
    printf("Writing new player file for %s!", name);
    Character character;
    character._attr = attr;
    character._pwd = pwd;
    character._name = name;
    character.InitializeScores();
    auto loader = CharacterFileLoader(character); // copies, but character creation should be infrequent
    return loader;
}

CharacterFileLoader::CharacterFileLoader(const Character& character) : _character(character)
{
    rapidjson::StringBuffer strbuff;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuff);

    writer.StartObject();

    writer.Key("name");
    writer.String(_character._name.c_str());
    writer.Key("password");
    writer.String(_character._pwd.c_str());

    writer.Key("location");
    writer.StartObject();
    writer.Key("world_x");
    writer.Uint(_character._location.major.x);
    writer.Key("world_y");
    writer.Uint(_character._location.major.y);
    writer.Key("zone_x");
    writer.Uint(_character._location.minor.x);
    writer.Key("zone_y");
    writer.Uint(_character._location.minor.y);
    writer.EndObject();

    writer.Key("attributes");
    writer.StartArray();
    writer.Uint(_character._attr.strength);
    writer.Uint(_character._attr.dexterity);
    writer.Uint(_character._attr.constitution);
    writer.Uint(_character._attr.intelligence);
    writer.Uint(_character._attr.willpower);
    writer.Uint(_character._attr.perception);
    writer.EndArray();

    writer.EndObject();

    std::string fileName = std::string("../playerfiles/") + _character._name + ".json";
    std::ofstream f(fileName.c_str());
    f << strbuff.GetString();

    if (f.good())
        printf("Successfully wrote newly created character file at %s", fileName.c_str());
    else
        printf("Failed to write newly created character file to %s", fileName.c_str());
    f.close();
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

void CharacterFileLoader::PopulateCharacterData(const rapidjson::Document& dom)
{
    std::string name = dom["name"].GetString();
    _character.SetName(std::move(name));
    _character._pwd = dom["password"].GetString();

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
