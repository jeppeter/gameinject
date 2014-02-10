// outmon.cpp :
//

#include <output_monitor.h>
#include <assert.h>


#define  ERROROUT(...) do{fprintf(stderr,"%s:%d\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}while(0)
#define  INFOOUT(...) do{fprintf(stderr,"%s:%d\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}while(0)


#define  FILE_CREATE    1
#define  FILE_APPEND    2

static char* st_pFile=NULL;
static int st_FileMode=FILE_CREATE;
static int st_Running=1;

BOOL WINAPI HandlerConsoleRoutine(DWORD dwCtrlType)
{
    BOOL bret=TRUE;
    switch(dwCtrlType)
    {
    case CTRL_C_EVENT:
        INFOOUT("CTRL_C_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_BREAK_EVENT:
        INFOOUT("CTRL_BREAK_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_CLOSE_EVENT:
        INFOOUT("CTRL_CLOSE_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_LOGOFF_EVENT:
        INFOOUT("CTRL_LOGOFF_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_SHUTDOWN_EVENT:
        INFOOUT("CTRL_SHUTDOWN_EVENT\n");
        st_Running = 0;
        break;
    default:
        INFOOUT("ctrltype %d\n",dwCtrlType);
        bret = FALSE;
        break;
    }

    return bret;
}

void Usage(int ec,const char* fmt,...)
{
    FILE* fp=stderr;
    va_list ap;
    if(ec == 0)
    {
        fp = stdout;
    }

    if(fmt)
    {
        va_start(ap,fmt);
        vfprintf(fp,fmt,ap);
        fprintf(fp,"\n");
    }

    fprintf(fp,"outmon [OPTIONS]\n");
    fprintf(fp,"\t-h|--help              to display this message\n");
    fprintf(fp,"\t-a|--append filename   to specify the file of output and append it\n");
    fprintf(fp,"\t-c|--create filename   to specify the file of output and create it\n");
    fprintf(fp,"default output is stdout\n");

    exit(ec);
}


int ParseParam(int argc,char* argv[])
{
    int i;
    int ret=0;

    for(i=1; i<argc; i++)
    {
        if(strcmp(argv[i],"-h")==0 ||
                strcmp(argv[i],"--help")==0)
        {
            Usage(0,NULL);
        }
        else if(strcmp(argv[i],"-a")==0 ||
                strcmp(argv[i],"--append")==0)
        {
            if((i+1)>= argc)
            {
                Usage(3,"%s need a parameter",argv[i]);
            }
            st_pFile = argv[i+1];
            st_FileMode = FILE_APPEND;
            i ++;
        }
        else if(strcmp(argv[i],"-c")==0 ||
                strcmp(argv[i],"--create")==0)
        {
            if((i+1)>= argc)
            {
                Usage(3,"%s need a parameter",argv[i]);
            }
            st_pFile = argv[i+1];
            st_FileMode = FILE_CREATE;
            i ++;
        }
        else
        {
            Usage(3,"unrecognize parameter %s",argv[i]);
        }
    }

    return 0;
}

int OutputMonitorWriteFile()
{
    FILE* fp=NULL;
    int ret=0;
    OutputMonitor* pMonitor=NULL;
    HANDLE hEvt=NULL;
    DWORD dret;
    std::vector<PDBWIN_BUFFER_t> pBuffers;
    PDBWIN_BUFFER_t pBuffer=NULL;
    UINT i;

    if(st_pFile)
    {
        if(st_FileMode == FILE_APPEND)
        {
            fopen_s(&fp,st_pFile,"a+");
        }
        else
        {
            fopen_s(&fp,st_pFile,"w+");
        }
    }
    else
    {
        fp = stdout;
    }

    if(fp == NULL)
    {
        ret=  GETERRNO();
        ERROROUT("%s %s Error(%d)\n",st_FileMode == FILE_APPEND ? "Append":"Create",st_pFile ? st_pFile : "stdout",ret);
        goto out;
    }

    pMonitor = new OutputMonitor();

    ret = pMonitor->Start();
    if(ret < 0)
    {
        ret = GETERRNO();
        ERROROUT("Start OutputMonitor Error(%d)\n",ret);
        goto out;
    }

    hEvt = pMonitor->GetNotifyHandle();
    assert(hEvt);

    while(st_Running)
    {
        dret = WaitForSingleObjectEx(hEvt,INFINITE,TRUE);
        if(dret == WAIT_OBJECT_0)
        {
            assert(pBuffers.size() == 0);
            ret = pMonitor->GetBuffer(pBuffers);
            if(ret < 0)
            {
                ret= GETERRNO();
                ERROROUT("GetBuffers Error(%d)\n",ret);
                goto out;
            }

            /*now to write down*/
            for(i=0; i<pBuffers.size(); i++)
            {
                pBuffer = pBuffers[i];
                ret = fprintf_s(fp,"[%d]%s\n",pBuffer->dwProcessId,pBuffer->data);
                if(ret < 0)
                {
                    ret = GETERRNO();
                    ERROROUT("write(%d:%s) Error(%d)\n",pBuffer->dwProcessId,pBuffer->data,ret);
                    goto out;
                }
                pBuffer = NULL;
            }

            if(pBuffers.size() > 0)
            {
                fflush(fp);
                pMonitor->ReleaseBuffer(pBuffers);
            }

        }
        else if(dret == WAIT_FAILED || dret == WAIT_ABANDONED)
        {
            ret=  GETERRNO();
            ERROROUT("wait failed (%d)\n",ret);
            goto out;
        }
    }

    ret = 0;


out:
    if(pBuffers.size() > 0)
    {
        assert(pMonitor);
        pMonitor->ReleaseBuffer(pBuffers);
    }
    assert(pBuffers.size() == 0);

    if(pMonitor)
    {
        delete pMonitor;
    }
    pMonitor = NULL;

    if(fp != stdout && fp)
    {
        fclose(fp);
    }
    fp = NULL;
    SETERRNO(ret);
    return -ret;
}



int main(int argc, char* argv[])
{
    BOOL bret;
    int ret;

    bret = SetConsoleCtrlHandler(HandlerConsoleRoutine,TRUE);
    if(!bret)
    {
        ret = GETERRNO();
        ERROROUT("SetControlCtrlHandler Error(%d)",ret);
        goto out;
    }

    ret = ParseParam(argc,argv);
    if(ret < 0)
    {
        ret= GETERRNO();
        ERROROUT("Parse Param Error(%d)\n",ret);
        goto out;
    }




    ret = OutputMonitorWriteFile();
    if(ret < 0)
    {
        ret = GETERRNO();
        goto out;
    }

	ret = 0;

out:
    SETERRNO(ret);
    return -ret;
}

