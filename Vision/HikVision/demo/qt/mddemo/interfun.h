#ifndef _INTERFUN_H_
#define _INTERFUN_H_

#include "QSDLWidget.h"

struct QsStruct
{
    QSDLWidget*     qsclass;
    void (QSDLWidget:: *pmf) (int, char*, int);

#ifdef RAWSTREAM
    void (QSDLWidget:: *ImageStreamCallbackpmf) (unsigned int, void*);
#endif
};

void    SetsQstruct(QsStruct* inter);
void    InterReadDataCallBack(int, char* , int);

#ifdef RAWSTREAM
void    InterImageStream(unsigned int, void* );
#endif
#endif
