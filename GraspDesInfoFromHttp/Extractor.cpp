///////////////////////////////////////////////////////////////////////
//文件名： Extractor.cpp
//功  能： 启动抓包程序并对数据包进行分析，输出目标数据(病人id)
//开发者： 贾治中
//创  建：2014/4/15
//修  改：2014/4/24
//长  度：371
//备  注：暂无
///////////////////////////////////////////////////////////////////////
#include "Extractor.h"
#include <io.h> 
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>

//功  能：构造函数
//参  数：无
//返回值：无
CExtractor::CExtractor()
{
	m_pos = 0;
}

//功  能：析构函数
//参  数：无
//返回值：无
CExtractor::~CExtractor()
{

}

//功  能：读取日志文件函数
//参  数：文件路径
//返回值：此文件是否读完，读取下个文件
bool CExtractor::ReadDataAndAnalyse(string path)
{
	locale loc = locale::global(locale("")); //要打开的文件路径含中文，设置全局locale为本地环境     
	ifstream in(path.c_str(), ios::binary);	
	locale::global(loc);//恢复全局locale
	if (!in) {
        cout<<"Can not open file"<<endl;
        return false;
    }

	string fileContent = "";
	string::size_type pos = 0;

	in.seekg(0, ios::end);
	long length = in.tellg(); // 文件总大小

	in.seekg(m_pos, ios::beg); // 转到上次读到的位置
	
	long len = length - in.tellg(); // 获取本次读取的数据段长度
	in.seekg(m_pos+1);

	fileContent.resize(len); // 设置缓冲区大小
	in.read((char*)&fileContent[0], len); // 读取指定位置和段长数据
	in.close(); // 关闭文件输入流
    bool isFind = false; //本次读入是否找到目标串
	string::size_type copyPos = 0; // 文件内容当前读取指针位置
	string::size_type firstPackMarkPos = 0; // 第一个包标志所在位置

	// 对于多个搜索标志位，逐个循环搜索，有一个找到则跳出循环
	vector<string>::iterator packMarkIter = m_packMarkVec.begin();	
	vector<string>::iterator prefixIter = m_prefixVec.begin();
	vector<int>::iterator repeatNumOfPrefixIter = m_repeatNumOfPrefixVec.begin();
	vector<int>::iterator ignoreNumIter = m_ignoreNumVec.begin();
	vector<string>::iterator postfixIter = m_postfixVec.begin();
	vector<int>::iterator beforePostfixIgnoreNumIter = m_beforePostfixIgnoreNumVec.begin();
	vector<int>::iterator allLengthFromPrefixToPostfixIter = m_allLengthFromPrefixToPostfixVec.begin();
	vector<int>::iterator allLengthFromPackMarkToPrefixIter = m_allLengthFromPackMarkToPrefixVec.begin();
	// 多值搜索时备份pos搜索位指针
	string::size_type copyPosForResearch = pos;
	// 读取包的偏移量是否以上一个包算
	string::size_type isUseLastPack;

	while(packMarkIter!=m_packMarkVec.end())
	{
		pos = copyPosForResearch;
		 
		// 各个搜索参数赋值
		m_packMark = *packMarkIter; // 包标识

		if(prefixIter!=m_prefixVec.end() && ignoreNumIter!=m_ignoreNumVec.end() && postfixIter!=m_postfixVec.end() && beforePostfixIgnoreNumIter!=m_beforePostfixIgnoreNumVec.end()
			&& allLengthFromPrefixToPostfixIter!=m_allLengthFromPrefixToPostfixVec.end() && allLengthFromPackMarkToPrefixIter!=m_allLengthFromPackMarkToPrefixVec.end() && repeatNumOfPrefixIter!=m_repeatNumOfPrefixVec.end())
		{
			m_prefix = *prefixIter; // 前缀
			m_repeatNumOfPrefix = *repeatNumOfPrefixIter; // 前缀重复次数
			m_ignoreNum = *ignoreNumIter; // 前缀到目标串可忽略字符数
			m_postfix = *postfixIter; // 后缀
			m_beforePostfixIgnoreNum = *beforePostfixIgnoreNumIter; // 目标串末尾至后缀可忽略字符数
			m_allLengthFromPrefixToPostfix = *allLengthFromPrefixToPostfixIter; // 前缀到后缀总长度最大值
			m_allLengthFromPackMarkToPrefix = *allLengthFromPackMarkToPrefixIter; // 包标志到前缀总长度最大值
			// 迭代器自增
			packMarkIter++;
			prefixIter++;
			ignoreNumIter++;
			postfixIter++;
			beforePostfixIgnoreNumIter++;
			allLengthFromPrefixToPostfixIter++;
			allLengthFromPackMarkToPrefixIter++;
			repeatNumOfPrefixIter++;
		} 
		else
		{
			break;
		}
		
		while(pos<len && pos!=string::npos)
		{
			pos = fileContent.find(m_packMark, pos);			
			isUseLastPack = true;

			if(pos != string::npos)
			{
				// 更新包标志位置为最靠后的位指针
				if(pos > firstPackMarkPos)
				{
					firstPackMarkPos = pos;
					isUseLastPack = false;
				}

				pos += m_packMark.length();

				// 此处的pos是否为最靠后的包位置所在
				if(!isUseLastPack)
					copyPos = pos;
				else
					copyPos = firstPackMarkPos+1;

				pos = Analyse(fileContent, pos, len);

				
				if(pos > 1)
					isFind = true;
				else if(pos == 1)
				{
					pos = copyPos + 1;
					continue;
				}
				else
					break;
			}
			else
				break;
		}

	}

	if(isFind)
		m_pos += copyPos;
	else
	{
		// 存在下一个包标志，则跳转当前已读指针
		vector<string>::iterator packMarkVecIter = m_packMarkVec.begin();	
		for(; packMarkVecIter!=m_packMarkVec.end(); ++packMarkVecIter)
		{
			string packMark = *packMarkVecIter;
			pos = fileContent.find(packMark, firstPackMarkPos+1);
			if(pos != string::npos)
			{
				m_pos += pos;	
				break;
			}
		}
	}
   
	if(length >= m_fileSize) // 此文件已分析完毕
	{
		m_pos = 0;
		return true;
	}
	return false;
}

//功  能：分析目标字符串
//参  数：目标串，分析起始位置，文件总长度
//返回值：目标串已读位置
string::size_type CExtractor::Analyse(string &str, string::size_type pos, long length)
{
	string idStr;
	// 通过从包标志到前缀的最大长度，判断目标串是否完整
	if(pos+m_allLengthFromPackMarkToPrefix >= length)
		return 0;

	// 跳过重复的前缀
	for(int i=0; i<m_repeatNumOfPrefix; i++)
	{
		pos = str.find(m_prefix, pos);
		pos++;
	}
	

	// 获得前缀位置
	pos = str.find(m_prefix, pos);
	while(pos != string::npos)
	{	
		string::size_type copyOfPos = pos;

		// 通过前缀到后缀的最大长度，判断目标串是否完整
		if(pos+m_allLengthFromPrefixToPostfix >= length)
			return 0;
		
		pos += m_prefix.length();
		pos += m_ignoreNum;
		
		string::size_type lastPos = str.find(m_postfix, pos);
		if(lastPos != string::npos)
			idStr = str.substr(pos, lastPos-pos-m_beforePostfixIgnoreNum);	
		else// 找不到后缀
			return 1;

		// 根据长度过滤
		if(idStr.length()>m_maxNumOfDes || idStr.length()<m_minNumOfDes)
		{
			pos = copyOfPos + 1;
			pos = str.find(m_prefix, pos);
			continue;
		}

		// 过滤干扰串
		vector<string>::iterator iter = m_nonTargetStrVec.begin();
		bool isFindNonTargetStr = false; 
		// 是否发现干扰串
		for(; iter!=m_nonTargetStrVec.end(); ++iter)
		{
			if(idStr == (*iter))
			{
				pos = copyOfPos + 1;
				pos = str.find(m_prefix, pos);
				isFindNonTargetStr = true;
				break;
			}
		}
		// 发现干扰子串
		if(isFindNonTargetStr || !IsLegalStr(idStr))
		{
			pos = copyOfPos + 1;
			pos = str.find(m_prefix, pos);
			continue;
		}

		pos += m_postfix.length();

		
		// 获取系统时间
		SYSTEMTIME st;					
		GetSystemTime(&st);
		char *buffer = new char[50];
		sprintf(buffer,"%d/%d/%d %d:%d:%d", st.wYear, st.wMonth, st.wDay, st.wHour+8, st.wMinute, st.wSecond);/*赋予数值*/
		string timeStr(buffer);
		m_idMap.insert(make_pair(timeStr, idStr));

		return pos;
	}
	// 包标志正确且完整，不存在前缀
	return 1;
}

// 功  能：设置根路径
//参  数：文件路径
//返回值：无
void CExtractor::SetWorkPath(string path) 
{
	while(true)   {   
        string::size_type pos = 0;   
        if((pos=path.find("\\")) != string::npos)   
            path.replace(pos, 1, "//");   
        else 
			break;   
    }   
    
	m_workPath = path; 
}

//功  能：获取待读文件名
//参  数：无
//返回值：目标文件名称
string CExtractor::GetLoggerFile()
{
	_finddata_t file;  
    long longf;  
  
    //_findfirst返回的是long型; long __cdecl _findfirst(const char *, struct _finddata_t *)  
	string strRex = m_filePrefix+"*.log";
	if((longf = _findfirst(strRex.c_str(), &file))==-1l)  
    {  
        //cout<<"No file found!/n"; 
		return "";
    }  
    else  
    {  
        //int __cdecl _findnext(long, struct _finddata_t *);如果找到下个文件的名字成功的话就返回0,否则返回-1  
        string tempName;  
		tempName = file.name;  
        if (tempName!=".." && m_fileNameSet.count(m_filePath+file.name)==0) 
		{			
			 _findclose(longf);  
            return m_filePath+file.name; 
		}

        while( _findnext( longf, &file ) == 0 )  
        {  
            tempName = "";  
			tempName = file.name;  
            if (tempName == "..") 
                continue;  
			else if(m_fileNameSet.count(m_filePath+file.name) > 0)
				continue;
			
			_findclose(longf);  
            return m_filePath+file.name; 
          
        }  		
		_findclose(longf);  
		return "";
	}  
  
}
 
//功  能：设置已读文件名
//参  数：名称
//返回值：无
void CExtractor::SetFileIntoSet(string name)
{
	m_fileNameSet.insert(name);
}

//功  能：清空历史日志
//参  数：目录路径
//返回值：是否清除成功
bool CExtractor::ClearHistory(string path)
{
	int i = 0;
	while(i < path.length())
	{
		i = path.find("//", i);
		if(i != string::npos)
		{
			path = path.replace(i, 2, 1, '\\');
			i++;
		}
		else
			break;
	}

	string cmd = "del "+ path + "*.log /a /s /f /q";
	
	if(system(cmd.c_str()) == -1)
		return false;
	return true;
}


//功  能：检查是否是非字母数字构成的合法串
//参  数：待检查字符串
//返回值：bool
bool CExtractor::IsLegalStr(string str)
{
	int length = str.length();
	for(int i=0; i<length; i++)
	{
		if((str[i]>='0' && str[i]<='9') || (str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z'))
			continue;
		else
			return false;
	}
	 
	return true;
}


//
////功  能：获取抓包程序线程句柄
////参  数：运行程序名称
////返回值：线程句柄
//HANDLE CExtractor::CheckProcess(string name)
//{
//	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hProcess == INVALID_HANDLE_VALUE)
//	{
//		return NULL;
//	}
//
//	PROCESSENTRY32 info = {0};
//	info.dwSize = sizeof(PROCESSENTRY32);
//	if (!Process32First(hProcess, &info))
//	{
//		return NULL;
//	}
//	BOOL bFound = FALSE;
//	while (1)
//	{
//		if (!Process32Next(hProcess, &info))
//		{
//			break;
//		}
//		if (info.szExeFile == (WCHAR*)name.c_str())
//		{
//			bFound = TRUE;
//			HANDLE hClose = ::OpenProcess(PROCESS_TERMINATE, FALSE, info.th32ProcessID);
//			if (hClose == NULL)
//			{
//				return NULL;
//			}
//			if (!::TerminateProcess(hClose, 0))
//			{
//				return NULL;
//			}
//			else
//			{
//				return hClose;
//			}
//		}
//	}
//
//	return NULL;
//}
