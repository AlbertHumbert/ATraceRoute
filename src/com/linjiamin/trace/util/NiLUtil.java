package com.linjiamin.trace.util;

import java.util.List;

public class NiLUtil {

    public static boolean isNIL(List l) {
        return l == null || l.isEmpty();
    }

    public static boolean isNIL(String s) {
        return s == null || s.trim().isEmpty();
    }
}
