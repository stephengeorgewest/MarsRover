

#include <gtk/gtk.h>

gboolean my_exit(GtkWidget * widget, GdkEvent * event, gpointer user_data);

void        on_open1_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_close_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_quit1_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_about1_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_open_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_quit_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_about_activate(GtkMenuItem* menuitem, gpointer user_data);

void        file_open_ok(GtkButton* button, gpointer user_data);

void        file_cancel_ok(GtkButton* button, gpointer user_data);

void        play_callback(GtkButton* button, gpointer user_data);

void        pause_callback(GtkButton* button, gpointer user_data);

void        stop_callback(GtkButton* button, gpointer user_data);

void        goto_start_callback(GtkButton* button, gpointer user_data);

void        forward_callback(GtkButton* button, gpointer user_data);

void        back_callback(GtkButton* button, gpointer user_data);

void        goto_end_callback(GtkButton* button, gpointer user_data);

void        step_frame_callback(GtkButton* button, gpointer user_data);

void        capture_picture_callback(GtkButton* button, gpointer user_data);

void        play_audio_callback(GtkButton* button, gpointer user_data);

void        process_range_callback(GtkRange* range, gpointer user_data);

gboolean    expose_ev_full(GtkWidget* widget, GdkEventExpose* event, gpointer user_data);

gboolean    click2(GtkWidget* widget, GdkEventButton* event, gpointer user_data);

gboolean    move_callback(GtkWidget* widget, GdkEventButton* event, gpointer user_data);

void        set_picture_quality_callback(GtkButton* button, gpointer user_data);

void        on_closebutton1_clicked(GtkButton* button, gpointer user_data);

void        on_file_info_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_closebutton2_clicked(GtkButton* button, gpointer user_data);

void        on_w_set_index_file_clicked(GtkButton* button, gpointer user_data);

void        on_cut_file_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_cancelbutton1_clicked(GtkButton* button, gpointer user_data);

void        on_okbutton1_clicked(GtkButton* button, gpointer user_data);

void        on_set_index_file_activate(GtkMenuItem* menuitem, gpointer user_data);

void        on_button1_clicked(GtkButton* button, gpointer user_data);

void        on_button2_clicked(GtkButton* button, gpointer user_data);

void        on_w_use_progress_clicked(GtkButton* button, gpointer user_data);

void        on_w_ok_begin_clicked(GtkButton* button, gpointer user_data);

void        on_w_ok_end_clicked(GtkButton* button, gpointer user_data);

void        on_replay_activate(GtkMenuItem* menuitem, gpointer user_data);
