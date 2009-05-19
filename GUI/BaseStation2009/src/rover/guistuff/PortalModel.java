package rover.guistuff;

import java.util.ArrayList;

import javax.swing.ListModel;
import javax.swing.event.ListDataListener;

import rover.portal.Portal;

public class PortalModel implements ListModel {

	ArrayList<Portal> portalList;
	ArrayList<ListDataListener> listenerList;
	
	public PortalModel(){
		portalList = new ArrayList<Portal>();
		listenerList = new ArrayList<ListDataListener>();
	}
	
	public void setPortalList(ArrayList<Portal> list){
		portalList = list;
		for(ListDataListener l: listenerList){
			l.contentsChanged(null);
		}
	}
	
	@Override
	public void addListDataListener(ListDataListener arg0) {
		listenerList.add(arg0);
	}

	@Override
	public Object getElementAt(int index) {
		return portalList.get(index);
	}

	@Override
	public int getSize() {
		return portalList.size();
	}

	@Override
	public void removeListDataListener(ListDataListener arg0) {
		listenerList.remove(arg0);
	}

}
