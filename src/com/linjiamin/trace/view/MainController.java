package com.linjiamin.trace.view;

import com.linjiamin.trace.ATraceRoute;
import com.linjiamin.trace.LibResolver;
import com.linjiamin.trace.NodeInfo;
import com.linjiamin.trace.TraceResultListener;
import com.linjiamin.trace.util.PathUtil;
import com.linjiamin.trace.util.TimeUtil;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.*;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;


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
        ATraceRoute.getInstance().initEnv(libPath);
    }

    @FXML
    private void onStartClick() {
        if (!initTable) {
            resumeTable(rows);
            initTable = true;
        } else {
            rows.clear();
            refreshTable();
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

    @Override
    public void onNewNodeTraced(NodeInfo newNode) {
        Platform.runLater(() -> {
            try {

                AtomicBoolean isPresent = new AtomicBoolean(false);
                rows.stream()
                        .filter(rows -> rows.getTtl().equals(String.valueOf(newNode.getTtl())))
                        .findAny()
                        .ifPresent(rows -> {
                            isPresent.set(true);
                            if (rows.addressProperty().getValue() == null) {
                                rows.addressProperty().setValue(rows.getAddress());
                            }

                            String preTime = rows.timeMilliProperty().getValue();
                            String newTime = preTime + (newNode.getTimeMili() == -1 ? "  *" : "  " + TimeUtil.microsec2millsec(newNode.getTimeMili()));
                            rows.timeMilliProperty().setValue(newTime);

                        });

                if (!isPresent.get()) {
                    rows.add(new TableRow(newNode));
                }

                refreshTable();
            } catch (Exception e) {
                showAlertWindow("网络错误", "请检查输入和网络状态");
            }
        });
    }


    @FXML
    private void onClearClick() {
        tfMaxTTL.clear();
        tfHostName.clear();
        nodeInfos.clear();
        rows.clear();
        refreshTable();
        btnStart.setText("开始");
    }

    @Override
    public void onFinish() {
        //showAlertWindow("请求完成", "共经过" + rows.size() + "个节点");

        Platform.runLater(() -> btnStart.setText("重试"));

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
