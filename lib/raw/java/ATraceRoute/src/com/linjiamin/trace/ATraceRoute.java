package com.linjiamin.trace;

import java.io.File;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.WeakHashMap;


//todo 当前只支持唯一任务，和设置ttl，之后添加-q，再考虑能够同时执行多个
public class ATraceRoute {

    public static void main(String args[]) {
        ATraceRoute.getInstance().register(args[0], new TraceResultListener() {
            @Override
            public void onNewNodeTraced(NodeInfo nodeInfo) {
                System.out.println(nodeInfo);
            }

            @Override
            public void onFinish() {
                System.out.println("finish");
            }
        });

        ATraceRoute.getInstance().start(args[0], 16);
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
        try {
            URI uri = getClass().getResource("/libtrace.dylib").toURI();
            System.load(new File(uri).getAbsolutePath());
        } catch (URISyntaxException e) {
            e.printStackTrace();
        }
        init();
        stop();

    }


    public void register(String dest, TraceResultListener listener) {
        mListeners.put(dest, listener);
    }

    public void unRegister(String desc) {
        mListeners.remove(desc);
    }

    public void onNodeTraced(String dest, String curAddress, int ttl, int time) {
        System.out.println(dest + " " + curAddress + " " + ttl + " " + time);
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