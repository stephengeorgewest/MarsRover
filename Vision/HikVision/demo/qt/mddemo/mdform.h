

#ifndef MDFORM_H
#define MDFORM_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class   QVBoxLayout;
class   QHBoxLayout;
class   QGridLayout;
class   QCheckBox;
class   QFrame;
class   QGroupBox;
class   QLabel;
class   QLineEdit;
class   QPushButton;
class   QSDLWidget;
class   QTimer;

class mdForm : public QWidget
{
    Q_OBJECT

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    mdForm(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~               mdForm();

    QSDLWidget*     w_full_area;
    QLineEdit*      TitleEdit;
    QFrame*         frameTotal;
    QGroupBox*      groupBoxGSI;
    QLabel*         DateLabel;
    QLabel*         TimeLabel;
    QLineEdit*      TimeEdit;
    QLabel*         FreeSpaceLabel;
    QLineEdit*      FreespaceEdit;
    QLineEdit*      DateEdit;
    QGroupBox*      groupBoxCInfo;
    QLineEdit*      w_decode_framerate;
    QLineEdit*      w_set_serverip;
    QLineEdit*      w_fileposition;
    QLabel*         TotalFramesLabel;
    QLineEdit*      w_totalframe;
    QLabel*         FrameRateLabel;
    QLabel*         BitRateLabel;
    QLabel*         ServerChanLabel;
    QLabel*         ServerIpLabel;
    QLabel*         FilePositionLabel;
    QLabel*         ChannelNumLabel;
    QLineEdit*      w_set_serverchan;
    QLineEdit*      w_bit_rate;
    QLineEdit*      w_channel;
    QGroupBox*      groupBoxDecode;
    QCheckBox*      AllSetCheckButton;
    QCheckBox*      w_decodefile;
    QCheckBox*      w_decodestream;
    QCheckBox*      w_videoout;
    QCheckBox*      w_sound;
    QPushButton*    ButtonExit;

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
    public slots : void changeEdit();
    void    OpenFile();
    void    OpenStream();
    void    AreaChanged(int);
    void    StatusChanged(int);
    void    BitRateSet(int, int);
    void    FrameRateSet(int, unsigned long);
    void    FrameSet(int, unsigned long);
    void    ServerIPSet();
    void    ServerChanSet();
    void    OpenVideoOut();
    void    SoundSet();

    protected slots : virtual void languageChange();

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    QPixmap     image0;
    QTimer*     dstimer;
};
#endif /* MDFORM_H */
