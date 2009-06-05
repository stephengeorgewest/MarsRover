

#ifndef DSFORM_H
#define DSFORM_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class   QVBoxLayout;
class   QHBoxLayout;
class   QGridLayout;
class   QCheckBox;
class   QComboBox;
class   QFrame;
class   QGroupBox;
class   QLabel;
class   QLineEdit;
class   QPushButton;
class   QRadioButton;
class   QSDLWidget;
class   QSlider;
class   QTabWidget;
class   QTextEdit;
class   QScrollView;

class dsForm : public QWidget
{
    Q_OBJECT

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    dsForm(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~               dsForm();

    void            AccordDsSet(int index);
    QSDLWidget*     w_full_area;
    QLineEdit*      TitleEdit;
    QTabWidget*     tabWidget3;
    QWidget*        tab;
    QGroupBox*      groupBoxglobal;
    QGroupBox*      groupBoxGCI;
    QLabel*         ChannelNumLabel;
    QLineEdit*      w_channel;
    QCheckBox*      w_preview;
    QCheckBox*      w_record;
    QLabel*         AudioRateLabel;
    QLineEdit*      w_audio_frame;
    QLineEdit*      w_totalframe;
    QLabel*         TotalFrameLabel;
    QLabel*         BitRateLabel;
    QLineEdit*      w_bit_rate;
    QLineEdit*      w_video_frame;
    QLabel*         VideoRateLabel;
    QGroupBox*      groupBoxGNS;
    QFrame*         line1_4_2_2;
    QLabel*         NetCountLabel;
    QLineEdit*      w_netusercount;
    QCheckBox*      w_net_transfer;
    QCheckBox*      w_stringtoclient;
    QPushButton*    ButtonEnableG;
    QPushButton*    w_exit;
    QGroupBox*      groupBoxGSI;
    QLineEdit*      DateEdit;
    QLineEdit*      TimeEdit;
    QLineEdit*      FreespaceEdit;
    QLabel*         FreeSpaceLabel;
    QLabel*         TimeLabel;
    QLabel*         DateLabel;
    QWidget*        tab_2;
    QScrollView*    Qsv;
    QGroupBox*      groupBox7;
    QLabel*         LabelChannelL;
    QGroupBox*      groupBoxOS;
    QFrame*         line1_4_2;
    QCheckBox*      CheckTranslucentL;
    QSlider*        SliderOsdPosXL;
    QLabel*         LabelOsdBrightnessL;
    QSlider*        SliderOsdBrightnessL;
    QLabel*         LabelOsdPosXL;
    QLabel*         LabelOsdPosYL;
    QSlider*        SliderOsdPosYL;
    QCheckBox*      CheckOsdL;
    QGroupBox*      groupBoxCI;
    QRadioButton*   RadioBmpL;
    QFrame*         line1_4_2_2_3;
    QRadioButton*   RadioJpegL;
    QLabel*         LabelPictureQL;
    QSlider*        SliderPictureQL;
    QPushButton*    ButtonCaptureL;
    QGroupBox*      groupBoxIS;
    QFrame*         line1_5_2;
    QCheckBox*      CheckImageCaptureL;
    QComboBox*      CBImageStreamWL;
    QLabel*         LabelImageStreamWL;
    QComboBox*      CBImageStreamHL;
    QLabel*         LabelImageStreamHL;
    QGroupBox*      groupBoxAudio;
    QCheckBox*      CheckAudioL;
    QLineEdit*      EditSoundLevelL;
    QLabel*         LabelSoundLevelL;
    QGroupBox*      groupBoxMS;
    QCheckBox*      CheckMotionDSL;
    QLabel*         LabelMotionSensL;
    QSlider*        SliderMotionSensL;
    QGroupBox*      groupBoxMask;
    QFrame*         line1_4_3;
    QCheckBox*      CheckMaskL;
    QComboBox*      CBRectangleL;
    QLabel*         LabelRectangleL;
    QLabel*         LabelBoundL;
    QLineEdit*      EditBoundL;
    QPushButton*    ButtonNewL;
    QPushButton*    ButtonDeleteL;
    QGroupBox*      groupBoxLogo;
    QFrame*         line1_3_2_2;
    QCheckBox*      CheckLogoL;
    QCheckBox*      CheckLogoTransL;
    QSlider*        SliderLogoPosXL;
    QLabel*         LabelLogoPosXL;
    QLabel*         LabelLogoPosYL;
    QSlider*        SliderLogoPosYL;
    QGroupBox*      groupBoxBC;
    QLabel*         LabelMaxBpsL;
    QComboBox*      CBModeL;
    QLabel*         LabelModeL;
    QLineEdit*      EditMaxBpsL;
    QGroupBox*      groupBoxVI;
    QLabel*         LabelBrightnessL;
    QLabel*         LabelContrastL;
    QSlider*        SliderContrastL;
    QLabel*         LabelSaturationL;
    QSlider*        SliderSaturationL;
    QLabel*         LabelHueL;
    QSlider*        SliderHueL;
    QSlider*        SliderBrightnessL;
    QPushButton*    ButtonVideoRestoreL;
    QComboBox*      CBChannelL;
    QGroupBox*      groupBox9;
    QCheckBox*      CheckMinorRecordL;
    QCheckBox*      CheckMajorRecordL;
    QCheckBox*      CheckPreviewLl;
    QGroupBox*      groupBoxCD;
    QLabel*         LabelMajorStreamL;
    QComboBox*      CBMajorStreamL;
    QLabel*         LabelMinorStreamL;
    QComboBox*      CBMinorStreamL;
    QGroupBox*      groupBoxVQ;
    QLabel*         LabelIQuantL;
    QLineEdit*      EditIQuantL;
    QLineEdit*      EditBQuantL;
    QLabel*         LabelBQuantL;
    QLineEdit*      EditPQuantL;
    QLabel*         LabelPQuantL;
    QGroupBox*      groupBoxFS;
    QLabel*         LabelKeyFrameL;
    QLineEdit*      EditBFramesL;
    QLabel*         LabelBFramesL;
    QLineEdit*      EditPFramesL;
    QLabel*         LabelPFramesL;
    QLabel*         LabelFramesRL;
    QLineEdit*      EditFramesRL;
    QLineEdit*      EditKeyFrameL;
    QGroupBox*      groupBoxPF;
    QComboBox*      CBMinorEncoderL;
    QLabel*         LabelMinorEncoderL;
    QComboBox*      CBMajorEncoderL;
    QLabel*         LabelMajorEncoderL;
    QWidget*        tab_3;
    QGroupBox*      groupBoxInformation;
    QGroupBox*      groupBoxBoard;
    QTextEdit*      EditBoard;
    QGroupBox*      groupBoxDSP;
    QTextEdit*      EditDsp;
    QGroupBox*      groupBoxChannel;
    QTextEdit*      EditChannel;

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
    bool    eventFilter(QObject* , QEvent* );

    public slots : void OpenSetting (int);
    void    PageChange(QWidget* );
    void    changeEdit();
    void    areaClick(int);
    void    netChangeSet(bool);
    void    audioChangeSet(int Select, float data);
    void    videoChangeSet(int Select, float data);
    void    totalChangeSet(int Select, int data);
    void    bitRateChangeSet(int Select, int data);
    void    countChangeSet(int data);
    void    fullSpaceSet();
    void    dsPreviewSet();
    void    dsMajorRecordSet(bool on);
    void    dsMinorRecordSet(bool on);

    /*
     * void dsMajorEncoder(int);
     */
    void    dsChannel(const QString& Qstxt);

    /*
     * void dsDCodingSet(bool);
     */
    void    dsFrameinfo();

    /* tyu add at 2007.02.08 */
    void    dsMajorEncoder(int);
    void    dsMinorEncoder(int);
    void    dsMajorStream(int);
    void    dsMinorStream(int);

    void    dsBpsModeSet(int index);
    void    dsMaxBpsSet();

    void    dsVideoSet(int);
    void    dsRestoreSet();

    void    dsOsdSet(bool);
    void    dsOsdTransSet(bool);
    void    dsOsdChangeSet(int);

    void    dsLogoChangeSet(int value);
    void    dsLogoTransSet(bool on);
    void    dsLogoSet(bool on);

    void    dsMaskSet(bool on);
    void    dsNewButtonClicked();
    void    dsDeleteButtonClicked();
    void    dsRectangleChange(int index);
    void    dsMaskEditPress();

    void    dsMotionSet(bool);
    void    dsMotionSenSet(int);
    void    dsAudioSet(bool);

    void    dsBmpSet(bool on);
    void    dsJpegSet(bool on);
    void    dsCaptureButtonClicked();
    void    dsImageStreamSet(bool);

    void    noNetSet();
    protected slots : virtual void languageChange();

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    QPixmap         image0;
    QTimer*         dstimer;

    void            SetComBox();
    int             SetBoardDspChannelInf();
    void            SetAllEnabled(bool on);
    void            SetALLValueDefault();
    void            DisconnectCheck();
    void            ConnectCheck();
    int             GSelectPort;
    bool            GFullArea;
    void            checkspace();
    pthread_mutex_t mutex;
};
#endif /* DSFORM_H */
