#ifndef _OUTER_FACTORY_IMP_H_
#define _OUTER_FACTORY_IMP_H_

#include <string>
#include <map>

#include "globe.h"
#include "servant/Application.h"
#include "OuterFactory.h"

//wbl
#include <wbl/regex_util.h>
#include <wbl/stream_util.h>

//db config
typedef struct _TDBConfig
{
    string Domain;       //域名
    string strDBHost;    //主机IP
    string strDBUser;    //DB用户名
    string strDBPwd;     //DB密码
    string strDBName;    //DB名字
    string strDBCharSet; //表的字符集
    int iDBPort;         //DB监听端口
} DBConfig;

//log config
typedef struct _TLogConfigItem
{
    string field;   //字段名
    int type;       //字段类型
} TLogConfigItem;

//日志配置
typedef struct _TLogConfig
{
    DBConfig dbConfig;  //数据库配置
    string tablename;   //表名
    vector<TLogConfigItem> vecLogConfigItems;  //字段
} TLogConfig;


//
class OuterFactoryImp;
typedef TC_AutoPtr<OuterFactoryImp> OuterFactoryImpPtr;

//
// using namespace DaqiGame;

/**
 * 外部工具接口对象工厂
 */
class OuterFactoryImp : public OuterFactory
{
private:
    /**
     *
    */
    OuterFactoryImp();

    /**
     *
    */
    ~OuterFactoryImp();

    //
    friend class Log2DBServantImp;
    friend class Log2DBServer;

public:
    //框架中用到的outer接口(不能修改):
    const OuterProxyFactoryPtr &getProxyFactory() const
    {
        return _pProxyFactory;
    }

    tars::TC_Config &getConfig() const
    {
        return *_pFileConf;
    }

public:
    //加载配置
    bool reloadConfig();
    //
    bool reloadConfig(const std::string &cmd, std::string &des);
    //读取所有配置
    void load();
    //db配置
    void readDBConfig();
    //打印db配置
    void printDBConfig();
    //取db配置
    DBConfig &getDBConfig();
    //log配置
    void readLogConfig();
    //打印log配置
    void printLogConfig();
    //取log配置
    const map<int, TLogConfig> &getLogConfig();
    //根据序号取log配置
    int getLogConfigByID(int id, TLogConfig &tLogConfig);

private:
    //
    void createAllObject();
    //
    void deleteAllObject();

public:
    //格式化时间
    string GetTLogTimeFormat();

private:
    //拆分字符串成整形
    int splitInt(string szSrc, vector<int> &vecInt);

private:
    //
    wbl::ReadWriteLocker m_rwlock;
    //框架用到的共享对象(不能修改):
    tars::TC_Config *_pFileConf;
    //
    OuterProxyFactoryPtr _pProxyFactory;
    //db config
    DBConfig dbConfig;

public:
    //日志配置
    map<int, TLogConfig> mapLogConfig;
};

////////////////////////////////////////////////////////////////////////////////
#endif


