#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <array>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "Colors.h"

struct Progress {
  std::size_t currentStep;
  std::size_t steps;
  std::string msg{""};
  bool isDownload;
};

class Configuration {
  std::map<std::string, std::string> options;
  const std::array<std::string, 4> requiredFields{ "SRC_USERNAME", "SRC_PASSWORD", "DST_USERNAME", "DST_PASSWORD" };

  void loadConfigFile(const std::string& configFilepath);
  void validateConfiguration() const;
  bool isValueMissing(const std::string& key) const;

public:
  Progress progress;

  Configuration(const std::string& configFilepath);
  
  std::string getString(const std::string& key, const std::string& fallback = "") const;
  int getInt(const std::string& key, int fallback = 0) const;
  bool getBool(const std::string& key, bool fallback = false) const;
};

#endif /*__CONFIGURATION_H__*/
