/*
 * File mddemo.c Description: This is a demo for the using for DS400xMD board
 * Author: write by yxq at 2005.7.25 (C) 2004-2010 Hangzhou Hikvision Digital
 * Technology Co. Ltd. Last update: 2005-7-25
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <gtk/gtktext.h>
#include <gdk/gdk.h>
#include <SDL/SDL.h>

#include <sys/vfs.h>                /* statfs() */
#include <time.h>
#include <dirent.h>                 /* opendir() */
#include <unistd.h>                 /* unlink(), getcwd() */
#include <sys/stat.h>               /* stat() */
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>

#include "DataType.h"
#include "ds40xxsdk.h"
#include <pthread.h>
#include <assert.h>
#include "tmclientpu.h"

/* define RAWSTREAM */
GtkWidget*              md_window;
GtkWidget*              w_full_area;
video_win               full_win;
SDL_Rect                full_rect;

GdkColormap*            camp;
GdkGC*                  gc1, *gc2;  /* gc1 for title window, gc2 for video window */

GtkWidget*              title;
GtkWidget*              w_buttons[3];
char*                   button_str[3] = { " Start ", "  Stop ", "  Exit  " };
GtkWidget*              w_decodefile_flag;
GtkWidget*              w_decodestream;
GtkWidget*              w_videoout;

GtkWidget*              w_status[3];
GtkTextBuffer*          w_status_buffer[3];

char*                   status_str[3] = { "Data", "Time", "Free space" };

GtkWidget*              w_info_scale[4];
char*                   info_str[4] = { "Brightness", "Contrast", "Saturation", " Hue " };
GtkObject*              w_adj[4];

struct statfs           bbuf;
static int              selectport = 0;

/* timer */
GTimer*                 count_timer = NULL;
gdouble                 elapse_time = 0;
unsigned long long      bit_rate_time = 0;

int                     processtimer = 0;
time_t                  now;
struct tm*              tm;
char                    timebuf[128];
char                    savepath[32] = ".";

/* frame infomations add at 2002.4.11 */
GtkWidget*              w_decode_framerate;
GtkTextBuffer*          w_decode_framerate_buffer;
GtkWidget*              w_totalframe;
GtkTextBuffer*          w_totalframe_buffer;
GtkWidget*              w_channel;
GtkTextBuffer*          w_channel_buffer;

GtkWidget*              w_bit_rate;
GtkTextBuffer*          w_bit_rate_buffer;

char                    starttime[100];

static SDL_Surface*     pOverlayScreen;
video_win               v_win[MAX_PORT];
SDL_Rect                dstRect[MAX_PORT];

int                     bfileopenok = 0;
sem_t                   FileEndSem[MAX_PORT];
void*                   PlayOver(void* arg);
pthread_t               hDecodeOver[MAX_PORT];
PREVIEWCONFIG           displayconf[MAX_PORT];

int                     ChannelHandle[MAX_PORT];
long                    TotalChannel = 0;
pthread_t               hDisplayThread[MAX_PORT];
sem_t                   beginSem[MAX_PORT];
void*                   videodisplay(void* arg);
SDL_Overlay*            pOverlaySurface[MAX_PORT];
int                     bDisplayOpen[MAX_PORT];
int                     bDisplayClose[MAX_PORT];
int                     displaythreadrun[MAX_PORT];
int                     audiopreviewport = 0;
static int              clickflag = 1;
SDL_Rect                tmpRect;
static int              clickport = -1;
sem_t                   displaySem[MAX_PORT];
static int              streamopenok = 0;
static int              clienthandle[MAX_PORT];
static int              displayChannelCount = 0;

static unsigned long long    datasum[MAX_PORT]; /* to prevent overflow */

char                    serverchan[MAX_PORT][16];
char                    serverip[MAX_PORT][64];
char*                   defaultserverip = "192.0.0.126";
GtkWidget*              w_set_serverip;
GtkWidget*              w_set_serverchan;

#define WIN_W   800
#define WIN_H   700

void                    demo_exit();
void                    ReadDataCallBack(int StockHandle, char* pPacketBuffer, int nPacketSize);
int                     stopstreamdecode();

#ifdef RAWSTREAM
int                     imagefile_fd = 0;
unsigned char           imageBuf[704 * 576 * 2];

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void ImageStreamCallback(unsigned int channelNumber, void* context)
{
    /*
     * fprintf(stderr,"Enter the image stream call back for chan %d!\n", channelNumber);
     */
    if(imagefile_fd)
        write(imagefile_fd, imageBuf, 704 * 576 * 3 / 2);
}
#endif

/*
 =======================================================================================================================
 * buttons operation functions
 =======================================================================================================================
 */
static void hik_set_sensitive(char* name, int sensitive)
{
    GtkWidget*  button;
    button = GTK_WIDGET(gtk_object_get_data(GTK_OBJECT(md_window), name));
    gtk_widget_set_sensitive(button, sensitive);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void set_serverip_callback(GtkWidget* widget)
{
    const char*     str;
    str = gtk_entry_get_text(GTK_ENTRY(w_set_serverip));
    strcpy(serverip[selectport], str);
    fprintf(stderr, "<demo_info> the chan %d have set sever ip %s!\n", selectport,
            serverip[selectport]);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void set_serverchan_callback(GtkWidget* widget)
{
    const char*     str;
    str = gtk_entry_get_text(GTK_ENTRY(w_set_serverchan));
    strcpy(serverchan[selectport], str);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void demo_exit()
{
    int i;
    if(processtimer)
    {
        gtk_timeout_remove(processtimer);
        processtimer = 0;
    }

    if(count_timer)
    {
        g_timer_destroy(count_timer);
        count_timer = NULL;
    }

    for(i = 0; i < TotalChannel; i++)
    {
        if(bDisplayOpen[i])
        {
            bDisplayOpen[i] = 0;
            sem_post(&displaySem[i]);
            usleep(100);
            displaythreadrun[i] = 0;
            sem_post(&beginSem[i]);
            usleep(100);
        }
    }

    if(bfileopenok)
    {
        for(i = 0; i < TotalChannel; i++)
        {
            sem_post(&FileEndSem[i]);
        }

        bfileopenok = 0;
    }

    if(streamopenok)
        stopstreamdecode();

    for(i = 0; i < TotalChannel; i++)
    {
        HW_ChannelClose(ChannelHandle[i]);
        sem_destroy(&FileEndSem[i]);
        sem_destroy(&beginSem[i]);
    }

    HW_ReleaseDecDevice();
#ifdef RAWSTREAM
    if(imagefile_fd)
        close(imagefile_fd);
#endif
    gdk_flush();
    SDL_Quit();
    gtk_main_quit();
    usleep(2000);
    printf("End ......\n");
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
static void dialog_cleanup(GtkWidget* dialog)
{
    gtk_grab_remove(dialog);
    gtk_widget_destroy(dialog);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
static void file_open_cancel(GtkWidget* widget, GtkWidget* dialog)
{
    assert(dialog);
    bfileopenok = 0;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_decodefile_flag), FALSE);
    dialog_cleanup(dialog);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
static void file_open_ok(GtkWidget* widget, GtkWidget* fs)
{
    int     i;
    gchar*  filename;   /* here gtk_file_selection_get_filename return value is const, but HW_OpenFile parameter
                         * shouldn't be const */
    filename = (gchar*)gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
    fprintf(stderr, "+++++++++++++++++++++++<demo_info> Open the file %s!\n", filename);
    for(i = 0; i < TotalChannel; i++)
    {
        if(HW_OpenFile(ChannelHandle[i], filename) < 0)
            fprintf(stderr, "<demo_info>the md card open file failed, for 0x%x!\n",
                    GetLastErrorNum());
        else
            fprintf(stderr, "<demo_info> the md open file ok!\n");
    }

    for(i = 0; i < TotalChannel; i++)
    {
        if(HW_Play(ChannelHandle[i]) < 0)
            fprintf(stderr, "<demo_info> play the file failed! error code 0x%x!\n",
                    GetLastErrorNum());
        else
        {
            pthread_attr_t  attr;
            int*            threadarg;
            threadarg = (int*)malloc(sizeof(int));
            *threadarg = i;
            sem_init(&FileEndSem[i], 0, 0);
            fprintf(stderr, "<demo_info> play the file ok,the file end sem is %p!\n",
                    &FileEndSem[i]);
            HW_SetFileEndMsg(ChannelHandle[i], &FileEndSem[i]);
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&hDecodeOver[i], &attr, PlayOver, (void*)threadarg);
            bfileopenok++;
            sem_post(&beginSem[i]);
        }
    }

    fprintf(stderr, "++++++++++++++++++++++++++ <demo_info> file open operate ok!\n");

    /* close the dialog */
    dialog_cleanup(GTK_WIDGET(fs));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void video_out(GtkWidget* widget, gpointer data)
{
    REGION_PARAM    param[MAX_DISPLAY_REGION];
    int             i;

    if(GTK_TOGGLE_BUTTON(w_videoout)->active)
    {
        memset(param, 0, sizeof(param));
        for(i = 0; i < MAX_DISPLAY_REGION; i++)
        {
            param[i].r = i * 8 + 128;
            param[i].g = i * 16;
            param[i].b = i * 24;
        }

        /* to set the port0 as the 4 cif windows */
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
        SetDisplayRegion(0, 4, param, 0);

        /* to set the port1 as the 1 d1 window */
        param[0].left = 0;
        param[0].top = 0;
        param[0].width = 704;
        param[0].height = 576;
        SetDisplayRegion(1, 1, param, 0);

        SetDecoderVideoOutput(0, 0, 1, 0, 0, 0);
        SetDecoderVideoOutput(1, 0, 1, 0, 1, 0);
        SetDecoderVideoOutput(2, 0, 1, 0, 2, 0);

        /*
         * Set the video from the last decode out to as the region4 in por0 and the whole
         * port1
         */
        SetDecoderVideoOutput(3, 0, 1, 0, 3, 0);
        SetDecoderVideoOutput(3, 1, 1, 1, 0, 0);

        SetDecoderAudioOutput(0, 1, 0);
        SetDecoderAudioOutput(1, 1, 1);

#ifdef RAWSTREAM
        /* start */
        if(SetDisplayVideoCapture(0, 1, 25, 704, 576, imageBuf) < 0)
            fprintf(stderr, "<demo_info>.............. set the image stream capture failed!\n");
        fprintf(stderr, "<demo_info> ................start image raw stream capture ok!\n");
#endif
    }
    else
    {
        /* stop the video out */
        SetDecoderVideoOutput(0, 0, 0, 0, 0, 0);
        SetDecoderVideoOutput(1, 0, 0, 0, 1, 0);
        SetDecoderVideoOutput(2, 0, 0, 0, 2, 0);
        SetDecoderVideoOutput(3, 0, 0, 0, 3, 0);
        SetDecoderVideoOutput(3, 1, 0, 1, 0, 0);

        /* stop the audio out */
        SetDecoderAudioOutput(0, 0, 0);
        SetDecoderAudioOutput(1, 0, 1);

        for(i = 0; i < displayChannelCount; i++)
            ClearDisplayRegion(i, 0xffff);
#ifdef RAWSTREAM
        /* stop */
        if(SetDisplayVideoCapture(0, 0, 25, 704, 576, imageBuf) < 0)
            fprintf(stderr, "<demo_info> set the image stream capture failed!\n");
        fprintf(stderr, "<demo_info> stop image raw stream capture ok!\n");
#endif
    }

    return;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void decode_file(GtkWidget* widget, gpointer data)
{
    GtkWidget*  file_sel = NULL;
    int         i;
    if(GTK_TOGGLE_BUTTON(w_decodefile_flag)->active)
    {
        file_sel = gtk_file_selection_new("Select a file to decode!");
        gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->ok_button), "clicked",
                           GTK_SIGNAL_FUNC(file_open_ok), file_sel);
        gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->cancel_button), "clicked",
                           GTK_SIGNAL_FUNC(file_open_cancel), file_sel);
        gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(file_sel));
        gtk_file_selection_set_filename(GTK_FILE_SELECTION(file_sel), "test000.mp4");
        gtk_widget_show(file_sel);
        hik_set_sensitive("b_DecodeStream", 0);
        hik_set_sensitive("b_videoaudioout", 1);
    }
    else
    {   /* stop decode */
        if(bfileopenok)
        {
            for(i = 0; i < TotalChannel; i++)
            {
                sem_post(&FileEndSem[i]);
            }

            bfileopenok = 0;
        }

        hik_set_sensitive("b_DecodeStream", 1);
        hik_set_sensitive("b_videoaudioout", 0);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_videoout), FALSE);
    }

    return;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int stopstreamdecode()
{
    int retval;
    int i;
    fprintf(stderr, "<demo_info> stop decoding the stream!\n");

    gdk_threads_leave();
    for(i = 0; i < TotalChannel; i++)
    {
        bDisplayOpen[i] = 0;
        sem_post(&displaySem[i]);
        
        while(!bDisplayClose[i])
            usleep(10);
    }
    gdk_threads_enter();

    for(i = 0; i < TotalChannel; i++)
    {
        HW_Stop(ChannelHandle[i]);
        HW_CloseStream(ChannelHandle[i]);
        MP4_ClientStopCapture(clienthandle[i]);
        retval = MP4_ClientStop(clienthandle[i]);
        if(retval == -1)
            fprintf(stderr, "<demo_info>clientstop error\n");
    }

    MP4_ClientCleanup();
    gdk_window_clear(w_full_area->window);
    streamopenok = 0;
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void decode_stream(GtkWidget* widget, gpointer data)
{
    int                 i;
    CLIENTSHOWRECT      clientrect;
    CLIENT_VIDEOINFO    Clientinfo;
    char                streamheader[128];
    int                 header_len;
    if(GTK_TOGGLE_BUTTON(w_decodestream)->active)
    {
        /* init the net client connect */
        MP4_ClientSetNetPort(5050, 6050);
        MP4_SetDecoder(0);  /* disable the client sdk to decode stream */

        MP4_ClientStartup(clientrect, NULL);
        Clientinfo.m_bRight = 0;

        /*
         * Clientinfo.m_bRemoteChannel = 0;
         */
        Clientinfo.m_sUserName = "username1234567890";
        Clientinfo.m_sUserPassword = "userpassword1234567890";
        Clientinfo.m_bUserCheck = 0;
        Clientinfo.subshow_x = 0;
        Clientinfo.subshow_y = 0;
        Clientinfo.subshow_uWidth = 0;
        Clientinfo.subshow_uHeight = 0;
        Clientinfo.m_bSendMode = PTOPTCPMODE;

        for(i = 0; i < TotalChannel; i++)
        {
            Clientinfo.m_bRemoteChannel = atoi(serverchan[i]);
            Clientinfo.m_sIPAddress = serverip[i];
            fprintf(stderr, "<demo_info>chan%d to connect the server:%s!chan %d\n", i,
                    Clientinfo.m_sIPAddress, Clientinfo.m_bRemoteChannel);
            clienthandle[i] = MP4_ClientStart(&Clientinfo, ReadDataCallBack);
            if(clienthandle[i] == -1)
            {
                fprintf(stderr, "<demo_info> clientstart error:%x\n", MP4_ClientGetLastErrorNum());
                MP4_ClientCleanup();
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_decodestream), FALSE);
                return;
            }

            datasum[i] = 0;
            header_len = 0;
            fprintf(stderr, "<demo_info> to start capture!\n");
            
            int trytimes = 5;
RETRY:            
            MP4_ClientStartCaptureEx(clienthandle[i], streamheader, &header_len);
            if(header_len == 0)
            {
                if(trytimes --)
                {
                    goto RETRY;
                }

                fprintf(stderr, "<demo_info> have not get the stream header!\n");
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_decodestream), FALSE);
                return;
            }

            fprintf(stderr, "<demo_info> get the stream header!len:%d\n", header_len);
        }

        g_timer_start(count_timer);

        for(i = 0; i < TotalChannel; i++)
        {
            if(HW_OpenStream(ChannelHandle[i], streamheader, header_len) < 0)
            {
                fprintf(stderr, "<demo_info> open stream failed for decode!\n");
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_decodestream), FALSE);
                return;
            }

            if(HW_Play(ChannelHandle[i]) < 0)
                fprintf(stderr, "<demo_info>in decode stream, play failed!\n");
            sem_post(&beginSem[i]);
            fprintf(stderr, "<demo_info> to start the display chan%d!flag is %d\n", i,
                    bDisplayOpen[i]);
        }

        streamopenok = 1;
        hik_set_sensitive("b_DecodeFile", 0);
        hik_set_sensitive("b_videoaudioout", 1);
        fprintf(stderr, "<demo_info> md start stream decode ok!\n");
    }
    else
    {
        stopstreamdecode();
        hik_set_sensitive("b_DecodeFile", 1);
        hik_set_sensitive("b_videoaudioout", 0);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_videoout), FALSE);
        g_timer_stop(count_timer);
        bit_rate_time = 0;
        for(i = 0; i < TotalChannel; i++)
            datasum[i] = 0;
        
        gtk_text_buffer_set_text(w_bit_rate_buffer, "0", strlen("0"));
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_bit_rate), w_bit_rate_buffer);
        
        gtk_text_buffer_set_text(w_decode_framerate_buffer, "0", strlen("0"));
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_decode_framerate), w_decode_framerate_buffer);
    }

    return;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void process_buttons(GtkWidget* widget, gpointer data)
{
    int i = GPOINTER_TO_INT(data);

    switch(i)
    {
        case 2: /* exit */
            demo_exit();
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
int PtInRect(SDL_Rect rect, int x, int y)
{
    if((x > (full_win.x + rect.x)) && (x < (full_win.x + rect.x + rect.w)) &&
       (y > (full_win.y + rect.y)) && (y < (full_win.y + rect.y + rect.h)))
        return 1;   /* point in rect */
    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int GetPort()
{
    int port = -1;
    int x, y, i;

    gdk_window_get_pointer(w_full_area->window, &x, &y, NULL);
    x += full_win.x;
    y += full_win.y;

    for(i = 0; i < TotalChannel; i++)
    {
        if(PtInRect(dstRect[i], x, y))
        {
            port = i;
            break;
        }
    }

    return port;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int click2(GtkWidget* widget, GdkEventButton* event, gpointer data)
{
    int     port, i;

    port = GetPort();

    gdk_threads_leave();
    if(port >= 0)
    {
        selectport = port;
        gtk_entry_set_text(GTK_ENTRY(w_set_serverip), serverip[selectport]);
        gtk_entry_set_text(GTK_ENTRY(w_set_serverchan), serverchan[selectport]);
    }

    gdk_threads_enter();

    if((!GTK_TOGGLE_BUTTON(w_decodefile_flag)->active) &&
       (!GTK_TOGGLE_BUTTON(w_decodestream)->active))
        return 0;

    fprintf(stderr, "<demo_info> enter the click2, the port is %d!\n", port);
    if(event->type != GDK_2BUTTON_PRESS)
        return TRUE;
    if(clickflag)
    {
        if(port < 0)
            return TRUE;

        gdk_threads_leave();
        for(i = 0; i < TotalChannel; i++)
        {
            bDisplayOpen[i] = 0;
            sem_post(&displaySem[i]);       /* fixed at 2005.10.8 */
            fprintf(stderr, "<demo_info> in click2  stop the display %d!\n", i);

            /*
             * sometimes, the channel is not realy stop the display, so do a delay. ;
             * of cause there have other more effective ways to do the wait that display
             * stoped ;
             * usleep(1000);
             */
            while(!bDisplayClose[i])
                usleep(10);
        }

        clickport = port;

        memcpy(&tmpRect, &dstRect[port], sizeof(SDL_Rect));
        memcpy(&dstRect[port], &full_rect, sizeof(SDL_Rect));

        sem_post(&beginSem[port]);
        gdk_threads_enter();
    }
    else
    {
        gdk_threads_leave();
        bDisplayOpen[clickport] = 0;
        sem_post(&displaySem[clickport]);   /* fixed at 2005.10.8 */
        while(!bDisplayClose[clickport])
            usleep(10);
        memcpy(&dstRect[clickport], &tmpRect, sizeof(SDL_Rect));

        /* generate expose event */
        gdk_window_hide(w_full_area->window);
        usleep(1000);
        gdk_window_show(w_full_area->window);

        gdk_window_clear(w_full_area->window);
        for(i = 0; i < TotalChannel; i++)
        {
            sem_post(&beginSem[i]);
        }

        gdk_threads_enter();
    }

    clickflag ^= 1;

    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
gint expose_ev(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    char*       str1 = "DS-400xMD demo:";
    GdkColor    color;
    char        title_str[160];
    char        tmpstr[] = " ";

    sprintf(title_str, "%sThere have %ld decode channels. %20s Start Time: %s", str1, TotalChannel,
            tmpstr, starttime);

    camp = gdk_colormap_get_system();
    gc1 = gdk_gc_new(title->window);                    /* get GC */

    /* title expose */
    if(widget->window == title->window)
    {
        /* set title background */
        color.red = 0;
        color.green = 0;
        color.blue = 0xffff;
        if(!gdk_color_alloc(camp, &color))
        {
            g_error("Cannot allocate color\n");
        }

        gdk_window_set_background(widget->window, &color);

        /* set foreground */
        color.red = 0xffff;
        color.green = 0xffff;
        color.blue = 0xffff;
        if(!gdk_color_alloc(camp, &color))
        {
            g_error("Cannot allocate color\n");
        }

        gdk_gc_set_foreground(gc1, &color);
        gdk_window_clear(widget->window);

		PangoLayout* layout = gtk_widget_create_pango_layout (widget, title_str);
		PangoFontDescription* fontdesc = pango_font_description_from_string ("Luxi Mono 12");
		if(layout && fontdesc)
		{
			pango_layout_set_font_description (layout, fontdesc); 
			gdk_draw_layout(widget->window, gc1, 10, 5, layout);
			pango_font_description_free (fontdesc);
			g_object_unref (layout);
		}
        
        return(TRUE);
    }

    /*
     * video_area expose ;
     * allocte color
     */
    gc2 = gdk_gc_new(w_full_area->window);              /* get GC */
    if(gdk_color_parse("black", &color))
    {
        if(!gdk_colormap_alloc_color(camp, &color, FALSE, TRUE))
        {
            g_error("Cannot allocate color\n");
        }
    }

    gdk_window_set_background(widget->window, &color);  /* set background color */

    /* set foreground to sign motion block */
    color.red = 0;
    color.green = 0xffff;
    color.blue = 0;
    if(!gdk_color_alloc(camp, &color))
    {
        g_error("Cannot allocate color\n");
    }

    gdk_gc_set_foreground(gc2, &color);
    gdk_window_clear(widget->window);

    return(TRUE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int process_time(gpointer data)
{
    char                buf[100];
    char                tmp[] = "%";
    int                 freespace;
    unsigned long       framerate;
    unsigned long       DecFrameNum;
    int                 bit_rate;

    now = time(NULL);
    tm = localtime(&now);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d", tm);

    gtk_text_buffer_set_text(w_status_buffer[0], timebuf, strlen(timebuf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_status[0]), w_status_buffer[0]);

    /*
     * gtk_text_backward_delete(GTK_TEXT_VIEW(w_status[0]), strlen(timebuf));
     * *gtk_text_insert(GTK_TEXT_VIEW(w_status[0]), NULL, NULL, NULL, timebuf, -1);
     */
    strftime(timebuf, sizeof(timebuf), "%T", tm);
    gtk_text_buffer_set_text(w_status_buffer[1], timebuf, strlen(timebuf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_status[1]), w_status_buffer[1]);

    /*
     * gtk_text_backward_delete(GTK_TEXT(w_status[1]), strlen(timebuf));
     * * gtk_text_insert(GTK_TEXT(w_status[1]), NULL, NULL, NULL, timebuf, -1);
     */
    if(statfs(savepath, &bbuf) < 0)
    {
        printf("statsfs() faied, error: %d\n", errno);

        /*
         * printf("savepath = %s, bbuf,f_bavail = %d, bbuf.f_blocks = %d\n", savepath,
         * bbuf.f_bavail, bbuf.f_blocks);
         */
    }
    else
    {
        freespace = (100 * bbuf.f_bavail) / bbuf.f_blocks;
        sprintf(buf, "%3d%s", freespace, tmp);
    }

    /*
     * printf("total block = %d, availble block = %d, block size = %d\n",
     * bbuf.f_blocks, bbuf.f_bavail, bbuf.f_bsize);
     * *printf("total block = %d, availble block = %d, block size = %d\n",
     * bbuf.f_blocks, bbuf.f_bavail, bbuf.f_bsize);
     */
    gtk_text_buffer_set_text(w_status_buffer[2], buf, strlen(buf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_status[2]), w_status_buffer[2]);

    /*
     * gtk_text_backward_delete(GTK_TEXT_VIEW(w_status[2]), strlen(buf));
     * * gtk_text_insert(GTK_TEXT_VIEW(w_status[2]), NULL, NULL, NULL, buf, -1);
     */
    sprintf(buf, "%9d", selectport);
    gtk_text_buffer_set_text(w_channel_buffer, buf, strlen(buf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_channel), w_channel_buffer);

    /*
     * gtk_text_backward_delete(GTK_TEXT_VIEW(w_channel), strlen(buf));
     * * gtk_text_insert(GTK_TEXT_VIEW(w_channel), NULL, NULL, NULL, buf, -1);
     * ;
     * bit rate
     */
    elapse_time = g_timer_elapsed(count_timer, NULL);
    bit_rate_time = (unsigned long long)elapse_time;
    if(bit_rate_time > 0)
    {
        bit_rate = (datasum[selectport] * 8) / elapse_time;
        sprintf(buf, "%10d", bit_rate);
        gtk_text_buffer_set_text(w_bit_rate_buffer, buf, strlen(buf));
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_bit_rate), w_bit_rate_buffer);
    }

    /*
     * gtk_text_backward_delete(GTK_TEXT_VIEW(w_bit_rate), strlen(buf));
     * gtk_text_insert(GTK_TEXT_VIEW(w_bit_rate), NULL, NULL, NULL, buf, -1);
     */
    HW_GetCurrentFrameRate(ChannelHandle[selectport], &framerate);
    sprintf(buf, "%10lu", framerate);
    gtk_text_buffer_set_text(w_decode_framerate_buffer, buf, strlen(buf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_decode_framerate), w_decode_framerate_buffer);

    /*
     * gtk_text_backward_delete(GTK_TEXT(w_decode_framerate), strlen(buf));
     * * gtk_text_insert(GTK_TEXT(w_decode_framerate), NULL, NULL, NULL, buf, -1);
     */
    HW_GetPlayedFrames(ChannelHandle[selectport], &DecFrameNum);
    sprintf(buf, "%10lu", DecFrameNum);
    gtk_text_buffer_set_text(w_totalframe_buffer, buf, strlen(buf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_totalframe), w_totalframe_buffer);

    /*
     * gtk_text_backward_delete(GTK_TEXT(w_totalframe), strlen(buf));
     * * gtk_text_insert(GTK_TEXT(w_totalframe), NULL, NULL, NULL, buf, -1);
     */
    return TRUE;    /* if return 0, will never call this func. */
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int main(int argc, char* argv[])
{
    GtkWidget*      frame, *main_box, *hbox, *hbox1, *vbox1, *vbox2, *hbox2;
    GtkWidget*      label;
    int             win_width, win_height;
    int             i;
    pthread_attr_t  attr;
    char            SDL_windowhack[32];

    /* init the device */
    fprintf(stderr, "<demo_info> Begin init the device!\n");

    /*
     * XInitThreads();
     */
    if(!g_thread_supported())
    {
        g_thread_init(NULL);
    }
    else
    {
        fprintf(stderr, "your glib is not support multi-threads\n");
        exit(-1);
    }

    gdk_threads_init();

    HW_InitDecDevice(&TotalChannel);
    fprintf(stderr, "<demo_info> There have %ld channels in the system!\n", TotalChannel);
    if(TotalChannel == 0)
    {
        fprintf(stderr, "<demo_info> There have no md device in this system!\n");
        return -1;
    }

    for(i = 0; i < TotalChannel; i++)
    {
        if(HW_ChannelOpen(i, &ChannelHandle[i]) < 0)
        {
            fprintf(stderr, "<demo_info> Open the decode channle failed!\n");
            HW_ReleaseDecDevice();
        }
    }

    displayChannelCount = GetDisplayChannelCount();
    for(i = 0; i < displayChannelCount; i++)
        SetDisplayStandard(i, StandardPAL);

    /* init display */
    gtk_set_locale();
    gtk_init(&argc, &argv);
    camp = gdk_colormap_get_system();

    /* get display size */
    win_width = gdk_screen_width();
    win_height = gdk_screen_height();

    fprintf(stderr, "<demo_info> the screen w:%d h:%d!\n", win_width, win_height);

    md_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_signal_connect(GTK_OBJECT(md_window), "delete_event", GTK_SIGNAL_FUNC(demo_exit), NULL);
    gtk_signal_connect(GTK_OBJECT(md_window), "destroy", GTK_SIGNAL_FUNC(demo_exit), NULL);

    gtk_window_set_position(GTK_WINDOW(md_window), GTK_WIN_POS_CENTER);
    gtk_widget_set_usize(md_window, win_width, win_height);

    frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(md_window), frame);

    main_box = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), main_box);

    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
    gtk_box_pack_start(GTK_BOX(main_box), hbox, FALSE, /* TRUE */ FALSE, 0);

    /* title window */
    title = gtk_drawing_area_new();
    gtk_widget_set_usize(title, win_width, 30);

    gtk_widget_set_events(title, GDK_EXPOSURE_MASK);
    gtk_signal_connect(GTK_OBJECT(title), "expose_event", GTK_SIGNAL_FUNC(expose_ev), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), title, FALSE, FALSE, 0);

    /* end title window */
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), hbox, FALSE, TRUE, 0);

    /* channel window */
    frame = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(hbox), frame, FALSE, FALSE, 0);

    full_win.w = WIN_W;
    full_win.h = WIN_H;
    w_full_area = gtk_drawing_area_new();
    gtk_widget_set_usize(w_full_area, full_win.w, full_win.h);
    gtk_widget_set_events(w_full_area, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
    gtk_signal_connect(GTK_OBJECT(w_full_area), "expose_event", GTK_SIGNAL_FUNC(expose_ev), NULL);
    gtk_signal_connect(GTK_OBJECT(w_full_area), "button_press_event", GTK_SIGNAL_FUNC(click2),
                       GINT_TO_POINTER(-1));
    gtk_container_add(GTK_CONTAINER(frame), w_full_area);

    /*
     * end channel window ;
     * opearate window
     */
    frame = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(hbox), frame, TRUE /* FALSE */, TRUE /* FALSE */, 0);

    vbox1 = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox1);

    /* status information */
    frame = gtk_frame_new("Status Information");
    gtk_box_pack_start(GTK_BOX(vbox1), frame, TRUE, FALSE, 0);

    vbox2 = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox2);

    for(i = 0; i < 3; i++)
    {
        hbox2 = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox2), hbox2, FALSE, FALSE, 5);
        label = gtk_label_new(status_str[i]);
        gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 5);

        w_status_buffer[i] = gtk_text_buffer_new(NULL);
        w_status[i] = gtk_text_view_new_with_buffer(w_status_buffer[i]);

        gtk_widget_set_usize(w_status[i], 100, 25);
        if(i == 2)
            gtk_widget_set_usize(w_status[i], 60, 25);
        gtk_text_view_set_editable(GTK_TEXT_VIEW(w_status[i]), FALSE);
        gtk_box_pack_start(GTK_BOX(hbox2), w_status[i], FALSE, FALSE, 0);
    }

    /* frame information */
    frame = gtk_frame_new("Channel Info");
    gtk_box_pack_start(GTK_BOX(vbox1), frame, /* FALSE */ TRUE, FALSE, 0);
    vbox2 = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox2);

    /* chan index */
    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, FALSE, 2);
    label = gtk_label_new("Channel");
    gtk_box_pack_start(GTK_BOX(hbox2), label, TRUE, FALSE, 2);

    w_channel_buffer = gtk_text_buffer_new(NULL);
    w_channel = gtk_text_view_new_with_buffer(w_channel_buffer);

    gtk_widget_set_usize(w_channel, 95, 24);
    gtk_box_pack_start(GTK_BOX(hbox2), w_channel, FALSE, FALSE, 0);

    /* set server ip */
    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, FALSE, 2);
    label = gtk_label_new("Set serverip");
    gtk_box_pack_start(GTK_BOX(hbox2), label, TRUE, FALSE, 2);
    w_set_serverip = gtk_entry_new_with_max_length(16);
    gtk_widget_set_usize(w_set_serverip, 95, 24);
    gtk_entry_set_text(GTK_ENTRY(w_set_serverip), defaultserverip);
    gtk_box_pack_start(GTK_BOX(hbox2), w_set_serverip, TRUE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(w_set_serverip), "activate",
                       GTK_SIGNAL_FUNC(set_serverip_callback), NULL);

    /* set server ip */
    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, FALSE, 2);
    label = gtk_label_new("Set server chan");
    gtk_box_pack_start(GTK_BOX(hbox2), label, TRUE, FALSE, 2);
    w_set_serverchan = gtk_entry_new_with_max_length(1);
    gtk_widget_set_usize(w_set_serverchan, 95, 24);
    gtk_entry_set_text(GTK_ENTRY(w_set_serverchan), "0");
    gtk_box_pack_start(GTK_BOX(hbox2), w_set_serverchan, TRUE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(w_set_serverchan), "activate",
                       GTK_SIGNAL_FUNC(set_serverchan_callback), NULL);

    /* get bit rate */
    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, FALSE, 2);
    label = gtk_label_new("BitRate(bps)");
    gtk_box_pack_start(GTK_BOX(hbox2), label, TRUE, FALSE, 2);

    w_bit_rate_buffer = gtk_text_buffer_new(NULL);
    w_bit_rate = gtk_text_view_new_with_buffer(w_bit_rate_buffer);

    gtk_widget_set_usize(w_bit_rate, 95, 24);
    gtk_box_pack_start(GTK_BOX(hbox2), w_bit_rate, FALSE, FALSE, 0);

    /* get decode frame rate */
    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, FALSE, 2);
    label = gtk_label_new("frame rate:");
    gtk_box_pack_start(GTK_BOX(hbox2), label, TRUE, FALSE, 2);

    w_decode_framerate_buffer = gtk_text_buffer_new(NULL);
    w_decode_framerate = gtk_text_view_new_with_buffer(w_decode_framerate_buffer);

    gtk_widget_set_usize(w_decode_framerate, 95, 24);
    gtk_box_pack_start(GTK_BOX(hbox2), w_decode_framerate, FALSE, FALSE, 0);

    /* get the info for the decode frames */
    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, FALSE, 2);
    label = gtk_label_new("Total Dec frames:");
    gtk_box_pack_start(GTK_BOX(hbox2), label, TRUE, FALSE, 2);

    w_totalframe_buffer = gtk_text_buffer_new(NULL);
    w_totalframe = gtk_text_view_new_with_buffer(w_totalframe_buffer);

    gtk_widget_set_usize(w_totalframe, 95, 24);
    gtk_box_pack_start(GTK_BOX(hbox2), w_totalframe, FALSE, FALSE, 0);

    /* decode file */
    frame = gtk_frame_new("Decode on MD");
    gtk_box_pack_start(GTK_BOX(vbox1), frame, TRUE, FALSE, 0);

    vbox2 = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox2), 5);
    gtk_container_add(GTK_CONTAINER(frame), vbox2);

    /* decode file */
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox1), 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox1, TRUE, FALSE, 5);

    w_decodefile_flag = gtk_check_button_new_with_label("DecodeFile");
    gtk_object_set_data_full(GTK_OBJECT(md_window), "b_DecodeFile", w_decodefile_flag,
                             (GtkDestroyNotify) 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_decodefile_flag), FALSE);
    gtk_box_pack_start(GTK_BOX(hbox1), w_decodefile_flag, TRUE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(w_decodefile_flag), "clicked", GTK_SIGNAL_FUNC(decode_file), NULL);

    /* decode stream */
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox1), 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox1, TRUE, FALSE, 5);

    w_decodestream = gtk_check_button_new_with_label("DecodeStream");
    gtk_object_set_data_full(GTK_OBJECT(md_window), "b_DecodeStream", w_decodestream,
                             (GtkDestroyNotify) 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_decodestream), FALSE);
    gtk_box_pack_start(GTK_BOX(hbox1), w_decodestream, TRUE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(w_decodestream), "clicked", GTK_SIGNAL_FUNC(decode_stream), NULL);

    /* video out */
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox1), 0);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox1, TRUE, FALSE, 5);

    w_videoout = gtk_check_button_new_with_label("VideoAudioOut");
    gtk_object_set_data_full(GTK_OBJECT(md_window), "b_videoaudioout", w_videoout,
                             (GtkDestroyNotify) 0);
    hik_set_sensitive("b_videoaudioout", 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_videoout), FALSE);
    gtk_box_pack_start(GTK_BOX(hbox1), w_videoout, TRUE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(w_videoout), "clicked", GTK_SIGNAL_FUNC(video_out), NULL);

    /* exit button */
    hbox1 = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox1), 0);
    gtk_box_pack_start(GTK_BOX(vbox1), hbox1, FALSE, FALSE, 5);
    w_buttons[2] = gtk_button_new_with_label(button_str[2]);
    gtk_signal_connect(GTK_OBJECT(w_buttons[2]), "clicked", GTK_SIGNAL_FUNC(process_buttons),
                       GINT_TO_POINTER(2));
    gtk_box_pack_start(GTK_BOX(hbox1), w_buttons[2], FALSE, /* TRUE */ FALSE, 0);

    /* setup timer */
    count_timer = g_timer_new();
    processtimer = gtk_timeout_add(1000 / 5, (GtkFunction) process_time, NULL); /* 1/5s */

    gtk_widget_show_all(md_window);

    /* init default parameters */
    for(i = 0; i < TotalChannel; i++)
    {
        sprintf(serverchan[i], "0");
        sprintf(serverip[i], defaultserverip);
    }

    /* the sdl hack must set after the show */
    sprintf(SDL_windowhack, "SDL_WINDOWID=%ld", GDK_WINDOW_XWINDOW(w_full_area->window));
    putenv(SDL_windowhack);

    /*
     * inital the sdl ;
     * XInitThreads();
     */
    putenv("SDL_VIDEO_YUV_HWACCEL=0");

    fprintf(stderr, "Init the sdl...");
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "<sdk_error> init sdl failed!%s\n", SDL_GetError());
        return -1;
    }

    fprintf(stderr, "ok!\n");

    pOverlayScreen = SDL_SetVideoMode(WIN_W, WIN_H, 0, SDL_HWSURFACE);
    if(pOverlayScreen == NULL)
    {
        fprintf(stderr, "<sdk_error> create the sdl screen failed! for %s!\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    /*
     * SDL init finished ;
     * get coordinate of video display window
     */
    gdk_window_get_deskrelative_origin(w_full_area->window, &full_win.x, &full_win.y);
    full_rect.x = 0;                    /* full_win.x;
                                         * */
    full_rect.y = 0;                    /* full_win.y;
                                         * */
    full_rect.w = full_win.w;
    full_rect.h = full_win.h;

    printf("full window at (%d, %d), w = %d, h = %d\n", full_win.x, full_win.y, full_win.w,
           full_win.h);

    int chan = 1;
    switch(TotalChannel)
    {
        case 1:
            chan = 1;
            break;

        case 2:
        case 3:
        case 4:
            chan = 2;
            break;

        case 5:
        case 6:
        case 7:
        case 8:
            chan = 3;
            break;

        default:
            chan = 4;
            break;
    }

    memset(bDisplayOpen, 0, sizeof(bDisplayOpen));
    memset(displaythreadrun, 0, sizeof(displaythreadrun));
    memset(bDisplayClose, 0, sizeof(bDisplayClose));

    for(i = 0; i < TotalChannel; i++)
    {
        int*    threadarg;
        threadarg = (int*)malloc(sizeof(int));
        *threadarg = i;
        v_win[i].w = full_win.w / chan;
        v_win[i].h = full_win.h / chan;
        dstRect[i].x = (i % chan) * v_win[i].w;
        dstRect[i].y = (i / chan) * v_win[i].h;
        dstRect[i].w = v_win[i].w;
        dstRect[i].h = v_win[i].h;

        displaythreadrun[i] = 1;
        sem_init(&beginSem[i], 0, 0);
        sem_init(&displaySem[i], 0, 0); /* fixed at 2005.10.8 */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&hDisplayThread[i], &attr, videodisplay, (void*)threadarg);
    }

#ifdef RAWSTREAM
    {
        char    filename[64];
        RegisterDisplayVideoCaptureCallback((IMAGE_STREAM_CALLBACK) ImageStreamCallback, NULL);
        sprintf(filename, "displayimagefile.yuv");
        imagefile_fd = open(filename, O_WRONLY | O_CREAT, 0777);
    }
#endif

    /* get time */
    now = time(NULL);
    tm = localtime(&now);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d", tm);
    sprintf(starttime, "%s  ", timebuf);

    gtk_text_buffer_set_text(w_status_buffer[0], timebuf, strlen(timebuf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_status[0]), w_status_buffer[0]);

    /*
     * gtk_text_insert(GTK_TEXT_VIEW(w_status[0]), NULL, NULL, NULL, timebuf, -1);
     */
    strftime(timebuf, sizeof(timebuf), "%T", tm);
    strcat(starttime, timebuf);
    gtk_text_buffer_set_text(w_status_buffer[1], timebuf, strlen(timebuf));
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(w_status[1]), w_status_buffer[1]);

    /*
     * gtk_text_insert(GTK_TEXT_VIEW(w_status[1]), NULL, NULL, NULL, timebuf, -1);
     */
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();

    return 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void* PlayOver(void* arg)
{
    UINT    chan = *(UINT*)arg;

    /*
     * fprintf(stderr,"<demo_info> .............. in play over for chan %d!\n", chan);
     */
    sem_wait(&FileEndSem[chan]);
    bDisplayOpen[chan] = 0;         /* to stop the display */
    sem_post(&displaySem[chan]);    /* fixed at 2005.10.8 */
    fprintf(stderr,
            "<demo_info> ......................... have get the play over sem for chan%d!flag :%d\n",
        chan, bDisplayOpen[chan]);

    HW_Stop(ChannelHandle[chan]);
    HW_CloseFile(ChannelHandle[chan]);
    sem_destroy(&FileEndSem[chan]);
    if(bfileopenok)
        bfileopenok--;

    /*
     * fprintf(stderr,"<demo_info> in play over thread, the file open count is %d!\n", bfileopenok);
     */
    if(bfileopenok == 0)
    {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_decodefile_flag), FALSE);
        gdk_window_clear(w_full_area->window);
    }

    if(arg)
        free(arg);
    pthread_exit(NULL);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void* videodisplay(void* arg)
{
    UINT    chan = *(UINT*)arg;
    UINT    useSyncSem = 1;
    ULONG   fnum;

    while(1)
    {
        sem_wait(&beginSem[chan]);
        if(displaythreadrun[chan] == 0)
            break;
        memset(&displayconf[chan], 0, sizeof(displayconf[chan]));
        displayconf[chan].w = dstRect[chan].w;
        displayconf[chan].h = dstRect[chan].h;

        /*
         * sem_init(&displaySem[chan], 0, 0);
         */
        displayconf[chan].SyncSem = &displaySem[chan];
        fprintf(stderr, "<demo_info> to start the display for chan%d!\n", chan);
        if(HW_StartDecVgaDisplay(ChannelHandle[chan], &displayconf[chan], useSyncSem) < 0)
        {
            fprintf(stderr, "<demo_info>start video display (port %d) failed, for 0x%x\n", chan,
                    GetLastErrorNum());
            goto DISPLAYEXIT;
        }

        fprintf(stderr,
                "<demo_info> Chan %d Start video display ok, the sync sem is %p, addr is %p!\n",
                chan, displayconf[chan].SyncSem, displayconf[chan].dataAddr);
        pOverlaySurface[chan] = SDL_CreateYUVOverlay(displayconf[chan].w, displayconf[chan].h,
                                                     SDL_YV12_OVERLAY, pOverlayScreen);
        if(pOverlaySurface[chan] == NULL)
        {
            fprintf(stderr, "chan %d the SDL overlay surface create failed!\n", chan);
            goto DISPLAYEXIT;
        }

        fprintf(stderr, "<demo_info> Chan %d Create the yuv overlay ok!\n", chan);
        bDisplayOpen[chan] = 1;
        bDisplayClose[chan] = 0;
        while(bDisplayOpen[chan])
        {
            usleep(100);

            if(sem_wait(&displaySem[chan]) == 0)
            {
                HW_GetCurrentFrameNum(chan, &fnum);

                /*
                 * if(chan==0) fprintf(stderr,"<dem_info> chan%d, frame number is %d!\n", chan, fnum);
                 */
                gdk_threads_enter();
                SDL_LockYUVOverlay(pOverlaySurface[chan]);
                memcpy(*(pOverlaySurface[chan]->pixels), displayconf[chan].dataAddr,
                       displayconf[chan].imageSize);
                SDL_UnlockYUVOverlay(pOverlaySurface[chan]);
                SDL_DisplayYUVOverlay(pOverlaySurface[chan], &dstRect[chan]);
                gdk_threads_leave();
            }
        }

        if(HW_StopDecChanVgaDisplay(ChannelHandle[chan]) < 0)
        {
            fprintf(stderr, "<demo_info>   stop the video display failed!error: 0x%x!\n",
                    GetLastErrorNum());
            goto DISPLAYEXIT;
        }

        SDL_LockYUVOverlay(pOverlaySurface[chan]);
        memset(*(pOverlaySurface[chan]->pixels), 0, displayconf[chan].imageSize);
        SDL_UnlockYUVOverlay(pOverlaySurface[chan]);

        SDL_FreeYUVOverlay(pOverlaySurface[chan]);
        bDisplayClose[chan] = 1;
        fprintf(stderr, "<demo_info>........................... stop display for chan%d!\n", chan);
    }

DISPLAYEXIT:
    fprintf(stderr, "<demo_info> exit the video display thread for chan%d!\n", chan);
    sem_destroy(&displaySem[chan]);
    if(arg)
        free(arg);
    pthread_exit(NULL);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void ReadDataCallBack(int StockHandle, char* pPacketBuffer, int nPacketSize)
{
    int chan;

    /*
     * fprintf(stderr,"!");
     */
    chan = StockHandle;
    datasum[chan] += nPacketSize;
    if(HW_InputData(ChannelHandle[chan], pPacketBuffer, nPacketSize) < 0)
    {
        /*
         * fprintf(stderr,"<demo_info> input data error:0x%x\n", GetLastErrorNum());
         */
    }
}
