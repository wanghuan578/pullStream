
#include "iniReader.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************
* ��  �ܣ����캯��
* ��  ������
* ����ֵ����
* ��  ע��
******************************************************************************/
CIniReader::CIniReader( )
{
 memset( m_szKey,0,sizeof(m_szKey) );
 m_fp = NULL;
}

/******************************************************************************
* ��  �ܣ���������
* ��  ������
* ����ֵ����
* ��  ע��
******************************************************************************/

CIniReader::~CIniReader()
{
 m_Map.clear();
}

/******************************************************************************
* ��  �ܣ����ļ�����
* ��  ������
* ����ֵ��
* ��  ע��
******************************************************************************/
INI_RES CIniReader::Load(const char* pathName, const char* type)
{
 string szLine,szMainKey,szLastMainKey,szSubKey;
 char strLine[ CONFIGLEN ] = { 0 };
 KEYMAP mLastMap;
 int  nIndexPos = -1;
 int  nLeftPos = -1;
 int  nRightPos = -1;
    m_fp = fopen(pathName, type);
    
    if (m_fp == NULL)
    {
  printf( "open inifile %s error!\n",pathName );
        return INI_OPENFILE_ERROR;
    }

 m_Map.clear();

 while( fgets( strLine, CONFIGLEN,m_fp) )
 {  
  szLine.assign( strLine );
  //ɾ���ַ����еķǱ�Ҫ�ַ�
  nLeftPos = szLine.find("\n" );
  if( string::npos != nLeftPos )
  {
   szLine.erase( nLeftPos,1 );
  }
  nLeftPos = szLine.find("\r" );
  if( string::npos != nLeftPos )
  {
   szLine.erase( nLeftPos,1 );
  }   
  //�ж��Ƿ�������
  nLeftPos = szLine.find("[");
  nRightPos = szLine.find("]");
  if(  nLeftPos != string::npos && nRightPos != string::npos )
  {
   szLine.erase( nLeftPos,1 );
   nRightPos--;
   szLine.erase( nRightPos,1 );
   m_Map[ szLastMainKey ] = mLastMap;
   mLastMap.clear();
   szLastMainKey =  szLine ;
  }
  else
  {  
   
    
   //�Ƿ����Ӽ�
   if( nIndexPos = szLine.find("=" ),string::npos != nIndexPos)
   {
    string szSubKey,szSubValue;
    szSubKey = szLine.substr( 0,nIndexPos );
    szSubValue = szLine.substr( nIndexPos+1,szLine.length()-nIndexPos-1);
    mLastMap[szSubKey] = szSubValue ;
   }
   else
   {
    //TODO:������ini��ֵģ������� ��ע�͵�
   }
  }

 }
 //�������һ������
 m_Map[ szLastMainKey ] = mLastMap;

    return INI_SUCCESS;
}

/******************************************************************************
* ��  �ܣ��ر��ļ�����
* ��  ������
* ����ֵ��
* ��  ע��
******************************************************************************/
INI_RES CIniReader::CloseFile()
{
 
 
    if (m_fp != NULL)
    {
        fclose(m_fp);
  m_fp = NULL;
    } 
 
    return INI_SUCCESS;
}

/******************************************************************************
* ��  �ܣ���ȡ[SECTION]�µ�ĳһ����ֵ���ַ���
* ��  ����
*  char* mAttr  �������    ����
*  char* cAttr  ������� �Ӽ�
*  char* value  ������� �Ӽ���ֵ
* ����ֵ��
* ��  ע��
******************************************************************************/
INI_RES CIniReader::GetKey(const char* mAttr, const char* cAttr, char* pValue)
{
 
    KEYMAP mKey = m_Map[ mAttr ];

 string sTemp = mKey[ cAttr ];
 
 strcpy( pValue,sTemp.c_str() );
 
    return INI_SUCCESS;
}

/******************************************************************************
* ��  �ܣ���ȡ���εļ�ֵ
* ��  ����
*       cAttr                     ����
*      cAttr                     �Ӽ�
* ����ֵ�������򷵻ض�Ӧ����ֵ δ��ȡ�ɹ��򷵻�0(��ֵ����Ϊ0����ͻ)
* ��  ע��
******************************************************************************/
int CIniReader::GetInt(const char* mAttr, const char* cAttr )
{
 int nRes = 0;

 memset( m_szKey,0,sizeof(m_szKey) );
 
 if( INI_SUCCESS == GetKey( mAttr,cAttr,m_szKey ) )
 {
  nRes = atoi( m_szKey );
 }
 return nRes;
}

/******************************************************************************
* ��  �ܣ���ȡ��ֵ���ַ���
* ��  ����
*       cAttr                     ����
*      cAttr                     �Ӽ�
* ����ֵ�������򷵻ض�ȡ�����Ӽ��ַ��� δ��ȡ�ɹ��򷵻�"NULL"
* ��  ע��
******************************************************************************/
char *CIniReader::GetStr(const char* mAttr, const char* cAttr )
{
	memset( m_szKey,0,sizeof(m_szKey) );
	 
	if( INI_SUCCESS != GetKey( mAttr,cAttr,m_szKey ) )
	{
		strcpy( m_szKey,"NULL" );
	}
	 
	return m_szKey;

}
