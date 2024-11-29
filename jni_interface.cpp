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

};
