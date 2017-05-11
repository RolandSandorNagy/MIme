#include "Controller.h"
#include "View.h"
#include <ctime>


#define CMD_PREFIX "CMD::"
#define SAVE_FILE_NAME "save.csv"
#define NO_SUGGESTION "none:none;"
#define SUGG_DELIMETER_CHAR ';'
#define STROKE_DELIMETER_CHAR ':'
#define CURRENT_STROKE "ime--current"
#define LEFT_OVER_POSS "ime--lop"
#define ALTER "ime--alt"

namespace global
{
    extern View* hgView;

    extern bool isRunning;

    std::wstring s2ws(const std::string& str, int* size_needed)
    {
        *size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(*size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], *size_needed);
        return wstrTo;
    }
}


Controller::Controller(View* v)
{
    isActive = global::isRunning;
    view = v;
    last_msg_was_Show_or_Hide = false;
    there_is_more = false;
    more_left = 0;
    suggestions.clear();
}

void Controller::processMessage(char* recvbuf, int recvbuflen, int iResult)
{
    if(last_msg_was_Show_or_Hide)
    {
        last_msg_was_Show_or_Hide = false;
        return;
    }

    if(commandReceived(recvbuf))
        return;
    if(isActive)
        messageReceived(recvbuf, recvbuflen, iResult);
}

bool Controller::commandReceived(char* recvbuf)
{
    std::string str(recvbuf);
    if(str.substr(0, strlen(CMD_PREFIX)) == CMD_PREFIX)
    {
        processCommand(str.substr(strlen(CMD_PREFIX), str.size()));
        return true;
    }
    return false;
}

void Controller::messageReceived(char* recvbuf, int recvbuflen, int iResult)
{
    std::string sv_str(recvbuf);
    suggestions.clear();
    suggestions = createSuggestionVector(sv_str);

    //if(suggestions.size() != 0)
        proceedShow();
    //else
        //proceedHide();
}

std::vector<Suggestion> Controller::createSuggestionVector(std::string sv_str)
{
    std::vector<Suggestion> suggs;
    suggs.clear();
    alters.clear();
    if(sv_str == NO_SUGGESTION)
        return suggs;

    return buildSuggestions(sv_str);
}

std::vector<Suggestion> Controller::buildSuggestions(std::string sv_str)
{
    std::vector<Suggestion> suggs;
    std::string s;
    std::stringstream s1(sv_str);
    while( getline(s1, s, SUGG_DELIMETER_CHAR) )
    {
        std::stringstream sparts(s);
        std::string translation;
        std::string stroke;
        getline(sparts, stroke, STROKE_DELIMETER_CHAR);
        getline(sparts, translation, STROKE_DELIMETER_CHAR);

        if(stroke == CURRENT_STROKE)
            storeCurrentStroke(&sparts, translation);
        else if(stroke == ALTER)
            storeAlters(&sparts, translation);
        else if(stroke == LEFT_OVER_POSS){
            there_is_more = true;
            more_left = atoi(translation.c_str());
        }
        else
            addSuggestionToSuggs(&suggs, stroke, translation);

    }
    if(there_is_more)   there_is_more = false;
    else                more_left = 0;

    return suggs;
}

void Controller::storeAlters(std::stringstream *sparts, std::string first_stroke)
{
    std::string stroke = first_stroke;
    std::string translation;
    getline(*sparts, translation, STROKE_DELIMETER_CHAR);

    addSuggestionToSuggs(&alters, stroke, translation);

    while( getline(*sparts, stroke, STROKE_DELIMETER_CHAR) )
    {
        getline(*sparts, translation, STROKE_DELIMETER_CHAR);
        addSuggestionToSuggs(&alters, stroke, translation);
    }
}

void Controller::storeCurrentStroke(std::stringstream *sparts, std::string stroke)
{
    std::string translation;
    getline(*sparts, translation, STROKE_DELIMETER_CHAR);
    int size_needed;
    if(translation == "none")
        current_stroke.setWStroke(global::s2ws(stroke, &size_needed));
    else
        current_stroke.setWStroke(global::s2ws(translation, &size_needed));
    current_stroke.setWText(global::s2ws(stroke, &size_needed));
}

void Controller::addSuggestionToSuggs(std::vector<Suggestion> *suggs, std::string stroke, std::string translation)
{
    int size_needed;
    std::wstring wstroke = global::s2ws(stroke, &size_needed);
    std::wstring wtranslation = global::s2ws(translation, &size_needed);
    Suggestion s(0, wstroke, wtranslation);
    suggs->push_back(s);
}

void Controller::processCommand(std::string str)
{
    if(!isActive && str != "RESUME" && str != "STOP")
        return;
    else if(str == "PAUSE")
        proceedPause();
    else if(str == "RESUME")
        proceedResume();
    else if(str == "STOP")
        proceedStop();
    else if(str == "SHOW")
    {
        last_msg_was_Show_or_Hide = true;
        proceedShow();
    }
    else if(str == "HIDE")
    {
        last_msg_was_Show_or_Hide = true;
        proceedHide();
    }
    else if(str == "SAVE")
        proceedSave();
}

void Controller::proceedPause()
{
    isActive = false;
    proceedHide();
}

void Controller::proceedResume()
{
    isActive = true;
}

void Controller::proceedStop()
{
    global::isRunning = false;
    view->closeView();
}

void Controller::proceedShow()
{
    view->displaySuggestions(suggestions, alters, current_stroke, more_left);
}

void Controller::proceedHide()
{
    view->hidePopup();
}


void Controller::proceedSave()
{
    // TODO: encoding is not right for some languages
    std::cout << "saving..." << std::endl;
    std::wofstream f(SAVE_FILE_NAME, std::wfstream::in | std::wfstream::out | std::wfstream::app);
    f << current_stroke.getWStroke();
    f << L", ";
    f << current_stroke.getWText();
    f << L", ";

    time_t t = time(0);
    struct tm * now = localtime( & t );
    f    << (now->tm_year + 1900) << L"-"
         << (now->tm_mon + 1) << L"-"
         <<  now->tm_mday
         << std::endl;

    f.close();
}


