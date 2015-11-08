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
    si.wShowWindow = false; //TRUE��ʾ��ʾ�����Ľ��̵Ĵ���  
	string cmd = programPath;
	char* szTemp = const_cast<char*>(cmd.c_str());
	TCHAR szOut[MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP,0,szTemp,-1,szOut,strlen(szTemp));

    BOOL bRet = ::CreateProcess(
        NULL,  
        szOut, //��Unicode�汾�д˲�������Ϊ�����ַ�������Ϊ�˲����ᱻ�޸�    
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

       /* printf(" �½��̵Ľ���ID�ţ�%d /n", pi.dwProcessId);   
        printf(" �½��̵����߳�ID�ţ�%d /n", pi.dwThreadId); */  
    }   
    else  
    {  
        printf("error/n");  
    }  
}

int test()
{
	//========================================================================
	// �رտ��ܴ��ڵĺ�̨�������

	STARTUPINFO si = { sizeof(si) };   
    PROCESS_INFORMATION pi;   
  
    si.dwFlags = STARTF_USESHOWWINDOW;   
    si.wShowWindow = false; //TRUE��ʾ��ʾ�����Ľ��̵Ĵ���  
	// �����������
	char* szTemp = "TASKKILL /F /IM dumpcap.exe* /T";
	TCHAR szOut[MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP,0,szTemp,-1,szOut,strlen(szTemp));
	// ��������
    BOOL bRet = ::CreateProcess(
        NULL,  
        szOut, //��Unicode�汾�д˲�������Ϊ�����ַ�������Ϊ�˲����ᱻ�޸�    
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
	// ����ץ������
	CExtractor extractor;
	extractor.SetWorkPath("D:\\crawler\\");
	// ��ȡ�����ļ�
	if(!extractor.InitParameter("setting.ini"))
	{
		cout<<"��ȡ����ʧ�ܣ�"<<endl;
		return false;
	}

	// �����ʷ��־��¼
	if(extractor.isClearHistory())
	{
		string filePrefix = extractor.GetFilePrefix();
		bool isSuccess = extractor.ClearHistory(filePrefix);

		if(!isSuccess)
		{		
			cout<<"�����ʷ��־ʧ�ܣ�"<<endl;
			getchar();
			return 0;
		}
	}

	// ��ʼץ��
	HANDLE downloadThread = (HANDLE)_beginthreadex(NULL, 0, (PTHREAD_START)StartGraspPackage, &extractor, 0, NULL);
	CloseHandle(downloadThread);

	// ��ȡ������־�����������̨���ⲿ��־�ļ�
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
	// ����ץ������
	//string programName = "myfirstbat";// extractor.GetProgramName();
	//HANDLE hProcess = extractor.CheckProcess(programName);
	
	//system("TASKKILL /F /T /FI \"WINDOWTITLE eq crawler_bat*\" ");
	//system("TASKKILL /F /IM cmd.exe /T");
	

	// �رպ�̨ץ������
	STARTUPINFO sInfo = { sizeof(si) };   
    PROCESS_INFORMATION pInfo;   
  
    sInfo.dwFlags = STARTF_USESHOWWINDOW;   
    sInfo.wShowWindow = false; //TRUE��ʾ��ʾ�����Ľ��̵Ĵ���  
	// �����������
	std::stringstream ss;
	std::string idStr;
	ss<<g_processId;
	ss>>idStr;
	string cmdStr = "TASKKILL /F /PID "+idStr+" /T";
	char* szTmp = const_cast<char*>(cmdStr.c_str());
	TCHAR sOut[MAX_PATH]={0};
	MultiByteToWideChar(CP_ACP,0,szTmp,-1,sOut,strlen(szTmp));
	// ��������
    BOOL returnVal = ::CreateProcess(
        NULL,  
        sOut, //��Unicode�汾�д˲�������Ϊ�����ַ�������Ϊ�˲����ᱻ�޸�    
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