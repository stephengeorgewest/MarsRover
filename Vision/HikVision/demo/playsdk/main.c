

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "hikplay.h"
#include "playsdkpu.h"
#include <time.h>
#include <gdk/gdkx.h>
#include <SDL/SDL.h>
extern int replay;
extern int          ok_begin;
extern int          use_progress;
extern GtkWidget*   cut_file;
extern int          progress_click;
extern int          click2_low, click2_high;
extern int          count;
extern GtkWidget*   dialog2;
extern int          click2_flag;
extern int          start_count;
extern int          end_count;
SDL_Surface*        pOverlayScreen;
SDL_Overlay*        pOverlaySurface;
GtkAdjustment*      adj;
long                current_time;
extern long         total_time;
extern int          port;
GtkWidget*          window1;
int                 current_frame;
GtkWidget*          temp;
GTimer*             countime = NULL;
extern int          fileopenok;
extern PLAYRECT     rect;
int                 temp_width;
int                 temp_height;
long                total_frame;
extern INITINFO     info;
extern char*        filename;
int                 w = 530, h = 450;
extern char         SDL_windowhack[32];
GtkWidget*          range;

int                 delay = 0;  /* add delay time by tyu while hori scale moved by handle */

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void xpm_button(GtkWidget* widget, const char ** xpm_name, const gchar* key)
{
    GtkWidget*  pixmap;
    GdkPixmap*  gdk_pixmap;
    GdkBitmap*  mask;
    GtkStyle*   style;

    style = gtk_widget_get_default_style();
    gdk_pixmap = gdk_pixmap_create_from_xpm_d(window1->window, &mask, &style->bg[GTK_STATE_NORMAL],
                                              (gchar**)xpm_name);
    pixmap = gtk_pixmap_new(gdk_pixmap, mask);
    gtk_widget_show(pixmap);
    gtk_container_add(GTK_CONTAINER(temp), pixmap);
    gtk_object_set_data_full(GTK_OBJECT(window1), key, temp, (GtkDestroyNotify) 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void ch_seek(GtkAdjustment* adjust, gpointer data)
{
    Hik_PlayM4_SetPlayPos(port, (float)adjust->value / 100.0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void Hik_ToggleFullScreen()
{
    SDL_Surface*    screen;

    screen = SDL_GetVideoSurface();
    if(screen != NULL)
        printf("screen is not NULL\n");
    if(SDL_WM_ToggleFullScreen(screen))
    {
        printf("Toggled full screen - now %s\n",
               screen->flags & SDL_FULLSCREEN ? "fullscreen" : "windowed");
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int process_time(gpointer data)
{
    GtkWidget*  temp_total_frame;
    char        buf[20];
    char        time_buf[20];
    static int  last_time;
    int         retval;
    static int  w_temp, h_temp;
    int         value;

    range = lookup_widget(window1, "w_time");
    temp = lookup_widget(window1, "w_current_frame");
    if((current_frame = Hik_PlayM4_GetCurrentFrameNum(port)) < 0)
    {
        fprintf(stderr, "\nHik_PlayM4_GetPlayedFrames failed!,Error  0x%lx",
                (unsigned long)Hik_PlayM4_GetLastErrorCode());
    }

    sprintf(buf, "%3d", current_frame);
    gtk_entry_set_text(GTK_ENTRY(temp), buf);

    if(delay > 0)
    {
        delay--;

        int temp = Hik_PlayM4_GetPlayedTime(port);

        if(delay == 0 && last_time == temp)
        {
            delay++;
        }
    }
    else
    {
        current_time = Hik_PlayM4_GetPlayedTime(port);
        last_time = current_time;
        sprintf(time_buf, "%02d:%02d:%02d", current_time / 3600, current_time % 3600 / 60,
                current_time % 3600 % 60);
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(window1, "w_start_time")), time_buf);
        adj = gtk_range_get_adjustment(GTK_RANGE(range));
        adj->value = (current_time * 100) / total_time;
    }

    gtk_window_set_resizable(GTK_WINDOW(window1), TRUE);

    if(replay)
    {
        if(current_time == total_time)
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
    }

    if(fileopenok)
    {
        w_temp = w;
        h_temp = h;
        if(Hik_PlayM4_GetPictureSize(port, &w, &h) < 0)
        {
            fprintf(stderr, "\nHik_PlayM4_GetPictureSize failed! Error 0x%lx",
                    (unsigned long)Hik_PlayM4_GetLastErrorCode());
        }

        if(((w_temp != w) || (h_temp != h)) && (click2_flag % 2 == 0))
        {
            rect.x = 0;
            rect.y = 0;
            info.uWidth = w;
            info.uHeight = h;
            rect.uWidth = w;
            rect.uHeight = h;
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

            gtk_window_set_resizable(GTK_WINDOW(window1), FALSE);
            gtk_widget_set_size_request(lookup_widget(window1, "vbox3"), w, h);
            gtk_widget_set_size_request(lookup_widget(window1, "w_full_area"), w, h);
            gtk_widget_set_size_request(window1, w, h + 80);
        }

        if(count)
        {
            int frame_rate;
            int abs_frame;

            frame_rate = Hik_PlayM4_GetCurrentFrameRate(port);
            sprintf(buf, "%3d", frame_rate);
            gtk_entry_set_text(GTK_ENTRY(lookup_widget(dialog2, "entry11")), buf);

            long int    played_frame;

            played_frame = Hik_PlayM4_GetPlayedFrames(port);
            sprintf(buf, "%3ld", played_frame);
            gtk_entry_set_text(GTK_ENTRY(lookup_widget(dialog2, "entry12")), buf);
            abs_frame = Hik_PlayM4_GetAbsFrameNum(port);
            sprintf(buf, "%3d", abs_frame);
            gtk_entry_set_text(GTK_ENTRY(lookup_widget(dialog2, "entry14")), buf);
        }

        if(progress_click)
        {
            if(use_progress)
            {
                char    buf[100];

                if((GTK_TOGGLE_BUTTON(lookup_widget(cut_file, "w_by_time"))->active))
                {
                    sprintf(buf, "%3d", Hik_PlayM4_GetPlayedTime(port));

                    gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_begin")), buf);
                }
                else
                {
                    sprintf(buf, "%3d", atoi(gtk_entry_get_text(
                                GTK_ENTRY(lookup_widget(window1, "w_current_frame")))));
                    gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_begin")), buf);
                }
            }
        }

        if(ok_begin)
        {
            char    buf[100];

            if(!(GTK_TOGGLE_BUTTON(lookup_widget(cut_file, "w_by_time"))->active))
            {
                sprintf(buf, "%3d", atoi(gtk_entry_get_text(
                            GTK_ENTRY(lookup_widget(window1, "w_current_frame")))));

                gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_end")), buf);
            }
            else
            {
                sprintf(buf, "%3d", Hik_PlayM4_GetPlayedTime(port));

                gtk_entry_set_text(GTK_ENTRY(lookup_widget(cut_file, "w_end")), buf);
            }
        }
    }

    return TRUE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int main(int argc, char* argv[])
{
#ifdef ENABLE_NLS
    bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif
    gtk_set_locale();
    gtk_init(&argc, &argv);

    char    sdl_windowhack[32];

    /*
     * The following code was added by Glade to create one of each component (except
     * popup menus), just so that you see something after building the project. Delete
     * any components that you don't want shown initially.
     */
    window1 = create_window1();
    gtk_widget_show(window1);
    temp = lookup_widget(window1, "w_play");
    xpm_button(temp, xpm_play, "play");
    temp = lookup_widget(window1, "w_pause");
    xpm_button(temp, xpm_pause, "pause");
    temp = lookup_widget(window1, "w_stop");
    xpm_button(temp, xpm_stop, "stop");
    temp = lookup_widget(window1, "w_goto_start");
    xpm_button(temp, xpm_goto_start, "goto_start");
    temp = lookup_widget(window1, "w_goto_end");
    xpm_button(temp, xpm_goto_end, "goto_end");
    temp = lookup_widget(window1, "w_back");
    xpm_button(temp, xpm_back, "back");
    temp = lookup_widget(window1, "w_forward");
    xpm_button(temp, xpm_forward, "forward");
    gtk_main();
    return 0;
}
