
#include <iocapcommon.h>
#include <vector>
#include "ioinject_thread.h"
#define  DIRECTINPUT_VERSION 0x800
#include <dinput.h>


#define MAX_HWND_SIZE   20
#define  MAX_STATE_BUFFER_SIZE   256


static CRITICAL_SECTION st_Dinput8KeyMouseStateCS;
static unsigned char st_Dinput8KeyState[MAX_STATE_BUFFER_SIZE];
static DIMOUSESTATE  st_Dinput8MouseState;

static std::vector<HWND> st_hWndVecs;
static std::vector<RECT> st_hWndRectVecs;
static std::vector<DWORD> st_hWndThreadId;
static unsigned int st_KeyDownTimes[256] = {0};
/********************************************
.left 0
.top 0
.right 2
.bottom 2
********************************************/
static RECT st_MaxRect = {0,0,2,2};
static POINT st_MousePoint = { 1,1};
static POINT st_MouseLastPoint = {1,1};
static UINT st_MouseBtnState[MOUSE_MAX_BTN] = {0};
static UINT st_MouseLastBtnState[MOUSE_MAX_BTN] = {0};

#define DIK_NULL  0xff

static int st_CodeMapDik[256] =
{
    DIK_A             ,DIK_B              ,DIK_C              ,DIK_D              ,DIK_E              ,  /*5*/
    DIK_F             ,DIK_G              ,DIK_H              ,DIK_I              ,DIK_J              ,  /*10*/
    DIK_K             ,DIK_L              ,DIK_M              ,DIK_N              ,DIK_O              ,  /*15*/
    DIK_P             ,DIK_Q              ,DIK_R              ,DIK_S              ,DIK_T              ,  /*20*/
    DIK_U             ,DIK_V              ,DIK_W              ,DIK_X              ,DIK_Y              ,  /*25*/
    DIK_Z             ,DIK_0              ,DIK_1              ,DIK_2              ,DIK_3              ,  /*30*/
    DIK_4             ,DIK_5              ,DIK_6              ,DIK_7              ,DIK_8              ,  /*35*/
    DIK_9             ,DIK_ESCAPE         ,DIK_MINUS          ,DIK_EQUALS         ,DIK_BACK           ,  /*40*/
    DIK_TAB           ,DIK_LBRACKET       ,DIK_RBRACKET       ,DIK_RETURN         ,DIK_LCONTROL       ,  /*45*/
    DIK_SEMICOLON     ,DIK_APOSTROPHE     ,DIK_GRAVE          ,DIK_LSHIFT         ,DIK_BACKSLASH      ,  /*50*/
    DIK_COMMA         ,DIK_PERIOD         ,DIK_SLASH          ,DIK_RSHIFT         ,DIK_MULTIPLY       ,  /*55*/
    DIK_LMENU         ,DIK_SPACE          ,DIK_CAPITAL        ,DIK_F1             ,DIK_F2             ,  /*60*/
    DIK_F3            ,DIK_F4             ,DIK_F5             ,DIK_F6             ,DIK_F7             ,  /*65*/
    DIK_F8            ,DIK_F9             ,DIK_F10            ,DIK_F11            ,DIK_F12            ,  /*70*/
    DIK_F13           ,DIK_F14            ,DIK_F15            ,DIK_NUMLOCK        ,DIK_SCROLL         ,  /*75*/
    DIK_SUBTRACT      ,DIK_NUMPAD0        ,DIK_NUMPAD1        ,DIK_NUMPAD2        ,DIK_NUMPAD3        ,  /*80*/
    DIK_NUMPAD4       ,DIK_NUMPAD5        ,DIK_NUMPAD6        ,DIK_NUMPAD7        ,DIK_NUMPAD8        ,  /*85*/
    DIK_NUMPAD9       ,DIK_ADD            ,DIK_DECIMAL        ,DIK_OEM_102        ,DIK_KANA           ,  /*90*/
    DIK_ABNT_C1       ,DIK_CONVERT        ,DIK_NOCONVERT      ,DIK_YEN            ,DIK_ABNT_C2        ,  /*95*/
    DIK_NUMPADEQUALS  ,DIK_PREVTRACK      ,DIK_AT             ,DIK_COLON          ,DIK_UNDERLINE      ,  /*100*/
    DIK_KANJI         ,DIK_STOP           ,DIK_AX             ,DIK_UNLABELED      ,DIK_NEXTTRACK      ,  /*105*/
    DIK_NUMPADENTER   ,DIK_RCONTROL       ,DIK_MUTE           ,DIK_CALCULATOR     ,DIK_PLAYPAUSE      ,  /*110*/
    DIK_MEDIASTOP     ,DIK_VOLUMEDOWN     ,DIK_VOLUMEUP       ,DIK_WEBHOME        ,DIK_NUMPADCOMMA    ,  /*115*/
    DIK_DIVIDE        ,DIK_SYSRQ          ,DIK_RMENU          ,DIK_PAUSE          ,DIK_HOME           ,  /*120*/
    DIK_UP            ,DIK_PRIOR          ,DIK_LEFT           ,DIK_RIGHT          ,DIK_END            ,  /*125*/
    DIK_DOWN          ,DIK_NEXT           ,DIK_INSERT         ,DIK_DELETE         ,DIK_LWIN           ,  /*130*/
    DIK_RWIN          ,DIK_APPS           ,DIK_POWER          ,DIK_SLEEP          ,DIK_WAKE           ,  /*135*/
    DIK_WEBSEARCH     ,DIK_WEBFAVORITES   ,DIK_WEBREFRESH     ,DIK_WEBSTOP        ,DIK_WEBFORWARD     ,  /*140*/
    DIK_WEBBACK       ,DIK_MYCOMPUTER     ,DIK_MAIL           ,DIK_MEDIASELECT    ,DIK_NULL           ,  /*144*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*150*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*155*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*160*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*165*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*170*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*175*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*180*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*185*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*190*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*195*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*200*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*205*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*210*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*215*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*220*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*225*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*230*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*235*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*240*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*245*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*250*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*255*/
    DIK_NULL
};



int __DetourDinputPressKeyDownNoLock(UINT scancode)
{
    int cnt;

    if(scancode >= 256)
    {
        cnt = ERROR_INVALID_PARAMETER;
        SetLastError(cnt);
        return -cnt;
    }
    st_KeyDownTimes[scancode] ++;
    cnt = st_KeyDownTimes[scancode];
    return cnt;
}

int __DetourDinputPressKeyUpNoLock(UINT scancode)
{
    int cnt;

    if(scancode >= 256)
    {
        cnt = ERROR_INVALID_PARAMETER;
        SetLastError(cnt);
        return -cnt;
    }
    st_KeyDownTimes[scancode] =0;
    cnt = 0;
    return cnt;

}



int DetourDinputPressKeyDownTimes(UINT scancode)
{
    int cnt;

    if(scancode >= 256)
    {
        cnt = ERROR_INVALID_PARAMETER;
        SetLastError(cnt);
        return -cnt;
    }
    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    cnt = st_KeyDownTimes[scancode];
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
    return cnt;
}



int __ReCalculateMaxWindowRectNoLock()
{
    UINT i;
    int pickidx=0;
    LONG hw,hh,mw,mh;

    if(st_hWndVecs.size() == 0)
    {
        st_MaxRect.top = 0;
        st_MaxRect.left = 0;
        st_MaxRect.right = 2;
        st_MaxRect.bottom = 2;
        return 0;
    }

    assert(st_hWndVecs.size() > 0);
    for(i=0; i<st_hWndVecs.size() ; i++)
    {
        hw = (st_hWndRectVecs[i].right - st_hWndRectVecs[i].left);
        hh = (st_hWndRectVecs[i].bottom - st_hWndRectVecs[i].top);
        mw = (st_hWndRectVecs[pickidx].right - st_hWndRectVecs[pickidx].left);
        mh = (st_hWndRectVecs[pickidx].bottom - st_hWndRectVecs[pickidx].top);
        if((hw > mw) &&
                (hh > mh))
        {
            DEBUG_INFO("picked %d\n",i);
            pickidx = i;
        }
    }

    /*now max window size is rect*/
    CopyMemory(&st_MaxRect,&(st_hWndRectVecs[pickidx]),sizeof(st_MaxRect));

    if(st_MaxRect.left < 0)
    {
        st_MaxRect.left = 0;
    }

    if(st_MaxRect.right < 0 || st_MaxRect.right <= st_MaxRect.left)
    {
        st_MaxRect.right = st_MaxRect.left + 1;
    }

    if(st_MaxRect.top < 0)
    {
        st_MaxRect.top = 0;
    }

    if(st_MaxRect.bottom < 0 || st_MaxRect.bottom <= st_MaxRect.top)
    {
        st_MaxRect.bottom = st_MaxRect.top + 1;
    }


    return pickidx;
}

int __ReCalculateMousePointNoLock(int check)
{
    //DEBUG_INFO("st_MousePoint (%d-%d) maxrect (%d-%d:%d-%d)\n",
    //           st_MousePoint.x,st_MousePoint.y,
    //           st_MaxRect.left,st_MaxRect.top,
    //           st_MaxRect.right,st_MaxRect.bottom);
    if(st_MousePoint.x <= st_MaxRect.left)
    {
        st_MousePoint.x = st_MaxRect.left + 1;
    }
    else if(st_MousePoint.x >= st_MaxRect.right)
    {
        st_MousePoint.x = st_MaxRect.right - 1;
    }

    if(st_MousePoint.y  <= st_MaxRect.top)
    {
        DEBUG_INFO("reset y (%d + 1)\n",st_MaxRect.top);
        st_MousePoint.y = st_MaxRect.top + 1;
    }
    else if(st_MousePoint.y >= st_MaxRect.bottom)
    {
        DEBUG_INFO("reset y (%d - 1)\n",st_MaxRect.bottom);
        st_MousePoint.y = st_MaxRect.bottom - 1;
    }

    if(st_MousePoint.x < 1)
    {
        st_MousePoint.x = 1;
    }

    if(st_MousePoint.y < 1)
    {
        st_MousePoint.y = 1;
    }

    if(check && (st_MousePoint.x != st_MouseLastPoint.x ||
                 st_MousePoint.y != st_MouseLastPoint.y))
    {
        ERROR_INFO("Should MousePoint(x:%d:y:%d) But MousePoint(x:%d:y:%d)\n",
                   st_MousePoint.x,
                   st_MousePoint.y,
                   st_MouseLastPoint.x,
                   st_MouseLastPoint.y);
    }

    /*now we should make sure the point for the last mouse point*/
    if(st_MouseLastPoint.x != st_MousePoint.x)
    {
        st_Dinput8MouseState.lX += (st_MousePoint.x - st_MouseLastPoint.x);
    }

    if(st_MouseLastPoint.y != st_MousePoint.y)
    {
        st_Dinput8MouseState.lY += (st_MousePoint.y - st_MouseLastPoint.y);
    }

    st_MouseLastPoint.x = st_MousePoint.x;
    st_MouseLastPoint.y = st_MousePoint.y;

    return 0;
}

int __MoveMouseRelativeNoLock(int x,int y)
{
    int ret= 0;
    st_MousePoint.x += x;
    st_MousePoint.y += y;
    __ReCalculateMousePointNoLock(0);
    return 0;
}

int __MoveMouseAbsoluteNoLock(int clientx,int clienty)
{
    int ret = 0;
    st_MousePoint.x = st_MaxRect.left + clientx;
    st_MousePoint.y = st_MaxRect.top + clienty;
    __ReCalculateMousePointNoLock(1);
    return ret;
}


int __SetDetourDinputMouseBtnNoLock(UINT btn,int down)
{
    int ret;

    if(btn > MOUSE_MAX_BTN || btn < MOUSE_MIN_BTN)
    {
        ret = ERROR_INVALID_PARAMETER;
        SetLastError(ret);
        return -ret;
    }

    if(down)
    {
        st_MouseBtnState[btn - 1] = 0x1;
    }
    else
    {
        st_MouseBtnState[ btn - 1] = 0x0;
    }


    return 0;
}

static int Dinput8CreateWindowNotify(LPVOID pParam,LPVOID pInput)
{
    HWND hwnd = (HWND) pInput;
    int ret=0;
    RECT rect= {0,0,0,0};

    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    st_hWndRectVecs.push_back(rect);
    st_hWndVecs.push_back(hwnd);
    st_hWndThreadId.push_back(GetCurrentThreadId());
    __ReCalculateMaxWindowRectNoLock();
    __ReCalculateMousePointNoLock(0);
    ret = 1;
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
    return ret;
}


static int Dinput8DestroyWindowNotify(LPVOID pParam,LPVOID pInput)
{
    HANDLE hwnd = (HANDLE) pInput;
    int ret = 0;
    UINT i;
    int findidx = -1;

    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    for(i=0; i<st_hWndVecs.size() ; i++)
    {
        if(st_hWndVecs[i] == hwnd)
        {
            findidx = i;
            break;
        }
    }
    if(findidx >= 0)
    {
        st_hWndRectVecs.erase(st_hWndRectVecs.begin() + findidx);
        st_hWndVecs.erase(st_hWndVecs.begin() + findidx);
        st_hWndThreadId.erase(st_hWndThreadId.begin() + findidx);
        ret = 1;
    }
    else
    {
        ERROR_INFO("hwnd (0x%08x) not register in the windows size\n",hwnd);
    }
    __ReCalculateMaxWindowRectNoLock();
    __ReCalculateMousePointNoLock(0);
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
    return ret;
}


int Dinput8SetWindowRectState(HWND hwnd)
{
    int findidx = -1;
    UINT i;
    int refreshed = 0;
    BOOL bret;
    RECT rRect,wRect;
    POINT pt;
    int ret;
    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);

    for(i=0; i<st_hWndVecs.size() ; i++)
    {
        if(hwnd == st_hWndVecs[i])
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        bret = ::GetClientRect(hwnd,&rRect);
        if(bret)
        {
            pt.x = rRect.left;
            pt.y = rRect.top;
            bret = ::ClientToScreen(hwnd,&pt);
            if(!bret)
            {
                ret = LAST_ERROR_CODE();
                ERROR_INFO("ClientToScreen(%d-%d) Error(%d)\n",pt.x,pt.y,ret);
                goto unlock;
            }

            wRect.left = pt.x;
            wRect.top = pt.y;

            pt.x = rRect.right;
            pt.y = rRect.bottom;
            bret = ::ClientToScreen(hwnd,&pt);
            if(!bret)
            {
                ret = LAST_ERROR_CODE();
                ERROR_INFO("ClientToScreen(%d-%d) Error(%d)\n",pt.x,pt.y,ret);
                goto unlock;
            }

            wRect.right = pt.x;
            wRect.bottom = pt.y;
            if(st_hWndRectVecs[findidx].top != wRect.top  ||
                    st_hWndRectVecs[findidx].left != wRect.left ||
                    st_hWndRectVecs[findidx].right != wRect.right ||
                    st_hWndRectVecs[findidx].bottom != wRect.bottom)
            {
                DEBUG_INFO("hwnd(0x%08x) (%d:%d)=>(%d:%d) Set (%d:%d)=>(%d:%d) wRect(%d:%d)=>(%d:%d)\n",
                           st_hWndVecs[findidx],
                           st_hWndRectVecs[findidx].left,
                           st_hWndRectVecs[findidx].top,
                           st_hWndRectVecs[findidx].right,
                           st_hWndRectVecs[findidx].bottom,
                           rRect.left,
                           rRect.top,
                           rRect.right,
                           rRect.bottom,
                           wRect.left,
                           wRect.top,
                           wRect.right,
                           wRect.bottom);
                st_hWndRectVecs[findidx] = wRect;
                refreshed ++;
            }
        }
    }
    else
    {
        for(i=0; i<st_hWndVecs.size(); i++)
        {
            bret = ::GetClientRect(st_hWndVecs[i],&rRect);
            if(bret)
            {
                pt.x = rRect.left;
                pt.y = rRect.top;
                bret = ::ClientToScreen(st_hWndVecs[i],&pt);
                if(!bret)
                {
                    ret = LAST_ERROR_CODE();
                    ERROR_INFO("ClientToScreen(%d-%d) Error(%d)\n",pt.x,pt.y,ret);
                    continue;
                }

                wRect.left = pt.x;
                wRect.top = pt.y;

                pt.x = rRect.right;
                pt.y = rRect.bottom;
                bret = ::ClientToScreen(st_hWndVecs[i],&pt);
                if(!bret)
                {
                    ret = LAST_ERROR_CODE();
                    ERROR_INFO("ClientToScreen(%d-%d) Error(%d)\n",pt.x,pt.y,ret);
                    continue;
                }

                wRect.right = pt.x;
                wRect.bottom = pt.y;
                if(st_hWndRectVecs[i].top != wRect.top  ||
                        st_hWndRectVecs[i].left != wRect.left ||
                        st_hWndRectVecs[i].right != wRect.right ||
                        st_hWndRectVecs[i].bottom != wRect.bottom)
                {
                    DEBUG_INFO("hwnd(0x%08x) (%d:%d)=>(%d:%d) Set (%d:%d)=>(%d:%d)  wRect(%d:%d)=>(%d:%d)\n",
                               st_hWndVecs[i],
                               st_hWndRectVecs[i].left,
                               st_hWndRectVecs[i].top,
                               st_hWndRectVecs[i].right,
                               st_hWndRectVecs[i].bottom,
                               rRect.left,
                               rRect.top,
                               rRect.right,
                               rRect.bottom,
                               wRect.left,
                               wRect.top,
                               wRect.right,
                               wRect.bottom);
                    st_hWndRectVecs[i] = wRect;
                    GetWindowRect(st_hWndVecs[i],&wRect);
                    DEBUG_INFO("hwnd(0x%08x) wRect(%d:%d)=>(%d:%d)\n",
                               st_hWndVecs[i],
                               wRect.left,
                               wRect.top,
                               wRect.right,
                               wRect.bottom);
                    refreshed ++;
                }
            }
        }
    }
unlock:
    if(refreshed > 0)
    {
        /*we have refreshed window ,so recalculate the window*/
        __ReCalculateMaxWindowRectNoLock();
        __ReCalculateMousePointNoLock(0);
    }
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
    return refreshed;
}



int __DetourDinput8SetKeyStateNoLock(LPDEVICEEVENT pDevEvent)
{
    int ret;
    int scancode;

    if(pDevEvent->devid != 0)
    {
        ret = ERROR_DEV_NOT_EXIST;
        ERROR_INFO("<0x%p>Keyboard devid(%d) invalid\n",pDevEvent,pDevEvent->devid);
        SetLastError(ret);
        return -ret;
    }

    /*not check for the code*/

    if(pDevEvent->event.keyboard.code >= KEYBOARD_CODE_NULL)
    {
        ret= ERROR_INVALID_PARAMETER;
        ERROR_INFO("<0x%p>Keyboard code(%d) invalid\n",pDevEvent,pDevEvent->event.keyboard.code);
        SetLastError(ret);
        return -ret;
    }

    if(pDevEvent->event.keyboard.event >= KEYBOARD_EVENT_MAX)
    {
        ret= ERROR_INVALID_PARAMETER;
        ERROR_INFO("<0x%p>Keyboard event(%d) invalid\n",pDevEvent,pDevEvent->event.keyboard.event);
        SetLastError(ret);
        return -ret;
    }


    scancode = st_CodeMapDik[pDevEvent->event.keyboard.code];
    if(scancode == DIK_NULL)
    {
        ret= ERROR_INVALID_PARAMETER;
        ERROR_INFO("<0x%p>Keyboard code(%d) TO DIK_NULL invalid\n",pDevEvent,pDevEvent->event.keyboard.code);
        SetLastError(ret);
        return -ret;
    }

    if(pDevEvent->event.keyboard.event == KEYBOARD_EVENT_DOWN)
    {
        st_Dinput8KeyState[scancode] = 0x80;
        __DetourDinputPressKeyDownNoLock(scancode);
    }
    else
    {
        st_Dinput8KeyState[scancode] = 0x00;
        __DetourDinputPressKeyUpNoLock(scancode);
    }
    return 0;
}

int __DetourDinput8SetMouseStateNoLock(LPDEVICEEVENT pDevEvent)
{
    int ret;

    if(pDevEvent->devid != 0)
    {
        ret = ERROR_DEV_NOT_EXIST;
        ERROR_INFO("<0x%p>Mouse devid(%d) invalid\n",pDevEvent,pDevEvent->devid);
        SetLastError(ret);
        return -ret;
    }

    if(pDevEvent->event.mouse.code >= MOUSE_CODE_MAX)
    {
        ret = ERROR_INVALID_PARAMETER;
        ERROR_INFO("<0x%p>Mouse code(%d) invalid\n",pDevEvent,pDevEvent->event.mouse.code);
        SetLastError(ret);
        return -ret;
    }

    if(pDevEvent->event.mouse.event >= MOUSE_EVENT_MAX)
    {
        ret = ERROR_INVALID_PARAMETER;
        ERROR_INFO("<0x%p>Mouse event(%d) invalid\n",pDevEvent,pDevEvent->event.mouse.event);
        SetLastError(ret);
        return -ret;
    }


    if(pDevEvent->event.mouse.code == MOUSE_CODE_MOUSE)
    {
        if(pDevEvent->event.mouse.event == MOUSE_EVNET_MOVING)
        {
            /*this is relative one*/
            __MoveMouseRelativeNoLock(pDevEvent->event.mouse.x,pDevEvent->event.mouse.y);
            //DEBUG_INFO("x %d y %d mousepoint(%d:%d)\n",pDevEvent->event.mouse.x,pDevEvent->event.mouse.y,
            //           st_MousePoint.x,st_MousePoint.y);
        }
        else if(pDevEvent->event.mouse.event ==  MOUSE_EVENT_SLIDE)
        {
            st_Dinput8MouseState.lZ += pDevEvent->event.mouse.x;
        }
        else if(pDevEvent->event.mouse.event == MOUSE_EVENT_ABS_MOVING)
        {
            __MoveMouseAbsoluteNoLock(pDevEvent->event.mouse.x,pDevEvent->event.mouse.y);
        }
        else
        {
            ret = ERROR_INVALID_PARAMETER;
            ERROR_INFO("<0x%p>Mouse Invalid code(%d) event(%d)\n",pDevEvent,
                       pDevEvent->event.mouse.code,
                       pDevEvent->event.mouse.event);
            SetLastError(ret);
            return -ret;
        }
    }
    else if(pDevEvent->event.mouse.code == MOUSE_CODE_LEFTBUTTON)
    {
        if(pDevEvent->event.mouse.event == MOUSE_EVENT_KEYDOWN)
        {
            st_Dinput8MouseState.rgbButtons[MOUSE_LEFT_BTN - 1] = 0x80;
            __SetDetourDinputMouseBtnNoLock(MOUSE_LEFT_BTN,1);
            DEBUG_INFO("MouseLeftDown Point(%d:%d)\n",st_MousePoint.x,st_MousePoint.y);
        }
        else if(pDevEvent->event.mouse.event == MOUSE_EVENT_KEYUP)
        {
            st_Dinput8MouseState.rgbButtons[MOUSE_LEFT_BTN - 1] = 0x0;
            __SetDetourDinputMouseBtnNoLock(MOUSE_LEFT_BTN,0);
            DEBUG_INFO("MouseLeftUp Point(%d:%d)\n",st_MousePoint.x,st_MousePoint.y);
        }
        else
        {
            ret = ERROR_INVALID_PARAMETER;
            ERROR_INFO("<0x%p>Mouse Invalid code(%d) event(%d)\n",pDevEvent,
                       pDevEvent->event.mouse.code,
                       pDevEvent->event.mouse.event);
            SetLastError(ret);
            return -ret;
        }
    }
    else if(pDevEvent->event.mouse.code == MOUSE_CODE_RIGHTBUTTON)
    {
        if(pDevEvent->event.mouse.event == MOUSE_EVENT_KEYDOWN)
        {
            st_Dinput8MouseState.rgbButtons[MOUSE_RIGHT_BTN - 1] = 0x80;
            __SetDetourDinputMouseBtnNoLock(MOUSE_RIGHT_BTN,1);
        }
        else if(pDevEvent->event.mouse.event == MOUSE_EVENT_KEYUP)
        {
            st_Dinput8MouseState.rgbButtons[MOUSE_RIGHT_BTN - 1] = 0x0;
            __SetDetourDinputMouseBtnNoLock(MOUSE_RIGHT_BTN,0);
        }
        else
        {
            ret = ERROR_INVALID_PARAMETER;
            ERROR_INFO("<0x%p>Mouse Invalid code(%d) event(%d)\n",pDevEvent,
                       pDevEvent->event.mouse.code,
                       pDevEvent->event.mouse.event);
            SetLastError(ret);
            return -ret;
        }
    }
    else if(pDevEvent->event.mouse.code == MOUSE_CODE_MIDDLEBUTTON)
    {
        if(pDevEvent->event.mouse.event == MOUSE_EVENT_KEYDOWN)
        {
            st_Dinput8MouseState.rgbButtons[MOUSE_MIDDLE_BTN - 1] = 0x80;
            __SetDetourDinputMouseBtnNoLock(MOUSE_MIDDLE_BTN,1);
        }
        else if(pDevEvent->event.mouse.event == MOUSE_EVENT_KEYUP)
        {
            st_Dinput8MouseState.rgbButtons[MOUSE_MIDDLE_BTN - 1] = 0x0;
            __SetDetourDinputMouseBtnNoLock(MOUSE_MIDDLE_BTN,0);
        }
        else
        {
            ret = ERROR_INVALID_PARAMETER;
            ERROR_INFO("<0x%p>Mouse Invalid code(%d) event(%d)\n",pDevEvent,
                       pDevEvent->event.mouse.code,
                       pDevEvent->event.mouse.event);
            SetLastError(ret);
            return -ret;
        }
    }
    else
    {
        /*we check before*/
        assert(0!=0);
    }

    return 0;
}

int __DetourDinput8Init(void)
{
	ZeroMemory(st_Dinput8KeyState,sizeof(st_Dinput8KeyState));	
	ZeroMemory(&st_Dinput8MouseState,sizeof(st_Dinput8MouseState));
	ZeroMemory(st_KeyDownTimes,sizeof(st_KeyDownTimes));
	return 0;
}

int DetourDinput8Init(LPVOID pParam,LPVOID pInput)
{
	int ret=0;	

	EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
	ret = __DetourDinput8Init();
	LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
	return ret;
}

int DetourDinput8SetKeyMouseState(LPVOID pParam,LPVOID pInput)
{
    int ret;
    LPDEVICEEVENT pDevEvent=(LPDEVICEEVENT)pInput;
    if(pDevEvent->devtype != DEVICE_TYPE_KEYBOARD &&
            pDevEvent->devtype != DEVICE_TYPE_MOUSE)
    {
        ret = ERROR_NOT_SUPPORTED;
        ERROR_INFO("<0x%p> Not Supported devtype(%d)\n",pDevEvent,pDevEvent->devtype);
        SetLastError(ret);
        return -ret;
    }

    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    if(pDevEvent->devtype == DEVICE_TYPE_KEYBOARD)
    {
        ret= __DetourDinput8SetKeyStateNoLock(pDevEvent);
    }
    else if(pDevEvent->devtype == DEVICE_TYPE_MOUSE)
    {
        ret=  __DetourDinput8SetMouseStateNoLock(pDevEvent);
    }
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);

    if(ret != 0)
    {
        ret = LAST_ERROR_CODE();
        SetLastError(ret);
    }
    return -ret;
}

int DetourDinputMouseBtnDown(UINT btn)
{
    int ret;

    if(btn > MOUSE_MAX_BTN || btn < MOUSE_MIN_BTN)
    {
        ret = ERROR_INVALID_PARAMETER;
        SetLastError(ret);
        return -ret;
    }
    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    ret = st_MouseBtnState[btn - 1];
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);

    return ret;
}

int DetourDinputSetWindowsRect(HWND hWnd)
{
    return Dinput8SetWindowRectState(hWnd);
}


int DetourDinputScreenMousePoint(HWND hwnd,POINT* pPoint)
{
    /*we test for the client point of this window*/
    UINT i;
    int findidx = -1;
    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    /*now first to make sure */
    for(i=0; i<st_hWndVecs.size(); i++)
    {
        if(st_hWndVecs[i] == hwnd)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        /*now to make sure this mouse point is in the kernel*/
        if(st_MousePoint.x > st_hWndRectVecs[findidx].left && st_MousePoint.x < st_hWndRectVecs[findidx].right &&
                st_MousePoint.y > st_hWndRectVecs[findidx].top && st_MousePoint.y < st_hWndRectVecs[findidx].bottom)
        {
            pPoint->x = (st_MousePoint.x - st_hWndRectVecs[findidx].left);
            pPoint->y = (st_MousePoint.y - st_hWndRectVecs[findidx].top);
        }
        else
        {
            ERROR_INFO("Mouse(x:%d:y:%d) [%d] Rect(top-left:%d-%d  bottom-right:%d-%d)\n",
                       st_MousePoint.x,st_MousePoint.y,
                       findidx,
                       st_hWndRectVecs[findidx].top,
                       st_hWndRectVecs[findidx].left,
                       st_hWndRectVecs[findidx].bottom,
                       st_hWndRectVecs[findidx].right);
            if(st_MousePoint.x <= st_hWndRectVecs[findidx].left)
            {
                pPoint->x = 1;
            }
            else if(st_MousePoint.x >= st_hWndRectVecs[findidx].right)
            {
                pPoint->x = (st_hWndRectVecs[findidx].right - st_hWndRectVecs[findidx].left - 1);
            }
            else
            {
                pPoint->x = (st_MousePoint.x - st_hWndRectVecs[findidx].left);
            }

            if(st_MousePoint.y <= st_hWndRectVecs[findidx].top)
            {
                pPoint->y = 1;
            }
            else if(st_MousePoint.y >= st_hWndRectVecs[findidx].bottom)
            {
                pPoint->y = (st_hWndRectVecs[findidx].bottom - st_hWndRectVecs[findidx].top - 1);
            }
            else
            {
                pPoint->y = (st_MousePoint.y -st_hWndRectVecs[findidx].top);
            }
        }
    }
    else
    {
        /*not find ,so we put it in the top-left point for the max rect*/
        if(st_MousePoint.x <= st_MaxRect.left)
        {
            pPoint->x = 1;
        }
        else if(st_MousePoint.x >= st_MaxRect.right)
        {
            pPoint->x = (st_MaxRect.right - st_MaxRect.left -1);
        }
        else
        {
            pPoint->x = (st_MousePoint.x - st_MaxRect.left);
        }

        if(st_MousePoint.y <= st_MaxRect.top)
        {
            pPoint->y = 1;
        }
        else if(st_MousePoint.y >= st_MaxRect.bottom)
        {
            pPoint->y = (st_MaxRect.bottom - st_MaxRect.top - 1);
        }
        else
        {
            pPoint->y = (st_MousePoint.y - st_MaxRect.top);
        }
    }
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);

    return 0;
}



int DetourDinput8GetMousePointAbsolution(POINT *pPoint)
{
    int ret=0;

    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    pPoint->x = st_MousePoint.x;
    pPoint->y = st_MousePoint.y;
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
    return ret;
}

int __CopyDiMouseState(PVOID pData, UINT cbSize)
{
    int ret=0;
    if(cbSize < sizeof(st_Dinput8MouseState))
    {
        ret=  ERROR_INSUFFICIENT_BUFFER;
        SetLastError(ret);
        return -ret;
    }

    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    CopyMemory(pData,&st_Dinput8MouseState,sizeof(st_Dinput8MouseState));
    /*to clear the relative moving*/
    st_Dinput8MouseState.lX = 0;
    st_Dinput8MouseState.lY = 0;
    st_Dinput8MouseState.lZ = 0;
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
    return sizeof(st_Dinput8MouseState);
}

int __CopyDiKeyState(PVOID pData,UINT cbSize)
{
    int ret=0;
    if(cbSize < sizeof(st_Dinput8KeyState))
    {
        ret=  ERROR_INSUFFICIENT_BUFFER;
        SetLastError(ret);
        return -ret;
    }

    EnterCriticalSection(&st_Dinput8KeyMouseStateCS);
    CopyMemory(pData,st_Dinput8KeyState,sizeof(st_Dinput8KeyState));
    LeaveCriticalSection(&st_Dinput8KeyMouseStateCS);
    return sizeof(st_Dinput8KeyState);
}


/***************************************************
*  COM api for DirectInput8
*
***************************************************/


class CDirectInputDevice8AHook;
class CDirectInputDevice8WHook;



static std::vector<IDirectInputDevice8A*> st_Key8AVecs;
static std::vector<CDirectInputDevice8AHook*> st_Key8AHookVecs;
static std::vector<IDirectInputDevice8A*> st_Mouse8AVes;
static std::vector<CDirectInputDevice8AHook*> st_Mouse8AHookVecs;
static std::vector<IDirectInputDevice8A*> st_NotSet8AVecs;
static std::vector<CDirectInputDevice8AHook*> st_NotSet8AHookVecs;


static std::vector<IDirectInputDevice8W*> st_Key8WVecs;
static std::vector<CDirectInputDevice8WHook*> st_Key8WHookVecs;
static std::vector<IDirectInputDevice8W*> st_Mouse8WVecs;
static std::vector<CDirectInputDevice8WHook*> st_Mouse8WHookVecs;
static std::vector<IDirectInputDevice8W*> st_NotSet8WVecs;
static std::vector<CDirectInputDevice8WHook*> st_NotSet8WHookVecs;
static CRITICAL_SECTION st_Dinput8DeviceCS;

#define IS_IID_MOUSE(riid)  ( (riid)	== GUID_SysMouse ||(riid) == GUID_SysMouseEm ||(riid) == GUID_SysMouseEm2 )
#define IS_IID_KEYBOARD(riid) ((riid) == GUID_SysKeyboard ||(riid) == GUID_SysKeyboardEm ||(riid) == GUID_SysKeyboardEm2)

#define EQUAL_DEVICE_8A_VECS() \
do\
{\
	assert(st_Key8AVecs.size() == st_Key8AHookVecs.size());\
	assert(st_Mouse8AVes.size() == st_Mouse8AHookVecs.size());\
	assert(st_NotSet8AVecs.size() == st_NotSet8AHookVecs.size());\
}while(0)


#define  EQUAL_DEVICE_8W_VECS()  \
do\
{\
	assert(st_Key8WVecs.size() == st_Key8WHookVecs.size());\
	assert(st_Mouse8WVecs.size() == st_Mouse8WHookVecs.size());\
	assert(st_NotSet8WVecs.size() == st_NotSet8WHookVecs.size());\
}while(0)

ULONG UnRegisterDirectInputDevice8AHook(IDirectInputDevice8A* ptr)
{
    int findidx = -1;
    ULONG uret=1;
    unsigned int i;

    EnterCriticalSection(&st_Dinput8DeviceCS);
    EQUAL_DEVICE_8A_VECS();

    findidx = -1;
    for(i=0; i<st_Key8AVecs.size() ; i++)
    {
        if(st_Key8AVecs[i] == ptr)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_Key8AHookVecs.erase(st_Key8AHookVecs.begin() + findidx);
        st_Key8AVecs.erase(st_Key8AVecs.begin() + findidx);
    }
    else
    {
        findidx = -1;
        for(i=0; i<st_Mouse8AVes.size() ; i++)
        {
            if(st_Mouse8AVes[i] == ptr)
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            st_Mouse8AVes.erase(st_Mouse8AVes.begin() + findidx);
            st_Mouse8AHookVecs.erase(st_Mouse8AHookVecs.begin() + findidx);
        }
        else
        {
            findidx = -1;
            for(i=0; i<st_NotSet8AVecs.size(); i++)
            {
                if(st_NotSet8AVecs[i] == ptr)
                {
                    findidx = i;
                    break;
                }
            }

            if(findidx >= 0)
            {
                st_NotSet8AVecs.erase(st_NotSet8AVecs.begin() + findidx);
                st_NotSet8AHookVecs.erase(st_NotSet8AHookVecs.begin() + findidx);
            }
            else
            {
                ERROR_INFO("<0x%p> not found for dinput device\n",ptr);
            }
        }
    }
    LeaveCriticalSection(&st_Dinput8DeviceCS);


    uret = 1;
    if(findidx >= 0)
    {
        uret = ptr->Release();
    }
    return uret;
}






#define  DIRECT_INPUT_DEVICE_8A_IN()  do{DINPUT_DEBUG_INFO("Device8A::%s 0x%p in\n",__FUNCTION__,this->m_ptr);}while(0)
#define  DIRECT_INPUT_DEVICE_8A_OUT()  do{DINPUT_DEBUG_INFO("Device8A::%s 0x%p out\n",__FUNCTION__,this->m_ptr);}while(0)


#define  SET_BIT(c)  ((c)=0x80)
#define  CLEAR_BIT(c)  ((c)=0x00)


class CDirectInputDevice8AHook : public IDirectInputDevice8A
{
private:
    IDirectInputDevice8A* m_ptr;
    IID m_iid;
private:
    int __IsMouseDevice()
    {
        int ret = 0;
        if(IS_IID_MOUSE((this->m_iid)))
        {
            ret = 1;
        }

        return ret;
    }

    int __IsKeyboardDevice()
    {
        int ret = 0;
        if(IS_IID_KEYBOARD(this->m_iid))
        {
            ret = 1;
        }
        return ret;
    }


public:
    CDirectInputDevice8AHook(IDirectInputDevice8A* ptr,REFIID riid) : m_ptr(ptr)
    {
        m_iid = riid;
    };

    ~CDirectInputDevice8AHook()
    {
        m_iid = IID_NULL;
    }




public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }
    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_DEVICE_8A_IN();
        uret = m_ptr->AddRef();
        DIRECT_INPUT_DEVICE_8A_OUT();
        return uret;
    }
    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_DEVICE_8A_IN();
        uret = m_ptr->Release();
        DIRECT_INPUT_DEVICE_8A_OUT();
        if(uret == 1)
        {
            uret = UnRegisterDirectInputDevice8AHook(this->m_ptr);
            if(uret == 0)
            {
                delete this;
            }
        }
        return uret;
    }

    COM_METHOD(HRESULT,GetCapabilities)(THIS_ LPDIDEVCAPS lpDIDevCaps)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetCapabilities(lpDIDevCaps);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->EnumObjects(lpCallback,pvRef,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetProperty)(THIS_ REFGUID rguidProp,LPDIPROPHEADER pdiph)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetProperty(rguidProp,pdiph);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetProperty)(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->SetProperty(rguidProp,pdiph);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Acquire)(THIS)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->Acquire();
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,Unacquire)(THIS)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->Unacquire();
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceState)(THIS_ DWORD cbData,LPVOID lpvData)
    {
        HRESULT hr;
        int ret;
        DIRECT_INPUT_DEVICE_8A_IN();
        if(this->__IsMouseDevice())
        {
            if(lpvData == NULL)
            {
                ret = ERROR_INVALID_PARAMETER;
                hr = DIERR_INVALIDPARAM;
                SetLastError(ret);
            }
            else
            {
                ret=  __CopyDiMouseState(lpvData,cbData);
                if(ret < 0)
                {
                    hr = DIERR_INVALIDPARAM;
                }
                else
                {
                    hr = DI_OK;
                }
            }
        }
        else if(this->__IsKeyboardDevice())
        {
            if(lpvData == NULL)
            {
                ret = ERROR_INVALID_PARAMETER;
                hr = DIERR_INVALIDPARAM;
                SetLastError(ret);
            }
            else
            {
                ret=  __CopyDiKeyState(lpvData,cbData);
                if(ret < 0)
                {
                    hr = DIERR_INVALIDPARAM;
                }
                else
                {
                    hr = DI_OK;
                }
            }
        }
        else
        {
            hr = this->m_ptr->GetDeviceState(cbData,lpvData);
        }
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceData)(THIS_ DWORD cbObjectData,LPDIDEVICEOBJECTDATA rgdod,LPDWORD pdwInOut,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetDeviceData(cbObjectData,rgdod,pdwInOut,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetDataFormat)(THIS_ LPCDIDATAFORMAT lpdf)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->SetDataFormat(lpdf);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetEventNotification)(THIS_ HANDLE hEvent)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->SetEventNotification(hEvent);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetCooperativeLevel)(THIS_ HWND hwnd,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->SetCooperativeLevel(hwnd,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEA pdidoi,DWORD dwObj,DWORD dwHow)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetObjectInfo(pdidoi,dwObj,dwHow);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEA pdidi)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetDeviceInfo(pdidi);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,RunControlPanel)(THIS_ HWND hwndOwner,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->RunControlPanel(hwndOwner,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Initialize)(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->Initialize(hinst,dwVersion,rguid);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateEffect)(THIS_ REFGUID rguid,LPCDIEFFECT lpeff,LPDIRECTINPUTEFFECT * ppdeff,LPUNKNOWN punkOuter)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->CreateEffect(rguid,lpeff,ppdeff,punkOuter);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKA lpCallback,LPVOID pvRef,DWORD dwEffType)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->EnumEffects(lpCallback,pvRef,dwEffType);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,GetEffectInfo)(THIS_ LPDIEFFECTINFOA pdei,REFGUID rguid)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetEffectInfo(pdei,rguid);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetForceFeedbackState)(THIS_  LPDWORD pdwOut)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetForceFeedbackState(pdwOut);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SendForceFeedbackCommand)(THIS_ DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->SendForceFeedbackCommand(dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumCreatedEffectObjects)(THIS_  LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback,LPVOID pvRef,DWORD fl)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->EnumCreatedEffectObjects(lpCallback,pvRef,fl);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Escape)(THIS_ LPDIEFFESCAPE pesc)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->Escape(pesc);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Poll)(THIS)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->Poll();
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SendDeviceData)(THIS_ DWORD cbObjectData,LPCDIDEVICEOBJECTDATA rgdod,LPDWORD pdwInOut,DWORD fl)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->SendDeviceData(cbObjectData,rgdod,pdwInOut,fl);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumEffectsInFile)(THIS_ LPCSTR lpszFileName,LPDIENUMEFFECTSINFILECALLBACK pec,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->EnumEffectsInFile(lpszFileName,pec,pvRef,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,WriteEffectToFile)(THIS_ LPCSTR lpszFileName,DWORD dwEntries,LPDIFILEEFFECT rgDiFileEft,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->WriteEffectToFile(lpszFileName,dwEntries,rgDiFileEft,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,BuildActionMap)(THIS_ LPDIACTIONFORMATA lpdiaf,LPCSTR lpszUserName,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->BuildActionMap(lpdiaf,lpszUserName,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetActionMap)(THIS_  LPDIACTIONFORMATA lpdiActionFormat,LPCSTR lptszUserName,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->SetActionMap(lpdiActionFormat,lptszUserName,dwFlags);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetImageInfo)(THIS_  LPDIDEVICEIMAGEINFOHEADERA lpdiDevImageInfoHeader)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8A_IN();
        hr = m_ptr->GetImageInfo(lpdiDevImageInfoHeader);
        DIRECT_INPUT_DEVICE_8A_OUT();
        return hr;
    }

};

CDirectInputDevice8AHook* RegisterDirectInputDevice8AHook(IDirectInputDevice8A* ptr,REFIID riid)
{
    CDirectInputDevice8AHook* pHookA=NULL;
    int findidx = -1;
    unsigned int i;


    EnterCriticalSection(&st_Dinput8DeviceCS);
    EQUAL_DEVICE_8A_VECS();
    if(IS_IID_MOUSE(riid))
    {
        for(i=0; i<st_Mouse8AVes.size(); i++)
        {
            if(st_Mouse8AVes[i] == ptr)
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            pHookA = st_Mouse8AHookVecs[findidx];
        }
        else
        {
            pHookA =new CDirectInputDevice8AHook(ptr,riid);
            st_Mouse8AVes.push_back(ptr);
            st_Mouse8AHookVecs.push_back(pHookA);
            ptr->AddRef();
        }
        if(st_Mouse8AVes.size() > 1)
        {
            DEBUG_INFO("Mouse8AVes size (%d)\n",st_Mouse8AVes.size());
        }

    }
    else if(IS_IID_KEYBOARD(riid))
    {
        for(i=0; i<st_Key8AVecs.size(); i++)
        {
            if(st_Key8AVecs[i] == ptr)
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            pHookA = st_Key8AHookVecs[findidx];
        }
        else
        {
            pHookA =new CDirectInputDevice8AHook(ptr,riid);
            st_Key8AVecs.push_back(ptr);
            st_Key8AHookVecs.push_back(pHookA);
            ptr->AddRef();
        }
        if(st_Key8AVecs.size() > 1)
        {
            DEBUG_INFO("Key8AVecs size (%d)\n",st_Key8AVecs.size());
        }
    }
    else
    {
        for(i=0; i<st_NotSet8AVecs.size(); i++)
        {
            if(st_NotSet8AVecs[i] == ptr)
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            pHookA = st_NotSet8AHookVecs[findidx];
        }
        else
        {
            pHookA =new CDirectInputDevice8AHook(ptr,riid);
            st_NotSet8AVecs.push_back(ptr);
            st_NotSet8AHookVecs.push_back(pHookA);
            ptr->AddRef();
        }
    }

    LeaveCriticalSection(&st_Dinput8DeviceCS);

    return pHookA;
}



/*****************************************
*  to make the IDirectInputDevice8W hook
*
*****************************************/

ULONG UnRegisterDirectInputDevice8WHook(IDirectInputDevice8W* ptr)
{
    int findidx = -1;
    ULONG uret=1;
    unsigned int i;

    EnterCriticalSection(&st_Dinput8DeviceCS);
    EQUAL_DEVICE_8W_VECS();
    findidx = -1;
    for(i=0; i<st_Key8WVecs.size() ; i++)
    {
        if(st_Key8WVecs[i] == ptr)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_Key8WVecs.erase(st_Key8WVecs.begin() + findidx);
        st_Key8WHookVecs.erase(st_Key8WHookVecs.begin() + findidx);
    }
    else
    {
        for(i=0; i<st_Mouse8WVecs.size() ; i++)
        {
            if(st_Mouse8WVecs[i] == ptr)
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            st_Mouse8WVecs.erase(st_Mouse8WVecs.begin() + findidx);
            st_Mouse8WHookVecs.erase(st_Mouse8WHookVecs.begin() + findidx);
        }
        else
        {
            for(i=0; i<st_NotSet8WVecs.size() ; i++)
            {
                if(st_NotSet8WVecs[i] == ptr)
                {
                    findidx = i;
                    break;
                }
            }

            if(findidx >= 0)
            {
                st_NotSet8WVecs.erase(st_NotSet8WVecs.begin() + findidx);
                st_NotSet8WHookVecs.erase(st_NotSet8WHookVecs.begin() + findidx);
            }
            else
            {
                ERROR_INFO("<0x%p> not in the vectors\n",ptr);
            }
        }
    }

    LeaveCriticalSection(&st_Dinput8DeviceCS);

    uret = 1;
    if(findidx >= 0)
    {
        uret = ptr->Release();
    }
    return uret;
}


//#define  DIRECT_INPUT_DEVICE_8W_IN()  do{DINPUT_DEBUG_INFO("Device8W::%s 0x%p->0x%p in\n",__FUNCTION__,this,this->m_ptr);}while(0)
//#define  DIRECT_INPUT_DEVICE_8W_OUT()  do{DINPUT_DEBUG_INFO("Device8W::%s 0x%p->0x%p out\n",__FUNCTION__,this,this->m_ptr);}while(0)
#define  DIRECT_INPUT_DEVICE_8W_IN()
#define  DIRECT_INPUT_DEVICE_8W_OUT()


class CDirectInputDevice8WHook : public IDirectInputDevice8W
{
private:
    IDirectInputDevice8W* m_ptr;
    IID m_iid;
private:
    int __IsMouseDevice()
    {
        int ret = 0;
        if(IS_IID_MOUSE((this->m_iid)))
        {
            ret = 1;
        }

        return ret;
    }

    int __IsKeyboardDevice()
    {
        int ret = 0;
        if(IS_IID_KEYBOARD(this->m_iid))
        {
            ret = 1;
        }
        return ret;
    }
public:
    CDirectInputDevice8WHook(IDirectInputDevice8W* ptr,REFIID riid) : m_ptr(ptr)
    {
        m_iid = riid;
    };
    ~CDirectInputDevice8WHook()
    {
        m_iid = IID_NULL;
    }
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }
    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_DEVICE_8W_IN();
        uret = m_ptr->AddRef();
        DIRECT_INPUT_DEVICE_8W_OUT();
        return uret;
    }
    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_DEVICE_8W_IN();
        uret = m_ptr->Release();
        DIRECT_INPUT_DEVICE_8W_OUT();
        DINPUT_DEBUG_INFO("<0x%p->0x%p>uret = %d\n",this,this->m_ptr,uret);
        if(uret == 1)
        {
            uret = UnRegisterDirectInputDevice8WHook(this->m_ptr);
            if(uret == 0)
            {
                delete this;
            }
        }
        return uret;
    }

    COM_METHOD(HRESULT,GetCapabilities)(THIS_ LPDIDEVCAPS lpDIDevCaps)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetCapabilities(lpDIDevCaps);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumObjects)(THIS_ LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->EnumObjects(lpCallback,pvRef,dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetProperty)(THIS_ REFGUID rguidProp,LPDIPROPHEADER pdiph)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetProperty(rguidProp,pdiph);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetProperty)(THIS_ REFGUID rguidProp,LPCDIPROPHEADER pdiph)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->SetProperty(rguidProp,pdiph);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Acquire)(THIS)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->Acquire();
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,Unacquire)(THIS)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->Unacquire();
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceState)(THIS_ DWORD cbData,LPVOID lpvData)
    {
        HRESULT hr;
        int ret;
        DIRECT_INPUT_DEVICE_8W_IN();
        if(this->__IsMouseDevice())
        {
            if(lpvData == NULL)
            {
                ret = ERROR_INVALID_PARAMETER;
                hr = DIERR_INVALIDPARAM;
                SetLastError(ret);
            }
            else
            {
                ret=  __CopyDiMouseState(lpvData,cbData);
                if(ret < 0)
                {
                    hr = DIERR_INVALIDPARAM;
                }
                else
                {
                    hr = DI_OK;
                }
            }
        }
        else if(this->__IsKeyboardDevice())
        {
            if(lpvData == NULL)
            {
                ret = ERROR_INVALID_PARAMETER;
                hr = DIERR_INVALIDPARAM;
                SetLastError(ret);
            }
            else
            {
                ret=  __CopyDiKeyState(lpvData,cbData);
                if(ret < 0)
                {
                    hr = DIERR_INVALIDPARAM;
                }
                else
                {
                    hr = DI_OK;
                }
            }
        }
        else
        {
            hr = this->m_ptr->GetDeviceState(cbData,lpvData);
        }
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceData)(THIS_ DWORD cbObjectData,LPDIDEVICEOBJECTDATA rgdod,LPDWORD pdwInOut,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetDeviceData(cbObjectData,rgdod,pdwInOut,dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetDataFormat)(THIS_ LPCDIDATAFORMAT lpdf)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->SetDataFormat(lpdf);
        if(hr == DI_OK)
        {
            if(this->m_iid == GUID_SysMouse)
            {
                DINPUT_DEBUG_INFO("<0x%p> SysMouse Format\n",this->m_ptr);
            }
            else if(this->m_iid == GUID_SysKeyboard)
            {
                DINPUT_DEBUG_INFO("<0x%p> SysKeyboard Format\n",this->m_ptr);
            }
            else if(this->m_iid == GUID_Joystick)
            {
                DINPUT_DEBUG_INFO("<0x%p> Joystick Format\n",this->m_ptr);
            }
            else if(this->m_iid == GUID_SysMouseEm)
            {
                DINPUT_DEBUG_INFO("<0x%p> SysMouseEm Format\n",this->m_ptr);
            }
            else if(this->m_iid == GUID_SysMouseEm2)
            {
                DINPUT_DEBUG_INFO("<0x%p> SysMouseEm2 Format\n",this->m_ptr);
            }
            else if(this->m_iid == GUID_SysKeyboardEm)
            {
                DINPUT_DEBUG_INFO("<0x%p> SysKeyboardEm Format\n",this->m_ptr);
            }
            else if(this->m_iid == GUID_SysKeyboardEm2)
            {
                DINPUT_DEBUG_INFO("<0x%p> SysKeyboardEm2 Format\n",this->m_ptr);
            }

            if(lpdf == &c_dfDIMouse)
            {
                DINPUT_DEBUG_INFO("c_dfDIMouse\n");
            }
            else if(lpdf == &c_dfDIMouse2)
            {
                DINPUT_DEBUG_INFO("c_dfDIMouse2\n");
            }
            else if(lpdf == &c_dfDIKeyboard)
            {
                DINPUT_DEBUG_INFO("c_dfDIKeyboard\n");
            }
            else if(lpdf == &c_dfDIJoystick)
            {
                DINPUT_DEBUG_INFO(" c_dfDIJoystick\n");
            }
            else if(lpdf == &c_dfDIJoystick2)
            {
                DINPUT_DEBUG_INFO(" c_dfDIJoystick2\n");
            }
            if(lpdf->dwSize)
            {
                DINPUT_DEBUG_BUFFER_FMT(lpdf,lpdf->dwSize,"<0x%p> format",this->m_ptr);
            }

            if(lpdf->rgodf && lpdf->dwObjSize)
            {
                DINPUT_DEBUG_BUFFER_FMT(lpdf->rgodf,lpdf->dwObjSize,"<0x%p> flag 0x%08x datasize 0x%08x numobjs 0x%08x",this->m_ptr,lpdf->dwFlags,lpdf->dwDataSize,lpdf->dwNumObjs);
            }


        }
        else
        {
            ERROR_INFO("<0x%p> SetDataFormat Error\n",this->m_ptr);
        }
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetEventNotification)(THIS_ HANDLE hEvent)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->SetEventNotification(hEvent);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetCooperativeLevel)(THIS_ HWND hwnd,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->SetCooperativeLevel(hwnd,dwFlags);
        if(hr == DI_OK)
        {
            DINPUT_DEBUG_INFO("<0x%p> hwnd 0x%08x dwLevel 0x%08x\n",this->m_ptr,
                              hwnd,dwFlags);
        }
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetObjectInfo)(THIS_ LPDIDEVICEOBJECTINSTANCEW pdidoi,DWORD dwObj,DWORD dwHow)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetObjectInfo(pdidoi,dwObj,dwHow);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceInfo)(THIS_ LPDIDEVICEINSTANCEW pdidi)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetDeviceInfo(pdidi);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,RunControlPanel)(THIS_ HWND hwndOwner,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->RunControlPanel(hwndOwner,dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Initialize)(THIS_ HINSTANCE hinst,DWORD dwVersion,REFGUID rguid)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->Initialize(hinst,dwVersion,rguid);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,CreateEffect)(THIS_ REFGUID rguid,LPCDIEFFECT lpeff,LPDIRECTINPUTEFFECT * ppdeff,LPUNKNOWN punkOuter)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->CreateEffect(rguid,lpeff,ppdeff,punkOuter);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumEffects)(THIS_ LPDIENUMEFFECTSCALLBACKW lpCallback,LPVOID pvRef,DWORD dwEffType)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->EnumEffects(lpCallback,pvRef,dwEffType);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,GetEffectInfo)(THIS_ LPDIEFFECTINFOW pdei,REFGUID rguid)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetEffectInfo(pdei,rguid);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetForceFeedbackState)(THIS_  LPDWORD pdwOut)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetForceFeedbackState(pdwOut);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SendForceFeedbackCommand)(THIS_ DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->SendForceFeedbackCommand(dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumCreatedEffectObjects)(THIS_  LPDIENUMCREATEDEFFECTOBJECTSCALLBACK lpCallback,LPVOID pvRef,DWORD fl)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->EnumCreatedEffectObjects(lpCallback,pvRef,fl);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Escape)(THIS_ LPDIEFFESCAPE pesc)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->Escape(pesc);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Poll)(THIS)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->Poll();
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SendDeviceData)(THIS_ DWORD cbObjectData,LPCDIDEVICEOBJECTDATA rgdod,LPDWORD pdwInOut,DWORD fl)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->SendDeviceData(cbObjectData,rgdod,pdwInOut,fl);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumEffectsInFile)(THIS_ LPCWSTR lpszFileName,LPDIENUMEFFECTSINFILECALLBACK pec,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->EnumEffectsInFile(lpszFileName,pec,pvRef,dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,WriteEffectToFile)(THIS_ LPCWSTR lpszFileName,DWORD dwEntries,LPDIFILEEFFECT rgDiFileEft,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->WriteEffectToFile(lpszFileName,dwEntries,rgDiFileEft,dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,BuildActionMap)(THIS_ LPDIACTIONFORMATW lpdiaf,LPCTSTR lpszUserName,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->BuildActionMap(lpdiaf,lpszUserName,dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,SetActionMap)(THIS_	LPDIACTIONFORMATW lpdiActionFormat,LPCTSTR lptszUserName,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->SetActionMap(lpdiActionFormat,lptszUserName,dwFlags);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetImageInfo)(THIS_	LPDIDEVICEIMAGEINFOHEADERW lpdiDevImageInfoHeader)
    {
        HRESULT hr;
        DIRECT_INPUT_DEVICE_8W_IN();
        hr = m_ptr->GetImageInfo(lpdiDevImageInfoHeader);
        DIRECT_INPUT_DEVICE_8W_OUT();
        return hr;
    }

};

CDirectInputDevice8WHook* RegisterDirectInputDevice8WHook(IDirectInputDevice8W* ptr,REFIID riid)
{
    CDirectInputDevice8WHook* pHookW=NULL;
    int findidx = -1;
    unsigned int i;

    EnterCriticalSection(&st_Dinput8DeviceCS);
    EQUAL_DEVICE_8W_VECS();
    if(IS_IID_KEYBOARD(riid))
    {
        for(i=0; i<st_Key8WVecs.size() ; i++)
        {
            if(ptr == st_Key8WVecs[i])
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            pHookW = st_Key8WHookVecs[findidx];
        }
        else
        {
            pHookW = new CDirectInputDevice8WHook(ptr,riid);
            assert(pHookW);
            st_Key8WVecs.push_back(ptr);
            st_Key8WHookVecs.push_back(pHookW);
            /*to make it not freed by accident*/
            ptr->AddRef();
        }
        if(st_Key8WVecs.size() > 1)
        {
            DEBUG_INFO("Key8WVecs size (%d)\n",st_Key8WVecs.size());
        }
    }
    else if(IS_IID_MOUSE(riid))
    {
        for(i=0; i<st_Mouse8WVecs.size() ; i++)
        {
            if(ptr == st_Mouse8WVecs[i])
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            pHookW = st_Mouse8WHookVecs[findidx];
        }
        else
        {
            pHookW = new CDirectInputDevice8WHook(ptr,riid);
            assert(pHookW);
            st_Mouse8WVecs.push_back(ptr);
            st_Mouse8WHookVecs.push_back(pHookW);
            /*to make it not freed by accident*/
            ptr->AddRef();
        }
        if(st_Mouse8WVecs.size() > 1)
        {
            DEBUG_INFO("Mouse8WVecs size (%d)\n",st_Mouse8WVecs.size());
        }
    }
    else
    {
        for(i=0; i<st_NotSet8WVecs.size() ; i++)
        {
            if(ptr == st_NotSet8WVecs[i])
            {
                findidx = i;
                break;
            }
        }

        if(findidx >= 0)
        {
            pHookW = st_NotSet8WHookVecs[findidx];
        }
        else
        {
            pHookW = new CDirectInputDevice8WHook(ptr,riid);
            assert(pHookW);
            st_NotSet8WVecs.push_back(ptr);
            st_NotSet8WHookVecs.push_back(pHookW);
            /*to make it not freed by accident*/
            ptr->AddRef();
        }
    }

    LeaveCriticalSection(&st_Dinput8DeviceCS);

    return pHookW;
}



/*****************************************
*   to make the DirectInput8A hook
*
*****************************************/
class CDirectInput8AHook;

static std::vector<IDirectInput8A*> st_DI8AVecs;
static std::vector<CDirectInput8AHook*> st_CDI8AHookVecs;
static CRITICAL_SECTION st_DI8ACS;

#define EQUAL_DI8A_VECS() \
do\
{\
	assert(st_DI8AVecs.size() == st_CDI8AHookVecs.size());\
}while(0)

ULONG UnRegisterDirectInput8AHook(IDirectInput8A* ptr)
{
    int findidx = -1;
    ULONG uret=1;
    unsigned int i;

    EnterCriticalSection(&st_DI8ACS);
    EQUAL_DI8A_VECS();
    for(i=0; i<st_DI8AVecs.size() ; i++)
    {
        if(st_DI8AVecs[i] == ptr)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_DI8AVecs.erase(st_DI8AVecs.begin()+findidx);
        st_CDI8AHookVecs.erase(st_CDI8AHookVecs.begin() + findidx);
    }
    else
    {
        ERROR_INFO("could not find 0x%p DirectInput8A\n",ptr);
    }
    LeaveCriticalSection(&st_DI8ACS);

    uret = 1;
    if(findidx >= 0)
    {
        uret = ptr->Release();
    }
    return uret;
}

#define  DIRECT_INPUT_8A_IN() do{DINPUT_DEBUG_INFO("Input8A::%s 0x%p in\n",__FUNCTION__,this->m_ptr);}while(0)
#define  DIRECT_INPUT_8A_OUT() do{DINPUT_DEBUG_INFO("Input8A::%s 0x%p out\n",__FUNCTION__,this->m_ptr);}while(0)
class CDirectInput8AHook :public IDirectInput8A
{
private:
    IDirectInput8A* m_ptr;

public:
    CDirectInput8AHook(IDirectInput8A* ptr):m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        DIRECT_INPUT_8A_OUT();
        return hr;
    }
    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_8A_IN();
        uret = m_ptr->AddRef();
        DIRECT_INPUT_8A_OUT();
        return uret;
    }
    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_8A_IN();
        uret = m_ptr->Release();
        DIRECT_INPUT_8A_OUT();
        DINPUT_DEBUG_INFO("uret = %d\n",uret);
        if(uret == 1)
        {
            uret = UnRegisterDirectInput8AHook(this->m_ptr);
            if(uret == 0)
            {
                delete this;
            }
        }
        return uret;
    }

    COM_METHOD(HRESULT,CreateDevice)(THIS_ REFGUID rguid,
                                     LPDIRECTINPUTDEVICE8A * lplpDirectInputDevice,
                                     LPUNKNOWN pUnkOuter)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->CreateDevice(rguid,lplpDirectInputDevice,pUnkOuter);
        if(SUCCEEDED(hr))
        {
            CDirectInputDevice8AHook* pHookA=NULL;
            if(rguid == GUID_SysMouse)
            {
                pHookA = RegisterDirectInputDevice8AHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("sysmouse 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookA);
            }
            else if(rguid == GUID_SysKeyboard)
            {
                pHookA = RegisterDirectInputDevice8AHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("syskeyboard 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookA);
            }
            else if(rguid == GUID_Joystick)
            {
                pHookA = RegisterDirectInputDevice8AHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("joystick 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookA);
            }
            else if(rguid == GUID_SysMouseEm)
            {
                pHookA = RegisterDirectInputDevice8AHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("sysmouseem 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookA);
            }
            else if(rguid == GUID_SysMouseEm2)
            {
                pHookA = RegisterDirectInputDevice8AHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("sysmouseem2 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookA);
            }
            else if(rguid == GUID_SysKeyboardEm)
            {
                pHookA = RegisterDirectInputDevice8AHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("syskeyboardem 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookA);
            }
            else if(rguid == GUID_SysKeyboardEm2)
            {
                pHookA = RegisterDirectInputDevice8AHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("syskeyboardem2 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookA);
            }


            DINPUT_DEBUG_INFO("rguid %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
                              rguid.Data1,
                              rguid.Data2,
                              rguid.Data3,
                              rguid.Data4[0],
                              rguid.Data4[1],
                              rguid.Data4[2],
                              rguid.Data4[3],
                              rguid.Data4[4],
                              rguid.Data4[5],
                              rguid.Data4[6],
                              rguid.Data4[7]);

            if(pHookA)
            {
                /*if we make the hook object ,just replace it to the upper caller*/
                *lplpDirectInputDevice = pHookA;
            }
        }
        DIRECT_INPUT_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumDevices)(THIS_ DWORD dwDevType,LPDIENUMDEVICESCALLBACKA lpCallback,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        if(dwDevType == DI8DEVCLASS_GAMECTRL)
        {
            /*we do not get any gamectrl ,so just return ok*/
            DEBUG_INFO("\n");
            hr = DI_OK;
        }
        else
        {
            hr = m_ptr->EnumDevices(dwDevType,lpCallback,pvRef,dwFlags);
        }
        DIRECT_INPUT_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceStatus)(THIS_  REFGUID rguidInstance)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->GetDeviceStatus(rguidInstance);
        DIRECT_INPUT_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,RunControlPanel)(THIS_ HWND hwndOwner,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->RunControlPanel(hwndOwner,dwFlags);
        DIRECT_INPUT_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Initialize)(THIS_  HINSTANCE hinst,DWORD dwVersion)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->Initialize(hinst,dwVersion);
        DIRECT_INPUT_8A_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,FindDevice)(THIS_ REFGUID rguidClass,LPCSTR ptszName,LPGUID pguidInstance)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->FindDevice(rguidClass,ptszName,pguidInstance);
        DIRECT_INPUT_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumDevicesBySemantics)(THIS_ LPCSTR ptszUserName,LPDIACTIONFORMATA lpdiActionFormat,LPDIENUMDEVICESBYSEMANTICSCBA lpCallback,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->EnumDevicesBySemantics(ptszUserName,lpdiActionFormat,lpCallback,pvRef,dwFlags);
        DIRECT_INPUT_8A_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK lpdiCallback,LPDICONFIGUREDEVICESPARAMSA lpdiCDParams,DWORD dwFlags,LPVOID pvRefData)
    {
        HRESULT hr;
        DIRECT_INPUT_8A_IN();
        hr = m_ptr->ConfigureDevices(lpdiCallback,lpdiCDParams,dwFlags,pvRefData);
        DIRECT_INPUT_8A_OUT();
        return hr;
    }

};

CDirectInput8AHook* RegisterDirectInput8AHook(IDirectInput8A* ptr)
{
    int findidx=-1;
    unsigned int i;
    CDirectInput8AHook* pHook=NULL;

    EnterCriticalSection(&st_DI8ACS);
    EQUAL_DI8A_VECS();
    for(i=0; i<st_DI8AVecs.size() ; i++)
    {
        if(st_DI8AVecs[i]==ptr)
        {
            findidx = i;
            break;
        }
    }

    if(findidx < 0)
    {
        pHook =new CDirectInput8AHook(ptr);
        st_DI8AVecs.push_back(ptr);
        st_CDI8AHookVecs.push_back(pHook);
        /*to add reference to control the delete procedure*/
        ptr->AddRef();
    }
    else
    {
        pHook = st_CDI8AHookVecs[findidx];
    }
    LeaveCriticalSection(&st_DI8ACS);
    return pHook;
}


/*****************************************
*   to make the DirectInput8W hook
*
*****************************************/


class CDirectInput8WHook;

static std::vector<IDirectInput8W*> st_DI8WVecs;
static std::vector<CDirectInput8WHook*> st_CDI8WHookVecs;
static CRITICAL_SECTION st_DI8WCS;

#define EQUAL_DI8W_VECS() \
do\
{\
	assert(st_DI8WVecs.size() == st_CDI8WHookVecs.size());\
}while(0)

ULONG UnRegisterDirectInput8WHook(IDirectInput8W* ptr)
{
    int findidx = -1;
    ULONG uret=1;
    unsigned int i;

    EnterCriticalSection(&st_DI8WCS);
    EQUAL_DI8W_VECS();
    for(i=0; i<st_DI8WVecs.size() ; i++)
    {
        if(st_DI8WVecs[i] == ptr)
        {
            findidx = i;
            break;
        }
    }

    if(findidx >= 0)
    {
        st_DI8WVecs.erase(st_DI8WVecs.begin()+findidx);
        st_CDI8WHookVecs.erase(st_CDI8WHookVecs.begin() + findidx);
    }
    else
    {
        ERROR_INFO("Not Register 0x%p\n",ptr);
    }
    LeaveCriticalSection(&st_DI8WCS);

    uret = 1;
    if(findidx >= 0)
    {
        uret = ptr->Release();
    }
    return uret;
}



#define  DIRECT_INPUT_8W_IN()  do{DINPUT_DEBUG_INFO("Input8W::%s 0x%p in\n",__FUNCTION__,this->m_ptr);}while(0)
#define  DIRECT_INPUT_8W_OUT()  do{DINPUT_DEBUG_INFO("Input8W::%s 0x%p out\n",__FUNCTION__,this->m_ptr);}while(0)

class CDirectInput8WHook : IDirectInput8W
{
private:
    IDirectInput8W* m_ptr;
public:
    CDirectInput8WHook(IDirectInput8W* ptr) : m_ptr(ptr) {};
public:
    COM_METHOD(HRESULT,QueryInterface)(THIS_ REFIID riid,void **ppvObject)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->QueryInterface(riid,ppvObject);
        DIRECT_INPUT_8W_OUT();
        return hr;
    }
    COM_METHOD(ULONG,AddRef)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_8W_IN();
        uret = m_ptr->AddRef();
        DIRECT_INPUT_8W_OUT();
        return uret;
    }
    COM_METHOD(ULONG,Release)(THIS)
    {
        ULONG uret;
        DIRECT_INPUT_8W_IN();
        uret = m_ptr->Release();
        DIRECT_INPUT_8W_OUT();
        if(uret == 1)
        {
            uret = UnRegisterDirectInput8WHook(this->m_ptr);
            if(uret == 0)
            {
                delete this;
            }
        }
        return uret;
    }

    COM_METHOD(HRESULT,CreateDevice)(THIS_ REFGUID rguid,
                                     LPDIRECTINPUTDEVICE8W * lplpDirectInputDevice,
                                     LPUNKNOWN pUnkOuter)
    {
        HRESULT hr;
        SetUnHandlerExceptionDetour();
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->CreateDevice(rguid,lplpDirectInputDevice,pUnkOuter);
        if(SUCCEEDED(hr))
        {
            CDirectInputDevice8WHook* pHookW=NULL;
            if(rguid == GUID_SysMouse)
            {
                pHookW = RegisterDirectInputDevice8WHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("sysmouse 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookW);
            }
            else if(rguid == GUID_SysKeyboard)
            {
                pHookW = RegisterDirectInputDevice8WHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("syskeyboard 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookW);
            }
            else if(rguid == GUID_Joystick)
            {
                pHookW = RegisterDirectInputDevice8WHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("joystick 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookW);
            }
            else if(rguid == GUID_SysMouseEm)
            {
                pHookW = RegisterDirectInputDevice8WHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("sysmouseem 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookW);
            }
            else if(rguid == GUID_SysMouseEm2)
            {
                pHookW = RegisterDirectInputDevice8WHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("sysmouseem2 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookW);
            }
            else if(rguid == GUID_SysKeyboardEm)
            {
                pHookW = RegisterDirectInputDevice8WHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("syskeyboardem 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookW);
            }
            else if(rguid == GUID_SysKeyboardEm2)
            {
                pHookW = RegisterDirectInputDevice8WHook(*lplpDirectInputDevice,rguid);
                DINPUT_DEBUG_INFO("syskeyboardem2 0x%p hook 0x%p\n",*lplpDirectInputDevice,pHookW);
            }


            DINPUT_DEBUG_INFO("rguid %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
                              rguid.Data1,
                              rguid.Data2,
                              rguid.Data3,
                              rguid.Data4[0],
                              rguid.Data4[1],
                              rguid.Data4[2],
                              rguid.Data4[3],
                              rguid.Data4[4],
                              rguid.Data4[5],
                              rguid.Data4[6],
                              rguid.Data4[7]);

            if(pHookW)
            {
                /*if we make the hook object ,just replace it to the upper caller*/
                *lplpDirectInputDevice = pHookW;
            }
        }
        DIRECT_INPUT_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumDevices)(THIS_ DWORD dwDevType,LPDIENUMDEVICESCALLBACKW lpCallback,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        if(dwDevType == DI8DEVCLASS_GAMECTRL)
        {
            DEBUG_INFO("\n");
            hr = DI_OK;
        }
        else
        {
            hr = m_ptr->EnumDevices(dwDevType,lpCallback,pvRef,dwFlags);
        }
        DIRECT_INPUT_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,GetDeviceStatus)(THIS_  REFGUID rguidInstance)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->GetDeviceStatus(rguidInstance);
        DIRECT_INPUT_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,RunControlPanel)(THIS_ HWND hwndOwner,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->RunControlPanel(hwndOwner,dwFlags);
        DIRECT_INPUT_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,Initialize)(THIS_  HINSTANCE hinst,DWORD dwVersion)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->Initialize(hinst,dwVersion);
        DIRECT_INPUT_8W_OUT();
        return hr;
    }


    COM_METHOD(HRESULT,FindDevice)(THIS_ REFGUID rguidClass,LPCTSTR ptszName,LPGUID pguidInstance)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->FindDevice(rguidClass,ptszName,pguidInstance);
        DIRECT_INPUT_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,EnumDevicesBySemantics)(THIS_ LPCTSTR ptszUserName,LPDIACTIONFORMAT lpdiActionFormat,LPDIENUMDEVICESBYSEMANTICSCB lpCallback,LPVOID pvRef,DWORD dwFlags)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->EnumDevicesBySemantics(ptszUserName,lpdiActionFormat,lpCallback,pvRef,dwFlags);
        DIRECT_INPUT_8W_OUT();
        return hr;
    }

    COM_METHOD(HRESULT,ConfigureDevices)(THIS_ LPDICONFIGUREDEVICESCALLBACK lpdiCallback,LPDICONFIGUREDEVICESPARAMSW lpdiCDParams,DWORD dwFlags,LPVOID pvRefData)
    {
        HRESULT hr;
        DIRECT_INPUT_8W_IN();
        hr = m_ptr->ConfigureDevices(lpdiCallback,lpdiCDParams,dwFlags,pvRefData);
        DIRECT_INPUT_8W_OUT();
        return hr;
    }

};

CDirectInput8WHook* RegisterDirectInput8WHook(IDirectInput8W* ptr)
{
    int findidx=-1;
    unsigned int i;
    CDirectInput8WHook* pHookW=NULL;

    EnterCriticalSection(&st_DI8WCS);
    EQUAL_DI8W_VECS();
    for(i=0; i<st_DI8WVecs.size() ; i++)
    {
        if(st_DI8WVecs[i]==ptr)
        {
            findidx = i;
            break;
        }
    }

    if(findidx < 0)
    {
        pHookW =new CDirectInput8WHook(ptr);
        st_DI8WVecs.push_back(ptr);
        st_CDI8WHookVecs.push_back(pHookW);
        /*to add reference to control the delete procedure*/
        ptr->AddRef();
    }
    else
    {
        pHookW = st_CDI8WHookVecs[findidx];
    }
    LeaveCriticalSection(&st_DI8WCS);
    return pHookW;
}


void DetourDinputEmulationFini(HMODULE hModule)
{
	UnRegisterEventListInit(DetourDinput8Init);
    UnRegisterEventListHandler(DetourDinput8SetKeyMouseState);
    UnRegisterCreateWindowFunc(Dinput8CreateWindowNotify);
    UnRegisterDestroyWindowFunc(Dinput8DestroyWindowNotify);
    return ;
}

int DetourDinputEmulationInit(HMODULE hModule)
{
    int ret;
    InitializeCriticalSection(&st_Dinput8DeviceCS);
    InitializeCriticalSection(&st_Dinput8KeyMouseStateCS);
    ret = RegisterDestroyWindowFunc(Dinput8DestroyWindowNotify,NULL,30);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not register destroy window Notify Error(%d)\n",ret);
        goto fail;
    }
    ret = RegisterCreateWindowFunc(Dinput8CreateWindowNotify,NULL,100);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not register create window Notify Error(%d)\n",ret);
        goto fail;
    }
    ret = RegisterEventListHandler(DetourDinput8SetKeyMouseState,NULL,DINPUT_EMULATION_PRIOR);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not register Eventlist Handler Error(%d)\n",ret);
        goto fail;
    }

	ret = RegisterEventListInit(DetourDinput8Init,NULL,DINPUT_EMULATION_PRIOR);
    if(ret < 0)
    {
        ret = LAST_ERROR_CODE();
        ERROR_INFO("could not register Eventlist Init Error(%d)\n",ret);
        goto fail;
    }
	

    SetLastError(0);
    return 0;
fail:
    assert(ret > 0);
	UnRegisterEventListInit(DetourDinput8Init);
    UnRegisterEventListHandler(DetourDinput8SetKeyMouseState);
    UnRegisterCreateWindowFunc(Dinput8CreateWindowNotify);
    UnRegisterDestroyWindowFunc(Dinput8DestroyWindowNotify);
    SetLastError(ret);
    return FALSE;
}


