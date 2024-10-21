#include "Config.h"

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the config file: " << filename << std::endl;
        return false;
    }

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        line.erase(0, line.find_first_not_of(" \n\r\t"));

        // Skip empty lines
        if (line.empty()) continue;

        // Check for section headers
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }

        // Split line into key and value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            data[currentSection][key] = value;
        }
    }

    file.close();
    return true;
}

std::string Config::getString(const std::string& section, const std::string& key) {
    if (data.find(section) != data.end()) {
        if (data[section].find(key) != data[section].end()) {
            std::string value = data[section][key];

            // Remove quotes from the start and end if they exist
            if (value.length() >= 2 && value.front() == '\"' && value.back() == '\"') {
                value = value.substr(1, value.length() - 2); // Remove quotes
            }

            return value;
        }
        else {
            std::cerr << "Warning: Key '" << key << "' not found in section '" << section << "'." << std::endl;
        }
    }
    else {
        std::cerr << "Warning: Section '" << section << "' not found." << std::endl;
    }
    return ""; // Return empty string or a default value if section/key not found
}

int Config::getInt(const std::string& section, const std::string& key) {
    return std::stoi(data[section][key]);
}

float Config::getFloat(const std::string& section, const std::string& key) {
    return std::stof(data[section][key]);
}