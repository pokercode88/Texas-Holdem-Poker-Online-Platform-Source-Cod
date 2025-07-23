#ifndef __LOGCOMM_H__
#define __LOGCOMM_H__

//
#include <util/tc_logger.h>
#include <sstream>
#include <vector>
#include "servant/RemoteLogger.h"

//
using namespace tars;

//
#define ROLLLOG(level) (LOG->level() << "[" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "] ")
#define ROLLLOG_DEBUG (ROLLLOG(debug))
#define ROLLLOG_INFO (ROLLLOG(info))
#define ROLLLOG_WARN (ROLLLOG(warn))
#define ROLLLOG_ERROR (ROLLLOG(error))

#define FUNC_ENTRY(in) (ROLLLOG(debug) << ">>>> Enter " << __FUNCTION__ << "() in(" << in << ")" << endl)
#define FUNC_EXIT(out, ret) (ROLLLOG(debug) << "<<<< Exit " << __FUNCTION__ << "() out[" << out << "], ret = " << ret << endl)

#define FDLOG_ERROR (FDLOG("error") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")
#define FDLOG_EXCEPT (FDLOG("except") << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "|")

//解码出错日志
#define FDLOG_DECODE_ERR (FDLOG("decode_error") << "|")

//reload命令日志
#define FDLOG_RELOAD_CMD_INFO (FDLOG("reload_cmd_info") << "|") 

//db配置日志
#define FDLOG_DB_CONFIG_INFO (FDLOG("db_config_info") << "|") 

//log配置日志
#define FDLOG_LOG_CONFIG_INFO (FDLOG("log_config_info") << "|") 

//入库成功账单
#define FDLOG_LOG_2_DB_SUCCESS_BILL (FDLOG("log_2_db_success_bill") << "|")

//入库失败账单
#define FDLOG_LOG_2_DB_FAIL_BILL (FDLOG("log_2_db_fail_bill") << "|")

//查询账单
#define FDLOG_QUERY_DB_BILL (FDLOG("query_db_bill") << "|")

#endif

