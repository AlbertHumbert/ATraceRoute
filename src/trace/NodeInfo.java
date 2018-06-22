package trace;


/**
 * 路由节点的信息
 * 包含ttl，时间，地址（暂定为ip）
 */
public class NodeInfo {

    private int ttl;
    private int timeMilli;
    private String address;

    public NodeInfo() {
    }

    public NodeInfo(int ttl, int timeMili, String ip) {
        this.ttl = ttl;
        this.timeMilli = timeMili;
        this.address = ip;
    }

    public int getTtl() {
        return ttl;
    }

    public void setTtl(int ttl) {
        this.ttl = ttl;
    }

    public int getTimeMili() {
        return timeMilli;
    }

    public void setTimeMili(int timeMili) {
        this.timeMilli = timeMili;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }
}
