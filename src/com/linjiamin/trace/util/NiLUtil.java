package com.linjiamin.trace.util;

import java.util.List;

public class NiLUtil {

    public boolean isNIL(List l) {
        return l == null || l.isEmpty();
    }

    public boolean isNIL(String s) {
        return s == null || s.trim().isEmpty();
    }
}
