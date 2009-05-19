package rover.utils;

import java.io.File;

import javax.swing.filechooser.FileFilter;


public class ExtFilter extends FileFilter {

	String ext;
	
	public ExtFilter(String extension){
		ext = extension;
	}
	
	@Override
	public boolean accept(File file) {
		return file.getName().endsWith(ext);
	}

	@Override
	public String getDescription() {
		return "" + ext + " files";
	}

}
