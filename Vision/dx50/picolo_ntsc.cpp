/*
+-------------------------------- DISCLAIMER ---------------------------------+
|                                                                             |
| This application program is provided to you free of charge as an example.   |
| Despite the considerable efforts of Euresys personnel to create a usable    |
| example, you should not assume that this program is error-free or suitable  |
| for any purpose whatsoever.                                                 |
|                                                                             |
| EURESYS does not give any representation, warranty or undertaking that this |
| program is free of any defect or error or suitable for any purpose. EURESYS |
| shall not be liable, in contract, in torts or otherwise, for any damages,   |
| loss, costs, expenses or other claims for compensation, including those     |
| asserted by third parties, arising out of or in connection with the use of  |
| this program.                                                               |
|                                                                             |
+-----------------------------------------------------------------------------+
*/


/***********************************************************************
MultiCam C++ Sample program.

Acquires an image from the first board and save it in a ppm file.
The board must come from Picolo family.

The ppm file can be easily viewed with most image display programs,
like gimp, kview, xv.
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RoverNetwork.h"

#include "/usr/local/euresys/multicam/include/MultiCamCpp.h"
#define COMPRESS true
#ifndef BYTE
#define BYTE unsigned char
#endif

using namespace Euresys::MultiCam;

void ProcessImage(BYTE *pImage, int sizeX, int sizeY, int *saved_images);
void SaveImage(BYTE *pImage, int count);

class CPicoloGrab
{
public:
    unsigned int imageBuffer;
    char* buffer;
    int count;
    int buffer_size;
    int buffer_index;
    int sizex, sizey, bufferPitch;
    int saved_images;
    int sizeb;    
    RoverNetwork RN;
    Channel *pChannel;
    bool done;
    CPicoloGrab();
    ~CPicoloGrab();
    BOOL Init();
    void AcquireImages();
    void ProcessingCallback(Channel &channel, SignalInfo &info);
    void AcqFailureCallback(Channel &channel, SignalInfo &info);

};


CPicoloGrab::CPicoloGrab()
{
    /* Initialize the MultiCam driver.
    Do it once at the beginning of your program.
    */
    buffer_size = 100000;
    buffer = new char[buffer_size];
    buffer_index = 0;
    sizex = 0;
    sizey = 0;
    sizeb = -1;
    count = 0;
    done = false;
    saved_images = 0;
    pChannel = NULL;
    init_multicast(&RN, ROVER_GROUP_VIDEO, ROVER_PORT_VIDEO);

}

CPicoloGrab::~CPicoloGrab()
{
    if (pChannel != NULL)
        delete pChannel;
}

BOOL CPicoloGrab::Init()
{
    try
    {
        // Create a channel and associate it with the first connector on the first board
        pChannel = new Channel(Boards[0], "VID1");
        pChannel->SetParam(MC_CamFile, "NTSC");
        pChannel->SetParam(MC_ColorFormat, MC_ColorFormat_RGB24);
	if(COMPRESS){
		pChannel->SetParam(MC_CompressionType, MC_CompressionType_DX50);
		pChannel->SetParam(MC_ColorFormat, MC_ColorFormat_DX50);
//		pChannel->SetParam(MC_BitrateControl, MC_BitrateControl_CBR);
//		pChannel->SetParam(MC_AverageBitrate, 10000*8);
		pChannel->SetParam(MC_BitrateControl, MC_BitrateControl_VBR);
		pChannel->SetParam(MC_VideoQuality, 10);
		pChannel->SetParam(MC_GopStructure, MC_GopStructure_IPONLY);
		pChannel->SetParam(MC_GopSize, 20);	
		pChannel->SetParam(MC_Resolution, MC_Resolution_CIF);

	}
        pChannel->SetParam(MC_TrigMode, MC_TrigMode_IMMEDIATE);
	pChannel->SetParam(MC_NextTrigMode, MC_NextTrigMode_PERIODIC);
	pChannel->SetParam(MC_TargetFrameRate_Hz, 20);
        pChannel->SetParam(MC_AcquisitionMode, MC_AcquisitionMode_VIDEO);
        pChannel->SetParam(MC_SeqLength_Fr, MC_INDETERMINATE);

        pChannel->GetParam(MC_ImageSizeX, sizex);
        pChannel->GetParam(MC_ImageSizeY, sizey);

	// Register the callback function called when a new image is available
        pChannel->RegisterCallback(this, &CPicoloGrab::ProcessingCallback, MC_SIG_SURFACE_PROCESSING);
        pChannel->RegisterCallback(this, &CPicoloGrab::AcqFailureCallback, MC_SIG_ACQUISITION_FAILURE);

        /* Enable the signals we need: 
        MC_SIG_SURFACE_PROCESSING: acquisition done and locked for processing
        MC_SIG_ACQUISITION_FAILURE: acquisition failed.
        */
        // Enable the signal corresponding to the callback function 
        pChannel->SetParam(MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
        pChannel->SetParam(MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);

        // Prepare the channel in order to minimize the acquisition sequence startup latency
        pChannel->Prepare();
    }
    catch(Euresys::MultiCam::Exception &e)
    {
        printf("Error : %s\n", e.What());

        if (pChannel != NULL)
            delete pChannel;

        return FALSE;
    }

    return TRUE;
}

/*
Acquire images and process them.
*/
void CPicoloGrab::AcquireImages()
{
    /*
    Start Acquisitions for this channel.
    Acquisition will start when the channel is active.
    */
    fprintf(stderr,"SetParam MC_ChannelState <MC_ChannelState_ACTIVE>\n");
    pChannel->SetParam(MC_ChannelState, MC_ChannelState_ACTIVE);

    fprintf(stderr,"WAITING for image processing callbacks\n");

    /* Wait 3 seconds. That should be enough for the acquisition to finish.
    In industrial programs, this will probably be replaced with some wait
    for the end of the application.
    */
    while(!done){
	usleep(100);
    }
}

/*
Callback function registered to our channel. 
Called at the asynchronous signals.
*/
void CPicoloGrab::ProcessingCallback(Channel &Ch, SignalInfo &Info)
{
    try
    {
       // fprintf(stderr,"Processing surface.\n");

        // Update the bitmap image with the acquired image data 
        Info.Surf->GetParam(MC_FillCount, sizeb); 
        pChannel->GetParam(MC_ImageSizeX, sizex);
        pChannel->GetParam(MC_ImageSizeY, sizey);
        Info.Surf->GetParam(MC_SurfacePitch, bufferPitch);
        Info.Surf->GetParam(MC_SurfaceAddr, imageBuffer);
//	Info.Surf->GetParam(MC_
       // ProcessImage(reinterpret_cast<unsigned char*>(imageBuffer), sizex, sizey, &saved_images);
        /* Insert image analysis and processing code here */
	if(COMPRESS){
	   int type = -1;
	   Info.Surf->GetParam(MC_FrameType, type);
	   if(type == MC_FrameType_I)
		printf("I frame ");
	   if(type == MC_FrameType_B)
		printf("B frame ");
	   if(type == MC_FrameType_P)
		printf("P frame ");
	} else {
		printf("Raw Frame ");
	}
	printf(" size = %i\n", sizeb);


	printf("sizex = %i, sizey = %i\n", sizex, sizey);
	char vidcode = ROVER_MAGIC_VIDEO;
	char chanID = 0;
	int frameID = count++;
	char packetNum = 1;
	char totalPackets = 1;
	int packetPayload = sizeb ;
	int totalPayload = sizeb;
	char vidType[4] = {'d', 'x', '5', '0'};
	int width = sizex;
	int height = sizey;
	char fps = 20;
	char empty = 0;

	buffer[0] = vidcode;
	buffer[1] = chanID;
	buffer[2] = (frameID >> 24) & 0xFF;
	buffer[3] = (frameID >> 16) & 0xFF;
	buffer[4] = (frameID >> 8) & 0xFF;
	buffer[5] = (frameID) & 0xFF;
	buffer[6] = packetNum;
	buffer[7] = totalPackets;
	buffer[8] = (packetPayload >> 24) & 0xFF;
	buffer[9] = (packetPayload >> 16) & 0xFF;
	buffer[10] = (packetPayload >> 8) & 0xFF;
	buffer[11] = (packetPayload) & 0xFF;
	buffer[12] = (totalPayload >> 24) & 0xFF;
	buffer[13] = (totalPayload >> 16) & 0xFF;
	buffer[14] = (totalPayload >> 8) & 0xFF;
	buffer[15] = (totalPayload ) & 0xFF;
	memcpy(buffer + 16, vidType, 4);
 	buffer[20] = (width >> 24) & 0xFF;
	buffer[21] = (width >> 16) & 0xFF;
	buffer[22] = (width >> 8) & 0xFF;
	buffer[23] = (width ) & 0xFF;
	buffer[24] = (height  >> 24) & 0xFF;
	buffer[25] = (height  >> 16) & 0xFF;
	buffer[26] = (height  >> 8) & 0xFF;
	buffer[27] = (height ) & 0xFF;
	buffer[28] = fps;
	buffer[29] = 0;		
	buffer[30] = 0;
	buffer[31] = 0;

	memcpy(buffer+32, (char*)imageBuffer, sizeb);

	send_message(&RN, buffer, sizeb+32);
    }
    catch (Euresys::MultiCam::Exception &e)
    {
        printf("Error during callback processing : %s\n", e.What());
    }
}

void CPicoloGrab::AcqFailureCallback(Channel &channel, SignalInfo &info)
{
    fprintf(stderr, "Acquisition Failure. Is a video signal connected ?\n");
}
int main(int argc, char* argv[])
{
    CPicoloGrab *PicoloGrab;

    try
    {
        /* Initialize the MultiCam driver. */
        Euresys::MultiCam::Initialize();

        PicoloGrab = new CPicoloGrab();

        if (PicoloGrab->Init() == FALSE)
        {
            delete PicoloGrab;

            /* Close the MultiCam driver. */
            Euresys::MultiCam::Terminate();

            return -1;
        }

        /* Acquire the images and process them.*/
        PicoloGrab->AcquireImages();

        delete PicoloGrab;

        /* Close the MultiCam driver. */
        Euresys::MultiCam::Terminate();
    }
    catch(Euresys::MultiCam::Exception &e)
    {
        printf("Error during image grabbing : %s\n", e.What());
    }


    return 0;
}
