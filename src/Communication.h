#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include <string>
#include <vector>

#include "Configuration.h"

std::vector<std::string> getIdVec(const Configuration& cfg, const std::string& folder);
std::string getEmail(const Configuration& cfg, const std::string& folder, const std::string& id);
void createFolder(const Configuration& cfg, const std::string& folder);
void sendEmail(const Configuration& cfg, const std::string& folder, const std::string& data);

#endif /*__COMMUNICATION_H__*/
