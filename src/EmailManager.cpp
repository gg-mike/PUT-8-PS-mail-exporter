#include "Communication.h"

void cursorShow(bool isShow) { printf(isShow ? "\e[?25h" : "\e[?25l"); }

int main(int argc, char** argv) {

  if (argc < 2) {
    std::cerr << ERROR("[ERROR]: Missing configuration filepath") << std::endl;
    exit(EXIT_FAILURE);
  }
  Configuration cfg(argv[1]);
  const std::string srcFolder = cfg.getString("SRC_FOLDER");
  const std::string srcUser = cfg.getString("SRC_USERNAME");
  const std::string dstFolder = cfg.getString("DST_FOLDER");
  const std::string dstUser = cfg.getString("DST_USERNAME");
    
  cursorShow(false);
  cfg.progress = { 1, 1, "Downloading list of emails in folder [" + srcFolder + "] from <" + srcUser + ">", true };
  std::vector<std::string> ids = getIdVec(cfg, srcFolder);
  cursorShow(true);
  std::cout << "\nFound " << ids.size() << " emails in folder [" << srcFolder << "]. Do you want to continue? (Y/n) ";
  std::cout.flush();
  
  char choice;
  std::cin.get(choice);
  if (!(choice == 'Y' || choice == 'y' || choice == '\n')) {
    std::cout << "\rExiting...";
    return 0;
  }
    
  cursorShow(false);
  std::cout << "Starting download of " << ids.size() << " emails from <" << srcUser << ">" << std::endl;
  std::vector<std::string> emails;
  
  for (std::size_t i = 0; i < ids.size(); i++) {
    cfg.progress = { i + 1, ids.size(), "Downloading emails (UID=" + ids[i] + ")", true };
    emails.push_back(getEmail(cfg, srcFolder, ids[i]));
  }
  std::cout << "\nSuccessfully downloaded all emails." << std::endl;

  cfg.progress = { 1, 1, "Creating folder [" + dstFolder + "] for user <" + dstUser + ">", true };
  createFolder(cfg, dstFolder);
  std::cout << "Starting upload of " << emails.size() << " emails to folder [" << dstFolder << "] and for user <" << dstUser << ">" << std::endl;

  for (std::size_t i = 0; i < emails.size(); i++) {
    cfg.progress = { i + 1, emails.size(), "Uploading emails (UID=" + ids[i] + ")", false };
    sendEmail(cfg, dstFolder, emails[i]);
  }
  cursorShow(true);

  return 0;
}
