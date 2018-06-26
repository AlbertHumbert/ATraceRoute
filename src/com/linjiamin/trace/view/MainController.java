package com.linjiamin.trace.view;

import com.linjiamin.trace.ATraceRoute;
import com.linjiamin.trace.LibResolver;
import com.linjiamin.trace.NodeInfo;
import com.linjiamin.trace.TraceResultListener;
import com.linjiamin.trace.util.PathUtil;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.*;

import java.util.ArrayList;
import java.util.List;


public class MainController implements TraceResultListener {

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

    private boolean initTable;

    private List<NodeInfo> nodeInfos = new ArrayList<>();
    private ObservableList<TableRow> rows = FXCollections.observableArrayList();

    static {
        String libType = LibResolver.getOsType().getLibType();
        String libPath = PathUtil.getProjectPath() + "/libtrace" + libType;
        System.out.println(libPath);
        ATraceRoute.getInstance().initEnv(libPath);
    }

    @FXML
    private void onStartClick() {

        if (!initTable) {
            resumeTable(rows);
        }
        ATraceRoute.getInstance().register(tfHostName.getText(), this);

        //new Thread(this::beginTrace).start();
        beginTrace();
    }

    private void beginTrace() {
        try {
            ATraceRoute.getInstance().start(tfHostName.getText(), Integer.parseInt(tfMaxTTL.getText()));
        } catch (NumberFormatException e) {
            showAlertWindow("输入出错", "请正确填写主机和TTL");
        }
    }

    @FXML
    private void onClearClick() {
        tfMaxTTL.clear();
        tfHostName.clear();
        nodeInfos.clear();
        rows.clear();
        refreshTable();
    }

    @Override
    public void onNewNodeTraced(NodeInfo newNode) {
        Platform.runLater(() -> {
            try {

                nodeInfos.add(newNode);
                rows.add(new TableRow(newNode));

                if (newNode.getAddress() == null) {
                    nodeInfos.stream()
                            .filter(nodeInfo -> nodeInfo.getTtl() == newNode.getTtl() && nodeInfo.getAddress() != null)
                            .findAny()
                            .ifPresent(preNode -> newNode.setAddress(preNode.getAddress()));

                }
                refreshTable();
            } catch (Exception e) {
                showAlertWindow("网络错误", "请检查输入和网络状态");
            }
        });
    }

    @Override
    public void onFinish() {
        showAlertWindow("请求完成", "共经过" + nodeInfos.size() + "个节点");
    }

    private void refreshTable() {
        tbNode.refresh();
    }

    private void resumeTable(ObservableList<TableRow> rows) {
        colTTl.setCellValueFactory(cellData -> cellData.getValue().ttlProperty());
        colAddress.setCellValueFactory(cellData -> cellData.getValue().addressProperty());
        colTime.setCellValueFactory(cellData -> cellData.getValue().timeMilliProperty());
        tbNode.setItems(rows);
    }

    private void showAlertWindow(String headerText, String msg) {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setHeaderText(headerText);
        alert.setContentText(msg);
        alert.showAndWait();
    }
}
