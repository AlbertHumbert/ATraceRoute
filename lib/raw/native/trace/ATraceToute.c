
//
// Created by Albert Humbert on 18/6/23.
//

#include "ATraceRoute.h"
#include "traceroute.h"

void callOnNodeTraced(JNIEnv *env, char *dest, char *curAddress, int ttl, int time);

void callOnTraceFinish(JNIEnv *env);

void nodeInfoRecv(char *dest, char *curAddress, int ttl, int time, char *msg);

jobject jTraceRoute;
JavaVM *jvm;

JNIEXPORT void JNICALL Java_com_linjiamin_trace_ATraceRoute_init
        (JNIEnv *env, jobject jobj) {
    //java层的ATraceRoute对象
    jTraceRoute = (*env)->NewGlobalRef(env, jobj);
    //缓存JVM
    (*env)->GetJavaVM(env, &jvm);
}

JNIEXPORT void JNICALL Java_com_linjiamin_trace_ATraceRoute_stop
        (JNIEnv *env, jobject jObj) {
    callOnTraceFinish(env);
}

JNIEXPORT void JNICALL Java_com_linjiamin_trace_ATraceRoute_start
        (JNIEnv *env, jobject jObj, jstring jAddress, jint jTTL) {

    const char *temp = (*env)->GetStringUTFChars(env, jAddress, 0);
    char * hostname = malloc(strlen(temp)+1);
    strcpy(hostname,temp);
    start(hostname, jTTL, 10, nodeInfoRecv);
}

//调用 java 层 onNodeTraced 方法
void callOnNodeTraced(JNIEnv *env, char *dest, char *curAddress, int ttl, int time) {
    jstring jStrDest = (*env)->NewStringUTF(env, dest);
    jstring jStrCurAddress = (*env)->NewStringUTF(env, curAddress);
    jclass clazz = (*env)->FindClass(env, "com/linjiamin/trace/ATraceRoute");
    jmethodID methodID = (*env)->GetMethodID(env, clazz, "onNodeTraced", "(Ljava/lang/String;Ljava/lang/String;II)V");
    (*env)->CallVoidMethod(env, jTraceRoute, methodID, jStrDest, jStrCurAddress, ttl, time);
}


//调用 java 层 onTraceFinish 方法
void callOnTraceFinish(JNIEnv *env) {
/*    jstring jStrDest = env->NewStringUTF("111.111.111.111");
    jclass clazz = env->FindClass("com/linjiamin/trace/ATraceRoute");
    jmethodID methodID = env->GetMethodID(clazz, "onTraceFinish", "(Ljava/lang/String;)V");
    env->CallVoidMethod(jTraceRoute, methodID, jStrDest);*/
}

//接收节点信息
void nodeInfoRecv(char *dest, char *curAddress, int ttl, int time, char *msg) {
    JNIEnv *env;
    (*jvm)->AttachCurrentThread(jvm, (void **) &env, NULL);
    callOnNodeTraced(env, dest, curAddress, ttl, time);
    (*jvm)->DetachCurrentThread(jvm);
}



