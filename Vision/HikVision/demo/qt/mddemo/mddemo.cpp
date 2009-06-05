#include "mddemo.h"

#include <qapplication.h>
extern QApplication* app;

#define USLEEPTIME (20 * 1000)

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

QPlayOverThread::QPlayOverThread(MDDEMO* demo) :
    mdOver(demo)
{
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QPlayOverThread::run()
{
    OutputDebugString("<demo_info> .............. in play over for port %d!\n", mdOver->SignPort);

    sem_wait(&(mdOver->FileEndSem));
    mdOver->bDisplayOpen = 0;   /* to stop the display */
    sem_post(&(mdOver->displaySem));

    pthread_mutex_lock(&(mdOver->DisplayPrepare));
    pthread_mutex_unlock(&(mdOver->DisplayPrepare));

    HW_Stop(mdOver->ChannelHandle);
    HW_CloseFile(mdOver->ChannelHandle);
    mdOver->bDisplayOpen = 0;
    mdOver->playType = 0;

    OutputDebugString("<demo_info> in play over thread, close file port: %d!\n", mdOver->SignPort);

    /* here should add some code for paint the port map */
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
QDisplayThread::QDisplayThread(MDDEMO* demo, SDL_Surface* display)
{
    mdPlay = demo;
    pOverlayScreen = display;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void QDisplayThread::run()
{
    UINT                useSyncSem = 1;
    unsigned long int   WSize = 0;
    unsigned long int   HSize = 0;
    int                 i;

    while(1)
    {
        OutputDebugString("before sem_wait beginSem in Displaythread \n");
        sem_wait(&(mdPlay->beginSem));

        if(!(mdPlay->bDisplayThreadRun))
            break;

        pthread_mutex_lock(&(mdPlay->DisplayPrepare));
        memset(&(mdPlay->displayconf), 0x0, sizeof(PREVIEWCONFIG));

        /* 以下赋值有没有必要 */
        (mdPlay->displayconf).w = (mdPlay->dstRect).w;
        (mdPlay->displayconf).h = (mdPlay->dstRect).h;
        (mdPlay->displayconf).SyncSem = &(mdPlay->displaySem);

        /*
         * OutputDebugString("<demo_info> .............. start display for port %d!\n", mdPlay->SignPort);
         * *OutputDebugString("<demo_info>w,h,ChangeSem %ld,%ld,%p\n",
         * (mdPlay->displayconf).w , (mdPlay->displayconf).h, (mdPlay->displayconf).ChangeSem);
         */
        if(HW_StartDecVgaDisplay(mdPlay->ChannelHandle, &(mdPlay->displayconf), useSyncSem) < 0)
        {
            OutputErrorString("<demo_info> start video display (port %d) failed, for 0x%x\n",
                              mdPlay->SignPort, GetLastErrorNum());
            goto DISPLAYEXIT;
        }

        mdPlay->bDisplayOpen = 1;

        if(!(mdPlay->OverLayPrepare))
        {
            mdPlay->pOverLay[0] = SDL_CreateYUVOverlay(352 / 2, 288 / 2, SDL_YV12_OVERLAY,
                                                       pOverlayScreen);
            mdPlay->pOverLay[1] = SDL_CreateYUVOverlay(352, 288, SDL_YV12_OVERLAY, pOverlayScreen);
            mdPlay->pOverLay[2] = SDL_CreateYUVOverlay(352 * 2, 288, SDL_YV12_OVERLAY,
                                                       pOverlayScreen);
            mdPlay->pOverLay[3] = SDL_CreateYUVOverlay(352 * 3 / 2, 288 * 4 / 3, SDL_YV12_OVERLAY,
                                                       pOverlayScreen);
            mdPlay->pOverLay[4] = SDL_CreateYUVOverlay(352 * 2, 288 * 2, SDL_YV12_OVERLAY,
                                                       pOverlayScreen);
            if(!(mdPlay->pOverLay[0] && mdPlay->pOverLay[1] && mdPlay->pOverLay[2] &&
               mdPlay->pOverLay[3] && mdPlay->pOverLay[4]))
            {
                OutputErrorString("port %d the SDL overlay surface create failed!\n",
                                  mdPlay->SignPort);
                goto DISPLAYEXIT;
            }

            mdPlay->OverLayPrepare = 1;
        }

        while(mdPlay->bDisplayOpen)
        {
            usleep(USLEEPTIME);
            if(sem_wait(&(mdPlay->displaySem)) == 0)
            /*
             * everytime mdPlay->displaySem discreases, for it has been added in function ;
             * HW_StartDecVgaDisplay, so when the data coming , it will be add to 1
             */
            {
                if((WSize != (mdPlay->displayconf).w) || (HSize != (mdPlay->displayconf).h))
                {
                    WSize = (mdPlay->displayconf).w;
                    HSize = (mdPlay->displayconf).h;

                    if(!(WSize > 0 && HSize > 0))
                        OutputErrorString("WSize and HSize stand error\n");

                    for(i = 0; i < 5; i++)
                    {
                        if(((int)WSize == (mdPlay->pOverLay[i])->w) &&
                           ((int)HSize == (mdPlay->pOverLay[i])->h))
                        {
                            mdPlay->pOverlaySurface = mdPlay->pOverLay[i];
                            OutputDebugString("Port/OverlayPort/WSize/HSize = %d, %d, %ld, %ld\n",
                                              mdPlay->SignPort, i, WSize, HSize);
                            break;
                        }
                    }
                }

                if(app)
                {
                    app->lock();
                }

                SDL_LockYUVOverlay(mdPlay->pOverlaySurface);
                memcpy(*((mdPlay->pOverlaySurface)->pixels), (mdPlay->displayconf).dataAddr,
                       (mdPlay->displayconf).imageSize);
                SDL_UnlockYUVOverlay(mdPlay->pOverlaySurface);
                SDL_DisplayYUVOverlay(mdPlay->pOverlaySurface, &(mdPlay->dstRect));

                OutputDebugString("222222222222222222222222222\n");

                if(app)
                {
                    app->unlock();
                }
            }
        }

        OutputDebugString("after sem_wait displaySem in Displaythread here ChannelNum%2d \n",
                          mdPlay->ChannelHandle);

        if(HW_StopDecChanVgaDisplay(mdPlay->ChannelHandle) < 0) /* here mdPlay->displaySem discreases */
        {
            OutputErrorString("<demo_info> stop the video display failed!error: 0x%x!\n",
                              GetLastErrorNum());
            goto DISPLAYEXIT;
        }

        WSize = 0;  /* 因为每次重新display画面的时候要关闭pOverlaySurface, 所以WSize 和 HSize 必须重新置为0 */
        HSize = 0;
        OutputDebugString("<demo_info>........................... stop display for port%d!\n",
                          mdPlay->SignPort);
        mdPlay->pOverlaySurface = NULL;
        pthread_mutex_unlock(&(mdPlay->DisplayPrepare));
    }

DISPLAYEXIT:
    OutputDebugString("<demo_info> exit the video display thread for chan%d!\n", mdPlay->SignPort);
    for(i = 0; i < 4; i++)
    {
        if(mdPlay->pOverLay[i])
            SDL_FreeYUVOverlay(mdPlay->pOverLay[i]);
        mdPlay->pOverLay[i] = NULL;
        mdPlay->OverLayPrepare = 0;
    }

    /*
     * sem_destroy(&(mdPlay->displaySem));
     */
    return;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
Dsclass::Dsclass(int port, int ChannelHandle, SDL_Surface* Screen)
{
    pOverlayScreen = Screen;

    mddemo = (MDDEMO*)malloc(sizeof(MDDEMO));
    memset(mddemo, 0x0, sizeof(MDDEMO));

    mddemo->SignPort = port;
    mddemo->ChannelHandle = ChannelHandle;
    mddemo->clienthandle = -1;

    pthread_mutex_init(&(mddemo->DisplayPrepare), NULL);
    sem_init(&(mddemo->beginSem), 0, 0);
    sem_init(&(mddemo->displaySem), 0, 0);
    sem_init(&(mddemo->FileEndSem), 0, 0);
    sem_init(&(mddemo->changesem), 0, 0);
    (mddemo->displayconf).SyncSem = &(mddemo->displaySem);
    (mddemo->displayconf).ChangeSem = &(mddemo->changesem);

    OutputDebugString("******port, SyncSem, ChangeSem %d, %p,%p\n", port,
                      (mddemo->displayconf).SyncSem, (mddemo->displayconf).ChangeSem);
    mddemo->serverchan = 0;
    sprintf(mddemo->serverip, "%d.%d.%d.%d", address[0], address[1], address[2], address[3]);

    mddemo->playThread = new QPlayOverThread(mddemo);
    mddemo->displayThread = new QDisplayThread(mddemo, pOverlayScreen);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
bool Dsclass::IsPlayOver()
{
    return !(mddemo->playThread->running());
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::startDisplay()
{
    if(!(mddemo->bDisplayOpen))
    {
        mddemo->bDisplayOpen = 1;
        sem_post(&(mddemo->beginSem));
        usleep(USLEEPTIME);
        OutputDebugString("Start the showing port %d\n", mddemo->SignPort);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::stopDisplay()
{
    if(mddemo->bDisplayOpen)
    {
        mddemo->bDisplayOpen = 0;

        if(app)
        {
            app->unlock();
        }

        sem_post(&(mddemo->displaySem));

        pthread_mutex_lock(&(mddemo->DisplayPrepare));
        pthread_mutex_unlock(&(mddemo->DisplayPrepare));

        OutputDebugString("Close the showing port %d\n", mddemo->SignPort);

        if(app)
        {
            app->lock();
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::startDisplayThread()
{
    mddemo->bDisplayThreadRun = 1;
    mddemo->displayThread->start();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::stopDisplayThread()
{
    if(mddemo->bDisplayThreadRun)
    {
        stopDisplay();
        mddemo->bDisplayThreadRun = 0;
        sem_post(&(mddemo->beginSem));
        usleep(USLEEPTIME);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::stopPlayOverThread()
{
    sem_post(&(mddemo->FileEndSem));
    usleep(USLEEPTIME);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
Dsclass::~Dsclass()
{
    stopDisplayThread();

    if(mddemo->playType == DECODEFILE)
        stopPlayOverThread();

    OutputDebugString("in port %d, before playThread wait\n", mddemo->SignPort);

    mddemo->playThread->wait();
    sem_destroy(&(mddemo->FileEndSem));

    delete mddemo->playThread;

    OutputDebugString("in port %d, before displayThread wait\n", mddemo->SignPort);

    mddemo->displayThread->wait();
    delete mddemo->displayThread;

    pthread_mutex_destroy(&(mddemo->DisplayPrepare));
    sem_destroy(&(mddemo->beginSem));
    sem_destroy(&(mddemo->displaySem));
    sem_destroy(&(mddemo->changesem));

    free(mddemo);
    mddemo = NULL;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StopPlay()
{
    stopDisplay();

    if(mddemo->playType == DECODEFILE)
    {
        stopPlayOverThread();
        mddemo->playThread->wait();

        /*
         * mddemo->bDisplayOpen = 0;
         */
        mddemo->playType = NODECODE;

        HW_Stop(mddemo->ChannelHandle);
        HW_CloseFile(mddemo->ChannelHandle);
    }
    else if(mddemo->playType == DECODESTREAM)
    {
        /*
         * mddemo->bDisplayOpen = 0;
         */
        mddemo->playType = NODECODE;

        HW_Stop(mddemo->ChannelHandle);
        HW_CloseStream(mddemo->ChannelHandle);

        MP4_ClientStopCapture(mddemo->clienthandle);
        OutputDebugString("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
        if(MP4_ClientStop(mddemo->clienthandle) == -1)
            OutputErrorString("<demo_info> Client Stop error 0x%x\n", MP4_ClientGetLastErrorNum());
        OutputDebugString("In function StopPlay, now the ChannelHandle is %d\n",
                          mddemo->ChannelHandle);
        mddemo->clienthandle = -1;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StartFilePlay()
{
    char*   filename = mddemo->filename;
    OutputDebugString(filename);
    if(HW_OpenFile(mddemo->ChannelHandle, filename) < 0)
    {
        OutputErrorString("<demo_info> the md card open file failed, for 0x%x!\n", GetLastErrorNum());
        return;
    }

    OutputErrorString("<demo_info> the md channel%2d  open file ok!\n", mddemo->ChannelHandle);

    if(HW_Play(mddemo->ChannelHandle) < 0)
        OutputErrorString("<demo_info> play the file failed! error code 0x%x!\n", GetLastErrorNum());
    else
    {
        OutputDebugString("<demo_info> play the file ok,the file end sem is 0x%px!\n",
                          &(mddemo->FileEndSem));
        HW_SetFileEndMsg(mddemo->ChannelHandle, &(mddemo->FileEndSem));
        mddemo->playThread->start();
        sem_post(&(mddemo->beginSem));
    }

    mddemo->datasum = 0;
    mddemo->playType = DECODEFILE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::StartStreamPlay(char* pFileHead, int nHeadSize)
{
    if(HW_OpenStream(mddemo->ChannelHandle, pFileHead, nHeadSize) < 0)
    {
        OutputErrorString("<demo_info> open stream failed for decode!\n");
        return;
    }

    OutputDebugString("<demo_info> open stream successful for decode!\n");

    if(HW_Play(mddemo->ChannelHandle) < 0)
        OutputErrorString("<demo_info>in decode stream, play failed!\n");

    sem_post(&(mddemo->beginSem));

    mddemo->playType = DECODESTREAM;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Dsclass::SetdstRect(const int chan)
{
    int SignPort = mddemo->SignPort;

    if(chan < 2)
    {
        OutputErrorString("<demo_info> chan setting error \n");
        return;
    }
    (mddemo->dstRect).x = (SignPort % chan) * (WIN_W / chan);
    (mddemo->dstRect).y = (SignPort / chan) * (WIN_H / chan);
    (mddemo->dstRect).w = WIN_W / chan;
    (mddemo->dstRect).h = WIN_H / chan;
}
