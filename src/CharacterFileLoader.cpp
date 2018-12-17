#include "CharacterFileLoader.h"

#include "Attributes.h"
#include "Utils.h"

#include <cstdio>
#include <fstream>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

CharacterFileLoader CharacterFileLoader::SaveCharacterToFile(const Character& character)
{
    printf("Saving player file for %s!\n", character.GetName().c_str());
    auto loader = CharacterFileLoader(character); // ctor saves to file
    return loader;
}

CharacterFileLoader CharacterFileLoader::CreateNewCharacterFile(const char* name, const std::string& pwd,
                                                                Attributes attr)
{
    printf("Writing new player file for %s!\n", name);
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

    writer.Key("equipment");

    writer.StartObject(); // start equipment

    writer.Key("weapon_style");
    writer.Int(CastToUnderlying(_character._equipment.weaponSet.style));

    writer.Key("twohand_type");
    writer.Int(CastToUnderlying(_character._equipment.weaponSet.twohand));
    writer.Key("twohand_tier");
    writer.Uint(_character._equipment.weaponSet.twohandTier);

    writer.Key("mainhand_type");
    writer.Int(CastToUnderlying(_character._equipment.weaponSet.mainhand));
    writer.Key("mainhand_tier");
    writer.Uint(_character._equipment.weaponSet.mainhandTier);

    writer.Key("offhand_type");
    writer.Int(CastToUnderlying(_character._equipment.weaponSet.offhand));
    writer.Key("offhand_tier");
    writer.Uint(_character._equipment.weaponSet.offhandTier);

    writer.Key("twohand_durability");
    writer.Uint(_character._equipment.weaponSet.offhandTier);
    writer.Key("mainhand_durability");
    writer.Uint(_character._equipment.weaponSet.offhandTier);
    writer.Key("offhand_durability");
    writer.Uint(_character._equipment.weaponSet.offhandDurability);

    writer.Key("ranged_type");
    writer.Int(CastToUnderlying(_character._equipment.rangedType));
    writer.Key("ranged_tier");
    writer.Uint(_character._equipment.rangedTier);

    writer.Key("armour_type");
    writer.Int(CastToUnderlying(_character._equipment.armourType));
    writer.Key("armour_tier");
    writer.Uint(_character._equipment.armourTier);

    writer.Key("ranged_durability");
    writer.Uint(_character._equipment.rangedDurability);
    writer.Key("armour_durability");
    writer.Uint(_character._equipment.armourDurability);
    writer.Key("implements");
    writer.Uint(_character._equipment.implements);
    // encumbrance is always derived from other values

    writer.EndObject(); // end equipment

    writer.Key("items");
    writer.StartObject(); // start items
    writer.Key("maxsupplies");
    writer.Uint(_character._items.maxsupplies);
    writer.Key("maxammo");
    writer.Uint(_character._items.maxammo);
    writer.Key("supplies");
    writer.Uint(_character._items.supplies);
    writer.Key("ammo");
    writer.Uint(_character._items.ammo);
    writer.Key("money");
    writer.Uint(_character._items.money);
    writer.Key("banked");
    writer.Uint(_character._items.banked);
    writer.EndObject(); // end items

    writer.Key("loot");
    writer.StartArray();
    writer.Uint(_character._loot.loot[0]);
    writer.Uint(_character._loot.loot[1]);
    writer.Uint(_character._loot.loot[2]);
    writer.Uint(_character._loot.loot[3]);
    writer.Uint(_character._loot.loot[4]);
    writer.EndArray();
    writer.Key("maxloot");
    writer.Uint(_character._loot.maxloot);

    writer.Key("progress");
    writer.StartObject(); // start progress
    writer.Key("experience");
    writer.Uint(_character._progress.experience);
    writer.Key("ability_points");
    writer.Uint(_character._progress.abilityPoints);
    writer.Key("used_points");
    writer.Uint(_character._progress.usedPoints);
    writer.Key("level");
    writer.Uint(_character._progress.level);
    writer.EndObject(); // end progress

    writer.Key("score");
    writer.StartArray(); // start score
    writer.Uint(_character._score.hp);
    writer.Uint(_character._score.mp);
    writer.Uint(_character._score.sta);
    writer.Uint(_character._score.maxhp);
    writer.Uint(_character._score.maxmp);
    writer.Uint(_character._score.maxsta);
    writer.EndArray(); // end score

    writer.EndObject(); // end

    std::string fileName = std::string("../playerfiles/") + _character._name + ".json";
    std::ofstream f(fileName.c_str());
    f << strbuff.GetString();

    if (f.good())
        printf("Successfully wrote character file at %s\n", fileName.c_str());
    else
        printf("Failed to write character file to %s\n", fileName.c_str());
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

    _character._equipment.weaponSet.style = WeaponStyle(dom["equipment"]["weapon_style"].GetInt());
    _character._equipment.weaponSet.twohand = WeaponType(dom["equipment"]["twohand_type"].GetInt());
    _character._equipment.weaponSet.twohandTier = dom["equipment"]["twohand_tier"].GetInt();
    _character._equipment.weaponSet.mainhand = WeaponType(dom["equipment"]["mainhand_type"].GetInt());
    _character._equipment.weaponSet.mainhandTier = dom["equipment"]["mainhand_tier"].GetInt();
    _character._equipment.weaponSet.offhand = WeaponType(dom["equipment"]["offhand_type"].GetInt());
    _character._equipment.weaponSet.offhandTier = dom["equipment"]["offhand_tier"].GetInt();
    _character._equipment.weaponSet.twohandDurability = dom["equipment"]["twohand_durability"].GetInt();
    _character._equipment.weaponSet.mainhandDurability = dom["equipment"]["mainhand_durability"].GetInt();
    _character._equipment.weaponSet.offhandDurability = dom["equipment"]["offhand_durability"].GetInt();
    _character._equipment.rangedType = RangedType(dom["equipment"]["ranged_type"].GetInt());
    _character._equipment.rangedTier = dom["equipment"]["ranged_tier"].GetInt();
    _character._equipment.armourType = ArmourType(dom["equipment"]["armour_type"].GetInt());
    _character._equipment.armourTier = dom["equipment"]["armour_tier"].GetInt();
    _character._equipment.rangedDurability = dom["equipment"]["ranged_durability"].GetInt();
    _character._equipment.armourDurability = dom["equipment"]["armour_durability"].GetInt();
    _character._equipment.implements = dom["equipment"]["implements"].GetInt();

    _character._items.maxsupplies = dom["items"]["maxsupplies"].GetInt();
    _character._items.maxammo = dom["items"]["maxammo"].GetInt();
    _character._items.supplies = dom["items"]["supplies"].GetInt();
    _character._items.ammo = dom["items"]["ammo"].GetInt();
    _character._items.money = dom["items"]["money"].GetInt();
    _character._items.banked = dom["items"]["banked"].GetInt();

    _character._loot.loot[0] = dom["loot"][0].GetInt();
    _character._loot.loot[1] = dom["loot"][1].GetInt();
    _character._loot.loot[2] = dom["loot"][2].GetInt();
    _character._loot.loot[3] = dom["loot"][3].GetInt();
    _character._loot.loot[4] = dom["loot"][4].GetInt();
    _character._loot.maxloot = dom["maxloot"].GetInt();

    _character._progress.experience = dom["progress"]["experience"].GetInt();
    _character._progress.abilityPoints = dom["progress"]["ability_points"].GetInt();
    _character._progress.usedPoints = dom["progress"]["used_points"].GetInt();
    _character._progress.level = dom["progress"]["level"].GetInt();

    _character._score.hp = dom["score"][0].GetInt();
    _character._score.mp = dom["score"][1].GetInt();
    _character._score.sta = dom["score"][2].GetInt();
    _character._score.maxhp = dom["score"][3].GetInt();
    _character._score.maxmp = dom["score"][4].GetInt();
    _character._score.maxsta = dom["score"][5].GetInt();
    // Delay should always be 0 at login

    printf("Loaded character %s from file!\n", name.c_str());
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
