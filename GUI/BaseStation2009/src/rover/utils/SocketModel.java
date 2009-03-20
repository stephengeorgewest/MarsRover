package rover.utils;

import java.util.ArrayList;

import javax.swing.table.AbstractTableModel;

import rover.network.SocketInfo;

public class SocketModel extends AbstractTableModel {
	private static final long serialVersionUID = 1L;
    private ArrayList<SocketInfo> data;

    public SocketModel(){
    	data = new ArrayList<SocketInfo>();
    }
    public int getColumnCount() { return SocketInfo.classTypes.length;}
    public int getRowCount() { return data.size();}
    public String getColumnName(int col) {return SocketInfo.Names[col];}

    public Object getValueAt(int row, int col){
        return data.get(row).data[col];
    }
    public Class getColumnClass(int c) {
        return SocketInfo.classTypes[c];
    }
    public boolean isCellEditable(int row, int col) {
       return SocketInfo.editable[col];
    }
    public void setValueAt(Object value, int row, int col) {
        data.get(row).data[col] = value;
        fireTableCellUpdated(row, col);
    }
    public void addSocket(SocketInfo si){
    	data.add(si);
    	this.fireTableDataChanged();
    }
    public void removeSocket(SocketInfo si){
    	data.remove(si);
    	this.fireTableDataChanged();
    }

}