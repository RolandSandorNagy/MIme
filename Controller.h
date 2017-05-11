#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "includes.h"
#include "Suggestion.h"

class View;


class Controller
{

public: /* members */


private: /* members */
    View* view;
    std::ofstream file;
    std::vector<Suggestion> suggestions;
    std::vector<Suggestion> alters;
    Suggestion current_stroke;
    bool last_msg_was_Show_or_Hide;
    bool there_is_more;
    bool isActive;
    int more_left;


public:  /* methods */
    Controller(View*);
    void processMessage(char*, int, int);


private: /* methods */
    bool commandReceived(char*);
    void messageReceived(char*, int, int);
    std::vector<Suggestion> createSuggestionVector(std::string s);
    std::vector<Suggestion> buildSuggestions(std::string);
    void storeAlters(std::stringstream*, std::string);
    void storeCurrentStroke(std::stringstream*, std::string);
    void addSuggestionToSuggs(std::vector<Suggestion>*, std::string, std::string);
    void processCommand(std::string);
    void proceedPause();
    void proceedResume();
    void proceedStop();
    void proceedShow();
    void proceedHide();
    void proceedSave();

};


#endif // CONTROLLER_H_INCLUDED
