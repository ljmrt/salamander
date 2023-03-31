#include <core/Config/Config.h>
#include <core/Logging/ErrorLogger.h>

#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>


ConfigDB::ConfigDB()
{
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
        std::string valueDelimiters = ":";
        std::string eolDelimiters = "#";

        size_t valueDelimitersPosition = line.find_first_of(valueDelimiters);
        size_t eolDelimitersPosition = line.find_first_of(eolDelimiters);
        
        std::string strippedLine = line.substr(0, eolDelimitersPosition);
        if (strippedLine.size() == 0) {  // if the line is empty or only contains comments(stripped out).
            continue;
        }
            
        std::string key = strippedLine.substr(0, valueDelimitersPosition);
        std::string value = strippedLine.substr((valueDelimitersPosition + valueDelimiters.length()), strippedLine.length());

        // remove value seperator garbage.
        int spacePosition = key.find_first_of(' ');
        key = key.substr(0, spacePosition);
        
        int firstMarkPosition = value.find_first_of('"');
        if (firstMarkPosition == (int)std::string::npos) {  // if there are no quotation marks.
            value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
        } else {
            std::string toFirstMark = value.substr(0, firstMarkPosition);
            value.erase(0, toFirstMark.length());
            value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
        }

        struct m_lookupPair linePair{};
        linePair.key = key;
        linePair.value = value;
        m_configDatabase.push_back(linePair);
    }
}

std::string ConfigDB::lookupKey(std::string key)
{
    for (struct m_lookupPair configPair : m_configDatabase) {
        if (key.compare(configPair.key) == 0) {
            return configPair.value;
        }
    }

    throwDebugException("Key: " + key + " value not explicitly found in database.");
    return nullptr;
}

void ConfigDB::dumpDatabase()
{
    std::ostringstream databaseBuffer;
    
    databaseBuffer << "# *****DATABASE DUMP START***** #\n";
    for (struct m_lookupPair configPair : m_configDatabase) {
        databaseBuffer << (configPair.key + " : " + configPair.value + "\n");
    }
    databaseBuffer << "# *****DATABASE DUMP END******* #\n";

    std::cout << databaseBuffer.str();
}
