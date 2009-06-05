#ifndef _EXVIDEO_H_
#define _EXVIDEO_H_

#include "QSDLWidget.h"

struct DsStruct
{
    Dsclass*    dsclass;
    void*  (Dsclass:: *pmf) ();
};

#if 1
struct QsStruct
{
    QSDLWidget*     qsclass;
    void (QSDLWidget:: *pmf) (int, char*, int, int);
    void (QSDLWidget:: *Startpmf) (int);
    void (QSDLWidget:: *Stoppmf) (int);
    void (QSDLWidget:: *Messagepmf) (char*, int);
    int  (QSDLWidget:: *IPpmf) (int, char*);
    int  (QSDLWidget:: *Passwordpmf) (char*, int, char*, int);
    int  (QSDLWidget:: *Checkpmf) (int, char*, int, char*, int, char*);
    void (QSDLWidget:: *Disconnectpmf) (int, char*);
    void (QSDLWidget:: *ImageStreamCallbackpmf) (unsigned int, void*);
};
#endif
void*   intermediary(void* );

void    SetsQstruct(QsStruct* inter);

void    InterStream(int channel, char* databuf, int frametype, int framelength);

void    InterStart(int port);

void    InterStop(int port);

void    InterMessage(char* buf, int length);

int     InterIP(int channel, char* sIP);

int     InterPassword(char* username, int namelen, char* password, int passlen);

int     InterCheck(int channel, char*  username, int namelen, char*  password, int passlen, char*  sIP);

void    InterDisconnect(int nport, char* sIP);

void    InterImageStreamCallback(unsigned int channelNumber, void* context);
#endif
