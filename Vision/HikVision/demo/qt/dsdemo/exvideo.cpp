#include "exvideo.h"

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
void InterStream(int channel, char* databuf, int frametype, int framelength)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *pmf) (int, char*, int, int) = sQstruct.pmf;
    (ph->*pmf) (channel, databuf, frametype, framelength);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void InterStart(int port)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *Startpmf) (int) = sQstruct.Startpmf;
    (ph->*Startpmf) (port);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void InterStop(int port)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *Stoppmf) (int) = sQstruct.Stoppmf;
    (ph->*Stoppmf) (port);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void InterMessage(char* buf, int length)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *Messagepmf) (char*, int) = sQstruct.Messagepmf;
    (ph->*Messagepmf) (buf, length);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int InterIP(int channel, char* sIP)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    int (QSDLWidget:: *IPpmf) (int, char*) = sQstruct.IPpmf;

    int temp = (ph->*IPpmf) (channel, sIP);

    printf(" IP return value: %d \n", temp);
    return temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int InterPassword(char* username, int namelen, char* password, int passlen)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    int (QSDLWidget:: *Passwordpmf) (char*, int, char*, int) = sQstruct.Passwordpmf;

    int temp = (ph->*Passwordpmf) (username, namelen, password, passlen);

    printf(" Password return value: %d \n", temp);
    return temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int InterCheck(int channel, char* username, int namelen, char* password, int passlen, char* sIP)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    int (QSDLWidget:: *Checkpmf) (int, char*, int, char*, int, char*) = sQstruct.Checkpmf;

    int temp = (ph->*Checkpmf) (channel, username, namelen, password, passlen, sIP);

    printf(" Check return value: %d \n", temp);
    return temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void InterDisconnect(int nport, char* sIP)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *Disconnectpmf) (int, char*) = sQstruct.Disconnectpmf;
    (ph->*Disconnectpmf) (nport, sIP);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void* intermediary(void* ptr)
{
    DsStruct*   pa = (DsStruct*)ptr;
    Dsclass*    ph = pa->dsclass;
    void*  (Dsclass:: *pmf) () = pa->pmf;

    return((ph->*pmf) ());
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void InterImageStreamCallback(unsigned int channelNumber, void* context)
{
    QSDLWidget*     ph = sQstruct.qsclass;
    void (QSDLWidget:: *ImageStreamCallbackpmf)
        (
            unsigned int,
            void*
        ) = sQstruct.ImageStreamCallbackpmf;
    (ph->*ImageStreamCallbackpmf) (channelNumber, context);
}
