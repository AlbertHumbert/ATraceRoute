package com.linjiamin.trace.util;

public class TimeUtil {


    public static String microsec2millsec(int micro) {
        return String.format("%.3f", micro / 1000.0);
    }
}
