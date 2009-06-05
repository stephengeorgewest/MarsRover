

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component, widget, name) \
    g_object_set_data_full(G_OBJECT(component), name, gtk_widget_ref(widget), \
                           (GDestroyNotify) gtk_widget_unref)
#define GLADE_HOOKUP_OBJECT_NO_REF(component, widget, name) \
    g_object_set_data(G_OBJECT(component), name, widget)

GtkWidget* create_window1 (void)
{
    GtkWidget*      window1;
    GtkWidget*      vbox1;
    GtkWidget*      menubar1;
    GtkWidget*      menuitem4;
    GtkWidget*      menuitem4_menu;
    GtkWidget*      open;
    GtkWidget*      close;
    GtkWidget*      quit;
    GtkWidget*      menuitem7;
    GtkWidget*      menuitem7_menu;
    GtkWidget*      about;
    GtkWidget*      vbox2;
    GtkWidget*      vbox3;
    GtkWidget*      w_full_area;
    GtkWidget*      hbox1;
    GtkWidget*      hbox2;
    GtkWidget*      w_play;
    GtkWidget*      w_pause;
    GtkWidget*      w_stop;
    GtkWidget*      w_goto_start;
    GtkWidget*      w_back;
    GtkWidget*      w_forward;
    GtkWidget*      w_goto_end;
    GtkWidget*      hbox3;
    GtkWidget*      togglebutton7;
    GtkWidget*      togglebutton8;
    GtkWidget*      togglebutton9;
    GtkWidget*      hbox5;
    GtkWidget*      w_start_time;
    GtkWidget*      w_time;
    GtkWidget*      w_end_time;
    GtkWidget*      hbox6;
    GtkWidget*      label1;
    GtkWidget*      w_current_frame;
    GtkWidget*      label2;
    GtkWidget*      w_total_frame;
    GtkWidget*      checkbutton2;
    GtkWidget*      hscale2;
    GtkWidget*      hbox7;
    GtkWidget*      hbox8;
    GtkWidget*      label3;
    GtkWidget*      w_frame_rate;
    GtkWidget*      hbox9;
    GtkWidget*      label4;
    GtkWidget*      w_played_frame;
    GtkWidget*      hbox10;
    GtkWidget*      label5;
    GtkWidget*      hbox11;
    GtkWidget*      w_high;
    GtkWidget*      w_low;
    GtkWidget*      stream1;
    GtkWidget*      stream1_menu;
    GtkWidget*      openstream;
    GtkWidget*      closestream;
    GtkWidget*      file_info, *Other_Info;
    GtkWidget*      Option;
    GtkWidget*      Option_menu;
    GtkWidget*      replay;
    GtkAccelGroup*  accel_group;
    GtkWidget*      file_info_menu, *cut_file, *set_index_file, *togglebutton10;

    accel_group = gtk_accel_group_new();

    window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window1, 352, 386);
    gtk_window_set_title(GTK_WINDOW(window1), _("Hikvision Mpeg4 Player"));
    gtk_window_set_position(GTK_WINDOW(window1), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window1), FALSE);

    vbox1 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox1);
    gtk_container_add(GTK_CONTAINER(window1), vbox1);
    gtk_widget_set_size_request(vbox1, -1, 40);

    menubar1 = gtk_menu_bar_new();
    gtk_widget_show(menubar1);
    gtk_box_pack_start(GTK_BOX(vbox1), menubar1, FALSE, FALSE, 0);

    menuitem4 = gtk_menu_item_new_with_mnemonic(_("_File"));
    gtk_widget_show(menuitem4);
    gtk_container_add(GTK_CONTAINER(menubar1), menuitem4);

    menuitem4_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem4), menuitem4_menu);

    open = gtk_menu_item_new_with_mnemonic(_("Open(_O)"));
    gtk_widget_show(open);
    gtk_container_add(GTK_CONTAINER(menuitem4_menu), open);
    gtk_widget_add_accelerator(open, "activate", accel_group, GDK_O, (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    close = gtk_menu_item_new_with_mnemonic(_("Close(_C)"));
    gtk_widget_show(close);
    gtk_container_add(GTK_CONTAINER(menuitem4_menu), close);
    gtk_widget_add_accelerator(close, "activate", accel_group, GDK_C, (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    cut_file = gtk_menu_item_new_with_mnemonic(_("Cut File(_W)"));
    gtk_widget_show(cut_file);
    gtk_container_add(GTK_CONTAINER(menuitem4_menu), cut_file);
    gtk_widget_add_accelerator(cut_file, "activate", accel_group, GDK_W, (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    set_index_file = gtk_check_menu_item_new_with_mnemonic(_("Set Index file(_S)"));
    gtk_widget_show(set_index_file);
    gtk_container_add(GTK_CONTAINER(menuitem4_menu), set_index_file);
    gtk_widget_add_accelerator(set_index_file, "activate", accel_group, GDK_S, (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(set_index_file), FALSE);
    quit = gtk_menu_item_new_with_mnemonic(_("Quit(_Q)"));
    gtk_widget_show(quit);
    gtk_container_add(GTK_CONTAINER(menuitem4_menu), quit);
    gtk_widget_add_accelerator(quit, "activate", accel_group, GDK_Q, (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    stream1 = gtk_menu_item_new_with_mnemonic(_("_Stream"));
    gtk_widget_show(stream1);
    gtk_container_add(GTK_CONTAINER(menubar1), stream1);
    gtk_widget_hide(stream1);

    stream1_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(stream1), stream1_menu);

    openstream = gtk_menu_item_new_with_mnemonic(_("OpenStream"));
    gtk_widget_show(openstream);
    gtk_container_add(GTK_CONTAINER(stream1_menu), openstream);

    closestream = gtk_menu_item_new_with_mnemonic(_("CloseStream"));
    gtk_widget_show(closestream);
    gtk_container_add(GTK_CONTAINER(stream1_menu), closestream);

    Option = gtk_menu_item_new_with_mnemonic(_("_Option"));
    gtk_widget_show(Option);
    gtk_container_add(GTK_CONTAINER(menubar1), Option);

    Option_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(Option), Option_menu);

    Other_Info = gtk_menu_item_new_with_mnemonic(_("FIle Info(_I)"));
    gtk_widget_show(Other_Info);
    gtk_container_add(GTK_CONTAINER(Option_menu), Other_Info);
    gtk_widget_add_accelerator
        (
            Other_Info, "activate", accel_group, GDK_I,
                (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE
        );

    replay = gtk_check_menu_item_new_with_mnemonic(_("Rewind(_R)"));
    gtk_widget_show(replay);
    gtk_container_add(GTK_CONTAINER(Option_menu), replay);
    gtk_widget_add_accelerator
        (
            replay, "activate", accel_group, GDK_R,
                (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE
        );
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(replay), FALSE);

    menuitem7 = gtk_menu_item_new_with_mnemonic(_("_Help"));
    gtk_widget_show(menuitem7);
    gtk_container_add(GTK_CONTAINER(menubar1), menuitem7);

    menuitem7_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem7), menuitem7_menu);

    about = gtk_menu_item_new_with_mnemonic(_("About(_A)"));
    gtk_widget_show(about);
    gtk_container_add(GTK_CONTAINER(menuitem7_menu), about);
    gtk_widget_add_accelerator(about, "activate", accel_group, GDK_A,(GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    vbox2 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox2);
    gtk_box_pack_start(GTK_BOX(vbox1), vbox2, TRUE, TRUE, 0);

    vbox3 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox3);
    gtk_box_pack_start(GTK_BOX(vbox2), vbox3, TRUE, TRUE, 0);
    gtk_widget_set_size_request(vbox3, -1, -1);

    w_full_area = gtk_drawing_area_new();
    gtk_widget_show(w_full_area);
    gtk_box_pack_start(GTK_BOX(vbox3), w_full_area, TRUE, TRUE, 0);

    gtk_widget_set_events(w_full_area, GDK_ALL_EVENTS_MASK);

    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox1, FALSE, FALSE, 0);
    gtk_widget_set_size_request(hbox1, -1, 30);

    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox2);
    gtk_box_pack_start(GTK_BOX(hbox1), hbox2, FALSE, FALSE, 0);
    gtk_widget_set_size_request(hbox2, 210, -1);

    w_play = gtk_button_new();
    gtk_widget_show(w_play);
    gtk_box_pack_start(GTK_BOX(hbox2), w_play, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_play, 30, -1);
    w_pause = gtk_toggle_button_new();
    gtk_widget_show(w_pause);
    gtk_box_pack_start(GTK_BOX(hbox2), w_pause, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_pause, 30, -1);

    w_stop = gtk_toggle_button_new();
    gtk_widget_show(w_stop);
    gtk_box_pack_start(GTK_BOX(hbox2), w_stop, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_stop, 30, -1);

    w_goto_start = gtk_toggle_button_new();
    gtk_widget_show(w_goto_start);
    gtk_box_pack_start(GTK_BOX(hbox2), w_goto_start, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_goto_start, 30, -1);

    w_back = gtk_toggle_button_new();
    gtk_widget_show(w_back);
    gtk_box_pack_start(GTK_BOX(hbox2), w_back, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_back, 30, -1);

    w_forward = gtk_toggle_button_new();
    gtk_widget_show(w_forward);
    gtk_box_pack_start(GTK_BOX(hbox2), w_forward, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_forward, 30, -1);

    w_goto_end = gtk_toggle_button_new();
    gtk_widget_show(w_goto_end);
    gtk_box_pack_start(GTK_BOX(hbox2), w_goto_end, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_goto_end, 30, -1);

    hbox3 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox3);
    gtk_box_pack_start(GTK_BOX(hbox1), hbox3, FALSE, FALSE, 0);
    gtk_widget_set_size_request(hbox3, 160, -1);

    togglebutton7 = gtk_toggle_button_new_with_mnemonic(_("Back"));
    gtk_widget_show(togglebutton7);
    gtk_box_pack_start(GTK_BOX(hbox3), togglebutton7, FALSE, FALSE, 0);
    gtk_widget_set_size_request(togglebutton7, 40, -1);

    togglebutton8 = gtk_toggle_button_new_with_mnemonic(_("Fore"));
    gtk_widget_show(togglebutton8);
    gtk_box_pack_start(GTK_BOX(hbox3), togglebutton8, FALSE, FALSE, 0);
    gtk_widget_set_size_request(togglebutton8, 40, -1);

    togglebutton9 = gtk_toggle_button_new_with_mnemonic(_("Bmp"));
    gtk_widget_show(togglebutton9);
    gtk_box_pack_start(GTK_BOX(hbox3), togglebutton9, FALSE, FALSE, 0);
    gtk_widget_set_size_request(togglebutton9, 35, -1);

    togglebutton10 = gtk_toggle_button_new_with_mnemonic(_("Jpg"));
    gtk_widget_show(togglebutton10);
    gtk_box_pack_start(GTK_BOX(hbox3), togglebutton10, FALSE, FALSE, 0);
    gtk_widget_set_size_request(togglebutton10, 35, 27);

    hbox5 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox5);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox5, FALSE, FALSE, 0);

    w_start_time = gtk_entry_new();
    gtk_widget_show(w_start_time);
    gtk_box_pack_start(GTK_BOX(hbox5), w_start_time, FALSE, FALSE, 0);
    gtk_widget_set_size_request(w_start_time, 80, -1);
    gtk_entry_set_text(GTK_ENTRY(w_start_time), _("00:00:00"));

    w_time = gtk_hscale_new(GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 100, 1, 0, 0)));
    gtk_widget_show(w_time);
    gtk_box_pack_start(GTK_BOX(hbox5), w_time, TRUE, TRUE, 0);
    gtk_scale_set_draw_value(GTK_SCALE(w_time), FALSE);

    w_end_time = gtk_entry_new();
    gtk_widget_show(w_end_time);
    gtk_box_pack_start(GTK_BOX(hbox5), w_end_time, FALSE, TRUE, 0);
    gtk_widget_set_size_request(w_end_time, 80, -1);
    gtk_entry_set_text(GTK_ENTRY(w_end_time), _("00:00:00"));

    hbox6 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox6);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox6, FALSE, FALSE, 0);
    gtk_widget_set_size_request(hbox6, 100, -1);

    label1 = gtk_label_new(_("Current F"));
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(hbox6), label1, FALSE, FALSE, 0);

    w_current_frame = gtk_entry_new();
    gtk_widget_show(w_current_frame);
    gtk_box_pack_start(GTK_BOX(hbox6), w_current_frame, FALSE, TRUE, 0);
    gtk_widget_set_size_request(w_current_frame, 50, -1);
    gtk_entry_set_text(GTK_ENTRY(w_current_frame), _("  0"));

    label2 = gtk_label_new(_("Total F"));
    gtk_widget_show(label2);
    gtk_box_pack_start(GTK_BOX(hbox6), label2, FALSE, FALSE, 0);

    w_total_frame = gtk_entry_new();
    gtk_widget_show(w_total_frame);
    gtk_box_pack_start(GTK_BOX(hbox6), w_total_frame, FALSE, TRUE, 0);
    gtk_widget_set_size_request(w_total_frame, 50, -1);
    gtk_entry_set_text(GTK_ENTRY(w_total_frame), _("  0"));

    checkbutton2 = gtk_check_button_new_with_mnemonic(_("AudioVolume"));
    gtk_widget_show(checkbutton2);
    gtk_box_pack_start(GTK_BOX(hbox6), checkbutton2, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton2), TRUE);

    hscale2 = gtk_hscale_new(GTK_ADJUSTMENT(gtk_adjustment_new(100, 0, 100, 1, 1, 0)));
    gtk_widget_show(hscale2);
    gtk_box_pack_start(GTK_BOX(hbox6), hscale2, TRUE, TRUE, 0);
    gtk_widget_set_size_request(hscale2, 70, -1);
    gtk_scale_set_draw_value(GTK_SCALE(hscale2), FALSE);

    hbox7 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox7);
    gtk_box_pack_start(GTK_BOX(vbox2), hbox7, FALSE, FALSE, 0);
    gtk_widget_hide(hbox7);

    hbox8 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox8);
    gtk_box_pack_start(GTK_BOX(hbox7), hbox8, FALSE, TRUE, 0);
    gtk_widget_set_size_request(hbox8, 105, -1);

    label3 = gtk_label_new(_("FrameRate"));
    gtk_widget_show(label3);
    gtk_box_pack_start(GTK_BOX(hbox8), label3, FALSE, FALSE, 0);

    w_frame_rate = gtk_entry_new();
    gtk_widget_show(w_frame_rate);
    gtk_box_pack_start(GTK_BOX(hbox8), w_frame_rate, TRUE, TRUE, 0);

    hbox9 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox9);
    gtk_box_pack_start(GTK_BOX(hbox7), hbox9, FALSE, FALSE, 0);
    gtk_widget_set_size_request(hbox9, 100, -1);

    label4 = gtk_label_new(_("Decode F"));
    gtk_widget_show(label4);
    gtk_box_pack_start(GTK_BOX(hbox9), label4, FALSE, FALSE, 0);

    w_played_frame = gtk_entry_new();
    gtk_widget_show(w_played_frame);
    gtk_box_pack_start(GTK_BOX(hbox9), w_played_frame, TRUE, TRUE, 0);

    hbox10 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox10);
    gtk_box_pack_start(GTK_BOX(hbox7), hbox10, TRUE, TRUE, 0);

    label5 = gtk_label_new(_("PicQuality"));
    gtk_widget_show(label5);
    gtk_box_pack_start(GTK_BOX(hbox10), label5, FALSE, FALSE, 0);

    hbox11 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox11);
    gtk_box_pack_start(GTK_BOX(hbox10), hbox11, TRUE, TRUE, 0);

    w_high = gtk_check_button_new_with_mnemonic(_("high"));
    gtk_widget_show(w_high);
    gtk_box_pack_start(GTK_BOX(hbox11), w_high, FALSE, FALSE, 0);

    w_low = gtk_check_button_new_with_mnemonic(_("low"));
    gtk_widget_show(w_low);
    gtk_box_pack_start(GTK_BOX(hbox11), w_low, FALSE, FALSE, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_low), TRUE);
    g_signal_connect((gpointer) window1, "destroy_event", G_CALLBACK(my_exit), NULL);
    g_signal_connect((gpointer) window1, "delete_event", G_CALLBACK(my_exit), NULL);
    g_signal_connect((gpointer) open, "activate", G_CALLBACK(on_open_activate), NULL);
    g_signal_connect((gpointer) close, "activate", G_CALLBACK(on_close_activate), NULL);
    g_signal_connect((gpointer) quit, "activate", G_CALLBACK(on_quit_activate), NULL);
    g_signal_connect((gpointer) about, "activate", G_CALLBACK(on_about_activate), NULL);
    g_signal_connect((gpointer) w_full_area, "expose_event", G_CALLBACK(expose_ev_full), NULL);
    g_signal_connect((gpointer) w_full_area, "button_press_event", G_CALLBACK(click2), NULL);
    g_signal_connect((gpointer) w_full_area, "configure_event", G_CALLBACK(move_callback), NULL);

    g_signal_connect((gpointer) w_play, "clicked", G_CALLBACK(play_callback), NULL);
    g_signal_connect((gpointer) w_pause, "clicked", G_CALLBACK(pause_callback), NULL);
    g_signal_connect((gpointer) w_stop, "clicked", G_CALLBACK(stop_callback), NULL);
    g_signal_connect((gpointer) w_goto_start, "clicked", G_CALLBACK(goto_start_callback), NULL);
    g_signal_connect((gpointer) w_forward, "clicked", G_CALLBACK(forward_callback), NULL);
    g_signal_connect((gpointer) w_back, "clicked", G_CALLBACK(back_callback), NULL);
    g_signal_connect((gpointer) w_goto_end, "clicked", G_CALLBACK(goto_end_callback), NULL);
    g_signal_connect((gpointer) togglebutton7, "clicked", G_CALLBACK(step_frame_callback), (gpointer*)1);
    g_signal_connect((gpointer) togglebutton8, "clicked", G_CALLBACK(step_frame_callback), NULL);
    g_signal_connect((gpointer) togglebutton9, "clicked", G_CALLBACK(capture_picture_callback), (gpointer*)0);
    g_signal_connect((gpointer) togglebutton10, "clicked", G_CALLBACK(capture_picture_callback), (gpointer*)1);
    g_signal_connect((gpointer) w_time, "value_changed", G_CALLBACK(process_range_callback), (gpointer*)2);
    g_signal_connect((gpointer) checkbutton2, "clicked", G_CALLBACK(play_audio_callback), NULL);
    g_signal_connect((gpointer) hscale2, "value_changed", G_CALLBACK(process_range_callback), NULL);

    g_signal_connect((gpointer) Other_Info, "activate", G_CALLBACK(on_file_info_activate), NULL);
    g_signal_connect((gpointer) cut_file, "activate", G_CALLBACK(on_cut_file_activate), NULL);
    g_signal_connect((gpointer) set_index_file, "activate", G_CALLBACK(on_set_index_file_activate), NULL);
    g_signal_connect((gpointer) replay, "activate", G_CALLBACK(on_replay_activate), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(window1, window1, "window1");
    GLADE_HOOKUP_OBJECT(window1, vbox1, "vbox1");
    GLADE_HOOKUP_OBJECT(window1, menubar1, "menubar1");
    GLADE_HOOKUP_OBJECT(window1, menuitem4, "menuitem4");
    GLADE_HOOKUP_OBJECT(window1, menuitem4_menu, "menuitem4_menu");
    GLADE_HOOKUP_OBJECT(window1, open, "open");
    GLADE_HOOKUP_OBJECT(window1, close, "close");
    GLADE_HOOKUP_OBJECT(window1, quit, "quit");
    GLADE_HOOKUP_OBJECT(window1, menuitem7, "menuitem7");
    GLADE_HOOKUP_OBJECT(window1, menuitem7_menu, "menuitem7_menu");
    GLADE_HOOKUP_OBJECT(window1, about, "about");
    GLADE_HOOKUP_OBJECT(window1, vbox2, "vbox2");
    GLADE_HOOKUP_OBJECT(window1, vbox3, "vbox3");
    GLADE_HOOKUP_OBJECT(window1, w_full_area, "w_full_area");
    GLADE_HOOKUP_OBJECT(window1, hbox1, "hbox1");
    GLADE_HOOKUP_OBJECT(window1, hbox2, "hbox2");
    GLADE_HOOKUP_OBJECT(window1, w_play, "w_play");
    GLADE_HOOKUP_OBJECT(window1, w_pause, "w_pause");
    GLADE_HOOKUP_OBJECT(window1, w_stop, "w_stop");
    GLADE_HOOKUP_OBJECT(window1, w_goto_start, "w_goto_start");
    GLADE_HOOKUP_OBJECT(window1, w_back, "w_back");
    GLADE_HOOKUP_OBJECT(window1, w_forward, "w_forward");
    GLADE_HOOKUP_OBJECT(window1, w_goto_end, "w_goto_end");
    GLADE_HOOKUP_OBJECT(window1, hbox3, "hbox3");
    GLADE_HOOKUP_OBJECT(window1, togglebutton7, "togglebutton7");
    GLADE_HOOKUP_OBJECT(window1, togglebutton8, "togglebutton8");
    GLADE_HOOKUP_OBJECT(window1, togglebutton9, "togglebutton9");
    GLADE_HOOKUP_OBJECT(window1, hbox5, "hbox5");
    GLADE_HOOKUP_OBJECT(window1, w_start_time, "w_start_time");
    GLADE_HOOKUP_OBJECT(window1, w_time, "w_time");
    GLADE_HOOKUP_OBJECT(window1, w_end_time, "w_end_time");
    GLADE_HOOKUP_OBJECT(window1, hbox6, "hbox6");
    GLADE_HOOKUP_OBJECT(window1, label1, "label1");
    GLADE_HOOKUP_OBJECT(window1, w_current_frame, "w_current_frame");
    GLADE_HOOKUP_OBJECT(window1, label2, "label2");
    GLADE_HOOKUP_OBJECT(window1, w_total_frame, "w_total_frame");
    GLADE_HOOKUP_OBJECT(window1, checkbutton2, "checkbutton2");
    GLADE_HOOKUP_OBJECT(window1, hscale2, "hscale2");
    GLADE_HOOKUP_OBJECT(window1, hbox7, "hbox7");
    GLADE_HOOKUP_OBJECT(window1, hbox8, "hbox8");
    GLADE_HOOKUP_OBJECT(window1, label3, "label3");
    GLADE_HOOKUP_OBJECT(window1, w_frame_rate, "w_frame_rate");
    GLADE_HOOKUP_OBJECT(window1, hbox9, "hbox9");
    GLADE_HOOKUP_OBJECT(window1, label4, "label4");
    GLADE_HOOKUP_OBJECT(window1, w_played_frame, "w_played_frame");
    GLADE_HOOKUP_OBJECT(window1, hbox10, "hbox10");
    GLADE_HOOKUP_OBJECT(window1, label5, "label5");
    GLADE_HOOKUP_OBJECT(window1, hbox11, "hbox11");
    GLADE_HOOKUP_OBJECT(window1, w_high, "w_high");
    GLADE_HOOKUP_OBJECT(window1, w_low, "w_low");
    GLADE_HOOKUP_OBJECT(window1, togglebutton10, "togglebutton10");

    gtk_window_add_accel_group(GTK_WINDOW(window1), accel_group);

    return window1;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
GtkWidget* create_fileselection1(void)
{
    GtkWidget*  fileselection1;
    GtkWidget*  ok_button1;
    GtkWidget*  cancel_button1;

    fileselection1 = gtk_file_selection_new(_("\351\200\211\346\213\251\346\226\207\344\273\266"));
    gtk_container_set_border_width(GTK_CONTAINER(fileselection1), 10);
    gtk_window_set_type_hint(GTK_WINDOW(fileselection1), GDK_WINDOW_TYPE_HINT_DIALOG);

    ok_button1 = GTK_FILE_SELECTION(fileselection1)->ok_button;
    gtk_widget_show(ok_button1);
    GTK_WIDGET_SET_FLAGS(ok_button1, GTK_CAN_DEFAULT);

    cancel_button1 = GTK_FILE_SELECTION(fileselection1)->cancel_button;
    gtk_widget_show(cancel_button1);
    GTK_WIDGET_SET_FLAGS(cancel_button1, GTK_CAN_DEFAULT);

    g_signal_connect((gpointer) ok_button1, "clicked", G_CALLBACK(file_open_ok), NULL);
    g_signal_connect((gpointer) cancel_button1, "clicked", G_CALLBACK(file_cancel_ok), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(fileselection1, fileselection1, "fileselection1");
    GLADE_HOOKUP_OBJECT_NO_REF(fileselection1, ok_button1, "ok_button1");
    GLADE_HOOKUP_OBJECT_NO_REF(fileselection1, cancel_button1, "cancel_button1");

    return fileselection1;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
GtkWidget* create_dialog1(void)
{
    GtkWidget*  dialog1;
    GtkWidget*  w_about;
    GtkWidget*  vbox4;
    GtkWidget*  vbox5;
    GtkWidget*  hbox15;
    GtkWidget*  label6;
    GtkWidget*  w_sdk_version;
    GtkWidget*  hbox16;
    GtkWidget*  label7;
    GtkWidget*  entry3;
    GtkWidget*  hbox17;
    GtkWidget*  label8;
    GtkWidget*  entry4;
    GtkWidget*  dialog_action_area1;
    GtkWidget*  closebutton1;

    dialog1 = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog1), _("About"));
    gtk_window_set_type_hint(GTK_WINDOW(dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

    w_about = GTK_DIALOG(dialog1)->vbox;
    gtk_widget_show(w_about);

    vbox4 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox4);
    gtk_box_pack_start(GTK_BOX(w_about), vbox4, TRUE, TRUE, 0);

    vbox5 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox5);
    gtk_box_pack_start(GTK_BOX(vbox4), vbox5, TRUE, TRUE, 0);

    hbox15 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox15);
    gtk_box_pack_start(GTK_BOX(vbox5), hbox15, TRUE, TRUE, 0);

    label6 = gtk_label_new(_("          Company"));
    gtk_widget_show(label6);
    gtk_box_pack_start(GTK_BOX(hbox15), label6, FALSE, FALSE, 0);

    w_sdk_version = gtk_entry_new();
    gtk_widget_show(w_sdk_version);
    gtk_box_pack_start(GTK_BOX(hbox15), w_sdk_version, FALSE, TRUE, 0);
    gtk_editable_set_editable(GTK_EDITABLE(w_sdk_version), FALSE);
    gtk_entry_set_text(GTK_ENTRY(w_sdk_version), _("    Hangzhou Hikvision"));
    gtk_entry_set_has_frame(GTK_ENTRY(w_sdk_version), FALSE);

    hbox16 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox16);
    gtk_box_pack_start(GTK_BOX(vbox5), hbox16, FALSE, FALSE, 0);

    label7 = gtk_label_new(_("          Author         "));
    gtk_widget_show(label7);
    gtk_box_pack_start(GTK_BOX(hbox16), label7, FALSE, FALSE, 0);

    entry3 = gtk_entry_new();
    gtk_widget_show(entry3);
    gtk_box_pack_start(GTK_BOX(hbox16), entry3, FALSE, TRUE, 0);
    gtk_editable_set_editable(GTK_EDITABLE(entry3), FALSE);
    gtk_entry_set_text(GTK_ENTRY(entry3), _("xujian"));
    gtk_entry_set_has_frame(GTK_ENTRY(entry3), FALSE);

    hbox17 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox17);
    gtk_box_pack_start(GTK_BOX(vbox5), hbox17, TRUE, TRUE, 0);

    label8 = gtk_label_new(_("          Tel         "));
    gtk_widget_show(label8);
    gtk_box_pack_start(GTK_BOX(hbox17), label8, FALSE, FALSE, 0);

    entry4 = gtk_entry_new();
    gtk_widget_show(entry4);
    gtk_box_pack_start(GTK_BOX(hbox17), entry4, TRUE, TRUE, 0);
    gtk_editable_set_editable(GTK_EDITABLE(entry4), FALSE);
    gtk_entry_set_text(GTK_ENTRY(entry4), _("86-571-88075998-8824"));
    gtk_entry_set_has_frame(GTK_ENTRY(entry4), FALSE);

    dialog_action_area1 = GTK_DIALOG(dialog1)->action_area;
    gtk_widget_show(dialog_action_area1);

    closebutton1 = gtk_button_new_with_mnemonic(_("Close"));
    gtk_widget_show(closebutton1);
    gtk_dialog_add_action_widget(GTK_DIALOG(dialog1), closebutton1, GTK_RESPONSE_CLOSE);
    GTK_WIDGET_SET_FLAGS(closebutton1, GTK_CAN_DEFAULT);

    g_signal_connect((gpointer) closebutton1, "clicked", G_CALLBACK(on_closebutton1_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(dialog1, dialog1, "dialog1");
    GLADE_HOOKUP_OBJECT_NO_REF(dialog1, w_about, "w_about");
    GLADE_HOOKUP_OBJECT(dialog1, vbox4, "vbox4");
    GLADE_HOOKUP_OBJECT(dialog1, vbox5, "vbox5");
    GLADE_HOOKUP_OBJECT(dialog1, hbox15, "hbox15");
    GLADE_HOOKUP_OBJECT(dialog1, label6, "label6");
    GLADE_HOOKUP_OBJECT(dialog1, w_sdk_version, "w_sdk_version");
    GLADE_HOOKUP_OBJECT(dialog1, hbox16, "hbox16");
    GLADE_HOOKUP_OBJECT(dialog1, label7, "label7");
    GLADE_HOOKUP_OBJECT(dialog1, entry3, "entry3");
    GLADE_HOOKUP_OBJECT(dialog1, hbox17, "hbox17");
    GLADE_HOOKUP_OBJECT(dialog1, label8, "label8");
    GLADE_HOOKUP_OBJECT(dialog1, entry4, "entry4");
    GLADE_HOOKUP_OBJECT_NO_REF(dialog1, dialog_action_area1, "dialog_action_area1");
    GLADE_HOOKUP_OBJECT(dialog1, closebutton1, "closebutton1");

    return dialog1;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
GtkWidget* create_dialog2(void)
{
    GtkWidget*  dialog2;
    GtkWidget*  dialog_vbox1;
    GtkWidget*  hbox23;
    GtkWidget*  table6;
    GtkWidget*  label15;
    GtkWidget*  entry11;
    GtkWidget*  label17;
    GtkWidget*  w_high;
    GtkWidget*  label18;
    GtkWidget*  entry13;
    GtkWidget*  label19;
    GtkWidget*  w_set_index_file;
    GtkWidget*  table7;
    GtkWidget*  label16;
    GtkWidget*  entry12;
    GtkWidget*  w_low;
    GtkWidget*  label28;
    GtkWidget*  entry14;
    GtkWidget*  dialog_action_area2;
    GtkWidget*  closebutton2;

    dialog2 = gtk_dialog_new();
    gtk_widget_set_size_request(dialog2, -1, 150);
    gtk_window_set_title(GTK_WINDOW(dialog2), _("File Information"));
    gtk_window_set_resizable(GTK_WINDOW(dialog2), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(dialog2), GDK_WINDOW_TYPE_HINT_DIALOG);

    dialog_vbox1 = GTK_DIALOG(dialog2)->vbox;
    gtk_widget_show(dialog_vbox1);
    gtk_widget_set_size_request(dialog_vbox1, 400, 200);

    hbox23 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox23);
    gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox23, TRUE, TRUE, 0);

    table6 = gtk_table_new(4, 2, FALSE);
    gtk_widget_show(table6);
    gtk_box_pack_start(GTK_BOX(hbox23), table6, FALSE, FALSE, 0);
    gtk_widget_set_size_request(table6, -1, 50);

    label15 = gtk_label_new(_("       Frame Rate"));
    gtk_widget_show(label15);
    gtk_table_attach(GTK_TABLE(table6), label15, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label15), 0, 0.5);

    entry11 = gtk_entry_new();
    gtk_widget_show(entry11);
    gtk_table_attach(GTK_TABLE(table6), entry11, 1, 2, 0, 1,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(entry11, 70, -1);
    gtk_entry_set_text(GTK_ENTRY(entry11), _("    0"));

    label17 = gtk_label_new(_("       Picture Quality"));
    gtk_widget_show(label17);
    gtk_table_attach(GTK_TABLE(table6), label17, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label17), 0, 0.5);

    w_high = gtk_check_button_new_with_mnemonic(_("high"));
    gtk_widget_show(w_high);
    gtk_table_attach(GTK_TABLE(table6), w_high, 1, 2, 1, 2, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);

    label18 = gtk_label_new(_("       File Head"));
    gtk_widget_show(label18);
    gtk_table_attach(GTK_TABLE(table6), label18, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label18), 0, 0.5);

    entry13 = gtk_entry_new();
    gtk_widget_show(entry13);
    gtk_table_attach(GTK_TABLE(table6), entry13, 1, 2, 2, 3,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(entry13, 70, -1);
    gtk_entry_set_text(GTK_ENTRY(entry13), _("    0"));

    label19 = gtk_label_new(_("      Set Index File"));
    gtk_widget_show(label19);
    gtk_table_attach(GTK_TABLE(table6), label19, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);

    gtk_misc_set_alignment(GTK_MISC(label19), 0, 0.5);
    gtk_widget_hide(label19);

    w_set_index_file = gtk_check_button_new_with_mnemonic("");
    gtk_widget_show(w_set_index_file);
    gtk_table_attach(GTK_TABLE(table6), w_set_index_file, 1, 2, 3, 4, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);
    gtk_widget_hide(w_set_index_file);

    table7 = gtk_table_new(3, 2, FALSE);
    gtk_widget_show(table7);
    gtk_box_pack_start(GTK_BOX(hbox23), table7, FALSE, FALSE, 0);

    label16 = gtk_label_new(_("      Decoded Frame"));
    gtk_widget_show(label16);
    gtk_table_attach(GTK_TABLE(table7), label16, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label16), 0, 0.5);

    entry12 = gtk_entry_new();
    gtk_widget_show(entry12);
    gtk_table_attach(GTK_TABLE(table7), entry12, 1, 2, 0, 1,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(entry12, 70, -1);
    gtk_entry_set_text(GTK_ENTRY(entry12), _("     0"));

    w_low = gtk_check_button_new_with_mnemonic(_(" low"));
    gtk_widget_show(w_low);
    gtk_table_attach(GTK_TABLE(table7), w_low, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w_low), TRUE);

    label28 = gtk_label_new(_("Abs Frame Num"));
    gtk_widget_show(label28);
    gtk_table_attach(GTK_TABLE(table7), label28, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL),
                     (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC(label28), 0, 0.5);

    entry14 = gtk_entry_new();
    gtk_widget_show(entry14);
    gtk_table_attach(GTK_TABLE(table7), entry14, 1, 2, 2, 3,
                     (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text(GTK_ENTRY(entry14), _("     0"));

    dialog_action_area2 = GTK_DIALOG(dialog2)->action_area;
    gtk_widget_show(dialog_action_area2);
    gtk_widget_set_size_request(dialog_action_area2, 100, -1);

    closebutton2 = gtk_button_new_with_mnemonic("Close");
    gtk_widget_show(closebutton2);
    gtk_dialog_add_action_widget(GTK_DIALOG(dialog2), closebutton2, GTK_RESPONSE_CLOSE);
    GTK_WIDGET_SET_FLAGS(closebutton2, GTK_CAN_DEFAULT);

    g_signal_connect((gpointer) w_set_index_file, "clicked",
                     G_CALLBACK(on_w_set_index_file_clicked), NULL);
    g_signal_connect((gpointer) closebutton2, "clicked", G_CALLBACK(on_closebutton2_clicked), NULL);
    g_signal_connect((gpointer) closebutton2, "clicked", G_CALLBACK(on_closebutton2_clicked), NULL);
    g_signal_connect((gpointer) w_low, "clicked", G_CALLBACK(set_picture_quality_callback), NULL);
    g_signal_connect((gpointer) w_high, "clicked", G_CALLBACK(set_picture_quality_callback),
                     (gpointer*)1);
    g_signal_connect((gpointer) w_set_index_file, "clicked",
                     G_CALLBACK(on_w_set_index_file_clicked), NULL);
    g_signal_connect((gpointer) dialog2, "destroy_event", G_CALLBACK(on_closebutton2_clicked), NULL);
    g_signal_connect((gpointer) dialog2, "delete_event", G_CALLBACK(on_closebutton2_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(dialog2, dialog2, "dialog2");
    GLADE_HOOKUP_OBJECT_NO_REF(dialog2, dialog_vbox1, "dialog_vbox1");
    GLADE_HOOKUP_OBJECT(dialog2, hbox23, "hbox23");
    GLADE_HOOKUP_OBJECT(dialog2, table6, "table6");
    GLADE_HOOKUP_OBJECT(dialog2, label15, "label15");
    GLADE_HOOKUP_OBJECT(dialog2, entry11, "entry11");
    GLADE_HOOKUP_OBJECT(dialog2, label17, "label17");
    GLADE_HOOKUP_OBJECT(dialog2, w_high, "w_high");
    GLADE_HOOKUP_OBJECT(dialog2, label18, "label18");
    GLADE_HOOKUP_OBJECT(dialog2, entry13, "entry13");
    GLADE_HOOKUP_OBJECT(dialog2, label19, "label19");
    GLADE_HOOKUP_OBJECT(dialog2, w_set_index_file, "w_set_index_file");
    GLADE_HOOKUP_OBJECT(dialog2, table7, "table7");
    GLADE_HOOKUP_OBJECT(dialog2, label16, "label16");
    GLADE_HOOKUP_OBJECT(dialog2, entry12, "entry12");
    GLADE_HOOKUP_OBJECT(dialog2, w_low, "w_low");
    GLADE_HOOKUP_OBJECT(dialog2, label28, "label28");
    GLADE_HOOKUP_OBJECT(dialog2, entry14, "entry14");
    GLADE_HOOKUP_OBJECT_NO_REF(dialog2, dialog_action_area2, "dialog_action_area2");
    GLADE_HOOKUP_OBJECT(dialog2, closebutton2, "closebutton2");

    return dialog2;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
GtkWidget* create_dialog3(void)
{
    GtkWidget*  dialog3;
    GtkWidget*  dialog_vbox2;
    GtkWidget*  vbox6;
    GtkWidget*  hbox24;
    GtkWidget*  w_by_num;
    GSList*     w_by_num_group = NULL;
    GtkWidget*  w_by_time;
    GtkWidget*  w_use_progress;
    GtkWidget*  frame1;
    GtkWidget*  alignment1;
    GtkWidget*  vbox7;
    GtkWidget*  hbox25;
    GtkWidget*  label21;
    GtkWidget*  w_frame_num;
    GtkWidget*  hbox26;
    GtkWidget*  label22;
    GtkWidget*  w_frame_time;
    GtkWidget*  label20;
    GtkWidget*  frame2;
    GtkWidget*  alignment2;
    GtkWidget*  vbox8;
    GtkWidget*  hbox28;
    GtkWidget*  label25;
    GtkWidget*  w_begin;
    GtkWidget*  w_ok_begin;
    GtkWidget*  hbox29;
    GtkWidget*  label26;
    GtkWidget*  w_end;
    GtkWidget*  w_ok_end;
    GtkWidget*  hbox30;
    GtkWidget*  label27;
    GtkWidget*  w_save_file;
    GtkWidget*  label24;
    GtkWidget*  dialog_action_area3;
    GtkWidget*  cancelbutton1;
    GtkWidget*  okbutton1;

    dialog3 = gtk_dialog_new();
    gtk_widget_set_size_request(dialog3, 350, 300);
    gtk_window_set_title(GTK_WINDOW(dialog3), _("Cut File"));
    gtk_window_set_resizable(GTK_WINDOW(dialog3), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(dialog3), GDK_WINDOW_TYPE_HINT_DIALOG);

    dialog_vbox2 = GTK_DIALOG(dialog3)->vbox;
    gtk_widget_show(dialog_vbox2);

    vbox6 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox6);
    gtk_box_pack_start(GTK_BOX(dialog_vbox2), vbox6, TRUE, TRUE, 0);

    hbox24 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox24);
    gtk_box_pack_start(GTK_BOX(vbox6), hbox24, FALSE, FALSE, 0);

    w_by_num = gtk_radio_button_new_with_mnemonic(NULL, _("BY FRAMENUM"));
    gtk_widget_show(w_by_num);
    gtk_box_pack_start(GTK_BOX(hbox24), w_by_num, TRUE, TRUE, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(w_by_num), w_by_num_group);
    w_by_num_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(w_by_num));

    w_by_time = gtk_radio_button_new_with_mnemonic(NULL, _("BY FRAMETIME"));
    gtk_widget_show(w_by_time);
    gtk_box_pack_start(GTK_BOX(hbox24), w_by_time, TRUE, TRUE, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(w_by_time), w_by_num_group);
    w_by_num_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(w_by_time));

    w_use_progress = gtk_check_button_new_with_mnemonic(_("Use Progress"));
    gtk_widget_show(w_use_progress);
    gtk_box_pack_start(GTK_BOX(hbox24), w_use_progress, FALSE, FALSE, 0);

    frame1 = gtk_frame_new(NULL);
    gtk_widget_show(frame1);
    gtk_box_pack_start(GTK_BOX(vbox6), frame1, FALSE, TRUE, 0);
    gtk_widget_set_size_request(frame1, 116, -1);
    gtk_frame_set_shadow_type(GTK_FRAME(frame1), GTK_SHADOW_NONE);

    alignment1 = gtk_alignment_new(0.5, 0.5, 1, 1);
    gtk_widget_show(alignment1);
    gtk_container_add(GTK_CONTAINER(frame1), alignment1);

    /*
     * gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);
     */
    vbox7 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox7);
    gtk_container_add(GTK_CONTAINER(alignment1), vbox7);
    gtk_widget_set_size_request(vbox7, 100, 50);

    hbox25 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox25);
    gtk_box_pack_start(GTK_BOX(vbox7), hbox25, FALSE, TRUE, 0);
    gtk_widget_set_size_request(hbox25, -1, 25);

    label21 = gtk_label_new(_("Frame Num"));
    gtk_widget_show(label21);
    gtk_box_pack_start(GTK_BOX(hbox25), label21, FALSE, FALSE, 0);

    w_frame_num = gtk_entry_new();
    gtk_widget_show(w_frame_num);
    gtk_box_pack_start(GTK_BOX(hbox25), w_frame_num, FALSE, TRUE, 0);
    gtk_widget_set_size_request(w_frame_num, 100, -1);
    gtk_editable_set_editable(GTK_EDITABLE(w_frame_num), FALSE);
    gtk_entry_set_has_frame(GTK_ENTRY(w_frame_num), FALSE);

    hbox26 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox26);
    gtk_box_pack_start(GTK_BOX(vbox7), hbox26, FALSE, TRUE, 0);

    label22 = gtk_label_new(_("Frame Time"));
    gtk_widget_show(label22);
    gtk_box_pack_start(GTK_BOX(hbox26), label22, FALSE, FALSE, 0);

    w_frame_time = gtk_entry_new();
    gtk_widget_show(w_frame_time);
    gtk_box_pack_start(GTK_BOX(hbox26), w_frame_time, FALSE, TRUE, 0);
    gtk_widget_set_size_request(w_frame_time, 100, -1);
    gtk_editable_set_editable(GTK_EDITABLE(w_frame_time), FALSE);
    gtk_entry_set_has_frame(GTK_ENTRY(w_frame_time), FALSE);

    label20 = gtk_label_new(_("<b>Frame Range</b>"));
    gtk_widget_show(label20);
    gtk_frame_set_label_widget(GTK_FRAME(frame1), label20);
    gtk_label_set_use_markup(GTK_LABEL(label20), TRUE);

    frame2 = gtk_frame_new(NULL);
    gtk_widget_show(frame2);
    gtk_box_pack_start(GTK_BOX(vbox6), frame2, FALSE, TRUE, 0);
    gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_NONE);

    alignment2 = gtk_alignment_new(0.5, 0.5, 1, 1);
    gtk_widget_show(alignment2);
    gtk_container_add(GTK_CONTAINER(frame2), alignment2);

    /*
     * gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);
     */
    vbox8 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox8);
    gtk_container_add(GTK_CONTAINER(alignment2), vbox8);

    hbox28 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox28);
    gtk_box_pack_start(GTK_BOX(vbox8), hbox28, FALSE, TRUE, 0);

    label25 = gtk_label_new(_("Begin Position"));
    gtk_widget_show(label25);
    gtk_box_pack_start(GTK_BOX(hbox28), label25, FALSE, FALSE, 0);

    w_begin = gtk_entry_new();
    gtk_widget_show(w_begin);
    gtk_box_pack_start(GTK_BOX(hbox28), w_begin, FALSE, TRUE, 0);
    gtk_widget_set_size_request(w_begin, 70, -1);
    gtk_entry_set_text(GTK_ENTRY(w_begin), "   0");

    w_ok_begin = gtk_check_button_new_with_mnemonic(_("Set begin__pos value ok "));
    gtk_widget_show(w_ok_begin);
    gtk_box_pack_start(GTK_BOX(hbox28), w_ok_begin, FALSE, FALSE, 0);

    hbox29 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox29);
    gtk_box_pack_start(GTK_BOX(vbox8), hbox29, FALSE, TRUE, 0);

    label26 = gtk_label_new(_("End  Position "));
    gtk_widget_show(label26);
    gtk_box_pack_start(GTK_BOX(hbox29), label26, FALSE, FALSE, 0);

    w_end = gtk_entry_new();
    gtk_widget_show(w_end);
    gtk_box_pack_start(GTK_BOX(hbox29), w_end, FALSE, TRUE, 0);
    gtk_widget_set_size_request(w_end, 70, -1);
    gtk_entry_set_text(GTK_ENTRY(w_end), "   0");

    w_ok_end = gtk_check_button_new_with_mnemonic(_("Set end__pos value ok"));
    gtk_widget_show(w_ok_end);
    gtk_box_pack_start(GTK_BOX(hbox29), w_ok_end, FALSE, FALSE, 0);

    hbox30 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox30);
    gtk_box_pack_start(GTK_BOX(vbox8), hbox30, FALSE, TRUE, 0);

    label27 = gtk_label_new(_("File Save To  "));
    gtk_widget_show(label27);
    gtk_box_pack_start(GTK_BOX(hbox30), label27, FALSE, FALSE, 0);

    w_save_file = gtk_entry_new();
    gtk_widget_show(w_save_file);
    gtk_box_pack_start(GTK_BOX(hbox30), w_save_file, TRUE, TRUE, 0);
    gtk_entry_set_text(GTK_ENTRY(w_save_file), "./clip.mp4");

    label24 = gtk_label_new(_("<b>Cut File</b>"));
    gtk_widget_show(label24);
    gtk_frame_set_label_widget(GTK_FRAME(frame2), label24);
    gtk_label_set_use_markup(GTK_LABEL(label24), TRUE);

    dialog_action_area3 = GTK_DIALOG(dialog3)->action_area;
    gtk_widget_show(dialog_action_area3);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area3), GTK_BUTTONBOX_SPREAD);

    cancelbutton1 = gtk_button_new_with_mnemonic(_("cancel"));
    gtk_widget_show(cancelbutton1);
    gtk_dialog_add_action_widget(GTK_DIALOG(dialog3), cancelbutton1, GTK_RESPONSE_CANCEL);
    GTK_WIDGET_SET_FLAGS(cancelbutton1, GTK_CAN_DEFAULT);

    okbutton1 = gtk_button_new_with_mnemonic(_("ok"));
    gtk_widget_show(okbutton1);
    gtk_dialog_add_action_widget(GTK_DIALOG(dialog3), okbutton1, GTK_RESPONSE_OK);
    GTK_WIDGET_SET_FLAGS(okbutton1, GTK_CAN_DEFAULT);

    g_signal_connect((gpointer) w_use_progress, "clicked", G_CALLBACK(on_w_use_progress_clicked),
                     NULL);
    g_signal_connect((gpointer) w_ok_begin, "clicked", G_CALLBACK(on_w_ok_begin_clicked), NULL);
    g_signal_connect((gpointer) w_ok_end, "clicked", G_CALLBACK(on_w_ok_end_clicked), NULL);
    g_signal_connect((gpointer) cancelbutton1, "clicked", G_CALLBACK(on_cancelbutton1_clicked), NULL);
    g_signal_connect((gpointer) okbutton1, "clicked", G_CALLBACK(on_okbutton1_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(dialog3, dialog3, "dialog3");
    GLADE_HOOKUP_OBJECT_NO_REF(dialog3, dialog_vbox2, "dialog_vbox2");
    GLADE_HOOKUP_OBJECT(dialog3, vbox6, "vbox6");
    GLADE_HOOKUP_OBJECT(dialog3, hbox24, "hbox24");
    GLADE_HOOKUP_OBJECT(dialog3, w_by_num, "w_by_num");
    GLADE_HOOKUP_OBJECT(dialog3, w_by_time, "w_by_time");
    GLADE_HOOKUP_OBJECT(dialog3, w_use_progress, "w_use_progress");
    GLADE_HOOKUP_OBJECT(dialog3, frame1, "frame1");
    GLADE_HOOKUP_OBJECT(dialog3, alignment1, "alignment1");
    GLADE_HOOKUP_OBJECT(dialog3, vbox7, "vbox7");
    GLADE_HOOKUP_OBJECT(dialog3, hbox25, "hbox25");
    GLADE_HOOKUP_OBJECT(dialog3, label21, "label21");
    GLADE_HOOKUP_OBJECT(dialog3, w_frame_num, "w_frame_num");
    GLADE_HOOKUP_OBJECT(dialog3, hbox26, "hbox26");
    GLADE_HOOKUP_OBJECT(dialog3, label22, "label22");
    GLADE_HOOKUP_OBJECT(dialog3, w_frame_time, "w_frame_time");
    GLADE_HOOKUP_OBJECT(dialog3, label20, "label20");
    GLADE_HOOKUP_OBJECT(dialog3, frame2, "frame2");
    GLADE_HOOKUP_OBJECT(dialog3, alignment2, "alignment2");
    GLADE_HOOKUP_OBJECT(dialog3, vbox8, "vbox8");
    GLADE_HOOKUP_OBJECT(dialog3, hbox28, "hbox28");
    GLADE_HOOKUP_OBJECT(dialog3, label25, "label25");
    GLADE_HOOKUP_OBJECT(dialog3, w_begin, "w_begin");
    GLADE_HOOKUP_OBJECT(dialog3, w_ok_begin, "w_ok_begin");
    GLADE_HOOKUP_OBJECT(dialog3, hbox29, "hbox29");
    GLADE_HOOKUP_OBJECT(dialog3, label26, "label26");
    GLADE_HOOKUP_OBJECT(dialog3, w_end, "w_end");
    GLADE_HOOKUP_OBJECT(dialog3, w_ok_end, "w_ok_end");
    GLADE_HOOKUP_OBJECT(dialog3, hbox30, "hbox30");
    GLADE_HOOKUP_OBJECT(dialog3, label27, "label27");
    GLADE_HOOKUP_OBJECT(dialog3, w_save_file, "w_save_file");
    GLADE_HOOKUP_OBJECT(dialog3, label24, "label24");
    GLADE_HOOKUP_OBJECT_NO_REF(dialog3, dialog_action_area3, "dialog_action_area3");
    GLADE_HOOKUP_OBJECT(dialog3, cancelbutton1, "cancelbutton1");
    GLADE_HOOKUP_OBJECT(dialog3, okbutton1, "okbutton1");

    return dialog3;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
GtkWidget* create_fileselection2(void)
{
    GtkWidget*  fileselection2;
    GtkWidget*  ok_button2;
    GtkWidget*  cancel_button2;

    fileselection2 = gtk_file_selection_new(_("\351\200\211\346\213\251\346\226\207\344\273\266"));
    gtk_container_set_border_width(GTK_CONTAINER(fileselection2), 10);
    gtk_window_set_type_hint(GTK_WINDOW(fileselection2), GDK_WINDOW_TYPE_HINT_DIALOG);

    ok_button2 = GTK_FILE_SELECTION(fileselection2)->ok_button;
    gtk_widget_show(ok_button2);
    GTK_WIDGET_SET_FLAGS(ok_button2, GTK_CAN_DEFAULT);

    cancel_button2 = GTK_FILE_SELECTION(fileselection2)->cancel_button;
    gtk_widget_show(cancel_button2);
    GTK_WIDGET_SET_FLAGS(cancel_button2, GTK_CAN_DEFAULT);

    g_signal_connect((gpointer) ok_button2, "clicked", G_CALLBACK(on_button2_clicked), NULL);
    g_signal_connect((gpointer) cancel_button2, "clicked", G_CALLBACK(on_button1_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(fileselection2, fileselection2, "fileselection2");
    GLADE_HOOKUP_OBJECT_NO_REF(fileselection2, ok_button2, "ok_button2");
    GLADE_HOOKUP_OBJECT_NO_REF(fileselection2, cancel_button2, "cancel_button2");

    return fileselection2;
}
