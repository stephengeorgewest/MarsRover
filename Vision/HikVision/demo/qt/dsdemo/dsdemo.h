

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
const int   TIMECONST = 60 * 8;                     /* 8分钟 */
const int   TIMEDAYS = 24 * 60 * 60 * 1000 - 300;   /* 空出400ms来 */
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
    int                     bPreviewOpen;           /* 该port口是否打开 */
    PREVIEWCONFIG           previewconf;            /* 该port口的视频预览定义 */
    SDL_Overlay*            pOverlaySurface;        /* 该port口的YUV图像内存指针 */
    pthread_t               hPreviewThread;         /* 该port口对应的线程指针 */
    UINT                    bPreviewThreadRun;      /* 该port口线程运行的状况 ，三种状态1,2,3？费解 */
    sem_t                   previewSem;             /* 信号量 */
    int                     cifqcif_flag;           /* cifqcif标志,是否双编码开始 */
    VideoStandard_t         videostandard;          /* 用GetVideoPara函数返回的各路的视频标准 */
    video_info              v_info;                 /* 该路视频相关信息 */
    video_info              videoDefault;
    int                     count;                  /* 记录生成的文件个数 */
    int                     Maxcount;
    int                     countdays;              /* 运行的天数 */
    int                     fullspace_flag;         /* 硬盘空间满的标志 */
    int                     ChannelHandle;          /* 各个通道的句柄 file point */
    QFile*                  record_fd;              /* mp4文件标志符 */
    QFile*                  index_fd;               /* dat文件标志符 */
    pthread_mutex_t         major_start;            /* 标志主通道写开始 */
    INDEX_NODE              nodeinfo;               /* 用来记录I帧位置和大小，用于dat文件中 */
    QFile*                  cifqcif_fd;             /* 标识符 */
    pthread_mutex_t         minor_start;            /* 标志多编码写开始 */
    char                    file_head[40];          /* 文件头 */
    UCHAR                   filehead_flag;
    UCHAR                   cifqciffilehead_flag;
    char                    cifqciffile_head[40];   /* 标志符，在这里保留 */
    int                     record_flag;            /* 标志该路是否开始记录 */
    int                     net_flag;               /* 网络标志 */
    unsigned char           motion_data[4 * 18];    /* PktMotionFrames 长度就是72 各路motiondetect数据 */
    int                     motion_flag;            /* 标志motion_flag是否开启 */
    int                     motionSens;             /* 运动侦测时的灵敏度 */
    RECT                    motion_rect;            /* 这个是全屏的时候用来设置Motiondetect */
    RECT                    motion_list;            /* 这个是没有全屏的时候用来设置Motiondetect的 */

    int                     net_head_flag;          /* here static， 表示net_head是否传出去了？ */
    int                     subchan_net_head_flag;  /* 双编码时候子通道的head是否传出去了 */
    unsigned int            bit_rate_size;          /* 记录该路速率大小 */
    int                     set_bit_rate;           /* 用来设置bit率大小 */
    SDL_Rect                dstRect;                /* 记录该路尺寸的大小 */

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

    int                     imagefile_flag;         /* 是否开启记录 */
    QFile*                  imagefile_fd;
    pthread_mutex_t         imagefile_fd_start;     /* 标志写开始 */
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
