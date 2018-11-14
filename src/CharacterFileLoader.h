#include "Character.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class Attributes;

class CharacterFileLoader
{
public:
    friend class Character;

    static void SaveCharacterToFile(const Character& character);

    // ctor for when loading from player file
    CharacterFileLoader(const char* name, const std::string& pwd);

    // ctor for when writing a new player file
    CharacterFileLoader(const char* name, const std::string& pwd, Attributes attr);

    operator bool() const
    {
        return !_name.empty();
    }
    const std::string& GetName() const
    {
        return _name;
    }
private:
    bool CheckDomValid(const rapidjson::Document& dom) const;
    void PopulateCharacterData(const rapidjson::Document& dom);

    std::string _name;
    Character _character;
};
