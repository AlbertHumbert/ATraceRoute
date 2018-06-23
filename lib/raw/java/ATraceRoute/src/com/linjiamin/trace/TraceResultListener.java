package com.linjiamin.trace;

/**
 * trace结果监听
 */
public interface TraceResultListener {

    /**
     * 每获取到一个新节点时调用该方法传递节点的扫描信息
     *
     * @param nodeInfo 节点信息
     */
    void onNewNodeTraced(NodeInfo nodeInfo);


    /**
     * trace结束时调用
     * 导致trace 正常结束的情况有如下几种：
     * 1.到达目的主机
     * 2.到达最大ttl
     * 3.接受到ICMP错误报文
     */
    void onFinish();

}
