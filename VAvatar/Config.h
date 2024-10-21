#pragma once


#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

class Config {
public:
    bool load(const std::string& filename);

    std::string getString(const std::string& section, const std::string& key);
    int getInt(const std::string& section, const std::string& key);
    float getFloat(const std::string& section, const std::string& key);

private:
    std::map<std::string, std::map<std::string, std::string>> data;
};