#include "View.h"
#include <sstream>


#define WINDOW_NAME "MIME POPUP WINDOW"
#define MIME_APP_NAME "MIME Windows App"
#define DEFAULT_POPUP_HEIGHT 300
#define DEFAULT_POPUP_WIDTH 300
#define PLOVER_GRAY 240
#define MARGIN 5
#define MULT_WIDTH 75
#define SUG_WIDTH 10
#define MAX_LINES 20
#define MAX_SUGGS 10
#define SECOND 1000
#define GAP 10
#define CHAR_WIDTH 10
#define LINE_HEIGHT 18


namespace ViewNS
{
    bool on;
    TCHAR szClassName[ ] = _T(WINDOW_NAME);

    LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    void* wThreadMethod(void*);
    void* toThreadMethod(void* view);
}

namespace global
{
    extern View* hgView;

    extern bool isRunning;

    extern std::wstring s2ws(const std::string&, int*);
}


View::View(HINSTANCE* hInst)
{
    hInstance = hInst;

    popupHeight = DEFAULT_POPUP_HEIGHT;
    popupWidth  = DEFAULT_POPUP_WIDTH;
    ViewNS::on = true;
    maxThreadId = 0;
    ln = 0;

    bgColor = RGB(PLOVER_GRAY, PLOVER_GRAY, PLOVER_GRAY);
    fontColor = RGB(227, 143, 41);
    greenColor = RGB(178, 255, 120);
    blackColor = RGB(0, 0, 0);
    whiteColor = RGB(255, 255, 255);
    greyColor = RGB(140,140,140);
    gray2Color = RGB(190,190,190);
    borderColor = RGB(172,172,172);

    initWincl(hInst);

    if(!register_Class())
        return;
    createWindow();
}

View::View() {}

View::~View() {}

void View::initWincl(HINSTANCE *hInst)
{
    wincl.hInstance = *hInst;
    wincl.lpszClassName = ViewNS::szClassName;
    wincl.lpfnWndProc = ViewNS::WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = CreateSolidBrush(bgColor);
}

bool View::register_Class()
{
    if (!RegisterClassEx (&wincl))
        return false;
    return true;
}

void View::createWindow()
{
    hwnd = CreateWindowEx ( WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                            ViewNS::szClassName,
                            _T(MIME_APP_NAME),
                            WS_POPUP,
                            0,
                            0,
                            popupWidth,
                            popupHeight,
                            NULL,
                            NULL,
                            *hInstance,
                            NULL);
}

void View::showPopup(std::vector<Suggestion> suggestions, std::vector<Suggestion> alters, Suggestion current, int more_left)
{
    ShowWindow(hwnd, SW_SHOW);
    clearPopup(MAX_LINES + 1);
    drawCurrentStringOnPopUp(current);
    for(int i = suggestions.size() - 1; i >= 0 /*&& suggestions.size() - i < MAX_SUGGS*/; --i)
        drawStringOnPopUp(suggestions[i]);

    if(alters.size() > 0)
        drawAltersOnPopUp(alters);

    if(more_left > 0)
        drawLeftMoreNumberOnPopUp(more_left);

    drawPopupBorder();
}

void View::drawAltersOnPopUp(std::vector<Suggestion> alters)
{
	PAINTSTRUCT ps;
	RECT rect;

	HDC hDC = GetDC(hwnd);

    rect.left   = 1;
    rect.top    = popupHeight -  popupH4 - popupH3;
    rect.right  = popupWidth;
    rect.bottom = popupHeight - popupH4 + 2 * MARGIN;

    FillRect(hDC, &rect, (HBRUSH) CreateSolidBrush(whiteColor));


    MoveToEx(hDC, 1, popupH1 + popupH2 - 1, NULL);
    LineTo(hDC, popupWidth - 1, popupH1 + popupH2 - 1);

    for(int i = 0; i < alters.size(); ++i)
    {
        SetBkColor(hDC, whiteColor);
        SetTextColor(hDC, blackColor);

        rect.left   = MARGIN;
        rect.top    = 2 * MARGIN + popupH1 + popupH2 + i * LINE_HEIGHT;
        rect.right  = popupW1;
        rect.bottom = rect.top + LINE_HEIGHT;

        DrawText(hDC, alters[i].getWStroke().c_str(), alters[i].getWStroke().length(), &rect, 0);

        SetBkColor(hDC, whiteColor);
        SetTextColor(hDC, blackColor);

        rect.left   = rect.right + popupW2;
        rect.right += popupW2 + popupW3;
        std::wstring ws = formatOutline(alters[i].getWText());
        DrawText(hDC, ws.c_str(), ws.length(), &rect, 0);
    }

	EndPaint(hwnd, &ps);
	handleNextLine(hDC);
}

void View::drawPopupBorder()
{
	HDC hDC = GetDC(hwnd);

	//SetBkColor(hDC, borderColor);
	//SetTextColor(hDC, borderColor);

    MoveToEx(hDC, 1, 1, NULL);
    LineTo(hDC, popupWidth, 1);

    MoveToEx(hDC, 1, 1, NULL);
    LineTo(hDC, 1, popupHeight);

    MoveToEx(hDC, popupWidth, popupHeight -1, NULL);
    LineTo(hDC, 1, popupHeight -1);

    MoveToEx(hDC, popupWidth - 1, popupHeight, NULL);
    LineTo(hDC, popupWidth - 1, 1);
}

void View::drawLeftMoreNumberOnPopUp(int more_left)
{
	PAINTSTRUCT ps;
	RECT rect;
	RECT rect_bk;

	HDC hDC = GetDC(hwnd);

	SetBkColor(hDC, gray2Color);
	SetTextColor(hDC, bgColor);

    std::stringstream ss;
    ss << more_left;
    std::string more_left_str = ss.str() + " more available";
    int size_needed;
    std::wstring more_left_wstr = global::s2ws(more_left_str, &size_needed);

    rect_bk.left   = 1;
    rect_bk.top    = popupHeight -  popupH4 + 2 * MARGIN;
    rect_bk.right  = popupWidth;
    rect_bk.bottom = popupHeight;

    FillRect(hDC, &rect_bk, (HBRUSH) CreateSolidBrush(gray2Color));

    rect.left   = (popupWidth / 2) - (more_left_str.size() * CHAR_WIDTH / 3 );
    rect.top    = popupHeight -  popupH4 + 2 * MARGIN;
    rect.right  = popupWidth;
    rect.bottom = popupHeight;

    DrawText(hDC, more_left_wstr.c_str(), more_left_wstr.length(), &rect, 0);

    MoveToEx(hDC, 1, rect.top /*popupH1*/, NULL);
    LineTo(hDC, popupWidth, rect.top /*popupH1*/);

	EndPaint(hwnd, &ps);
}

void View::hidePopup()
{
    ShowWindow(hwnd, SW_HIDE);
}

void View::movePopup(int x, int y, int width, int height)
{
	MoveWindow(hwnd, x, y, width, height, true);
}

void View::drawCurrentStringOnPopUp(Suggestion current)
{
	PAINTSTRUCT ps;
	RECT rect_bk;
	RECT rect;

	HDC hDC = GetDC(hwnd);

	SetBkColor(hDC, gray2Color);
	SetTextColor(hDC, greenColor);


    rect_bk.left   = 1;
    rect_bk.top    = 1;
    rect_bk.right  = popupWidth - 1;
    rect_bk.bottom = popupH1;

    FillRect(hDC, &rect_bk, (HBRUSH) CreateSolidBrush(gray2Color));


    rect.left   = MARGIN;
    rect.top    = MARGIN;
    rect.right  = popupW1;
    rect.bottom = popupH1;

    DrawText(hDC, current.getWStroke().c_str(), current.getWStroke().length(), &rect, 0);

	SetBkColor(hDC, gray2Color);
	SetTextColor(hDC, greenColor);

    rect.left   = MARGIN + rect.right + popupW2;
    rect.right += popupWidth;
    std::wstring ws = formatOutline(current.getWText());
    DrawText(hDC, ws.c_str(), ws.length(), &rect, 0);

    MoveToEx(hDC, 1, popupH1, NULL);
    LineTo(hDC, popupWidth, popupH1);

	EndPaint(hwnd, &ps);
}

void View::drawStringOnPopUp(Suggestion s)
{
	PAINTSTRUCT ps;
	RECT rect;

	HDC hDC = GetDC(hwnd);

	SetBkColor(hDC, bgColor);
	SetTextColor(hDC, blackColor);

    rect.left   = MARGIN;
    rect.top    = 2 * MARGIN + popupH1 + ln * LINE_HEIGHT;
    rect.right  = popupW1;
    rect.bottom = rect.top + LINE_HEIGHT;
    DrawText(hDC, s.getWStroke().c_str(), s.getWStroke().length(), &rect, 0);

    SetBkColor(hDC, bgColor);
	SetTextColor(hDC, fontColor);

    rect.left   = rect.right + popupW2;
    rect.right += popupW2 + popupW3;
    std::wstring ws = formatOutline(s.getWText());
    DrawText(hDC, ws.c_str(), ws.length(), &rect, 0);

	EndPaint(hwnd, &ps);
	handleNextLine(hDC);
}

std::wstring View::formatOutline(std::wstring outline)
{
    int size_needed;
    std::string str( outline.begin(), outline.end() );
    std::string str2 = "";
    for(int i = 0; i < str.size(); ++i)
    {
        if(str[i] == '/')
        {
            str2 += ' ';
            str2 += str[i];
            str2 += ' ';
        }
        else
            str2 += str[i];
    }
    std::wstring ws = global::s2ws(str2, &size_needed);
    return ws;
}

void View::handleNextLine(HDC hDC)
{
	ln = (ln > MAX_LINES ? 0 : ln + 1);
	if (ln == 0)
    clearPopup(MAX_LINES + 1);
}

void View::clearPopup(int l)
{
    if(ln + l <= MAX_LINES)
        return;

    RECT rect;
    HDC hDC = GetDC(hwnd);
    rect.left = 0;
    rect.top = 0;
    rect.right = DEFAULT_POPUP_WIDTH;
    rect.bottom = DEFAULT_POPUP_HEIGHT;
    FillRect(hDC, &rect, CreateSolidBrush(bgColor));
    SetROP2(hDC, R2_NOTXORPEN);
    ln = 0;
}

void View::closeView()
{
    SendMessage(hwnd, WM_DESTROY, 0, 0);
}

void View::displaySuggestions(std::vector<Suggestion> suggestions, std::vector<Suggestion> alters, Suggestion current, int more_left)
{
    Sleep(100);

    hidePopup();
    adjustPopUp(suggestions.size(), alters.size(), getMaxStrokeLength(suggestions, alters), getMaxTextLength(suggestions, alters), more_left);
    showPopup(suggestions, alters, current, more_left);
    hideTimeout();
}

void View::adjustPopUp(int entries, int alts, int maxTextLength, int maxStrokeLength, int more_left)
{
	POINT p = getCaretPosition();

	popupW1 = maxTextLength * CHAR_WIDTH;
	popupW2 = GAP;
	popupW3 = maxStrokeLength * CHAR_WIDTH + 2 * MARGIN;
	popupH1 = LINE_HEIGHT + 2 * MARGIN;
	popupH2 = (entries * LINE_HEIGHT + 2 * MARGIN);
	popupH3 = (alts * LINE_HEIGHT + 2 * MARGIN);
	popupH4 = 0;
	if(more_left > 0)
        popupH4 += (LINE_HEIGHT + 2 * MARGIN);
	popupWidth = popupW1 + popupW2 + popupW3;
	popupHeight = popupH1 + popupH2 + popupH3 + popupH4;

	avoidScreenEdges(&p);
	movePopup(p.x + 3, p.y - 3, popupWidth, popupHeight);

	return;
}

void View::avoidScreenEdges(POINT* p)
{
    RECT actualDesktop;
    GetWindowRect(GetDesktopWindow(), &actualDesktop);

    int W = actualDesktop.right;                // get screen width
    int H = GetSystemMetrics(SM_CYFULLSCREEN);  // get screen height
    int w = popupWidth;                         // get popup width
    int h = popupHeight;                        // get popup height
    int wb = W - w;                             // width border
    int hb = H - h + LINE_HEIGHT;               // height border

    if(p->x < 0 || p->x > W || p->y < 0 || p->y > actualDesktop.bottom)
        return;

    if(p->x <= wb && p->y <= hb)
        return;
    else if(p->x > wb && p->y <= hb) {// Right side of screen
        p->x  = wb;
        p->y += LINE_HEIGHT;
    } else if(p->x <= wb && p->y > hb) // Bottom side of screen
        p->y = hb - (H - p->y) - LINE_HEIGHT;
    else if(p->x > wb && p->y > hb)  // Bottom right corner of screen
    {
        p->x = wb;
        p->y = hb - (H - p->y) - LINE_HEIGHT;
    }
}


POINT View::getCaretPosition()
{
    POINT *point = new POINT();
    point->x = 0;
    point->y = 0;

	HWND Wnd = NULL;
	HWND Result = NULL;
	DWORD TId, PId;

    Result = GetFocus();
    Wnd = GetForegroundWindow();
	if (Result || !Wnd)
        return *point;

    TId = GetWindowThreadProcessId(Wnd, &PId);
    if (   !AttachThreadInput(GetCurrentThreadId(), TId, TRUE)
        || GetCaretPos(point) == 0)
        return *point;

    Result = GetFocus();
    ClientToScreen(Result, point);
    AttachThreadInput(GetCurrentThreadId(), TId, FALSE);
	return *point;
}

int View::getMaxTextLength(std::vector<Suggestion> suggestions, std::vector<Suggestion> alters)
{
    int max = 0;
    for(int i = suggestions.size() - 1; i >= 0 /*&& suggestions.size() - i < MAX_SUGGS*/; --i)
        if((int)suggestions[i].getWText().size() > max)
            max = suggestions[i].getWText().size();
    for(int i = alters.size() - 1; i >= 0 /*&& alters.size() - i < MAX_SUGGS*/; --i)
        if((int)alters[i].getWText().size() > max)
            max = alters[i].getWText().size();
    return max;
}

int View::getMaxStrokeLength(std::vector<Suggestion> suggestions, std::vector<Suggestion> alters)
{
    int max = 0;
    for(int i = suggestions.size() - 1; i >= 0 /*&& suggestions.size() - i < MAX_SUGGS*/; --i)
        if((int)suggestions[i].getWStroke().size() > max)
            max = suggestions[i].getWStroke().size();
    for(int i = alters.size() - 1; i >= 0 /*&& alters.size() - i < MAX_SUGGS*/; --i)
        if((int)alters[i].getWStroke().size() > max)
            max = alters[i].getWStroke().size();
    return max;
}

void View::hideTimeout()
{
    pthread_t toThread;
    maxThreadId++;
    pthread_create(&toThread, NULL, ViewNS::toThreadMethod, (void*)maxThreadId);
    timeoutThread = &toThread;
}

int View::getPopupTimeout()
{
    return popupTimeout;
}

void View::setPopupTimeout(int val)
{
    popupTimeout = val;
}


// ----------------------------------------------------- //
/* ***************************************************** */
// ----------------------------------------------------- //



LRESULT CALLBACK ViewNS::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        case WM_CHAR:
            if (wParam == VK_ESCAPE) {
                global::hgView->hidePopup();
            }
            break;
        default: return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void* ViewNS::wThreadMethod(void* hInst)
{
    View view((HINSTANCE*)hInst);
    global::hgView = &view;

    MSG messages;
    while (GetMessage (&messages, NULL, 0, 0) && global::isRunning)
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return (void*)messages.wParam;
}

void* ViewNS::toThreadMethod(void* id)
{
    int to = ((View*)global::hgView)->getPopupTimeout();
    if(to == 0)
        return (void*) 0;
    Sleep(to * SECOND);
    if(((View*)global::hgView)->maxThreadId != (INT)id)
        return (void*) 1;
    ((View*)global::hgView)->hidePopup();
    ((View*)global::hgView)->maxThreadId = 0;
    return (void*) 0;
}
