#ifndef CONFIG_H
#define CONFIG_H

#include <vector>


class ConfigDB
{
private:
    struct m_lookupPair {
        const char *key;  // key name before the value.
        void *value;  // key generic value.
    };
    
    std::vector<struct lookupPair> m_configDatabase;
public:
    const char *m_configPath = "CONFIG NOT SET";
    
    
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
    void *lookupKey(const char *key);
};


#endif CONFIG_H
