package tool.cache;

import com.danga.MemCached.SockIOPool;
import com.danga.MemCached.MemCachedClient;

import java.util.Date;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import tool.SuperString;
import tool.SuperDate;
import my.myproperties.tool.PropUtil;

/**
 * Created by IntelliJ IDEA.
 * @version 1.0
 *          Date: 2008-4-10
 *          Time: 19:45:46
 */
public class MemCache {

    private static final Log logger = LogFactory.getLog(MemCache.class);

    protected static MemCachedClient mcc = new MemCachedClient();

    static {
        initCacheServer();
    }

    public static void initCacheServer() {
        //String[] servers = {"192.168.21.253:11211"};
        String serverstr = PropUtil.getInstance("memcache").getProperty("com.vip.memcache.server");
        int initconn = SuperString.getInt(PropUtil.getInstance("memcache").getProperty("com.vip.memcache.initconn"));
        int minconn = SuperString.getInt(PropUtil.getInstance("memcache").getProperty("com.vip.memcache.minconn"));
        int maxconn = SuperString.getInt(PropUtil.getInstance("memcache").getProperty("com.vip.memcache.maxconn"));

        String[] servers = {serverstr};
        Integer[] weights = {3};

        //创建一个实例对象SockIOPool
        SockIOPool pool = SockIOPool.getInstance("memcache");
        mcc.setPoolName("memcache");
        // set the servers and the weights
        //设置Memcached Server
        pool.setServers(servers);
        pool.setWeights(weights);

        pool.setInitConn(initconn);
        pool.setMinConn(minconn);
        pool.setMaxConn(maxconn);
        pool.setMaxIdle(1000 * 60 * 60 * 6);

        pool.setMaintSleep(30);

        //        Tcp的规则就是在发送一个包之前，本地机器会等待远程主机
        //        对上一次发送的包的确认信息到来；这个方法就可以关闭套接字的缓存，
        //        以至这个包准备好了就发；
        pool.setNagle(false);
        //连接建立后对超时的控制
        pool.setSocketTO(3000);
        //连接建立时对超时的控制
        pool.setSocketConnectTO(0);

        // initialize the connection pool
        //初始化一些值并与MemcachedServer段建立连接
        pool.initialize();

        // lets set some compression on for the client
        // compress anything larger than 64k
        mcc.setCompressEnable(true);
        mcc.setCompressThreshold(64 * 1024);

        logger.info("********** MemCache init connect ok, " + new SuperDate().getDateTimeString() + " *************");

    }

    public static  boolean set(String key, Object o) {
        return mcc.set(key, o);
    }

    public static boolean set(String key, Object o, Date d) {
        return mcc.set(key, o, d);
    }

    public static  boolean delete(String key) {
        return mcc.delete(key);
    }

    public static  Object get(String key){
        return mcc.get(key);
    }

}
