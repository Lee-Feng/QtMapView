#pragma once

#ifdef _Windows

#include <Windows.h>
#include <QDebug>
#include <Dbghelp.h>
#include <QDateTime>
#include <QDir>
#include <QString>

//#ifndef _M_IX86
//  #error "The following code only works for x86!"
//#endif

#pragma comment(lib, "Dbghelp.lib")

namespace utiles {

static QString toQString(const wchar_t* wchars){
    if(wchars == NULL){
        return "<null>";
    }
    return QString::fromUtf16(reinterpret_cast<const ushort *>(wchars));
}

static void minidump(struct _EXCEPTION_POINTERS *  pExp){
    QDateTime time = QDateTime::currentDateTime();
    QString file = QString("crash/minidump_%1.dmp").arg(time.toString("yyyy-MM-dd_HH-mm-ss-zzz"));
#ifdef UNICODE
    HANDLE hFile = ::CreateFile(
            reinterpret_cast<const wchar_t *>(file.utf16()),
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
#else
	HANDLE hFile = ::CreateFile(
		file.toStdString().c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
#endif
        if(INVALID_HANDLE_VALUE != hFile)
        {
            MINIDUMP_EXCEPTION_INFORMATION einfo;
            einfo.ThreadId          = ::GetCurrentThreadId();
            einfo.ExceptionPointers = pExp;
            einfo.ClientPointers    = FALSE;

            ::MiniDumpWriteDump(
                ::GetCurrentProcess(),
                ::GetCurrentProcessId(),
                hFile,
                MiniDumpWithFullMemory,
                &einfo,
                NULL,
                NULL);
            ::CloseHandle(hFile);
         }
}

static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
    LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
return NULL;
}

/// EXCEPTION_EXECUTE_HANDLER equ 1 表示我已经处理了异常,可以优雅地结束了
/// EXCEPTION_CONTINUE_SEARCH equ 0 表示我不处理,其他人来吧,于是windows调用默认的处理程序显示一个错误框,并结束
/// EXCEPTION_CONTINUE_EXECUTION equ -1 表示错误已经被修复,请从异常发生处继续执行
static LONG Win32FaultHandler(struct _EXCEPTION_POINTERS *  ExInfo)
{
    qDebug() <<"Crashed!!!!";
    minidump(ExInfo);
    return EXCEPTION_EXECUTE_HANDLER;
}

static BOOL PreventSetUnhandledExceptionFilter()
{
#ifdef UNICODE
    HMODULE hKernel32 = LoadLibrary(L"kernel32.dll");
#else
    HMODULE hKernel32 = LoadLibrary("kernel32.dll");
#endif
    if (hKernel32 == NULL) return FALSE;
    void *pOrgEntry = GetProcAddress(hKernel32,"SetUnhandledExceptionFilter");
    if(pOrgEntry == NULL) return FALSE;

    DWORD dwOldProtect = 0;
    SIZE_T jmpSize = 5;
#ifdef _M_X64
    jmpSize = 13;
#endif
    BOOL bProt = VirtualProtect(pOrgEntry, jmpSize,
                                PAGE_EXECUTE_READWRITE, &dwOldProtect);
    BYTE newJump[20];
    void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
#ifdef _M_IX86
    DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
    dwOrgEntryAddr += jmpSize; // add 5 for 5 op-codes for jmp rel32
    DWORD dwNewEntryAddr = (DWORD) pNewFunc;
    DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
    // JMP rel32: Jump near, relative, displacement relative to next instruction.
    newJump[0] = 0xE9;  // JMP rel32
    memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
#elif _M_X64
    // We must use R10 or R11, because these are "scratch" registers
    // which need not to be preserved accross function calls
    // For more info see: Register Usage for x64 64-Bit
    // http://msdn.microsoft.com/en-us/library/ms794547.aspx
    // Thanks to Matthew Smith!!!
    newJump[0] = 0x49;  // MOV R11, ...
    newJump[1] = 0xBB;  // ...
    memcpy(&newJump[2], &pNewFunc, sizeof (pNewFunc));
    //pCur += sizeof (ULONG_PTR);
    newJump[10] = 0x41;  // JMP R11, ...
    newJump[11] = 0xFF;  // ...
    newJump[12] = 0xE3;  // ...
#endif
    SIZE_T bytesWritten;
    BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
                                   pOrgEntry, newJump, jmpSize, &bytesWritten);
    if (bProt != FALSE) {
        DWORD dwBuf;
        VirtualProtect(pOrgEntry, jmpSize, dwOldProtect, &dwBuf);
    }
    return bRet;


//#ifdef UNICODE
//	HMODULE hKernel32 = LoadLibrary(L"kernel32.dll");
//#else
//	HMODULE hKernel32 = LoadLibrary("kernel32.dll");
//#endif
//    if (hKernel32 ==   NULL)
//       return FALSE;

//    void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
//    if(pOrgEntry == NULL)
//       return FALSE;

//    unsigned char newJump[ 100 ];
//    DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
//    dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
//    void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
//    DWORD dwNewEntryAddr = (DWORD) pNewFunc;
//    DWORD dwRelativeAddr = dwNewEntryAddr -  dwOrgEntryAddr;
//    newJump[ 0 ] = 0xE9;  // JMP absolute
//    memcpy(&newJump[ 1 ], &dwRelativeAddr, sizeof(pNewFunc));
//    SIZE_T bytesWritten;
//    BOOL bRet = WriteProcessMemory(GetCurrentProcess(),    pOrgEntry, newJump, sizeof(pNewFunc) + 1, &bytesWritten);
//    return bRet;
}

static void myInvalidParameterHandler(const wchar_t* expression,
          const wchar_t* function,
          const wchar_t* file,
          unsigned int line,
          uintptr_t pReserved)
{
    // function、file、line在Release下无效
    qWarning() << QString("Invalid parameter detected in function %1. File: %2 Line: %3").
                  arg(toQString(function)).arg(toQString(file)).arg(line);
    qWarning() << QString("Expression: %1").arg(toQString(expression));
    // 必须抛出异常，否则无法定位错误位置
    throw 1;
}
static void myPurecallHandler(void)
{
    qWarning() << "In _purecall_handler.";
    // 必须抛出异常，否则无法定位错误位置
    throw 1;
}

static void InstallFaultHandler()
{
    QDir dir;
    dir.mkdir("crash");
    qDebug() <<"SetUnhandledExceptionFilter";
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER) Win32FaultHandler);
    /// 这样才能捕获栈溢出的错误
    BOOL bRet = PreventSetUnhandledExceptionFilter();
    qDebug() << QString("Prevented: %1").arg(bRet?"TRUE":"FALSE");
    /// 不然将弹框等待用户确认，守护进程不起作用
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    _set_abort_behavior(0,_WRITE_ABORT_MSG);

    /// 解决printf(NULL)这种参数错误没捕获的问题，但是没效果
    _invalid_parameter_handler oldHandler;
    oldHandler = _set_invalid_parameter_handler(myInvalidParameterHandler);
    /// 解决调用纯虚函数的问题
    _purecall_handler old_pure_handle;
    old_pure_handle = _set_purecall_handler(myPurecallHandler);
}


}

#else
namespace utiles{
static void InstallFaultHandler(){
}
}
#endif
