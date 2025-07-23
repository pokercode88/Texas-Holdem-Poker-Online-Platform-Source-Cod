#include "Log2DBServer.h"
#include "Log2DBServantImp.h"
#include "LogComm.h"

//
using namespace std;

//
Log2DBServer g_app;

/////////////////////////////////////////////////////////////////
void Log2DBServer::initialize()
{
    //initialize application here:
    //...

    addServant<Log2DBServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".Log2DBServantObj");

    ///
    _pOuter = new OuterFactoryImp();

    ///
    //initialDBOperator();

    string des;
    reloadConfig("reload", des);

#if 0
    //初始化DB配置,读取DB信息
    if(DBOperatorSingleton::getInstance()->init() < 0)
    {
        ROLLLOG_ERROR << "Init DBOperatorSingleton failed, exit server!" << endl;
        terminate();
        return;
    }
    else
    {
        DBOperatorSingleton::getInstance()->loadDataFromDB();
    }
#endif
}

/////////////////////////////////////////////////////////////////
void Log2DBServer::destroyApp()
{
    //destroy application here:
    //...
}

bool Log2DBServer::reloadConfig(const std::string &cmd, std::string &des)
{
    des = "";

    if (cmd.length() == 0)
    {
        des = "parameter error, cmd is empty.";
        return false;
    }

    if (cmd.compare("reload") == 0)
    {
        destroyDBOperator();
        initialDBOperator();
        des = "reload config success.";
        return true;
    }

    des = "cmd not exists.";
    return false;
}

//初始化数据库
int Log2DBServer::initialDBOperator()
{
    const map<int, TLogConfig> &mapLogConfig = _pOuter->getLogConfig();
    for (auto it = mapLogConfig.begin(); it != mapLogConfig.end(); it++)
    {
        auto dbptr = new CDBOperator()  ;
        if (dbptr)
            dbptr->init(it->second);

        mapDBOperator[it->first] = dbptr;
    }

    LOG_DEBUG << "initial db opetrator size: " << mapDBOperator.size() << endl;
    return 0;
}

//释放所有db操作对象
int Log2DBServer::destroyDBOperator()
{
    for (auto it = mapDBOperator.begin(); it != mapDBOperator.end();)
    {
        if (it->second)
            delete it->second;

        mapDBOperator.erase(it++);
    }

    return 0;
}

//根据类型查找db操作对象
int Log2DBServer::getDBOperatorByIndex(int index, CDBOperator *&dbptr)
{
    wbl::ReadLocker lock(m_rwlock);

    auto it = mapDBOperator.find(index);
    if (it != mapDBOperator.end())
    {
        dbptr = it->second;
        return 0;
    }

    return -1;
}

/////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    try
    {
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch (std::exception &e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }

    return -1;
}
/////////////////////////////////////////////////////////////////


