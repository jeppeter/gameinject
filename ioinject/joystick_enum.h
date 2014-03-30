
#ifndef __JOYSTICK_ENUM_H__
#define __JOYSTICK_ENUM_H__

class JoyStickBase
{
public:
    JoyStickBase();
    virtual ~JoyStickBase();
    virtual int SetJoyStickConfig(const char* name,void* pConfig,int configlen);
    virtual int
};

class JoyStickConfig8Base
{
public:
    JoyStickConfig8Base();
    virtual ~JoyStickConfig8Base();
    virtual int SetJoyStickConfig(const char* name,void* pConfig,int configlen);
    virtual HRESULT CallEnumTypes(LPDIJOYTYPECALLBACK lpCallback,LPVOID pvRef);
    virtual HRESULT GetTypeInfo(LPCWSTR pwszTypeName,LPDIJOYTYPEINFO pjti,DWORD dwFlags);
    virtual HRESULT SetTypeInfo(LPCWSTR pwszTypeName,LPDIJOYTYPEINFO pjti, DWORD dwFlags);
    virtual HRESULT DeleteType(LPCWSTR pwszTypeName);
	virtual HRESULT GetConfig(UINT uiJoy,LPDIJOYCONFIG pjc,DWORD dwFlags);
	virtual HRESULT SetConfig(UINT idJoy,LPCDIJOYCONFIG pcfg,DWORD dwFlags);
	virtual HRESULT DeleteConfig(UINT idJoy);
	virtual HRESULT GetUserValues(LPDIJOYUSERVALUES pjuv,DWORD dwFlags);
	virtual HRESULT SetUserValues(LPCDIJOYUSERVALUES pjuv,DWORD dwFlags);
	virtual HRESULT AddNewHardware(HWND hwndOwner,REFGUID rguidClass);
	virtual HRESULT OpenTypeKey(LPCWSTR pwszType,REGSAM regsam,PHKEY phk);
	virtual HRESULT OpenConfigKey(UINT idJoy,REGSAM regsam,PHKEY phk);
};

class JoyStickBetopEnum : public JoyStickConfig8Base
{
};

#endif /*__JOYSTICK_ENUM_H__*/

