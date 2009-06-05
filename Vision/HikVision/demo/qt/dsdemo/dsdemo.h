

#ifndef _DSDEMO_H_
#define _DSDEMO_H_

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

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
#include <qfile.h>

#include "DataType.h"
#include "ds40xxsdk.h"
#include "tmserverpu.h"

#define WRITE_FILE
#define BMPFILE
#define OSDDEMO
#define LOGO
#define PREVIEWOPEN

/* define RAWSTREAM */
#define YUVFILE

/*
 * define USE_PREVIEW_TIMER ;
 * #define TEST_GETORIIMAGE ;
 * #define MOTION_DEBUG ;
 * end define
 */
const int   WIN_L = 0;
const int   WIN_T = 0;
const int   WIN_W = 800;
const int   WIN_H = 700;
const int   FIXSIZE = 1024 * 5 / 2;                 /* per second */
const int   TIMECONST = 60 * 8;                     /* 8���� */
const int   TIMEDAYS = 24 * 60 * 60 * 1000 - 300;   /* �ճ�400ms�� */
const int   RunQuest = 1024 * 1024 * 10;
const int   LastSpace = 1024 * 1024 * 0;            /* KB */

#define LEFTCLICKED                     0
#define RIGHTCLICKED                    1

#define OutputErrorString(fmt, args...) fprintf(stderr, fmt, ## args)

//#define DEBUG
#ifdef DEBUG
#define OutputDebugString(fmt, args...) fprintf(stderr, fmt, ## args)
#else
#define OutputDebugString(fmt, args...)
#endif
struct _dsdemo
{
    int                     bPreviewOpen;           /* ��port���Ƿ�� */
    PREVIEWCONFIG           previewconf;            /* ��port�ڵ���ƵԤ������ */
    SDL_Overlay*            pOverlaySurface;        /* ��port�ڵ�YUVͼ���ڴ�ָ�� */
    pthread_t               hPreviewThread;         /* ��port�ڶ�Ӧ���߳�ָ�� */
    UINT                    bPreviewThreadRun;      /* ��port���߳����е�״�� ������״̬1,2,3���ѽ� */
    sem_t                   previewSem;             /* �ź��� */
    int                     cifqcif_flag;           /* cifqcif��־,�Ƿ�˫���뿪ʼ */
    VideoStandard_t         videostandard;          /* ��GetVideoPara�������صĸ�·����Ƶ��׼ */
    video_info              v_info;                 /* ��·��Ƶ�����Ϣ */
    video_info              videoDefault;
    int                     count;                  /* ��¼���ɵ��ļ����� */
    int                     Maxcount;
    int                     countdays;              /* ���е����� */
    int                     fullspace_flag;         /* Ӳ�̿ռ����ı�־ */
    int                     ChannelHandle;          /* ����ͨ���ľ�� file point */
    QFile*                  record_fd;              /* mp4�ļ���־�� */
    QFile*                  index_fd;               /* dat�ļ���־�� */
    pthread_mutex_t         major_start;            /* ��־��ͨ��д��ʼ */
    INDEX_NODE              nodeinfo;               /* ������¼I֡λ�úʹ�С������dat�ļ��� */
    QFile*                  cifqcif_fd;             /* ��ʶ�� */
    pthread_mutex_t         minor_start;            /* ��־�����д��ʼ */
    char                    file_head[40];          /* �ļ�ͷ */
    UCHAR                   filehead_flag;
    UCHAR                   cifqciffilehead_flag;
    char                    cifqciffile_head[40];   /* ��־���������ﱣ�� */
    int                     record_flag;            /* ��־��·�Ƿ�ʼ��¼ */
    int                     net_flag;               /* �����־ */
    unsigned char           motion_data[4 * 18];    /* PktMotionFrames ���Ⱦ���72 ��·motiondetect���� */
    int                     motion_flag;            /* ��־motion_flag�Ƿ��� */
    int                     motionSens;             /* �˶����ʱ�������� */
    RECT                    motion_rect;            /* �����ȫ����ʱ����������Motiondetect */
    RECT                    motion_list;            /* �����û��ȫ����ʱ����������Motiondetect�� */

    int                     net_head_flag;          /* here static�� ��ʾnet_head�Ƿ񴫳�ȥ�ˣ� */
    int                     subchan_net_head_flag;  /* ˫����ʱ����ͨ����head�Ƿ񴫳�ȥ�� */
    unsigned int            bit_rate_size;          /* ��¼��·���ʴ�С */
    int                     set_bit_rate;           /* ��������bit�ʴ�С */
    SDL_Rect                dstRect;                /* ��¼��·�ߴ�Ĵ�С */

    /* new add */
    Frame_info              frameinfo;
    Osd_info                osdinfo;
    int                     osd_flag;
    Logo_info               logoinfo;
    int                     logo_flag;

    PictureFormat_t         pictureFormat;
    PictureFormat_t         subpictureFormat;

    int                     MaxBps;
    BitrateControlType_t    brc;

    int                     audio_flag;
    RECT*                   maskRect;

    int                     imagefile_flag;         /* �Ƿ�����¼ */
    QFile*                  imagefile_fd;
    pthread_mutex_t         imagefile_fd_start;     /* ��־д��ʼ */
    unsigned char*          imageBuf;
    unsigned                imagewidth;
    unsigned                imageheight;
};

typedef struct _dsdemo  DSDEMO;

class Dsclass
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    DSDEMO*     dsdemo;
    int         SignPort;

    Dsclass(int port, int ChannelHandle, SDL_Surface* Screen);
    ~           Dsclass();

    /*
     * void AfterInit(int ChannelHandle, SDL_Surface *Screen);
     */
    int         GetVideoHeight();
    void        SetOsddemo();
    void        SetDsLogo();
    void        SetMaxBpsMode();
    void        SetOsdTranslucent(unsigned short Translucent);
    void        SetOsdBrightness(int Brightness);
    void        SetOsdPosX(unsigned short PosX);
    void        SetOsdPosY(unsigned short PosY);
    void        SetLogoTranslucent(unsigned short Translucent);
    void        SetLogoPosX(unsigned short PosX);
    void        SetLogoPosY(unsigned short PosY);
    void        SetEncoderByFlag();
    void        SetupMotion();
    void        DestoryMotion();
    void        SetdstRect(const int chan);
    void        SetMotion(const int chan);
    void        MotionEnable();
    void        SetPreview(UINT useSyncSem);
    void        CreateThread();
    void        DestoryThread();
    void        StopPreview();

    void        CreateFiles();
    void        CreateCifQcifFiles();
    void        CreateImageStreamFiles();
    void        StartCapture();
    void        StartSubCapture();
    void        StopSubCapture();
    void        StopCapture();
    void        end_record();
    void        end_subrecord();
    void        end_imagestream();

    void        GetImage();
    void        SetBrightScale(const int brightness);
    void        SetContrastScale(const int contrast);
    void        SetSaturationScale(const int saturation);
    void        SetHueScale(const int hue);
    void        SetFrameIQuantVal(int number);
    void        SetFrameBQuantVal(int number);
    void        SetFramePQuantVal(int number);
    void        SetFrameKeyFrameI(int number);
    void        SetFrameBFrames(int number);
    void        SetFrameFrameRate(int number);
    void        SetMask();

    void*       videopreview();

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    SDL_Surface*    pOverlayScreen;
    unsigned char*  logoptr;

    void            GetCardInfo();
};
#endif
