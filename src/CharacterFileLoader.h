#include "Character.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class Attributes;

class CharacterFileLoader
{
public:
    friend class Character;

    static CharacterFileLoader SaveCharacterToFile(const Character& character);
    static CharacterFileLoader CreateNewCharacterFile(const char* name, const std::string& pwd,
                                                      Attributes attr);

    // ctor for when saving to a player file
    CharacterFileLoader(const Character& character);

    // ctor for when loading from player file
    CharacterFileLoader(const char* name, const std::string& pwd);

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
