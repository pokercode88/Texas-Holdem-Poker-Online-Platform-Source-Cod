#include <sstream>
#include <util/tc_mysql.h>
#include <string.h>
#include "OuterFactoryImp.h"
#include "LogComm.h"
#include "Log2DBServer.h"

//
using namespace wbl;

/**
 *
*/
OuterFactoryImp::OuterFactoryImp(): _pFileConf(NULL)
{
    createAllObject();
}

/**
 *
*/
OuterFactoryImp::~OuterFactoryImp()
{
    deleteAllObject();
}

void OuterFactoryImp::deleteAllObject()
{
    if (_pFileConf)
    {
        delete _pFileConf;
        _pFileConf = NULL;
    }
}

void OuterFactoryImp::createAllObject()
{
    try
    {
        //deleteAllObject();

        //拉取远程配置
        g_app.addConfig(ServerConfig::ServerName + ".conf");

        //本地配置文件
        _pFileConf = new tars::TC_Config();
        _pFileConf->parseFile(ServerConfig::BasePath + ServerConfig::ServerName + ".conf");
        LOG_DEBUG << "init config file succ:" << ServerConfig::BasePath + ServerConfig::ServerName + ".conf" << endl;

        //taf代理Factory,访问其他taf接口时使用
        _pProxyFactory = new OuterProxyFactory();
        LOG_DEBUG << "init proxy factory succ." << endl;

        //读取所有配置
        load();
    }
    catch (TC_Exception &ex)
    {
        LOG->error() << ex.what() << endl;
        throw;
    }
    catch (exception &e)
    {
        LOG->error() << e.what() << endl;
        throw;
    }
    catch (...)
    {
        LOG->error() << "unknown exception." << endl;
        throw;
    }

    return;
}

//加载配置
bool OuterFactoryImp::reloadConfig()
{
    _pFileConf->parseFile(ServerConfig::BasePath + ServerConfig::ServerName + ".conf");
    LOG_DEBUG << "init config file succ:" << ServerConfig::BasePath + ServerConfig::ServerName + ".conf" << endl;

    //读取所有配置
    load();
    return true;
}

bool OuterFactoryImp::reloadConfig(const std::string &cmd, std::string &des)
{
    des = "";

    //参数错误
    if(cmd.length() == 0)
    {
        des = "parameter error, cmd is empty.";
        return false;
    }

    //
    if(cmd.compare("reload") == 0)
    {
        reloadConfig();
        des = "reload config success.";
    }
    else
    {
        des = "cmd not exists.";
        return false;
    }

    return true;
}

//读取所有配置
void OuterFactoryImp::load()
{
    WriteLocker lock(m_rwlock);

    ///db配置
    readDBConfig();
    printDBConfig();

    //log配置
    readLogConfig();
    printLogConfig();
}

//db配置
void OuterFactoryImp::readDBConfig()
{
    dbConfig.Domain = (*_pFileConf).get("/" + ServerConfig::ServerName + "/DB<domain>");
    dbConfig.strDBHost = (*_pFileConf).get("/" + ServerConfig::ServerName + "/DB<Host>");
    dbConfig.strDBUser = (*_pFileConf).get("/" + ServerConfig::ServerName + "/DB<User>");
    dbConfig.strDBPwd = (*_pFileConf).get("/" + ServerConfig::ServerName + "/DB<Pwd>");
    dbConfig.strDBName = (*_pFileConf).get("/" + ServerConfig::ServerName + "/DB<Name>");
    dbConfig.strDBCharSet = (*_pFileConf).get("/" + ServerConfig::ServerName + "/DB<CharSet>");
    dbConfig.iDBPort = TC_Common::strto<int>((*_pFileConf).get("/" + ServerConfig::ServerName + "/DB<Port>"), "3306");

    //域名
    if(dbConfig.Domain.length() > 0)
    {
        string szHost = getIpStr(dbConfig.Domain);
        if(szHost.length() > 0)
        {
            dbConfig.strDBHost = szHost;

            ROLLLOG_DEBUG << "get host by domain, Domain: " << dbConfig.Domain << ", szHost: " << szHost << endl;
        }
    }
}

//打印db配置
void OuterFactoryImp::printDBConfig()
{
    FDLOG_DB_CONFIG_INFO << "Get DB config, Domain: " << dbConfig.Domain
                         << ", Host: " << dbConfig.strDBHost
                         << ", User: " << dbConfig.strDBUser
                         << ", Password: " << dbConfig.strDBPwd
                         << ", Name: " << dbConfig.strDBName
                         << ", CharSet: " << dbConfig.strDBCharSet
                         << ", Port: " << dbConfig.iDBPort
                         << endl;
}

//取db配置
DBConfig &OuterFactoryImp::getDBConfig()
{
    wbl::ReadLocker lock(m_rwlock);

    return dbConfig;
}

//log配置
void OuterFactoryImp::readLogConfig()
{
    const vector<string> &vDomain = (*_pFileConf).getDomainVector("/" + ServerConfig::ServerName + "/logconfig");
    for(size_t i = 0; i < vDomain.size(); i++)
    {
        TLogConfig tLogConfig;

        //表名
        tLogConfig.tablename = (*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/table/<name>");

        //db配置
        tLogConfig.dbConfig.Domain = (*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/DB<domain>");
        tLogConfig.dbConfig.strDBHost = (*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/DB<Host>");
        tLogConfig.dbConfig.strDBUser = (*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/DB<User>");
        tLogConfig.dbConfig.strDBPwd = (*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/DB<Pwd>");
        tLogConfig.dbConfig.strDBName = (*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/DB<Name>");
        tLogConfig.dbConfig.strDBCharSet = (*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/DB<CharSet>");
        tLogConfig.dbConfig.iDBPort = TC_Common::strto<int>((*_pFileConf).get("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/DB<Port>"), "3306");
        //域名
        if(tLogConfig.dbConfig.Domain.length() > 0)
        {
            string szHost = getIpStr(tLogConfig.dbConfig.Domain);
            if(szHost.length() > 0)
            {
                tLogConfig.dbConfig.strDBHost = szHost;

                ROLLLOG_DEBUG << "get host by domain, Domain: " << tLogConfig.dbConfig.Domain << ", szHost: " << szHost << endl;
            }
        }

        //字段
        vector<TLogConfigItem> vecConfigItems;
        vecConfigItems.clear();

        const vector<string> vDomainLine = (*_pFileConf).getDomainLine("/" + ServerConfig::ServerName + "/logconfig/" + vDomain[i] + "/field");
        for(size_t j = 0; j < vDomainLine.size(); j++)
        {
            vector<string> vecItem;
            split_string(vDomainLine[j], "[ \t]*\\|[ \t]*", vecItem);

            //键值对
            if(vecItem.size() != 2)
            {
                continue;
            }

            //单个配置
            TLogConfigItem tLogConfigItem;
            tLogConfigItem.field = vecItem[0];
            if(strcmp(vecItem[1].c_str(), "DB_INT") == 0)
            {
                tLogConfigItem.type = TC_Mysql::DB_INT;
            }
            else if(strcmp(vecItem[1].c_str(), "DB_STR") == 0)
            {
                tLogConfigItem.type = TC_Mysql::DB_STR;
            }
            else
            {
                tLogConfigItem.type = 100;
            }

            //保存
            vecConfigItems.push_back(tLogConfigItem);
        }

        //赋值
        tLogConfig.vecLogConfigItems = vecConfigItems;
        mapLogConfig[TC_Common::strto<int>(vDomain[i])] = tLogConfig;
    }
}

//打印log配置
void OuterFactoryImp::printLogConfig()
{
    ostringstream os;

    os << endl;

    map<int, TLogConfig>::iterator it = mapLogConfig.begin();
    for(; it != mapLogConfig.end(); it++)
    {
        os << "id: " << it->first << endl;
        os << "table name: " << it->second.tablename << endl;

        //数据源
        os << "Get DB config, Domain: " << it->second.dbConfig.Domain
           << ", Host: " << it->second.dbConfig.strDBHost
           << ", User: " << it->second.dbConfig.strDBUser
           << ", Password: " << it->second.dbConfig.strDBPwd
           << ", Name: " << it->second.dbConfig.strDBName
           << ", CharSet: " << it->second.dbConfig.strDBCharSet
           << ", Port: " << it->second.dbConfig.iDBPort
           << endl;

        //字段
        for(vector<TLogConfigItem>::iterator itItem = it->second.vecLogConfigItems.begin();
                itItem != it->second.vecLogConfigItems.end();
                itItem++)
        {
            os << "field: " << itItem->field << ", type: " << itItem->type << endl;
        }
    }

    //
    FDLOG_LOG_CONFIG_INFO << os.str() << endl;
}

//取log配置
const map<int, TLogConfig> &OuterFactoryImp::getLogConfig()
{
    wbl::ReadLocker lock(m_rwlock);

    return mapLogConfig;
}

//根据序号取log配置
int OuterFactoryImp::getLogConfigByID(int id, TLogConfig &tLogConfig)
{
    wbl::ReadLocker lock(m_rwlock);

    int iRet = 0;

    map<int, TLogConfig>::iterator it = mapLogConfig.find(id);
    if(it != mapLogConfig.end())
    {
        tLogConfig = it->second;
    }
    else
    {
        return -1;
    }

    return iRet;
}


//格式化时间
string OuterFactoryImp::GetTLogTimeFormat()
{
    string sFormat("%Y-%m-%d %H:%M:%S");
    time_t t = time(NULL);
    struct tm *pTm = localtime(&t);
    if(pTm == NULL)
    {
        return "";
    }

    char sTimeString[255] = "\0";
    strftime(sTimeString, sizeof(sTimeString), sFormat.c_str(), pTm);
    return string(sTimeString);
}

//拆分字符串成整形
int OuterFactoryImp::splitInt(string szSrc, vector<int> &vecInt)
{
    split_int(szSrc, "[ \t]*\\|[ \t]*", vecInt);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////


