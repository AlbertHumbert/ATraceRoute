package view;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import trace.NodeInfo;

public class TableRow {

    private StringProperty ttl;
    private StringProperty timeMilli;
    private StringProperty address;

    public TableRow(NodeInfo nodeInfo) {
        ttl = new SimpleStringProperty(nodeInfo.getTtl() + "");
        timeMilli = new SimpleStringProperty(nodeInfo.getTimeMili() + "");
        address = new SimpleStringProperty(nodeInfo.getAddress() + "");

    }

    public String getTtl() {
        return ttl.get();
    }

    public StringProperty ttlProperty() {
        return ttl;
    }

    public String getTimeMilli() {
        return timeMilli.get();
    }

    public StringProperty timeMilliProperty() {
        return timeMilli;
    }

    public String getAddress() {
        return address.get();
    }

    public StringProperty addressProperty() {
        return address;
    }
}
