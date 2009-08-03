package rover.video;

import java.io.IOException;

import javax.media.protocol.ContentDescriptor;
import javax.media.protocol.PushSourceStream;
import javax.media.protocol.SourceTransferHandler;

public class MulticastPushStream implements PushSourceStream{

	@Override
	public int getMinimumTransferSize() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int read(byte[] arg0, int arg1, int arg2) throws IOException {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public void setTransferHandler(SourceTransferHandler arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean endOfStream() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public ContentDescriptor getContentDescriptor() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public long getContentLength() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public Object getControl(String arg0) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Object[] getControls() {
		// TODO Auto-generated method stub
		return null;
	}

}
