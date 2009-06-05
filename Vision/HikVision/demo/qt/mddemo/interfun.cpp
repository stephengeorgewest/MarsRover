#include "interfun.h"

static QsStruct sQstruct;

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

void SetsQstruct(QsStruct* inter)
{
    memcpy(&sQstruct, inter, sizeof(QsStruct));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void InterReadDataCallBack(int StockHandle, char* pPacketBuffer, int nPacketSize)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *interpmf) (int, char*, int) = sQstruct.pmf;
    (ph->*interpmf) (StockHandle, pPacketBuffer, nPacketSize);
}

#ifdef RAWSTREAM

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void InterImageStream(unsigned int channelNumber, void* context)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *interImageStreampmf)
        (
            unsigned int,
            void*
        ) = sQstruct.ImageStreamCallbackpmf;
    (ph->*interImageStreampmf) (channelNumber, context);
}
#endif
