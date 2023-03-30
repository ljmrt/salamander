#include <core/Config/Config.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>


ConfigDB::ConfigDB()
{
    m_configDatabase.insert(m_configDatabase.begin(), (struct m_lookupPair){"ERROR", (void *)"CONFIG NOT SET"});
}

ConfigDB::ConfigDB(std::string filePath)
{
    loadConfig(filePath);
}

void ConfigDB::loadConfig(std::string filePath)
{
    m_configPath = filePath;
    
    std::ifstream inputFile(filePath);
    for (std::string line; getline(inputFile, line);) {  // get every line in file seperately.
        std::string valueDelimiter = " : ";
        std::string eolDelimiters = " #";

        int valueDelimiterPosition = line.find(valueDelimiter);
        int eolDelimitersPosition = line.find_first_of(eolDelimiters);
        std::string key = line.substr(0, valueDelimiterPosition);
        std::string value = line.substr((valueDelimiterPosition + valueDelimiter.length()), eolDelimitersPosition);

        std::cout << eolDelimitersPosition << std::endl;
        std::cout << ("key: \"" + key + "\" value: \"" + value + "\"") << std::endl;
    }
}

void *ConfigDB::lookupKey(std::string key)
{
    for (struct m_lookupPair configPair : m_configDatabase) {
        if (key.compare(configPair.key) == 0) {
            return configPair.value;
        }
    }

    throwDebugException("Key: " + key + " value not explicitly found in database.");
    return nullptr;
}
