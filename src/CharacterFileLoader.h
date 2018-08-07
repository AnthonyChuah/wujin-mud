#include "Character.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

struct CharacterTemplate
{
};

class CharacterFileLoader
{
public:
    CharacterFileLoader(const char* name, const std::string& pwd);

    // Injects data into CharacterTemplate
    bool LoadCharacterData(CharacterTemplate& templait);

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
