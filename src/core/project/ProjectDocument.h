#pragma once
#include <string>
#include <unordered_map>
#include <vector>
struct ProjectAssetIssue{std::string key,storedPath,resolvedPath;};
class ProjectDocument{public:static constexpr int CurrentVersion=3;int version=CurrentVersion;std::unordered_map<std::string,std::string> values;bool load(const std::string&);bool save(const std::string&)const;std::string get(const std::string&,const std::string& fallback={})const;void set(const std::string&,const std::string&);std::vector<ProjectAssetIssue> missingAssets(const std::string&)const;bool replaceAsset(const std::string&,const std::string&,const std::string&);const std::string& error()const{return lastError;}private:bool loadLegacy(const std::string&);bool loadJson(const std::string&);std::string lastError;};
