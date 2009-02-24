package rover.portal;

import javax.swing.JPanel;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.BorderLayout;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;

import javax.swing.JDialog;

public abstract class Portal extends JDialog {


	public Portal(Frame owner) {
		super(owner);
		
	}

	
	
	public abstract void Shutdown();
	

}
