#include "Log2DBServantImp.h"
#include "servant/Application.h"
#include "globe.h"
#include "LogComm.h"
#include "Log2DBServer.h"
//#include "DBOperator.h"

//
using namespace std;

//////////////////////////////////////////////////////
void Log2DBServantImp::initialize()
{
    //initialize servant here:
    //...
}

//////////////////////////////////////////////////////
void Log2DBServantImp::destroy()
{
    //destroy servant here:
    //...
}

/*
* 处理请求接口
* @param [in] reqBuf: 协议请求包(tars编码)
* @param [in] extraInfo: 额外参数
* @param [out] rspBuf: 协议响应包(tars编码)
* @return 0 - 成功, 其它异常
*/
tars::Int32 Log2DBServantImp::doRequest(const vector<tars::Char> &reqBuf, const map<std::string, std::string> &extraInfo, vector<tars::Char> &rspBuf, tars::TarsCurrentPtr current)
{
    FUNC_ENTRY("");

    int iRet = 0;

    ROLLLOG_DEBUG << "Rev doRequest,  reqBuf:" << reqBuf.size() << endl;

    THttpPackage thttpPack;
    THttpPackage thttpPackRsp;

    TLog2DBMsg stLog2DBMsgReq;
    TLog2DBMsg stLog2DBMsgRsp;

    if (reqBuf.empty())
    {
        iRet = -1;
        return iRet;
    }

    __TRY__
    if (!reqBuf.empty())
    {
        toObj(reqBuf, thttpPack);
    }
    __CATCH__

    if (thttpPack.vecData.empty())
    {
        iRet = -2;
        return iRet;
    }

    __TRY__
    if (!thttpPack.vecData.empty())
    {
        toObj(thttpPack.vecData, stLog2DBMsgReq);
    }
    __CATCH__


    ////dispatch
    switch(stLog2DBMsgReq.sMsgID)
    {
    case LOG_2_DB: //日志入库
        iRet = ProcessOnLog2DB(thttpPack, stLog2DBMsgReq, stLog2DBMsgRsp);
        ROLLLOG_DEBUG << "ProcessOnLog2DB, iRet: " << iRet << endl;
        break;
    case QUERY_DB: //查数据库
        iRet = ProcessQueryDB(thttpPack, stLog2DBMsgReq, stLog2DBMsgRsp);
        ROLLLOG_DEBUG << "ProcessQueryDB, iRet: " << iRet << endl;
        break;
    default: //异常消息
        ROLLLOG_ERROR << "Invalid msg id." << endl;
        //stLog2DBRsp.iResult = -1;
        break;
    }

    // /////回包
    // thttpPackRsp.stUid.uid = thttpPack.stUid.uid;
    // thttpPackRsp.stUid.sOpenId = thttpPack.stUid.sOpenId;
    // thttpPackRsp.stUid.sToken = thttpPack.stUid.sToken;

    // thttpPackRsp.iVer = thttpPack.iVer;
    // thttpPackRsp.iSeq = thttpPack.iSeq;
    // thttpPackRsp.nCmd = thttpPack.nCmd;
    // thttpPackRsp.nGameId = thttpPack.nGameId;

    // thttpPackRsp.extHead.platform = thttpPack.extHead.platform;
    // thttpPackRsp.extHead.channel = thttpPack.extHead.channel;
    // thttpPackRsp.extHead.scrRes = thttpPack.extHead.scrRes;
    // thttpPackRsp.extHead.uuid = thttpPack.extHead.uuid;

    // thttpPackRsp.vecSkey = thttpPack.vecSkey;

    // ///
    // tobuffer(stLog2DBMsgRsp, thttpPackRsp.vecData);

    // //
    // tobuffer(thttpPackRsp, rspBuf);

    FUNC_EXIT("", iRet);
    return iRet;
}

// //加载配置
// tars::Int32 Log2DBServantImp::reloadConfig(const std::string& cmd, std::string& des, tars::TarsCurrentPtr current)
// {
//  FUNC_ENTRY("");

//  int iRet = 0;

//  __TRY__

//  //
//  bool flag = g_app.getOuterFactoryPtr()->reloadConfig(cmd, des);
//  if (flag != true)
//  {
//      iRet = -1;
//      FDLOG_RELOAD_CMD_INFO << "|" << cmd << "|" << des << "|" << iRet << "|" << endl;
//      return iRet;
//  }

//  //
//  flag = g_app.reloadConfig(cmd, des);
//  if (flag != true)
//  {
//      iRet = -2;
//      FDLOG_RELOAD_CMD_INFO << "|" << cmd << "|" << des << "|" << iRet << "|" << endl;
//      return iRet;
//  }

//  __CATCH__

//  FDLOG_RELOAD_CMD_INFO << "|" << cmd << "|" << des << "|" << iRet << "|" << endl;

//  FUNC_EXIT("", iRet);

//  return iRet;
// }

//处理日志入库
int Log2DBServantImp::ProcessOnLog2DB(THttpPackage &thttpPack, TLog2DBMsg &stLog2DBMsgReq, TLog2DBMsg &stLog2DBMsgRsp)
{
    FUNC_ENTRY("");

    int iRet = 0;

    TLog2DBReq stLog2DBReq;
    TLog2DBRsp stLog2DBRsp;

    __TRY__
    if (!stLog2DBMsgReq.vecData.empty())
    {
        toObj(stLog2DBMsgReq.vecData, stLog2DBReq);
    }
    __CATCH__

    //
    CDBOperator *dbptr = NULL;

    //查找db操作对象
    iRet = g_app.getDBOperatorByIndex(stLog2DBReq.sLogType, dbptr);
    if (iRet != 0)
    {
        iRet = -1;
        ROLLLOG_ERROR << "getDBOperatorByIndex error, iRet: " << iRet << endl;
        goto end;
    }

    //db对象是否为空
    if (dbptr == NULL)
    {
        iRet = -2;
        ROLLLOG_ERROR << "dbptr is null, iRet: " << iRet << endl;
        goto end;
    }

    //入库
    //iRet = dbptr->log(stLog2DBReq.vecLogData);
    iRet = OnLog2DB(thttpPack, dbptr, stLog2DBReq);
    if (iRet != 0)
    {
        iRet = -3;
        ROLLLOG_ERROR << "OnLog2DB error, iRet: " << iRet << endl;
        goto end;
    }

end:
    stLog2DBRsp.iResult = iRet;

    //日志

    //返回结构
    stLog2DBMsgRsp.sMsgID = LOG_2_DB;

    __TRY__
    tobuffer(stLog2DBRsp, stLog2DBMsgRsp.vecData);
    __CATCH__

    FUNC_EXIT("", iRet);

    return iRet;
}

//处理日志查询
int Log2DBServantImp::ProcessQueryDB(THttpPackage &thttpPack, TLog2DBMsg &stLog2DBMsgReq, TLog2DBMsg &stLog2DBMsgRsp)
{
    FUNC_ENTRY("");

    int iRet = 0;

    TQueryDBReq tQueryDBReq;
    TQueryDBRsp tQueryDBRsp;


    __TRY__
    if (!stLog2DBMsgReq.vecData.empty())
    {
        toObj(stLog2DBMsgReq.vecData, tQueryDBReq);
    }
    __CATCH__

    CDBOperator *dbptr = NULL;

    //查找db操作对象
    iRet = g_app.getDBOperatorByIndex(tQueryDBReq.sLogType, dbptr);
    if (iRet != 0)
    {
        iRet = -1;
        ROLLLOG_ERROR << "getDBOperatorByIndex error, iRet: " << iRet << endl;
        goto end;
    }

    //db对象是否存在
    if (dbptr == NULL)
    {
        iRet = -2;
        ROLLLOG_ERROR << "dbptr is null, iRet: " << iRet << endl;
        goto end;
    }

    //查询
    iRet = dbptr->queryDB(tQueryDBReq.szCondition, tQueryDBRsp.vecData);
    if (iRet != 0)
    {
        iRet = -3;
        ROLLLOG_ERROR << "queryDB error, iRet: " << iRet << endl;
        goto end;
    }

end:
    tQueryDBRsp.iResult = iRet;

    //日志
    FDLOG_QUERY_DB_BILL << thttpPack.stUid.lUid << "|" << tQueryDBReq.sLogType << "|" << tQueryDBReq.szCondition << "|" << tQueryDBRsp.iResult << "|" << endl;

    //返回结构
    stLog2DBMsgRsp.sMsgID = QUERY_DB;

    __TRY__
    tobuffer(tQueryDBRsp, stLog2DBMsgRsp.vecData);
    __CATCH__

    FUNC_EXIT("", iRet);

    return iRet;
}

//逐条日志入库
int Log2DBServantImp::OnLog2DB(THttpPackage &thttpPack, CDBOperator *dbptr, const TLog2DBReq &stLog2DBReq)
{
    FUNC_ENTRY("");

    int iRet = 0;
    bool bflag = false;

    //
    if (dbptr == NULL)
    {
        ROLLLOG_ERROR << "Invalid db operator." << endl;
        return -1;
    }

    //入库
    auto it = stLog2DBReq.vecLogData.begin();
    for (; it != stLog2DBReq.vecLogData.end(); ++it)
    {
        iRet = dbptr->log(*it);
        if (iRet != 0)
        {
            //记录未入库日志
            //FDLOG_LOG_2_DB_FAIL_BILL << iRet << "|" << logJce(stLog2DBReq) << endl;
            //FDLOG_LOG_2_DB_FAIL_BILL << thttpPack.stUid.uid << "|" << iRet << "|" << stLog2DBReq.sLogType << "|" << stLog2DBReq.vecLogData.size() << "|" << endl;

            //#if 0
            ostringstream os;
            os << thttpPack.stUid.lUid << "|" << iRet << "|" << stLog2DBReq.sLogType << "|" << stLog2DBReq.vecLogData.size() << "|";
            for (auto itData = it->begin(); itData != it->end(); ++itData)
            {
                os << *itData << ";";
            }

            FDLOG_LOG_2_DB_FAIL_BILL << os.str() << endl;
            //#endif
        }
        else
        {
            //FDLOG_LOG_2_DB_SUCCESS_BILL << iRet << "|" << logJce(stLog2DBReq) << endl;
            //FDLOG_LOG_2_DB_SUCCESS_BILL << thttpPack.stUid.uid << "|" << iRet << "|" << stLog2DBReq.sLogType << "|" << stLog2DBReq.vecLogData.size() << "|" << endl;

            ostringstream os;
            os << thttpPack.stUid.lUid << "|" << iRet << "|" << stLog2DBReq.sLogType << "|" << stLog2DBReq.vecLogData.size() << "|";
            for (auto itData = it->begin(); itData != it->end(); ++itData)
            {
                os << *itData << ";";
            }

            FDLOG_LOG_2_DB_SUCCESS_BILL << os.str() << endl;
            bflag = true;
        }
    }

    /**
    *有一条记录成功的，都返回成功
    *未成功的记录日志，后面导入
    */
    if (bflag)
        iRet = 0;
    else
        iRet = -2;

    FUNC_EXIT("", iRet);
    return iRet;
}

//日志入库
tars::Int32 Log2DBServantImp::log(tars::Int32 id, const std::string &buffer, tars::TarsCurrentPtr current)
{
    FUNC_ENTRY("");
    int iRet = 0;

    CDBOperator *dbptr = NULL;

    //查找db操作对象
    iRet = g_app.getDBOperatorByIndex(id, dbptr);
    if (iRet != 0)
    {
        iRet = -1;
        goto end;
    }

    //db对象是否存在
    if (dbptr == NULL)
    {
        iRet = -2;
        goto end;
    }

    //入库
    iRet = dbptr->log(id, buffer);
    if (iRet != 0)
    {
        iRet = -3;
        goto end;
    }

end:
    FUNC_EXIT("", iRet);
    return iRet;
}


//日志入库
tars::Int32 Log2DBServantImp::log2db(const DaqiGame::TLog2DBReq &tLog2DBReq, DaqiGame::TLog2DBRsp &tLog2DBRsp, tars::TarsCurrentPtr current)
{
    FUNC_ENTRY("");

    int iRet = 0;

    THttpPackage thttpPack;

    CDBOperator *dbptr = NULL;

    //查找db操作对象
    iRet = g_app.getDBOperatorByIndex(tLog2DBReq.sLogType, dbptr);
    if (iRet != 0)
    {
        iRet = -1;
        tLog2DBRsp.iResult = -1;
        goto end;
    }

    //db对象是否存在
    if (dbptr == NULL)
    {
        iRet = -2;
        tLog2DBRsp.iResult = -2;
        goto end;
    }

    //入库
    //iRet = dbptr->log(tLog2DBReq.vecLogData);
    thttpPack.stUid.lUid = 0;
    iRet = OnLog2DB(thttpPack, dbptr, tLog2DBReq);
    if (iRet != 0)
    {
        iRet = -3;
        tLog2DBRsp.iResult = -3;
        goto end;
    }

end:

    //日志

    FUNC_EXIT("", iRet);

    return iRet;
}

//日志查询
tars::Int32 Log2DBServantImp::query_db(const DaqiGame::TQueryDBReq &tQueryDBReq, DaqiGame::TQueryDBRsp &tQueryDBRsp, tars::TarsCurrentPtr current)
{
    FUNC_ENTRY("");

    int iRet = 0;

    CDBOperator *dbptr = NULL;

    //查找db操作对象
    iRet = g_app.getDBOperatorByIndex(tQueryDBReq.sLogType, dbptr);
    if (iRet != 0)
    {
        iRet = -1;
        tQueryDBRsp.iResult = -1;
        goto end;
    }

    //db对象是否存在
    if (dbptr == NULL)
    {
        iRet = -2;
        tQueryDBRsp.iResult = -2;
        goto end;
    }

    //查询
    iRet = dbptr->queryDB(tQueryDBReq.szCondition, tQueryDBRsp.vecData);
    if (iRet != 0)
    {
        iRet = -3;
        tQueryDBRsp.iResult = -3;
        goto end;
    }

end:
    //日志
    FDLOG_QUERY_DB_BILL << 0 << "|" << tQueryDBReq.sLogType << "|" << tQueryDBReq.szCondition << "|" << tQueryDBRsp.iResult << "|" << endl;

    FUNC_EXIT("", iRet);

    return iRet;
}



