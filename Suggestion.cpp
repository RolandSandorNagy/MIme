#include "Suggestion.h"


Suggestion::Suggestion(int m, std::wstring wt)
{
    multiplicity = m;
    wtext = wt;
}

Suggestion::Suggestion(int m, std::wstring wt, std::wstring ws)
{
    multiplicity = m;
    wtext = wt;
    wstroke = ws;
}

Suggestion::Suggestion()
{

}

Suggestion::~Suggestion()
{

}

int Suggestion::getMultiplicity()
{
    return multiplicity;
}

std::wstring Suggestion::getWText()
{
    return wtext;
}

std::wstring Suggestion::getWStroke()
{
    return wstroke;
}

void Suggestion::setMultiplicity(int m)
{
    multiplicity = m;
}

void Suggestion::setWText(std::wstring wt)
{
    wtext = wt;
}

void Suggestion::setWStroke(std::wstring ws)
{
    wstroke = ws;
}
