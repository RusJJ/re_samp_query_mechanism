#include <jni.h>
#include "sqm.hpp"

extern "C" {
    
JNIEXPORT jlong JNICALL Java_net_rusjj_sampquery_NativeInterface_doQuery(JNIEnv* env, jobject thiz, jint type, jstring ip, jint port)
{
    jlong ret = 0;
    const char* ipPtr = env->GetStringUTFChars(ip, NULL);
    
    auto sqm = c_sqm::singleton();
	sqm->initialize(ipPtr, port);
    if(type == QUERYTYPE_SERVERINFO)
    {
        ret = (jlong)( sqm->handleInst<s_server_info>(sqm->query(QUERYTYPE_SERVERINFO, 1)) );
    }
    sqm->shutdown();
    return ret;
}

JNIEXPORT void JNICALL Java_net_rusjj_sampquery_NativeInterface_cleanQuery(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return;
    s_query_base* query_base = (s_query_base*)query;
    if(query_base->type == QUERYTYPE_SERVERINFO)
    {
        delete ((s_server_info*)query);
    }
}

JNIEXPORT jint JNICALL Java_net_rusjj_sampquery_NativeInterface_getPlayers(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return 0;
    s_server_info* s_query = (s_server_info*)query;
    if(s_query->type != QUERYTYPE_SERVERINFO) return 0;
    return s_query->players;
}

JNIEXPORT jint JNICALL Java_net_rusjj_sampquery_NativeInterface_getMaxPlayers(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return 0;
    s_server_info* s_query = (s_server_info*)query;
    if(s_query->type != QUERYTYPE_SERVERINFO) return 0;
    return s_query->max_players;
}

JNIEXPORT jstring JNICALL Java_net_rusjj_sampquery_NativeInterface_getHostname(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return env->NewStringUTF("Unresolved server...");
    s_server_info* s_query = (s_server_info*)query;
    if(s_query->type != QUERYTYPE_SERVERINFO) env->NewStringUTF("Unresolved server...");
    return env->NewStringUTF(s_query->host_name.c_str());
}

JNIEXPORT jstring JNICALL Java_net_rusjj_sampquery_NativeInterface_getGamemode(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return env->NewStringUTF("-");
    s_server_info* s_query = (s_server_info*)query;
    if(s_query->type != QUERYTYPE_SERVERINFO) env->NewStringUTF("-");
    return env->NewStringUTF(s_query->game_mode.c_str());
}

JNIEXPORT jstring JNICALL Java_net_rusjj_sampquery_NativeInterface_getLanguage(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return env->NewStringUTF("-");
    s_server_info* s_query = (s_server_info*)query;
    if(s_query->type != QUERYTYPE_SERVERINFO) env->NewStringUTF("-");
    return env->NewStringUTF(s_query->language.c_str());
}

};
