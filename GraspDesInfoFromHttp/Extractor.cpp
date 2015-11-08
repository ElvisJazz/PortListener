///////////////////////////////////////////////////////////////////////
//�ļ����� Extractor.cpp
//��  �ܣ� ����ץ�����򲢶����ݰ����з��������Ŀ������(����id)
//�����ߣ� ������
//��  ����2014/4/15
//��  �ģ�2014/4/24
//��  �ȣ�371
//��  ע������
///////////////////////////////////////////////////////////////////////
#include "Extractor.h"
#include <io.h> 
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>

//��  �ܣ����캯��
//��  ������
//����ֵ����
CExtractor::CExtractor()
{
	m_pos = 0;
}

//��  �ܣ���������
//��  ������
//����ֵ����
CExtractor::~CExtractor()
{

}

//��  �ܣ���ȡ��־�ļ�����
//��  �����ļ�·��
//����ֵ�����ļ��Ƿ���꣬��ȡ�¸��ļ�
bool CExtractor::ReadDataAndAnalyse(string path)
{
	locale loc = locale::global(locale("")); //Ҫ�򿪵��ļ�·�������ģ�����ȫ��localeΪ���ػ���     
	ifstream in(path.c_str(), ios::binary);	
	locale::global(loc);//�ָ�ȫ��locale
	if (!in) {
        cout<<"Can not open file"<<endl;
        return false;
    }

	string fileContent = "";
	string::size_type pos = 0;

	in.seekg(0, ios::end);
	long length = in.tellg(); // �ļ��ܴ�С

	in.seekg(m_pos, ios::beg); // ת���ϴζ�����λ��
	
	long len = length - in.tellg(); // ��ȡ���ζ�ȡ�����ݶγ���
	in.seekg(m_pos+1);

	fileContent.resize(len); // ���û�������С
	in.read((char*)&fileContent[0], len); // ��ȡָ��λ�úͶγ�����
	in.close(); // �ر��ļ�������
    bool isFind = false; //���ζ����Ƿ��ҵ�Ŀ�괮
	string::size_type copyPos = 0; // �ļ����ݵ�ǰ��ȡָ��λ��
	string::size_type firstPackMarkPos = 0; // ��һ������־����λ��

	// ���ڶ��������־λ�����ѭ����������һ���ҵ�������ѭ��
	vector<string>::iterator packMarkIter = m_packMarkVec.begin();	
	vector<string>::iterator prefixIter = m_prefixVec.begin();
	vector<int>::iterator repeatNumOfPrefixIter = m_repeatNumOfPrefixVec.begin();
	vector<int>::iterator ignoreNumIter = m_ignoreNumVec.begin();
	vector<string>::iterator postfixIter = m_postfixVec.begin();
	vector<int>::iterator beforePostfixIgnoreNumIter = m_beforePostfixIgnoreNumVec.begin();
	vector<int>::iterator allLengthFromPrefixToPostfixIter = m_allLengthFromPrefixToPostfixVec.begin();
	vector<int>::iterator allLengthFromPackMarkToPrefixIter = m_allLengthFromPackMarkToPrefixVec.begin();
	// ��ֵ����ʱ����pos����λָ��
	string::size_type copyPosForResearch = pos;
	// ��ȡ����ƫ�����Ƿ�����һ������
	string::size_type isUseLastPack;

	while(packMarkIter!=m_packMarkVec.end())
	{
		pos = copyPosForResearch;
		 
		// ��������������ֵ
		m_packMark = *packMarkIter; // ����ʶ

		if(prefixIter!=m_prefixVec.end() && ignoreNumIter!=m_ignoreNumVec.end() && postfixIter!=m_postfixVec.end() && beforePostfixIgnoreNumIter!=m_beforePostfixIgnoreNumVec.end()
			&& allLengthFromPrefixToPostfixIter!=m_allLengthFromPrefixToPostfixVec.end() && allLengthFromPackMarkToPrefixIter!=m_allLengthFromPackMarkToPrefixVec.end() && repeatNumOfPrefixIter!=m_repeatNumOfPrefixVec.end())
		{
			m_prefix = *prefixIter; // ǰ׺
			m_repeatNumOfPrefix = *repeatNumOfPrefixIter; // ǰ׺�ظ�����
			m_ignoreNum = *ignoreNumIter; // ǰ׺��Ŀ�괮�ɺ����ַ���
			m_postfix = *postfixIter; // ��׺
			m_beforePostfixIgnoreNum = *beforePostfixIgnoreNumIter; // Ŀ�괮ĩβ����׺�ɺ����ַ���
			m_allLengthFromPrefixToPostfix = *allLengthFromPrefixToPostfixIter; // ǰ׺����׺�ܳ������ֵ
			m_allLengthFromPackMarkToPrefix = *allLengthFromPackMarkToPrefixIter; // ����־��ǰ׺�ܳ������ֵ
			// ����������
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
				// ���°���־λ��Ϊ����λָ��
				if(pos > firstPackMarkPos)
				{
					firstPackMarkPos = pos;
					isUseLastPack = false;
				}

				pos += m_packMark.length();

				// �˴���pos�Ƿ�Ϊ���İ�λ������
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
		// ������һ������־������ת��ǰ�Ѷ�ָ��
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
   
	if(length >= m_fileSize) // ���ļ��ѷ������
	{
		m_pos = 0;
		return true;
	}
	return false;
}

//��  �ܣ�����Ŀ���ַ���
//��  ����Ŀ�괮��������ʼλ�ã��ļ��ܳ���
//����ֵ��Ŀ�괮�Ѷ�λ��
string::size_type CExtractor::Analyse(string &str, string::size_type pos, long length)
{
	string idStr;
	// ͨ���Ӱ���־��ǰ׺����󳤶ȣ��ж�Ŀ�괮�Ƿ�����
	if(pos+m_allLengthFromPackMarkToPrefix >= length)
		return 0;

	// �����ظ���ǰ׺
	for(int i=0; i<m_repeatNumOfPrefix; i++)
	{
		pos = str.find(m_prefix, pos);
		pos++;
	}
	

	// ���ǰ׺λ��
	pos = str.find(m_prefix, pos);
	while(pos != string::npos)
	{	
		string::size_type copyOfPos = pos;

		// ͨ��ǰ׺����׺����󳤶ȣ��ж�Ŀ�괮�Ƿ�����
		if(pos+m_allLengthFromPrefixToPostfix >= length)
			return 0;
		
		pos += m_prefix.length();
		pos += m_ignoreNum;
		
		string::size_type lastPos = str.find(m_postfix, pos);
		if(lastPos != string::npos)
			idStr = str.substr(pos, lastPos-pos-m_beforePostfixIgnoreNum);	
		else// �Ҳ�����׺
			return 1;

		// ���ݳ��ȹ���
		if(idStr.length()>m_maxNumOfDes || idStr.length()<m_minNumOfDes)
		{
			pos = copyOfPos + 1;
			pos = str.find(m_prefix, pos);
			continue;
		}

		// ���˸��Ŵ�
		vector<string>::iterator iter = m_nonTargetStrVec.begin();
		bool isFindNonTargetStr = false; 
		// �Ƿ��ָ��Ŵ�
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
		// ���ָ����Ӵ�
		if(isFindNonTargetStr || !IsLegalStr(idStr))
		{
			pos = copyOfPos + 1;
			pos = str.find(m_prefix, pos);
			continue;
		}

		pos += m_postfix.length();

		
		// ��ȡϵͳʱ��
		SYSTEMTIME st;					
		GetSystemTime(&st);
		char *buffer = new char[50];
		sprintf(buffer,"%d/%d/%d %d:%d:%d", st.wYear, st.wMonth, st.wDay, st.wHour+8, st.wMinute, st.wSecond);/*������ֵ*/
		string timeStr(buffer);
		m_idMap.insert(make_pair(timeStr, idStr));

		return pos;
	}
	// ����־��ȷ��������������ǰ׺
	return 1;
}

// ��  �ܣ����ø�·��
//��  �����ļ�·��
//����ֵ����
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

//��  �ܣ���ȡ�����ļ���
//��  ������
//����ֵ��Ŀ���ļ�����
string CExtractor::GetLoggerFile()
{
	_finddata_t file;  
    long longf;  
  
    //_findfirst���ص���long��; long __cdecl _findfirst(const char *, struct _finddata_t *)  
	string strRex = m_filePrefix+"*.log";
	if((longf = _findfirst(strRex.c_str(), &file))==-1l)  
    {  
        //cout<<"No file found!/n"; 
		return "";
    }  
    else  
    {  
        //int __cdecl _findnext(long, struct _finddata_t *);����ҵ��¸��ļ������ֳɹ��Ļ��ͷ���0,���򷵻�-1  
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
 
//��  �ܣ������Ѷ��ļ���
//��  ��������
//����ֵ����
void CExtractor::SetFileIntoSet(string name)
{
	m_fileNameSet.insert(name);
}

//��  �ܣ������ʷ��־
//��  ����Ŀ¼·��
//����ֵ���Ƿ�����ɹ�
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


//��  �ܣ�����Ƿ��Ƿ���ĸ���ֹ��ɵĺϷ���
//��  ����������ַ���
//����ֵ��bool
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
////��  �ܣ���ȡץ�������߳̾��
////��  �������г�������
////����ֵ���߳̾��
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
