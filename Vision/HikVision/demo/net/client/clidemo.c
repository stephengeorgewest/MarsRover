
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "tmclientpu.h"

void    gmessage(int imark, char command, char* buf, int iLen);
void    ReadDataCallBack(int StockHandle, char* pPacketBuffer, int nPacketSize);

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

int main(int argc, char ** argv)
{
    int                 i;
    int                 num;
    char                command[307];
    int                 retval, clienthandle, clienthandle2, clienthandle3, clienthandle4;
    CLIENT_VIDEOINFO    Clientinfo, Clientinfo2, Clientinfo3, Clientinfo4;
    CLIENTSHOWRECT      clientrect;
    int                 ichannelnum, iconnecttype;
    int                 iPlayTime;
    char                testmessage[128];
    char                mrserverip[256];
    int                 mrserverport;

    if(argc < 2)
    {
        printf("Usage:%s <ServerIP>[ChannelNum][ConnectType(0-tcp,1-udp,2-multicast, 3-audiodetach)][Relay Server IP]\n",
               argv[0]);
        return 0;
    }

    if(argc >= 3)
    {
        ichannelnum = atoi(argv[2]);
    }
    else
    {
        ichannelnum = 0;
    }

    if(argc >= 4)
    {
        iconnecttype = atoi(argv[3]);
    }
    else
    {
        iconnecttype = 0;
        fprintf(stderr, "Now with TCP connect!\n");
    }

    fprintf(stderr, "server:%s\n", argv[1]);
    fprintf(stderr, "argc=%d,iPlaytime=%d,ichannelnum=%d,iconnecttype=%d\n", argc, iPlayTime,
            ichannelnum, iconnecttype);

    MP4_ClientSetNetPort(5050, 6050);

    fprintf(stderr, "\n<<Type 'q' to exit!>>\n");

    clientrect.uWidth = 352;
    clientrect.uHeight = 288;

    MP4_ClientStartup(clientrect, gmessage);    /* NULL);
                                                 * */

    retval = MP4_ClientGetServerChanNum(argv[1]);
    printf("server:%s has %d channels\n", argv[1], retval);

    /* yxq test at 2004.7.20 */
    sprintf(testmessage, "this is a test message!");

    /* yxq test at 2004.8.19 */
#if 0
    if(argc >= 5)
    {
        strncpy(mrserverip, argv[4], 256);

        /*
         * fprintf(stderr,"<demo_info> want to set the mr server %s!to command relay\n",mrserverip);
         */
        MP4_ClientSetMR(mrserverip, NULL, 8050);
    }

    MP4_ClientCommandToServer(argv[1], testmessage, 128);
    for(num = 0; num < 10; num++)
    {
        sprintf(testmessage, "the times%d test message!\n", num);
        if(argc >= 5)
        {
            strncpy(mrserverip, argv[4], 256);

            /*
             * fprintf(stderr,"<demo_info> want to set the mr server %s!to command relay\n",mrserverip);
             */
            MP4_ClientSetMR(mrserverip, NULL, 8050);
        }

        MP4_ClientCommandToServer(argv[1], testmessage, 128);
    }
#endif
    usleep(100);
    Clientinfo.m_bRight = 0;                    /* yxq test at 2004.11.10 */

    /*
     * Clientinfo.m_bRight = 1;
     * yxq test at 2004.11.10
     */
    Clientinfo.m_bRemoteChannel = ichannelnum;
    Clientinfo.m_sUserName = "username1234567890";
    Clientinfo.m_sUserPassword = "userpassword1234567890";
    Clientinfo.m_bUserCheck = 1;
    switch(iconnecttype)
    {
        case 0:
            Clientinfo.m_bSendMode = PTOPTCPMODE;
            break;

        case 1:
            Clientinfo.m_bSendMode = PTOPUDPMODE;
            break;

        case 2:
            Clientinfo.m_bSendMode = MULTIMODE;
            break;

        case 3:
            Clientinfo.m_bSendMode = ONLYAUDIOMODE;
            break;

        default:
            Clientinfo.m_bSendMode = PTOPUDPMODE;
            break;
    }

    /*
     * Clientinfo.m_bSendMode = PTOPTCPMODE;
     * *Clientinfo.m_bSendMode = PTOPUDPMODE;
     * *Clientinfo.m_bSendMode = MULTIMODE;
     */
    Clientinfo.m_sIPAddress = argv[1];
    Clientinfo.subshow_x = 0;
    Clientinfo.subshow_y = 0;
    Clientinfo.subshow_uWidth = 352;
    Clientinfo.subshow_uHeight = 288;

    /* yxq test at 2004.8.16 */
    if(argc >= 5)
    {
        strncpy(mrserverip, argv[4], 256);
        fprintf(stderr, "<demo_info> want to set the mr server %s!\n", mrserverip);
        MP4_ClientSetMR(mrserverip, argv[5], 8050);
    }

    /*
     * clienthandle = MP4_ClientStart(&Clientinfo, NULL);
     */
    clienthandle = MP4_ClientStart(&Clientinfo, ReadDataCallBack);

    if(clienthandle == -1)
    {
        fprintf(stderr, "clientstart error:%x\n", MP4_ClientGetLastErrorNum());
        MP4_ClientCleanup();
        return 0;
    }

    /*
     * MP4_ClientSetDecodeQuality(clienthandle, 1);
     * ;
     * yxq test at 8.11 to connect the window's server demo channel 0
     */
#if 0
    if(Clientinfo.m_bSendMode == MULTIMODE)
        MP4_ClientSetMulticast(clienthandle, "226.0.0.10", 6000);
#endif
    MP4_ClientAudioStart(clienthandle);
    MP4_ClientAudioVolume(clienthandle, 100);
#if 0
    if(MP4_ClientStartCaptureFile(clienthandle, "yxqtest.mp4") < 0)
        fprintf(stderr, "<demo_info> start capture file failed!0x%x\n", MP4_ClientGetLastErrorNum());
#endif
    printf("====================\n");
#if 0
    /* test the ClientShut() */
    sleep(5);
    MP4_ClientShut("192.0.0.128", 0);
    printf("shut ok!\n");
#endif
    do
    {
        usleep(1000);
    } while(getc(stdin) != 'q');

    MP4_ClientAudioStop();

    MP4_ClientStopCapture(clienthandle);
    printf("-----close\n");
    retval = MP4_ClientStop(clienthandle);
    if(retval == -1)
        fprintf(stderr, "clientstop error\n");

    MP4_ClientCleanup();

    return 1;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void gmessage(int imark, char command, char* buf, int iLen)
{
    static int  num = 0;
    fprintf(stderr, "imark = %d\n", imark);
    if(imark == 1)
    {
        num++;
        fprintf(stderr, "The No %x data is coming!buf=%s\n", num, buf);
    }
    else
    {
        fprintf(stderr, "command = %d\n", command);
    }

    return;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void ReadDataCallBack(int StockHandle, char* pPacketBuffer, int nPacketSize)
{
}
