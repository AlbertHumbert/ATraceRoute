package com.linjiamin.trace.util;

import java.io.File;

public class PathUtil {

    public static String getProjectPath() {

        java.net.URL url = PathUtil.class.getProtectionDomain().getCodeSource().getLocation();

        String filePath = null;

        try {

            filePath = java.net.URLDecoder.decode(url.getPath(), "utf-8");

        } catch (Exception e) {

            e.printStackTrace();

        }

        if (filePath == null) {
            return null;
        }

        if (filePath.endsWith(".jar")) {
            filePath = filePath.substring(0, filePath.lastIndexOf("/") + 1);
        }

        File file = new File(filePath);

        filePath = file.getAbsolutePath();

        return filePath;

    }
}
