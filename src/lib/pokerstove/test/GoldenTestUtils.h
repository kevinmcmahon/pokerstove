#ifndef POKERSTOVE_TEST_GOLDENTESTUTILS_H_
#define POKERSTOVE_TEST_GOLDENTESTUTILS_H_

#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace pokerstove
{
namespace test
{

inline std::string trim(const std::string& input)
{
    const std::string::size_type begin =
        input.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos)
        return "";

    const std::string::size_type end =
        input.find_last_not_of(" \t\r\n");
    return input.substr(begin, end - begin + 1);
}

inline std::vector<std::string> split(const std::string& input, char delimiter)
{
    std::vector<std::string> out;
    std::stringstream stream(input);
    std::string item;
    while (std::getline(stream, item, delimiter))
        out.push_back(item);
    return out;
}

inline std::vector<std::string> splitWhitespace(const std::string& input)
{
    std::vector<std::string> out;
    std::istringstream stream(input);
    std::string token;
    while (stream >> token)
        out.push_back(token);
    return out;
}

inline std::string fixturePath(const std::string& name)
{
    return std::string(POKERSTOVE_TESTDATA_DIR) + "/" + name;
}

inline std::map<std::string, std::string>
parseRecord(const std::string& line, const std::string& fixtureName, int lineNumber)
{
    std::map<std::string, std::string> record;
    const std::vector<std::string> tokens = splitWhitespace(line);
    for (const std::string& token : tokens)
    {
        const std::string::size_type pos = token.find('=');
        if (pos == std::string::npos)
        {
            throw std::runtime_error(
                fixtureName + ":" + std::to_string(lineNumber) +
                ": expected key=value token, got: " + token);
        }

        record[token.substr(0, pos)] = token.substr(pos + 1);
    }
    return record;
}

inline std::vector<std::map<std::string, std::string>>
readRecords(const std::string& name)
{
    std::ifstream input(fixturePath(name).c_str());
    if (!input)
        throw std::runtime_error("unable to open fixture file: " + fixturePath(name));

    std::vector<std::map<std::string, std::string>> records;
    std::string line;
    int lineNumber = 0;
    while (std::getline(input, line))
    {
        ++lineNumber;
        const std::string stripped = trim(line);
        if (stripped.empty() || stripped[0] == '#')
            continue;
        records.push_back(parseRecord(stripped, name, lineNumber));
    }

    return records;
}

inline const std::string&
requireField(const std::map<std::string, std::string>& record,
             const std::string& key)
{
    const std::map<std::string, std::string>::const_iterator it = record.find(key);
    if (it == record.end())
        throw std::runtime_error("missing field: " + key);
    return it->second;
}

inline bool isEmptyField(const std::string& value)
{
    return value.empty() || value == "-";
}

inline bool parseBool(const std::string& value)
{
    if (value == "true")
        return true;
    if (value == "false")
        return false;
    throw std::runtime_error("invalid boolean: " + value);
}

inline std::vector<double> parseDoubles(const std::string& input)
{
    std::vector<double> values;
    for (const std::string& token : split(input, ','))
    {
        if (!token.empty())
            values.push_back(std::stod(token));
    }
    return values;
}

inline std::vector<std::string> parseList(const std::string& input, char delimiter)
{
    if (isEmptyField(input))
        return std::vector<std::string>();
    return split(input, delimiter);
}

}  // namespace test
}  // namespace pokerstove

#endif  // POKERSTOVE_TEST_GOLDENTESTUTILS_H_
