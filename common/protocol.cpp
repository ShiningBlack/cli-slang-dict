#include "protocol.hpp"

nlohmann::json MadSlangDict::to_json(const SlangDefinition &def)
{
    return nlohmann::json();
}

MadSlangDict::SlangDefinition MadSlangDict::from_json(const nlohmann::json& j)
{
    return MadSlangDict::SlangDefinition();
}