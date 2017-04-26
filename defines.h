#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED



#define _WIN32_WINNT 0x501
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "12345"

#define UNICODE

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif


#endif // DEFINES_H_INCLUDED
