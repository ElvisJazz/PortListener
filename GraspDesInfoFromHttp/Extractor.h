///////////////////////////////////////////////////////////////////////
//文件名： Extractor.h
//功  能： 启动抓包程序并对数据包进行分析，输出目标数据(病人id)
//开发者： 贾治中
//创  建：2014/4/15
//修  改：2014/4/24
//长  度：100
//备  注：暂无
///////////////////////////////////////////////////////////////////////

#ifndef _EXTRACTOR_H
#define _EXTRACTOR_H

#include <string>
#include <map>
#include <set>
#include <vector>
#include <windows.h> 
#include <shellapi.h> 
#include <tlhelp32.h>
#include <stdio.h>
using namespace std;

class CExtractor
{
public:
	// 构造和析构函数
	CExtractor();
	~CExtractor();

	// 读取日志文件函数
	bool ReadDataAndAnalyse(string path);
	// 分析
	string::size_type Analyse(string &str, string::size_type pos, long length);
	// 读取配置文件
	bool InitParameter(string path);
	// 根据key值读取ini对应值
	vector<string> ReadStrFromIni(string path, string key);
	vector<int> ReadIntFromIni(string path, string key);
	// 将字符串转换成整数
	int IntegerToStr(string numStr);
	// 设置根路径
	void SetWorkPath(string path);

	// 清空和获取id
	void ClearIdSet() { m_idMap.clear(); }
	multimap<string, string> GetIdMap() { return m_idMap; }
	// 获取待读文件名
	string GetLoggerFile();
	// 设置已读文件名
	void SetFileIntoSet(string name);
	// 设置单个日志文件大小
	void SetFileSize(long size) { m_fileSize = size; }
	// 获取抓包执行程序路径
	string GetProgramPath() { return m_programPath;}

	// 获取抓包执行程序名称
	string GetProgramName() { return m_programName;}

	// 获取日志文件前缀表达式
	string GetFilePrefix() { return m_filePrefix;	}
	// 是否清空
	bool isClearHistory() {return m_isDeleteHistory;}
	// 清空历史日志
	bool ClearHistory(string path);

	// 检查是否是非字母数字构成的合法串
	bool IsLegalStr(string str);
	// 程序进程检测函数
	//HANDLE CheckProcess(string name);

private:
	// 日志文件地址（不含文件名）
	string m_filePath;
	// 已读文件名(含路径)容器
	set<string> m_fileNameSet;
	// id容器
	multimap<string, string> m_idMap; 
	// 读文件位置指针
	unsigned long m_pos;
	// 单个日志文件默认大小
	unsigned long m_fileSize;

	// 配置参数
	string m_filePrefix;
	// 包标识
	vector<string> m_packMarkVec;
	string m_packMark;
	// 前缀标识
	vector<string> m_prefixVec;
	string m_prefix;
	// 前缀重复
	vector<int> m_repeatNumOfPrefixVec;
	int m_repeatNumOfPrefix;
	// 前缀标识后忽略字符位数
	vector<int> m_ignoreNumVec;
	int m_ignoreNum;
	// 目标字符串最大位数
	int m_maxNumOfDes;
	// 目标字符串最小位数
	int m_minNumOfDes;
	// 后缀字符串
	vector<string> m_postfixVec;
	string m_postfix;
	// 后缀前向忽略字符数锁定目标串
	vector<int> m_beforePostfixIgnoreNumVec;
	int m_beforePostfixIgnoreNum;
	// 目标串前缀到后缀、包标志到前缀总长度
	vector<int> m_allLengthFromPrefixToPostfixVec;
	vector<int> m_allLengthFromPackMarkToPrefixVec;
	int m_allLengthFromPrefixToPostfix;
	int m_allLengthFromPackMarkToPrefix;
	// 干扰串过滤
	vector<string> m_nonTargetStrVec;
	// 是否开启清空历史记录
	bool m_isDeleteHistory;
	
	// 抓包启动程序
	string m_programPath;
	// 程序名
	string m_programName;
	// 工作路径
	string m_workPath;
};

#endif