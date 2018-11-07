#include "Character.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class CharacterFileLoader
{
public:
    friend class Character;

    CharacterFileLoader(const char* name, const std::string& pwd);

    void PopulateCharacterData();

    operator bool() const
    {
        return !_name.empty();
    }
    const std::string& GetName() const
    {
        return _name;
    }
private:
    bool CheckDomValid() const;
    std::string _name;
    rapidjson::Document _dom;
    Character _character;
};
