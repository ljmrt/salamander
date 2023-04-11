#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>


// TODO: customizable config file locations.
// TODO: GUI config.
// TODO: customize common configs through GUI.
// TODO: comment logging, display on GUI.
// TODO: configurable logging colors.
class ConfigDB
{
private:
    struct m_lookupPair {
        std::string key;  // key name before the value.
        std::string value;  // key generic value(has to be casted).
    };
    
    std::vector<struct m_lookupPair> m_configDatabase;
public:
    std::string m_configPath = "CONFIG NOT SET";
    
    
    ConfigDB();
    ConfigDB(const char *filePath);
    
    // load a .scfg file as the current database.
    //
    // @param filePath path of the desired file to load.
    void loadConfig(const char *filePath);
    
    // lookup a key's value in the database.
    //
    // @param key key to lookup the value of.
    // @return key value.
    std::string lookupKey(std::string key);

    // dump the config database into std::cout formatted as a .scfg file.
    void dumpDatabase();
};


#endif  // CONFIG_H
