#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include <semaphore.h>
#include "DataType.h"
#include "ds40xxsdk.h"
#include "tmserverpu.h"

unsigned short  format1[] = { 16, 19, 'O', 'f', 'f', 'i', 'c', 'e', '\0' };
unsigned short  format2[] = { 8, 3, _OSD_YEAR4, ':', _OSD_MONTH2, ':', _OSD_HOUR24,
                              ':', _OSD_MINUTE, ':', _OSD_SECOND, '\0' };

int             gcheckpass(char* usename, int namelen, char* password, int passlen);
int             gcheckip(int iChannel, char* sIP);
void            gmessage(char* buf, int iLen);
void            callback(int channelNum, char* buf, int frametype, int length);

int             audio_port = 0;
static int      done = 0;

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void poked(int sig)
{
    done = 1;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int main()
{
 	printf("Main function\n");
    int                     channelCount;
    int                     i, retVal, chan;
    int                     channelHandle[16];
    SERVER_VIDEOINFO        videoinfo;
    RECT                    winRect;
    STREAM_READ_CALLBACK    pStreamRead;
    pStreamRead = (STREAM_READ_CALLBACK) callback;

    int     tei = 0;
    char    data[4];
    
    printf("Setting up signal handlers\n");

    signal(SIGHUP, poked);
    signal(SIGINT, poked);
    signal(SIGTERM, poked);
	
    printf("Initiating DSPs\n");
    retVal = InitDSPs();
    if(retVal == 0)
    {   /* fixed at 2005.1.13 */
        fprintf(stderr, "Do InitDSPs Error(%08x)\n", GetLastErrorNum());
        return 0;
    }

    fprintf(stderr, "Success!----InitDSPs()\n");

    channelCount = GetTotalChannels();
    fprintf(stderr, "channelcount = %d\n", channelCount);

    videoinfo.m_channum = channelCount;
    MP4_ServerSetNetPort(5050, 6050);

    if(MP4_ServerStart(&videoinfo) < 0)
    {
        fprintf(stderr, "Error Number is %x\n", MP4_ServerGetLastErrorNum());
        goto EXIT;
    }

    MP4_ServerSetBufNum(1, 60);

    printf("videoinfo at 0x%x, m_channum at 0x%x\n", &videoinfo, &videoinfo.m_channum);

    MP4_ServerCheckPassword(gcheckpass);
    MP4_ServerCheckIP(gcheckip);
    MP4_ServerSetMessage(gmessage);

    /* open channel */
    for(i = 0; i < channelCount; i++)
    {
        channelHandle[i] = ChannelOpen(i, callback);
    }

#if 0
    sleep(1);
    for(chan = 0; chan < channelCount; chan++)
    {
        usleep(40);
        if(SetOsdDisplayMode(channelHandle[chan], 0xff, 1, 0x0000, format1, format2) < 0)
        {
            fprintf(stderr, "<demo_info> set osd failed!\n");
            return -1;
        }

        if(SetOsd(channelHandle[chan], 1) < 0)
        {
            fprintf(stderr, "<demo_info> start osd failed!\n");
            return -1;
        }
    }   /* for(chan=0;
         * cahn<channelCount;
         * chan++) */

    /*
     * SetEncodePictureFormat(channelHandle[1],ENC_QCIF_FORMAT);
     */
    sleep(1);
    if(SetAudioPreview(channelHandle[audio_port], 1) < 0)
    {
        fprintf(stderr, "<demo_info> set audio failed!\n");
        return -1;
    }
#endif

    /* start capture */
    for(i = 0; i < channelCount; i++)
    {
        if(StartVideoCapture(channelHandle[i]) < 0)
        {
            fprintf(stderr, "<demo_info>start video capture failed!error num:0x%x\n",
                    GetLastErrorNum());
            return -1;
        }
    }

    fprintf(stderr, "<demo_info> Start video capture ok!\n");
    while(!done)
    {
        sleep(1);
    }

    fprintf(stderr, "beging stop!\n");
    MP4_ServerStop();

    /* stop preview */
    fprintf(stderr, "stop video!\n");
    for(i = 0; i < channelCount; i++)
    {
        StopVideoPreview(channelHandle[i]);
    }

    fprintf(stderr, "stop audio!\n");
    SetAudioPreview(channelHandle[audio_port], 0);
    fprintf(stderr, "stop motion detection!\n");

    /* stop motion detect */
    for(i = 0; i < channelCount; i++)
    {
        StopMotionDetection(channelHandle[i]);
    }

    fprintf(stderr, "stop video capture!\n");

    /* stop capture */
    for(i = 0; i < channelCount; i++)
    {
        StopVideoCapture(channelHandle[i]);
    }

    fprintf(stderr, "close channel!\n");

    /* close channel */
    for(i = 0; i < channelCount; i++)
    {
        ChannelClose(channelHandle[i]);
    }

EXIT:
    fprintf(stderr, "deinit dsp!\n");
    retVal = DeInitDSPs();
    if(retVal < 0)
    {
        fprintf(stderr, "Do DeInitDSPs Error(%08x)\n", GetLastErrorNum());
        return 0;
    }

    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void callback(int channelNum, char* buf, int frametype, int length)
{
    int retval;

    switch(frametype)
    {
        case PktSysHeader:
        case PktIFrames:
        case PktPFrames:
        case PktBBPFrames:
        case PktAudioFrames:
            {
                if(channelNum == 0)
                    fprintf(stderr, ".");
                retval = MP4_ServerWriteData(channelNum, (char*)buf, length, frametype, 0);
    #if 0
                if(retval == 0)
                {
                    if(0x0001 == frametype)
                        fprintf(stderr, "*%d", channelNum);
                }
                else
                {
                    if(channelNum == 0)
                        fprintf(stderr, ".");
                }
    #endif
                break;
            }

        case PktMotionDetection:
            break;

        default:
            break;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int gcheckpass(char* usename, int namelen, char* password, int passlen)
{
    fprintf(stderr, "i am in checkpass\n");
    fprintf(stderr, "usename=%s\n,pass=%s\n", usename, password);
    fprintf(stderr, "lenname=%d,passlen=%d\n", namelen, passlen);
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int gcheckip(int iChannel, char* sIP)
{
    fprintf(stderr, "i am in checkip\n");
    fprintf(stderr, "ichannel=%d,ip=%s\n", iChannel, sIP);

    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void gmessage(char* buf, int iLen)
{
    fprintf(stderr, "i am in messagecallback\n");
    fprintf(stderr, "len=%d,message=%s\n", iLen, buf);
}
