#ifndef _Log2DBServer_H_
#define _Log2DBServer_H_

#include <iostream>
#include "servant/Application.h"
#include "OuterFactoryImp.h"
#include "DBOperator.h"

//
using namespace tars;

/**
 *
 **/
class Log2DBServer : public Application
{
public:
    /**
     *
     **/
    virtual ~Log2DBServer() {};

    /**
     *
     **/
    virtual void initialize();

    /**
     *
     **/
    virtual void destroyApp();

public:
    OuterFactoryImpPtr getOuterFactoryPtr()
    {
        return _pOuter;
    }

public:
    //加载配置
    bool reloadConfig(const std::string &cmd, std::string &des);

public:
    //初始化数据库
    int initialDBOperator();
    //释放所有db操作对象
    int destroyDBOperator();
    //根据类型查找db操作对象
    int getDBOperatorByIndex(int index, CDBOperator *&dbptr);

private:
    //
    OuterFactoryImpPtr _pOuter;

public:
    //
    map<int, CDBOperator *> mapDBOperator;

private:
    //
    wbl::ReadWriteLocker m_rwlock;
};

//
extern Log2DBServer g_app;

////////////////////////////////////////////
#endif


