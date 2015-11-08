///////////////////////////////////////////////////////////////////////
//�ļ����� Extractor_init.cpp
//��  �ܣ� ����ץ�������ʼ��
//�����ߣ� ������
//��  ����2014/5/26
//��  �ģ�2014/5/26
//��  �ȣ�371
//��  ע������
///////////////////////////////////////////////////////////////////////
#include "Extractor.h"
#include <io.h> 
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>

//��  �ܣ���ȡ�����ļ�
//��  �����ļ�·��
//����ֵ���Ƿ���ȷ��ȡ��������
bool CExtractor::InitParameter(string path)
{
	vector<int> tmpIntVec;
	vector<string> tmpStrVec;
	// ----------------��־�ļ�ǰ׺----------------
	tmpStrVec = ReadStrFromIni(path, "filePrefix");
	if(tmpStrVec.size() != 0)
		m_filePrefix = (*tmpStrVec.begin());
	else
		return false;
	string::size_type pos = m_filePrefix.find_last_of("//");
	m_filePath = m_workPath + m_filePrefix.substr(0, pos+1);
	m_filePrefix = m_workPath + m_filePrefix;
	// ----------------���ļ���С----------------
	tmpIntVec = ReadIntFromIni(path, "fileSize");
	if(tmpIntVec.size() != 0)
	{
		m_fileSize = *(tmpIntVec.begin());
	}
	else
		return false;

	// ----------------����ʶ ----------------
	m_packMarkVec = ReadStrFromIni(path, "packMark");

	// ----------------ǰ׺��ʶ----------------
	m_prefixVec = ReadStrFromIni(path, "prefix");

	// ----------------ǰ׺�ظ�����----------------
	m_repeatNumOfPrefixVec = ReadIntFromIni(path, "repeatNumOfPrefix");

	// ----------------ǰ׺��ʶ������ַ�λ��----------------
	m_ignoreNumVec = ReadIntFromIni(path, "ignoreNum");

	// ----------------Ŀ���ַ����λ��----------------
	tmpIntVec = ReadIntFromIni(path, "maxNumOfDes");
	if(tmpIntVec.size() != 0)
	{
		m_maxNumOfDes = *(tmpIntVec.begin());
	}
	else
		return false;
	// ----------------Ŀ���ַ�����Сλ��----------------
	tmpIntVec = ReadIntFromIni(path, "minNumOfDes");
	if(tmpIntVec.size() != 0)
	{
		m_minNumOfDes = *(tmpIntVec.begin());
	}
	else
		return false;

	// ----------------��׺�ַ���----------------
	m_postfixVec = ReadStrFromIni(path, "postfix");

	// ----------------��׺ǰ������ַ�������Ŀ�괮----------------
	m_beforePostfixIgnoreNumVec = ReadIntFromIni(path, "beforePostfixIgnoreNum");

	// ----------------Ŀ�괮�ܳ�----------------
	m_allLengthFromPrefixToPostfixVec = ReadIntFromIni(path, "allLengthFromPrefixToPostfix");

	m_allLengthFromPackMarkToPrefixVec = ReadIntFromIni(path, "allLengthFromPackMarkToPrefix");

	// -------------���Ŵ�����----------------
	m_nonTargetStrVec = ReadStrFromIni(path, "nonTargetStr"); 

	// �Ƿ������ʷ��¼
	tmpIntVec = ReadIntFromIni(path, "isDeleteHistory");
	if(tmpIntVec.size() != 0)
	{
		m_isDeleteHistory  = *(tmpIntVec.begin());
	}
	else
		return false;

	// ����·��
	tmpStrVec = ReadStrFromIni(path, "programPath");
	if(tmpStrVec.size() != 0)
	{
		m_programPath = m_workPath + *(tmpStrVec.begin());
	}
	else
		return false;

	// ��������
	tmpStrVec = ReadStrFromIni(path, "programName");
	if(tmpStrVec.size() != 0)
	{
		m_programName = *(tmpStrVec.begin());
	}
	else
		return false;
	
	return true;
}
 
//��  �ܣ�����keyֵ��ȡini��Ӧֵ
//��  ���������ļ�·������ֵ
//����ֵ����ȡ��Ŀ������ֵ(�ַ���)
vector<string> CExtractor::ReadStrFromIni(string path, string key)
{
	vector<string> strVec;
	strVec.clear();

	locale loc = locale::global(locale("")); //Ҫ�򿪵��ļ�·�������ģ�����ȫ��localeΪ���ػ���     
	ifstream file_op(path.c_str(), ios::in);	
	locale::global(loc);//�ָ�ȫ��locale	

    if (!file_op) {
        cout<<"Can not open file"<<endl;
        return strVec;
    }
    string line;
    while (getline(file_op, line) ) {
        int i = line.find(key, 0);//��0λ��ʼ��
        if (i==0) {//�ҵ���keyֵ
            int k = line.find("=", 0);//�ҵ��ں�
			file_op.close();
            string valStr =  line.substr(k+1);//ȡ���ںź�ߵ�ֵ
			// �滻���е�ʮ��������Ϊ��Ӧ�ַ�
			while(i < valStr.length())
			{
				i = valStr.find("0x", i);
				if(i != string::npos)
				{
					int c1, c2;
					// ת����һ���ַ�
					if(valStr[i+2] >= 97)
						c1 = valStr[i+2] - 87;
					else
						c1 = valStr[i+2] - 48;
					// ת���ڶ����ַ�
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

			// �滻���е����ڱ�־Ϊ��Ӧ�ַ�
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
					sprintf(buffer,"%d%d%d", year, month, day);/*������ֵ*/
					valStr = valStr.replace(i, i+6, buffer);
					i += 7;
				}
				else
					break;
			}

			// ��ȡ��&&�������ַ���
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
				// ���һ�������Ӵ�
				subStr = valStr.substr(first_pos, pos-first_pos);
				strVec.push_back(subStr);
			}
			return strVec;
        }
    }
	file_op.close();
}

//��  �ܣ�����keyֵ��ȡini��Ӧֵ
//��  ���������ļ�·������ֵ
//����ֵ����ȡ��Ŀ������ֵ(����)
vector<int> CExtractor::ReadIntFromIni(string path, string key)
{
	locale loc = locale::global(locale("")); //Ҫ�򿪵��ļ�·�������ģ�����ȫ��localeΪ���ػ���     
	ifstream file_op(path.c_str(), ios::in);	
	locale::global(loc);//�ָ�ȫ��locale	

	vector<int> numVec;
	numVec.clear();

    if (!file_op) {
        cout<<"Can not open file"<<endl;
        return numVec;
    }
    string line;
    while (getline(file_op, line) )
	{
        int i = line.find(key, 0);//��0λ��ʼ��
        if(i==0) //�ҵ���keyֵ
		{
            int k = line.find("=", 0);//�ҵ��ں�
            string valStr = line.substr(k+1);//ȡ���ںź�ߵ�ֵ

			// ��ȡ�Կո��������������
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
			// ���һ�������Ӵ�
			subStr = valStr.substr(first_pos, pos-first_pos);
			numVec.push_back(IntegerToStr(subStr));
			
			break;
        }
    }
	file_op.close();
	return numVec;
}


//��  �ܣ����ַ���ת��������
//��  �����ַ���
//����ֵ������
int CExtractor::IntegerToStr(string numStr)
{
	int num;
	stringstream ss;
	ss.clear();
	ss<<numStr;
	ss>>num;
	return num;
}