

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "playsdkpu.h"
#include <gdk/gdkx.h>
#include <SDL/SDL.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


int pic_index;
int                     replay = 0;
int                     use_progress = 0;
int                     progress_click = 0;
int                     ok_begin = 0;
GtkWidget*              select_index_file;
int                     set_index_file = 0;
int                     click2_low = 0;
int                     click2_high = 0;
char                    indexfile[100]; /* change indexfile point to indexfile matrix */
int                     click_index = 0;
int                     fileopencount = 0;
GtkWidget*              dialog2, *dialog, *cut_file;
int                     count = 0;
int                     audio = 0;
int                     play_audio = 0;
int                     forword = 0;
int                     capture_count = 0;
char                    SDL_windowhack[32];
GtkWidget*              w_drawingarea;
extern long             current_time;
long                    total_time;
int                     port = 0;
GtkWidget*              file_select;
PLAYRECT                rect;
INITINFO                info;
char*                   filename;
int                     fileopenok = 0;
extern GtkWidget*       window1;
int                     process_time(gpointer data);
extern GtkWidget*       range;
extern GtkAdjustment*   adj;
extern int              w, h;
extern long             total_frame;
extern int              current_frame;
extern int              delay;          /* tyu add for moving hori scale */
static int              speed = 0;      /* tyu add for record the speed */

extern GtkWidget* create_fileselection2(void);
/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
gboolean my_exit(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
    gtk_main_quit();
    return TRUE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
gboolean expose_ev_full(GtkWidget* widget, GdkEventExpose* event, gpointer user_data)
{
    GdkColormap*    colormap;
    GdkColor        color;

    colormap = gdk_colormap_get_system();
    gdk_color_parse("black", &color);
    if(gdk_color_alloc(colormap, &color) < 0)
        g_error("Alloc color failed!");

    gdk_window_set_background(widget->window, &color);
    gdk_window_clear(widget->window);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_open_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    if(fileopencount)
    {
        if(Hik_PlayM4_StopSound() < 0)
        {
            if(Hik_PlayM4_GetLastErrorCode() != 0)
            {
                fprintf(stderr, "\nHik_PlayM4_StopSound failed!,Error  0x%lx",
                        (unsigned long)Hik_PlayM4_GetLastErrorCode());
            }
        }
    }

    progress_click = 0;
    use_progress = 0;
    file_select = (GtkWidget*) create_fileselection1();
    gtk_widget_show(file_select);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_close_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    if(fileopenok)
    {
        if(Hik_PlayM4_Stop(port) < 0)
            fprintf(stderr, "\nHik_pLAYM4_Stop failed!");
        gdk_window_clear(lookup_widget(window1, "w_full_area")->window);
        if(fileopencount)
        {
            if(Hik_PlayM4_StopSound() < 0)
            {
                if(Hik_PlayM4_GetLastErrorCode() != 0)
                {
                    fprintf(stderr, "\nHik_PlayM4_StopSound failed!,Error  0x%lx",
                            (unsigned long)Hik_PlayM4_GetLastErrorCode());
                }
            }
        }

        progress_click = 0;
        use_progress = 0;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_quit_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    gtk_main_quit();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_about_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    dialog = (GtkWidget*) create_dialog1();
    gtk_widget_show(dialog);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void filerefcallback(int port)
{
    //fprintf(stderr, "Index File has been created\n");
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void file_open_ok(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        if(Hik_PlayM4_CloseFile(port) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_Closeile failed!,Error  0x%x",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        fileopenok = 0;
    }

    long        totalframes = -1;
    long        total_frame;
    char        buf[30];
    GtkWidget*  temp_total_frame;

    rect.x = 0;
    rect.y = 0;
    w_drawingarea = lookup_widget(window1, "w_full_area");
    filename = (char*)(gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_select)));
    {
        sprintf(SDL_windowhack, "SDL_WINDOWID=%ld", GDK_WINDOW_XWINDOW(w_drawingarea->window));
        putenv(SDL_windowhack);
    }

    info.uWidth = 1280;
    info.uHeight = 1024;
    rect.uWidth = 352;
    rect.uHeight = 288;
    if(Hik_PlayM4_Init(info, 1) < 0)
    {
        fprintf(stderr, "\nHik_PlayM4_Init  failed! Error 0x%lx",
                (unsigned long)Hik_PlayM4_GetLastErrorCode());
    }

    if(Hik_PlayM4_SetDisplay(info.uWidth, info.uHeight) < 0)
    {
        fprintf(stderr, "\nHik_PlayM4_SetDisplay failed! Error 0x%lx",
                (unsigned long)Hik_PlayM4_GetLastErrorCode());
    }

    if(Hik_PlayM4_SetFileRefCallBack(port, filerefcallback) < 0)
    {
        fprintf(stderr, "\nHik_PlayM4_SetFileRefCallBack  failed! Error 0x%lx",
                (unsigned long)Hik_PlayM4_GetLastErrorCode());
    }

    if(set_index_file)
    {
        if(Hik_PlayM4_SetIndexFile(port, indexfile) < 0)
            fprintf(stderr, "\nHIk_PlayM4_SetIndexFile failed! Error 0x%x",
                    Hik_PlayM4_GetLastErrorCode());
        set_index_file = 0;
        fprintf(stderr, "\nSet index file OK!\n");
    }

    if(Hik_PlayM4_OpenFile(port, filename) < 0)
    {
        fprintf(stderr, "\nHik_PlayM4_Openfile failed!,Error  0x%lx",
                (unsigned long)Hik_PlayM4_GetLastErrorCode());
        return;
    }

    if(Hik_PlayM4_Play(port, rect) < 0)
        fprintf(stderr, "\nHik_PlayM4_Playfailed!,Error  0x%lx",
                (unsigned long)Hik_PlayM4_GetLastErrorCode());
    fileopencount++;
    if(Hik_PlayM4_PlaySound(port) < 0)
    {
        fprintf(stderr, "\nHik_PlayM4_PlaySound failed!,Error  0x%lx",
                (unsigned long)Hik_PlayM4_GetLastErrorCode());
    }

    gtk_timeout_add(1000 / 10, (GtkFunction) process_time, NULL);
    temp_total_frame = lookup_widget(window1, "w_total_frame");
    total_frame = Hik_PlayM4_GetFileFrames(port);
    sprintf(buf, "%3ld", total_frame);
    gtk_entry_set_text(GTK_ENTRY(temp_total_frame), buf);
    total_time = Hik_PlayM4_GetFileTime(port);
    sprintf(buf, "%02d:%02d:%02d", total_time / 3600, total_time % 3600 / 60, total_time % 3600 % 60);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(window1, "w_end_time")), buf);
    gtk_widget_hide(file_select);
    fileopenok++;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void file_cancel_ok(GtkButton* button, gpointer user_data)
{
    gtk_widget_hide(file_select);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void play_callback(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        if(Hik_PlayM4_Play(port, rect) < 0)
            fprintf(stderr, "\nHik_PlayM4_Play failed!.Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        if(audio)
        {
            if(Hik_PlayM4_PlaySound(port) < 0)
            {
                if((unsigned long)Hik_PlayM4_GetLastErrorCode() != 0)
                {
                    fprintf(stderr, "\nHik_PlayM4_PlaySouind failed!,Error  0x%lx",
                            (unsigned long)Hik_PlayM4_GetLastErrorCode());
                }
            }
        }
    }

    if((current_time - 1 == total_time) || (current_time == total_time))
    {
        if(Hik_PlayM4_SetCurrentFrameNum(port, 0) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetCurrentFrameNum failed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        if(Hik_PlayM4_SetPlayPos(port, 0) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetPlayPosfailed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void pause_callback(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        if(GTK_TOGGLE_BUTTON(button)->active)
        {
            if(Hik_PlayM4_Pause(port, 1) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_Pause  failed!.Error 0x%lx",
                        (unsigned long)Hik_PlayM4_GetLastErrorCode());
            }
        }
        else
        {
            if(Hik_PlayM4_Pause(port, 0) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_Pause  failed!.Error 0x%lx",
                        (unsigned long)Hik_PlayM4_GetLastErrorCode());
            }

            if(Hik_PlayM4_PlaySound(port) < 0)
            {
                if((unsigned long)Hik_PlayM4_GetLastErrorCode() != 0)
                {
                    fprintf(stderr, "\nHik_PlayM4_PlaySouind failed!,Error  0x%lx",
                            (unsigned long)Hik_PlayM4_GetLastErrorCode());
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
void stop_callback(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        if(GTK_TOGGLE_BUTTON(button)->active)
        {
            if(Hik_PlayM4_Stop(port) < 0)
                fprintf(stderr, "\nHik_PlayM4_Stop failed!.Error 0x%lx",
                        (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        audio++;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void goto_start_callback(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        if(Hik_PlayM4_SetCurrentFrameNum(port, 0) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetCurrentFrameNum failed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        if(Hik_PlayM4_SetPlayPos(port, 0) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetPlayPosfailed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void forward_callback(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        /* 
         * when click forward button, then speed should increase,
         * then if speed is bigger than 3, it was not true for playersdk, should
         * put speed to 3.
         */
        speed++;

        if(speed <= 3)
        {
            if(Hik_PlayM4_Fast(port) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_Fast failed!,Error  0x%lx",
                        (unsigned long)Hik_PlayM4_GetLastErrorCode());
            }

            if(speed == 3)
            {
                /* 
                 * at the same time should throw B Frame, because the timer cann't be 
                 * lower than 40/8 = 5ms.
                 */
                Hik_PlayM4_ThrowBFrameNum(port, 1);
            }
        }
        else
        {
            speed = 3;
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void back_callback(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        /* 
         * when click back button, then speed should decrease,
         * then if speed is lower than -3, it was not true for playersdk, should
         * put speed to -3.
         */
        speed--;

        if(speed >= -3)
        {
            if(Hik_PlayM4_Slow(port) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_Slow  failed!,Error  0x%lx",
                        (unsigned long)Hik_PlayM4_GetLastErrorCode());
            }

            if(speed != 3)
            {
                /* if the speed was lower than 3, then should not throw B Frame */
                Hik_PlayM4_ThrowBFrameNum(port, 0);
            }
        }
        else
        {
            speed = -3;
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void goto_end_callback(GtkButton* button, gpointer user_data)
{
    if(fileopenok)
    {
        Hik_PlayM4_SetCurrentFrameNum(port, total_frame - 1);
    }

    Hik_PlayM4_SetPlayPos(port, 1);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void step_frame_callback(GtkButton* button, gpointer user_data)
{
    int i;

    i = GPOINTER_TO_INT(user_data);
    if(i == 1)
    {
        if(fileopenok)
        {
            if(Hik_PlayM4_OneByOneBack(port) < 0)
            {
                if((unsigned long)Hik_PlayM4_GetLastErrorCode() != 0)
                {
                    fprintf(stderr, "\nHik_Play_OneByOneBack failed! Error 0x%lx",
                            (unsigned long)Hik_PlayM4_GetLastErrorCode());
                }
            }
        }
    }
    else
    {
        if(fileopenok)
        {
            if(Hik_PlayM4_OneByOne(port) < 0)
                fprintf(stderr, "\nHik_Play_OneByOne failed! Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void DisplayCallBack(int port, char*  pBuf, long size, long width, long height, long stamp,
                     long type, long reserved)
{
    static int  num = 0;
    char        bmp_filename[100];
    char        jpg_filename[100];
    struct tm*  tm;
    char        time_buf[100];
    time_t      now;

    capture_count++;
    if(capture_count == 1)
    {
        now = time(NULL);
        tm = localtime(&now);
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d", tm);
        if(pic_index == 0)
        {
            sprintf(bmp_filename, "%s_%d.bmp", time_buf, num);
            num++;
            if(Hik_PlayM4_ConvertToBmpFile(pBuf, size, width, height, type, bmp_filename) < 0)
                fprintf(stderr, "\nHik_PlayM4_COvertToBmpFile  failed!.Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
        }
        else
        {
            sprintf(jpg_filename, "%s_%d.jpg", time_buf, num);
            num++;
            Hik_PlayM4_SetJpegQuality(50);
            if(Hik_PlayM4_ConvertToJpegFile(pBuf, size, width, height, type, jpg_filename) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_Playm4_CovertToBmpFile  failed!.Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
            }
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void capture_picture_callback(GtkButton* button, gpointer user_data)
{
    pic_index = GPOINTER_TO_INT(user_data);
    if(Hik_PlayM4_SetDisplayCallBack(port, DisplayCallBack) < 0)
        fprintf(stderr, "\nHik_PlayM4_DisplayCallBack failed!.Error 0x%x",
                Hik_PlayM4_GetLastErrorCode());
    capture_count = 0;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void play_audio_callback(GtkButton* button, gpointer user_data)
{
    if(GTK_TOGGLE_BUTTON(button)->active)
    {
        if(Hik_PlayM4_PlaySound(port) < 0)
        {
            if(Hik_PlayM4_GetLastErrorCode() != 0)
                fprintf(stderr, "\nHik_PlaytM4_PlaySound failed!.Error  0x%x",
                        Hik_PlayM4_GetLastErrorCode());
        }
    }
    else
    {
        if(Hik_PlayM4_StopSound() < 0)
        {
            if(Hik_PlayM4_GetLastErrorCode() != 0)
                fprintf(stderr, "\nHik_PlaytM4_StopSound failed!.Error  0x%x",
                        Hik_PlayM4_GetLastErrorCode());
        }
    }
}

int j = 0;

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void process_range_callback(GtkRange* range, gpointer user_data)
{
    int             i;
    int             value;
    GtkAdjustment*  adju;

    i = GPOINTER_TO_INT(user_data);
    if(fileopenok)
    {
        if(i != 2)
        {
            adju = gtk_range_get_adjustment(range);
            value = (gint) adju->value;
            if(Hik_PlayM4_SetVolume(port, value) < 0)
                fprintf(stderr, "\nHik_PlayM4_SetVolume failed!.Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
        }
        else
        {
            delay = 2;
            j++;
            use_progress++;
            gtk_window_set_resizable(GTK_WINDOW(window1), FALSE);
            Hik_PlayM4_ResetBuffer(port, BUF_VIDEO_RENDER);
            Hik_PlayM4_ResetBuffer(port, BUF_AUDIO_RENDER);
            if(Hik_PlayM4_SetPlayPos(port, (float)(adj->value / 100)) < 0)
            {
                if(adj->value > 0 && adj->value < 100)
                    fprintf(stderr, "\nHik_PlayM4_SetPlayPos  failed!.Error 0x%x",
                            Hik_PlayM4_GetLastErrorCode());
            }

            /*
             * gtk_range_set_adjustment(range,adj);
             * tyu modified at 2007.11.27
             */
        }
    }
}

int click2_flag = 0;
int i = 0;

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
gboolean click2(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
    static int  click = 0;

    if(event->type != GDK_2BUTTON_PRESS)
        return 0;
    click2_flag++;
    if(click2_flag % 2 == 1)
    {
        gtk_widget_hide(lookup_widget(window1, "hbox1"));
        gtk_widget_hide(lookup_widget(window1, "hbox5"));
        gtk_widget_hide(lookup_widget(window1, "hbox6"));
        gtk_widget_hide(lookup_widget(window1, "menubar1"));
        w = 1024;
        h = 768;
        rect.x = 0;
        rect.y = 0;
        info.uWidth = w;
        info.uHeight = h;
        rect.uWidth = w;
        rect.uHeight = h;
        if(Hik_PlayM4_SetDisplay(info.uWidth, info.uHeight) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetDisplay failed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        if(Hik_PlayM4_SetPlayRect(port, rect) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetPlayRect failed!,Error  0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        gtk_window_set_resizable(GTK_WINDOW(window1), TRUE);
        gtk_window_fullscreen(GTK_WINDOW(window1));
        i++;
    }
    else
    {
        int w;
        int h;

        if(Hik_PlayM4_GetPictureSize(port, &w, &h) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_GetPictureSize failed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        rect.x = 0;
        rect.y = 0;
        info.uWidth = w;
        info.uHeight = h;
        rect.uWidth = w;
        rect.uHeight = h;
        if(Hik_PlayM4_SetDisplay(info.uWidth, info.uHeight) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetDisplay failed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        if(Hik_PlayM4_SetPlayRect(port, rect) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetPlayRect failed!,Error  0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        gtk_widget_set_size_request(lookup_widget(window1, "vbox3"), w, h);
        gtk_widget_set_size_request(lookup_widget(window1, "w_full_area"), w, h);
        gtk_widget_set_size_request(window1, w, h + 80);
        gtk_widget_show(lookup_widget(window1, "hbox1"));
        gtk_widget_show(lookup_widget(window1, "hbox5"));
        gtk_widget_show(lookup_widget(window1, "hbox6"));
        gtk_widget_show(lookup_widget(window1, "menubar1"));
        gtk_window_unfullscreen(GTK_WINDOW(window1));
        gtk_widget_set_size_request(lookup_widget(window1, "vbox3"), w, h);
        gtk_widget_set_size_request(lookup_widget(window1, "w_full_area"), w, h);
        gtk_widget_set_size_request(window1, w, h + 80);
    }

    gtk_window_set_resizable(GTK_WINDOW(window1), FALSE);
    click++;
    return TRUE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
gboolean move_callback(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
    if(fileopenok)
    {
        int width;
        int height;

        gdk_window_get_size(GDK_WINDOW(lookup_widget(window1, "w_full_area")->window), &width,
                            &height);
        info.uWidth = width;
        info.uHeight = height;
        rect.x = 0;
        rect.y = 0;
        rect.uWidth = width;
        rect.uHeight = height;

        fprintf(stderr, "Change size to width and height to %d and %d\n", rect.uWidth, rect.uHeight);
        if(Hik_PlayM4_SetDisplay(info.uWidth, info.uHeight) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetDisplay failed! Error  0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        if(Hik_PlayM4_SetPlayRect(port, rect) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_SetPlayRect failed!,Error  0x%x",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }
    }

    return TRUE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void set_picture_quality_callback(GtkButton* button, gpointer user_data)
{
    int i;

    i = GPOINTER_TO_INT(user_data);
    if(i == 1)
    {
        if(GTK_TOGGLE_BUTTON(button)->active)
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(dialog2, "w_low")), FALSE);
            if(Hik_PlayM4_SetPicQuality(port, 1) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_SetPicQuality failed! Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
            }

            click2_high++;
        }
        else
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(dialog2, "w_low")), TRUE);
            if(Hik_PlayM4_SetPicQuality(port, 0) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_SetPicQuality failed! Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
            }

            click2_low++;
        }
    }
    else
    {
        if(GTK_TOGGLE_BUTTON(button)->active)
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(dialog2, "w_high")), FALSE);
            if(Hik_PlayM4_SetPicQuality(port, 0) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_SetPicQuality failed! Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
            }

            click2_low++;
        }
        else
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(dialog2, "w_high")), TRUE);
            if(Hik_PlayM4_SetPicQuality(port, 1) < 0)
            {
                fprintf(stderr, "\nHik_PlayM4_SetPicQuality failed! Error 0x%x",
                        Hik_PlayM4_GetLastErrorCode());
            }

            click2_high++;
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_closebutton1_clicked(GtkButton* button, gpointer user_data)
{
    gtk_widget_hide(dialog);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_file_info_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    dialog2 = (GtkWidget*) create_dialog2();
    gtk_widget_show(dialog2);
    count++;

    char    buf[100];

    sprintf(buf, "%3d", Hik_PlayM4_GetFileHeadLength());
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(dialog2, "entry13")), buf);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_closebutton2_clicked(GtkButton* button, gpointer user_data)
{
    gtk_widget_hide(dialog2);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_w_set_index_file_clicked(GtkButton* button, gpointer user_data)
{
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_cut_file_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    cut_file = (GtkWidget*) create_dialog3();
    gtk_widget_show(cut_file);
    if(fileopenok)
    {
        char    buf[100];

        sprintf(buf, "0~%ld", Hik_PlayM4_GetFileFrames(port));
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_frame_num")), buf);
        sprintf(buf, "0~%ld", Hik_PlayM4_GetFileTime(port));
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_frame_time")), buf);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_cancelbutton1_clicked(GtkButton* button, gpointer user_data)
{
    gtk_widget_hide(cut_file);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_okbutton1_clicked(GtkButton* button, gpointer user_data)
{
#if 1
    unsigned int    begin_pos;
    unsigned int    end_pos;
    FRAME_POS       pos_begin;
    FRAME_POS       pos_end;
    long            newfilelen;
    char*           savefilename;
    char            buf[2048];
    long            size = 2048;
    long            block;
    long            count;
    long            remain;
    int             save_fd;
    FILE*           org_fd;

    begin_pos = atoi(gtk_entry_get_text(GTK_ENTRY(lookup_widget(cut_file, "w_begin"))));
    end_pos = atoi(gtk_entry_get_text(GTK_ENTRY(lookup_widget(cut_file, "w_end"))));
    if(fileopenok)
    {
        if(GTK_TOGGLE_BUTTON(lookup_widget(cut_file, "w_by_num"))->active)
        {
            if((begin_pos > Hik_PlayM4_GetFileFrames(port)) ||
               (end_pos > Hik_PlayM4_GetFileFrames(port)) || (begin_pos > end_pos) || (end_pos == 0))
            {
                fprintf(stderr, "\nInput value of pos error !\n");
                return;
            }
            else
            {
                if(Hik_PlayM4_GetKeyFramePos(port, begin_pos, BY_FRAMENUM, &pos_begin) < 0)
                    fprintf(stderr, "\nHIk_PlayM4_GetKeyFramePos  failed! Error 0x%x",
                            Hik_PlayM4_GetLastErrorCode());
                if(Hik_PlayM4_GetNextKeyFramePos(port, end_pos, BY_FRAMENUM, &pos_end) < 0)
                    fprintf(stderr, "\nHIk_PlayM4_GetNextKeyFramePos  failed! Error 0x%x",
                            Hik_PlayM4_GetLastErrorCode());
                newfilelen = pos_end.nFilePos - pos_begin.nFilePos;
                savefilename = (char*)gtk_entry_get_text(GTK_ENTRY(lookup_widget(cut_file, "w_save_file")));
                save_fd = open(savefilename, O_WRONLY | O_CREAT, 0777);
                if(save_fd < 0)
                {
                    perror(">>>open");
                }
                else
                {
                    ftruncate(save_fd, 0);
                    org_fd = fopen(filename, "rb");
                    if(org_fd == 0)
                        fprintf(stderr, "\nRead file %s failed!", filename);
                    else
                    {
                        fseek(org_fd, 0, SEEK_SET);
                        if(fread(buf, Hik_PlayM4_GetFileHeadLength(), 1, org_fd) == 0)
                            fprintf(stderr, "\nRead filehead failed!");
                        else
                        {
                            fprintf(stderr, "\nRead  file head from org_file ok!");
                            if(write(save_fd, buf, Hik_PlayM4_GetFileHeadLength()) < 0)
                                fprintf(stderr, "\nWrite file head failed!");
                            else
                            {
                                fprintf(stderr, "\nWrite file head to save_file ok!\n   fileheadlen=%d\n",
                                        Hik_PlayM4_GetFileHeadLength());
                            }
                        }

                        fseek(org_fd, pos_begin.nFilePos, SEEK_SET);
                        block = newfilelen / size;
                        remain = newfilelen % size;
                        for(count = 0; count < block; count++)
                        {
                            if(fread(buf, size, 1, org_fd) == 0)
                                fprintf(stderr, "\nRead file stream failed!");
                            else
                            {
                                if(write(save_fd, buf, size) < 0)
                                    fprintf(stderr, "\nWrite file stream failed!");
                            }
                        }

                        if(fread(buf, remain, 1, org_fd) == 0)
                            fprintf(stderr, "\nRead remain failed!");
                        else
                        {
                            if(write(save_fd, buf, remain) < 0)
                                fprintf(stderr, "\nWrite  remain failed!");
                        }
                    }
                }
            }
        }
        else if(GTK_TOGGLE_BUTTON(lookup_widget(cut_file, "w_by_time"))->active)
        {
            if((begin_pos > Hik_PlayM4_GetFileFrames(port)) ||
               (end_pos > Hik_PlayM4_GetFileFrames(port)) || (begin_pos > end_pos) || (end_pos == 0))
            {
                fprintf(stderr, "\nInput value of pos error !");
                return;
            }
            else
            {
                if(Hik_PlayM4_GetKeyFramePos(port, begin_pos * 1000, BY_FRAMETIME, &pos_begin) < 0)
                    fprintf(stderr, "\nHIk_PlayM4_GetKeyFramePos  failed! Error 0x%x",
                            Hik_PlayM4_GetLastErrorCode());
                if(Hik_PlayM4_GetNextKeyFramePos(port, end_pos * 1000, BY_FRAMETIME, &pos_end) < 0)
                    fprintf(stderr, "\nHIk_PlayM4_GetNextKeyFramePos  failed! Error 0x%x",
                            Hik_PlayM4_GetLastErrorCode());
                newfilelen = pos_end.nFilePos - pos_begin.nFilePos;
                savefilename = (char*)gtk_entry_get_text(GTK_ENTRY(lookup_widget(cut_file, "w_save_file")));
                save_fd = open(savefilename, O_WRONLY | O_CREAT, 0777);
                if(save_fd < 0)
                {
                    perror(">>>open");
                }
                else
                {
                    ftruncate(save_fd, 0);
                    org_fd = fopen(filename, "rb");
                    if(org_fd == 0)
                        fprintf(stderr, "\nRead file %s failed!", filename);
                    else
                    {
                        fseek(org_fd, 0, SEEK_SET);
                        if(fread(buf, Hik_PlayM4_GetFileHeadLength(), 1, org_fd) == 0)
                            fprintf(stderr, "\nRead filehead failed!");
                        else
                        {
                            fprintf(stderr, "\nRead  file head from org_file ok!");
                            if(write(save_fd, buf, Hik_PlayM4_GetFileHeadLength()) < 0)
                                fprintf(stderr, "\nWrite file head failed!");
                            else
                            {
                                fprintf(stderr, "\nWrite file head to save_file ok!\n  fileheadlen=%d\n", 
                                        Hik_PlayM4_GetFileHeadLength());
                            }
                        }

                        fseek(org_fd, pos_begin.nFilePos, SEEK_SET);
                        block = newfilelen / size;
                        remain = newfilelen % size;
                        for(count = 0; count < block; count++)
                        {
                            if(fread(buf, size, 1, org_fd) == 0)
                                fprintf(stderr, "\nRead file stream failed!");
                            else
                            {
                                if(write(save_fd, buf, size) < 0)
                                    fprintf(stderr, "\nWrite file stream failed!");
                            }
                        }

                        if(fread(buf, remain, 1, org_fd) == 0)
                            fprintf(stderr, "\nRead remain failed!");
                        else
                        {
                            if(write(save_fd, buf, remain) < 0)
                                fprintf(stderr, "\nWrite  remain failed!");
                        }
                    }
                }
            }
        }
    }

    gtk_widget_hide(cut_file);
#endif
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_set_index_file_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    if(GTK_CHECK_MENU_ITEM(menuitem)->active)
    {
        select_index_file = (GtkWidget*) create_fileselection2();
        gtk_widget_show(select_index_file);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_button1_clicked(GtkButton* button, gpointer user_data)
{
    gtk_widget_hide(select_index_file);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_button2_clicked(GtkButton* button, gpointer user_data)
{
    /* correct this bug */
    char*   temp = (char*)(gtk_file_selection_get_filename(GTK_FILE_SELECTION(select_index_file)));

    memcpy(indexfile, temp, strlen(temp) + 1);
    set_index_file++;
    gtk_widget_hide(select_index_file);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_w_use_progress_clicked(GtkButton* button, gpointer user_data)
{
    if(GTK_TOGGLE_BUTTON(button)->active)
    {
        progress_click++;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_w_ok_begin_clicked(GtkButton* button, gpointer user_data)
{
    progress_click = 0;

    static int  start_pos;
    char        buf[100];

    if(GTK_TOGGLE_BUTTON(lookup_widget(cut_file, "w_ok_begin"))->active)
    {
        if(!(GTK_TOGGLE_BUTTON(lookup_widget(cut_file, "w_by_time"))->active))
        {
            start_pos = atoi(gtk_entry_get_text(GTK_ENTRY(lookup_widget(window1, "w_current_frame"))));
            sprintf(buf, "%3d", start_pos);
            gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_begin")), buf);
        }
        else
        {
            start_pos = Hik_PlayM4_GetPlayedTime(port);
            sprintf(buf, "%3d", start_pos);
            gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_begin")), buf);
        }

        ok_begin++;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_w_ok_end_clicked(GtkButton* button, gpointer user_data)
{
    int     end_pos;
    char    buf[100];

    if(ok_begin)
    {
        if(GTK_TOGGLE_BUTTON(button)->active)
        {
            if(!(GTK_TOGGLE_BUTTON(lookup_widget(cut_file, "w_by_time"))->active))
            {
                end_pos = atoi(gtk_entry_get_text(GTK_ENTRY(lookup_widget(window1, "w_current_frame"))));
                sprintf(buf, "%3d", end_pos);
                gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_end")), buf);
            }
            else
            {
                end_pos = Hik_PlayM4_GetPlayedTime(port);
                sprintf(buf, "%3d", end_pos);
                gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_end")), buf);
            }
        }

        ok_begin = 0;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void on_replay_activate(GtkMenuItem* menuitem, gpointer user_data)
{
    if(GTK_CHECK_MENU_ITEM(menuitem)->active)
    {
        replay++;
    }
    else
    {
        replay = 0;
    }
}
