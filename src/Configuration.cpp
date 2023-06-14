#include "Configuration.h"

#include <array>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "Colors.h"

using std::cerr;
using std::string;

void Configuration::loadConfigFile(const string& configFilepath) {
  std::ifstream ifs(configFilepath, std::ifstream::in);
  if (!ifs.is_open()) {
    cerr << ERROR("[ERROR]: Config file \"" UNDL(<< configFilepath <<)) ERROR( "\" not found") << std::endl;
    exit(EXIT_FAILURE);
  }

  string line;
  int i = 0;
  do {
    i++;
    getline(ifs, line);
    if (line == "" || line[0] == '#') continue;

    size_t pos = 0;
    string key, value;
    pos = line.find("=");
    if (pos == string::npos) {
      cerr <<  ERROR("[ERROR]: Bad format in config (ln: " << i << "): required format \"<key>=<value>\", got=\"" << line << "\")\n" );
      exit(EXIT_FAILURE);
    }
    key = line.substr(0, pos);
    value = line.erase(0, pos + 1);

    options[key] = value;

  } while (ifs.good());
}

void Configuration::validateConfiguration() const {
  for (string reqField: requiredFields) {
    if (options.find(reqField) == options.end()) {
      cerr << ERROR("[ERROR]: Required field \"" << reqField << "\" is missing from configuration") << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

bool Configuration::isValueMissing(const string& key) const {
  if (options.find(key) != options.end()) return false;
  cerr << ERROR("[ERROR]: Option \"" << key << "\" is missing") << std::endl;
  return true;
}

Configuration::Configuration(const string& configFilepath) {
  loadConfigFile(configFilepath);
  validateConfiguration();
}

string Configuration::getString(const string& key, const string& fallback) const {
  if(isValueMissing(key)) return fallback;
  return options.at(key);
}

int Configuration::getInt(const string& key, int fallback) const {
  if(isValueMissing(key)) return fallback;

  try {
      return std::stoi(options.at(key));
  } catch (const std::invalid_argument &e) {
      cerr << ERROR("[ERROR]: Option \"" << key << "\" could not be cast to integer") << std::endl;
      return fallback;
  } catch (const std::out_of_range &e) {
    cerr << ERROR("[ERROR]: Option \"" << key << "\" exceeds integer limits") << std::endl;
      return fallback;
  }
}

bool Configuration::getBool(const string& key, bool fallback) const {
  if(isValueMissing(key)) return fallback;

  string value = options.at(key);
  std::for_each(value.begin(), value.end(), [](char & c) { c = std::tolower(c); });

  if (value == "1" || value == "t" || value == "true") return true;
  else if (value == "0" || value == "f" || value == "false") return false;

  std::cout << ERROR("[ERROR]: Option \"" << key << "\" does not match any possible boolean values") << std::endl;
  return fallback; 
}
