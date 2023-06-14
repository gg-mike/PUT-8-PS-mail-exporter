#include "Communication.h"

#include <iostream>
#include <vector>
#include <regex>
#include <cmath>

#include <curl/curl.h>

#include "Colors.h"
#include "Configuration.h"

using std::string;
using std::vector;

size_t write_callback(char* buffer, size_t size, size_t nmemb, void* outstream) {
  string* out = static_cast<string*>(outstream);
  
  out->append(buffer, size * nmemb);
  
  return size * nmemb;
}

size_t read_callback(char *buffer, size_t size, size_t nmemb, void *instream) {
  string *in = static_cast<string*>(instream);
  size_t len = size * nmemb;
  
  if (in->empty()) return 0;
  if (len > in->size()) len = in->size();
  memcpy(buffer, in->data(), len);
  in->erase(0, len);
  
  return len;
}

int xferinfo_callback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
  Progress *progress = static_cast<Progress*>(p);
  char msg[200];
  
  sprintf(msg, "(%ld/%ld) %s", progress->currentStep, progress->steps, progress->msg.c_str());
  string m{msg};
  if (m.size() > 80) {
    m.erase(m.begin() + 77, m.end());
    m += "...";
  }


  double curr = progress->isDownload ? dlnow : ulnow;
  double total = progress->isDownload ? dltotal : ultotal;
  double percent = total != 0 ? curr / total : 0;
  int bars = (int)round(percent * 25.0) - 1;
  string progressBar = string(bars < 0 ? 0: bars, '=') + ">";
  
  printf("\r%-80s [%-25s] %3d%%", m.c_str(), progressBar.c_str(), int(percent * 100.0));

  return 0;
}

CURL* init(const Configuration& cfg, bool isSrc) {
  CURL* curl = curl_easy_init();
  if (!curl) {
    std::cerr << ERROR("[ERROR]: Failed to initialize libcurl") << std::endl;
    exit(EXIT_FAILURE);
  }
  string who = isSrc? "SRC" : "DST";
  curl_easy_setopt(curl, CURLOPT_USERNAME, cfg.getString(who + "_USERNAME").c_str());
  curl_easy_setopt(curl, CURLOPT_PASSWORD, cfg.getString(who + "_PASSWORD").c_str());

  curl_easy_setopt(curl, CURLOPT_VERBOSE, (long)cfg.getBool("VERBOSE"));
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo_callback);
  curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &cfg.progress);

  return curl;
}

string request(const Configuration& cfg, bool isSrc, const string& url, const string& customRequest = "") {
  string data;
  CURL* curl = init(cfg, isSrc);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  if (customRequest != "") curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, customRequest.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
  CURLcode res = curl_easy_perform(curl);

  xferinfo_callback((void*)&cfg.progress, 1, 1, 1, 1);

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  if (res != CURLE_OK) {
    std::cerr << ERROR("[ERROR]: Failed to download data: " << curl_easy_strerror(res) <<) "\n";  
    exit(EXIT_FAILURE);
  }

  return data;
}

bool requestTry(const Configuration& cfg, bool isSrc, const string& url, const string& customRequest = "") {
  string data;
  CURL* curl = init(cfg, isSrc);

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  if (customRequest != "") curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, customRequest.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
  CURLcode res = curl_easy_perform(curl);

  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

  curl_easy_cleanup(curl);
  curl_global_cleanup();
  
  return res == CURLE_OK;
}

void upload(const Configuration& cfg, const string& url, const string& data) {
  CURL* curl = init(cfg, false);
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  curl_easy_setopt(curl, CURLOPT_READDATA, &data);
  curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (long)strlen(data.c_str()));
  CURLcode res = curl_easy_perform(curl);

  xferinfo_callback((void*)&cfg.progress, 1, 1, 1, 1);

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  if (res != CURLE_OK) {
    std::cerr << ERROR("[ERROR]: Failed to upload data: " << curl_easy_strerror(res) <<) "\n"; 
    exit(EXIT_FAILURE);
  }
}

vector<string> getIdVec(const Configuration& cfg, const string& folder) {
  string res = request(cfg, true, "imaps://imap.gmail.com/" + folder, "FETCH 1:* (UID)");
  vector<string> ids;
  std::regex rgx(R"(\(UID ([0-9]+)\))");
  std::sregex_iterator it = std::sregex_iterator(res.begin(), res.end(), rgx);
  for (;it != std::sregex_iterator(); it++) ids.push_back((*it).str(1));
  return ids;
}

std::string getEmail(const Configuration& cfg, const string& folder, const string& id) {
  string res = request(cfg, true, "imaps://imap.gmail.com/" + folder + "/;UID=" + id);
  return res;
}

void createFolder(const Configuration& cfg, const string& folder) {
  if (requestTry(cfg, false, "imaps://imap.gmail.com", "EXAMINE " + folder)) return;
  request(cfg, false, "imaps://imap.gmail.com", "CREATE " + folder);
  std::cout << std::endl;  
}

void sendEmail(const Configuration& cfg, const string& folder, const string& data) {
  upload(cfg, "imaps://imap.gmail.com/" + folder, data);
}
