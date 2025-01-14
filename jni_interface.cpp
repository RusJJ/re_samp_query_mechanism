#include <jni.h>
#include "sqm.hpp"

extern "C" {
    
    // All queries
JNIEXPORT jlong JNICALL Java_net_rusjj_sampquery_NativeInterface_doQuery(JNIEnv* env, jobject thiz, jint type, jstring ip, jint port)
{
    jlong ret = 0;
    const char* ipPtr = env->GetStringUTFChars(ip, NULL);
    
    auto sqm = c_sqm::singleton();
	sqm->initialize(ipPtr, port);
    if(type == QUERYTYPE_SERVERINFO)
    {
        ret = (jlong)( sqm->handleInst<s_server_info>(sqm->query((e_query_type)type, 1)) );
    }
    else if(type == QUERYTYPE_PLAYERLIST)
    {
        ret = (jlong)( sqm->handleInst<s_player_list>(sqm->query((e_query_type)type, 1)) );
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
    else if(query_base->type == QUERYTYPE_PLAYERLIST)
    {
        delete ((s_player_list*)query);
    }
}
JNIEXPORT jint JNICALL Java_net_rusjj_sampquery_NativeInterface_getLastDelay(JNIEnv* env, jobject thiz)
{
    return c_sqm::last_ping;
}



    // Server info query
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
    if(!query) return env->NewStringUTF("English");
    s_server_info* s_query = (s_server_info*)query;
    if(s_query->type != QUERYTYPE_SERVERINFO) env->NewStringUTF("-");
    return env->NewStringUTF(s_query->language.c_str());
}



    // Players list query
JNIEXPORT jint JNICALL Java_net_rusjj_sampquery_NativeInterface_getPlayerListCount(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return 0;
    s_player_list* s_query = (s_player_list*)query;
    if(s_query->type != QUERYTYPE_PLAYERLIST) return 0;

    return s_query->players.size();
}
JNIEXPORT jlong JNICALL Java_net_rusjj_sampquery_NativeInterface_getPlayersIterator(JNIEnv* env, jobject thiz, jlong query)
{
    if(!query) return 0;
    s_player_list* s_query = (s_player_list*)query;
    if(s_query->type != QUERYTYPE_PLAYERLIST) return 0;

    return (jlong)s_query->players.begin();
}

};
