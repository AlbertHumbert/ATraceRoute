package com.linjiamin.trace;

public class TraceTest {
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
}
