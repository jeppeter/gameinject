#ifndef __IMG_CAP_CONTROLLER_H__
#define __IMG_CAP_CONTROLLER_H__

#include <imgcapcommon.h>
#include <Windows.h>
#include <stdint.h>

class CImgCapController
{
public:
    CImgCapController();
    ~CImgCapController();


    BOOL Start(HANDLE hProc,const char* dllname, int iMaxDelay);
    VOID Stop();

    BOOL CapImage(uint8_t *pData, int iLen, int * iFormat, int * iWidth, int * iHeight, int * iTimeStamp, int * pRetLen);
    int GetState();
    int GetOperation();

private:
	BOOL __CapImageWindowed(uint8_t *pData, int iLen, int * iFormat, int * iWidth, int * iHeight, int * iTimeStamp, int * pRetLen);


private:
    HANDLE m_hProc;         // ����ͼ���̾��
    unsigned char m_strDllName[256];
    int m_iState;
	int m_iOperation;
    int m_iMaxDelay;        // ���Խ�ͼ�����ʱ��
    int m_iLastTimeStamp;   // ���һ�ν�ͼʱ���

    uint64_t m_iTotalCapTime;
    uint64_t m_iTotalCapImgNum;
    uint64_t m_iMaxCapTime;
    uint64_t m_iMinCapTime;
};

#endif /*__IMG_CAP_CONTROLLER_H__*/
