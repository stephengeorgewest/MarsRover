package rover.video;
import javax.media.Buffer;

/*
 * Name: FrameListener
 * Author: Travis L Brown
 * Description: 
 * Interface for raising events when new video frames are available. 
 * 
 */
public interface FrameListener {

	void nextFrameReady(Buffer current_frame, int channel);

}
