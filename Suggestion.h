#ifndef SUGGESTION_H_INCLUDED
#define SUGGESTION_H_INCLUDED

#include "includes.h"


class Suggestion
{

public:  /* members */


private: /* members */
    int multiplicity;
    std::wstring wtext;
    std::wstring wstroke;


public:  /* methods */
    Suggestion(int, std::wstring, std::wstring);
    Suggestion();
    int getMultiplicity();
    std::wstring getWText();
    std::wstring getWStroke();
    void setMultiplicity(int);
    void setWText(std::wstring);
    void setWStroke(std::wstring);


private: /* methods */

};

struct suggestion_compare_operator
{
    inline bool operator() (Suggestion& sug1, Suggestion& sug2)
    {
        return (sug1.getMultiplicity() < sug2.getMultiplicity());
    }
};


#endif // SUGGESTION_H_INCLUDED
