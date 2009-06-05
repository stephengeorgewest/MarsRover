/**********************************************************************
Mars Rover Video Streaming
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RoverNetwork.h"
#include <math.h>
#include "/usr/local/euresys/multicam/include/MultiCamCpp.h"
#define COMPRESS true
#ifndef BYTE
#define BYTE unsigned char
#endif

#define VID1_ON 0 
#define VID2_ON 0
#define VID3_ON 1
#define VID4_ON 0

int max_packet_size = 1200;

int fps[] = {20,20,5,5};
int quality[] = {1,1,40,1};
int gop[] = {3, 10, 5, 5};
using namespace Euresys::MultiCam;

void ProcessImage(BYTE *pImage, int sizeX, int sizeY, int *saved_images);
void SaveImage(BYTE *pImage, int count);

class CPicoloGrab
{
public:
    unsigned int imageBuffer;
    char* buffer;
    int buffer_size;
    int buffer_index;
    int sizex, sizey, bufferPitch;
    int saved_images;
    int sizeb;    
    RoverNetwork RN;
    Channel *Chan1;
    Channel *Chan2;
    Channel *Chan3;
    Channel *Chan4;
    int count[4];

    bool done;
    CPicoloGrab();
    ~CPicoloGrab();
    BOOL Init();
    BOOL Chan1Init();
    BOOL Chan2Init();
    BOOL Chan3Init();
    BOOL Chan4Init();
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
    count[0] = count[1] = count[2] = count[3] = 0;
    done = false;
    saved_images = 0;
    Chan1 = NULL;
    init_multicast(&RN, ROVER_GROUP_VIDEO, ROVER_PORT_VIDEO);

}

CPicoloGrab::~CPicoloGrab()
{
    if (Chan1 != NULL)
        delete Chan1;
}

BOOL CPicoloGrab::Init()
{
	if(VID1_ON) Chan1Init();
	if(VID2_ON) Chan2Init();
	if(VID3_ON) Chan3Init();
	if(VID4_ON) Chan4Init();


}

BOOL CPicoloGrab::Chan1Init(){
 try
    {
        // Create a channel and associate it with the first connector on the first board
        Chan1 = new Channel(Boards[0], "VID1");
        Chan1->SetParam(MC_CamFile, "NTSC");
        Chan1->SetParam(MC_ColorFormat, MC_ColorFormat_RGB24);
	if(COMPRESS){
		Chan1->SetParam(MC_CompressionType, MC_CompressionType_DX50);
		Chan1->SetParam(MC_ColorFormat, MC_ColorFormat_DX50);
//		Chan1->SetParam(MC_BitrateControl, MC_BitrateControl_CBR);
//		Chan1->SetParam(MC_AverageBitrate, 10000*8);
		Chan1->SetParam(MC_BitrateControl, MC_BitrateControl_VBR);
		Chan1->SetParam(MC_VideoQuality, quality[0]);
		Chan1->SetParam(MC_GopStructure, MC_GopStructure_IPONLY);
		Chan1->SetParam(MC_GopSize, gop[0]);	
		Chan1->SetParam(MC_Resolution, MC_Resolution_CIF);

	}
        Chan1->SetParam(MC_TrigMode, MC_TrigMode_IMMEDIATE);
	Chan1->SetParam(MC_NextTrigMode, MC_NextTrigMode_PERIODIC);
	Chan1->SetParam(MC_TargetFrameRate_Hz, fps[0]);
        Chan1->SetParam(MC_AcquisitionMode, MC_AcquisitionMode_VIDEO);
        Chan1->SetParam(MC_SeqLength_Fr, MC_INDETERMINATE);

        Chan1->GetParam(MC_ImageSizeX, sizex);
        Chan1->GetParam(MC_ImageSizeY, sizey);

	// Register the callback function called when a new image is available
        Chan1->RegisterCallback(this, &CPicoloGrab::ProcessingCallback, MC_SIG_SURFACE_PROCESSING);
        Chan1->RegisterCallback(this, &CPicoloGrab::AcqFailureCallback, MC_SIG_ACQUISITION_FAILURE);

        /* Enable the signals we need: 
        MC_SIG_SURFACE_PROCESSING: acquisition done and locked for processing
        MC_SIG_ACQUISITION_FAILURE: acquisition failed.
        */
        // Enable the signal corresponding to the callback function 
        Chan1->SetParam(MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
        Chan1->SetParam(MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);

        // Prepare the channel in order to minimize the acquisition sequence startup latency
        Chan1->Prepare();
    }
    catch(Euresys::MultiCam::Exception &e)
    {
        printf("Error on channel 1: %s\n", e.What());

        if (Chan1 != NULL)
            delete Chan1;

        return FALSE;
    }

    return TRUE;


}

BOOL CPicoloGrab::Chan2Init(){
 try
    {
        // Create a channel and associate it with the first connector on the first board
        Chan2 = new Channel(Boards[0], "VID2");
        Chan2->SetParam(MC_CamFile, "NTSC");
        Chan2->SetParam(MC_ColorFormat, MC_ColorFormat_RGB24);
	if(COMPRESS){
		Chan2->SetParam(MC_CompressionType, MC_CompressionType_DX50);
		Chan2->SetParam(MC_ColorFormat, MC_ColorFormat_DX50);
//		Chan2->SetParam(MC_BitrateControl, MC_BitrateControl_CBR);
//		Chan2->SetParam(MC_AverageBitrate, 10000*8);
		Chan2->SetParam(MC_BitrateControl, MC_BitrateControl_VBR);
		Chan2->SetParam(MC_VideoQuality, quality[1]);
		Chan2->SetParam(MC_GopStructure, MC_GopStructure_IPONLY);
		Chan2->SetParam(MC_GopSize, gop[1]);	
		Chan2->SetParam(MC_Resolution, MC_Resolution_CIF);

	}
        Chan2->SetParam(MC_TrigMode, MC_TrigMode_IMMEDIATE);
	Chan2->SetParam(MC_NextTrigMode, MC_NextTrigMode_PERIODIC);
	Chan2->SetParam(MC_TargetFrameRate_Hz, fps[1]);
        Chan2->SetParam(MC_AcquisitionMode, MC_AcquisitionMode_VIDEO);
        Chan2->SetParam(MC_SeqLength_Fr, MC_INDETERMINATE);

        Chan2->GetParam(MC_ImageSizeX, sizex);
        Chan2->GetParam(MC_ImageSizeY, sizey);

	// Register the callback function called when a new image is available
        Chan2->RegisterCallback(this, &CPicoloGrab::ProcessingCallback, MC_SIG_SURFACE_PROCESSING);
        Chan2->RegisterCallback(this, &CPicoloGrab::AcqFailureCallback, MC_SIG_ACQUISITION_FAILURE);

        /* Enable the signals we need: 
        MC_SIG_SURFACE_PROCESSING: acquisition done and locked for processing
        MC_SIG_ACQUISITION_FAILURE: acquisition failed.
        */
        // Enable the signal corresponding to the callback function 
        Chan2->SetParam(MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
        Chan2->SetParam(MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);

        // Prepare the channel in order to minimize the acquisition sequence startup latency
        Chan2->Prepare();
    }
    catch(Euresys::MultiCam::Exception &e)
    {
        printf("Error on channel 2: %s\n", e.What());

        if (Chan2 != NULL)
            delete Chan2;

        return FALSE;
    }

    return TRUE;

}

BOOL CPicoloGrab::Chan3Init(){
 try
    {
        // Create a channel and associate it with the first connector on the first board
        Chan3 = new Channel(Boards[0], "VID3");
        Chan3->SetParam(MC_CamFile, "NTSC");
        Chan3->SetParam(MC_ColorFormat, MC_ColorFormat_RGB24);
	if(COMPRESS){
		Chan3->SetParam(MC_CompressionType, MC_CompressionType_DX50);
		Chan3->SetParam(MC_ColorFormat, MC_ColorFormat_DX50);
//		Chan3->SetParam(MC_BitrateControl, MC_BitrateControl_CBR);
//		Chan3->SetParam(MC_AverageBitrate, 10000*8);
		Chan3->SetParam(MC_BitrateControl, MC_BitrateControl_VBR);
		Chan3->SetParam(MC_VideoQuality, quality[2]);
		Chan3->SetParam(MC_GopStructure, MC_GopStructure_IONLY);
		Chan3->SetParam(MC_GopSize, gop[2]);	
		Chan3->SetParam(MC_Resolution, MC_Resolution_CIF);

	}
        Chan3->SetParam(MC_TrigMode, MC_TrigMode_IMMEDIATE);
	Chan3->SetParam(MC_NextTrigMode, MC_NextTrigMode_PERIODIC);
	Chan3->SetParam(MC_TargetFrameRate_Hz, fps[2]);
        Chan3->SetParam(MC_AcquisitionMode, MC_AcquisitionMode_VIDEO);
        Chan3->SetParam(MC_SeqLength_Fr, MC_INDETERMINATE);

        Chan3->GetParam(MC_ImageSizeX, sizex);
        Chan3->GetParam(MC_ImageSizeY, sizey);

	// Register the callback function called when a new image is available
        Chan3->RegisterCallback(this, &CPicoloGrab::ProcessingCallback, MC_SIG_SURFACE_PROCESSING);
        Chan3->RegisterCallback(this, &CPicoloGrab::AcqFailureCallback, MC_SIG_ACQUISITION_FAILURE);

        /* Enable the signals we need: 
        MC_SIG_SURFACE_PROCESSING: acquisition done and locked for processing
        MC_SIG_ACQUISITION_FAILURE: acquisition failed.
        */
        // Enable the signal corresponding to the callback function 
        Chan3->SetParam(MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
        Chan3->SetParam(MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);

        // Prepare the channel in order to minimize the acquisition sequence startup latency
        Chan3->Prepare();
    }
    catch(Euresys::MultiCam::Exception &e)
    {
        printf("Error on channel 3: %s\n", e.What());

        if (Chan3 != NULL)
            delete Chan3;

        return FALSE;
    }

    return TRUE;

}

BOOL CPicoloGrab::Chan4Init(){
 try
    {
        // Create a channel and associate it with the first connector on the first board
        Chan4 = new Channel(Boards[0], "VID4");
        Chan4->SetParam(MC_CamFile, "NTSC");
        Chan4->SetParam(MC_ColorFormat, MC_ColorFormat_RGB24);
	if(COMPRESS){
		Chan4->SetParam(MC_CompressionType, MC_CompressionType_DX50);
		Chan4->SetParam(MC_ColorFormat, MC_ColorFormat_DX50);
//		Chan4->SetParam(MC_BitrateControl, MC_BitrateControl_CBR);
//		Chan4->SetParam(MC_AverageBitrate, 10000*8);
		Chan4->SetParam(MC_BitrateControl, MC_BitrateControl_VBR);
		Chan4->SetParam(MC_VideoQuality, quality[3]);
		Chan4->SetParam(MC_GopStructure, MC_GopStructure_IPONLY);
		Chan4->SetParam(MC_GopSize, gop[3]);	
//		Chan4->SetParam(MC_Resolution, MC_Resolution_CIF);

	}
        Chan4->SetParam(MC_TrigMode, MC_TrigMode_IMMEDIATE);
	Chan4->SetParam(MC_NextTrigMode, MC_NextTrigMode_PERIODIC);
	Chan4->SetParam(MC_TargetFrameRate_Hz, fps[3]);
        Chan4->SetParam(MC_AcquisitionMode, MC_AcquisitionMode_VIDEO);
        Chan4->SetParam(MC_SeqLength_Fr, MC_INDETERMINATE);

        Chan4->GetParam(MC_ImageSizeX, sizex);
        Chan4->GetParam(MC_ImageSizeY, sizey);

	// Register the callback function called when a new image is available
        Chan4->RegisterCallback(this, &CPicoloGrab::ProcessingCallback, MC_SIG_SURFACE_PROCESSING);
        Chan4->RegisterCallback(this, &CPicoloGrab::AcqFailureCallback, MC_SIG_ACQUISITION_FAILURE);

        /* Enable the signals we need: 
        MC_SIG_SURFACE_PROCESSING: acquisition done and locked for processing
        MC_SIG_ACQUISITION_FAILURE: acquisition failed.
        */
        // Enable the signal corresponding to the callback function 
        Chan4->SetParam(MC_SignalEnable + MC_SIG_SURFACE_PROCESSING, MC_SignalEnable_ON);
        Chan4->SetParam(MC_SignalEnable + MC_SIG_ACQUISITION_FAILURE, MC_SignalEnable_ON);

        // Prepare the channel in order to minimize the acquisition sequence startup latency
        Chan4->Prepare();
    }
    catch(Euresys::MultiCam::Exception &e)
    {
        printf("Error on channel 4: %s\n", e.What());

        if (Chan4 != NULL)
            delete Chan4;

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
   
    fprintf(stderr,"Aquisition loop started\n");

    while(!done){
	if(VID1_ON){
	//	 printf("Starting channel 1\n");
		Chan1->SetParam(MC_ChannelState, MC_ChannelState_ACTIVE);
	}
	if(VID2_ON){
	//	printf("Starting channel 2\n");
		Chan2->SetParam(MC_ChannelState, MC_ChannelState_ACTIVE);
	}
  	if(VID3_ON){
	//	printf("Starting channel 3\n");
		Chan3->SetParam(MC_ChannelState, MC_ChannelState_ACTIVE);
   	}
	if(VID4_ON){
	//	 printf("Starting channel 4\n");
		Chan4->SetParam(MC_ChannelState, MC_ChannelState_ACTIVE);
   	}
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
		int chan_index = 0;
		if(&Ch == Chan1){
			chan_index = 0;
		}else if(&Ch == Chan2){
			chan_index = 1;
		}else if(&Ch == Chan3){
			chan_index = 2;
		}else if(&Ch == Chan4){
			chan_index = 3;
		}

        Info.Surf->GetParam(MC_FillCount, sizeb); 
        Chan1->GetParam(MC_ImageSizeX, sizex);
        Chan1->GetParam(MC_ImageSizeY, sizey);
        Info.Surf->GetParam(MC_SurfacePitch, bufferPitch);
        Info.Surf->GetParam(MC_SurfaceAddr, imageBuffer);
/*
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
	printf(" on channel %i size = %i\n", chan_index,  sizeb);
*/
//	printf("sizex = %i, sizey = %i\n", sizex, sizey);

	int packets = (sizeb)/(max_packet_size-32) + 1;

	int sentb = 0;
	for(int i = 0;i<packets;i++){
		int remaining_bytes = sizeb - sentb;
		char vidcode = ROVER_MAGIC_VIDEO;
		char chanID = chan_index;
		int frameID = count[chan_index];
		char packetNum = i+1;
		char totalPackets = packets;
		int packetPayload = std::min(max_packet_size-32, remaining_bytes);
		int totalPayload = sizeb;
		char vidType[4] = {'d', 'x', '5', '0'};
		int width = sizex;
		int height = sizey;
		char rate = fps[chan_index];
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
		buffer[28] = rate;
		buffer[29] = 0;		
		buffer[30] = 0;
		buffer[31] = 0;

		//char a = *((char*)imageBuffer + sentb);
		//char b = *((char*)imageBuffer + sentb + 1);
		//char c = *((char*)imageBuffer + sentb + 2);
		//char d = *((char*)imageBuffer + sentb + 3);

		//printf("%i, %i, %i, %i\n", a, b, c, d);
		
		memcpy(buffer+32, (char*)imageBuffer + sentb, packetPayload);
	//	printf("sending...\n");
		send_message(&RN, buffer, packetPayload+32);
	//	printf("done\n");
		sentb += packetPayload;
		//printf("sentb = %i\n", sentb);
	}
	count[chan_index]++;
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
