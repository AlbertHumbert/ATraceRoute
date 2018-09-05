package com.linjiamin.trace;


import java.util.WeakHashMap;


//todo 当前只支持唯一任务，和设置ttl，之后添加-q
public class ATraceRoute {

    public static void main(String args[]) {
    }

    private static ATraceRoute INSTANCE;
    private WeakHashMap<String, TraceResultListener> mListeners;


    public static ATraceRoute getInstance() {
        if (INSTANCE == null) {
            synchronized (ATraceRoute.class) {
                if (INSTANCE == null) {
                    INSTANCE = new ATraceRoute();
                }
            }
        }

        return INSTANCE;
    }

    private ATraceRoute() {
        mListeners = new WeakHashMap<>();
    }

    public void initEnv(String libPath) {
        System.load(libPath);
        init();
    }

    public void register(String dest, TraceResultListener listener) {
        mListeners.put(dest, listener);
    }

    public void unRegister(String desc) {
        mListeners.remove(desc);
    }

    public void onNodeTraced(String dest, String curAddress, int ttl, int time) {
        TraceResultListener listener = mListeners.get(dest);
        if (listener != null) {
            listener.onNewNodeTraced(new NodeInfo(ttl, time, curAddress));
        }
    }


    public void onTraceFinish(String dest) {
        System.out.println(dest);
        TraceResultListener listener = mListeners.get(dest);
        if (listener != null) {
            listener.onFinish();
        }
    }

    private native void init();

    public native void start(String address, int ttl);

    public native void stop();


}