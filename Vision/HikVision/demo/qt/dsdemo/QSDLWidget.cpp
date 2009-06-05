#include <qwidget.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qnamespace.h>
#include <qevent.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <qapplication.h>

#include "QSDLWidget.h"
#include "exvideo.h"

#define DSCLASSDEMO(i)  (dsdemoclass[i]->dsdemo)
extern int  fsWidth;
extern int  fsHeight;

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

QSDLWidget::QSDLWidget(QWidget* parent, const char* name) :
    QWidget(parent, name, 0)
{
    int totalcard, totalport;

    FullArea = FALSE;
    AllArea = FALSE;

    if(InitDSPs() == 0)
    {
        OutputErrorString("Initial DSP failed.error code is 0x%x\n", GetLastErrorNum());
        exit(1);
    }

    totalcard = GetTotalDSPs();
    totalport = GetTotalChannels();

    SelectPort = 0;
    LastPort = 0;
    pm = NULL;

    OutputDebugString("total cards = %d, total ports = %d\n", totalcard, totalport);

    dsdemoclass = (Dsclass**)malloc(totalport * sizeof(Dsclass *));
    memset(dsdemoclass, 0x0, totalport * sizeof(Dsclass *));

    checkpass = (short int*)malloc(totalport * sizeof(short int));
    memset(checkpass, 0x0, totalport * sizeof(short int));

    if(!(QIm.load("motion.bmp", 0)))    /* .... */
    {
        OutputErrorString("can't load motion.bmp\n");
        pm = NULL;
    }
    else
        pm = new QPixmap(QIm);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
video_info* QSDLWidget::GetVideoParam()
{
    return &(DSCLASSDEMO(SelectPort)->v_info);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::SetSelectPort(int port)
{
    if(port <= GetTotalChannels())
    {
        LastPort = SelectPort;
        SelectPort = port;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ChangeFullPort()   /* 都是全屏的情况下切换不同通道 */
{
    QRect       qr(0, 0, WIN_W, WIN_H);
    QPaintEvent Qp(qr, TRUE);

    int         chan;
    chan = GetChanNum();

    if(SelectPort == LastPort)
        return;
    else
    {
        memcpy(&(DSCLASSDEMO(SelectPort)->dstRect), &(DSCLASSDEMO(LastPort)->dstRect),
               sizeof(SDL_Rect));
        memcpy(&(DSCLASSDEMO(SelectPort)->motion_list), &(DSCLASSDEMO(LastPort)->motion_list),
               sizeof(RECT));

        dsdemoclass[LastPort]->DestoryMotion();
        dsdemoclass[LastPort]->StopPreview();
        dsdemoclass[LastPort]->SetdstRect(chan);
        dsdemoclass[LastPort]->SetMotion(chan);

        erase();
        paintEvent(&Qp);

#ifdef USE_PREVIEW_TIMER
        dsdemoclass[SelectPort]->SetPreview(0);
#else
        dsdemoclass[SelectPort]->SetPreview(1);
#endif
        if(DSCLASSDEMO(SelectPort)->motion_flag)
            dsdemoclass[SelectPort]->SetupMotion();
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
video_info* QSDLWidget::GetVideoDefault(int port)
{
    return &(DSCLASSDEMO(port)->videoDefault);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::SetDsclassDefault()
{
    int port;
    int LastPort = SelectPort;
    int totalport = GetTotalChannels();

    for(port = 0; port < totalport; port++)
    {
        if(!(DSCLASSDEMO(port)->bPreviewOpen))
        {
#ifdef USE_PREVIEW_TIMER
            dsdemoclass[port]->SetPreview(0);
#else
            dsdemoclass[port]->SetPreview(1);
#endif
        }

        if(DSCLASSDEMO(port)->record_flag)
        {
            SelectPort = port;
            RecordSet(FALSE);
        }

        if(DSCLASSDEMO(port)->cifqcif_flag)
        {
            SelectPort = port;
            RecordSubSet(FALSE);
        }

        DSCLASSDEMO(port)->cifqcif_flag = 0;
        DSCLASSDEMO(port)->pictureFormat = ENC_CIF_FORMAT;
        SetEncoderPictureFormat(DSCLASSDEMO(port)->ChannelHandle, ENC_CIF_FORMAT);
        (DSCLASSDEMO(port)->frameinfo).IQuantVal = 12;
        (DSCLASSDEMO(port)->frameinfo).PQuantVal = 12;
        (DSCLASSDEMO(port)->frameinfo).BQuantVal = 17;
        (DSCLASSDEMO(port)->frameinfo).KeyFrameIntervals = 25;
        (DSCLASSDEMO(port)->frameinfo).BFrames = 2;
        (DSCLASSDEMO(port)->frameinfo).PFrames = 0;
        (DSCLASSDEMO(port)->frameinfo).FrameRate = 25;
        SetDefaultQuant(DSCLASSDEMO(port)->ChannelHandle, 12, 12, 17);
        SetIBPMode(DSCLASSDEMO(port)->ChannelHandle, 25, 2, 0, 25);

        DSCLASSDEMO(port)->brc = brVBR;
        DSCLASSDEMO(port)->MaxBps = 768000;
        SetBitrateControlMode(DSCLASSDEMO(port)->ChannelHandle, brVBR);
        SetupBitrateControl(DSCLASSDEMO(port)->ChannelHandle, 768000);
        memcpy(&(DSCLASSDEMO(port)->v_info), &(DSCLASSDEMO(port)->videoDefault), sizeof(video_info));
        (DSCLASSDEMO(port)->osdinfo).Translucent = 1;
        (DSCLASSDEMO(port)->osdinfo).Brightness = 255;
        (DSCLASSDEMO(port)->osdinfo).PosX = 76;
        (DSCLASSDEMO(port)->osdinfo).PosY = 240;
        dsdemoclass[port]->SetOsddemo();
        if(!(DSCLASSDEMO(port)->osd_flag))
        {
            SetOsd(DSCLASSDEMO(port)->ChannelHandle, TRUE);
            DSCLASSDEMO(port)->osd_flag = 1;
        }
        (DSCLASSDEMO(port)->logoinfo).Translucent = 0;
        (DSCLASSDEMO(port)->logoinfo).PosX = 576;
        (DSCLASSDEMO(port)->logoinfo).PosY = (dsdemoclass[port]->GetVideoHeight()) - 32;
        if(!(DSCLASSDEMO(port)->logo_flag))
        {
            dsdemoclass[port]->SetDsLogo();
            DSCLASSDEMO(port)->logo_flag = 1;
        }

        memset(DSCLASSDEMO(port)->maskRect, 0, 5 * sizeof(RECT));
        dsdemoclass[port]->SetMask();

        if(DSCLASSDEMO(port)->audio_flag)
        {
            SetAudioPreview(DSCLASSDEMO(port)->ChannelHandle, TRUE);
            DSCLASSDEMO(port)->audio_flag = TRUE;
        }

        if(DSCLASSDEMO(port)->motion_flag)
        {
            dsdemoclass[port]->DestoryMotion();
            DSCLASSDEMO(port)->motion_flag = 0;
        }

        if(DSCLASSDEMO(port)->imagefile_flag)
        {
            SelectPort = port;
            ImageStreamSet(0, DSCLASSDEMO(port)->imagewidth, DSCLASSDEMO(port)->imageheight);
            DSCLASSDEMO(port)->imagefile_flag = 0;
        }
    }

    SelectPort = LastPort;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
DSDEMO* QSDLWidget::GetDsinfo(int port)
{
    return DSCLASSDEMO(port);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::SetFullEnable(bool on)
{
    if(FullArea == on)
    {
        if(on)
        {
            OutputDebugString("<><><>in function SetFullEnable \n");
            ChangeFullPort();
        }

        return;
    }

    mouseDoubleClickEvent(NULL);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ImageStreamCallback(unsigned int channelNumber, void* context)
{
    int width = DSCLASSDEMO(channelNumber)->imagewidth;
    int height = DSCLASSDEMO(channelNumber)->imageheight;

    pthread_mutex_lock(&(DSCLASSDEMO(channelNumber)->imagefile_fd_start));
    if(DSCLASSDEMO(channelNumber)->imagefile_fd)
    {
        (DSCLASSDEMO(channelNumber)->imagefile_fd)->writeBlock((char*)(DSCLASSDEMO(channelNumber)->imageBuf),
                                                                           (width * height * 3) / 2);
    }

    pthread_mutex_unlock(&(DSCLASSDEMO(channelNumber)->imagefile_fd_start));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ImageStreamSet(int start, unsigned width, unsigned height)
{
    if(start && (!(DSCLASSDEMO(SelectPort)->imageBuf)))
    {
        DSCLASSDEMO(SelectPort)->imageBuf = (unsigned char*)malloc((width * height * 3) / 2 * sizeof(char));
        memset(DSCLASSDEMO(SelectPort)->imageBuf, 0x0, (width * height * 3) / 2 * sizeof(char));
        dsdemoclass[SelectPort]->CreateImageStreamFiles();
    }
    else if((!start) && (DSCLASSDEMO(SelectPort)->imageBuf))
    {
        free(DSCLASSDEMO(SelectPort)->imageBuf);
        DSCLASSDEMO(SelectPort)->imageBuf = NULL;
        dsdemoclass[SelectPort]->end_imagestream(); /* 注意文件大小不能超过2G */
    }

    DSCLASSDEMO(SelectPort)->imagefile_flag = start;
    DSCLASSDEMO(SelectPort)->imagewidth = width;
    DSCLASSDEMO(SelectPort)->imageheight = height;
    SetImageStream(DSCLASSDEMO(SelectPort)->ChannelHandle, start, 5, width, height,
                   DSCLASSDEMO(SelectPort)->imageBuf);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ImageStreamAllSet(int start, unsigned width, unsigned height)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        ImageStreamSet(start, width, height);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::Createdsclass()
{
    int         i, totalport;
    char        SDL_windowhack[32];
    int         ChannelHandle;
    int         chan;

    QsStruct    QsTemp;

    QsTemp.qsclass = this;
    QsTemp.pmf = &QSDLWidget::StreamRead_callback;
    QsTemp.Startpmf = &QSDLWidget::start_capture_callback;
    QsTemp.Stoppmf = &QSDLWidget::stop_capture_callback;
    QsTemp.Messagepmf = &QSDLWidget::message_callback;
    QsTemp.IPpmf = &QSDLWidget::check_IP_callback;
    QsTemp.Passwordpmf = &QSDLWidget::check_password_callback;
    QsTemp.Checkpmf = &QSDLWidget::checkIpAndPass;
    QsTemp.Disconnectpmf = &QSDLWidget::disconnect_callback;
    QsTemp.ImageStreamCallbackpmf = &QSDLWidget::ImageStreamCallback;
    SetsQstruct(&QsTemp);

    totalport = GetTotalChannels();
    chan = GetChanNum();

    /* Init SDL * */
    sprintf(SDL_windowhack, "SDL_WINDOWID=%ld", winId());
    OutputDebugString("%ld \n", winId());
    putenv(SDL_windowhack);

    putenv("SDL_VIDEO_YUV_HWACCEL=0");

    fprintf(stderr, "Init the sdl...\n");
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        OutputErrorString("<sdk_error> init sdl failed!%s\n", SDL_GetError());
        exit(0);
    }

    pOverlayScreen = SDL_SetVideoMode(fsWidth, fsHeight, 0, SDL_HWSURFACE); /* here it is freed by
                                                                             * SDL_Quit();
                                                                             * */

    if(pOverlayScreen == NULL)
    {
        OutputErrorString("<sdk_error> create the sdl screen failed! for %s!\n", SDL_GetError());
        SDL_Quit();
        exit(0);
    }

    for(i = 0; i < totalport; i++)
    {
        if((ChannelHandle = ChannelOpen(i, InterStream)) < 0)
        {
            OutputErrorString("open port %d failed\n", i);
        }

        dsdemoclass[i] = new Dsclass(i, ChannelHandle, pOverlayScreen);
        dsdemoclass[i]->SetdstRect(chan);

#ifdef USE_PREVIEW_TIMER
        dsdemoclass[i]->SetPreview(0);
#else
        dsdemoclass[i]->SetPreview(1);
#endif
        dsdemoclass[i]->CreateThread();

        dsdemoclass[i]->SetMotion(chan);
        dsdemoclass[i]->MotionEnable();
    }

    SetDsclassDefault();

    RegisterImageStreamCallback((IMAGE_STREAM_CALLBACK) InterImageStreamCallback, NULL);

    timer = (QTimer**)malloc(totalport * sizeof(QTimer *));
    memset(timer, 0x0, totalport * sizeof(QTimer *));

    Qtloop = (QTimer**)malloc(totalport * sizeof(QTimer *));
    memset(Qtloop, 0x0, totalport * sizeof(QTimer *));

    time = (QTime**)malloc(totalport * sizeof(QTime *));
    memset(time, 0x0, totalport * sizeof(QTime *));

    for(i = 0; i < totalport; i++)
    {
        char    temp[3];

        sprintf(temp, "%2d", i);
        temp[3] = '\0';

        timer[i] = new QTimer(this, temp);
        connect(timer[i], SIGNAL(timeout()), SLOT(process_time()));

        Qtloop[i] = new QTimer(this, temp);
        connect(Qtloop[i], SIGNAL(timeout()), SLOT(process_time_loop()));

        time[i] = new QTime();
        time[i]->start();
    }   /* 需要在板卡初始化化之后才调用的么? */
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
QSDLWidget::~QSDLWidget()
{
    OutputDebugString("Enter into delete QSDLWidget\n");

    int port;
    int totalport;

    totalport = GetTotalChannels();

    RecordAllSet(FALSE);
    RecordSubAllSet(FALSE);
    ImageStreamAllSet(0, 704, 576);

    if(DSCLASSDEMO(SelectPort)->net_flag)   /* 考虑到网传是统一设置的,所以可以只需考虑一个flag */
        NetTransferSet(FALSE);

    for(port = 0; port < totalport; port++)
    {
        dsdemoclass[port]->DestoryThread();
        pthread_join(DSCLASSDEMO(port)->hPreviewThread, NULL);
        OutputErrorString("*****************close port%d******************\n", port);   /* 借用一下OutputErrorString */
    }

    OutputErrorString("ending threads <<<<<<<<<<<<<<<<<<<<\n");

    MotionAllSet(FALSE);

    if(pm)
    {
        delete pm;
        pm = NULL;
    }

    for(port = 0; port < totalport; port++)
    {
        if(DSCLASSDEMO(port)->audio_flag)
            break;
    }

    if(port != totalport)
    {
        SetAudioPreview(DSCLASSDEMO(port)->ChannelHandle, 0);
        DSCLASSDEMO(port)->audio_flag = 0;
    }

    for(port = 0; port < totalport; port++)
    {
        ChannelClose(DSCLASSDEMO(port)->ChannelHandle);
        dsdemoclass[port]->end_record();
        dsdemoclass[port]->end_subrecord();
        dsdemoclass[port]->end_imagestream();
        delete dsdemoclass[port];
        delete time[port];
        delete timer[port];
        delete Qtloop[port];
    }

    if(dsdemoclass)
    {
        free(dsdemoclass);
        dsdemoclass = NULL;
    }

    if(checkpass)
    {
        free(checkpass);
        checkpass = NULL;
    }

    if(time)
    {
        free(time);
        time = NULL;
    }

    if(timer)
    {
        free(timer);
        timer = NULL;
    }

    if(Qtloop)
    {
        free(Qtloop);
        Qtloop = NULL;
    }

    OutputDebugString("Before DeInitDSPs\n");
    DeInitDSPs();

    SDL_Quit();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::process_time()
{
    int                 temp;
    int                 Nowport = 0;
    int                 timepassed;
    unsigned int        elapseTime;
    FRAMES_STATISTICS   frame_statistics, sub_frame_statistics;
    float               audioframerate, videoframerate;

    /*
     * int bitRate;
     */
    int                 totalFrame;
    int                 userCount;
    int                 LostFrame;

    for(temp = 0; temp < GetTotalChannels(); temp++)
    {
        QTimer*     Qtemp = (QTimer *) (this->sender());
        if(atoi(Qtemp->name()) == temp)
        {
            Nowport = temp;
            break;
        }
    }

    timepassed = time[Nowport]->elapsed();
    elapseTime = (unsigned int)timepassed + (unsigned int)(TIMEDAYS * (DSCLASSDEMO(Nowport)->countdays));

    /* 注意这里,QTime24小时，计数器会自动转换为0 */
    if(timepassed > TIMEDAYS)   /* 49天的问题 */
        DSCLASSDEMO(Nowport)->countdays++;

    GetFramesStatistics(DSCLASSDEMO(Nowport)->ChannelHandle, &frame_statistics);

#if 0
    SetupSubChannel(DSCLASSDEMO(Nowport)->ChannelHandle, 1);
    GetFramesStatistics(DSCLASSDEMO(Nowport)->ChannelHandle, &sub_frame_statistics);
    SetupSubChannel(DSCLASSDEMO(Nowport)->ChannelHandle, 0);
#endif
    audioframerate = ((float)frame_statistics.AudioFrames) / ((float)elapseTime / (float)1000);

    emit    audioChange(Nowport, audioframerate);

    videoframerate = ((float)frame_statistics.VideoFrames) / ((float)elapseTime / (float)1000);

    emit    videoChange(Nowport, videoframerate);

    totalFrame = frame_statistics.VideoFrames + frame_statistics.AudioFrames;

    emit    totalChange(Nowport, totalFrame);

    LostFrame = sub_frame_statistics.FramesLost;

    /*
     * bitRate= (int)(float(DSCLASSDEMO(SelectPort)->bit_rate_size)/((float)elapseTime/(float)8000));
     */
    emit    bitRateChange(Nowport, frame_statistics.CurBps);
    if(Nowport == 4)
    {
        OutputDebugString("frame_statistics: %d,%d,%d\n", frame_statistics.AudioFrames,
                          frame_statistics.VideoFrames, frame_statistics.CurBps);
    }

    userCount = MP4_ServerGetState();
    if(userCount <= 0)
        userCount = 0;

    emit    countChange(userCount);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::process_time_loop()
{
    int             totalport;
    char            savepath[10] = ".";
    struct statfs   bbuf;
    int             Nowport = 0;
    int             temp;
    int             ImageLength = 0;
    int             width;
    int             height;
    int             fps = 5;                        /* 在函数SetImageStream中用到 */

    totalport = GetTotalChannels();

    for(temp = 0; temp < totalport; temp++)
    {
        QTimer*     Qtemp = (QTimer *) (this->sender());
        if(atoi(Qtemp->name()) == temp)
        {
            Nowport = temp;
            break;
        }
    }

    for(temp = 0; temp < totalport; temp++)
    {
        if(DSCLASSDEMO(temp)->imagefile_flag)
        {
            width = DSCLASSDEMO(temp)->imagewidth;
            height = DSCLASSDEMO(temp)->imageheight;
            ImageLength += (fps * (width * height * 3 / 2) * TIMECONST) / (8 * 1024);
        }
    }

    if(statfs(savepath, &bbuf) < 0)
    {
        OutputErrorString("statsfs() faied, error: %d\n", errno);
    }
    else
    {
        if((int)(bbuf.f_bavail * (bbuf.f_bsize / 1024)) <= (int)
               ((TIMECONST * (totalport * FIXSIZE) + ImageLength) + LastSpace))
        /* If no enough space, reset count */
        {
            OutputDebugString("No enough space.avialable blocks = %ld\n", bbuf.f_bavail);
            if(!(DSCLASSDEMO(Nowport)->fullspace_flag))
            {
                emit    fullSpace();
                DSCLASSDEMO(Nowport)->Maxcount = DSCLASSDEMO(Nowport)->count;
                OutputDebugString("fullspace$$$$$ the max count of port %02d is %d\n", Nowport,
                                  DSCLASSDEMO(Nowport)->Maxcount);

                DSCLASSDEMO(Nowport)->count = -1;   /* 当硬盘第一次满的时候清除count值 */
                DSCLASSDEMO(Nowport)->fullspace_flag = 1;
            }
        }
    }

    if((DSCLASSDEMO(Nowport)->Maxcount == DSCLASSDEMO(Nowport)->count) &&
       (DSCLASSDEMO(Nowport)->fullspace_flag))
    {
        OutputDebugString("&&&&&&port %02d get the count %d\n", Nowport, DSCLASSDEMO(Nowport)->count);
        DSCLASSDEMO(Nowport)->count = -1;           /* 当count值达到最大值的时候, 清除count值 */
    }

#ifdef WRITE_FILE
    if(DSCLASSDEMO(Nowport)->record_flag || DSCLASSDEMO(Nowport)->cifqcif_flag ||
       DSCLASSDEMO(Nowport)->imagefile_flag)
        DSCLASSDEMO(Nowport)->count++;
    else
        return;

    if(DSCLASSDEMO(Nowport)->record_flag)
    {
        dsdemoclass[Nowport]->CreateFiles();
    }

    if(DSCLASSDEMO(Nowport)->cifqcif_flag)
    {
        dsdemoclass[Nowport]->CreateCifQcifFiles();
    }

    if(DSCLASSDEMO(Nowport)->imagefile_flag)
    {
        dsdemoclass[Nowport]->CreateImageStreamFiles();

        /*
         * After stop imagestream and then start imagestream right now, it will overwrite
         * the newest imagefile
         */
#if 0
        int width = DSCLASSDEMO(Nowport)->imagewidth;
        int height = DSCLASSDEMO(Nowport)->imageheight;
        (DSCLASSDEMO(Nowport)->imagefile_fd)->writeBlock((char*)(DSCLASSDEMO(Nowport)->imageBuf),
                                                         (width * height * 3) / 2);
#endif
    }
#endif
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdEnableSet(bool on)
{
    DSCLASSDEMO(SelectPort)->osd_flag = int(on);
    SetOsd(DSCLASSDEMO(SelectPort)->ChannelHandle, int(on));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdEnableAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        if(on)
        {
            if(!(DSCLASSDEMO(port)->osd_flag))
            {
                SelectPort = port;
                OsdEnableSet(TRUE);
            }
        }
        else
        {
            if(DSCLASSDEMO(port)->osd_flag)
            {
                SelectPort = port;
                OsdEnableSet(FALSE);
            }
        }
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdTranslucentSet(bool on)
{
    dsdemoclass[SelectPort]->SetOsdTranslucent(short(on));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdTranslucentAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        OsdTranslucentSet(on);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdBrightnessSet(int number)
{
    dsdemoclass[SelectPort]->SetOsdBrightness(number);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdBrightnessAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        OsdBrightnessSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdPosXSet(int number)
{
    dsdemoclass[SelectPort]->SetOsdPosX(short(number));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdPosXAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        OsdPosXSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdPosYSet(int number)
{
    dsdemoclass[SelectPort]->SetOsdPosY(short(number));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OsdPosYAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        OsdPosYSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoEnabledSet(bool on)
{
    DSCLASSDEMO(SelectPort)->logo_flag = int(on);

    if(on)
        dsdemoclass[SelectPort]->SetDsLogo();
    else
        StopLogo(DSCLASSDEMO(SelectPort)->ChannelHandle);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoEnabledAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        if(on)
        {
            if(!(DSCLASSDEMO(port)->logo_flag))
            {
                SelectPort = port;
                LogoEnabledSet(TRUE);
            }
        }
        else
        {
            if(DSCLASSDEMO(port)->logo_flag)
            {
                SelectPort = port;
                LogoEnabledSet(FALSE);
            }
        }
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoTranslucentSet(bool on)
{
    dsdemoclass[SelectPort]->SetLogoTranslucent(short(on));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoTranslucentAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        LogoTranslucentSet(on);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoPosXSet(int number)
{
    dsdemoclass[SelectPort]->SetLogoPosX(short(number));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoPosXAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        LogoPosXSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoPosYSet(int number)
{
    dsdemoclass[SelectPort]->SetLogoPosY(short(number));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::LogoPosYAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        LogoPosYSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
RECT* QSDLWidget::GetMaskinfo(int port)
{
    return(DSCLASSDEMO(port)->maskRect);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MaskEnableSet(bool on)
{
    if(on)
        dsdemoclass[SelectPort]->SetMask();
    else
        StopMask(DSCLASSDEMO(SelectPort)->ChannelHandle);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MaskEnableAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        MaskEnableSet(on);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MaskAddSet(RECT* add)
{
    int     i;
    RECT*   tempmask = DSCLASSDEMO(SelectPort)->maskRect;

    for(i = 0; i < 5; i++)
    {
        if(!(tempmask[i].RectTop || tempmask[i].RectBottom || tempmask[i].RectLeft ||
           tempmask[i].RectRight))
            break;
    }

    if(i == 5)
        return;

    memcpy(&(tempmask[i]), add, sizeof(RECT));

    if(i < 4)
        memset(&(tempmask[i + 1]), 0x0, sizeof(RECT) * (4 - i));

    dsdemoclass[SelectPort]->SetMask();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MaskAddAllSet(RECT* add)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        MaskAddSet(add);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MaskDelSet(int index)
{
    int     i;
    RECT*   temp;
    RECT*   tempmask = DSCLASSDEMO(SelectPort)->maskRect;

    for(i = 0; i < 5; i++)
    {
        if(!(tempmask[i].RectTop || tempmask[i].RectBottom || tempmask[i].RectLeft ||
           tempmask[i].RectRight))
            break;
    }

    if(index == (i - 1))
    {
        memset(&(tempmask[index]), 0x0, sizeof(RECT));
        dsdemoclass[SelectPort]->SetMask();
        return;
    }

    temp = (RECT*)malloc((i - 1 - index) * sizeof(RECT));
    memcpy(temp, &(tempmask[index + 1]), (i - 1 - index) * sizeof(RECT));
    memset(&(tempmask[index]), 0x0, (i - index) * sizeof(RECT));
    memcpy(&(tempmask[index]), temp, (i - 1 - index) * sizeof(RECT));
    free(temp);

    dsdemoclass[SelectPort]->SetMask();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MaskDelAllSet(int index)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        MaskDelSet(index);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::VideoRestoreSet()
{
    video_info*     tempvideo = GetVideoDefault(SelectPort);
    memcpy(&(DSCLASSDEMO(SelectPort)->v_info), tempvideo, sizeof(video_info));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::VideoRestoreAllSet()
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        VideoRestoreSet();
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::CifQCifAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        CifQCifSet(on);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::CifQCifSet(bool on)
{
    DSCLASSDEMO(SelectPort)->cifqcif_flag = int(on);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::IQuantSet(int number)
{
    dsdemoclass[SelectPort]->SetFrameIQuantVal(number);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::IQuantAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        IQuantSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BQuantSet(int number)
{
    dsdemoclass[SelectPort]->SetFrameBQuantVal(number);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BQuantAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        BQuantSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::PQuantSet(int number)
{
    dsdemoclass[SelectPort]->SetFramePQuantVal(number);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::PQuantAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        PQuantSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::KeyFrameISet(int number)
{
    dsdemoclass[SelectPort]->SetFrameKeyFrameI(number);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::KeyFrameIAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        KeyFrameISet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BFramesSet(int number)
{
    dsdemoclass[SelectPort]->SetFrameBFrames(number);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BFramesAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        BFramesSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::FrameRateSet(int number)
{
    dsdemoclass[SelectPort]->SetFrameFrameRate(number);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::FrameRateAllSet(int number)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        FrameRateSet(number);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::StreamTypeSet(int StreamType)
{
    fprintf(stderr, "Set StreamType for Port %d\n", SelectPort);
    SetStreamType(DSCLASSDEMO(SelectPort)->ChannelHandle, StreamType);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::StreamTypeAllSet(int StreamType)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        StreamTypeSet(StreamType);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::StreamTypeSubSet(int StreamType)
{
    fprintf(stderr, "Set SubStreamType for Port %d\n", SelectPort);
    SetSubStreamType(DSCLASSDEMO(SelectPort)->ChannelHandle, StreamType);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::StreamTypeSubAllSet(int StreamType)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        StreamTypeSubSet(StreamType);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::EncoderSet(PictureFormat_t pictureFormat)
{
    DSCLASSDEMO(SelectPort)->pictureFormat = pictureFormat;
    SetEncoderPictureFormat(DSCLASSDEMO(SelectPort)->ChannelHandle, pictureFormat);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::EncoderSubSet(PictureFormat_t pictureFormat)
{
    DSCLASSDEMO(SelectPort)->subpictureFormat = pictureFormat;
    SetSubEncoderPictureFormat(DSCLASSDEMO(SelectPort)->ChannelHandle, pictureFormat);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::EncoderAllSet(PictureFormat_t pictureFormat)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        EncoderSet(pictureFormat);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::EncoderSubAllSet(PictureFormat_t pictureFormat)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        EncoderSubSet(pictureFormat);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BitRateSet(int MaxBps, BitrateControlType_t brc)
{
    DSCLASSDEMO(SelectPort)->MaxBps = MaxBps;
    DSCLASSDEMO(SelectPort)->brc = brc;

    OutputDebugString("brc = %d\n", brc);

    dsdemoclass[SelectPort]->SetMaxBpsMode();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BitRateAllSet(int MaxBps, BitrateControlType_t brc)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        BitRateSet(MaxBps, brc);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::AllRecordStatus()
{
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        if(!(DSCLASSDEMO(port)->record_flag))
        {
            return 0;
        }
    }

    return 1;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::RecordAllSet(bool on)
{
    OutputDebugString("Beginning Record.........\n");

    int temp = SelectPort;
    int port;

    if(on)
    {
        for(port = 0; port < GetTotalChannels(); port++)
        {
            SelectPort = port;
            if(!(DSCLASSDEMO(SelectPort)->record_flag))
                RecordSet(TRUE);
        }
    }
    else
    {
        for(port = 0; port < GetTotalChannels(); port++)
        {
            SelectPort = port;
            if(DSCLASSDEMO(SelectPort)->record_flag)
                RecordSet(FALSE);
        }
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::RecordSet(bool on)
{
    struct timespec tv;

    if(on)
    {
        Qtloop[SelectPort]->start(1000 * TIMECONST);

        if(!(DSCLASSDEMO(SelectPort)->net_flag))
        {
            timer[SelectPort]->start(200);
            time[SelectPort]->restart();
            DSCLASSDEMO(SelectPort)->countdays = 0;
        }

        dsdemoclass[SelectPort]->CreateFiles();
        if(!DSCLASSDEMO(SelectPort)->net_flag)
            dsdemoclass[SelectPort]->StartCapture();
        DSCLASSDEMO(SelectPort)->record_flag = 1;
    }
    else
    {
        OutputDebugString("in RecordSet false \n");
        Qtloop[SelectPort]->stop();
        if(!(DSCLASSDEMO(SelectPort)->net_flag))
        {
            timer[SelectPort]->stop();

            emit    audioChange(SelectPort, (float)0.0);
            emit    videoChange(SelectPort, (float)0.0);
            emit    totalChange(SelectPort, 0);
            emit    bitRateChange(SelectPort, 0);
            emit    countChange(0);

            dsdemoclass[SelectPort]->StopCapture();

            /* sleep 40 ms, for protecting that data in stream buffer is written into the file and then come to empty */
            tv.tv_sec  = 0;
            tv.tv_nsec = 40 * 1000 * 1000;
            nanosleep(&tv, NULL);
            /*
             * if(StopVideoCapture(DSCLASSDEMO(SelectPort)->ChannelHandle) < 0) ;
             * printf("record_callback: StopvideoCapture(port %d) failed\n", SelectPort);
             */
        }

        dsdemoclass[SelectPort]->end_record();
        DSCLASSDEMO(SelectPort)->bit_rate_size = 0;
        DSCLASSDEMO(SelectPort)->count = 0;
        DSCLASSDEMO(SelectPort)->record_flag = 0;
        DSCLASSDEMO(SelectPort)->Maxcount = 0;
        DSCLASSDEMO(SelectPort)->filehead_flag = 0;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::RecordSubAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    if(on)
    {
        for(port = 0; port < GetTotalChannels(); port++)
        {
            SelectPort = port;
            if(!(DSCLASSDEMO(SelectPort)->cifqcif_flag))
                RecordSubSet(TRUE);
        }
    }
    else
    {
        for(port = 0; port < GetTotalChannels(); port++)
        {
            SelectPort = port;
            if(DSCLASSDEMO(SelectPort)->cifqcif_flag)
                RecordSubSet(FALSE);
        }
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::RecordSubSet(bool on)
{
    if(on)
    {
        if(DSCLASSDEMO(SelectPort)->cifqcif_flag == 0)
        {
            DSCLASSDEMO(SelectPort)->cifqcif_flag = 1;
            dsdemoclass[SelectPort]->CreateCifQcifFiles();
            dsdemoclass[SelectPort]->StartSubCapture();

            /*
             * if( DSCLASSDEMO(SelectPort)-> cifqciffilehead_flag == 0 &&
             * (DSCLASSDEMO(SelectPort)->cifqciffile_head[0]!=0)) ;
             * (DSCLASSDEMO(SelectPort)->
             * cifqcif_fd)->writeBlock(DSCLASSDEMO(SelectPort)->cifqciffile_head, 40);
             */
        }
    }
    else
    {
        if(DSCLASSDEMO(SelectPort)->cifqcif_flag)
        {
            dsdemoclass[SelectPort]->end_subrecord();
            dsdemoclass[SelectPort]->StopSubCapture();
            DSCLASSDEMO(SelectPort)->cifqcif_flag = 0;
            DSCLASSDEMO(SelectPort)->cifqciffilehead_flag = 0;
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::GetBmpPicture(int count)
{
    int             size;
    unsigned char   imagebuf[704 * 576 * 2];

    char            savepath[10] = ".";
    QFile*          temp;
    char            qsFile[40];

    sprintf(qsFile, "%s/BMP_P%02dC%02d.bmp", savepath, SelectPort, count);
    GetOriginalImage(DSCLASSDEMO(SelectPort)->ChannelHandle, imagebuf, &size);
    temp = new QFile(qsFile);
    temp->open(IO_ReadWrite | IO_Truncate);
    delete temp;

    if(size == 704 * 576 * 2)   /* PAL */
        SaveYUVToBmpFile(qsFile, imagebuf, 704, 576);
    if(size == 704 * 480 * 2)   /* NTSC */
        SaveYUVToBmpFile(qsFile, imagebuf, 704, 480);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::GetJpegPicture(int count, int quality)
{
    long unsigned int   size = 704 * 576 * 2;
    unsigned char*      imagebuf;
    char                savepath[10] = ".";
    char                qsFile[40];

    imagebuf = new unsigned char[size];

    QFile*  temp;

    sprintf(qsFile, "%s/Jpeg_P%02dC%02d.jpg", savepath, SelectPort, count);
    GetJpegImage(DSCLASSDEMO(SelectPort)->ChannelHandle, imagebuf, &size, quality);
    temp = new QFile(qsFile);
    temp->open(IO_WriteOnly | IO_Truncate);
    temp->writeBlock((char*)imagebuf, size);
    delete temp;
    delete imagebuf;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::GetBmpAllPicture(int count)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        GetBmpPicture(count);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::GetJpegAllPicture(int count, int quality)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        OutputDebugString("start get port%2d jpeg picture\n", port);
        GetJpegPicture(count, quality);
    }

    SelectPort = temp;
    OutputDebugString("end GetJpegAllPicture\n");
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::start_capture_callback(int port)
{
    int totalport;

    totalport = GetTotalChannels();

    if((port < totalport) && (!(DSCLASSDEMO(port)->record_flag)))   /* fixed at 2004.11.11 */
    {
        OutputDebugString("start capture callback\n");
        dsdemoclass[port]->StartCapture();
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::stop_capture_callback(int port)
{
    int totalport;

    totalport = GetTotalChannels();
    if((port < totalport) && (!(DSCLASSDEMO(port)->record_flag)))   /* fixed at 2004.11.11 */
    {
        OutputDebugString("in stop_capture_callback\n");
        dsdemoclass[port]->StopCapture();

        emit    audioChange(port, (float)0.0);
        emit    videoChange(port, (float)0.0);
        emit    totalChange(port, 0);
        emit    bitRateChange(port, 0);
        emit    countChange(0);
    } DSCLASSDEMO(port)->net_head_flag = 0;
    DSCLASSDEMO(port)->subchan_net_head_flag = 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::message_callback(char* buf, int length)
{
    OutputDebugString("Get a message from client: length = %d, message = %s\n", length, buf);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::check_IP_callback(int channel, char* sIP)
{
    OutputErrorString("Check IP: channel = %d, IP= %s\n", channel, sIP);
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::check_password_callback(char* username, int namelen, char* password, int passlen)
{
    OutputErrorString("Check password: username = %s, namelen = %d, password = %s, passname = %d\n",
                      username, namelen, password, passlen);
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::checkIpAndPass(int channel, char*  username, int namelen, char*  password,
                               int passlen, char*  sIP)
{
    OutputDebugString("Check IP: channel = %d, IP= %s; username = %s, namelen = %d, password = %s, passname = %d\n",
                  channel, sIP, username, namelen, password, passlen);
    checkpass[channel] = 1;
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::disconnect_callback(int nport, char* sIP)
{
    OutputErrorString("the client from ip %s to channel %d disconnect!\n", sIP, nport);
    checkpass[nport] = 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::NetTransferSet(bool on)
{
    int                 port;
    SERVER_VIDEOINFO    videoinfo;
    int                 totalport;

    totalport = GetTotalChannels();
    memset(&videoinfo, 0, sizeof(SERVER_VIDEOINFO));

    if(on)
    {
        for(port = 0; port < totalport; port++)
            MP4_ServerSetBufNum(port, 60);

        MP4_ServerSetStart(InterStart);
        MP4_ServerSetStop(InterStop);
        MP4_ServerSetMessage(InterMessage);
        MP4_ServerSetNetPort(5050, 6050);

        videoinfo.m_channum = totalport;
        for(port = 0; port < totalport; port++)
        {
            if(!(DSCLASSDEMO(port)->record_flag))
            {
                timer[port]->start(200);
                time[port]->restart();
                DSCLASSDEMO(SelectPort)->countdays = 0;
            }
        }

        if(MP4_ServerStart(&videoinfo) < 0)
        {
            for(port = 0; port < totalport; port++)
            {
                if((timer[port]->isActive()) && (!(DSCLASSDEMO(port)->record_flag)))
                    timer[port]->stop();
                DSCLASSDEMO(port)->net_flag = 0;
                OutputErrorString("because of MP4_ServerStart failed, so stop the timer! \n");
            }
        }
        else
        {
            for(port = 0; port < totalport; port++)
                DSCLASSDEMO(port)->net_flag = 1;
            MP4_ServerCheckIP(InterIP);
            MP4_ServerCheckPassword(InterPassword);
            MP4_ServerCheckIPandPassword(InterCheck);
            MP4_ServerDisconnectEvent(InterDisconnect);
        }
    }
    else
    {
        for(port = 0; port < totalport; port++)
        {
            if(!(DSCLASSDEMO(port)->record_flag))
                timer[port]->stop();
            DSCLASSDEMO(port)->net_flag = 0;
        }

        MP4_ServerStop();
        OutputDebugString("After MP4_serverStop() func, totalport = %d\n", totalport);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MotionSet(bool on)
{
    if(on)
    {
        OutputDebugString("<demo_info> start chan%d 's motion detection!\n", SelectPort);
        if(!(DSCLASSDEMO(SelectPort)->motion_flag))
        {
            dsdemoclass[SelectPort]->SetupMotion();
            DSCLASSDEMO(SelectPort)->motion_flag = 1;
        }
    }
    else    /* stop motion detect */
    {
        if(DSCLASSDEMO(SelectPort)->motion_flag)
        {
            dsdemoclass[SelectPort]->DestoryMotion();
            DSCLASSDEMO(SelectPort)->motion_flag = 0;
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MotionAllSet(bool on)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        MotionSet(on);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MotionSenSet(int value)
{
    DSCLASSDEMO(SelectPort)->motionSens = value;
    AdjustMotionDetectPrecision(DSCLASSDEMO(SelectPort)->ChannelHandle, value, 2, 0);
    StartMotionDetection(DSCLASSDEMO(SelectPort)->ChannelHandle);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MotionSenAllSet(int value)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        MotionSenSet(value);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::AudioSet(bool on)
{
    int ret;
    int temp;

    ret = GetTotalChannels();

    if(on)
    {
        for(temp = 0; temp < ret; temp++)
        {
            if(DSCLASSDEMO(temp)->audio_flag)
                break;
        }

        if(temp != ret)
        {
            SetAudioPreview(DSCLASSDEMO(temp)->ChannelHandle, 0);
            DSCLASSDEMO(temp)->audio_flag = 0;
        }

        ret = SetAudioPreview(DSCLASSDEMO(SelectPort)->ChannelHandle, 1);
        if(ret < 0)
        {
            OutputErrorString("Audio preview error \n");
            return;
        }

        DSCLASSDEMO(SelectPort)->audio_flag = 1;
    }
    else
    {
        SetAudioPreview(DSCLASSDEMO(SelectPort)->ChannelHandle, 0);
        DSCLASSDEMO(SelectPort)->audio_flag = 0;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::PreviewAllSet(bool on)
{
    int tempport = SelectPort;
    int port;

    if(FullArea)
        PreviewSet(on);
    else
    {
        for(port = 0; port < GetTotalChannels(); port++)
        {
            SelectPort = port;
            PreviewSet(on);
        }
    }

    SelectPort = tempport;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::PreviewSet(bool on)
{
    int port;
    int totalport;

    totalport = GetTotalChannels();
    if(on)
    {
        if(FullArea)
        {
            OutputDebugString("debug here FULL\n");
            if(!(DSCLASSDEMO(SelectPort)->bPreviewOpen))
            {
#ifdef USE_PREVIEW_TIMER
                dsdemoclass[SelectPort]->SetPreview(0);
#else
                dsdemoclass[SelectPort]->SetPreview(1);
#endif
            }
        }
        else
        {
            OutputDebugString("debug here\n");
            if(!(DSCLASSDEMO(SelectPort)->bPreviewOpen))
            {
                OutputDebugString("PreviewSet:Port%d=%d\n", SelectPort,
                                  DSCLASSDEMO(SelectPort)->bPreviewOpen);
#ifdef USE_PREVIEW_TIMER
                dsdemoclass[SelectPort]->SetPreview(0);
#else
                dsdemoclass[SelectPort]->SetPreview(1);
#endif
            }

            for(port = 0; port < totalport; port++)
            {
                if(!(DSCLASSDEMO(port)->bPreviewOpen))
                {
                    QPainter    Qp;
                    Qp.begin(this);
                    Qp.eraseRect((DSCLASSDEMO(SelectPort)->dstRect).x,
                                 (DSCLASSDEMO(SelectPort)->dstRect).y,
                                 (DSCLASSDEMO(SelectPort)->dstRect).w,
                                 (DSCLASSDEMO(SelectPort)->dstRect).h);
                    Qp.end();
                }
            }
        }
    }
    else
    {
        if(FullArea)
        {
            if(DSCLASSDEMO(SelectPort)->bPreviewOpen)
                dsdemoclass[SelectPort]->StopPreview();

            QPainter    Qp;
            Qp.begin(this);
            Qp.eraseRect(WIN_L, WIN_T, WIN_W, WIN_H);
            Qp.end();
        }
        else
        {
            if(DSCLASSDEMO(SelectPort)->bPreviewOpen)
                dsdemoclass[SelectPort]->StopPreview();

            for(port = 0; port < totalport; port++)
            {
                if(!(DSCLASSDEMO(port)->bPreviewOpen))
                {
                    QPainter    Qp;
                    Qp.begin(this);
                    Qp.eraseRect((DSCLASSDEMO(SelectPort)->dstRect).x,
                                 (DSCLASSDEMO(SelectPort)->dstRect).y,
                                 (DSCLASSDEMO(SelectPort)->dstRect).w,
                                 (DSCLASSDEMO(SelectPort)->dstRect).h);
                    Qp.end();
                }
            }
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::StringToClientSet(bool on)
{
    int     port;
    char    messagebuf[256];

    if((!(DSCLASSDEMO(SelectPort)->net_flag)) && on)
    {
        emit    noNet();
        return;
    }

    if(on)
    {
        for(port = 0; port < 2; port++) /* test the string to client */
        {
            sprintf(messagebuf, "server message to client, no.%d!\n", port);
            if(MP4_ServerStringToClient("192.0.4.166", messagebuf, strlen(messagebuf)))
                OutputErrorString("return bad value : %d\n", MP4_ServerGetLastErrorNum());

            OutputDebugString("<demo_info> have send the len %d message to client!\n",
                              strlen(messagebuf));
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BrightnessSet(int value)
{
    dsdemoclass[SelectPort]->SetBrightScale(value);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::BrightnessAllSet(int value)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        BrightnessSet(value);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ContrastSet(int value)
{
    dsdemoclass[SelectPort]->SetContrastScale(value);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ContrastAllSet(int value)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        ContrastSet(value);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::SaturationSet(int value)
{
    dsdemoclass[SelectPort]->SetSaturationScale(value);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::SaturationAllSet(int value)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        SaturationSet(value);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::HueSet(int value)
{
    dsdemoclass[SelectPort]->SetHueScale(value);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::HueAllSet(int value)
{
    int temp = SelectPort;
    int port;

    for(port = 0; port < GetTotalChannels(); port++)
    {
        SelectPort = port;
        HueSet(value);
    }

    SelectPort = temp;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::GetChanNum()
{
    int totalport;

    totalport = GetTotalChannels();

    int chan = 1;

    if(totalport <= 4)
        chan = 2;
    else
    {
        if(totalport <= 9)
            chan = 3;
        else
        {
            if(totalport <= 16)
                chan = 4;
            else
            {
                if(totalport <= 25)
                    chan = 5;
                else
                    chan = 6;
            }
        }
    }

    return chan;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::paintEvent(QPaintEvent* e)
{
    setPaletteBackgroundColor(QColor(0, 0, 0));
    setPaletteForegroundColor(QColor(0, 255, 0));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::mousePressEvent(QMouseEvent* e)
{
    int i;
    int totalport;

    if(FullArea || AllArea)
        return;

    QPoint  pnt = e->pos();
    totalport = GetTotalChannels();

    /*
     * if (e->button() == Qt::LeftButton) ;
     * {
     */
    for(i = 0; i < totalport; i++)
    {
        if(PtInRect(DSCLASSDEMO(i)->dstRect, pnt.x(), pnt.y()))
        {
            SelectPort = i;
            break;
        }
    }

    if(i == totalport)
        return;

    if(DSCLASSDEMO(SelectPort)->record_flag)
    {
        emit    audioChange(SelectPort, (float)0.0);
        emit    videoChange(SelectPort, (float)0.0);
        emit    totalChange(SelectPort, 0);
        emit    bitRateChange(SelectPort, 0);
    }

    emit  clicked(LEFTCLICKED);

    /* } */
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
    int         temp;
    int         totalport;
    int         chan;
    QRect       qr(0, 0, WIN_W, WIN_H);
    QPaintEvent Qp(qr, TRUE);

    totalport = GetTotalChannels();
    chan = GetChanNum();

    if(e != NULL && (e->button() == Qt::LeftButton || e->button() == Qt::RightButton) &&
       (!(FullArea || AllArea)))
    {   /* 保证是有效区域内点击 */
        int     i;
        QPoint  pnt = e->pos();
        for(i = 0; i < totalport; i++)
        {
            if(PtInRect(DSCLASSDEMO(i)->dstRect, pnt.x(), pnt.y()))
            {
                SelectPort = i;
                break;
            }
        }

        if(i == totalport)
            return;
    }

    if(e != NULL && e->button() == Qt::LeftButton && AllArea)       /* 当全屏时双击左键实效 */
        return;

    if(e != NULL && e->button() == Qt::RightButton && FullArea)     /* 当局部全屏时双击右键实效 */
        return;

    if(e != NULL && e->button() == Qt::RightButton && !FullArea)    /* 当局部没有全屏时双击左键 */
        emit    clicked(RIGHTCLICKED);

    if(e == NULL || (e->button() == Qt::LeftButton && !AllArea))
    {
        if(DSCLASSDEMO(SelectPort)->bPreviewOpen)
        {
            if(!FullArea)
            {
                for(temp = 0; temp < totalport; temp++)
                {
                    if(DSCLASSDEMO(temp)->bPreviewOpen)
                        dsdemoclass[temp]->StopPreview();
                    if(DSCLASSDEMO(temp)->motion_flag)
                        dsdemoclass[temp]->DestoryMotion();
                }
                (DSCLASSDEMO(SelectPort)->dstRect).x = 0;
                (DSCLASSDEMO(SelectPort)->dstRect).y = 0;
                (DSCLASSDEMO(SelectPort)->dstRect).w = WIN_W;
                (DSCLASSDEMO(SelectPort)->dstRect).h = WIN_H;
                (DSCLASSDEMO(SelectPort)->motion_list).RectLeft = 0;
                (DSCLASSDEMO(SelectPort)->motion_list).RectTop = 0;
                (DSCLASSDEMO(SelectPort)->motion_list).RectRight = WIN_W;
                (DSCLASSDEMO(SelectPort)->motion_list).RectBottom = WIN_H;

#ifdef USE_PREVIEW_TIMER
                dsdemoclass[SelectPort]->SetPreview(0);
#else
                dsdemoclass[SelectPort]->SetPreview(1);
#endif
                if(DSCLASSDEMO(SelectPort)->motion_flag)
                    dsdemoclass[SelectPort]->SetupMotion();
                FullArea = TRUE;
            }
            else
            {
                dsdemoclass[SelectPort]->DestoryMotion();
                dsdemoclass[SelectPort]->StopPreview();
                dsdemoclass[SelectPort]->SetdstRect(chan);
                dsdemoclass[SelectPort]->SetMotion(chan);

                erase();
                paintEvent(&Qp);
                for(temp = 0; temp < totalport; temp++)
                {
#ifdef USE_PREVIEW_TIMER
                    dsdemoclass[temp]->SetPreview(0);
#else
                    dsdemoclass[temp]->SetPreview(1);
#endif
                }

                MotionBegin();
                FullArea = FALSE;
            }
        }
    }
    else if(e->button() == Qt::RightButton && !FullArea)
    {
        OutputDebugString("change the status, bPreviewOpen[%2d] = %d\n", SelectPort,
                          DSCLASSDEMO(SelectPort)->bPreviewOpen);
        if(DSCLASSDEMO(SelectPort)->bPreviewOpen)
        {
            if(!AllArea)
            {
                for(temp = 0; temp < totalport; temp++)
                {
                    if(DSCLASSDEMO(temp)->bPreviewOpen)
                        dsdemoclass[temp]->StopPreview();
                    if(DSCLASSDEMO(temp)->motion_flag)
                        dsdemoclass[temp]->DestoryMotion();
                }
                (DSCLASSDEMO(SelectPort)->dstRect).x = 0;
                (DSCLASSDEMO(SelectPort)->dstRect).y = 0;
                (DSCLASSDEMO(SelectPort)->dstRect).w = fsWidth;
                (DSCLASSDEMO(SelectPort)->dstRect).h = fsHeight;
                (DSCLASSDEMO(SelectPort)->motion_list).RectLeft = 0;
                (DSCLASSDEMO(SelectPort)->motion_list).RectTop = 0;
                (DSCLASSDEMO(SelectPort)->motion_list).RectRight = fsWidth;
                (DSCLASSDEMO(SelectPort)->motion_list).RectBottom = fsHeight;

#ifdef USE_PREVIEW_TIMER
                dsdemoclass[SelectPort]->SetPreview(0);
#else
                dsdemoclass[SelectPort]->SetPreview(1);
#endif
                if(DSCLASSDEMO(SelectPort)->motion_flag)
                    dsdemoclass[SelectPort]->SetupMotion();
                AllArea = TRUE;
                OutputDebugString("change the status complete, bPreviewOpen[%2d] = %d\n",
                                  SelectPort, DSCLASSDEMO(SelectPort)->bPreviewOpen);
            }
            else
            {
                OutputDebugString("&&&&&&&&&&&&&&&&&&&&&&Begin Ending FullScreen\n");
                dsdemoclass[SelectPort]->DestoryMotion();
                dsdemoclass[SelectPort]->StopPreview();
                dsdemoclass[SelectPort]->SetdstRect(chan);
                dsdemoclass[SelectPort]->SetMotion(chan);

                erase();

                /*
                 * paintEvent( &Qp );
                 */
                for(temp = 0; temp < totalport; temp++)
                {
#ifdef USE_PREVIEW_TIMER
                    dsdemoclass[temp]->SetPreview(0);
#else
                    dsdemoclass[temp]->SetPreview(1);
#endif
                }

                MotionBegin();
                AllArea = FALSE;
            }
        }

        OutputDebugString("change the status, bPreviewOpen[%2d] = %d\n", SelectPort,
                          DSCLASSDEMO(SelectPort)->bPreviewOpen);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::MotionBegin()  /* 本应该这段程序直接放在mouseDoubleClickEvent中, */

/* 但是经过测试,发现立刻打开motion会产生干扰,所以另新建了个MotionBegin程序 */
{
    int port;
    for(port = 0; port < GetTotalChannels(); port++)
    {
        if(DSCLASSDEMO(port)->motion_flag)
            dsdemoclass[port]->SetupMotion();
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::PtInRect(SDL_Rect rect, int x, int y)
{
    if((x > rect.x) && (x < (rect.x + rect.w)) && (y > rect.y) && (y < (rect.y + rect.h)))
        return 1;
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::GetSelectPort()
{
    return SelectPort;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::GetLastPort()
{
    return LastPort;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
bool QSDLWidget::GetFullState()
{
    return FullArea;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::StreamRead_callback(int channel, char* databuf, int frametype, int framelength)
{
    int head_size = 40;

    pthread_mutex_lock(&(DSCLASSDEMO(channel)->major_start));
    pthread_mutex_lock(&(DSCLASSDEMO(channel)->minor_start));

#if 0
    switch(frametype)
    {
        case PktSubIFrames:
            printf(" here PktSubIFrames\n");
            break;

        case PktSubPFrames:
            printf(" here PktSubPFrames\n");
            break;

        case PktSubBBPFrames:
            {
                printf(" here PktSubBBPFrames\n");
                break;
            }

        case PktSubSysHeader:
            {
                printf(" here PktSubSysHeader\n");
                break;
            }

        case PktSysHeader:
            {
                printf(" here PktSysHeader: ,net: %d\n", DSCLASSDEMO(channel)->net_flag);
                break;
            }

        case PktIFrames:
            {
                printf(" here PktIFrames\n");
                break;
            }

        case PktPFrames:
            {
                printf(" here PktPFrames\n");
                break;
            }

        case PktBBPFrames:
            {
                printf(" here PktBBPFrames\n");
                break;
            }

        case PktAudioFrames:
            {
                printf(" here PktAudioFrames\n");
                break;
            }

        case PktSFrames:
            {
                printf(" here PktSFrames\n");
                break;
            }

        case PktMotionDetection:
            {
                printf(" here PktMotionDetection\n");
                break;
            }

        default:
            break;
    }
#endif
    switch(frametype)
    {
        case PktSubIFrames:
        case PktSubPFrames:
        case PktSubBBPFrames:
        case PktSubSysHeader:
            {
                if(frametype == PktSubSysHeader)
                {
                    memcpy(DSCLASSDEMO(channel)->cifqciffile_head, databuf, framelength);
                    if(DSCLASSDEMO(channel)->cifqcif_fd)
                    {
                        (DSCLASSDEMO(channel)->cifqcif_fd)->writeBlock(databuf, framelength);
                        DSCLASSDEMO(channel)->cifqciffilehead_flag = 1;
                    }

                    head_size = framelength;
                }

                if(DSCLASSDEMO(channel)->cifqcif_flag && DSCLASSDEMO(channel)->cifqcif_fd &&
                   (DSCLASSDEMO(channel)->cifqciffilehead_flag))
                {
                    (DSCLASSDEMO(channel)->cifqcif_fd)->writeBlock(databuf, framelength);
                }
                break;
            }

        case PktAudioFrames:
            {
                if(DSCLASSDEMO(channel)->cifqcif_flag && DSCLASSDEMO(channel)->cifqcif_fd &&
                   DSCLASSDEMO(channel)->cifqciffilehead_flag)
                {
                    (DSCLASSDEMO(channel)->cifqcif_fd)->writeBlock(databuf, framelength);
                }

                if(DSCLASSDEMO(channel)->record_flag && DSCLASSDEMO(channel)->record_fd &&
                   DSCLASSDEMO(channel)->filehead_flag)
                {
                    (DSCLASSDEMO(channel)->record_fd)->writeBlock(databuf, framelength);
                }
                break;
            }

        case PktSysHeader:
        case PktIFrames:
        case PktPFrames:
        case PktBBPFrames:
        case PktSFrames:
            {
                if(frametype == PktSysHeader)
                {
                    memcpy(DSCLASSDEMO(channel)->file_head, databuf, framelength);
                    if(DSCLASSDEMO(channel)->record_fd)
                    {
                        (DSCLASSDEMO(channel)->record_fd)->writeBlock(databuf, framelength);
                        DSCLASSDEMO(channel)->nodeinfo.FrameNum = 0;
                        DSCLASSDEMO(channel)->filehead_flag = 1;
                    }

                    head_size = framelength;
                }

                if(DSCLASSDEMO(channel)->record_flag && DSCLASSDEMO(channel)->record_fd &&
                   DSCLASSDEMO(channel)->index_fd && DSCLASSDEMO(channel)->filehead_flag)
                {
                    (DSCLASSDEMO(channel)->record_fd)->writeBlock(databuf, framelength);
                    DSCLASSDEMO(channel)->nodeinfo.FilePos += framelength;
                    if(frametype == PktIFrames)
                    {
                        (DSCLASSDEMO(channel)->index_fd)->writeBlock((char*)(&(DSCLASSDEMO(channel)->nodeinfo)),
                                                                                     sizeof(INDEX_NODE));
                        DSCLASSDEMO(channel)->nodeinfo.FrameNum++;
                    }

                    if(frametype == PktBBPFrames)
                        DSCLASSDEMO(channel)->nodeinfo.FrameNum += 3;
                }

                DSCLASSDEMO(channel)->bit_rate_size += framelength;
                break;
            }

        case PktMotionDetection:
            {
                int length;
                length = framelength;   /* fprintf(stderr,"motion
                                         * pkt!\n");
                                         * */
                memcpy(DSCLASSDEMO(channel)->motion_data, databuf, length); /* fixed at 2005.3.1 */
                if(DSCLASSDEMO(channel)->motion_flag)
                    motion_detect(channel);
                break;
            }

        default:
            break;
    }

    if(DSCLASSDEMO(channel)->net_flag)
    {
        switch(frametype)
        {
            case PktSysHeader:
            case PktIFrames:
            case PktPFrames:
            case PktBBPFrames:
            case PktSFrames:
                {
                    if(DSCLASSDEMO(channel)->net_head_flag == 0)
                    {
                        if(frametype != PktSysHeader)
                        {
                            MP4_ServerWriteDataEx(channel, DSCLASSDEMO(channel)->file_head, head_size,
                                                  PktSysHeader, 0, 0);      /* 双编码的时候用这个函数？ */
                        }
                        else
                            MP4_ServerWriteDataEx(channel, (char*)databuf, framelength, frametype, 0, 0);

                        DSCLASSDEMO(channel)->net_head_flag = 1;
                    }
                    else
                        MP4_ServerWriteDataEx(channel, (char*)databuf, framelength, frametype, 0, 0);

                    break;
                }

            case PktSubIFrames:
            case PktSubPFrames:
            case PktSubBBPFrames:
            case PktSubSysHeader:
                {
                    if(DSCLASSDEMO(channel)->subchan_net_head_flag == 0)
                    {
                        if(frametype != PktSubSysHeader)
                        {
                            MP4_ServerWriteDataEx(channel, DSCLASSDEMO(channel)->cifqciffile_head,
                                                  head_size, PktSysHeader, 0, 1);
                        }
                        else
                            MP4_ServerWriteDataEx(channel, (char*)databuf, framelength, frametype, 0, 1);
                        DSCLASSDEMO(channel)->subchan_net_head_flag = 1;
                    }
                    else
                        MP4_ServerWriteDataEx(channel, (char*)databuf, framelength, frametype, 0, 1);

                    break;
                }

            case PktAudioFrames:
                {
                    MP4_ServerWriteDataEx(channel, (char*)databuf, framelength, frametype, 0, 0);
                    MP4_ServerWriteDataEx(channel, (char*)databuf, framelength, frametype, 0, 1);
                    break;
                }
        }

        if(checkpass[channel])
        {
            CaptureIFrame(DSCLASSDEMO(channel)->ChannelHandle);
            checkpass[channel] = 0;
        }
    }

    pthread_mutex_unlock(&(DSCLASSDEMO(channel)->major_start));
    pthread_mutex_unlock(&(DSCLASSDEMO(channel)->minor_start));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::motion_detect(int port)
{
    float           ratioX, ratioY;
    unsigned int*   pBuf;
    int             maxY;
    int             i, j;
    int             chan;
    int             Left, Top, Width, Height;

    OutputDebugString("in function QSDLWidget::motion_detect\n");

    chan = GetChanNum();
    maxY = dsdemoclass[port]->GetVideoHeight();
    if(FullArea)
    {
        if(port != SelectPort)
            return;
        ratioX = (float)WIN_W / (float)704; /* 由于全屏后存在一定的差异 800*700 */
        ratioY = (float)WIN_H / (float)(maxY);
    }
    else
    {
        ratioX = (float)(WIN_W / chan) / (float)704;
        ratioY = (float)(WIN_H / chan) / (float)(maxY);
    }

    maxY = maxY / 32;
    pBuf = (unsigned int*)(DSCLASSDEMO(port)->motion_data);

    for(i = 0; i < maxY; i++)
    {
        int val = pBuf[i];

        for(j = 0; j < 22; j++)
        {
            if(val & (1 << j))
            {
                Left = (DSCLASSDEMO(port)->motion_list).RectLeft + (int)((float)j * ratioX * 32.0);
                Top = (DSCLASSDEMO(port)->motion_list).RectTop + (int)((float)i * ratioY * 32.0);
                Width = int(ratioX * 32.0);
                Height = int(ratioY * 32.0);
#if 0
                QPainter    Qp;
                QBrush      Qb(QColor(0, 255, 0), Qt::SolidPattern);
                Qp.begin(this);
                Qp.fillRect(Left, Top, Width, Height, Qb);
                Qp.end();
#endif
#if 0
                bitBlt(this, Left, Top, pm, 0, 0, Width, Height);
#endif

                QRect*  rectp;
                rectp = new QRect(Left, Top, Width, Height);

                QCustomEvent*   event;
                event = new QCustomEvent(198109);
                event->setData(rectp);
                QApplication::postEvent(this, event);
            }
        }
    }
}
/**/
void QSDLWidget::customEvent(QCustomEvent* e)
{
    QRect*  rt = (QRect *) (e->data());

    if(e->type() == 198109)                 /* It must be a QMotionEvent */
    {
        bitBlt(this, rt->left(), rt->top(), pm, 0, 0, rt->width(), rt->height());
    }

    delete rt;
}
