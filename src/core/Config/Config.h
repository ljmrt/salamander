#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>


class ConfigDB
{
private:
    struct m_lookupPair {
        std::string key;  // key name before the value.
        void *value;  // key generic value.
    };
    
    std::vector<struct m_lookupPair> m_configDatabase;
public:
    std::string m_configPath = "CONFIG NOT SET";
    
    
    ConfigDB();
    ConfigDB(std::string filePath);
    
    // load a .scfg file as the current database.
    //
    // @param filePath path of the desired file to load.
    void loadConfig(std::string filePath);
    // lookup a key's value in the database.
    //
    // @param key key to lookup the value of.
    // @return key value.
    void *lookupKey(std::string key);
};


#endif  // CONFIG_H
