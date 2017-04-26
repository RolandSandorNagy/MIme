#include "globals.h"
#include "Config.h"
#include "Server.h"
#include <fstream>

//#define LOCK_FILE_LOCATION "C:\\Users\\Rol\\AppData\\Local\\plover\\plover\\lock.txt"
#define LOCK_FILE_NAME "\\ime_lock.txt"
#define DONE 0
#define CONFIG_FILE_NOT_FOUND -1
#define LOCK_FILE_EXIST -2
#define DELETE_FILE_FAILED "Error deleting file"


bool lockFileExist(std::string);
void makeLockFile(std::string);
void deleteLockFile(std::string);


int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    Config config;
    if(config.file_not_found)
        return CONFIG_FILE_NOT_FOUND;

    std::string path = config.getFullPath() + LOCK_FILE_NAME;

    if(lockFileExist(path))
        return LOCK_FILE_EXIST;
    else
        makeLockFile(path);


    pthread_t windowThread;
    pthread_t serverThread;

    pthread_create(&windowThread, NULL, ViewNS::wThreadMethod, (void*)&hThisInstance);

    while(!global::hgView){}
    global::hgView->setPopupTimeout(config.getPopupTimeout());

    pthread_create(&serverThread, NULL, ServerNS::sThreadMethod, (void*)&config);

    void *windowThreadStatus;
    void *serverThreadStatus;

    pthread_join(windowThread, &windowThreadStatus);
    pthread_join(serverThread, &serverThreadStatus);

    deleteLockFile(path);

    return DONE;
}

bool lockFileExist(std::string fname)
{
    std::ifstream lockFile(fname.c_str());
    if(lockFile)
        return true;
    return false;
}

void makeLockFile(std::string fname)
{
    std::ofstream lockFile(fname.c_str());
    if(lockFile.is_open())
        lockFile.close();
}

void deleteLockFile(std::string fname)
{
    if(remove(fname.c_str()) != 0)
        std::cerr << DELETE_FILE_FAILED << std::endl;
}
