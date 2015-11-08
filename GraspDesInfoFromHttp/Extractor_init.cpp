///////////////////////////////////////////////////////////////////////
//文件名： Extractor_init.cpp
//功  能： 启动抓包程序初始化
//开发者： 贾治中
//创  建：2014/5/26
//修  改：2014/5/26
//长  度：371
//备  注：暂无
///////////////////////////////////////////////////////////////////////
#include "Extractor.h"
#include <io.h> 
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>

//功  能：读取配置文件
//参  数：文件路径
//返回值：是否正确获取配置数据
bool CExtractor::InitParameter(string path)
{
	vector<int> tmpIntVec;
	vector<string> tmpStrVec;
	// ----------------日志文件前缀----------------
	tmpStrVec = ReadStrFromIni(path, "filePrefix");
	if(tmpStrVec.size() != 0)
		m_filePrefix = (*tmpStrVec.begin());
	else
		return false;
	string::size_type pos = m_filePrefix.find_last_of("//");
	m_filePath = m_workPath + m_filePrefix.substr(0, pos+1);
	m_filePrefix = m_workPath + m_filePrefix;
	// ----------------包文件大小----------------
	tmpIntVec = ReadIntFromIni(path, "fileSize");
	if(tmpIntVec.size() != 0)
	{
		m_fileSize = *(tmpIntVec.begin());
	}
	else
		return false;

	// ----------------包标识 ----------------
	m_packMarkVec = ReadStrFromIni(path, "packMark");

	// ----------------前缀标识----------------
	m_prefixVec = ReadStrFromIni(path, "prefix");

	// ----------------前缀重复次数----------------
	m_repeatNumOfPrefixVec = ReadIntFromIni(path, "repeatNumOfPrefix");

	// ----------------前缀标识后忽略字符位数----------------
	m_ignoreNumVec = ReadIntFromIni(path, "ignoreNum");

	// ----------------目标字符最大串位数----------------
	tmpIntVec = ReadIntFromIni(path, "maxNumOfDes");
	if(tmpIntVec.size() != 0)
	{
		m_maxNumOfDes = *(tmpIntVec.begin());
	}
	else
		return false;
	// ----------------目标字符串最小位数----------------
	tmpIntVec = ReadIntFromIni(path, "minNumOfDes");
	if(tmpIntVec.size() != 0)
	{
		m_minNumOfDes = *(tmpIntVec.begin());
	}
	else
		return false;

	// ----------------后缀字符串----------------
	m_postfixVec = ReadStrFromIni(path, "postfix");

	// ----------------后缀前向忽略字符数锁定目标串----------------
	m_beforePostfixIgnoreNumVec = ReadIntFromIni(path, "beforePostfixIgnoreNum");

	// ----------------目标串总长----------------
	m_allLengthFromPrefixToPostfixVec = ReadIntFromIni(path, "allLengthFromPrefixToPostfix");

	m_allLengthFromPackMarkToPrefixVec = ReadIntFromIni(path, "allLengthFromPackMarkToPrefix");

	// -------------干扰串过滤----------------
	m_nonTargetStrVec = ReadStrFromIni(path, "nonTargetStr"); 

	// 是否清空历史记录
	tmpIntVec = ReadIntFromIni(path, "isDeleteHistory");
	if(tmpIntVec.size() != 0)
	{
		m_isDeleteHistory  = *(tmpIntVec.begin());
	}
	else
		return false;

	// 程序路径
	tmpStrVec = ReadStrFromIni(path, "programPath");
	if(tmpStrVec.size() != 0)
	{
		m_programPath = m_workPath + *(tmpStrVec.begin());
	}
	else
		return false;

	// 程序名称
	tmpStrVec = ReadStrFromIni(path, "programName");
	if(tmpStrVec.size() != 0)
	{
		m_programName = *(tmpStrVec.begin());
	}
	else
		return false;
	
	return true;
}
 
//功  能：根据key值读取ini对应值
//参  数：配置文件路径，键值
//返回值：读取的目标配置值(字符串)
vector<string> CExtractor::ReadStrFromIni(string path, string key)
{
	vector<string> strVec;
	strVec.clear();

	locale loc = locale::global(locale("")); //要打开的文件路径含中文，设置全局locale为本地环境     
	ifstream file_op(path.c_str(), ios::in);	
	locale::global(loc);//恢复全局locale	

    if (!file_op) {
        cout<<"Can not open file"<<endl;
        return strVec;
    }
    string line;
    while (getline(file_op, line) ) {
        int i = line.find(key, 0);//从0位开始找
        if (i==0) {//找到了key值
            int k = line.find("=", 0);//找等于号
			file_op.close();
            string valStr =  line.substr(k+1);//取等于号后边的值
			// 替换其中的十六进制数为对应字符
			while(i < valStr.length())
			{
				i = valStr.find("0x", i);
				if(i != string::npos)
				{
					int c1, c2;
					// 转换第一个字符
					if(valStr[i+2] >= 97)
						c1 = valStr[i+2] - 87;
					else
						c1 = valStr[i+2] - 48;
					// 转换第二个字符
					if(valStr[i+3] >= 97)
						c2 = valStr[i+3] - 87;
					else
						c2 = valStr[i+3] - 48;
					int val = c1*16+c2;
					valStr = valStr.replace(i, 4, 1, char(val));
					i++;
				}
				else
					break;
			}

			// 替换其中的日期标志为对应字符
			while(i < valStr.length())
			{
				i = valStr.find("%date%", i);
				if(i != string::npos)
				{
					SYSTEMTIME st;
					GetSystemTime(&st);
					int year = st.wYear;
					int month = st.wMonth;
					int day = st.wDay;
					char *buffer = new char[20];
					sprintf(buffer,"%d%d%d", year, month, day);/*赋予数值*/
					valStr = valStr.replace(i, i+6, buffer);
					i += 7;
				}
				else
					break;
			}

			// 获取以&&隔开的字符串
			if(valStr != "")
			{
				string::size_type first_pos = 0;
				string::size_type pos = valStr.find("&&");
				string subStr;
				while(pos != string::npos)
				{
					subStr = valStr.substr(first_pos, pos-first_pos);
					strVec.push_back(subStr);
					first_pos = pos + 2;
					pos = valStr.find("&&", pos+2);
				}
				// 最后一个过滤子串
				subStr = valStr.substr(first_pos, pos-first_pos);
				strVec.push_back(subStr);
			}
			return strVec;
        }
    }
	file_op.close();
}

//功  能：根据key值读取ini对应值
//参  数：配置文件路径，键值
//返回值：读取的目标配置值(整数)
vector<int> CExtractor::ReadIntFromIni(string path, string key)
{
	locale loc = locale::global(locale("")); //要打开的文件路径含中文，设置全局locale为本地环境     
	ifstream file_op(path.c_str(), ios::in);	
	locale::global(loc);//恢复全局locale	

	vector<int> numVec;
	numVec.clear();

    if (!file_op) {
        cout<<"Can not open file"<<endl;
        return numVec;
    }
    string line;
    while (getline(file_op, line) )
	{
        int i = line.find(key, 0);//从0位开始找
        if(i==0) //找到了key值
		{
            int k = line.find("=", 0);//找等于号
            string valStr = line.substr(k+1);//取等于号后边的值

			// 获取以空格隔开的所有整数
			string::size_type first_pos = 0;
			string::size_type pos = valStr.find("&&");
			string subStr;
			while(pos != string::npos)
			{
				subStr = valStr.substr(first_pos, pos-first_pos);
				numVec.push_back(IntegerToStr(subStr));
				first_pos = pos + 2;
				pos = valStr.find("&&", pos+2);
			}
			// 最后一个整数子串
			subStr = valStr.substr(first_pos, pos-first_pos);
			numVec.push_back(IntegerToStr(subStr));
			
			break;
        }
    }
	file_op.close();
	return numVec;
}


//功  能：将字符串转换成整数
//参  数：字符串
//返回值：整数
int CExtractor::IntegerToStr(string numStr)
{
	int num;
	stringstream ss;
	ss.clear();
	ss<<numStr;
	ss>>num;
	return num;
}