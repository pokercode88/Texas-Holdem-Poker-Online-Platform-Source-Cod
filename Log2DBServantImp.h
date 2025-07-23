#ifndef _Log2DBServantImp_H_
#define _Log2DBServantImp_H_

//
#include "servant/Application.h"
#include "Log2DBServant.h"
#include "Log2DBProto.h"
#include "JFGameHttpProto.h"
#include "DBOperator.h"

//
using namespace DaqiGame;
using namespace JFGameHttpProto;

/**
 *
 *
 */
class Log2DBServantImp : public DaqiGame::Log2DBServant
{
public:
    /**
     *
     */
    virtual ~Log2DBServantImp() {}

    /**
     *
     */
    virtual void initialize();

    /**
     *
     */
    virtual void destroy();

public:
    /*
    * 处理请求接口
    * @param [in] reqBuf: 协议请求包(JCE编码)
    * @param [in] extraInfo: 额外参数
    * @param [out] rspBuf: 协议响应包(JCE编码)
    * @return 0 - 成功, 其它异常
    */
    virtual tars::Int32 doRequest(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf, tars::TarsCurrentPtr current);
    //加载配置
    // virtual tars::Int32 reloadConfig(const std::string& cmd, std::string&des, tars::TarsCurrentPtr current);
    //日志入库
    virtual tars::Int32 log(tars::Int32 id, const std::string &buffer, tars::TarsCurrentPtr current);
    //日志入库
    virtual tars::Int32 log2db(const DaqiGame::TLog2DBReq &tLog2DBReq, DaqiGame::TLog2DBRsp &tLog2DBRsp, tars::TarsCurrentPtr current);
    //日志查询
    virtual tars::Int32 query_db(const DaqiGame::TQueryDBReq &tQueryDBReq, DaqiGame::TQueryDBRsp &tQueryDBRsp, tars::TarsCurrentPtr current);

private:
    //处理日志入库
    int ProcessOnLog2DB(THttpPackage &thttpPack, TLog2DBMsg &stLog2DBMsgReq, TLog2DBMsg &stLog2DBMsgRsp);

    //逐条日志入库
    int OnLog2DB(THttpPackage &thttpPack, CDBOperator *dbptr, const TLog2DBReq &stLog2DBReq);

    //处理日志查询
    int ProcessQueryDB(THttpPackage &thttpPack, TLog2DBMsg &stLog2DBMsgReq, TLog2DBMsg &stLog2DBMsgRsp);
};
/////////////////////////////////////////////////////
#endif

