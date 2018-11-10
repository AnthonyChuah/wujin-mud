#include "Character.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class CharacterFileLoader
{
public:
    friend class Character;

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
