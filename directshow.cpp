
#include "directshow.h"
#include <stdio.h>


//dshow�ӿڳ�ʼ��
MediaControl::MediaControl() 
{
 //��DSHOW�ӿ��ÿ�
	m_pGB = NULL;
	m_pMC = NULL; 
	m_pME = NULL;
	m_pVW = NULL;
	m_pBA = NULL;
	m_pBV = NULL;
	m_pMS = NULL;
	//��ʼ�� COM ����
	CoInitialize(NULL); 
}


MediaControl::~MediaControl() 
{
 DshowUninit();
 //�ͷ�COM ����
 CoUninitialize();
}


void MediaControl::DshowInit() 
{
    m_pGB = NULL;
	m_pMC = NULL; 
	m_pME = NULL;
	m_pVW = NULL;
	m_pBA = NULL;
	m_pBV = NULL;
	m_pMS = NULL;
	m_pMP = NULL;
	//��ʼ��COM�ӿ�
	CoInitialize(NULL); 
}


//dshow�ӿ��ͷ�
void MediaControl::DshowUninit()  
{
 if (m_pMC != NULL)
	{
		//ֹͣ��Ƶ����
		m_pMC->Stop(); 
		m_pMC->Release();
		m_pMC = NULL;
	}
	
	//2,�ͷ�ý���¼��ӿ�
	if (m_pME != NULL)
	{
		//��Ϣ֪ͨ�����ÿ�
		m_pME->SetNotifyWindow(NULL, 0, 0);
		m_pME->Release();
		m_pME = NULL;
	}

	//3,�ͷ���Ƶ���Ŵ��ڽӿ�
	if (m_pVW != NULL)
	{
		//������Ƶ����
		m_pVW->put_Visible(OAFALSE);
		//������Ƶ���Ŵ��ھ��Ϊ��
		m_pVW->put_Owner(NULL);
		m_pVW->Release();
		m_pVW = NULL;
	}

	//4,�ͷŻ�����Ƶ���ӿ�
	if (m_pBA != NULL)
	{
		m_pBA->Release();
		m_pBA = NULL;
	}

	//5���ͷŻ�����Ƶ���ӿ�
	if (m_pBV != NULL)
	{
		m_pBV->Release();
		m_pBV = NULL;
	}

	//6���ͷ�ý�������ӿ�
	if (m_pMS != NULL)
	{
		m_pMS->Release();
		m_pMS = NULL;
	}

	//7,����ͷ�FilterGpaph�ӿ�
	if (m_pGB != NULL)
	{
		m_pGB->Release();
		m_pGB = NULL;
	}
    
	
	
}


/*******************************************************
*�������Ŵ��� ����Ⱦ�ļ�
*InPut : inWindow      ��ʾ��Ƶ��Ƶ���Ĵ��ھ��
         strFileName   Ҫ���ŵ��ļ�·��
		 hNotifyWnd    ����ý���¼��Ĵ���
*OutPut : none
********************************************************/
int MediaControl::DshowCreate(HWND inWindow,TCHAR *strFileName,HWND hNotifyWnd) 
{
  int FLAG=1;
	DshowUninit();
  HRESULT hResult; 
  hResult = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB);
  hResult = m_pGB->RenderFile(strFileName,NULL);
 if (hResult != S_OK)
 {
	
   //MessageBox(hNotifyWnd,L"��֧�ֵ�ý���ʽ",NULL,NULL);
   return 0;
 }

  hResult = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC); 
  if (hResult != S_OK)
  {

	  //MessageBox(hNotifyWnd,L"��֧�ֵ�ý���ʽ",NULL,NULL);
	  return 0;
  }
  hResult = m_pGB->QueryInterface(IID_IMediaSeeking,(void**)&m_pMS);  
  if (hResult != S_OK)
  {

	  //MessageBox(hNotifyWnd,L"��֧�ֵ�ý���ʽ",NULL,NULL);
	  return 0;
  }
  GUID guid_timeFormat = TIME_FORMAT_MEDIA_TIME;
  m_pMS->SetTimeFormat(&guid_timeFormat); 

  hResult = m_pGB->QueryInterface(IID_IMediaEventEx,(void**)&m_pME);
  if (hResult != S_OK)
  {

	  //MessageBox(hNotifyWnd,L"��֧�ֵ�ý���ʽ",NULL,NULL);
	  return 0;
  }
  m_pME->SetNotifyWindow((OAHWND)hNotifyWnd, WM_GRAPHNOTIFY, 0);                 //���Ž���ʱ������ܴ��ڷ���һ��������Ϣ Ȼ����Ӧ����Ϣ  

  hResult = m_pGB->QueryInterface(IID_IVideoWindow, (void **)&m_pVW); 
  if (hResult != S_OK)
  {

	  //MessageBox(hNotifyWnd,L"��֧�ֵ�ý���ʽ",NULL,NULL);
	  return 0;
  }
  m_pVW->put_Owner((OAHWND)inWindow);
  m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

  hResult = m_pGB->QueryInterface(IID_IBasicVideo, (void **)&m_pBV);
  if (hResult != S_OK)
  {

	  //MessageBox(hNotifyWnd,L"��֧�ֵ�ý���ʽ",NULL,NULL);
	  return 0;
  }
  hResult = m_pGB->QueryInterface(IID_IBasicAudio, (void **)&m_pBA);  
  if (hResult != S_OK)
  {

	  //MessageBox(hNotifyWnd,L"��֧�ֵ�ý���ʽ",NULL,NULL);
	  return 0;
  }
  
  DshowFitWindow(inWindow);

  DshowSetAudioVolume(m_volume);
  return FLAG;
}


void MediaControl::DshowSetNotifyWindow(HWND hNotifyWnd)
{
 m_pME->SetNotifyWindow((OAHWND)hNotifyWnd, WM_GRAPHNOTIFY, 0); 
}


//����
BOOL MediaControl::DshowPlay() 
{
 //m_pMC->Pause();
	if (m_pMC == NULL)
	{
		return false;
	}
   HRESULT hResult = m_pMC->Run();
   if (hResult != S_OK)
   {
	   return false;
   }
   return TRUE;
}

//��ͣ
BOOL MediaControl::DshowPause() 
{
	if (m_pMC == NULL)
	{
		return FALSE;
	}
  HRESULT hResult = m_pMC->Pause();
  if (hResult != S_OK)
  {
	  return FALSE;
  }
  return TRUE;
}


//ֹͣ
BOOL MediaControl::DshowStop() 
{
	if (m_pMC == NULL)
	{
		return FALSE;
	}
	HRESULT hResult;
 hResult = m_pMC->Stop();
 if (hResult != S_OK)
 {
	 return FALSE;
 }
 LONGLONG pos = 0;
 hResult = m_pMS->SetPositions (&pos, AM_SEEKING_AbsolutePositioning ,     //�ṹ���Աʵ��  API����  �ӿ�ֱ�ӵ���
                                   NULL, AM_SEEKING_NoPositioning);
 return true;
}


//��Ӧ���ڴ�С��ʾ
void MediaControl::DshowFitWindow(HWND inWindow) 
{
    m_hWindow = inWindow;
	RECT rc;
	GetClientRect(inWindow,&rc);
	long lngHeight = 0;
	long lngWidth = 0;
	m_pBV->get_SourceHeight(&lngHeight);//ԭʼ��С
	m_pBV->get_SourceWidth(&lngWidth);
	
	m_pVW->put_Visible(OATRUE);
	//m_pVW->put_Owner((OAHWND)m_hWindow);
	
	m_pVW->put_Left(rc.left );
	m_pVW->put_Top(rc.top );
	m_pVW->put_Width(rc.right -rc.left );
	m_pVW->put_Height(rc.bottom - rc.top );
	m_pVW->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	
	m_pVW->put_MessageDrain((OAHWND)m_hWindow);
}


//ȫ����ʾ
BOOL MediaControl::DshowFullScreen ()
{
    LONG lMode = 0;
	static HWND hDrain=0;

    m_pVW->get_FullScreenMode(&lMode);   //  �õ���Ļ״̬   //ȫ��  Ҳ��APIʵ�ֵ�
	
	if (lMode == OAFALSE)
    {
		// Save current message drain
        //m_pVW->get_MessageDrain((OAHWND *) &hDrain);

        // Set message drain to application main window
        m_pVW->put_MessageDrain((OAHWND)m_hWindow );
		
		//����ȫ��Ļ
        lMode = OATRUE;
        m_pVW->put_FullScreenMode(lMode);
    }
    else
    {
        //�л�����ȫ��ģʽ
        lMode = OAFALSE;
        m_pVW->put_FullScreenMode(lMode);

		DshowFitWindow(m_hWindow);

        m_pVW->SetWindowForeground(-1);
	}
   return true;
}
//��ʼ��С
void MediaControl::DshowInitWindow(HWND inWindow) 
{
    m_hWindow = inWindow;
	RECT rc;
	GetClientRect(inWindow,&rc);
	long lngHeight = 0;
	long lngWidth = 0;
	m_pBV->get_SourceHeight(&lngHeight);//ԭʼ��С
	m_pBV->get_SourceWidth(&lngWidth);
	
	//m_pVW->put_Visible(OATRUE);           //��Ƶ��Ӱ������
	m_pVW->put_Owner((OAHWND)m_hWindow);
	
	m_pVW->put_Left((rc.right -rc.left)/5 );
	m_pVW->put_Top((rc.bottom - rc.top)/5 );
	m_pVW->put_Width((rc.right -rc.left)/5*3 );
	m_pVW->put_Height((rc.bottom - rc.top)/5*3 );
	m_pVW->put_WindowStyle(WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	
	m_pVW->put_MessageDrain((OAHWND)m_hWindow);
}


//ý���¼�
BOOL MediaControl::DshowGetMediaEvent(long *lEventCode)
{
   LONG evCode, evParam1, evParam2;
    HRESULT hr=S_OK;

	if (m_pME == NULL)
	{
		return FALSE;
	}

	hr = m_pME->GetEvent(&evCode, &evParam1, &evParam2, 0);   
    if (SUCCEEDED(hr))
	{
        *lEventCode = evCode;
        hr = m_pME->FreeEventParams(evCode, evParam1, evParam2);
		
		return TRUE;
	}
	return FALSE;
}

//���õ�ǰλ��
BOOL MediaControl::DshowSetPositions(DWORD dwPos)
{
	printf("IN DshowSetPositions dwPos=%d\n",dwPos);
  LONGLONG llPos = dwPos * 10000 * 1000;
	if (m_pMS == NULL)
	{
		return FALSE;
	}
	//������Ƶ���ŵ�ǰλ��
	HRESULT hr = m_pMS->SetPositions(&llPos,AM_SEEKING_AbsolutePositioning ,
                                   NULL, AM_SEEKING_NoPositioning);        
	if (hr == S_OK)  
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


//��õ�ǰλ��
LONGLONG MediaControl::DshowGetCurrentPos(DWORD &dwPos)
{
    dwPos = 0;
	LONGLONG llPos = 0;
	if (m_pMS== NULL)
	{
		return FALSE;
	}
	
	//�õ���ǰ��Ƶ����λ��
	HRESULT hResult = m_pMS->GetCurrentPosition(&llPos);
	if (hResult != S_OK)
	{
		return FALSE;
	}

	//ת��������Ϊ��λ
	llPos = llPos / 10000 ;
	llPos = llPos / 1000;
	dwPos  = (DWORD)llPos;
	return dwPos;
}

//�����ʱ�䳤��
BOOL MediaControl::DshowGetDuration(DWORD &dwLength)
{
 dwLength = 0;
	if (m_pMS == NULL)
	{
		return FALSE;
	}

	//�õ���Ƶ��ʱ�䳤��
	LONGLONG llDuration;
	HRESULT hResult = m_pMS->GetDuration(&llDuration);       //APIʵ�ֵĻ�ó���ʱ��
	if (hResult != S_OK)
	{
		return FALSE;
	}

	//ת��������Ϊ��λ
	llDuration = llDuration / 10000 ;
	llDuration = llDuration / 1000;
	dwLength  = (DWORD)llDuration;
	return TRUE;
}



//��������
BOOL MediaControl::DshowSetAudioVolume(long nValue)
{
 m_volume = nValue;
  if (m_pBA)
	  return SUCCEEDED(m_pBA->put_Volume (nValue));
  return false;
}

//�������
long MediaControl::DshowGetAudioVolume()
{
  long volume = 0;

  if(m_pBA)
  { 
	 m_pBA->get_Volume (&volume);
  }
  
 
 return volume;
}


//��õ�ǰλ��  ����Ϊ��λ
BOOL MediaControl::DshowGetCurrentPosMESL(LONGLONG & dwPos)
{
    dwPos = 0;
	LONGLONG llPos = 0;
	if (m_pMS== NULL)
	{
		return FALSE;
	}
	
	//�õ���ǰ��Ƶ����λ��
	HRESULT hResult = m_pMS->GetCurrentPosition(&llPos);
	if (hResult != S_OK)
	{
		return FALSE;
	}

	
	llPos = llPos / 10000 ;  //��ú���
	dwPos  = llPos;
	return TRUE;
}