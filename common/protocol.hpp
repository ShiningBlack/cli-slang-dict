#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <nlohmann/json.hpp>

namespace MadSlangDict {

struct SlangDefinition {
    std::string term;
    std::string definition;
    std::string origin;
};

// 将结构体转换为 JSON
nlohmann::json to_json(const SlangDefinition& def);

// 将 JSON 转换为结构体
SlangDefinition from_json(const nlohmann::json& j);

} // namespace MadSlangDict


#endif // PROTOCOL_H