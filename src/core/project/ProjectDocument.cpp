#include "ProjectDocument.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
std::string escapeJson(const std::string& value)
{
    std::string result;
    for (char character : value) {
        switch (character) {
        case '\\': result += "\\\\"; break;
        case '"': result += "\\\""; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\t': result += "\\t"; break;
        default: result += character; break;
        }
    }
    return result;
}

void skipWhitespace(const std::string& text, size_t& position)
{
    while (position < text.size() &&
           std::isspace(static_cast<unsigned char>(text[position]))) {
        ++position;
    }
}

bool readString(const std::string& text, size_t& position, std::string& output)
{
    skipWhitespace(text, position);
    if (position >= text.size() || text[position] != '"') return false;
    ++position;
    std::string result;
    bool escaped = false;
    while (position < text.size()) {
        const char character = text[position++];
        if (escaped) {
            switch (character) {
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            default: result += character; break;
            }
            escaped = false;
        } else if (character == '\\') {
            escaped = true;
        } else if (character == '"') {
            output = result;
            return true;
        } else {
            result += character;
        }
    }
    return false;
}

bool isAssetKey(const std::string& key)
{
    return key == "png.base" ||
        key == "png.mouthOpen" ||
        key == "png.mouthClosed" ||
        key == "png.blink" ||
        key == "png.expression" ||
        key == "model3d.path" ||
        key == "camera.faceCascade" ||
        key == "camera.landmarkModel" ||
        key.find(".imagePath") != std::string::npos;
}
}

bool ProjectDocument::load(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        lastError = "Project file not found";
        return false;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    const std::string text = buffer.str();
    size_t position = 0;
    skipWhitespace(text, position);
    values.clear();
    lastError.clear();
    return position < text.size() && text[position] == '{' ? loadJson(text) : loadLegacy(text);
}

bool ProjectDocument::loadLegacy(const std::string& text)
{
    std::istringstream input(text);
    std::string line;
    while (std::getline(input, line)) {
        const size_t separator = line.find('=');
        if (separator != std::string::npos) values[line.substr(0, separator)] = line.substr(separator + 1);
    }
    version = CurrentVersion;
    values["schema.migratedFrom"] = "legacy-v1";
    return true;
}

bool ProjectDocument::loadJson(const std::string& text)
{
    const size_t versionKey = text.find("\"version\"");
    const size_t valuesKey = text.find("\"values\"");
    if (versionKey == std::string::npos || valuesKey == std::string::npos) {
        lastError = "Invalid project JSON envelope";
        return false;
    }
    const size_t versionColon = text.find(':', versionKey);
    if (versionColon == std::string::npos) {
        lastError = "Invalid project version";
        return false;
    }
    const int loadedVersion = std::atoi(text.c_str() + versionColon + 1);
    if (loadedVersion <= 0 || loadedVersion > CurrentVersion) {
        lastError = loadedVersion > CurrentVersion ? "Project was created by a newer VAvatar version" : "Invalid project version";
        return false;
    }
    const size_t valuesOpen = text.find('{', valuesKey);
    if (valuesOpen == std::string::npos) {
        lastError = "Invalid project values object";
        return false;
    }
    size_t position = valuesOpen + 1;
    while (true) {
        skipWhitespace(text, position);
        if (position >= text.size()) {
            lastError = "Unexpected end of project JSON";
            return false;
        }
        if (text[position] == '}') break;
        std::string key;
        std::string value;
        if (!readString(text, position, key)) {
            lastError = "Invalid JSON key";
            return false;
        }
        skipWhitespace(text, position);
        if (position >= text.size() || text[position] != ':') {
            lastError = "Missing JSON key separator";
            return false;
        }
        ++position;
        if (!readString(text, position, value)) {
            lastError = "Invalid JSON value";
            return false;
        }
        values[key] = value;
        skipWhitespace(text, position);
        if (position >= text.size()) {
            lastError = "Unexpected end of project JSON";
            return false;
        }
        if (text[position] == ',') ++position;
        else if (text[position] != '}') {
            lastError = "Invalid JSON values separator";
            return false;
        }
    }
    version = CurrentVersion;
    if (loadedVersion < CurrentVersion) values["schema.migratedFrom"] = "json-v" + std::to_string(loadedVersion);
    return true;
}

bool ProjectDocument::save(const std::string& path) const
{
    const std::filesystem::path projectPath(path);
    if (!projectPath.parent_path().empty()) std::filesystem::create_directories(projectPath.parent_path());
    std::ofstream file(projectPath);
    if (!file) return false;
    std::vector<std::string> keys;
    keys.reserve(values.size());
    for (const auto& entry : values) keys.push_back(entry.first);
    std::sort(keys.begin(), keys.end());
    file << "{\n  \"version\": " << CurrentVersion << ",\n  \"values\": {\n";
    for (size_t i = 0; i < keys.size(); ++i) {
        file << "    \"" << escapeJson(keys[i]) << "\": \"" << escapeJson(values.at(keys[i])) << "\""
             << (i + 1 < keys.size() ? "," : "") << "\n";
    }
    file << "  }\n}\n";
    return static_cast<bool>(file);
}

std::string ProjectDocument::get(const std::string& key, const std::string& fallback) const
{
    const auto found = values.find(key);
    return found == values.end() ? fallback : found->second;
}

void ProjectDocument::set(const std::string& key, const std::string& value)
{
    values[key] = value;
}

std::vector<ProjectAssetIssue> ProjectDocument::missingAssets(const std::string& folder) const
{
    std::vector<ProjectAssetIssue> result;
    for (const auto& entry : values) {
        if (!isAssetKey(entry.first) || entry.second.empty()) continue;
        std::filesystem::path path(entry.second);
        if (path.is_relative()) path = std::filesystem::path(folder) / path;
        if (!std::filesystem::exists(path)) result.push_back({entry.first, entry.second, path.string()});
    }
    return result;
}

bool ProjectDocument::replaceAsset(const std::string& key, const std::string& replacement, const std::string& folder)
{
    if (!std::filesystem::exists(replacement)) return false;
    try { values[key] = std::filesystem::relative(replacement, folder).generic_string(); }
    catch (...) { values[key] = replacement; }
    return true;
}
