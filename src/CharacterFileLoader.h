#include "Character.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class CharacterFileLoader
{
public:
    CharacterFileLoader(const char* name);

    bool LoadCharacterData(Character& character);

    operator bool() const
    {
        return !_name.empty();
    }
    const std::string& GetName() const
    {
        return _name;
    }
private:
    std::string _name;
    rapidjson::Document _dom;
};
