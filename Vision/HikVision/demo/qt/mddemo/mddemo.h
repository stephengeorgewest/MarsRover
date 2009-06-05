#ifndef _MDDEMO_H_
#define _MDDEMO_H_

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include <qthread.h>
#include <qfile.h>

#include <sys/vfs.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>

#include "DataType.h"
#include "ds40xxsdk.h"
#include "tmclientpu.h"

#define WRITE_FILE
#define BMPFILE
#define OSDDEMO
#define LOGO
#define PREVIEWOPEN
#define YUVFILE

const int   WIN_L = 0;
const int   WIN_T = 0;
const int   WIN_W = 800;
const int   WIN_H = 700;

const int   address[4] = { 192, 0, 1, 63 };

/* define RAWSTREAM */
#define NODECODE        0
#define DECODEFILE      1
#define DECODESTREAM    2

#define DEBUG_OUT

#define OutputErrorString(fmt, args...) fprintf(stderr, fmt, ## args)
#ifdef DEBUG_OUT
#define OutputDebugString(fmt, args...) fprintf(stderr, fmt, ## args)
#else
#define OutputDebugString(fmt, args...)
#endif
typedef SDL_Overlay*    PSDL_Overlay;
class                   QPlayOverThread;
class                   QDisplayThread;

struct _mddemo
{
    int                 SignPort;
    int                 bDisplayOpen;       /* ��port���Ƿ�� */
    pthread_mutex_t     DisplayPrepare;     /* �����־��Ϊ���ж�display�߳��Ƿ��Ѿ��˳���ʾ */
    int                 playType;           /* NULL or file or stream */
    PREVIEWCONFIG       displayconf;        /* ��port�ڵ���ƵԤ������ */
    PSDL_Overlay        pOverlaySurface;    /* ��port�ڵ�YUVͼ���ڴ�ָ�� */
    PSDL_Overlay        pOverLay[5];
    int                 OverLayPrepare;

    QPlayOverThread*    playThread;
    QDisplayThread*     displayThread;
    UINT                bDisplayThreadRun;  /* ��port���߳����е�״�� */

    sem_t               displaySem;         /* �ź��� */
    sem_t               FileEndSem;
    sem_t               beginSem;
    sem_t               changesem;

    int                 ChannelHandle;      /* ����ͨ���ľ�� file point */
    SDL_Rect            dstRect;            /* ��¼��·�ߴ�Ĵ�С */
    pthread_mutex_t     OverDisplay;        /* �´������������� */

    int                 clienthandle;
    unsigned long       datasum;
    int                 serverchan;
    char                serverip[16];
    char                filename[100];

    /*
     * QFile *mdfile;
     */
};

typedef struct _mddemo  MDDEMO;

class QPlayOverThread : public QThread
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    QPlayOverThread(MDDEMO* demo);
    virtual void    run();

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    MDDEMO*     mdOver;
};

class QDisplayThread : public QThread
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    QDisplayThread(MDDEMO* demo, SDL_Surface* display);
    virtual void    run();

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    MDDEMO*         mdPlay;
    SDL_Surface*    pOverlayScreen;
};

class   Dsclass
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    Dsclass(int port, int ChannelHandle, SDL_Surface* Screen);
    ~           Dsclass();
    MDDEMO*     mddemo;
    bool        IsPlayOver();

    void        StartFilePlay();
    void        StartStreamPlay(char* pFileHead, int nHeadSize);
    void        StopPlay();

    void        startDisplay();
    void        stopDisplay();

    void        startDisplayThread();
    void        stopDisplayThread();
    void        stopPlayOverThread();

    void        SetdstRect(const int chan);

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    SDL_Surface*    pOverlayScreen;
};
#endif
