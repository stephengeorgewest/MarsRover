
#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qscrollview.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

#include "dsform.h"
#include "QSDLWidget.h"
#include "forminfo.h"

int fsWidth;
int fsHeight;

/*
 =======================================================================================================================
 * Constructs a dsForm as a child of 'parent', with the name 'name' and widget flags set to 'f'.
 =======================================================================================================================
 */
dsForm::dsForm(QWidget* parent, const char* name, WFlags fl) :
    QWidget(parent, name, fl)
{
    QDateTime   Dt = QDateTime::currentDateTime();
    QString     Qs;

    Qs.fill(' ', 20);

    QDesktopWidget*     d = QApplication::desktop();
    fsWidth = d->width();               /* 返回桌面宽度 */
    fsHeight = d->height();             /* 返回桌面高度 */
    OutputDebugString("width/height = %d, %d\n", fsWidth, fsHeight);

    if(!name)
        setName("dsForm");
    setMinimumSize(QSize(1024, 710));   /* 分辨率 */
    setMaximumSize(QSize(fsWidth, fsHeight));
    setBaseSize(QSize(1024, 710));

    w_full_area = new QSDLWidget(this, "w_full_area");
    w_full_area->setMinimumSize(QSize(800, 700));
    w_full_area->setMinimumSize(QSize(fsWidth, fsHeight));
    w_full_area->setBaseSize(QSize(800, 700));

    TitleEdit = new QLineEdit(this, "TitleEdit");
    TitleEdit->setGeometry(QRect(0, 0, 1024, 20));
    TitleEdit->setPaletteForegroundColor(QColor(255, 255, 255));
    TitleEdit->setPaletteBackgroundColor(QColor(0, 0, 255));

    QFont   TitleEdit_font(TitleEdit->font());
    TitleEdit_font.setFamily("Courier");
    TitleEdit_font.setBold(TRUE);
    TitleEdit->setFont(TitleEdit_font);
    TitleEdit->setReadOnly(TRUE);

    tabWidget3 = new QTabWidget(this, "tabWidget3");
    tabWidget3->setGeometry(QRect(800, 20, 224, 703));
    tabWidget3->setMinimumSize(QSize(0, 0));

    tab = new QWidget(tabWidget3, "tab");

    groupBoxglobal = new QGroupBox(tab, "groupBoxglobal");
    groupBoxglobal->setGeometry(QRect(0, 0, 220, 1900));

    groupBoxGCI = new QGroupBox(groupBoxglobal, "groupBoxGCI");
    groupBoxGCI->setGeometry(QRect(11, 115, 196, 166));

    ChannelNumLabel = new QLabel(groupBoxGCI, "ChannelNumLabel");
    ChannelNumLabel->setGeometry(QRect(10, 20, 80, 20));

    w_channel = new QLineEdit(groupBoxGCI, "w_channel");
    w_channel->setGeometry(QRect(105, 20, 83, 20));

    AudioRateLabel = new QLabel(groupBoxGCI, "AudioRateLabel");
    AudioRateLabel->setGeometry(QRect(3, 50, 106, 20));

    w_audio_frame = new QLineEdit(groupBoxGCI, "w_audio_frame");
    w_audio_frame->setGeometry(QRect(105, 50, 83, 20));

    w_totalframe = new QLineEdit(groupBoxGCI, "w_totalframe");
    w_totalframe->setGeometry(QRect(105, 110, 83, 20));

    TotalFrameLabel = new QLabel(groupBoxGCI, "TotalFrameLabel");
    TotalFrameLabel->setGeometry(QRect(16, 110, 80, 20));

    BitRateLabel = new QLabel(groupBoxGCI, "BitRateLabel");
    BitRateLabel->setGeometry(QRect(20, 140, 80, 20));

    w_bit_rate = new QLineEdit(groupBoxGCI, "w_bit_rate");
    w_bit_rate->setGeometry(QRect(105, 140, 83, 20));

    w_video_frame = new QLineEdit(groupBoxGCI, "w_video_frame");
    w_video_frame->setGeometry(QRect(105, 80, 83, 20));

    VideoRateLabel = new QLabel(groupBoxGCI, "VideoRateLabel");
    VideoRateLabel->setGeometry(QRect(3, 80, 101, 20));

    groupBoxGNS = new QGroupBox(groupBoxglobal, "groupBoxGNS");
    groupBoxGNS->setGeometry(QRect(10, 285, 196, 77));

    line1_4_2_2 = new QFrame(groupBoxGNS, "line1_4_2_2");
    line1_4_2_2->setGeometry(QRect(11, 45, 175, 5));
    line1_4_2_2->setFrameShape(QFrame::HLine);
    line1_4_2_2->setFrameShadow(QFrame::Sunken);
    line1_4_2_2->setFrameShape(QFrame::HLine);

    NetCountLabel = new QLabel(groupBoxGNS, "NetCountLabel");
    NetCountLabel->setGeometry(QRect(4, 20, 92, 20));

    w_netusercount = new QLineEdit(groupBoxGNS, "w_netusercount");
    w_netusercount->setGeometry(QRect(105, 20, 83, 20));

    w_net_transfer = new QCheckBox(groupBoxGNS, "w_net_transfer");
    w_net_transfer->setGeometry(QRect(3, 50, 86, 20));

    w_stringtoclient = new QCheckBox(groupBoxGNS, "w_stringtoclient");
    w_stringtoclient->setGeometry(QRect(96, 50, 96, 20));

    w_preview = new QCheckBox(groupBoxglobal, "w_preview");
    w_preview->setGeometry(QRect(13, 375, 86, 20));
    w_preview->setChecked(TRUE);

    w_record = new QCheckBox(groupBoxglobal, "w_preview");
    w_record->setGeometry(QRect(106, 375, 86, 20));

    ButtonEnableG = new QPushButton(groupBoxglobal, "ButtonEnableG");
    ButtonEnableG->setGeometry(QRect(28, 420, 150, 26));

    w_exit = new QPushButton(groupBoxglobal, "w_exit");
    w_exit->setGeometry(QRect(28, 470, 150, 25));

    groupBoxGSI = new QGroupBox(groupBoxglobal, "groupBoxGSI");
    groupBoxGSI->setGeometry(QRect(10, 6, 196, 107));

    DateEdit = new QLineEdit(groupBoxGSI, "DateEdit");
    DateEdit->setGeometry(QRect(90, 20, 100, 20));

    TimeEdit = new QLineEdit(groupBoxGSI, "TimeEdit");
    TimeEdit->setGeometry(QRect(90, 50, 100, 20));

    FreespaceEdit = new QLineEdit(groupBoxGSI, "FreespaceEdit");
    FreespaceEdit->setGeometry(QRect(90, 80, 100, 20));

    FreeSpaceLabel = new QLabel(groupBoxGSI, "FreeSpaceLabel");
    FreeSpaceLabel->setGeometry(QRect(10, 80, 60, 20));

    TimeLabel = new QLabel(groupBoxGSI, "TimeLabel");
    TimeLabel->setGeometry(QRect(10, 50, 60, 20));

    DateLabel = new QLabel(groupBoxGSI, "DateLabel");
    DateLabel->setGeometry(QRect(10, 20, 60, 20));
    tabWidget3->insertTab(tab, "");

    tab_2 = new QWidget(tabWidget3, "tab_2");
    Qsv = new QScrollView(tab_2);
    Qsv->setGeometry(QRect(0, 0, 218, 678));

    groupBox7 = new QGroupBox(tab_2, "groupBox7");
    groupBox7->setGeometry(QRect(0, 0, 200, 1500));
    groupBox7->setBackgroundOrigin(QGroupBox::WidgetOrigin);

    LabelChannelL = new QLabel(groupBox7, "LabelChannelL");
    LabelChannelL->setGeometry(QRect(13, 5, 90, 20));

    groupBoxOS = new QGroupBox(groupBox7, "groupBoxOS");
    groupBoxOS->setGeometry(QRect(2, 701, 196, 131));

    line1_4_2 = new QFrame(groupBoxOS, "line1_4_2");
    line1_4_2->setGeometry(QRect(10, 65, 175, 5));
    line1_4_2->setFrameShape(QFrame::HLine);
    line1_4_2->setFrameShadow(QFrame::Sunken);
    line1_4_2->setFrameShape(QFrame::HLine);

    CheckTranslucentL = new QCheckBox(groupBoxOS, "CheckTranslucentL");
    CheckTranslucentL->setGeometry(QRect(101, 17, 80, 20));

    SliderOsdPosXL = new QSlider(groupBoxOS, "SliderOsdPosXL");
    SliderOsdPosXL->setGeometry(QRect(89, 76, 100, 20));
    SliderOsdPosXL->setCursor(QCursor(0));
    SliderOsdPosXL->setOrientation(QSlider::Horizontal);

    LabelOsdBrightnessL = new QLabel(groupBoxOS, "LabelOsdBrightnessL");
    LabelOsdBrightnessL->setGeometry(QRect(16, 43, 59, 20));

    SliderOsdBrightnessL = new QSlider(groupBoxOS, "SliderOsdBrightnessL");
    SliderOsdBrightnessL->setGeometry(QRect(89, 43, 100, 20));
    SliderOsdBrightnessL->setCursor(QCursor(0));
    SliderOsdBrightnessL->setOrientation(QSlider::Horizontal);

    LabelOsdPosXL = new QLabel(groupBoxOS, "LabelOsdPosXL");
    LabelOsdPosXL->setGeometry(QRect(16, 76, 59, 20));

    LabelOsdPosYL = new QLabel(groupBoxOS, "LabelOsdPosYL");
    LabelOsdPosYL->setGeometry(QRect(16, 105, 59, 20));

    SliderOsdPosYL = new QSlider(groupBoxOS, "SliderOsdPosYL");
    SliderOsdPosYL->setGeometry(QRect(89, 105, 100, 20));
    SliderOsdPosYL->setCursor(QCursor(0));
    SliderOsdPosYL->setOrientation(QSlider::Horizontal);

    CheckOsdL = new QCheckBox(groupBoxOS, "CheckOsdL");
    CheckOsdL->setGeometry(QRect(5, 17, 100, 20));

    groupBoxCI = new QGroupBox(groupBox7, "groupBoxCI");
    groupBoxCI->setGeometry(QRect(2, 1351, 196, 126));

    RadioBmpL = new QRadioButton(groupBoxCI, "RadioBmpL");
    RadioBmpL->setGeometry(QRect(10, 20, 70, 20));

    line1_4_2_2_3 = new QFrame(groupBoxCI, "line1_4_2_2_3");
    line1_4_2_2_3->setGeometry(QRect(10, 40, 175, 5));
    line1_4_2_2_3->setFrameShape(QFrame::HLine);
    line1_4_2_2_3->setFrameShadow(QFrame::Sunken);
    line1_4_2_2_3->setFrameShape(QFrame::HLine);

    RadioJpegL = new QRadioButton(groupBoxCI, "RadioJpegL");
    RadioJpegL->setGeometry(QRect(10, 48, 60, 20));

    LabelPictureQL = new QLabel(groupBoxCI, "LabelPictureQL");
    LabelPictureQL->setGeometry(QRect(3, 70, 84, 20));

    SliderPictureQL = new QSlider(groupBoxCI, "SliderPictureQL");
    SliderPictureQL->setGeometry(QRect(87, 70, 100, 20));
    SliderPictureQL->setCursor(QCursor(0));
    SliderPictureQL->setOrientation(QSlider::Horizontal);

    ButtonCaptureL = new QPushButton(groupBoxCI, "ButtonCaptureL");
    ButtonCaptureL->setGeometry(QRect(25, 94, 148, 26));

    groupBoxIS = new QGroupBox(groupBox7, "groupBoxIS");
    groupBoxIS->setGeometry(QRect(2, 1219, 196, 132));

    line1_5_2 = new QFrame(groupBoxIS, "line1_5_2");
    line1_5_2->setGeometry(QRect(8, 44, 175, 5));
    line1_5_2->setFrameShape(QFrame::HLine);
    line1_5_2->setFrameShadow(QFrame::Sunken);
    line1_5_2->setFrameShape(QFrame::HLine);

    CheckImageCaptureL = new QCheckBox(groupBoxIS, "CheckImageCaptureL");
    CheckImageCaptureL->setGeometry(QRect(5, 20, 110, 20));

    CBImageStreamWL = new QComboBox(FALSE, groupBoxIS, "CBImageStreamWL");
    CBImageStreamWL->setGeometry(QRect(80, 55, 83, 20));

    LabelImageStreamWL = new QLabel(groupBoxIS, "LabelImageStreamWL");
    LabelImageStreamWL->setGeometry(QRect(15, 55, 48, 20));

    CBImageStreamHL = new QComboBox(FALSE, groupBoxIS, "CBImageStreamHL");
    CBImageStreamHL->setGeometry(QRect(80, 95, 83, 20));

    LabelImageStreamHL = new QLabel(groupBoxIS, "LabelImageStreamHL");
    LabelImageStreamHL->setGeometry(QRect(15, 95, 48, 20));

    groupBoxAudio = new QGroupBox(groupBox7, "groupBoxAudio");
    groupBoxAudio->setGeometry(QRect(2, 1152, 196, 67));

    CheckAudioL = new QCheckBox(groupBoxAudio, "CheckAudioL");
    CheckAudioL->setGeometry(QRect(5, 17, 100, 20));

    EditSoundLevelL = new QLineEdit(groupBoxAudio, "EditSoundLevelL");
    EditSoundLevelL->setGeometry(QRect(100, 40, 83, 20));

    LabelSoundLevelL = new QLabel(groupBoxAudio, "LabelSoundLevelL");
    LabelSoundLevelL->setGeometry(QRect(15, 40, 73, 20));

    groupBoxMS = new QGroupBox(groupBox7, "groupBoxMS");
    groupBoxMS->setGeometry(QRect(2, 1082, 196, 70));

    CheckMotionDSL = new QCheckBox(groupBoxMS, "CheckMotionDSL");
    CheckMotionDSL->setGeometry(QRect(15, 17, 140, 20));

    LabelMotionSensL = new QLabel(groupBoxMS, "LabelMotionSensL");
    LabelMotionSensL->setGeometry(QRect(14, 43, 74, 20));

    SliderMotionSensL = new QSlider(groupBoxMS, "SliderMotionSensL");
    SliderMotionSensL->setGeometry(QRect(89, 43, 100, 20));
    SliderMotionSensL->setCursor(QCursor(0));
    SliderMotionSensL->setOrientation(QSlider::Horizontal);

    groupBoxMask = new QGroupBox(groupBox7, "groupBoxMask");
    groupBoxMask->setGeometry(QRect(2, 932, 196, 150));

    line1_4_3 = new QFrame(groupBoxMask, "line1_4_3");
    line1_4_3->setGeometry(QRect(8, 37, 175, 5));
    line1_4_3->setFrameShape(QFrame::HLine);
    line1_4_3->setFrameShadow(QFrame::Sunken);
    line1_4_3->setFrameShape(QFrame::HLine);

    CheckMaskL = new QCheckBox(groupBoxMask, "CheckMaskL");
    CheckMaskL->setGeometry(QRect(5, 17, 100, 20));

    CBRectangleL = new QComboBox(FALSE, groupBoxMask, "CBRectangleL");
    CBRectangleL->setGeometry(QRect(80, 50, 83, 20));

    LabelRectangleL = new QLabel(groupBoxMask, "LabelRectangleL");
    LabelRectangleL->setGeometry(QRect(10, 50, 48, 20));

    LabelBoundL = new QLabel(groupBoxMask, "LabelBoundL");
    LabelBoundL->setGeometry(QRect(10, 80, 39, 20));

    EditBoundL = new QLineEdit(groupBoxMask, "EditBoundL");
    EditBoundL->setGeometry(QRect(80, 80, 100, 22));

    ButtonNewL = new QPushButton(groupBoxMask, "ButtonNewL");
    ButtonNewL->setGeometry(QRect(10, 110, 83, 29));

    ButtonDeleteL = new QPushButton(groupBoxMask, "ButtonDeleteL");
    ButtonDeleteL->setGeometry(QRect(100, 110, 83, 29));

    groupBoxLogo = new QGroupBox(groupBox7, "groupBoxLogo");
    groupBoxLogo->setGeometry(QRect(2, 832, 196, 100));

    line1_3_2_2 = new QFrame(groupBoxLogo, "line1_3_2_2");
    line1_3_2_2->setGeometry(QRect(8, 37, 175, 5));
    line1_3_2_2->setFrameShape(QFrame::HLine);
    line1_3_2_2->setFrameShadow(QFrame::Sunken);
    line1_3_2_2->setFrameShape(QFrame::HLine);

    CheckLogoL = new QCheckBox(groupBoxLogo, "CheckLogoL");
    CheckLogoL->setGeometry(QRect(5, 17, 100, 20));

    CheckLogoTransL = new QCheckBox(groupBoxLogo, "CheckLogoTransL");
    CheckLogoTransL->setGeometry(QRect(101, 17, 80, 20));

    SliderLogoPosXL = new QSlider(groupBoxLogo, "SliderLogoPosXL");
    SliderLogoPosXL->setGeometry(QRect(83, 46, 100, 20));
    SliderLogoPosXL->setCursor(QCursor(0));
    SliderLogoPosXL->setOrientation(QSlider::Horizontal);

    LabelLogoPosXL = new QLabel(groupBoxLogo, "LabelLogoPosXL");
    LabelLogoPosXL->setGeometry(QRect(16, 46, 59, 20));

    LabelLogoPosYL = new QLabel(groupBoxLogo, "LabelLogoPosYL");
    LabelLogoPosYL->setGeometry(QRect(16, 75, 59, 20));

    SliderLogoPosYL = new QSlider(groupBoxLogo, "SliderLogoPosYL");
    SliderLogoPosYL->setGeometry(QRect(84, 75, 100, 20));
    SliderLogoPosYL->setCursor(QCursor(0));
    SliderLogoPosYL->setOrientation(QSlider::Horizontal);

    groupBoxBC = new QGroupBox(groupBox7, "groupBoxBC");
    groupBoxBC->setGeometry(QRect(2, 471, 196, 70));

    LabelMaxBpsL = new QLabel(groupBoxBC, "LabelMaxBpsL");
    LabelMaxBpsL->setGeometry(QRect(19, 20, 48, 20));

    CBModeL = new QComboBox(FALSE, groupBoxBC, "CBModeL");
    CBModeL->setGeometry(QRect(105, 46, 83, 20));

    LabelModeL = new QLabel(groupBoxBC, "LabelModeL");
    LabelModeL->setGeometry(QRect(22, 46, 36, 20));

    EditMaxBpsL = new QLineEdit(groupBoxBC, "EditMaxBpsL");
    EditMaxBpsL->setGeometry(QRect(105, 20, 83, 20));

    groupBoxVI = new QGroupBox(groupBox7, "groupBoxVI");
    groupBoxVI->setGeometry(QRect(1, 541, 196, 160));

    LabelBrightnessL = new QLabel(groupBoxVI, "LabelBrightnessL");
    LabelBrightnessL->setGeometry(QRect(10, 20, 59, 20));

    LabelContrastL = new QLabel(groupBoxVI, "LabelContrastL");
    LabelContrastL->setGeometry(QRect(10, 45, 55, 20));

    SliderContrastL = new QSlider(groupBoxVI, "SliderContrastL");
    SliderContrastL->setGeometry(QRect(78, 45, 110, 20));
    SliderContrastL->setOrientation(QSlider::Horizontal);

    LabelSaturationL = new QLabel(groupBoxVI, "LabelSaturationL");
    LabelSaturationL->setGeometry(QRect(10, 70, 57, 20));

    SliderSaturationL = new QSlider(groupBoxVI, "SliderSaturationL");
    SliderSaturationL->setGeometry(QRect(78, 70, 110, 20));
    SliderSaturationL->setOrientation(QSlider::Horizontal);

    LabelHueL = new QLabel(groupBoxVI, "LabelHueL");
    LabelHueL->setGeometry(QRect(10, 95, 42, 20));

    SliderHueL = new QSlider(groupBoxVI, "SliderHueL");
    SliderHueL->setGeometry(QRect(78, 95, 110, 20));
    SliderHueL->setOrientation(QSlider::Horizontal);

    SliderBrightnessL = new QSlider(groupBoxVI, "SliderBrightnessL");
    SliderBrightnessL->setGeometry(QRect(78, 20, 110, 20));
    SliderBrightnessL->setCursor(QCursor(0));
    SliderBrightnessL->setOrientation(QSlider::Horizontal);

    ButtonVideoRestoreL = new QPushButton(groupBoxVI, "ButtonVideoRestoreL");
    ButtonVideoRestoreL->setGeometry(QRect(42, 120, 120, 29));

    CBChannelL = new QComboBox(FALSE, groupBox7, "CBChannelL");
    CBChannelL->setGeometry(QRect(113, 5, 83, 20));
    CBChannelL->setCurrentItem(0);

    groupBox9 = new QGroupBox(groupBox7, "groupBox9");
    groupBox9->setGeometry(QRect(2, 30, 196, 60));

    CheckMinorRecordL = new QCheckBox(groupBox9, "CheckMinorRecordL");
    CheckMinorRecordL->setGeometry(QRect(101, 30, 90, 20));

    CheckMajorRecordL = new QCheckBox(groupBox9, "CheckMajorRecordL");
    CheckMajorRecordL->setGeometry(QRect(4, 30, 86, 20));

    CheckPreviewLl = new QCheckBox(groupBox9, "CheckPreviewLl");
    CheckPreviewLl->setGeometry(QRect(5, 5, 100, 20));

    groupBoxCD = new QGroupBox(groupBox7, "groupBoxCD");
    groupBoxCD->setGeometry(QRect(0, 90, 196, 380));

    LabelMajorStreamL = new QLabel(groupBoxCD, "LabelMajorStreamL");
    LabelMajorStreamL->setGeometry(QRect(10, 28, 86, 20));

    CBMajorStreamL = new QComboBox(FALSE, groupBoxCD, "CBMajorStreamL");
    CBMajorStreamL->setGeometry(QRect(108, 28, 83, 20));

    LabelMinorStreamL = new QLabel(groupBoxCD, "LabelMinorStreamL");
    LabelMinorStreamL->setGeometry(QRect(10, 66, 86, 20));

    CBMinorStreamL = new QComboBox(FALSE, groupBoxCD, "CBMinorStreamL");
    CBMinorStreamL->setGeometry(QRect(108, 67, 83, 20));

    groupBoxVQ = new QGroupBox(groupBoxCD, "groupBoxVQ");
    groupBoxVQ->setGeometry(QRect(2, 90, 192, 95));

    LabelIQuantL = new QLabel(groupBoxVQ, "LabelIQuantL");
    LabelIQuantL->setGeometry(QRect(10, 20, 48, 20));

    EditIQuantL = new QLineEdit(groupBoxVQ, "EditIQuantL");
    EditIQuantL->setGeometry(QRect(105, 20, 83, 20));

    EditBQuantL = new QLineEdit(groupBoxVQ, "EditBQuantL");
    EditBQuantL->setGeometry(QRect(105, 44, 83, 20));

    LabelBQuantL = new QLabel(groupBoxVQ, "LabelBQuantL");
    LabelBQuantL->setGeometry(QRect(10, 44, 48, 20));

    EditPQuantL = new QLineEdit(groupBoxVQ, "EditPQuantL");
    EditPQuantL->setGeometry(QRect(105, 68, 83, 20));

    LabelPQuantL = new QLabel(groupBoxVQ, "LabelPQuantL");
    LabelPQuantL->setGeometry(QRect(10, 68, 48, 20));

    groupBoxFS = new QGroupBox(groupBoxCD, "groupBoxFS");
    groupBoxFS->setGeometry(QRect(2, 186, 192, 116));

    LabelKeyFrameL = new QLabel(groupBoxFS, "LabelKeyFrameL");
    LabelKeyFrameL->setGeometry(QRect(2, 20, 104, 20));

    EditBFramesL = new QLineEdit(groupBoxFS, "EditBFramesL");
    EditBFramesL->setGeometry(QRect(105, 44, 83, 20));

    LabelBFramesL = new QLabel(groupBoxFS, "LabelBFramesL");
    LabelBFramesL->setGeometry(QRect(20, 44, 48, 20));

    EditPFramesL = new QLineEdit(groupBoxFS, "EditPFramesL");
    EditPFramesL->setGeometry(QRect(105, 68, 83, 20));
    EditPFramesL->setReadOnly(TRUE);

    LabelPFramesL = new QLabel(groupBoxFS, "LabelPFramesL");
    LabelPFramesL->setGeometry(QRect(20, 68, 48, 20));

    LabelFramesRL = new QLabel(groupBoxFS, "LabelFramesRL");
    LabelFramesRL->setGeometry(QRect(11, 92, 68, 20));

    EditFramesRL = new QLineEdit(groupBoxFS, "EditFramesRL");
    EditFramesRL->setGeometry(QRect(105, 92, 83, 20));

    EditKeyFrameL = new QLineEdit(groupBoxFS, "EditKeyFrameL");
    EditKeyFrameL->setGeometry(QRect(105, 20, 83, 20));

    groupBoxPF = new QGroupBox(groupBoxCD, "groupBoxPF");
    groupBoxPF->setGeometry(QRect(2, 303, 192, 72));

    CBMinorEncoderL = new QComboBox(FALSE, groupBoxPF, "CBMinorEncoderL");
    CBMinorEncoderL->setGeometry(QRect(105, 47, 83, 20));

    LabelMinorEncoderL = new QLabel(groupBoxPF, "LabelMinorEncoderL");
    LabelMinorEncoderL->setGeometry(QRect(3, 47, 85, 20));

    CBMajorEncoderL = new QComboBox(FALSE, groupBoxPF, "CBMajorEncoderL");
    CBMajorEncoderL->setGeometry(QRect(105, 20, 83, 20));

    LabelMajorEncoderL = new QLabel(groupBoxPF, "LabelMajorEncoderL");
    LabelMajorEncoderL->setGeometry(QRect(2, 20, 90, 20));
    Qsv->addChild(groupBox7);

    tabWidget3->insertTab(tab_2, "");

    tab_3 = new QWidget(tabWidget3, "tab_3");

    groupBoxInformation = new QGroupBox(tab_3, "groupBoxInformation");
    groupBoxInformation->setGeometry(QRect(0, 0, 223, 678));

    groupBoxBoard = new QGroupBox(groupBoxInformation, "groupBoxBoard");
    groupBoxBoard->setGeometry(QRect(2, 2, 219, 224));

    EditBoard = new QTextEdit(groupBoxBoard, "EditBoard");
    EditBoard->setGeometry(QRect(2, 16, 214, 205));
    EditBoard->setWordWrap(QTextEdit::WidgetWidth);
    EditBoard->setReadOnly(TRUE);

    groupBoxDSP = new QGroupBox(groupBoxInformation, "groupBoxDSP");
    groupBoxDSP->setGeometry(QRect(2, 228, 219, 224));

    EditDsp = new QTextEdit(groupBoxDSP, "EditDsp");
    EditDsp->setGeometry(QRect(2, 16, 214, 205));
    EditDsp->setWordWrap(QTextEdit::WidgetWidth);
    EditDsp->setWrapColumnOrWidth(210);
    EditDsp->setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary);
    EditDsp->setReadOnly(TRUE);

    groupBoxChannel = new QGroupBox(groupBoxInformation, "groupBoxChannel");
    groupBoxChannel->setGeometry(QRect(2, 454, 219, 220));

    EditChannel = new QTextEdit(groupBoxChannel, "EditChannel");
    EditChannel->setGeometry(QRect(2, 16, 214, 201));
    EditChannel->setWordWrap(QTextEdit::WidgetWidth);
    EditChannel->setWrapColumnOrWidth(210);
    EditChannel->setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary);
    EditChannel->setReadOnly(TRUE);
    tabWidget3->insertTab(tab_3, "");

    show();
    w_full_area->Createdsclass();

    video_info*     temp;
    temp = w_full_area->GetVideoParam();
    SliderBrightnessL->setValue(temp->Brightness);
    SliderContrastL->setValue(temp->Contrast);
    SliderSaturationL->setValue(temp->Saturation);
    SliderHueL->setValue(temp->Hue);

    TitleEdit->setText(QString("HikVision demo - loop test for DS-40xxHC card") + Qs + QString(
                           "Start time: ") + Dt.date().toString(" yyyy-MM-dd ") + Dt.time().toString(" hh:mm:ss "));

    dstimer = new QTimer(this);
    connect(dstimer, SIGNAL(timeout()), this, SLOT(changeEdit()));
    dstimer->start(200);                /* 200ms */

    w_channel->setText(QString("        0"));
    w_audio_frame->setText(QString("        0"));
    w_totalframe->setText(QString("        0"));
    w_bit_rate->setText(QString("        0"));
    w_video_frame->setText(QString("        0"));
    w_netusercount->setText(QString("        0"));
    w_channel->setReadOnly(TRUE);
    w_audio_frame->setReadOnly(TRUE);
    w_totalframe->setReadOnly(TRUE);
    w_bit_rate->setReadOnly(TRUE);
    w_video_frame->setReadOnly(TRUE);
    w_netusercount->setReadOnly(TRUE);
    DateEdit->setReadOnly(TRUE);
    TimeEdit->setReadOnly(TRUE);
    FreespaceEdit->setReadOnly(TRUE);

    SetComBox();
    CBChannelL->setCurrentItem(0);
    SetBoardDspChannelInf();
    SetAllEnabled(FALSE);
    languageChange();
    resize(QSize(1024, 710).expandedTo(minimumSizeHint()));

    checkspace();

    w_full_area->installEventFilter(this);

    GSelectPort = 0;
    GFullArea = FALSE;

    connect(CBChannelL, SIGNAL(activated (int)), SLOT(OpenSetting (int)));
    connect(tabWidget3, SIGNAL(currentChanged (QWidget*)), SLOT(PageChange (QWidget*)));

    connect(w_full_area, SIGNAL(clicked (int)), SLOT(areaClick (int)));
    connect(w_full_area, SIGNAL(audioChange (int, float)), SLOT(audioChangeSet (int, float)));
    connect(w_full_area, SIGNAL(videoChange (int, float)), SLOT(videoChangeSet (int, float)));
    connect(w_full_area, SIGNAL(totalChange (int, int)), SLOT(totalChangeSet (int, int)));
    connect(w_full_area, SIGNAL(bitRateChange (int, int)), SLOT(bitRateChangeSet (int, int)));
    connect(w_full_area, SIGNAL(countChange (int)), SLOT(countChangeSet (int)));
    connect(w_full_area, SIGNAL(fullSpace()), SLOT(fullSpaceSet()));

    connect(ButtonEnableG, SIGNAL(clicked()), SLOT(dsPreviewSet()));
    connect(w_preview, SIGNAL(toggled(bool)), w_full_area, SLOT(PreviewAllSet(bool)));
    connect(w_channel, SIGNAL(textChanged (const QString&)), SLOT(dsChannel (const QString&)));
    connect(w_net_transfer, SIGNAL(toggled(bool)), w_full_area, SLOT(NetTransferSet(bool)));
    connect(w_stringtoclient, SIGNAL(toggled(bool)), w_full_area, SLOT(StringToClientSet(bool)));
    connect(w_full_area, SIGNAL(noNet()), SLOT(noNetSet()));
    connect(w_exit, SIGNAL(clicked()), this, SLOT(close()));
    connect(w_record, SIGNAL(toggled(bool)), w_full_area, SLOT(RecordAllSet(bool)));

    pthread_mutex_init(&mutex, NULL);

    ConnectCheck();
}

/*
 =======================================================================================================================
 * Destroys the object and frees any allocated resources
 =======================================================================================================================
 */
dsForm::~dsForm()
{
    /* no need to delete child widgets, Qt does it all for us */
    delete dstimer;
    pthread_mutex_destroy(&mutex);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsPreviewSet()
{
    int         port = atoi(w_channel->text());
    DSDEMO*     temp = w_full_area->GetDsinfo(port);

    if(temp->bPreviewOpen)
        w_full_area->PreviewSet(FALSE);
    else
        w_full_area->PreviewSet(TRUE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsImageStreamSet(bool on)
{
    int index = CBChannelL->currentItem();
    int width = 704 / (1 << (CBImageStreamWL->currentItem()));
    int height = atoi(CBImageStreamHL->text(0)) / (1 << (CBImageStreamHL->currentItem()));
    OutputDebugString("ImageStreamSet width : %d****height:%d *****\n", width, height);

    if(index == 1)
        w_full_area->ImageStreamAllSet(on, width, height);
    else if(index >= 2)
        w_full_area->ImageStreamSet(on, width, height);

    if(on)
    {
        CBImageStreamWL->setEnabled(FALSE);
        CBImageStreamHL->setEnabled(FALSE);
    }
    else
    {
        CBImageStreamWL->setEnabled(TRUE);
        CBImageStreamHL->setEnabled(TRUE);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsBmpSet(bool on)
{
    if(on || RadioJpegL->isChecked())
        ButtonCaptureL->setEnabled(TRUE);
    else
        ButtonCaptureL->setEnabled(FALSE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsJpegSet(bool on)
{
    if(on || RadioBmpL->isChecked())
        ButtonCaptureL->setEnabled(TRUE);
    else
        ButtonCaptureL->setEnabled(FALSE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsCaptureButtonClicked()
{
    static int  count = 1;
    int         index = CBChannelL->currentItem();

    if(index == 1)
    {
        if(RadioBmpL->isChecked())
            w_full_area->GetBmpAllPicture(count);
        if(RadioJpegL->isChecked())
            w_full_area->GetJpegAllPicture(count, SliderPictureQL->value());
    }
    else if(index >= 2)
    {
        if(RadioBmpL->isChecked())
            w_full_area->GetBmpPicture(count);
        if(RadioJpegL->isChecked())
            w_full_area->GetJpegPicture(count, SliderPictureQL->value());
    }

    count++;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMotionSenSet(int value)
{
    int index = CBChannelL->currentItem();

    if(index == 1)
        w_full_area->MotionSenAllSet(value);
    else if(index >= 2)
    {
        w_full_area->MotionSenSet(value);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsAudioSet(bool on)
{
    w_full_area->AudioSet(on);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMotionSet(bool on)
{
    int index = CBChannelL->currentItem();

    if(index == 1)
        w_full_area->MotionAllSet(on);
    else if(index >= 2)
    {
        w_full_area->MotionSet(on);
    }

    SliderMotionSensL->setEnabled(on);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMaskSet(bool on)
{
    int index = CBChannelL->currentItem();

    OutputDebugString("In function dsMaskSet\n");

    if(index == 1)
        w_full_area->MaskEnableAllSet(on);
    else if(index >= 2)
        w_full_area->MaskEnableSet(on);

    if(on)
    {
        CBRectangleL->setEnabled(TRUE);
        EditBoundL->setText("");
        EditBoundL->setEnabled(FALSE);
        ButtonNewL->setEnabled(TRUE);
        ButtonDeleteL->setEnabled(FALSE);
    }
    else
    {
        CBRectangleL->setEnabled(FALSE);
        EditBoundL->setEnabled(FALSE);
        ButtonNewL->setEnabled(FALSE);
        ButtonDeleteL->setEnabled(FALSE);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsNewButtonClicked()
{
    EditBoundL->setText("");
    EditBoundL->setEnabled(TRUE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsDeleteButtonClicked()
{
    int index = CBRectangleL->currentItem();
    int Channel = CBChannelL->currentItem();

    if(Channel == 1)
        w_full_area->MaskDelAllSet(index);
    else if(Channel >= 2)
        w_full_area->MaskDelSet(index);

    CBRectangleL->removeItem(index);

    EditBoundL->setText("");
    EditBoundL->setEnabled(FALSE);

    if(CBRectangleL->count() == 0)
        ButtonDeleteL->setEnabled(FALSE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsRectangleChange(int index)
{
    EditBoundL->setText(CBRectangleL->text(index));
    EditBoundL->setEnabled(FALSE);

    if(CBRectangleL->count() > 0)
        ButtonDeleteL->setEnabled(TRUE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMaskEditPress()
{
    RECT    temp;
    int     i, j;
    bool    ok;

    QString qs = (EditBoundL->text()).stripWhiteSpace();
    QString qstemp;

    if(qs.contains(',', false) != 3)
    {
        EditBoundL->setText("");
        return;
    }

    i = qs.find(',', 0, false);
    qstemp = qs.mid(0, i);
    temp.RectTop = qstemp.toInt(&ok, 10);
    if((!ok) || temp.RectTop < 0)
    {
        EditBoundL->setText("");
        return;
    }

    j = i;
    i = qs.find(',', j + 1, false);
    qstemp = qs.mid(j + 1, i - j - 1);
    temp.RectBottom = qstemp.toInt(&ok, 10);
    if((!ok) || temp.RectBottom < 0)
    {
        EditBoundL->setText("");
        return;
    }

    j = i;
    i = qs.find(',', j + 1, false);
    qstemp = qs.mid(j + 1, i - j - 1);
    temp.RectLeft = qstemp.toInt(&ok, 10);
    if((!ok) || temp.RectLeft < 0)
    {
        EditBoundL->setText("");
        return;
    }

    j = i;
    i = qs.length();
    qstemp = qs.mid(j + 1, i - j - 1);
    temp.RectRight = qstemp.toInt(&ok, 10);
    if((!ok) || temp.RectRight < 0)
    {
        EditBoundL->setText("");
        return;
    }

    if((temp.RectRight <= temp.RectLeft) || (temp.RectBottom <= temp.RectTop) ||
       (temp.RectRight > 703) || (temp.RectBottom > 575))   /* NTSC */
    {
        EditBoundL->setText("");
        return;
    }

    temp.RectRight = (temp.RectRight - temp.RectLeft) / 8 * 8 + temp.RectLeft;
    temp.RectBottom = (temp.RectBottom - temp.RectTop) / 8 * 8 + temp.RectTop;

    int indexC = CBChannelL->currentItem();
    if(indexC == 1)
    {
        w_full_area->MaskAddAllSet(&temp);
    }
    else if(indexC >= 2)
    {
        w_full_area->MaskAddSet(&temp);
    }

    CBRectangleL->insertItem(qs.sprintf("%d,%d,%d,%d", temp.RectTop, temp.RectBottom, temp.RectLeft, temp.RectRight));

    if(CBRectangleL->count() == 5)
        ButtonNewL->setEnabled(FALSE);

    EditBoundL->setText("");
    EditBoundL->setEnabled(FALSE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsLogoChangeSet(int value)
{
    int             index = CBChannelL->currentItem();

    QSlider*        Qtemp = (QSlider *) (this->sender());
    const char*     str = Qtemp->name();

    if(index == 1)
    {
        if(!strcmp(str, "SliderLogoPosXL"))
            w_full_area->LogoPosXAllSet(value);
        else if(!strcmp(str, "SliderLogoPosYL"))
            w_full_area->LogoPosYAllSet(value);
    }
    else if(index >= 2)
    {
        if(!strcmp(str, "SliderLogoPosXL"))
            w_full_area->LogoPosXSet(value);
        else if(!strcmp(str, "SliderLogoPosYL"))
            w_full_area->LogoPosYSet(value);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsLogoTransSet(bool on)
{
    int index = CBChannelL->currentItem();

    if(index == 1)
    {
        w_full_area->LogoTranslucentAllSet(on);
    }
    else if(index >= 2)
    {
        w_full_area->LogoTranslucentSet(on);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsLogoSet(bool on)
{
    int index = CBChannelL->currentItem();

    if(index == 1)
        w_full_area->LogoEnabledAllSet(on);
    else if(index >= 2)
        w_full_area->LogoEnabledSet(on);

    CheckLogoTransL->setEnabled(on);
    SliderLogoPosXL->setEnabled(on);
    SliderLogoPosYL->setEnabled(on);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsOsdChangeSet(int value)
{
    int             index = CBChannelL->currentItem();

    QSlider*        Qtemp = (QSlider *) (this->sender());
    const char*     str = Qtemp->name();

    if(index == 1)
    {
        if(!strcmp(str, "SliderOsdBrightnessL"))
            w_full_area->OsdBrightnessAllSet(value);
        else if(!strcmp(str, "SliderOsdPosXL"))
            w_full_area->OsdPosXAllSet(value);
        else if(!strcmp(str, "SliderOsdPosYL"))
            w_full_area->OsdPosYAllSet(value);
    }
    else if(index >= 2)
    {
        if(!strcmp(str, "SliderOsdBrightnessL"))
            w_full_area->OsdBrightnessSet(value);
        else if(!strcmp(str, "SliderOsdPosXL"))
            w_full_area->OsdPosXSet(value);
        else if(!strcmp(str, "SliderOsdPosYL"))
            w_full_area->OsdPosYSet(value);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsOsdTransSet(bool on)
{
    int index = CBChannelL->currentItem();

    if(index == 1)
    {
        /*
         * DSDEMO *dstemp = w_full_area -> GetDsinfo(0);
         */
        w_full_area->OsdTranslucentAllSet(on);
    }
    else if(index >= 2)
    {
        w_full_area->OsdTranslucentSet(on);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsOsdSet(bool on)
{
    int index = CBChannelL->currentItem();

    if(index == 1)
    {
        /*
         * DSDEMO *dstemp = w_full_area -> GetDsinfo(0);
         */
        w_full_area->OsdEnableAllSet(on);
    }
    else if(index >= 2)
    {
        w_full_area->OsdEnableSet(on);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsRestoreSet()
{
    int             index = CBChannelL->currentItem();
    video_info*     tempvideo;

    if(index == 1)
    {
        w_full_area->VideoRestoreAllSet();
        tempvideo = w_full_area->GetVideoDefault(0);

        SliderBrightnessL->setValue(tempvideo->Brightness);
        SliderContrastL->setValue(tempvideo->Contrast);
        SliderSaturationL->setValue(tempvideo->Saturation);
        SliderHueL->setValue(tempvideo->Hue);
    }
    else if(index >= 2)
    {
        w_full_area->VideoRestoreSet();
        tempvideo = w_full_area->GetVideoDefault(index - 2);

        SliderBrightnessL->setValue(tempvideo->Brightness);
        SliderContrastL->setValue(tempvideo->Contrast);
        SliderSaturationL->setValue(tempvideo->Saturation);
        SliderHueL->setValue(tempvideo->Hue);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsVideoSet(int value)
{
    int             index = CBChannelL->currentItem();

    QSlider*        Qtemp = (QSlider *) (this->sender());
    const char*     str = Qtemp->name();

    if(index == 1)
    {
        if(!strcmp(str, "SliderBrightnessL"))
            w_full_area->BrightnessAllSet(value);
        else if(!strcmp(str, "SliderContrastL"))
            w_full_area->ContrastAllSet(value);
        else if(!strcmp(str, "SliderSaturationL"))
            w_full_area->SaturationAllSet(value);
        else if(!strcmp(str, "SliderHueL"))
            w_full_area->HueAllSet(value);
    }
    else if(index >= 2)
    {
        if(!strcmp(str, "SliderBrightnessL"))
            w_full_area->BrightnessSet(value);
        else if(!strcmp(str, "SliderContrastL"))
            w_full_area->ContrastSet(value);
        else if(!strcmp(str, "SliderSaturationL"))
            w_full_area->SaturationSet(value);
        else if(!strcmp(str, "SliderHueL"))
            w_full_area->HueSet(value);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsFrameinfo()
{
    bool            ok;
    int             Qstemp;

    QLineEdit*      Qtemp = (QLineEdit *) (this->sender());
    int             index = CBChannelL->currentItem();

    const char*     str = Qtemp->name();

    if(index == 1)
    {
        DSDEMO*     temp = w_full_area->GetDsinfo(0);
        QString     Qs;

        if(!strcmp(str, "EditIQuantL"))
        {
            Qstemp = (EditIQuantL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12 || Qstemp > 30)
            {
                EditIQuantL->setText(Qs.sprintf("%9d", (temp->frameinfo).IQuantVal));
            }
            else
                w_full_area->IQuantAllSet(Qstemp);
        }
        else if(!strcmp(str, "EditBQuantL"))
        {
            Qstemp = (EditBQuantL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12 || Qstemp > 30)
            {
                EditBQuantL->setText(Qs.sprintf("%9d", (temp->frameinfo).BQuantVal));
            }
            else
                w_full_area->BQuantAllSet(Qstemp);
        }
        else if(!strcmp(str, "EditPQuantL"))
        {
            Qstemp = (EditPQuantL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12 || Qstemp > 30)
            {
                EditPQuantL->setText(Qs.sprintf("%9d", (temp->frameinfo).PQuantVal));
            }
            else
                w_full_area->PQuantAllSet(Qstemp);
        }
        else if(!strcmp(str, "EditKeyFrameL"))
        {
            Qstemp = (EditKeyFrameL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12)
            {
                EditKeyFrameL->setText(Qs.sprintf("%9d", (temp->frameinfo).KeyFrameIntervals));
            }
            else
                w_full_area->KeyFrameIAllSet(Qstemp);
        }
        else if(!strcmp(str, "EditBFramesL"))
        {
            Qstemp = (EditBFramesL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 0 || Qstemp > 2)
            {
                EditBFramesL->setText(Qs.sprintf("%9d", (temp->frameinfo).BFrames));
            }
            else
                w_full_area->BFramesAllSet(Qstemp);
        }
        else if(!strcmp(str, "EditFramesRL"))
        {
            Qstemp = (EditFramesRL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 1 || Qstemp > 30)
            {
                EditFramesRL->setText(Qs.sprintf("%9d", (temp->frameinfo).FrameRate));
            }
            else
                w_full_area->FrameRateAllSet(Qstemp);
        }
    }
    else if(index >= 2)
    {
        DSDEMO*     temp = w_full_area->GetDsinfo(index - 2);
        QString     Qs;

        if(!strcmp(str, "EditIQuantL"))
        {
            Qstemp = (EditIQuantL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12 || Qstemp > 30)
            {
                EditIQuantL->setText(Qs.sprintf("%9d", (temp->frameinfo).IQuantVal));
            }
            else
                w_full_area->IQuantSet(Qstemp);
        }
        else if(!strcmp(str, "EditBQuantL"))
        {
            Qstemp = (EditBQuantL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12 || Qstemp > 30)
            {
                EditBQuantL->setText(Qs.sprintf("%9d", (temp->frameinfo).BQuantVal));
            }
            else
                w_full_area->BQuantSet(Qstemp);
        }
        else if(!strcmp(str, "EditPQuantL"))
        {
            Qstemp = (EditPQuantL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12 || Qstemp > 30)
            {
                EditPQuantL->setText(Qs.sprintf("%9d", (temp->frameinfo).PQuantVal));
            }
            else
                w_full_area->PQuantSet(Qstemp);
        }
        else if(!strcmp(str, "EditKeyFrameL"))
        {
            Qstemp = (EditKeyFrameL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 12)
            {
                EditKeyFrameL->setText(Qs.sprintf("%9d", (temp->frameinfo).KeyFrameIntervals));
            }
            else
                w_full_area->KeyFrameISet(Qstemp);
        }
        else if(!strcmp(str, "EditBFramesL"))
        {
            Qstemp = (EditBFramesL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 0 || Qstemp > 2)
            {
                EditBFramesL->setText(Qs.sprintf("%9d", (temp->frameinfo).BFrames));
            }
            else
                w_full_area->BFramesSet(Qstemp);
        }
        else if(!strcmp(str, "EditFramesRL"))
        {
            Qstemp = (EditFramesRL->text()).toInt(&ok, 10);

            if((!ok) || Qstemp < 1 || Qstemp > 30)
            {
                EditFramesRL->setText(Qs.sprintf("%9d", (temp->frameinfo).FrameRate));
            }
            else
                w_full_area->FrameRateSet(Qstemp);
        }
    }
}

/*
 =======================================================================================================================
 * tyu add at 2007.02.08
 =======================================================================================================================
 */
void dsForm::dsMajorStream(int index)
{
    int ChannelNum = CBChannelL->currentItem();

    if(ChannelNum == 1)
        w_full_area->StreamTypeAllSet(index + 1);
    else if(ChannelNum >= 2)
        w_full_area->StreamTypeSet(index + 1);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMinorStream(int index)
{
    int ChannelNum = CBChannelL->currentItem();

    if(ChannelNum == 1)
        w_full_area->StreamTypeSubAllSet(index + 1);
    else if(ChannelNum >= 2)
        w_full_area->StreamTypeSubSet(index + 1);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMajorEncoder(int index)
{
    int             ChannelNum = CBChannelL->currentItem();
    PictureFormat_t pictureFormat = ENC_CIF_FORMAT;

    if(index == 0)
        pictureFormat = ENC_4CIF_FORMAT;
    else if(index == 1)
        pictureFormat = ENC_2CIF_FORMAT;
    else if(index == 2)
        pictureFormat = ENC_DCIF_FORMAT;
    else if(index == 3)
        pictureFormat = ENC_CIF_FORMAT;
    else if(index == 4)
        pictureFormat = ENC_QCIF_FORMAT;

    if(ChannelNum == 1)
        w_full_area->EncoderAllSet(pictureFormat);
    else if(ChannelNum >= 2)
        w_full_area->EncoderSet(pictureFormat);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMinorEncoder(int index)
{
    int             ChannelNum = CBChannelL->currentItem();
    PictureFormat_t pictureFormat = ENC_CIF_FORMAT;

    if(index == 0)
        pictureFormat = ENC_4CIF_FORMAT;
    else if(index == 1)
        pictureFormat = ENC_2CIF_FORMAT;
    else if(index == 2)
        pictureFormat = ENC_DCIF_FORMAT;
    else if(index == 3)
        pictureFormat = ENC_CIF_FORMAT;
    else if(index == 4)
        pictureFormat = ENC_QCIF_FORMAT;

    if(ChannelNum == 1)
        w_full_area->EncoderSubAllSet(pictureFormat);
    else if(ChannelNum >= 2)
        w_full_area->EncoderSubSet(pictureFormat);
}

#if 0

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsDCodingSet(bool on)
{
    if(on)
    {
        CheckMinorRecordL->setEnabled(TRUE);

        /*
         * CBMajorStreamL->setEnabled(TRUE);
         * *CBMinorStreamL->setEnabled(TRUE);
         */
        CBMinorEncoderL->setEnabled(TRUE);
        CBMinorEncoderL->setCurrentItem(3);
    }
    else
    {
        CheckMinorRecordL->setEnabled(FALSE);
        CBMinorEncoderL->setCurrentItem(3);
        CBMinorEncoderL->setEnabled(FALSE);
    }
}
#endif

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsBpsModeSet(int index)
{
    int ChannelNum = CBChannelL->currentItem();

    if(ChannelNum == 1)
        w_full_area->BitRateAllSet(atoi(EditMaxBpsL->text()), BitrateControlType_t(index));
    else if(ChannelNum >= 2)
        w_full_area->BitRateSet(atoi(EditMaxBpsL->text()), BitrateControlType_t(index));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMaxBpsSet()
{
    bool    ok;
    QString Qs;
    int     Qstemp;
    int     index = CBChannelL->currentItem();

    Qstemp = (EditMaxBpsL->text()).toInt(&ok, 10);

    if(index == 1)
    {
        DSDEMO*     temp = w_full_area->GetDsinfo(0);
        if((!ok) || Qstemp < 0)
        {
            EditMaxBpsL->setText(Qs.sprintf("%9d", temp->MaxBps));
        }
        else
        {
            w_full_area->BitRateAllSet(Qstemp, BitrateControlType_t(CBModeL->currentItem()));
        }
    }
    else if(index >= 2)
    {
        DSDEMO*     temp = w_full_area->GetDsinfo(index - 2);
        if((!ok) || Qstemp < 0)
        {
            EditMaxBpsL->setText(Qs.sprintf("%9d", temp->MaxBps));
        }
        else
        {
            w_full_area->BitRateSet(Qstemp, BitrateControlType_t(CBModeL->currentItem()));
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsChannel(const QString& Qstxt)
{
    int         port = atoi(Qstxt);
    DSDEMO*     temp = w_full_area->GetDsinfo(port);

    if(!temp->record_flag)
    {
        w_audio_frame->setText(QString("        0"));
        w_totalframe->setText(QString("        0"));
        w_bit_rate->setText(QString("        0"));
        w_video_frame->setText(QString("        0"));
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMinorRecordSet(bool on)
{
    int             ChannelNum = CBChannelL->currentItem();
    int             index = CBMinorEncoderL->currentItem();
    PictureFormat_t pictureFormat = ENC_CIF_FORMAT;

    if(index == 0)
        pictureFormat = ENC_4CIF_FORMAT;
    else if(index == 1)
        pictureFormat = ENC_2CIF_FORMAT;
    else if(index == 2)
        pictureFormat = ENC_DCIF_FORMAT;
    else if(index == 3)
        pictureFormat = ENC_CIF_FORMAT;
    else if(index == 4)
        pictureFormat = ENC_QCIF_FORMAT;

    if(ChannelNum == 1)
    {
        if(on)
        {
            CBMinorEncoderL->setEnabled(FALSE);
            CBMinorStreamL->setEnabled(FALSE);
            w_full_area->StreamTypeSubAllSet(CBMinorStreamL->currentItem() + 1);
            w_full_area->EncoderSubAllSet(pictureFormat);
        }
        else
        {
            CBMinorEncoderL->setEnabled(TRUE);
            CBMinorStreamL->setEnabled(TRUE);
        }

        w_full_area->RecordSubAllSet(on);
    }
    else if(ChannelNum >= 2)
    {
        if(on)
        {
            CBMinorEncoderL->setEnabled(FALSE);
            CBMinorStreamL->setEnabled(FALSE);
            w_full_area->EncoderSubSet(pictureFormat);
            w_full_area->StreamTypeSubSet(CBMinorStreamL->currentItem() + 1);
        }
        else
        {
            CBMinorEncoderL->setEnabled(TRUE);
            CBMinorStreamL->setEnabled(TRUE);
        }

        w_full_area->RecordSubSet(on);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::dsMajorRecordSet(bool on)
{
    int             ChannelNum = CBChannelL->currentItem();
    int             index = CBMajorEncoderL->currentItem();
    PictureFormat_t pictureFormat = ENC_CIF_FORMAT;

    if(index == 0)
        pictureFormat = ENC_4CIF_FORMAT;
    else if(index == 1)
        pictureFormat = ENC_2CIF_FORMAT;
    else if(index == 2)
        pictureFormat = ENC_DCIF_FORMAT;
    else if(index == 3)
        pictureFormat = ENC_CIF_FORMAT;
    else if(index == 4)
        pictureFormat = ENC_QCIF_FORMAT;

    if(ChannelNum == 1)
    {
        if(on)
        {
            CBMajorEncoderL->setEnabled(FALSE);
            CBMajorStreamL->setEnabled(FALSE);
            w_full_area->EncoderAllSet(pictureFormat);
            w_full_area->StreamTypeAllSet(CBMajorStreamL->currentItem() + 1);
        }
        else
        {
            CBMajorEncoderL->setEnabled(TRUE);
            CBMajorStreamL->setEnabled(TRUE);
        }

        w_full_area->RecordAllSet(on);
    }
    else if(ChannelNum >= 2)
    {
        if(on)
        {
            CBMajorEncoderL->setEnabled(FALSE);
            CBMajorStreamL->setEnabled(FALSE);
            w_full_area->EncoderSet(pictureFormat);
            w_full_area->StreamTypeSet(CBMajorStreamL->currentItem() + 1);
        }
        else
        {
            CBMajorEncoderL->setEnabled(TRUE);
            CBMajorStreamL->setEnabled(TRUE);
        }

        w_full_area->RecordSet(on);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::areaClick(int click)
{
    static int  fullflag = 1;
    int         port;
    QString     qS;

    port = w_full_area->GetSelectPort();

    w_channel->setText(qS.sprintf("%9d", port));

    if(click == RIGHTCLICKED)
    {
        if(fullflag)
        {
            disconnect(tabWidget3, 0, 0, 0);
            TitleEdit->hide();
            tabWidget3->hide();
            showFullScreen();
            w_full_area->resize(1024, 768);
            fullflag = 0;
        }
        else
        {
            w_full_area->resize(800, 700);
            showNormal();
            TitleEdit->show();
            tabWidget3->show();
            fullflag = 1;
            connect(tabWidget3, SIGNAL(currentChanged (QWidget*)), SLOT(PageChange (QWidget*)));
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::audioChangeSet(int Select, float data)
{
    QString qS;

    if(Select == atoi(w_channel->text()))
        w_audio_frame->setText(qS.sprintf("%10.1f", data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::videoChangeSet(int Select, float data)
{
    QString qS;

    if(Select == atoi(w_channel->text()))
        w_video_frame->setText(qS.sprintf("%10.1f", data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::totalChangeSet(int Select, int data)
{
    QString qS;

    if(Select == atoi(w_channel->text()))
        w_totalframe->setText(qS.sprintf("%9d", data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::bitRateChangeSet(int Select, int data)
{
    QString qS;

    if(Select == atoi(w_channel->text()))
        w_bit_rate->setText(qS.sprintf("%9d", data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::countChangeSet(int data)
{
    QString qS;

    w_netusercount->setText(qS.sprintf("%9d", data));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::fullSpaceSet()
{
    static int  run = 0;
    FormInfo*   forminf;

    pthread_mutex_lock(&mutex);
    if(!run)
    {
        forminf = new FormInfo(this, "Information", Qt::WType_TopLevel);
        forminf->setGeometry(QRect(300, 300, 200, 120));
        forminf->show();
        run = 1;
    }

    pthread_mutex_unlock(&mutex);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::noNetSet()
{
    QMessageBox::warning(this, "MessageBox", "The net transfer setting isn't opened!",
                         QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::netChangeSet(bool on)
{
    OutputDebugString("In netChangeSet \n");
    w_full_area->NetTransferSet(on);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::checkspace()
{
    char            savepath[10] = ".";
    int             freespace;
    struct statfs   bbuf;

    if(statfs(savepath, &bbuf) < 0)
    {
        OutputErrorString("statsfs() faied, error: %d\n", errno);
    }
    else
    {
        freespace = bbuf.f_bavail * (bbuf.f_bsize / 1024);
        if(freespace < RunQuest)
        {
            OutputErrorString("The Space of Disk isn't enough!! So you'd better stop Recording\n");
            CheckMajorRecordL->setEnabled(FALSE);
            CheckMinorRecordL->setEnabled(FALSE);
            CheckImageCaptureL->setEnabled(FALSE);
        }
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::changeEdit()
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
void dsForm::PageChange(QWidget* CurrentPage)   /* 状态切换的时候,注意以前的状态 */
{
    static int  index = 0;
    static int  index_before = 0;
    int         tempindex;
    bool        tempfull;

    if(!strcmp(CurrentPage->name(), "tab"))
    {
        if(index == 2 && index_before == 0)
        {
            index = 0;
            return;
        }

        tempindex = CBChannelL->currentItem();
        tempfull = w_full_area->GetFullState();

        if(GFullArea)
        {
            w_full_area->SetSelectPort(GSelectPort);
            if(tempfull)
                w_full_area->ChangeFullPort();
            else
                w_full_area->SetFullEnable(TRUE);
        }
        else
        {
            if(tempfull)
                w_full_area->SetFullEnable(FALSE);
            w_full_area->SetSelectPort(GSelectPort);
        }

        disconnect(w_preview, 0, 0, 0);
        w_preview->setChecked(TRUE);
        connect(w_preview, SIGNAL(toggled(bool)), w_full_area, SLOT(PreviewAllSet(bool)));

        disconnect(w_record, 0, 0, 0);
        w_record->setChecked(bool(w_full_area->AllRecordStatus()));
        connect(w_record, SIGNAL(toggled(bool)), w_full_area, SLOT(RecordAllSet(bool)));

        GSelectPort = tempindex;
        GFullArea = tempfull;
        index = 0;
    }
    else if(!strcmp(CurrentPage->name(), "tab_2"))
    {
        fprintf(stderr, "Enter Page Two\n");
        if(index == 2 && index_before == 1)
        {
            index = 1;
            return;
        }

        w_full_area->PreviewAllSet(TRUE);   /* 保证切换到Localsetting时是正确的 */

        /*
         * disconnect(CheckMajorRecordL, 0, 0,0);
         * *CheckMajorRecordL->setChecked(w_record->isChecked());
         * *connect(CheckMajorRecordL, SIGNAL(toggled(bool)), SLOT(dsMajorRecordSet(bool)));
         */
        tempindex = w_full_area->GetSelectPort();
        tempfull = w_full_area->GetFullState();

        if(GFullArea)
        {
            if(GSelectPort == 0)
            {
                SetAllEnabled(FALSE);
                w_full_area->SetSelectPort(w_full_area->GetLastPort());
                w_full_area->SetFullEnable(TRUE);
            }
            else
            {
                w_full_area->SetSelectPort(GSelectPort - 2);
                w_full_area->SetFullEnable(TRUE);
            }
        }
        else
        {
            if(tempfull)
                w_full_area->SetFullEnable(FALSE);
            if(GSelectPort == 0)
                SetAllEnabled(FALSE);
            w_full_area->SetSelectPort(w_full_area->GetLastPort());
        }

        GSelectPort = tempindex;
        GFullArea = tempfull;
        index = 1;
    }
    else
    {
        if(index == 0)
            index_before = 0;
        else if(index == 1)
            index_before = 1;
        index = 2;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
bool dsForm::eventFilter(QObject* o, QEvent* e)
{
    int index = tabWidget3->currentPageIndex();

    if((o == w_full_area) && (index == 1 || index == 2))
    {
        if(((e->type() == QEvent::MouseButtonPress) || (e->type() == QEvent::MouseButtonRelease) ||
           (e->type() == QEvent::MouseButtonDblClick)))
        {
            OutputDebugString("eventFilter 1\n");
            return TRUE;
        }
    }

    return QWidget::eventFilter(o, e);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::SetComBox()
{
    int         temp, num;
    QString     Qs;

    DSDEMO*     dstemp = w_full_area->GetDsinfo(0);

    num = GetTotalChannels();
    CBChannelL->insertItem("NoSet");
    CBChannelL->insertItem("All");
    for(temp = 0; temp < num; temp++)
    {
        Qs.sprintf("Channel %2d", temp);
        CBChannelL->insertItem(Qs);
    }

    CBMajorStreamL->insertItem("VIDEO");
    CBMajorStreamL->insertItem("SOUND");
    CBMajorStreamL->insertItem("AVSYNC");
    CBMinorStreamL->insertItem("VIDEO");
    CBMinorStreamL->insertItem("SOUND");
    CBMinorStreamL->insertItem("AVSYNC");

    /*
     * CBMajorEncoderL->insertItem("No Set");
     */
    CBMajorEncoderL->insertItem("4CIF");
    CBMajorEncoderL->insertItem("2CIF");
    CBMajorEncoderL->insertItem("DCIF");
    CBMajorEncoderL->insertItem("CIF");
    CBMajorEncoderL->insertItem("QCIF");

    /*
     * CBMinorEncoderL->insertItem("No Set");
     */
    CBMinorEncoderL->insertItem("4CIF");
    CBMinorEncoderL->insertItem("2CIF");
    CBMinorEncoderL->insertItem("DCIF");
    CBMinorEncoderL->insertItem("CIF");
    CBMinorEncoderL->insertItem("QCIF");

    CBModeL->insertItem("Constant");
    CBModeL->insertItem("Varied");

    SliderBrightnessL->setMinValue(0);
    SliderBrightnessL->setMaxValue(255);
    SliderBrightnessL->setLineStep(1);
    SliderBrightnessL->setPageStep(10);

    SliderContrastL->setMinValue(0);
    SliderContrastL->setMaxValue(127);
    SliderContrastL->setLineStep(1);
    SliderContrastL->setPageStep(10);

    SliderSaturationL->setMinValue(0);
    SliderSaturationL->setMaxValue(127);
    SliderSaturationL->setLineStep(1);
    SliderSaturationL->setPageStep(10);

    SliderHueL->setMinValue(0);
    SliderHueL->setMaxValue(255);
    SliderHueL->setLineStep(1);
    SliderHueL->setPageStep(10);

    SliderOsdBrightnessL->setMinValue(0);
    SliderOsdBrightnessL->setMaxValue(255);
    SliderOsdBrightnessL->setLineStep(1);
    SliderOsdBrightnessL->setPageStep(10);

    SliderOsdPosXL->setMinValue(0);
    SliderOsdPosXL->setMaxValue(351);
    SliderOsdPosXL->setLineStep(8);
    SliderOsdPosXL->setPageStep(32);

    SliderOsdPosYL->setMinValue(0);
    SliderOsdPosYL->setLineStep(1);
    SliderOsdPosYL->setPageStep(10);

    SliderLogoPosXL->setMinValue(0);
    SliderLogoPosXL->setMaxValue(703);
    SliderLogoPosXL->setLineStep(1);
    SliderLogoPosXL->setPageStep(10);

    SliderLogoPosYL->setMinValue(0);
    SliderLogoPosYL->setLineStep(1);
    SliderLogoPosYL->setPageStep(10);
    if(dstemp->videostandard == StandardNTSC)
    {
        SliderOsdPosYL->setMaxValue(479);
        SliderLogoPosYL->setMaxValue(479 - 32);
        CBImageStreamHL->insertItem("480");
        CBImageStreamHL->insertItem("240");
        CBImageStreamHL->insertItem("120");
        CBImageStreamHL->insertItem("60");
    }
    else
    {
        SliderOsdPosYL->setMaxValue(575);
        SliderLogoPosYL->setMaxValue(575 - 32);
        CBImageStreamHL->insertItem("576");
        CBImageStreamHL->insertItem("288");
        CBImageStreamHL->insertItem("144");
        CBImageStreamHL->insertItem("72");
    }

    SliderMotionSensL->setMinValue(0);
    SliderMotionSensL->setMaxValue(6);
    SliderMotionSensL->setLineStep(1);
    SliderMotionSensL->setPageStep(1);

    CBImageStreamWL->insertItem("704");
    CBImageStreamWL->insertItem("352");
    CBImageStreamWL->insertItem("176");
    CBImageStreamWL->insertItem("88");

    SliderPictureQL->setMinValue(0);
    SliderPictureQL->setMaxValue(100);
    SliderPictureQL->setLineStep(1);
    SliderPictureQL->setPageStep(10);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::SetAllEnabled(bool on)
{
    QObjectList*    qlist;
    qlist = groupBox7->queryList("QWidget");

    QObjectListIt   it(*qlist);
    QObject*        obj;

    while((obj = it.current()) != 0)
    {
        ++it;
        if((strcmp(obj->name(), "LabelChannelL") == 0) || (strcmp(obj->name(), "CBChannelL") == 0))
            continue;
        ((QWidget*)obj)->setEnabled(on);
    }

    delete qlist;

    qlist = CBChannelL->queryList("QWidget");

    QObjectListIt   itemp(*qlist);
    while((obj = itemp.current()) != 0)
    {
        ++itemp;
        ((QWidget*)obj)->setEnabled(TRUE);
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::SetALLValueDefault()
{
    QString temp;

    CheckPreviewLl->setChecked(TRUE);
    CheckMajorRecordL->setEnabled(TRUE);
    CheckMinorRecordL->setEnabled(TRUE);
    CheckMajorRecordL->setChecked(FALSE);
    CheckMinorRecordL->setChecked(FALSE);

    CBMajorStreamL->setCurrentItem(2);
    CBMajorStreamL->setEnabled(TRUE);
    CBMinorStreamL->setCurrentItem(2);
    CBMinorStreamL->setEnabled(TRUE);

    EditIQuantL->setText(temp.sprintf("%9d", 12));
    EditBQuantL->setText(temp.sprintf("%9d", 12));
    EditPQuantL->setText(temp.sprintf("%9d", 17));

    EditKeyFrameL->setText(temp.sprintf("%9d", 25));
    EditBFramesL->setText(temp.sprintf("%9d", 2));
    EditPFramesL->setText(temp.sprintf("    Invalid"));
    EditFramesRL->setText(temp.sprintf("%9d", 25));

    CBMajorEncoderL->setCurrentItem(3);
    CBMinorEncoderL->setCurrentItem(3);
    CBMajorEncoderL->setEnabled(TRUE);
    CBMinorEncoderL->setEnabled(TRUE);

    EditMaxBpsL->setText(temp.sprintf("%9d", 768000));
    CBModeL->setCurrentItem(1);

    CBModeL->setCurrentItem(1);

    video_info*     tempvideo = w_full_area->GetVideoDefault(0);
    SliderBrightnessL->setValue(tempvideo->Brightness);
    SliderContrastL->setValue(tempvideo->Contrast);
    SliderSaturationL->setValue(tempvideo->Saturation);
    SliderHueL->setValue(tempvideo->Hue);

    CheckOsdL->setChecked(TRUE);
    CheckTranslucentL->setChecked(TRUE);
    SliderOsdBrightnessL->setValue(255);
    SliderOsdPosXL->setValue(76);
    SliderOsdPosYL->setValue(240);

    CheckLogoL->setChecked(TRUE);
    CheckLogoTransL->setChecked(FALSE);
    SliderLogoPosXL->setValue(576);
    SliderLogoPosYL->setValue(448);

    CheckMaskL->setChecked(FALSE);
    CBRectangleL->setEnabled(FALSE);

    int i, j;
    j = CBRectangleL->count();
    for(i = 0; i < j; i++)
        CBRectangleL->removeItem(0);
    CheckMaskL->setChecked(TRUE);
    dsMaskSet(TRUE);

    CheckMotionDSL->setEnabled(TRUE);
    CheckMotionDSL->setChecked(FALSE);
    SliderMotionSensL->setEnabled(FALSE);

    CheckAudioL->setEnabled(FALSE);
    CheckAudioL->setChecked(FALSE);
    EditSoundLevelL->setEnabled(FALSE);

    CheckImageCaptureL->setEnabled(TRUE);
    CheckImageCaptureL->setChecked(FALSE);
    CBImageStreamWL->setEnabled(TRUE);
    CBImageStreamHL->setEnabled(TRUE);
    ButtonCaptureL->setEnabled(FALSE);

    SliderPictureQL->setValue(100);
    ButtonCaptureL->setEnabled(FALSE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::AccordDsSet(int index)
{
    fprintf(stderr, "Enter into AccordDsSet\n");

    QString     Qstemp;
    int         StreamType;
    DSDEMO*     temp = w_full_area->GetDsinfo(index);

    OutputDebugString("port = %d , record = %d\n", index, int(temp->record_flag));

    CheckPreviewLl->setChecked(TRUE);   /* 自动打开preview */

    /*
     * disconnect(CheckMajorRecordL);
     */
    CheckMajorRecordL->setChecked(bool(temp->record_flag));
    CBMajorStreamL->setEnabled(!(bool(temp->record_flag)));
    CBMajorEncoderL->setEnabled(!(bool(temp->record_flag)));
    if(temp->record_flag)
    {
        GetStreamType(temp->ChannelHandle, &StreamType);
        CBMajorStreamL->setCurrentItem(StreamType - 1);
    }
    else
        CBMajorStreamL->setCurrentItem(2);

    CheckMinorRecordL->setEnabled(TRUE);
    CheckMinorRecordL->setChecked(bool(temp->cifqcif_flag));
    CBMinorStreamL->setEnabled(!(bool(temp->cifqcif_flag)));
    CBMinorEncoderL->setEnabled(!(bool(temp->cifqcif_flag)));
    if(temp->cifqcif_flag)
    {
        GetSubStreamType(temp->ChannelHandle, &StreamType);
        CBMinorStreamL->setCurrentItem(StreamType - 1);
    }
    else
        CBMinorStreamL->setCurrentItem(2);

    EditIQuantL->setText(Qstemp.sprintf("%9d", (temp->frameinfo).IQuantVal));
    EditBQuantL->setText(Qstemp.sprintf("%9d", (temp->frameinfo).PQuantVal));
    EditPQuantL->setText(Qstemp.sprintf("%9d", (temp->frameinfo).BQuantVal));
    EditKeyFrameL->setText(Qstemp.sprintf("%9d", (temp->frameinfo).KeyFrameIntervals));
    EditBFramesL->setText(Qstemp.sprintf("%9d", (temp->frameinfo).BFrames));
    EditPFramesL->setText(Qstemp.sprintf("    Invalid"));
    EditFramesRL->setText(Qstemp.sprintf("%9d", (temp->frameinfo).FrameRate));

    if(temp->cifqcif_flag == 1)
    {
        if(temp->subpictureFormat == ENC_CIF_FORMAT)
            CBMinorEncoderL->setCurrentItem(3);
        else if(temp->subpictureFormat == ENC_QCIF_FORMAT)
            CBMinorEncoderL->setCurrentItem(4);
        else if(temp->subpictureFormat == ENC_2CIF_FORMAT)
            CBMinorEncoderL->setCurrentItem(1);
        else if(temp->subpictureFormat == ENC_4CIF_FORMAT)
            CBMinorEncoderL->setCurrentItem(0);
        else if(temp->subpictureFormat == ENC_DCIF_FORMAT)
            CBMinorEncoderL->setCurrentItem(2);
    }
    else
        CBMinorEncoderL->setCurrentItem(3);

    if(temp->pictureFormat == ENC_CIF_FORMAT)
        CBMajorEncoderL->setCurrentItem(3);
    else if(temp->subpictureFormat == ENC_QCIF_FORMAT)
        CBMajorEncoderL->setCurrentItem(4);
    else if(temp->subpictureFormat == ENC_2CIF_FORMAT)
        CBMajorEncoderL->setCurrentItem(1);
    else if(temp->subpictureFormat == ENC_4CIF_FORMAT)
        CBMajorEncoderL->setCurrentItem(0);
    else if(temp->subpictureFormat == ENC_DCIF_FORMAT)
        CBMajorEncoderL->setCurrentItem(2);

    EditMaxBpsL->setText(Qstemp.sprintf("%9d", temp->MaxBps));
    CBModeL->setCurrentItem(temp->brc);

    video_info*     tempvideo;
    tempvideo = &(temp->v_info);
    SliderBrightnessL->setValue(tempvideo->Brightness);
    SliderContrastL->setValue(tempvideo->Contrast);
    SliderSaturationL->setValue(tempvideo->Saturation);
    SliderHueL->setValue(tempvideo->Hue);
    OutputDebugString("default Brightness : %d\n",
                      ((w_full_area->GetVideoDefault(index))->Brightness));
    OutputDebugString("default hue : %d\n", ((w_full_area->GetVideoDefault(index))->Hue));

    Osd_info*   temposd;
    temposd = &(temp->osdinfo);
    if(temp->osd_flag)
    {
        CheckOsdL->setChecked(TRUE);
        CheckTranslucentL->setChecked(bool(temposd->Translucent));
        SliderOsdBrightnessL->setValue(temposd->Brightness);
        SliderOsdPosXL->setValue(temposd->PosX);
        SliderOsdPosYL->setValue(temposd->PosY);
    }
    else
    {
        CheckOsdL->setChecked(FALSE);
        CheckTranslucentL->setChecked(FALSE);
        CheckTranslucentL->setEnabled(FALSE);
        SliderOsdBrightnessL->setValue(0);
        SliderOsdBrightnessL->setEnabled(FALSE);
        SliderOsdPosXL->setValue(0);
        SliderOsdPosXL->setEnabled(FALSE);
        SliderOsdPosYL->setValue(0);
        SliderOsdPosYL->setEnabled(FALSE);
    }

    Logo_info*  templogo;

    templogo = &(temp->logoinfo);
    if(temp->logo_flag)
    {
        CheckLogoL->setChecked(TRUE);
        CheckLogoTransL->setChecked(bool(templogo->Translucent));
        SliderLogoPosXL->setValue(templogo->PosX);
        SliderLogoPosYL->setValue(templogo->PosY);
    }
    else
    {
        CheckLogoL->setChecked(FALSE);
        CheckLogoTransL->setChecked(FALSE);
        CheckLogoTransL->setEnabled(FALSE);
        SliderLogoPosXL->setValue(0);
        SliderLogoPosXL->setEnabled(FALSE);
        SliderLogoPosYL->setValue(0);
        SliderLogoPosYL->setEnabled(FALSE);
    }

    int i, j;
    j = CBRectangleL->count();

    RECT*   temprect = w_full_area->GetMaskinfo(index);
    QString QstempRect;
    for(i = 0; i < j; i++)
        CBRectangleL->removeItem(0);

    for(i = 0; i < 5; i++)
    {
        if((temprect[i].RectTop || temprect[i].RectBottom || temprect[i].RectLeft ||
           temprect[i].RectRight))
        {
            CBRectangleL->insertItem(QstempRect.sprintf("%d,%d,%d,%d", temprect[i].RectTop,
                                     temprect[i].RectBottom, temprect[i].RectLeft,
                                     temprect[i].RectRight));
        }
    }

    w_full_area->MaskEnableSet(TRUE);
    CheckMaskL->setChecked(TRUE);
    dsMaskSet(TRUE);

    if(temp->motion_flag)
    {
        CheckMotionDSL->setChecked(TRUE);
        SliderMotionSensL->setValue(temp->motionSens);
    }
    else
    {
        CheckMotionDSL->setChecked(FALSE);
        SliderMotionSensL->setValue(0);
        SliderMotionSensL->setEnabled(FALSE);
    }

    CheckImageCaptureL->setEnabled(TRUE);
    if(temp->imagefile_flag)
    {
        CheckImageCaptureL->setChecked(TRUE);
        CBImageStreamWL->setEnabled(FALSE);
        CBImageStreamHL->setEnabled(FALSE);
    }
    else
    {
        CheckImageCaptureL->setChecked(FALSE);
        CBImageStreamWL->setEnabled(TRUE);
        CBImageStreamHL->setEnabled(TRUE);
    }

    CheckAudioL->setChecked(bool(temp->audio_flag));
    CheckAudioL->setEnabled(TRUE);
    EditSoundLevelL->setText(Qstemp.sprintf("%9d", GetSoundLevel(index)));
    EditSoundLevelL->setEnabled(FALSE);

    ButtonCaptureL->setEnabled(FALSE);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::OpenSetting(int index)
{
    static int  g_index = 0;

    OutputDebugString("g_index = %d, index =%d \n", g_index, index);
    if(index == g_index)
        return;

    if(index == 0)
    {
        SetAllEnabled(FALSE);
        g_index = index;
        return;
    }
    else
        SetAllEnabled(TRUE);

    if(index == 1)
    {
        switch
        (
            QMessageBox::information(this, "MessageBox", "Do you want to set for all channels?",
                                     QMessageBox::Ok, QMessageBox::Cancel, QMessageBox::NoButton)
        )
        {
            case 1:
                CheckPreviewLl->setEnabled(FALSE);
                w_full_area->SetFullEnable(FALSE);

                DisconnectCheck();
                SetALLValueDefault();
                ConnectCheck();

                w_full_area->SetDsclassDefault();
                g_index = index;
                break;

            case 2:
                CBChannelL->setCurrentItem(g_index);
                if(g_index == 0)
                    SetAllEnabled(FALSE);
                break;
        }
    }
    else
    {
        if(index >= 2)
        {
            CheckPreviewLl->setEnabled(FALSE);

            DisconnectCheck();
            AccordDsSet(index - 2);
            ConnectCheck();

            w_full_area->SetSelectPort(index - 2);
            w_full_area->SetFullEnable(TRUE);
        }

        g_index = index;
    }
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::DisconnectCheck()
{
    disconnect(CheckMajorRecordL, 0, 0, 0);
    disconnect(CheckMinorRecordL, 0, 0, 0);

    /*
     * disconnect( CheckMajorRecordL, 0, 0, 0 );
     */
    disconnect(EditIQuantL, 0, 0, 0);
    disconnect(EditBQuantL, 0, 0, 0);
    disconnect(EditPQuantL, 0, 0, 0);
    disconnect(EditKeyFrameL, 0, 0, 0);
    disconnect(EditBFramesL, 0, 0, 0);
    disconnect(EditFramesRL, 0, 0, 0);

    /*
     * disconnect(CBMajorEncoderL, 0, 0, 0);
     * ;
     * tyu add at 2007.02.08
     */
    disconnect(CBMajorEncoderL, 0, 0, 0);
    disconnect(CBMinorEncoderL, 0, 0, 0);
    disconnect(CBMajorStreamL, 0, 0, 0);
    disconnect(CBMinorStreamL, 0, 0, 0);

    disconnect(EditMaxBpsL, 0, 0, 0);
    disconnect(CBModeL, 0, 0, 0);
    disconnect(SliderBrightnessL, 0, 0, 0);
    disconnect(SliderContrastL, 0, 0, 0);
    disconnect(SliderSaturationL, 0, 0, 0);
    disconnect(SliderHueL, 0, 0, 0);

    disconnect(CheckOsdL, 0, 0, 0);
    disconnect(CheckTranslucentL, 0, 0, 0);
    disconnect(SliderOsdBrightnessL, 0, 0, 0);
    disconnect(SliderOsdPosXL, 0, 0, 0);
    disconnect(SliderOsdPosYL, 0, 0, 0);

    disconnect(CheckLogoL, 0, 0, 0);
    disconnect(CheckLogoTransL, 0, 0, 0);
    disconnect(SliderLogoPosXL, 0, 0, 0);
    disconnect(SliderLogoPosYL, 0, 0, 0);

    disconnect(CheckMaskL, 0, 0, 0);
    disconnect(CBRectangleL, 0, 0, 0);
    disconnect(EditBoundL, 0, 0, 0);
    disconnect(ButtonNewL, 0, 0, 0);
    disconnect(ButtonDeleteL, 0, 0, 0);
    disconnect(CheckMotionDSL, 0, 0, 0);
    disconnect(SliderMotionSensL, 0, 0, 0);

    disconnect(CheckAudioL, 0, 0, 0);
    disconnect(RadioBmpL, 0, 0, 0);
    disconnect(RadioJpegL, 0, 0, 0);
    disconnect(ButtonCaptureL, 0, 0, 0);
    disconnect(CheckImageCaptureL, 0, 0, 0);
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
void dsForm::ConnectCheck()
{
    connect(CheckMajorRecordL, SIGNAL(toggled(bool)), SLOT(dsMajorRecordSet(bool)));
    connect(CheckMinorRecordL, SIGNAL(toggled(bool)), SLOT(dsMinorRecordSet(bool)));

    /*
     * connect(CheckMinorRecordL, SIGNAL(toggled(bool)), SLOT(dsMinorRecordSet(bool)));
     */
    connect(EditIQuantL, SIGNAL(returnPressed()), SLOT(dsFrameinfo()));
    connect(EditBQuantL, SIGNAL(returnPressed()), SLOT(dsFrameinfo()));
    connect(EditPQuantL, SIGNAL(returnPressed()), SLOT(dsFrameinfo()));
    connect(EditKeyFrameL, SIGNAL(returnPressed()), SLOT(dsFrameinfo()));
    connect(EditBFramesL, SIGNAL(returnPressed()), SLOT(dsFrameinfo()));
    connect(EditFramesRL, SIGNAL(returnPressed()), SLOT(dsFrameinfo()));

    /* tyu add at 2007.02.08 */
    connect(CBMajorEncoderL, SIGNAL(activated (int)), SLOT(dsMajorEncoder (int)));
    connect(CBMinorEncoderL, SIGNAL(activated (int)), SLOT(dsMinorEncoder (int)));
    connect(CBMajorStreamL, SIGNAL(activated (int)), SLOT(dsMajorStream (int)));
    connect(CBMinorStreamL, SIGNAL(activated (int)), SLOT(dsMinorStream (int)));

    connect(EditMaxBpsL, SIGNAL(returnPressed()), SLOT(dsMaxBpsSet()));
    connect(CBModeL, SIGNAL(activated (int)), SLOT(dsBpsModeSet (int)));
    connect(SliderBrightnessL, SIGNAL(valueChanged (int)), SLOT(dsVideoSet (int)));
    connect(SliderContrastL, SIGNAL(valueChanged (int)), SLOT(dsVideoSet (int)));
    connect(SliderSaturationL, SIGNAL(valueChanged (int)), SLOT(dsVideoSet (int)));
    connect(SliderHueL, SIGNAL(valueChanged (int)), SLOT(dsVideoSet (int)));
    connect(ButtonVideoRestoreL, SIGNAL(clicked()), SLOT(dsRestoreSet()));

    connect(CheckOsdL, SIGNAL(toggled(bool)), SLOT(dsOsdSet(bool)));
    connect(CheckTranslucentL, SIGNAL(toggled(bool)), SLOT(dsOsdTransSet(bool)));
    connect(SliderOsdBrightnessL, SIGNAL(valueChanged (int)), SLOT(dsOsdChangeSet (int)));
    connect(SliderOsdPosXL, SIGNAL(valueChanged (int)), SLOT(dsOsdChangeSet (int)));
    connect(SliderOsdPosYL, SIGNAL(valueChanged (int)), SLOT(dsOsdChangeSet (int)));

    connect(CheckLogoL, SIGNAL(toggled(bool)), SLOT(dsLogoSet(bool)));
    connect(CheckLogoTransL, SIGNAL(toggled(bool)), SLOT(dsLogoTransSet(bool)));
    connect(SliderLogoPosXL, SIGNAL(valueChanged (int)), SLOT(dsLogoChangeSet (int)));
    connect(SliderLogoPosYL, SIGNAL(valueChanged (int)), SLOT(dsLogoChangeSet (int)));

    connect(CheckMaskL, SIGNAL(toggled(bool)), SLOT(dsMaskSet(bool)));
    connect(CBRectangleL, SIGNAL(activated (int)), SLOT(dsRectangleChange (int)));
    connect(EditBoundL, SIGNAL(returnPressed()), SLOT(dsMaskEditPress()));
    connect(ButtonNewL, SIGNAL(clicked()), SLOT(dsNewButtonClicked()));
    connect(ButtonDeleteL, SIGNAL(clicked()), SLOT(dsDeleteButtonClicked()));

    connect(CheckMotionDSL, SIGNAL(toggled(bool)), SLOT(dsMotionSet(bool)));
    connect(SliderMotionSensL, SIGNAL(valueChanged (int)), SLOT(dsMotionSenSet (int)));

    connect(CheckAudioL, SIGNAL(toggled(bool)), SLOT(dsAudioSet(bool)));

    connect(RadioBmpL, SIGNAL(toggled(bool)), SLOT(dsBmpSet(bool)));
    connect(RadioJpegL, SIGNAL(toggled(bool)), SLOT(dsJpegSet(bool)));
    connect(ButtonCaptureL, SIGNAL(clicked()), SLOT(dsCaptureButtonClicked()));

    connect(CheckImageCaptureL, SIGNAL(toggled(bool)), SLOT(dsImageStreamSet(bool)));
}

/*
 =======================================================================================================================
 * Sets the strings of the subwidgets using the current language.
 =======================================================================================================================
 */
void dsForm::languageChange()
{
    setCaption(tr("Ds4008HC"));
    groupBoxglobal->setTitle(QString::null);
    groupBoxGCI->setTitle(tr("Channel Info"));
    ChannelNumLabel->setText(tr("Channel Num"));
    AudioRateLabel->setText(tr("Audio Frame Rate"));
    TotalFrameLabel->setText(tr("Total Frame"));
    BitRateLabel->setText(tr("BitRate(bps)"));
    VideoRateLabel->setText(tr("Video Frame Rate"));
    groupBoxGNS->setTitle(tr("Net Setting"));
    NetCountLabel->setText(tr("Net User Count"));
    w_net_transfer->setText(tr("NetTransfer"));
    w_stringtoclient->setText(tr("StringToClient"));
    w_preview->setText(tr("All Preview"));
    w_record->setText(tr("All Record"));
    w_exit->setText(tr("Exit"));
    groupBoxGSI->setTitle(tr("Status Information"));
    FreeSpaceLabel->setText(tr("Free space"));
    TimeLabel->setText(tr("  Time"));
    DateLabel->setText(tr("  Date"));
    tabWidget3->changeTab(tab, tr("Global Setting"));
    groupBox7->setTitle(QString::null);
    LabelChannelL->setText(tr("Select Channel"));
    groupBoxOS->setTitle(tr("OSD Setting"));
    CheckTranslucentL->setText(tr("Translucent"));
    LabelOsdBrightnessL->setText(tr("Brightness"));
    LabelOsdPosXL->setText(tr("Positon X"));
    LabelOsdPosYL->setText(tr("Positon Y"));
    CheckOsdL->setText(tr("Osd Enable"));
    groupBoxCI->setTitle(tr("Capture Image"));
    RadioBmpL->setText(tr("Bmp"));
    RadioJpegL->setText(tr("Jpeg"));
    LabelPictureQL->setText(tr("Picture Quality"));
    ButtonCaptureL->setText(tr("Capture"));
    groupBoxIS->setTitle(tr("ImageStream"));
    CheckImageCaptureL->setText(tr("Capture Enable"));
    ButtonVideoRestoreL->setText(tr("Restore"));
    ButtonEnableG->setText(tr("Start/Stop Preview"));
    QToolTip::add(CBImageStreamWL, QString::null);
    LabelImageStreamWL->setText(tr("Width"));
    QToolTip::add(CBImageStreamHL, QString::null);
    LabelImageStreamHL->setText(tr("Height"));
    groupBoxAudio->setTitle(tr("Audio"));
    CheckAudioL->setText(tr("Audio Preview"));
    LabelSoundLevelL->setText(tr("Sound Level"));
    groupBoxMS->setTitle(tr("MotionDetect Setting"));
    CheckMotionDSL->setText(tr("MotionDetect Enable"));
    LabelMotionSensL->setText(tr("Sensitiveness"));
    groupBoxMask->setTitle(tr("Mask Setting"));
    CheckMaskL->setText(tr("Mask Enable"));
    QToolTip::add(CBRectangleL, QString::null);
    LabelRectangleL->setText(tr("Rectangle"));
    LabelBoundL->setText(tr("Bound"));
    ButtonNewL->setText(tr("New"));
    ButtonDeleteL->setText(tr("Delete"));
    groupBoxLogo->setTitle(tr("Logo Setting"));
    CheckLogoL->setText(tr("Logo Enable"));
    CheckLogoTransL->setText(tr("Translucent"));
    LabelLogoPosXL->setText(tr("Positon X"));
    LabelLogoPosYL->setText(tr("Positon Y"));
    groupBoxBC->setTitle(tr("BitRate Control"));
    LabelMaxBpsL->setText(tr("MaxBps"));
    QToolTip::add(CBModeL, QString::null);
    LabelModeL->setText(tr("Mode"));
    groupBoxVI->setTitle(tr("Video Info"));
    LabelBrightnessL->setText(tr("Brightness"));
    LabelContrastL->setText(tr(" Contrast"));
    LabelSaturationL->setText(tr("Saturation"));
    LabelHueL->setText(tr("  Hue"));
    groupBox9->setTitle(QString::null);
    CheckMinorRecordL->setText(tr("MinorRecord"));
    CheckMajorRecordL->setText(tr("MajorRecord"));
    CheckPreviewLl->setText(tr("Preview"));
    groupBoxCD->setTitle(tr("Coding"));
    LabelMajorStreamL->setText(tr("Major Stream"));
    QToolTip::add(CBMajorStreamL, QString::null);
    LabelMinorStreamL->setText(tr("Minor Stream"));
    groupBoxVQ->setTitle(tr("Video Quality "));
    LabelIQuantL->setText(tr("I frame"));
    LabelBQuantL->setText(tr("B frame"));
    LabelPQuantL->setText(tr("P frame"));
    groupBoxFS->setTitle(tr("Frame Structure"));
    LabelKeyFrameL->setText(tr("KeyFrameIntervals"));
    LabelBFramesL->setText(tr("BFrames"));
    LabelPFramesL->setText(tr("PFrames"));
    LabelFramesRL->setText(tr("FramesRate"));
    groupBoxPF->setTitle(tr("Picture Format"));
    QToolTip::add(CBMinorEncoderL, QString::null);
    LabelMinorEncoderL->setText(tr("Minor Encorder"));
    LabelMajorEncoderL->setText(tr("Major Encoder"));
    tabWidget3->changeTab(tab_2, tr("Local Setting"));
    groupBoxInformation->setTitle(QString::null);
    groupBoxBoard->setTitle(tr("Board Information"));
    groupBoxDSP->setTitle(tr("DSP Information"));
    groupBoxChannel->setTitle(tr("Channel Information"));
    tabWidget3->changeTab(tab_3, tr("Relation Information"));
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
int dsForm::SetBoardDspChannelInf()
{
    int                 i;
    int                 num;
    int                 temp;
    DS_BOARD_DETAIL*    DBorad;
    DSP_DETAIL*         DspDetail;
    char                ctemp[17];

    QString             Qs, Qstemp;

    num = GetBoardCount();
    Qs.sprintf("The number of board in system is %d \n", num);
    DBorad = (DS_BOARD_DETAIL*)malloc(num * sizeof(DS_BOARD_DETAIL));
    memset(DBorad, 0x0, num * sizeof(DS_BOARD_DETAIL));
    for(temp = 0; temp < num; temp++)
    {
        Qs.append(Qstemp.sprintf("*****Board: %d*****\n", temp + 1));
        if(GetBoardDetail(temp, &(DBorad[temp])) < 0)
            return 0;
        for(i = 0; i < 16; i++)
        {
            ctemp[i] = DBorad[temp].sn[i] + '0';
        }

        ctemp[16] = '\0';

        Qs.append(Qstemp.sprintf(
                      "The type for the board is %d;\nThe serial number is %s;\nThe dsp count in the board is %u;\nThe total index for the first dsp in the board is %u;\nThe encode channel count in the board is %u;\nThe total encode channel index for the first in the board is %u;\nThe decode channel count in the board is %u;\nThe total decode channel index for the first in the board is %u;\nThe display channel count in the board is %u;\nThe total display channel index for the first in the board is %u;\n", DBorad[temp].type, ctemp, DBorad[temp].dspCount, DBorad[temp].firstDspIndex,
                  DBorad[temp].encodeChannelCount, DBorad[temp].firstEncodeChannelIndex,
                  DBorad[temp].decodeChannelCount, DBorad[temp].firstDecodeChannelIndex,
                  DBorad[temp].displayChannelCount, DBorad[temp].firstDisplayChannelIndex));
    }

    EditBoard->setText(Qs);

    num = GetTotalDSPs();
    Qs.sprintf("The number of DSP in system is %d\n", num);
    DspDetail = (DSP_DETAIL*)malloc(num * sizeof(DSP_DETAIL));
    memset(DspDetail, 0x0, num * sizeof(DSP_DETAIL));
    for(temp = 0; temp < num; temp++)
    {
        Qs.append(Qstemp.sprintf("*****DSP: %d*****\n", temp + 1));
        if(GetDspDetail(temp, &(DspDetail[temp])) < 0)
            return 0;

        Qs.append(Qstemp.sprintf(
                      "The encode channel count in the dsp is %u;\nThe total encode channel index for the first in the dsp is %u;\nThe decode channel count in the dsp is %u;\nThe total decode channel index for the first in the dsp is %u;\nThe display channel count in the dsp is %u;\nThe total display channel index for the first in the dsp is %u;\n", DspDetail[temp].encodeChannelCount, DspDetail[temp].firstEncodeChannelIndex,
                  DspDetail[temp].decodeChannelCount, DspDetail[temp].firstDecodeChannelIndex,
                  DspDetail[temp].displayChannelCount, DspDetail[temp].firstDisplayChannelIndex));
    }

    EditDsp->setText(Qs);

    num = GetTotalChannels();
    Qs.sprintf("Total valid channel number in system is %d\n", num);

    num = GetEncodeChannelCount();
    Qstemp.sprintf("The all encode channel count in the system is %d;\n", num);
    Qs.append(Qstemp);

    num = GetDecodeChannelCount();
    Qstemp.sprintf("The all decode channel count in the system is %d;\n", num);
    Qs.append(Qstemp);

    num = GetDisplayChannelCount();
    Qstemp.sprintf("The all display channel count in the system is %d;\n", num);
    Qs.append(Qstemp);

    EditChannel->setText(Qs);

    free(DBorad);
    free(DspDetail);
    return 1;
}
