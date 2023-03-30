#include <core/Config/Config.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <string>
#include <fstream>


ConfigDB::ConfigDB()
{
    m_configDatabase.insert(m_configDatabase.begin(), (struct m_lookupPair){"ERROR", "CONFIG NOT SET"});
}

ConfigDB::ConfigDB(const char *filePath)
{
    loadConfig(filePath);
}

void ConfigDB::loadConfig(const char *filePath)
{
    std::ifstream inputFile(filePath);
    for (std::string line; getline(inputFile, line);) {  // get every line in file seperately.
        
    }
}

void *ConfigDB::lookupKey(const char *key)
{
    for (struct m_lookupPair configPair : m_configDatabase) {
        if (strcmp(key, configPair.key) == 0) {
            return configPair.value;
        }
    }

    throwDebugException("Key: " + *key + " value not explicitly found in database.");
    return nullptr;
}
