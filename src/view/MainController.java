package view;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import trace.NodeInfo;
import trace.TraceHelper;

import java.util.ArrayList;
import java.util.List;

public class MainController {

    @FXML
    private TableColumn<TableRow, String> colTTl;
    @FXML
    private TableColumn<TableRow, String> colAddress;
    @FXML
    private TableColumn<TableRow, String> colTime;
    @FXML
    private TableView<TableRow> tbNode;

    @FXML
    private Pagination pn_pagination;


    @FXML
    private TextField tfHostName;
    @FXML
    private TextField tfMaxTTL;
    @FXML
    private Button btnStart;
    @FXML
    private Button btnClear;
    @FXML
    private TextArea taNodeInfo;

    @FXML
    private void onClearClick() {
        tfMaxTTL.clear();
        tfHostName.clear();

    }

    @FXML
    private void onStartClick() {

        try {
            TraceHelper.INSTANCE.start(tfHostName.getText(), Integer.parseInt(tfMaxTTL.getText()));
        } catch (NumberFormatException e) {
            showAlertWindow("输入出错", "请正确填写主机和TTL");
            //  return;
        }

        try {
            showTestTable();
            taNodeInfo.setText("正在开始...");
        } catch (Exception e) {
            showAlertWindow("网络错误", "请检查输入和网络状态");
        }
    }

    private void showAlertWindow(String headerText, String msg) {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setHeaderText(headerText);
        alert.setContentText(msg);
        alert.showAndWait();
    }

    private void showTestTable() {

        ObservableList<TableRow> nodeInfos = FXCollections.observableArrayList();

        nodeInfos.add(new TableRow(new NodeInfo(1, 13424, "113.15.241.17")));
        nodeInfos.add(new TableRow(new NodeInfo(2, 13344, "161.13.13.12")));
        nodeInfos.add(new TableRow(new NodeInfo(3, 1364, "115.16.112.14")));
        nodeInfos.add(new TableRow(new NodeInfo(4, 1354, "141.111.165.61")));
        nodeInfos.add(new TableRow(new NodeInfo(5, 1314, "231.131.135.14")));
        nodeInfos.add(new TableRow(new NodeInfo(1, 13424, "113.15.241.17")));
        nodeInfos.add(new TableRow(new NodeInfo(2, 13344, "161.13.13.12")));
        nodeInfos.add(new TableRow(new NodeInfo(3, 1364, "115.16.112.14")));
        nodeInfos.add(new TableRow(new NodeInfo(4, 1354, "141.111.165.61")));
        nodeInfos.add(new TableRow(new NodeInfo(5, 1314, "231.131.135.14")));
        nodeInfos.add(new TableRow(new NodeInfo(1, 13424, "113.15.241.17")));
        nodeInfos.add(new TableRow(new NodeInfo(2, 13344, "161.13.13.12")));
        nodeInfos.add(new TableRow(new NodeInfo(3, 1364, "115.16.112.14")));
        nodeInfos.add(new TableRow(new NodeInfo(4, 1354, "141.111.165.61")));
        nodeInfos.add(new TableRow(new NodeInfo(5, 1314, "231.131.135.14")));
        nodeInfos.add(new TableRow(new NodeInfo(1, 13424, "113.15.241.17")));
        nodeInfos.add(new TableRow(new NodeInfo(2, 13344, "161.13.13.12")));
        nodeInfos.add(new TableRow(new NodeInfo(3, 1364, "115.16.112.14")));
        nodeInfos.add(new TableRow(new NodeInfo(4, 1354, "141.111.165.61")));
        nodeInfos.add(new TableRow(new NodeInfo(5, 1314, "231.131.135.14")));
        nodeInfos.add(new TableRow(new NodeInfo(1, 13424, "113.15.241.17")));
        nodeInfos.add(new TableRow(new NodeInfo(2, 13344, "161.13.13.12")));
        nodeInfos.add(new TableRow(new NodeInfo(3, 1364, "115.16.112.14")));
        nodeInfos.add(new TableRow(new NodeInfo(4, 1354, "141.111.165.61")));
        nodeInfos.add(new TableRow(new NodeInfo(5, 1314, "231.131.135.14")));
        nodeInfos.add(new TableRow(new NodeInfo(1, 13424, "113.15.241.17")));
        nodeInfos.add(new TableRow(new NodeInfo(2, 13344, "161.13.13.12")));
        nodeInfos.add(new TableRow(new NodeInfo(3, 1364, "115.16.112.14")));
        nodeInfos.add(new TableRow(new NodeInfo(4, 1354, "141.111.165.61")));
        nodeInfos.add(new TableRow(new NodeInfo(5, 1314, "231.131.135.14")));
        colTTl.setCellValueFactory(cellData -> cellData.getValue().ttlProperty());
        colAddress.setCellValueFactory(cellData -> cellData.getValue().addressProperty());
        colTime.setCellValueFactory(cellData -> cellData.getValue().timeMilliProperty());
        tbNode.setItems(nodeInfos);

    }
}
