#include "CommandParser.h"

#include <boost/algorithm/string.hpp>

namespace Parse
{

std::vector<std::string> TokenizeCommand(const std::string& command)
{
    std::vector<std::string> tokens;
    boost::split(tokens, command,
                 [](char c)
                 {
                     return c == ' ';
                 },
                 boost::token_compress_on);
    return tokens;
}

} // namespace Parse
