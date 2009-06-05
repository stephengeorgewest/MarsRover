#ifndef _QSDLWIDGET_H_
#define _QSDLWIDGET_H_

#include <qwidget.h>
#include <qimage.h>

#include "dsdemo.h"

class QSDLWidget : public QWidget
{
    Q_OBJECT

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
    Dsclass **       dsdemoclass;

    QSDLWidget(QWidget* parent, const char* name);
    ~               QSDLWidget();
    video_info*     GetVideoParam();
    video_info*     GetVideoDefault(int port);
    void            SetDsclassDefault();
    void            SetSelectPort(int port);
    void            ChangeFullPort();

    DSDEMO*         GetDsinfo(int port);

    void            StreamTypeSet(int StreamType);
    void            StreamTypeSubSet(int StreamType);
    void            StreamTypeAllSet(int StreamType);
    void            StreamTypeSubAllSet(int StreamType);
    void            EncoderSet(PictureFormat_t);
    void            EncoderSubSet(PictureFormat_t);
    void            EncoderAllSet(PictureFormat_t);
    void            EncoderSubAllSet(PictureFormat_t);

    void            SetFullEnable(bool on);
    void            SetDsclassDefualt();
    int             GetSelectPort();
    int             GetLastPort();
    bool            GetFullState();
    void            SetAudioPort(const int port);
    void            Createdsclass();
    void            StreamRead_callback(int channel, char* databuf, int frametype, int framelength);
    void            start_capture_callback(int port);
    void            stop_capture_callback(int port);
    void            message_callback(char* buf, int length);
    int             check_IP_callback(int channel, char* sIP);
    int             check_password_callback(char*  username, int namelen, char*  password,
                                            int passlen);
    int             checkIpAndPass(int channel, char*  username, int namelen, char*  password,
                                   int passlen, char*  sIP);
    void            disconnect_callback(int nport, char* sIP);

    void            BrightnessSet(int value);
    void            BrightnessAllSet(int value);
    void            ContrastSet(int value);
    void            ContrastAllSet(int value);
    void            SaturationSet(int value);
    void            SaturationAllSet(int value);
    void            HueSet(int value);
    void            HueAllSet(int value);
    void            VideoRestoreSet();
    void            VideoRestoreAllSet();
    void            OsdEnableSet(bool);
    void            OsdEnableAllSet(bool);
    void            OsdTranslucentSet(bool);
    void            OsdTranslucentAllSet(bool);
    void            OsdBrightnessSet(int number);
    void            OsdBrightnessAllSet(int number);
    void            OsdPosXSet(int number);
    void            OsdPosXAllSet(int number);
    void            OsdPosYSet(int number);
    void            OsdPosYAllSet(int number);

    void            LogoEnabledSet(bool on);
    void            LogoEnabledAllSet(bool on);
    void            LogoTranslucentSet(bool on);
    void            LogoTranslucentAllSet(bool on);
    void            LogoPosXSet(int number);
    void            LogoPosXAllSet(int number);
    void            LogoPosYSet(int number);
    void            LogoPosYAllSet(int number);

    RECT*           GetMaskinfo(int);
    void            MaskEnableSet(bool);
    void            MaskEnableAllSet(bool);
    void            MaskAddSet(RECT* add);
    void            MaskAddAllSet(RECT* add);
    void            MaskDelSet(int index);
    void            MaskDelAllSet(int index);

    void            MotionSet(bool on);
    void            MotionAllSet(bool on);
    void            MotionSenSet(int value);
    void            MotionSenAllSet(int value);
    void            AudioSet(bool on);

    void            ImageStreamSet(bool on, unsigned char* imagebuf);
    void            GetBmpPicture(int count);
    void            GetJpegPicture(int count, int quality);
    void            GetBmpAllPicture(int count);
    void            GetJpegAllPicture(int count, int quality);

    void            ImageStreamCallback(unsigned int channelNumber, void* context);
    void            ImageStreamSet(int start, unsigned width, unsigned height);
    void            ImageStreamAllSet(int start, unsigned width, unsigned height);

    int             AllRecordStatus();

    public slots :
		void BitRateSet (int MaxBps, BitrateControlType_t brc);
        void    BitRateAllSet(int MaxBps, BitrateControlType_t brc);

        void    RecordSet(bool on);
    	void    RecordSubSet(bool on);
        void    RecordAllSet(bool on);
        void    RecordSubAllSet(bool on);

    	void    NetTransferSet(bool on);
        void    PreviewSet(bool on);
        void    PreviewAllSet(bool on);
        void    StringToClientSet(bool on);
    	void    CifQCifAllSet(bool);
        void    CifQCifSet(bool);
        void    IQuantSet(int number);
        void    IQuantAllSet(int number);
    	void    BQuantSet(int number);
        void    BQuantAllSet(int number);
        void    PQuantSet(int number);
        void    PQuantAllSet(int number);
    	void    KeyFrameISet(int number);
        void    KeyFrameIAllSet(int number);
        void    BFramesSet(int number);
        void    BFramesAllSet(int number);
        void    FrameRateSet(int number);
        void    FrameRateAllSet(int number);

    private slots : 
	    void    process_time();
        void    process_time_loop();

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
protected:
    void    paintEvent(QPaintEvent* e);
    void    mousePressEvent(QMouseEvent* e);
    void    mouseDoubleClickEvent(QMouseEvent* e);
    void    customEvent(QCustomEvent* );
signals:
    void    clicked(int);
    void    audioChange(int, float);
    void    videoChange(int, float);
    void    totalChange(int, int);
    void    bitRateChange(int, int);
    void    countChange(int);
    void    fullSpace();
    void    noNet();

/*
 -----------------------------------------------------------------------------------------------------------------------
 *
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
    SDL_Surface*    pOverlayScreen;

    QTime **         time;
    QTimer **        timer;
    QTimer **        Qtloop;
    int             SelectPort;
    int             LastPort;
    bool            FullArea;
    bool            AllArea;
    QImage          QIm;
    QPixmap*        pm;

    int             GetChanNum();
    int             PtInRect(SDL_Rect rect, int x, int y);
    void            motion_detect(int port);
    void            MotionBegin();
    short int*      checkpass;
};
#endif
