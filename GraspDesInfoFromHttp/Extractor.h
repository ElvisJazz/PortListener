///////////////////////////////////////////////////////////////////////
//�ļ����� Extractor.h
//��  �ܣ� ����ץ�����򲢶����ݰ����з��������Ŀ������(����id)
//�����ߣ� ������
//��  ����2014/4/15
//��  �ģ�2014/4/24
//��  �ȣ�100
//��  ע������
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
	// �������������
	CExtractor();
	~CExtractor();

	// ��ȡ��־�ļ�����
	bool ReadDataAndAnalyse(string path);
	// ����
	string::size_type Analyse(string &str, string::size_type pos, long length);
	// ��ȡ�����ļ�
	bool InitParameter(string path);
	// ����keyֵ��ȡini��Ӧֵ
	vector<string> ReadStrFromIni(string path, string key);
	vector<int> ReadIntFromIni(string path, string key);
	// ���ַ���ת��������
	int IntegerToStr(string numStr);
	// ���ø�·��
	void SetWorkPath(string path);

	// ��պͻ�ȡid
	void ClearIdSet() { m_idMap.clear(); }
	multimap<string, string> GetIdMap() { return m_idMap; }
	// ��ȡ�����ļ���
	string GetLoggerFile();
	// �����Ѷ��ļ���
	void SetFileIntoSet(string name);
	// ���õ�����־�ļ���С
	void SetFileSize(long size) { m_fileSize = size; }
	// ��ȡץ��ִ�г���·��
	string GetProgramPath() { return m_programPath;}

	// ��ȡץ��ִ�г�������
	string GetProgramName() { return m_programName;}

	// ��ȡ��־�ļ�ǰ׺���ʽ
	string GetFilePrefix() { return m_filePrefix;	}
	// �Ƿ����
	bool isClearHistory() {return m_isDeleteHistory;}
	// �����ʷ��־
	bool ClearHistory(string path);

	// ����Ƿ��Ƿ���ĸ���ֹ��ɵĺϷ���
	bool IsLegalStr(string str);
	// ������̼�⺯��
	//HANDLE CheckProcess(string name);

private:
	// ��־�ļ���ַ�������ļ�����
	string m_filePath;
	// �Ѷ��ļ���(��·��)����
	set<string> m_fileNameSet;
	// id����
	multimap<string, string> m_idMap; 
	// ���ļ�λ��ָ��
	unsigned long m_pos;
	// ������־�ļ�Ĭ�ϴ�С
	unsigned long m_fileSize;

	// ���ò���
	string m_filePrefix;
	// ����ʶ
	vector<string> m_packMarkVec;
	string m_packMark;
	// ǰ׺��ʶ
	vector<string> m_prefixVec;
	string m_prefix;
	// ǰ׺�ظ�
	vector<int> m_repeatNumOfPrefixVec;
	int m_repeatNumOfPrefix;
	// ǰ׺��ʶ������ַ�λ��
	vector<int> m_ignoreNumVec;
	int m_ignoreNum;
	// Ŀ���ַ������λ��
	int m_maxNumOfDes;
	// Ŀ���ַ�����Сλ��
	int m_minNumOfDes;
	// ��׺�ַ���
	vector<string> m_postfixVec;
	string m_postfix;
	// ��׺ǰ������ַ�������Ŀ�괮
	vector<int> m_beforePostfixIgnoreNumVec;
	int m_beforePostfixIgnoreNum;
	// Ŀ�괮ǰ׺����׺������־��ǰ׺�ܳ���
	vector<int> m_allLengthFromPrefixToPostfixVec;
	vector<int> m_allLengthFromPackMarkToPrefixVec;
	int m_allLengthFromPrefixToPostfix;
	int m_allLengthFromPackMarkToPrefix;
	// ���Ŵ�����
	vector<string> m_nonTargetStrVec;
	// �Ƿ��������ʷ��¼
	bool m_isDeleteHistory;
	
	// ץ����������
	string m_programPath;
	// ������
	string m_programName;
	// ����·��
	string m_workPath;
};

#endif