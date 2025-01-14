package net.rusjj.sampquery;

public class NativeInterface
{
    public final static int QUERYTYPE_SERVERINFO = 0;
    public final static int QUERYTYPE_SERVERRULES = QUERYTYPE_SERVERINFO+1;
    public final static int QUERYTYPE_PSEUDORANDOM = QUERYTYPE_SERVERRULES+1;
    public final static int QUERYTYPE_PLAYERLIST = QUERYTYPE_PSEUDORANDOM+1;
    public final static int QUERYTYPE_DETAILPLAYERLIST = QUERYTYPE_PLAYERLIST+1;
    public final static int QUERYTYPE_RCON = QUERYTYPE_DETAILPLAYERLIST+1;
    public final static int QUERYTYPES_COUNT = QUERYTYPE_RCON+1;
    
    public static NativeInterface Instance = new NativeInterface();
    
    public native long doQuery(int type, String ip, int port);
    public native void cleanQuery(long queryObject);
    public native int getLastDelay();
    
    // server info
    public native int getPlayers(long queryObject);
    public native int getMaxPlayers(long queryObject);
    public native String getHostname(long queryObject);
    public native String getGamemode(long queryObject);
    public native String getLanguage(long queryObject);

    // player list
    public native int getPlayerListCount(long queryObject);
    
    static
    {
        System.loadLibrary("squery");
    }
}