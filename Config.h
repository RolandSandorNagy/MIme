#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "includes.h"


class Config
{
public: /* members */
    bool file_not_found;


private: /* members */
    int popupTimeout;
    int suggest_by;
    std::string host;
    std::string port;
    char folderPath[MAX_PATH];
    std::string full_path;

public: /* methods */
    Config();
    ~Config();
    std::string getHost();
    std::string getPort();
    int getPopupTimeout();
    std::string getFullPath();

private: /* methods */
    void handleProgramPath();
    void readDataFromConfigFile(std::ifstream*);

};


#endif // CONFIG_H_INCLUDED
