
//
// Created by Albert Humbert on 18/6/23.
//

#include "ATraceRoute.h"
#include "traceroute.h"

void callOnNodeTraced(JNIEnv *env, char *dest, char *curAddress, int ttl, int time);

void callOnTraceFinish(JNIEnv *env);

void nodeInfoRecv(const char *dest, char *curAddress, int ttl, int time,char * errmsg);

jobject jTraceRoute;
JavaVM *jvm;

JNIEXPORT void JNICALL Java_com_linjiamin_trace_ATraceRoute_init
        (JNIEnv *env, jobject jobj) {
    //java层的ATraceRoute对象
    jTraceRoute = env->NewGlobalRef(jobj);
    //缓存JVM
    env->GetJavaVM(&jvm);
}

JNIEXPORT void JNICALL Java_com_linjiamin_trace_ATraceRoute_stop
        (JNIEnv *env, jobject jObj) {
    callOnTraceFinish(env);
}

JNIEXPORT void JNICALL Java_com_linjiamin_trace_ATraceRoute_start
        (JNIEnv *env, jobject jObj, jstring jAddress, jint jTTL) {


    setTask(env->GetStringUTFChars(jAddress, 0), jTTL,3,3);
    start(nodeInfoRecv);
}

//调用 java 层 onNodeTraced 方法
void callOnNodeTraced(JNIEnv *env, const char *dest, char *curAddress, int ttl, int time) {
    jstring jStrDest = env->NewStringUTF(dest);
    jstring jStrCurAddress = env->NewStringUTF(curAddress);
    jclass clazz = env->FindClass("com/linjiamin/trace/ATraceRoute");
    jmethodID methodID = env->GetMethodID(clazz, "onNodeTraced", "(Ljava/lang/String;Ljava/lang/String;II)V");
    env->CallVoidMethod(jTraceRoute, methodID, jStrDest, jStrCurAddress, ttl, time);
}


//调用 java 层 onTraceFinish 方法
void callOnTraceFinish(JNIEnv *env) {
/*    jstring jStrDest = env->NewStringUTF("111.111.111.111");
    jclass clazz = env->FindClass("com/linjiamin/trace/ATraceRoute");
    jmethodID methodID = env->GetMethodID(clazz, "onTraceFinish", "(Ljava/lang/String;)V");
    env->CallVoidMethod(jTraceRoute, methodID, jStrDest);*/
}

//接收节点信息
void nodeInfoRecv(const char *dest, char *curAddress, int ttl, int time, char *errMsg) {
/*    JNIEnv *env;
    jvm->AttachCurrentThread(reinterpret_cast<void **>(&env), nullptr);
    callOnNodeTraced(env, dest, curAddress, ttl, time);
    jvm->DetachCurrentThread();*/
}



