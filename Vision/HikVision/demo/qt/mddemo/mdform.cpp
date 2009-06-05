

#include "mdform.h"
#include "QSDLWidget.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qfiledialog.h>

/*
 * Constructs a mdForm as a child of 'parent', with the name 'name' and widget
 * flags set to 'f'.
 */
mdForm::mdForm(QWidget * parent, const char *name, WFlags fl) :
    QWidget(parent, name, fl)
{
    QDateTime   Dt = QDateTime::currentDateTime();
    QString     Qs, Qstemp;

    Qs.fill(' ', 20);

    if(!name)
        setName("mdForm");
    setMinimumSize(QSize(1024, 768));
    setMaximumSize(QSize(1024, 768));
    setBaseSize(QSize(1024, 768));

    w_full_area = new QSDLWidget(this, "w_full_area");
    w_full_area->setGeometry(QRect(0, 25, 800, 700));
    w_full_area->setPaletteBackgroundColor(QColor(0, 0, 0));
    w_full_area->setPaletteForegroundColor(QColor(0, 255, 0));

    TitleEdit = new QLineEdit(this, "TitleEdit");
    TitleEdit->setEnabled(TRUE);
    TitleEdit->setGeometry(QRect(0, 0, 1024, 25));
    TitleEdit->setPaletteForegroundColor(QColor(255, 255, 255));
    TitleEdit->setPaletteBackgroundColor(QColor(0, 0, 255));
    QFont   TitleEdit_font(TitleEdit->font());
    TitleEdit_font.setFamily("Courier");
    TitleEdit_font.setBold(TRUE);
    TitleEdit->setFont(TitleEdit_font);
    TitleEdit->setReadOnly(TRUE);

    frameTotal = new QFrame(this, "frameTotal");
    frameTotal->setGeometry(QRect(800, 25, 224, 700));
    frameTotal->setFrameShape(QFrame::StyledPanel);
    frameTotal->setFrameShadow(QFrame::Raised);

    groupBoxGSI = new QGroupBox(frameTotal, "groupBoxGSI");
    groupBoxGSI->setGeometry(QRect(2, 5, 220, 125));
    QFont   groupBoxGSI_font(groupBoxGSI->font());
    groupBoxGSI_font.setFamily("Courier");
    groupBoxGSI->setFont(groupBoxGSI_font);

    DateLabel = new QLabel(groupBoxGSI, "DateLabel");
    DateLabel->setGeometry(QRect(9, 20, 70, 20));
    QFont   DateLabel_font(DateLabel->font());
    DateLabel->setFont(DateLabel_font);

    TimeLabel = new QLabel(groupBoxGSI, "TimeLabel");
    TimeLabel->setGeometry(QRect(9, 55, 60, 20));
    QFont   TimeLabel_font(TimeLabel->font());
    TimeLabel->setFont(TimeLabel_font);

    TimeEdit = new QLineEdit(groupBoxGSI, "TimeEdit");
    TimeEdit->setGeometry(QRect(100, 55, 110, 25));

    FreeSpaceLabel = new QLabel(groupBoxGSI, "FreeSpaceLabel");
    FreeSpaceLabel->setGeometry(QRect(5, 90, 88, 20));
    QFont   FreeSpaceLabel_font(FreeSpaceLabel->font());
    FreeSpaceLabel->setFont(FreeSpaceLabel_font);

    FreespaceEdit = new QLineEdit(groupBoxGSI, "FreespaceEdit");
    FreespaceEdit->setGeometry(QRect(100, 90, 110, 25));

    DateEdit = new QLineEdit(groupBoxGSI, "DateEdit");
    DateEdit->setGeometry(QRect(100, 20, 110, 25));

    groupBoxCInfo = new QGroupBox(frameTotal, "groupBoxCInfo");
    groupBoxCInfo->setGeometry(QRect(2, 145, 220, 265));
    QFont   groupBoxCInfo_font(groupBoxCInfo->font());
    groupBoxCInfo_font.setFamily("Courier");
    groupBoxCInfo->setFont(groupBoxCInfo_font);

    w_decode_framerate = new QLineEdit(groupBoxCInfo, "w_decode_framerate");
    w_decode_framerate->setGeometry(QRect(100, 160, 110, 25));
    w_decode_framerate->setText("0");
    w_decode_framerate->setReadOnly(TRUE);

    w_set_serverip = new QLineEdit(groupBoxCInfo, "w_set_serverip");
    w_set_serverip->setGeometry(QRect(100, 55, 110, 25));
    w_set_serverip->setText(Qstemp.sprintf("%d.%d.%d.%d", address[0], address[1], address[2], address[3]));

    TotalFramesLabel = new QLabel(groupBoxCInfo, "TotalFramesLabel");
    TotalFramesLabel->setGeometry(QRect(2, 196, 104, 20));
    QFont   TotalFramesLabel_font(TotalFramesLabel->font());
    TotalFramesLabel->setFont(TotalFramesLabel_font);

    w_totalframe = new QLineEdit(groupBoxCInfo, "w_totalframe");
    w_totalframe->setGeometry(QRect(100, 195, 110, 25));
    w_totalframe->setReadOnly(TRUE);
    w_totalframe->setText("0");

    FrameRateLabel = new QLabel(groupBoxCInfo, "FrameRateLabel");
    FrameRateLabel->setGeometry(QRect(14, 160, 80, 20));
    FrameRateLabel->setBackgroundOrigin(QLabel::ParentOrigin);
    QFont   FrameRateLabel_font(FrameRateLabel->font());
    FrameRateLabel->setFont(FrameRateLabel_font);

    BitRateLabel = new QLabel(groupBoxCInfo, "BitRateLabel");
    BitRateLabel->setGeometry(QRect(4, 126, 104, 20));
    QFont   BitRateLabel_font(BitRateLabel->font());
    BitRateLabel->setFont(BitRateLabel_font);

    ServerChanLabel = new QLabel(groupBoxCInfo, "ServerChanLabel");
    ServerChanLabel->setGeometry(QRect(4, 90, 101, 20));
    QFont   ServerChanLabel_font(ServerChanLabel->font());
    ServerChanLabel->setFont(ServerChanLabel_font);

    ServerIpLabel = new QLabel(groupBoxCInfo, "ServerIpLabel");
    ServerIpLabel->setGeometry(QRect(11, 55, 80, 20));
    QFont   ServerIpLabel_font(ServerIpLabel->font());
    ServerIpLabel->setFont(ServerIpLabel_font);

    ChannelNumLabel = new QLabel(groupBoxCInfo, "ChannelNumLabel");
    ChannelNumLabel->setGeometry(QRect(7, 20, 96, 20));
    QFont   ChannelNumLabel_font(ChannelNumLabel->font());
    ChannelNumLabel->setFont(ChannelNumLabel_font);

    FilePositionLabel = new QLabel(groupBoxCInfo, "FilePositionLabel");
    FilePositionLabel->setGeometry(QRect(10, 230, 96, 20));
    QFont   FilePositionLabel_font(FilePositionLabel->font());
    FilePositionLabel->setFont(FilePositionLabel_font);

    w_fileposition = new QLineEdit(groupBoxCInfo, "w_fileposition");
    w_fileposition->setGeometry(QRect(100, 230, 110, 25));
    w_fileposition->setReadOnly(TRUE);

    w_set_serverchan = new QLineEdit(groupBoxCInfo, "w_set_serverchan");
    w_set_serverchan->setGeometry(QRect(100, 90, 110, 25));
    w_set_serverchan->setText("0");

    w_bit_rate = new QLineEdit(groupBoxCInfo, "w_bit_rate");
    w_bit_rate->setGeometry(QRect(100, 125, 110, 25));
    w_bit_rate->setReadOnly(TRUE);
    w_bit_rate->setText("0");

    w_channel = new QLineEdit(groupBoxCInfo, "w_channel");
    w_channel->setGeometry(QRect(100, 20, 110, 25));
    w_channel->setReadOnly(TRUE);
    w_channel->setText("0");

    groupBoxDecode = new QGroupBox(frameTotal, "groupBoxDecode");
    groupBoxDecode->setGeometry(QRect(2, 425, 220, 180));
    QFont   groupBoxDecode_font(groupBoxDecode->font());
    groupBoxDecode_font.setFamily("Courier");
    groupBoxDecode->setFont(groupBoxDecode_font);

    AllSetCheckButton = new QCheckBox(groupBoxDecode, "AllSetCheckButton");
    AllSetCheckButton->setGeometry(QRect(9, 25, 86, 20));

    w_decodefile = new QCheckBox(groupBoxDecode, "w_decodefile");
    w_decodefile->setGeometry(QRect(28, 55, 150, 20));

    w_decodestream = new QCheckBox(groupBoxDecode, "w_decodestream");
    w_decodestream->setGeometry(QRect(28, 85, 170, 20));

    w_videoout = new QCheckBox(groupBoxDecode, "w_videoout");
    w_videoout->setEnabled(FALSE);
    w_videoout->setGeometry(QRect(28, 115, 190, 20));

    w_sound = new QCheckBox(groupBoxDecode, "w_sound");
    w_sound->setEnabled(FALSE);
    w_sound->setGeometry(QRect(9, 145, 150, 20));

    ButtonExit = new QPushButton(frameTotal, "ButtonExit");
    ButtonExit->setGeometry(QRect(28, 645, 150, 26));
    QFont   ButtonExit_font(ButtonExit->font());
    ButtonExit_font.setFamily("Courier");
    ButtonExit->setFont(ButtonExit_font);

    dstimer = new QTimer(this);
    connect(dstimer, SIGNAL(timeout()), this, SLOT(changeEdit()));
    dstimer->start(200);    /* 200ms */

    languageChange();
    resize(QSize(1024, 768).expandedTo(minimumSizeHint()));
    show();
    w_full_area->SetEnv();

    TitleEdit->setText
        (
            Qstemp.sprintf
                ("DS-400XMD demo: There have %ld decode channels", w_full_area->RetTotalPort()) +
                    Qs +
                    QString("Start time: ") +
                    Dt.date().toString(" yyyy-MM-dd ") +
                    Dt.time().toString(" hh:mm:ss ")
        );

    connect(ButtonExit, SIGNAL(clicked()), this, SLOT(close()));
    connect(w_decodefile, SIGNAL(clicked()), this, SLOT(OpenFile()));
    connect(w_decodestream, SIGNAL(clicked()), this, SLOT(OpenStream()));
    connect(w_set_serverip, SIGNAL(returnPressed()), this, SLOT(ServerIPSet()));
    connect(w_set_serverchan, SIGNAL(returnPressed()), this, SLOT(ServerChanSet()));
    connect(AllSetCheckButton, SIGNAL(clicked()), w_full_area, SLOT(SameSetting()));
    connect(w_full_area, SIGNAL(portchanged (int)), this, SLOT(AreaChanged (int)));
    connect(w_full_area, SIGNAL(playStatus (int)), this, SLOT(StatusChanged (int)));

    connect(w_full_area, SIGNAL(BitRateChange (int, int)), this, SLOT(BitRateSet (int, int)));
    connect
        (
            w_full_area, SIGNAL(RateChange (int, unsigned long)), this, SLOT
                (FrameRateSet (int, unsigned long))
        );
    connect
        (
            w_full_area, SIGNAL(FrameChange (int, unsigned long)), this, SLOT
                (FrameSet (int, unsigned long))
        );
    connect(w_videoout, SIGNAL(clicked()), this, SLOT(OpenVideoOut()));
    connect(w_sound, SIGNAL(clicked()), this, SLOT(SoundSet()));
}

/*
 =======================================================================================================================
 * Destroys the object and frees any allocated resources
 =======================================================================================================================
 */
mdForm::~mdForm()
{
    delete dstimer;

    /* no need to delete child widgets, Qt does it all for us */
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::SoundSet()
{
    if(w_sound->isChecked() == FALSE)
        w_full_area->StopSound();
    else
    {
        fprintf(stderr, "Ok, open sound for listening\n");
        if(w_full_area->PlaySound())
        {
            disconnect(w_sound, 0, 0, 0);
            w_sound->setChecked(FALSE);
            connect(w_sound, SIGNAL(clicked()), this, SLOT(SoundSet()));
        };
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::ServerIPSet()
{
    w_full_area->IPChange(w_set_serverip->text());
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::ServerChanSet()
{
    QString Qstemp;
    int     chan;
    bool    ok = FALSE;

    Qstemp = w_set_serverchan->text();

    chan = Qstemp.toInt(&ok, 10);

    if((!ok) || chan < 0 || chan > 36)
    {
        OutputErrorString("Wrong ServerChan, %d\n", chan);
        w_full_area->ServerChanSet(0);
        w_set_serverchan->setText("0");
        return;
    }

    w_full_area->ServerChanSet(chan);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::StatusChanged(int status)
{
    QString Qs;
    disconnect(w_decodefile, 0, 0, 0);
    disconnect(w_decodestream, 0, 0, 0);

    if(status == 1)
    {
        w_decodefile->setChecked(TRUE);
        w_decodefile->setEnabled(TRUE);
        w_fileposition->setEnabled(TRUE);
        w_fileposition->setText(w_full_area->RetFilePosition());
        w_decodestream->setChecked(FALSE);
        w_decodestream->setEnabled(FALSE);
        w_set_serverip->setEnabled(FALSE);
        w_set_serverchan->setEnabled(FALSE);
        w_sound->setEnabled(TRUE);
    }
    else if(status == 2)
    {
        w_decodefile->setChecked(FALSE);
        w_decodefile->setEnabled(FALSE);
        w_fileposition->setEnabled(FALSE);
        w_fileposition->clear();
        w_decodestream->setChecked(TRUE);
        w_decodestream->setEnabled(TRUE);
        w_set_serverip->setEnabled(TRUE);
        w_set_serverip->setText(w_full_area->RetIP());
        w_set_serverchan->setEnabled(TRUE);
        w_set_serverchan->setText(Qs.sprintf("%d", w_full_area->RetChan()));
        w_sound->setEnabled(TRUE);
    }
    else if(status == 0)
    {
        w_decodefile->setChecked(FALSE);
        w_decodefile->setEnabled(TRUE);
        w_fileposition->setEnabled(TRUE);
        w_fileposition->clear();
        w_decodestream->setChecked(FALSE);
        w_decodestream->setEnabled(TRUE);
        w_set_serverip->setEnabled(TRUE);
        w_set_serverchan->setEnabled(TRUE);
        w_full_area->ServerChanSet(0);
        w_set_serverchan->setText("0");
        w_sound->setChecked(FALSE);
        w_sound->setEnabled(FALSE);
    }

    connect(w_decodefile, SIGNAL(clicked()), this, SLOT(OpenFile()));
    connect(w_decodestream, SIGNAL(clicked()), this, SLOT(OpenStream()));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::AreaChanged(int port)
{
    QString qS;

    OutputDebugString("now it's turn to channel number\n");
    w_channel->setText(qS.sprintf("%d", port));

    disconnect(w_sound, 0, 0, 0);
    if(port != w_full_area->RetSoundPort())
        w_sound->setChecked(FALSE);
    else
        w_sound->setChecked(TRUE);

    connect(w_sound, SIGNAL(clicked()), this, SLOT(SoundSet()));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::OpenFile()
{
    if(w_decodefile->isChecked() == FALSE)
    {
        OutputDebugString("Close Decode File\n");
        w_decodestream->setEnabled(TRUE);
        w_videoout->setChecked(FALSE);
        w_videoout->setEnabled(FALSE);
        w_full_area->FileStreamClose(DECODEFILE);
        w_fileposition->clear();
        w_set_serverchan->setEnabled(TRUE);
        w_set_serverip->setEnabled(TRUE);
        w_full_area->erase();
        return;
    }

    QString SelectFile;
    OutputDebugString("OpenFile for decode\n");

    QFileDialog*    fd = new QFileDialog(this);
    fd->setMode(QFileDialog::ExistingFile);
    fd->setFilter("HikVision Files(*.mp4 *.264)");
    connect(fd, SIGNAL(fileSelected (const QString&)), w_full_area, SLOT(FileOpen (const QString&)));

    if(fd->exec() == QDialog::Accepted)
    {
        OutputDebugString("Open File for decode\n");
        w_decodestream->setEnabled(FALSE);
        w_fileposition->setText(fd->selectedFile());
        w_set_serverchan->setEnabled(FALSE);
        w_set_serverip->setEnabled(FALSE);
        w_videoout->setEnabled(TRUE);
    }
    else
    {
        disconnect(w_decodefile, 0, 0, 0);
        w_decodefile->setChecked(FALSE);
        connect(w_decodefile, SIGNAL(clicked()), this, SLOT(OpenFile()));
    }

    delete fd;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::OpenStream()
{
    if(w_decodestream->isChecked() == FALSE)
    {
        OutputDebugString("Close Decode Stream\n");
        w_decodefile->setEnabled(TRUE);
        w_videoout->setChecked(FALSE);
        w_videoout->setEnabled(FALSE);
        w_full_area->FileStreamClose(DECODESTREAM);
        w_full_area->erase();
        w_fileposition->setEnabled(TRUE);
        return;
    }

    OutputDebugString("-------------------------\n");
    if(w_full_area->OpenStream() == -1)
    {
        OutputDebugString("here initial the net error\n");
        disconnect(w_decodestream, 0, 0, 0);
        w_decodestream->setChecked(FALSE);
        w_fileposition->setEnabled(TRUE);
        connect(w_decodestream, SIGNAL(clicked()), this, SLOT(OpenStream()));
    }
    else
    {
        w_decodefile->setEnabled(FALSE);
        w_videoout->setEnabled(TRUE);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::OpenVideoOut()
{
    if(w_videoout->isChecked() == FALSE)
    {
        OutputDebugString("Close Video Out\n");
        w_full_area->CloseVideoOut();
        return;
    }

    w_full_area->OpenVideoOut();
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::changeEdit()
{
    char            savepath[10] = ".";
    char            tmp = '%';
    int             freespace;
    struct statfs   bbuf;

    QDateTime       Dtchange = QDateTime::currentDateTime();
    QString         Qschange;

    DateEdit->setText(Dtchange.date().toString("yyyy-MM-dd"));
    TimeEdit->setText(Dtchange.time().toString("hh:mm:ss"));

    if(statfs(savepath, &bbuf) < 0)
    {
        OutputErrorString("statsfs() faied, error: %d\n", errno);
    }
    else
    {
        freespace = (100 * bbuf.f_bavail) / bbuf.f_blocks;
        Qschange.sprintf(" %3d%c", freespace, tmp);
    }

    FreespaceEdit->setText(Qschange);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::BitRateSet(int Select, int data)
{
    QString qS;

    if(Select == atoi(w_channel->text()))
        w_bit_rate->setText(qS.sprintf("%d", data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::FrameRateSet(int Select, unsigned long data)
{
    QString qS;

    if(Select == atoi(w_channel->text()))
        w_decode_framerate->setText(qS.sprintf("%ld", data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void mdForm::FrameSet(int Select, unsigned long data)
{
    QString qS;

    if(Select == atoi(w_channel->text()))
        w_totalframe->setText(qS.sprintf("%ld", data));
}

/*
 =======================================================================================================================
 * Sets the strings of the subwidgets using the current language.
 =======================================================================================================================
 */
void mdForm::languageChange()
{
    setCaption(tr("mdForm"));
    groupBoxGSI->setTitle(tr("Status Information"));
    DateLabel->setText(tr("  Date"));
    TimeLabel->setText(tr("  Time"));
    FreeSpaceLabel->setText(tr("Free Space"));
    groupBoxCInfo->setTitle(tr("Channel Info"));
    TotalFramesLabel->setText(tr("Total Frames"));
    FrameRateLabel->setText(tr("FrameRate"));
    BitRateLabel->setText(tr("BitRate(bps)"));
    ServerChanLabel->setText(tr("Server Chan"));
    ServerIpLabel->setText(tr("Server Ip"));
    ChannelNumLabel->setText(tr("Channel Num"));
    FilePositionLabel->setText(tr("Decode File"));
    groupBoxDecode->setTitle(tr("Decode On MD"));
    AllSetCheckButton->setText(tr("    ALL"));
    w_decodefile->setText(tr("    Decode Files"));
    w_decodestream->setText(tr("    Decode Stream"));
    w_videoout->setText(tr("    Video Audio Out"));
    w_sound->setText(tr("  Audio Preview"));
    ButtonExit->setText(tr("Exit"));
}
