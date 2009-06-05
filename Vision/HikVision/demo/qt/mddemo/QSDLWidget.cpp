#include <qdatetime.h>
#include <qtimer.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>

#include "interfun.h"
#include "QSDLWidget.h"

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

QSDLWidget::QSDLWidget(QWidget* parent, const char* name) :
    QWidget(parent, name, 0)
{
    SelectPort = 0;
    sameSetting = 0;
    fullArea = 0;
    RunStream = 0;      /* 这个标志的作用是看是否有流媒体在运行 */
    RunVideoOut = 0;    /* 这个标志的作用是看是否有VideoOut在运行 */
    playSoundPort = -1;

#ifdef RAWSTREAM
    rawFile = new QFile("./displayimagefile.yuv");
    rawFile->open(IO_ReadWrite | IO_Truncate);

    imagebuf = (unsigned char*)malloc(704 * 576 * 3 / 2 * sizeof(unsigned char));
    memset(imagebuf, 0x0, 704 * 576 * 3 / 2 * sizeof(unsigned char));
#endif
#ifdef CIRCLE
    pthread_mutex_init(&mutex, NULL);
#endif
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::SetEnv()
{
    int             i, tempHandle;
    int             displayChannelCount;
    int             totalcard;
    SDL_Surface*    pOverlayScreen;
    char            SDL_windowhack[32];

    QsStruct        QsTemp;
    QsTemp.qsclass = this;
    QsTemp.pmf = &QSDLWidget::ReadDataCallBack;

#ifdef RAWSTREAM
    QsTemp.ImageStreamCallbackpmf = &QSDLWidget::ImageStreamCallback;
#endif
    SetsQstruct(&QsTemp);

    sprintf(SDL_windowhack, "SDL_WINDOWID=%ld", winId());
    OutputDebugString("%ld \n", winId());
    putenv(SDL_windowhack);

    putenv("SDL_VIDEO_YUV_HWACCEL=0");

    OutputDebugString("Init the sdl...\n");

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        OutputErrorString("<sdk_error> init sdl failed!%s\n", SDL_GetError());
        exit(0);
    }

    pOverlayScreen = SDL_SetVideoMode(WIN_W, WIN_H, 0, SDL_HWSURFACE);  /* here it is freed by
                                                                         * SDL_Quit();
                                                                         * */

    if(pOverlayScreen == NULL)
    {
        OutputErrorString("<sdk_error> create the sdl screen failed! for %s!\n", SDL_GetError());
        SDL_Quit();
        exit(0);
    }

    HW_InitDecDevice(&totalport);
    totalcard = GetTotalDSPs();

    OutputDebugString("total dsps = %d, total ports = %ld, Display Channel = %d\n", totalcard,
                      totalport, GetDisplayChannelCount());

    mddemoclass = (Dsclass**)malloc(totalport * sizeof(Dsclass *));
    memset(mddemoclass, 0x0, totalport * sizeof(Dsclass *));

    for(i = 0; i < totalport; i++)
    {
        if(HW_ChannelOpen(i, &tempHandle) < 0)
        {
            OutputErrorString("<demo_info> Open the decode channle failed!\n");
            HW_ReleaseDecDevice();
        }
        else
        {
            mddemoclass[i] = new Dsclass(i, tempHandle, pOverlayScreen);
            mddemoclass[i]->SetdstRect(GetChanNum());
            mddemoclass[i]->startDisplayThread();
        }
    }

    displayChannelCount = GetDisplayChannelCount();
    for(i = 0; i < displayChannelCount; i++)
        SetDisplayStandard(i, StandardPAL);

#ifdef RAWSTREAM
    RegisterDisplayVideoCaptureCallback((IMAGE_STREAM_CALLBACK) InterImageStream, NULL);
#endif
    timer = (QTimer**)malloc(totalport * sizeof(QTimer *));
    time = (QTime**)malloc(totalport * sizeof(QTime *));

    for(i = 0; i < totalport; i++)
    {
        char    temp[3];

        sprintf(temp, "%2d", i);
        temp[3] = '\0';

        timer[i] = new QTimer(this, temp);
        connect(timer[i], SIGNAL(timeout()), SLOT(process_time()));

        time[i] = new QTime();
        time[i]->start();
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::process_time()
{
    int             temp;
    int             Nowport = 0;
    int             timepassed;

    int             bitRate;
    unsigned long   framerate;
    unsigned long   totalDecFrame;

    for(temp = 0; temp < totalport; temp++)
    {
        QTimer*     Qtemp = (QTimer *) (this->sender());
        if(atoi(Qtemp->name()) == temp)
        {
            Nowport = temp;
            break;
        }
    }

    timepassed = time[Nowport]->elapsed();  /* ms */

    bitRate = (int)(((RetMDDEMO(Nowport)->datasum) * 8) / ((float)timepassed / 1000));

    emit    BitRateChange(Nowport, bitRate);

    HW_GetCurrentFrameRate(RetMDDEMO(Nowport)->ChannelHandle, &framerate);

    emit    RateChange(Nowport, framerate);

    HW_GetPlayedFrames(RetMDDEMO(Nowport)->ChannelHandle, &totalDecFrame);

    emit    FrameChange(Nowport, totalDecFrame);

    if(mddemoclass[Nowport]->IsPlayOver() && (!bitRate && !framerate))
    {
        erase(RetMDDEMO(Nowport)->dstRect.x, RetMDDEMO(Nowport)->dstRect.y,
              RetMDDEMO(Nowport)->dstRect.w, RetMDDEMO(Nowport)->dstRect.h);

        timer[Nowport]->stop();

#ifdef CIRCLE
        usleep(1000);
        pthread_mutex_lock(&mutex);
        SelectPort = Nowport;
        FileOpen(FileName);
        pthread_mutex_unlock(&mutex);
#endif
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
long int QSDLWidget::RetTotalPort()
{
    return totalport;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::SameSetting()
{
    sameSetting = !sameSetting;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ImageStreamCallback(unsigned int displayNum, void* context)
{
#ifdef RAWSTREAM
    if(displayNum == 1)
    {
        rawFile->writeBlock((char*)imagebuf, 704 * 576 * 3 / 2);
        OutputDebugString("have write stream into it\n");
    }
#endif
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::mousePressEvent(QMouseEvent* e)
{
    int i;

    if(fullArea)
        return;

    QPoint  pnt = e->pos();

    if(e->button() == Qt::LeftButton)
    {
        for(i = 0; i < totalport; i++)
        {
            if(PtInRect(RetMDDEMO(i)->dstRect, pnt.x(), pnt.y()))
            {
                SelectPort = i;
                break;
            }
        }

        if(i == totalport)
            return;

        emit    portchanged(SelectPort);

        emit    playStatus(RetMDDEMO(SelectPort)->playType);

        /* emit */
        emit    BitRateChange(SelectPort, 0);
        emit    RateChange(SelectPort, 0);
        emit    FrameChange(SelectPort, 0);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
    int     temp;

    QPoint  pnt = e->pos();
    int     chan = GetChanNum();

    if(!fullArea)
    {
        int i;
        for(i = 0; i < totalport; i++)
        {
            if(PtInRect(RetMDDEMO(i)->dstRect, pnt.x(), pnt.y()))
                break;
        }

        if(i == totalport)
            return;
    }

    if(e->button() == Qt::LeftButton)
    {
        if(RetMDDEMO(SelectPort)->bDisplayOpen)
        {
            if(!fullArea)
            {
                for(temp = 0; temp < totalport; temp++)
                {
                    if(RetMDDEMO(temp)->bDisplayOpen && RetMDDEMO(temp)->playType)
                        mddemoclass[temp]->stopDisplay();
                }

                /*
                 * OutputErrorString("@@@@stop Display not or yes\n");
                 */
                (RetMDDEMO(SelectPort)->dstRect).x = 0;
                (RetMDDEMO(SelectPort)->dstRect).y = 0;
                (RetMDDEMO(SelectPort)->dstRect).w = WIN_W;
                (RetMDDEMO(SelectPort)->dstRect).h = WIN_H;

                /* 必须保证同步,即打开startDisplay的时候必须保证已经关闭了stopDisplay. */
                mddemoclass[SelectPort]->startDisplay();
                fullArea = 1;
            }
            else
            {
                mddemoclass[SelectPort]->stopDisplay();
                mddemoclass[SelectPort]->SetdstRect(chan);

                erase();

                for(temp = 0; temp < totalport; temp++)
                {
                    if(RetMDDEMO(temp)->playType && !(RetMDDEMO(temp)->bDisplayOpen))
                    {
                        mddemoclass[temp]->startDisplay();
                    }
                }

                fullArea = 0;
            }
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
char* QSDLWidget::RetIP()
{
    return RetMDDEMO(SelectPort)->serverip;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::RetChan()
{
    return RetMDDEMO(SelectPort)->serverchan;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
char* QSDLWidget::RetFilePosition()
{
    return RetMDDEMO(SelectPort)->filename;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::RetSoundPort()
{
    return playSoundPort;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::PlaySound()
{
    int ret = -1;

    if(playSoundPort == SelectPort)
        return 0;

    if(playSoundPort == -1)
    {
        ret = HW_PlaySound(RetMDDEMO(SelectPort)->ChannelHandle);
        if(ret < 0)
        {
            fprintf(stderr, "sorry, SetAudioPreview failed\n");
            return -1;
        }

        ret = HW_SetVolume(RetMDDEMO(SelectPort)->ChannelHandle, 0xffff);
        if(ret < 0)
        {
            fprintf(stderr, "sorry, SetAudioPreview failed\n");
            return -1;
        }

        ret = HW_SetAudioPreview(RetMDDEMO(SelectPort)->ChannelHandle, 1);
        if(ret < 0)
        {
            fprintf(stderr, "sorry, SetAudioPreview failed\n");
            return -1;
        }

        playSoundPort = SelectPort;
        return 0;
    }

    ret = HW_SetAudioPreview(RetMDDEMO(playSoundPort)->ChannelHandle, 0);
    if(ret < 0)
    {
        fprintf(stderr, "sorry, SetAudioPreview failed\n");
        return -1;
    }

    HW_StopSound(RetMDDEMO(playSoundPort)->ChannelHandle);
    if(ret < 0)
    {
        fprintf(stderr, "sorry, SetAudioPreview failed\n");
        return -1;
    }

    HW_PlaySound(RetMDDEMO(SelectPort)->ChannelHandle);
    if(ret < 0)
    {
        fprintf(stderr, "sorry, SetAudioPreview failed\n");
        return -1;
    }

    HW_SetAudioPreview(RetMDDEMO(SelectPort)->ChannelHandle, 1);
    if(ret < 0)
    {
        fprintf(stderr, "sorry, SetAudioPreview failed\n");
        return -1;
    }

    playSoundPort = SelectPort;
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::StopSound()
{
    int ret = -1;

    ret = HW_SetAudioPreview(RetMDDEMO(playSoundPort)->ChannelHandle, 0);
    if(ret < 0)
    {
        fprintf(stderr, "sorry, SetAudioPreview failed\n");
        return -1;
    }

    ret = HW_StopSound(RetMDDEMO(playSoundPort)->ChannelHandle);
    if(ret < 0)
    {
        fprintf(stderr, "sorry, SetAudioPreview failed\n");
        return -1;
    }

    playSoundPort = -1;

    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::StringtoChar(const QString& aFile, char* cstr)
{
    int     m = aFile.length();
    QChar   da;

    int     k;

    for(k = 0; k < m; k++)
    {
        da = aFile.at(k);
        cstr[k] = da.latin1();
    }

    cstr[m] = '\0';
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::IPChange(const QString& ipaddress)
{
    int k;
    if(sameSetting)
    {
        for(k = 0; k < totalport; k++)
        {
            StringtoChar(ipaddress, RetMDDEMO(k)->serverip);
            OutputDebugString(RetMDDEMO(k)->serverip);
            OutputDebugString("\n");
        }
    }
    else
    {
        StringtoChar(ipaddress, RetMDDEMO(SelectPort)->serverip);
        OutputDebugString(RetMDDEMO(SelectPort)->serverip);
        OutputDebugString("\n");
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ServerChanSet(int chan)
{
    int k;

    if(sameSetting)
    {
        for(k = 0; k < totalport; k++)
        {
            RetMDDEMO(k)->serverchan = chan;
            OutputDebugString("Channel %2d Change Serverchan to %d\n", k, RetMDDEMO(k)->serverchan);
        }
    }
    else
    {
        RetMDDEMO(SelectPort)->serverchan = chan;
        OutputDebugString("Channel %2d Change Serverchan to %d\n", SelectPort,
                          RetMDDEMO(SelectPort)->serverchan);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::FileOpen(const QString& aFile)
{
    OutputDebugString("here is in function QSDLWidget::FileOpen\n");

    int k;

    FileName = aFile;

    if(sameSetting)
    {
        for(k = 0; k < totalport; k++)
        {
            StringtoChar(aFile, RetMDDEMO(k)->filename);
            OutputDebugString("*******Open File %s\n", RetMDDEMO(k)->filename);

            if(RetMDDEMO(k)->bDisplayOpen)
                mddemoclass[k]->StopPlay();
            timer[k]->start(200);
            time[k]->restart();
            mddemoclass[k]->StartFilePlay();
        }

#ifdef circle
        sameSetting = 0;    /* 临时加上 */
#endif
    }
    else
    {
        OutputDebugString("here sameSetting = 0 \n");
        StringtoChar(aFile, RetMDDEMO(SelectPort)->filename);
        OutputDebugString(RetMDDEMO(SelectPort)->filename);
        OutputDebugString("\n");

        if(RetMDDEMO(SelectPort)->bDisplayOpen)
            mddemoclass[SelectPort]->StopPlay();
        OutputDebugString("before  StartFilePlay\n");
        timer[SelectPort]->start(200);
        time[SelectPort]->restart();
        mddemoclass[SelectPort]->StartFilePlay();
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::FileStreamClose(int style)
{
    int k;

    if(sameSetting)
    {
        for(k = 0; k < totalport; k++)
        {
            if(RetMDDEMO(k)->bDisplayOpen && (RetMDDEMO(k)->playType == style))
            {
                mddemoclass[k]->StopPlay();
                timer[k]->stop();

                emit    RateChange(k, 0);
                emit    FrameChange(k, 0);
            }
        }
    }
    else
    {
        if(RetMDDEMO(SelectPort)->bDisplayOpen && (RetMDDEMO(SelectPort)->playType == style))
        {
            mddemoclass[SelectPort]->StopPlay();
            timer[SelectPort]->stop();

            emit    RateChange(SelectPort, 0);
            emit    FrameChange(SelectPort, 0);
        }
    }

    for(k = 0; k < totalport; k++)
    {
        if(RetMDDEMO(k)->playType == DECODESTREAM)
            break;
    }

    if(RunStream == 1 && (k == 9))
    {
        RunStream = 0;
        MP4_ClientCleanup();
    }

    OutputDebugString("<demo_info> file close\n");
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::ReadDataCallBack(int StockHandle, char* pPacketBuffer, int nPacketSize)
{
    int temp, port;

    for(temp = 0; temp < totalport; temp++)
    {
        if(!RetMDDEMO(temp))
            return;

        if(RetMDDEMO(temp)->clienthandle == StockHandle)
            break;
    }

    port = temp;

    RetMDDEMO(port)->datasum += nPacketSize;
    if(RetMDDEMO(port)->playType)
    {
        if(HW_InputData(RetMDDEMO(port)->ChannelHandle, pPacketBuffer, nPacketSize) < 0)
        {
            OutputErrorString("<demo_info> input data error:0x%x, port%d, Channel%d\n",
                              GetLastErrorNum(), port, RetMDDEMO(port)->ChannelHandle);
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::OpenStream()
{
    int             temp;
    int             retval;
    CLIENTSHOWRECT  clientrect;
    memset(&clientrect, 0x0, sizeof(CLIENTSHOWRECT));

    CLIENT_VIDEOINFO    Clientinfo;
    char                streamheader[128];
    int                 header_len;

    /* init the net client connect */
    MP4_ClientSetNetPort(5050, 6050);
    MP4_SetDecoder(0);                  /* disable the client sdk to decode stream */

    retval = MP4_ClientStartup(clientrect, NULL);
    OutputDebugString("ClientStartup successful, retval = %d, uWidth = %d, uHeight = %d\n", retval,
                      clientrect.uWidth, clientrect.uHeight);

    Clientinfo.m_bRight = 0;
    Clientinfo.m_sUserName = "username1234567890";
    Clientinfo.m_sUserPassword = "userpassword1234567890";
    Clientinfo.m_bUserCheck = 0;
    Clientinfo.subshow_x = 0;
    Clientinfo.subshow_y = 0;
    Clientinfo.subshow_uWidth = 0;
    Clientinfo.subshow_uHeight = 0;
    Clientinfo.m_bSendMode = PTOPTCPMODE;

    if(sameSetting)
    {
        for(temp = 0; temp < totalport; temp++)
        {
            if(RetMDDEMO(temp)->bDisplayOpen)
                mddemoclass[temp]->StopPlay();

            Clientinfo.m_bRemoteChannel = RetMDDEMO(temp)->serverchan;

            /*
             * strcpy(Clientinfo.m_sIPAddress, RetMDDEMO(i)->serverip);
             */
            Clientinfo.m_sIPAddress = RetMDDEMO(temp)->serverip;

            OutputDebugString("Create client %dth\n", temp);
            RetMDDEMO(temp)->clienthandle = MP4_ClientStart(&Clientinfo, InterReadDataCallBack);
            OutputDebugString("<debug_info> ......clienthandle = %d, port = %d, ChannelHandle = %d\n",
                              RetMDDEMO(temp)->clienthandle, temp, RetMDDEMO(temp)->ChannelHandle);

            if(RetMDDEMO(temp)->clienthandle == -1)
            {
                OutputErrorString("<demo_info> clientstart error:%x\n", MP4_ClientGetLastErrorNum());
                MP4_ClientCleanup();    /* 注意一点就是当初始化失败的时候必须将checkbutton置为FALSE */
                return -1;
            }

            RetMDDEMO(temp)->datasum = 0;
            header_len = 0;
            OutputDebugString("<demo_info> to start capture!\n");
            MP4_ClientStartCaptureEx(RetMDDEMO(temp)->clienthandle, streamheader, &header_len);
            if(header_len == 0)
            {
                OutputErrorString("<demo_info> have not get the stream header!\n");
                return -1;
            }

            OutputDebugString("<demo_info> get the stream header!len:%d\n", header_len);

            timer[temp]->start(200);
            time[temp]->restart();

            mddemoclass[temp]->StartStreamPlay(streamheader, header_len);
            OutputDebugString("here now the Mp4 client run well, %x\n", MP4_ClientGetLastErrorNum());
        }
    }
    else
    {
        if(RetMDDEMO(SelectPort)->bDisplayOpen)
            mddemoclass[SelectPort]->StopPlay();

        Clientinfo.m_bRemoteChannel = RetMDDEMO(SelectPort)->serverchan;

        /*
         * strcpy(Clientinfo.m_sIPAddress, RetMDDEMO(i)->serverip);
         */
        Clientinfo.m_sIPAddress = RetMDDEMO(SelectPort)->serverip;

        RetMDDEMO(SelectPort)->clienthandle = MP4_ClientStart(&Clientinfo, InterReadDataCallBack);
        OutputDebugString("<debug_info> ......clienthandle = %d, port = %d, ChannelHandle = %d\n",
                          RetMDDEMO(SelectPort)->clienthandle, SelectPort,
                          RetMDDEMO(SelectPort)->ChannelHandle);

        if(RetMDDEMO(SelectPort)->clienthandle == -1)
        {
            OutputErrorString("<demo_info> clientstart error:%x\n", MP4_ClientGetLastErrorNum());
            MP4_ClientCleanup();        /* 注意一点就是当初始化失败的时候必须将checkbutton置为FALSE */
            return -1;
        }

        RetMDDEMO(SelectPort)->datasum = 0;
        header_len = 0;
        OutputDebugString("<demo_info> to start capture!\n");
        MP4_ClientStartCaptureEx(RetMDDEMO(SelectPort)->clienthandle, streamheader, &header_len);
        if(header_len == 0)
        {
            OutputErrorString("<demo_info> have not get the stream header!\n");
            return -1;
        }

        OutputDebugString("<demo_info> get the stream header!len:%d\n", header_len);

        timer[SelectPort]->start(200);
        time[SelectPort]->restart();

        mddemoclass[SelectPort]->StartStreamPlay(streamheader, header_len);
    }

    RunStream = 1;
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::OpenVideoOut()
{
    REGION_PARAM    param[MAX_DISPLAY_REGION];

    int             i;
    int             retval;

    memset(param, 0x0, sizeof(param));

    for(i = 0; i < MAX_DISPLAY_REGION; i++)
    {
        param[i].r = i * 8 + 128;
        param[i].g = i * 16;
        param[i].b = i * 16;
    }

    /* to set the port 0 as the 4 cif windows */
    param[0].left = 0;
    param[0].top = 0;
    param[0].width = 352;
    param[0].height = 288;
    param[1].left = 352;
    param[1].top = 0;
    param[1].width = 352;
    param[1].height = 288;
    param[2].left = 0;
    param[2].top = 288;
    param[2].width = 352;
    param[2].height = 288;
    param[3].left = 352;
    param[3].top = 288;
    param[3].width = 352;
    param[3].height = 288;
    SetDisplayRegion(0, 4, param, 0);   /* here 4个视频输出(矩阵输出) */

    /* to set the port1 as the 1 4Cif window */
    param[0].left = 0;
    param[0].top = 0;
    param[0].width = 704;
    param[0].height = 576;
    SetDisplayRegion(1, 1, param, 0);   /* here 1个视频输出 */

#ifdef DEBUG_OUT
    param[0].left = 0;
    param[0].top = 0;
    param[0].width = 352;
    param[0].height = 288;
    param[1].left = 352;
    param[1].top = 288;
    param[1].width = 352;
    param[1].height = 288;
    retval = SetDisplayRegion(2, 2, param, 0);  /* here 1个视频输出 */
    if(retval < 0)
        OutputErrorString("SetDisplayRegion error 1, error number 0x%x\n", GetLastErrorNum());

    param[0].left = 352;
    param[0].top = 0;
    param[0].width = 352;
    param[0].height = 288;
    param[1].left = 0;
    param[1].top = 288;
    param[1].width = 352;
    param[1].height = 288;
    retval = SetDisplayRegion(3, 2, param, 0);  /* here 1个视频输出 */
    if(retval < 0)
        OutputErrorString("SetDisplayRegion error 2, error number 0x%x\n", GetLastErrorNum());
#endif
    retval = SetDecoderVideoOutput(0, 0, 1, 0, 0, 0);
    if(retval < 0)
        OutputErrorString("Video Out error 1, error number 0x%x\n", GetLastErrorNum());
    retval = SetDecoderVideoOutput(1, 0, 1, 0, 1, 0);
    if(retval < 0)
        OutputErrorString("Video Out error 2, error number 0x%x\n", GetLastErrorNum());
    retval = SetDecoderVideoOutput(2, 0, 1, 0, 2, 0);
    if(retval < 0)
        OutputErrorString("Video Out error 3, error number 0x%x\n", GetLastErrorNum());

    /*
     * Set the video from the last decode out to as the region4 in por0 and the whole
     * port1
     */
    SetDecoderVideoOutput(3, 0, 1, 0, 3, 0);
    SetDecoderVideoOutput(3, 1, 1, 1, 0, 0);

#ifdef DEBUG_OUT
    SetDecoderVideoOutput(4, 0, 1, 0, 0, 0);
    SetDecoderVideoOutput(4, 1, 1, 0, 1, 0);

    SetDecoderVideoOutput(5, 0, 1, 1, 0, 0);
    SetDecoderVideoOutput(6, 0, 1, 1, 1, 0);
#endif
    SetDecoderAudioOutput(0, 1, 0);
    SetDecoderAudioOutput(1, 1, 1);

    RunVideoOut = 1;

#ifdef RAWSTREAM    /* start */
    if(SetDisplayVideoCapture(1, 1, 25, 704, 576, imagebuf) < 0)    /* 第一个参数是显示通道 */
        OutputErrorString("<demo_info>.............. set the image stream capture failed!\n");

    OutputErrorString("<demo_info> ................start image raw stream capture ok!\n");
#endif
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QSDLWidget::CloseVideoOut()
{
    int i;

    SetDecoderVideoOutput(0, 0, 0, 0, 0, 0);
    SetDecoderVideoOutput(1, 0, 0, 0, 1, 0);
    SetDecoderVideoOutput(2, 0, 0, 0, 2, 0);
    SetDecoderVideoOutput(3, 0, 0, 0, 3, 0);
    SetDecoderVideoOutput(3, 1, 0, 1, 0, 0);

#ifdef DEBUG_OUT
    SetDecoderVideoOutput(4, 0, 0, 0, 0, 0);
    SetDecoderVideoOutput(4, 1, 0, 0, 1, 0);

    SetDecoderVideoOutput(5, 0, 0, 1, 0, 0);
    SetDecoderVideoOutput(6, 0, 0, 1, 1, 0);
#endif

    /* stop the audio out */
    SetDecoderAudioOutput(0, 0, 0);
    SetDecoderAudioOutput(1, 0, 1);

    for(i = 0; i < GetDisplayChannelCount(); i++)
        ClearDisplayRegion(i, 0xffff);

    RunVideoOut = 0;

#ifdef RAWSTREAM    /* stop */
    if(SetDisplayVideoCapture(1, 0, 25, 704, 576, imagebuf) < 0)
        OutputErrorString("<demo_info>.............. stop the image stream capture failed!\n");

    OutputErrorString("<demo_info> ................stop image raw stream capture ok!\n");
#endif
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
QSDLWidget::~QSDLWidget()
{
    int i;
    int tempHandle;

#ifdef RAWSTREAM
    if(!rawFile)
    {
        delete rawFile;
        rawFile = NULL;
    }

    if(!imagebuf)
    {
        free(imagebuf);
        imagebuf = NULL;
    }
#endif
    if(playSoundPort != -1)
        StopSound();

    FileStreamClose(DECODEFILE);
    usleep(200);
    OutputDebugString("ending Decode File\n");

    FileStreamClose(DECODESTREAM);
    usleep(100);
    OutputDebugString("ending Decode Net Stream\n");

    if(RunVideoOut)
        CloseVideoOut();
    RunVideoOut = 0;

    if(mddemoclass)
    {
        for(i = 0; i < totalport; i++)
        {
            tempHandle = RetMDDEMO(i)->ChannelHandle;
            delete mddemoclass[i];
            mddemoclass[i] = NULL;
            HW_ChannelClose(tempHandle);
        }

        free(mddemoclass);
        mddemoclass = NULL;
    }

    if(timer)
    {
        for(i = 0; i < totalport; i++)
        {
            delete timer[i];
            delete time[i];
        }

        free(timer);
        free(time);
    }

#ifdef CIRCLE
    pthread_mutex_destroy(&mutex);
#endif
    OutputDebugString("Before HW_ReleaseDecDevice\n");
    HW_ReleaseDecDevice();
    SDL_Quit();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
inline MDDEMO* QSDLWidget::RetMDDEMO(int port)
{
    if(port < 0 || port > totalport)
        return NULL;

    if(!mddemoclass || !(mddemoclass[port]) || !(mddemoclass[port]->mddemo))
        return NULL;

    return mddemoclass[port]->mddemo;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int QSDLWidget::GetChanNum()
{
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
int QSDLWidget::PtInRect(SDL_Rect rect, int x, int y)
{
    if((x > rect.x) && (x < (rect.x + rect.w)) && (y > rect.y) && (y < (rect.y + rect.h)))
        return 1;
    return 0;
}
