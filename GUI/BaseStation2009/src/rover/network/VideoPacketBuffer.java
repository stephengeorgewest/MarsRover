package rover.network;

import java.awt.image.BufferedImage;

import rover.utils.Bitmap;

public class VideoPacketBuffer
{
    int currentPacketCount;
    int currentFrameID;
    VideoPacket[] buffer;

    public BufferedImage current_image;
    public Bitmap b = null;

    public VideoPacketBuffer()
    {
        currentPacketCount = -1;
        currentFrameID = -1;
        current_image = null;
    }

    private void resetState(){
        currentFrameID = -1;
        for (int i = 0; i < currentPacketCount; i++)
        {
            buffer[i] = null;
        }
    }

    //video packets come in the following format [packet ID][ChannelID][image width][image height][FrameID][PacketNumber][TotalPackets][Data...]
    public void Process(VideoPacket p)
    {
        p.fillData();
        //if we've changed to a different number of packets/frame reset the state
        if(currentPacketCount != p.TotalPackets){
            //change the capacity of the buffer
            buffer = new VideoPacket[p.TotalPackets];
            currentPacketCount = p.TotalPackets;
            for(int i = 0;i<currentPacketCount;i++){
                buffer[i] = null;
            }
        }
        if (p.FrameID != currentFrameID)
        {
            //reset state
            for (int i = 0; i < currentPacketCount; i++)
            {
                buffer[i] = null;
            }
        }

        currentFrameID = p.FrameID;

        buffer[p.PacketNumber-1] = p;

        //System.out.println("Received " + p.PacketNumber + "/" + currentPacketCount);

        boolean allPacketsReceived = true;
        for (int i = 0; i < currentPacketCount; i++)
        {
            if(buffer[i] == null) allPacketsReceived = false;
        }

        if (allPacketsReceived) MergePackets();

    }

    private void MergePackets(){

        int rows = buffer[0].Height;
        int cols = buffer[0].Width;

        //merge the packets together to create a new image
        //System.out.println("rows = " + rows + "cols = " + cols);
        int rgb_size = rows * cols * 3;
        int yuv_size = rows*cols + rows*cols/2;

        //Console.WriteLine("Allocating space for rgb frame, total memory = " + System.GC.GetTotalMemory(true));

        byte[] rgb = new byte[rgb_size];

        //Console.WriteLine("Allocating space for yuv frame, total memory = " + System.GC.GetTotalMemory(true));
        byte[] yuv = new byte[yuv_size];

        int yuv_index = 0;
        for (int i = 0; i < currentPacketCount; i++)
        {
            //Console.WriteLine("merging packet " + i + " " + buffer[i].bytes + " bytes");
            int count = Math.min(buffer[i].bytes - VideoPacket.DATA_OFFSET, yuv_size - yuv_index);
            System.arraycopy(buffer[i].packet, VideoPacket.DATA_OFFSET, yuv, yuv_index, count);
            yuv_index += count;
            
            /*for(int j = VideoPacket.DATA_OFFSET;j<buffer[i].bytes;j++){
                yuv[yuv_index] = buffer[i].packet[j];
                yuv_index++;
                if (yuv_index > yuv_size)
                {
                    Console.WriteLine("Error, image packet corruption");
                    resetState();
                    return;
                }
            }*/
        }

        //Console.WriteLine("Converting YUV image of size " + yuv_index);

        //convert image to rgb
        convert_YUV420P_RGB(yuv, rgb, rows, cols);

        //Buffer.BlockCopy(yuv, 0, rgb, 0, yuv_size);

        //current_image = new Bitmap(cols, rows, PixelFormat.Format24bppRgb);
        //CopyDataToBitmap(rgb, current_image);

//        rgb = new byte[cols*rows*3];
//        for(int i = 0;i<rows;i++){
//        	for(int j = 0;j<cols;j++){
//        		rgb[3*(i*cols+j)] = (byte)(255*i/rows);
//        		rgb[3*(i*cols+j)+1] = (byte)(255);
//        		rgb[3*(i*cols+j)+2] = (byte)(255);
//        	}
//        }
        
        b = new Bitmap(rgb, cols, rows);

        current_image = b.getImage();
        
        //reset the state
        resetState();
    }


    void convert_YUV420P_RGB(byte[] yuv, byte[] rgb, int rows, int cols)
    {
        int rvScale = 91881;
        int guScale = -22553;
        int gvScale = -46801;
        int buScale = 116129;
        int yScale = 65536;

        
        int Y_index = 0;
        int U_index = rows*cols;
        int V_index = U_index + rows * cols/4;
        int RGB_index = 0;

        for (int r = 0; r <= rows - 2; r += 2)
        {
            for (int c = 0; c <= cols - 2; c += 2)
            {
                //gather yuv information for 4 pixel group
                int y00, y01, y10, y11, u, v;
                y00 = yuv[Y_index] & 0xFF;
                y01 = yuv[Y_index+1] & 0xFF;
                y10 = yuv[Y_index + cols] & 0xFF;
                y11 = yuv[Y_index + cols + 1] & 0xFF;
                u = (yuv[U_index] & 0xFF) - 128;
                v = (yuv[V_index] & 0xFF) - 128;

//                if(c == 20 && r == 40){
//                	System.out.println("y, u, v" + y00 + " " + y01 + " " + y10 + " " + y11 + " " + u + " " + v);
//                }
                
                //calculate rgb base components
                int R, G, B, x;
                G = guScale * u + gvScale * v;
                R = rvScale * v;
                B = buScale * u;
                /*
                rgb[RGB_index] = (byte) y00;
                rgb[RGB_index + 1] = (byte)y00;
                rgb[RGB_index + 2] = (byte)y00;
                rgb[RGB_index + 3] = (byte)y01;
                rgb[RGB_index + 4] = (byte)y01;
                rgb[RGB_index + 5] = (byte)y01;
                rgb[RGB_index + cols * 3] = (byte)y10;
                rgb[RGB_index + 1 + cols * 3] = (byte)y10;
                rgb[RGB_index + 2 + cols * 3] = (byte)y10;
                rgb[RGB_index + 3 + cols * 3] = (byte)y11;
                rgb[RGB_index + 4 + cols * 3] = (byte)y11;
                rgb[RGB_index + 5 + cols * 3] = (byte)y11;
                */

                
                //top left pixel
                x = R + y00 * yScale;
//                if(c == 20) System.out.print(x + " ");
                rgb[RGB_index] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y00 * yScale;
//                if(c == 20) System.out.print(x + " ");
                rgb[RGB_index + 1] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y00 * yScale;
//                if(c == 20) System.out.print(x + " ");
                rgb[RGB_index + 2] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));

//                if(c == 20) System.out.println("{" + (rgb[RGB_index] & 0xFF) + " " + (rgb[RGB_index+1] & 0xFF)+ " " + (rgb[RGB_index+2] & 0xFF) + "}");
                
                //top right pixel
                x = R + y01 * yScale;
                rgb[RGB_index + 3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y01 * yScale;
                rgb[RGB_index + 4] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y01 * yScale;
                rgb[RGB_index + 5] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));

                //bottom left pixel
                x = R + y10 * yScale;
                rgb[RGB_index + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y10 * yScale;
                rgb[RGB_index + 1 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y10 * yScale;
                rgb[RGB_index + 2 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));

                //bottom right pixel
                x = R + y11 * yScale;
                rgb[RGB_index + 3 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y11 * yScale;
                rgb[RGB_index + 4 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y11 * yScale;
                rgb[RGB_index + 5 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                
                

                Y_index += 2;
                U_index++;
                V_index++;

                RGB_index += 6;

            }
            Y_index += cols;
            RGB_index += 3 * cols;
        }

    }
}