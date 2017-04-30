#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "includes.h"
#include "Suggestion.h"

class View;
class Server;


class Controller
{

public: /* members */


private: /* members */
    View* view;
    Server* server;
    std::ofstream file;
    bool isActive;
    std::wstring lastInput;
    std::vector<std::wstring> inputHistory;
    std::vector<Suggestion> suggestions;
    Suggestion current_stroke;
    bool there_is_more;
    int more_left;
    bool last_msg_was_Show_or_Hide;


public:  /* methods */
    Controller(View*, Server*);
    Controller();
    ~Controller();
    void processMessage(char*, int, unsigned int);


private: /* methods */
    bool commandReceived(char*);
    void messageReceived(char*, int , unsigned int);
    std::vector<Suggestion> createSuggestionVector(std::string s);
    std::vector<Suggestion> buildSuggestions(std::string);
    void storeCurrentStroke(std::stringstream*, std::string);
    void addSuggestionToSuggs(std::vector<Suggestion>*, std::string, std::string);
    POINT getCaretPosition();
    void processCommand(std::string);
    void proceedPause();
    void proceedResume();
    void proceedStop();
    void proceedShow();
    void proceedHide();
    void proceedSave();

};


#endif // CONTROLLER_H_INCLUDED
