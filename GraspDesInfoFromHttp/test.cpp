#include <iostream>
#include <fstream>
#include "Extractor.h"
#include <string>
#include <sstream>
#include <io.h>
#include <stdio.h>
#include <process.h>

using namespace std;
typedef unsigned (_stdcall *PTHREAD_START) (void*);

int g_processId;

void StartGraspPackage(LPVOID lpParameter)
{
	CExtractor *extractor = (CExtractor*)lpParameter;
	string programPath = extractor->GetProgramPath();
	
	//system((LPCSTR)programPath.c_str());,SW_HIDE
	//system(("start \"crawler_bat\" "+programPath).c_str());
	STARTUPINFO si = { sizeof(si) };   
    PROCESS_INFORMATION pi;   
  
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = false; //TRUE表示显示创建的进程的窗口  
	string cmd = programPath;
	char* szTemp = const_cast<char*>(cmd.c_str());
	TCHAR szOut[MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP,0,szTemp,-1,szOut,strlen(szTemp));

    BOOL bRet = ::CreateProcess(
        NULL,  
        szOut, //在Unicode版本中此参数不能为常量字符串，因为此参数会被修改    
        NULL,   
        NULL,   
        FALSE,   
        CREATE_NEW_CONSOLE,   
        NULL,   
        NULL,   
        &si,   
        &pi);   

	if(bRet)   
    {   
		g_processId= pi.dwProcessId;
        ::CloseHandle (pi.hThread);   
        ::CloseHandle (pi.hProcess);   

       /* printf(" 新进程的进程ID号：%d /n", pi.dwProcessId);   
        printf(" 新进程的主线程ID号：%d /n", pi.dwThreadId); */  
    }   
    else  
    {  
        printf("error/n");  
    }  
}

int test()
{
	//========================================================================
	// 关闭可能存在的后台程序残留

	STARTUPINFO si = { sizeof(si) };   
    PROCESS_INFORMATION pi;   
  
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = false; //TRUE表示显示创建的进程的窗口  
	// 设置命令参数
	char* szTemp = "TASKKILL /F /IM dumpcap.exe* /T";
	TCHAR szOut[MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP,0,szTemp,-1,szOut,strlen(szTemp));
	// 运行命令
    BOOL bRet = ::CreateProcess(
        NULL,  
        szOut, //在Unicode版本中此参数不能为常量字符串，因为此参数会被修改    
        NULL,   
        NULL,   
        FALSE,   
        CREATE_NEW_CONSOLE,   
        NULL,   
        NULL,   
        &si,   
        &pi);   

	if(bRet)   
    {   
        ::CloseHandle (pi.hThread);   
        ::CloseHandle (pi.hProcess);   
    }   
    else  
    {  
        printf("Error in ending background program before! /n");  
    }  
	//system("TASKKILL /F /IM dumpcap.exe* /T");
	//========================================================================
	// 启动抓包程序
	CExtractor extractor;
	extractor.SetWorkPath("D:\\crawler\\");
	// 读取配置文件
	if(!extractor.InitParameter("setting.ini"))
	{
		cout<<"读取参数失败！"<<endl;
		return false;
	}

	// 清空历史日志记录
	if(extractor.isClearHistory())
	{
		string filePrefix = extractor.GetFilePrefix();
		bool isSuccess = extractor.ClearHistory(filePrefix);

		if(!isSuccess)
		{		
			cout<<"清空历史日志失败！"<<endl;
			getchar();
			return 0;
		}
	}

	// 开始抓包
	HANDLE downloadThread = (HANDLE)_beginthreadex(NULL, 0, (PTHREAD_START)StartGraspPackage, &extractor, 0, NULL);
	CloseHandle(downloadThread);

	// 读取分析日志，输出到控制台和外部日志文件
	ofstream output("id.log", ios::app);
	cout<<"Output ID: "<<endl;
	while(true)
	{
		string path = extractor.GetLoggerFile();
		while(path != "")
		{
			if(extractor.ReadDataAndAnalyse(path)) 
			{	
				extractor.SetFileIntoSet(path);
				path = extractor.GetLoggerFile();
			}
			multimap<string, string> resultMap = extractor.GetIdMap();
			for(multimap<string, string>::iterator iter=resultMap.begin(); iter!=resultMap.end(); ++iter)
			{
				output<<iter->first<<" "<<iter->second<<endl;
				cout<<iter->first<<" "<<iter->second<<endl;
				
			}			
			extractor.ClearIdSet();
		}
		//Sleep(100);
	}  
	
	output.close();
	cout<<"end?"<<endl;
	getchar();


	//========================================================================
	// 结束抓包程序
	//string programName = "myfirstbat";// extractor.GetProgramName();
	//HANDLE hProcess = extractor.CheckProcess(programName);
	
	//system("TASKKILL /F /T /FI \"WINDOWTITLE eq crawler_bat*\" ");
	//system("TASKKILL /F /IM cmd.exe /T");
	

	// 关闭后台抓包程序
	STARTUPINFO sInfo = { sizeof(si) };   
    PROCESS_INFORMATION pInfo;   
  
    sInfo.dwFlags = STARTF_USESHOWWINDOW;   
    sInfo.wShowWindow = false; //TRUE表示显示创建的进程的窗口  
	// 设置命令参数
	std::stringstream ss;
	std::string idStr;
	ss<<g_processId;
	ss>>idStr;
	string cmdStr = "TASKKILL /F /PID "+idStr+" /T";
	char* szTmp = const_cast<char*>(cmdStr.c_str());
	TCHAR sOut[MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP,0,szTmp,-1,sOut,strlen(szTmp));
	// 运行命令
    BOOL returnVal = ::CreateProcess(
        NULL,  
        sOut, //在Unicode版本中此参数不能为常量字符串，因为此参数会被修改    
        NULL,   
        NULL,   
        FALSE,   
        CREATE_NEW_CONSOLE,   
        NULL,   
        NULL,   
        &sInfo,   
        &pInfo);   

	if(returnVal)   
    {   
        ::CloseHandle (pInfo.hThread);   
        ::CloseHandle (pInfo.hProcess);   
    }   
    else  
    {  
        printf("Error in ending background program! /n");  
    } 

	getchar();

	return 0;

}

int main()
{
	test();
	cout<<"one more time?"<<endl;
	getchar();

	test();

}