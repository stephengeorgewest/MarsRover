#ifndef _QSDLWIDGET_H_
#define _QSDLWIDGET_H_

#include <qwidget.h>
#include <qstring.h>

#include "mddemo.h"

/*
 =======================================================================================================================
 * define CIRCLE
 =======================================================================================================================
 */
class QSDLWidget : public QWidget
{
    Q_OBJECT

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    QSDLWidget(QWidget* parent, const char* name);
    ~           QSDLWidget();
    void        SetEnv();

    void        ReadDataCallBack(int StockHandle, char* pPacketBuffer, int nPacketSize);
    void        IPChange(const QString& ipaddress);
    void        ServerChanSet(int chan);

    int         OpenStream();
    void        FileStreamClose(int style);

    void        OpenVideoOut();
    void        CloseVideoOut();

    void        mousePressEvent(QMouseEvent* e);
    void        mouseDoubleClickEvent(QMouseEvent* e);

    long int    RetTotalPort();
    char*       RetIP();
    int         RetChan();
    char*       RetFilePosition();
    int         RetSoundPort();
    int         PlaySound();
    int         StopSound();

    void        ImageStreamCallback(unsigned int channelNumber, void* context);

    public slots : void FileOpen (const QString&);

    void    SameSetting();
    void    process_time();

signals:
    void    portchanged(int);
    void    playStatus(int);
    void    BitRateChange(int, int);
    void    RateChange(int, unsigned long);
    void    FrameChange(int, unsigned long);

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    Dsclass **       mddemoclass;
    long int        totalport;
    int             SelectPort;
    int             playSoundPort;
    int             sameSetting;
    int             fullArea;
    QTime **         time;
    QTimer **        timer;
    int             RunStream;
    int             RunVideoOut;
    QString         FileName;

#ifdef CIRCLE
    pthread_mutex_t mutex;
#endif
#ifdef RAWSTREAM
    QFile*          rawFile;
    unsigned char*  imagebuf;
#endif
    int             GetChanNum();
    int             PtInRect(SDL_Rect rect, int x, int y);
    void            StringtoChar(const QString& aFile, char* cstr);
    inline MDDEMO*  RetMDDEMO(int);
};
#endif
