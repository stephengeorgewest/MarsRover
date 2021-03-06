package rover.guistuff;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;

import javax.swing.JPanel;

public class ImagePanel extends JPanel{

	Image image = null;
	
	ImagePanel.Mode m = Mode.CONST_ASPECT_STRETCH;
	
	
	public void setImage(Image i){
		image = i;
	}
	
	public void setMode(ImagePanel.Mode m){
		this.m = m;
	}
	
	protected void paintComponent(Graphics g)
    {
        if (image == null) return;

        Dimension d = getSize();
        double iheight = image.getHeight(null);
        double iwidth = image.getWidth(null);
//        System.out.println("height = " + iheight + " width = " + iwidth);
        switch(m){
	        case SKEW_STRETCH:
	        	g.drawImage(image, 0, 0, d.width, d.height, null);
	        break;
	        case CONST_ASPECT_STRETCH:
	        	if(d.height/iheight > d.width/iwidth){
	        		double wscale = d.width/iwidth;
	        		int new_height = (int)(wscale*iheight);
	        		g.drawImage(image, 0, (d.height-new_height)/2, d.width, new_height, null);
	        	}else{
	        		double hscale = d.height/iheight;
	        		int new_width = (int)(hscale*iwidth);
	        		g.drawImage(image, (d.width-new_width)/2, 0, new_width, d.height, null);
	        	}
	        break;
        
        }
        
       
    }
	
	public enum Mode{
		SKEW_STRETCH, 
		CONST_ASPECT_STRETCH;
	}

}
