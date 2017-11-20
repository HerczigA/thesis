#ifndef CLOSING_H_INCLUDED
#define CLOSING_H_INCLUDED

void setBackTermios(Threadcommon *arg,struct termios *old);

void closeOnFAIL(void *arg);

#endif // CLOSING_H_INCLUDED
