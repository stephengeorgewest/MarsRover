#include <SDL/SDL.h>

#include "exvideo.h"
#include "dsdemo.h"
#include <qapplication.h>
extern QApplication* app;

#define USLEEPTIME (20 * 1000)

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

Dsclass::Dsclass(int port, int ChannelHandle, SDL_Surface* Screen)
{
    pOverlayScreen = Screen;
    logoptr = NULL;
    SignPort = port;

    dsdemo = (DSDEMO*)malloc(sizeof(DSDEMO));
    memset(dsdemo, 0, sizeof(DSDEMO));

    dsdemo->maskRect = (RECT*)malloc(5 * sizeof(RECT));
    memset(dsdemo->maskRect, 0, 5 * sizeof(RECT));

    sem_init(&(dsdemo->previewSem), 0, 0);
    dsdemo->previewconf.SyncSem = &(dsdemo->previewSem);

    logoptr = (unsigned char*)malloc(128 * 128 * 2);
    memset(logoptr, 0x0, 128 * 128 * 2 * sizeof(unsigned char));

    pthread_mutex_init(&(dsdemo->major_start), NULL);
    pthread_mutex_init(&(dsdemo->minor_start), NULL);
    pthread_mutex_init(&(dsdemo->imagefile_fd_start), NULL);

    dsdemo->ChannelHandle = ChannelHandle;

    GetVideoPara(dsdemo->ChannelHandle, &(dsdemo->videostandard), &((dsdemo->v_info).Brightness),
                 &((dsdemo->v_info).Contrast), &((dsdemo->v_info).Saturation),
                 &((dsdemo->v_info).Hue));
    memcpy(&(dsdemo->videoDefault), &(dsdemo->v_info), sizeof(video_info));

    GetCardInfo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetMotion(const int chan)
{
    dsdemo->motion_list.RectLeft = (SignPort % chan) * (WIN_W / chan);
    dsdemo->motion_list.RectTop = (SignPort / chan) * (WIN_H / chan);
    dsdemo->motion_list.RectRight = (SignPort % chan + 1) * (WIN_W / chan);
    dsdemo->motion_list.RectBottom = (SignPort / chan + 1) * (WIN_H / chan);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::MotionEnable()
{
    dsdemo->motion_rect.RectLeft = 0;
    dsdemo->motion_rect.RectTop = 0;
    dsdemo->motion_rect.RectRight = 704 - 1;
    dsdemo->motion_rect.RectBottom = GetVideoHeight() - 1;

    if(SetupMotionDetection(dsdemo->ChannelHandle, &(dsdemo->motion_rect), 1) < 0)
        OutputErrorString("<demo_info> setup motion detect(port %d) failed\n", SignPort);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetupMotion()
{
    OutputDebugString("in Dsclass::SetupMotion function\n");
    AdjustMotionDetectPrecision(dsdemo->ChannelHandle, dsdemo->motionSens, 2, 0);
    StartMotionDetection(dsdemo->ChannelHandle);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::DestoryMotion()
{
    StopMotionDetection(dsdemo->ChannelHandle);
    memset(dsdemo->motion_data, 0, sizeof(dsdemo->motion_data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetdstRect(const int chan)
{
    if(chan < 2)
    {
        OutputErrorString("<demo_info> chan setting error \n");
        return;
    }
    (dsdemo->dstRect).x = (SignPort % chan) * (WIN_W / chan);
    (dsdemo->dstRect).y = (SignPort / chan) * (WIN_H / chan);
    (dsdemo->dstRect).w = WIN_W / chan;
    (dsdemo->dstRect).h = WIN_H / chan;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetEncoderByFlag()
{
    Frame_info*     infoFrame = &(dsdemo->frameinfo);
    SetIBPMode(dsdemo->ChannelHandle, infoFrame->KeyFrameIntervals, infoFrame->BFrames,
               infoFrame->PFrames, infoFrame->FrameRate);
    SetDefaultQuant(dsdemo->ChannelHandle, infoFrame->IQuantVal, infoFrame->PQuantVal,
                    infoFrame->BQuantVal);
    SetEncoderPictureFormat(dsdemo->ChannelHandle, dsdemo->pictureFormat);

    if(dsdemo->cifqcif_flag)
    {
        SetupSubChannel(dsdemo->ChannelHandle, 1);
        SetIBPMode(dsdemo->ChannelHandle, infoFrame->KeyFrameIntervals, infoFrame->BFrames,
                   infoFrame->PFrames, infoFrame->FrameRate);
        SetDefaultQuant(dsdemo->ChannelHandle, infoFrame->IQuantVal, infoFrame->PQuantVal,
                        infoFrame->BQuantVal);
        SetSubEncoderPictureFormat(dsdemo->ChannelHandle, dsdemo->subpictureFormat);
        SetupSubChannel(dsdemo->ChannelHandle, 0);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetMaxBpsMode()
{
    int                     MaxBps = dsdemo->MaxBps;
    BitrateControlType_t    brc = dsdemo->brc;

    SetupBitrateControl(dsdemo->ChannelHandle, MaxBps);
    SetBitrateControlMode(dsdemo->ChannelHandle, brc);

    SetupSubChannel(dsdemo->ChannelHandle, 1);
    SetupBitrateControl(dsdemo->ChannelHandle, MaxBps);
    SetBitrateControlMode(dsdemo->ChannelHandle, brc);
    SetupSubChannel(dsdemo->ChannelHandle, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetOsddemo()
{
    unsigned short      Translucent = (dsdemo->osdinfo).Translucent;
    int                 Brightness = (dsdemo->osdinfo).Brightness;
    int                 PosX = (dsdemo->osdinfo).PosX;
    int                 PosY = (dsdemo->osdinfo).PosY;

    unsigned short*     p[2];
    unsigned short      format1[] = { PosX, PosY, '°ì', '¹«', 'µã', '\0' };
    unsigned short      format2[] =
    {
        PosX + 64,
        PosY,
        _OSD_YEAR4,
        '-',
        _OSD_MONTH2,
        '-',
        _OSD_DAY,
        ' ',
        _OSD_HOUR24,
        ':',
        _OSD_MINUTE,
        ':',
        _OSD_SECOND,
        '\0'
    };

    p[0] = format1;
    p[1] = format2;
    SetOsdDisplayModeEx(dsdemo->ChannelHandle, Brightness, Translucent, 0x2020000, 2, p);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetDsLogo()
{
    int Translucent = (dsdemo->logoinfo).Translucent;
    int PosX = (dsdemo->logoinfo).PosX;
    int PosY = (dsdemo->logoinfo).PosY;

    int logo_w, logo_h;

    if(LoadYUVFromBmpFile("logo.bmp", logoptr, 128 * 128 * 2, &logo_w, &logo_h) < 0)
    {
        OutputErrorString("<demo_info> load bmp file error! error code is 0x%x!\n",
                          GetLastErrorNum());
        return;
    }

    if(dsdemo->videostandard == StandardNTSC)
    {
        if(SetLogo(dsdemo->ChannelHandle, PosX, PosY, logo_w, logo_h, logoptr) == 0)
            OutputErrorString("<demo_info> set the logo ok! chan%d\n", SignPort);
        else
        {
            OutputErrorString("<demo_info> set the logo failed!chan%d,w*h: %d*%d,error:0x%x\n",
                              SignPort, logo_w, logo_h, GetLastErrorNum());
        }
    }
    else
    {
        if(SetLogo(dsdemo->ChannelHandle, PosX, PosY, logo_w, logo_h, logoptr) == 0)
            OutputErrorString("<demo_info> set the logo ok! chan%d\n", SignPort);
        else
        {
            OutputErrorString("<demo_info> set the logo failed!chan%d,w*h: %d*%d,error:0x%x\n",
                              SignPort, logo_w, logo_h, GetLastErrorNum());
        }
    }

    SetLogoDisplayMode(dsdemo->ChannelHandle, 0xff, 0x0, 0x0, Translucent, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::GetCardInfo()
{
    int             j;
    unsigned int    boardtype;
    char            serialno[20], serialno_str[20];

    GetBoardInfo(dsdemo->ChannelHandle, &boardtype, serialno);

    for(j = 0; j < 12; j++)
    {
        serialno_str[j] = serialno[j] + 0x30;
    }

    serialno_str[j] = '\0';
    OutputDebugString("card[%d]'s serialno = %s\n", SignPort, serialno_str);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int Dsclass::GetVideoHeight()
{
    if(dsdemo->videostandard == StandardNTSC)
        return 480;
    else
        return 576;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void* Dsclass::videopreview()
{
    while(dsdemo->bPreviewThreadRun)
    {
        dsdemo->bPreviewThreadRun = 2;
        usleep(USLEEPTIME);
        if(!(dsdemo->bPreviewOpen))
            continue;

        if(sem_wait(dsdemo->previewconf.SyncSem) == 0)
        {
            if(app)
            {
                app->lock();
            }

            SDL_LockYUVOverlay(dsdemo->pOverlaySurface);
            memcpy(*((dsdemo->pOverlaySurface)->pixels), dsdemo->previewconf.dataAddr,
                   dsdemo->previewconf.imageSize);
            SDL_UnlockYUVOverlay(dsdemo->pOverlaySurface);
            SDL_DisplayYUVOverlay(dsdemo->pOverlaySurface, &(dsdemo->dstRect));
    
            if(app)
            {
                app->unlock();
            }
        }
        else
        {
            usleep(USLEEPTIME);
        }
    }

    OutputDebugString(">>>>exit thread %d exit<<<<<<< \n", SignPort);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::CreateThread()
{
    pthread_attr_t  attr;

    DsStruct        DsTemp;

    DsTemp.dsclass = this;
    DsTemp.pmf = &Dsclass::videopreview;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    dsdemo->bPreviewThreadRun = 1;
    pthread_create(&(dsdemo->hPreviewThread), &attr, intermediary, &DsTemp);

    while(dsdemo->bPreviewThreadRun == 1)
        usleep(USLEEPTIME);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetPreview(UINT useSyncSem)
{
    dsdemo->previewconf.w = dsdemo->dstRect.w;
    dsdemo->previewconf.h = dsdemo->dstRect.h;

    OutputDebugString("ChannelHandle[%d] %d\n", SignPort, dsdemo->ChannelHandle);   /* debug */

    OutputDebugString("imageSize = %lu, w = %lu, h = %lu, dataAddr = %p, SyncSem = %p \n",
                      (dsdemo->previewconf).imageSize, (dsdemo->previewconf).w,
                      (dsdemo->previewconf).h, (dsdemo->previewconf).dataAddr,
                      (dsdemo->previewconf).SyncSem);

    if(StartVideoPreview(dsdemo->ChannelHandle, &(dsdemo->previewconf), useSyncSem) < 0)
        OutputErrorString("<demo_info>start video preview failed \n");

    OutputDebugString("<demo_info>start video preview successful \n");

    OutputDebugString("<demo_info> Start video preview ok, the sync sem is %p!\n",
                      (dsdemo->previewconf).SyncSem);

    dsdemo->pOverlaySurface = SDL_CreateYUVOverlay(dsdemo->previewconf.w, dsdemo->previewconf.h,
                                                   SDL_YV12_OVERLAY, pOverlayScreen);

    if(dsdemo->pOverlaySurface == NULL)
    {
        OutputErrorString("<demo_info> the SDL overlay surface create failed!\n");
        return;
    }

    dsdemo->bPreviewOpen = 1;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StopPreview()
{
    dsdemo->bPreviewOpen = 0;
    OutputDebugString("The app is locked or not (:%d).......................\n", app->locked());

#ifndef USE_PREVIEW_TIMER
    
    if(app)
    {
        app->unlock();
    }

    sem_post(dsdemo->previewconf.SyncSem);
    dsdemo->bPreviewThreadRun = 3;

    while(dsdemo->bPreviewThreadRun == 3)
        usleep(USLEEPTIME);

    if(app)
    {
        app->lock();
    }

#endif
    if(StopVideoPreview(dsdemo->ChannelHandle) < 0)
        OutputErrorString("<demo_info> StoptVideoPreview(channel %d) failed\n", SignPort);
    if(dsdemo->pOverlaySurface != NULL)
    {
        SDL_FreeYUVOverlay(dsdemo->pOverlaySurface);
        dsdemo->pOverlaySurface = NULL;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::DestoryThread()
{
    if(dsdemo->bPreviewOpen)
        StopPreview();
    dsdemo->bPreviewThreadRun = 0;
    usleep(USLEEPTIME);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::CreateFiles()
{
    char    savepath[10] = ".";
    QString qsFile;
    QFile*  temp;

    pthread_mutex_lock(&(dsdemo->major_start));
    end_record();

    qsFile.sprintf("%s/Mp4P%02dC%02d.mp4", savepath, SignPort, dsdemo->count);
    temp = new QFile(qsFile);
    temp->open(IO_ReadWrite | IO_Truncate);
    dsdemo->record_fd = temp;

    qsFile.sprintf("%s/Mp4P%02dC%02d.dat", savepath, SignPort, dsdemo->count);
    temp = new QFile(qsFile);
    temp->open(IO_ReadWrite | IO_Truncate);
    dsdemo->index_fd = temp;

    if(dsdemo->filehead_flag)
    {
        (dsdemo->record_fd)->writeBlock(dsdemo->file_head, 40);
        dsdemo->nodeinfo.FilePos = 40;
        dsdemo->nodeinfo.FrameNum = 0;
    }

    pthread_mutex_unlock(&(dsdemo->major_start));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::CreateCifQcifFiles()
{
    char    savepath[10] = ".";
    QString qsFile;
    QFile*  temp;

    pthread_mutex_lock(&(dsdemo->minor_start));
    end_subrecord();

    qsFile.sprintf("%s/CifQCifP%02dC%02d.mp4", savepath, SignPort, dsdemo->count);
    temp = new QFile(qsFile);
    temp->open(IO_ReadWrite | IO_Truncate);
    dsdemo->cifqcif_fd = temp;

    if(dsdemo->cifqciffilehead_flag)
        (dsdemo->cifqcif_fd)->writeBlock(dsdemo->cifqciffile_head, 40);

    pthread_mutex_unlock(&(dsdemo->minor_start));
    OutputDebugString("create cifqcif file%02d\n", SignPort);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::CreateImageStreamFiles()
{
    char    savepath[10] = ".";
    QString qsFile;
    QFile*  temp;

    pthread_mutex_lock(&(dsdemo->imagefile_fd_start));
    end_imagestream();

    qsFile.sprintf("%s/imagefileP%02dC%02d.yuv", savepath, SignPort, dsdemo->count);
    temp = new QFile(qsFile);
    temp->open(IO_ReadWrite | IO_Truncate);
    dsdemo->imagefile_fd = temp;
    pthread_mutex_unlock(&(dsdemo->imagefile_fd_start));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StopCapture()
{
    StopVideoCapture(dsdemo->ChannelHandle);
    dsdemo->bit_rate_size = 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StopSubCapture()
{
    StopSubVideoCapture(dsdemo->ChannelHandle);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StartCapture()
{
    int retval;

    dsdemo->bit_rate_size = 0;
    retval = StartVideoCapture(dsdemo->ChannelHandle);
    if(retval < 0)
    {
        dsdemo->record_flag = 0;
        OutputErrorString("<demo_info> StartVideoCapture failed. \n");
        return;
    }

    OutputErrorString("<demo_info> StartVideoCapture Successful. \n");
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StartSubCapture()
{
    int retval;

    retval = StartSubVideoCapture(dsdemo->ChannelHandle);
    OutputDebugString("start port%02d \n", SignPort);
    if(retval < 0)
    {
        dsdemo->cifqcif_flag = 0;
        OutputErrorString("<demo_info> StartVideoCapture failed. \n");
        return;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::end_record()
{
    if(dsdemo->record_fd != NULL)
    {
        delete(dsdemo->record_fd);
        dsdemo->record_fd = NULL;
    }

    if(dsdemo->index_fd != NULL)
    {
        delete(dsdemo->index_fd);
        dsdemo->index_fd = NULL;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::end_subrecord()
{
    if(dsdemo->cifqcif_fd != NULL)
    {
        delete(dsdemo->cifqcif_fd);
        dsdemo->cifqcif_fd = NULL;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::end_imagestream()
{
    if(dsdemo->imagefile_fd != NULL)
    {
        delete(dsdemo->imagefile_fd);
        dsdemo->imagefile_fd = NULL;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::GetImage()
{
    int             retval = 0;
    int             size;           /* ;
                                     * = 384 * 288
                                     * 2;
                                     * */
    char            filename[100];
    int             VideoHeight;
    unsigned char*  imagebuf;
    static int      image_count = 0;

#ifdef YUVFILE
    int             fid;
#endif
    VideoHeight = GetVideoHeight();

    imagebuf = (unsigned char*)malloc(704 * VideoHeight * 2);
    memset(imagebuf, 0x0, 704 * VideoHeight * 2);
    retval = GetOriginalImage(dsdemo->ChannelHandle, imagebuf, &size);
    if(retval < 0)
    {
        OutputErrorString("<demo_info> GetOriginalImage error and return %d\n", retval);
    }
    else
    {
#ifdef BMPFILE
        sprintf(filename, "cap%d%d.bmp", SignPort, image_count);
        OutputDebugString("GetOriginalImage OK size = %d\n", size);

        if(size == 704 * 576 * 2)   /* PAL */
            SaveYUVToBmpFile(filename, imagebuf, 704, 576);
        if(size == 704 * 480 * 2)   /* NTSC */
            SaveYUVToBmpFile(filename, imagebuf, 704, 480);
#endif
#ifdef YUVFILE
        sprintf(filename, "cap%d.yuv", SignPort);
        if((fid = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0)
        {
            perror("get image:");
        }

        OutputDebugString("GetOriginalImage OK size = %d\n", size);
        write(fid, imagebuf, size);
        close(fid);
#endif
    }

    free(imagebuf);
    imagebuf = NULL;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetFrameIQuantVal(int number)
{
    (dsdemo->frameinfo).IQuantVal = number;

    SetDefaultQuant(dsdemo->ChannelHandle, (dsdemo->frameinfo).IQuantVal,
                    (dsdemo->frameinfo).BQuantVal, (dsdemo->frameinfo).PQuantVal);

    SetupSubChannel(dsdemo->ChannelHandle, 1);
    SetDefaultQuant(dsdemo->ChannelHandle, (dsdemo->frameinfo).IQuantVal,
                    (dsdemo->frameinfo).BQuantVal, (dsdemo->frameinfo).PQuantVal);
    SetupSubChannel(dsdemo->ChannelHandle, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetFrameBQuantVal(int number)
{
    (dsdemo->frameinfo).BQuantVal = number;

    SetDefaultQuant(dsdemo->ChannelHandle, (dsdemo->frameinfo).IQuantVal,
                    (dsdemo->frameinfo).BQuantVal, (dsdemo->frameinfo).PQuantVal);

    SetupSubChannel(dsdemo->ChannelHandle, 1);
    SetDefaultQuant(dsdemo->ChannelHandle, (dsdemo->frameinfo).IQuantVal,
                    (dsdemo->frameinfo).BQuantVal, (dsdemo->frameinfo).PQuantVal);
    SetupSubChannel(dsdemo->ChannelHandle, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetFramePQuantVal(int number)
{
    (dsdemo->frameinfo).PQuantVal = number;

    SetDefaultQuant(dsdemo->ChannelHandle, (dsdemo->frameinfo).IQuantVal,
                    (dsdemo->frameinfo).BQuantVal, (dsdemo->frameinfo).PQuantVal);

    SetupSubChannel(dsdemo->ChannelHandle, 1);
    SetDefaultQuant(dsdemo->ChannelHandle, (dsdemo->frameinfo).IQuantVal,
                    (dsdemo->frameinfo).BQuantVal, (dsdemo->frameinfo).PQuantVal);
    SetupSubChannel(dsdemo->ChannelHandle, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetFrameKeyFrameI(int number)
{
    (dsdemo->frameinfo).KeyFrameIntervals = number;

    SetIBPMode(dsdemo->ChannelHandle, (dsdemo->frameinfo).KeyFrameIntervals,
               (dsdemo->frameinfo).BFrames, 0, (dsdemo->frameinfo).FrameRate);

    SetupSubChannel(dsdemo->ChannelHandle, 1);
    SetIBPMode(dsdemo->ChannelHandle, (dsdemo->frameinfo).KeyFrameIntervals,
               (dsdemo->frameinfo).BFrames, 0, (dsdemo->frameinfo).FrameRate);
    SetupSubChannel(dsdemo->ChannelHandle, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetFrameBFrames(int number)
{
    (dsdemo->frameinfo).BFrames = number;

    SetIBPMode(dsdemo->ChannelHandle, (dsdemo->frameinfo).KeyFrameIntervals,
               (dsdemo->frameinfo).BFrames, 0, (dsdemo->frameinfo).FrameRate);

    SetupSubChannel(dsdemo->ChannelHandle, 1);
    SetIBPMode(dsdemo->ChannelHandle, (dsdemo->frameinfo).KeyFrameIntervals,
               (dsdemo->frameinfo).BFrames, 0, (dsdemo->frameinfo).FrameRate);
    SetupSubChannel(dsdemo->ChannelHandle, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetFrameFrameRate(int number)
{
    (dsdemo->frameinfo).FrameRate = number;

    SetIBPMode(dsdemo->ChannelHandle, (dsdemo->frameinfo).KeyFrameIntervals,
               (dsdemo->frameinfo).BFrames, 0, (dsdemo->frameinfo).FrameRate);

    SetupSubChannel(dsdemo->ChannelHandle, 1);
    SetIBPMode(dsdemo->ChannelHandle, (dsdemo->frameinfo).KeyFrameIntervals,
               (dsdemo->frameinfo).BFrames, 0, (dsdemo->frameinfo).FrameRate);
    SetupSubChannel(dsdemo->ChannelHandle, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetOsdTranslucent(unsigned short Translucent)
{
    (dsdemo->osdinfo).Translucent = Translucent;
    SetOsddemo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetOsdBrightness(int Brightness)
{
    (dsdemo->osdinfo).Brightness = Brightness;
    SetOsddemo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetOsdPosX(unsigned short PosX)
{
    (dsdemo->osdinfo).PosX = PosX;
    SetOsddemo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetOsdPosY(unsigned short PosY)
{
    (dsdemo->osdinfo).PosY = PosY;
    SetOsddemo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetLogoTranslucent(unsigned short Translucent)
{
    (dsdemo->logoinfo).Translucent = Translucent;
    SetDsLogo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetLogoPosX(unsigned short PosX)
{
    (dsdemo->logoinfo).PosX = PosX;
    SetDsLogo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetLogoPosY(unsigned short PosY)
{
    (dsdemo->logoinfo).PosY = PosY;
    SetDsLogo();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetBrightScale(const int brightness)
{
    (dsdemo->v_info).Brightness = brightness;
    SetVideoPara(dsdemo->ChannelHandle, (dsdemo->v_info).Brightness, (dsdemo->v_info).Contrast,
                 (dsdemo->v_info).Saturation, (dsdemo->v_info).Hue);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetContrastScale(const int contrast)
{
    (dsdemo->v_info).Contrast = contrast;
    SetVideoPara(dsdemo->ChannelHandle, (dsdemo->v_info).Brightness, (dsdemo->v_info).Contrast,
                 (dsdemo->v_info).Saturation, (dsdemo->v_info).Hue);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetSaturationScale(const int saturation)
{
    (dsdemo->v_info).Saturation = saturation;
    SetVideoPara(dsdemo->ChannelHandle, (dsdemo->v_info).Brightness, (dsdemo->v_info).Contrast,
                 (dsdemo->v_info).Saturation, (dsdemo->v_info).Hue);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetHueScale(const int hue)
{
    (dsdemo->v_info).Hue = hue;
    SetVideoPara(dsdemo->ChannelHandle, (dsdemo->v_info).Brightness, (dsdemo->v_info).Contrast,
                 (dsdemo->v_info).Saturation, (dsdemo->v_info).Hue);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetMask()
{
    int     i;
    RECT*   tempmask = dsdemo->maskRect;

    for(i = 0; i < 5; i++)
    {
        if(!(tempmask[i].RectTop || tempmask[i].RectBottom || tempmask[i].RectLeft ||
           tempmask[i].RectRight))
            break;
    }

    SetupMask(dsdemo->ChannelHandle, dsdemo->maskRect, i);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
Dsclass::~Dsclass()
{
#ifdef LOGO
    if(logoptr)
        free(logoptr);
#endif
    sem_destroy(dsdemo->previewconf.SyncSem);

    if(dsdemo->maskRect)
    {
        free(dsdemo->maskRect);
        dsdemo->maskRect = NULL;
    }

    pthread_mutex_destroy(&(dsdemo->major_start));
    pthread_mutex_destroy(&(dsdemo->minor_start));
    pthread_mutex_destroy(&(dsdemo->imagefile_fd_start));
    if(dsdemo)
    {
        free(dsdemo);
        dsdemo = NULL;
    }
}
