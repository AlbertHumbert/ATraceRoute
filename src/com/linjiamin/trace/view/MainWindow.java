package com.linjiamin.trace.view;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class MainWindow extends Application {

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        initWindow(primaryStage);
    }

    private void initWindow(Stage primaryStage) throws Exception {
        Parent root = FXMLLoader.load(getClass().getResource("main_window.fxml"));
        Scene scene = new Scene(root, 420, 500);
        scene.getStylesheets().setAll(getClass().getResource("MainTheme.css").toExternalForm());
        primaryStage.setTitle("TraceRoute");
        primaryStage.setScene(scene);
        primaryStage.show();
    }
}
