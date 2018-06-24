package trace;


/**
 * 以下为测试代码，写 Fx界面时使用，之后移到 jar 里面去
 */

public enum TraceHelper {


    //枚举单例，不确定可以调到native方法，暂时先这么写
    INSTANCE;

    TraceHelper() {

        //  System.loadLibrary("lib" + LibResolver.getOsType().getLibType());

    }

    public void stop(){

    }

    public void start(String address,int ttl){

    }

    //  public native void register();

    //  public native void start(String address, int ttl);
}
