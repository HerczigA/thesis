#ifndef CLOSING_H_INCLUDED
#define CLOSING_H_INCLUDED

void setBackTermios(Threadcommon *fileconf,struct termios *old);

void closeOnFAIL(void *arg);

#endif // CLOSING_H_INCLUDED
