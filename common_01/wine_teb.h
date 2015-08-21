#ifndef _WINE_TEB_H_
#define _WINE_TEB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

typedef struct _WINE_LIST_ENTRY {
    struct _WINE_LIST_ENTRY *Flink;
    struct _WINE_LIST_ENTRY *Blink;
} WINE_LIST_ENTRY, *PWINE_LIST_ENTRY; //, * RESTRICTED_POINTER PRWINE_LIST_ENTRY;

typedef struct _WINE_UNICODE_STRING {
    USHORT Length;        /* bytes */
    USHORT MaximumLength; /* bytes */
    PWSTR  Buffer;
} WINE_UNICODE_STRING, *PWINE_UNICODE_STRING;

typedef struct _WINE_RTL_CRITICAL_SECTION_DEBUG
{
    WORD   Type;
    WORD   CreatorBackTraceIndex;
    struct _WINE_RTL_CRITICAL_SECTION *CriticalSection;
    WINE_LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
#ifdef __WINESRC__  /* in Wine we store the name here */
    DWORD_PTR Spare[8/sizeof(DWORD_PTR)];
#else
    DWORD Spare[ 2 ];
#endif
} WINE_RTL_CRITICAL_SECTION_DEBUG, *PWINE_RTL_CRITICAL_SECTION_DEBUG, WINE_RTL_RESOURCE_DEBUG, *PWINE_RTL_RESOURCE_DEBUG;

typedef struct _WINE_RTL_CRITICAL_SECTION {
    PWINE_RTL_CRITICAL_SECTION_DEBUG DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;
}  WINE_RTL_CRITICAL_SECTION, *PWINE_RTL_CRITICAL_SECTION;

struct _WINE_EXCEPTION_REGISTRATION_RECORD;

typedef DWORD (*WINE_PEXCEPTION_HANDLER)(PEXCEPTION_RECORD,struct _WINE_EXCEPTION_REGISTRATION_RECORD*,
                                    PCONTEXT,struct _WINE_EXCEPTION_REGISTRATION_RECORD **);

typedef struct _WINE_EXCEPTION_REGISTRATION_RECORD
{
    struct _WINE_EXCEPTION_REGISTRATION_RECORD *Prev;
    WINE_PEXCEPTION_HANDLER       Handler;
} WINE_EXCEPTION_REGISTRATION_RECORD;

typedef struct _WINE_NT_TIB
{
    struct _WINE_EXCEPTION_REGISTRATION_RECORD *ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID SubSystemTib;
    union {
          PVOID FiberData;
          DWORD Version;
    } DUMMYUNIONNAME;
    PVOID ArbitraryUserPointer;
    struct _WINE_NT_TIB *Self;
} WINE_NT_TIB, *PWINE_NT_TIB;

typedef struct _WINE_CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} WINE_CLIENT_ID, *PWINE_CLIENT_ID;

typedef struct _WINE_CURDIR
{
    WINE_UNICODE_STRING DosPath;
    PVOID Handle;
} WINE_CURDIR, *PWINE_CURDIR;

typedef struct WINE_RTL_DRIVE_LETTER_CURDIR
{
    USHORT              Flags;
    USHORT              Length;
    ULONG               TimeStamp;
    WINE_UNICODE_STRING DosPath;
} WINE_RTL_DRIVE_LETTER_CURDIR, *PWINE_RTL_DRIVE_LETTER_CURDIR;

typedef struct _WINE_RTL_BITMAP {
    ULONG  SizeOfBitMap; /* Number of bits in the bitmap */
    PULONG Buffer; /* Bitmap data, assumed sized to a DWORD boundary */
} WINE_RTL_BITMAP, *PWINE_RTL_BITMAP;

typedef struct _WINE_PEB_LDR_DATA
{
    ULONG               Length;
    BOOLEAN             Initialized;
    PVOID               SsHandle;
    WINE_LIST_ENTRY     InLoadOrderModuleList;
    WINE_LIST_ENTRY     InMemoryOrderModuleList;
    WINE_LIST_ENTRY     InInitializationOrderModuleList;
    PVOID               EntryInProgress;
} WINE_PEB_LDR_DATA, *PWINE_PEB_LDR_DATA;

typedef struct _WINE_GDI_TEB_BATCH
{
    ULONG  Offset;
    HANDLE HDC;
    ULONG  Buffer[0x136];
} WINE_GDI_TEB_BATCH;

// wine-1.7.35\dlls\ntdll\actctx.c
#if 0
typedef struct _WINE_ACTIVATION_CONTEXT
{
    ULONG               magic;
    int                 ref_count;
    struct file_info    config;
    struct file_info    appdir;
    struct assembly    *assemblies;
    unsigned int        num_assemblies;
    unsigned int        allocated_assemblies;
    /* section data */
    DWORD               sections;
    struct strsection_header  *wndclass_section;
    struct strsection_header  *dllredirect_section;
    struct strsection_header  *progid_section;
    struct guidsection_header *tlib_section;
    struct guidsection_header *comserver_section;
    struct guidsection_header *ifaceps_section;
    struct guidsection_header *clrsurrogate_section;
} WINE_ACTIVATION_CONTEXT;
#endif

typedef struct _WINE_RTL_ACTIVATION_CONTEXT_STACK_FRAME
{
    struct _WINE_RTL_ACTIVATION_CONTEXT_STACK_FRAME *Previous;
    struct _WINE_ACTIVATION_CONTEXT                 *ActivationContext;
    ULONG                                       Flags;
} WINE_RTL_ACTIVATION_CONTEXT_STACK_FRAME, *PWINE_RTL_ACTIVATION_CONTEXT_STACK_FRAME;

typedef struct _WINE_ACTIVATION_CONTEXT_STACK
{
    ULONG                                    Flags;
    ULONG                                    NextCookieSequenceNumber;
    WINE_RTL_ACTIVATION_CONTEXT_STACK_FRAME *ActiveFrame;
    WINE_LIST_ENTRY                          FrameListCache;
} WINE_ACTIVATION_CONTEXT_STACK, *PWINE_ACTIVATION_CONTEXT_STACK;

typedef struct _WINE_RTL_USER_PROCESS_PARAMETERS
{
    ULONG               AllocationSize;
    ULONG               Size;
    ULONG               Flags;
    ULONG               DebugFlags;
    HANDLE              ConsoleHandle;
    ULONG               ConsoleFlags;
    HANDLE              hStdInput;
    HANDLE              hStdOutput;
    HANDLE              hStdError;
    WINE_CURDIR         CurrentDirectory;
    WINE_UNICODE_STRING DllPath;
    WINE_UNICODE_STRING ImagePathName;
    WINE_UNICODE_STRING CommandLine;
    PWSTR               Environment;
    ULONG               dwX;
    ULONG               dwY;
    ULONG               dwXSize;
    ULONG               dwYSize;
    ULONG               dwXCountChars;
    ULONG               dwYCountChars;
    ULONG               dwFillAttribute;
    ULONG               dwFlags;
    ULONG               wShowWindow;
    WINE_UNICODE_STRING WindowTitle;
    WINE_UNICODE_STRING Desktop;
    WINE_UNICODE_STRING ShellInfo;
    WINE_UNICODE_STRING RuntimeInfo;
    WINE_RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
} WINE_RTL_USER_PROCESS_PARAMETERS, *PWINE_RTL_USER_PROCESS_PARAMETERS;

typedef struct _WINE_PEB
{                                                                      /* win32/win64 */
    BOOLEAN                           InheritedAddressSpace;             /* 000/000 */
    BOOLEAN                           ReadImageFileExecOptions;          /* 001/001 */
    BOOLEAN                           BeingDebugged;                     /* 002/002 */
    BOOLEAN                           SpareBool;                         /* 003/003 */
    HANDLE                            Mutant;                            /* 004/008 */
    HMODULE                           ImageBaseAddress;                  /* 008/010 */
    PWINE_PEB_LDR_DATA                LdrData;                           /* 00c/018 */
    WINE_RTL_USER_PROCESS_PARAMETERS *ProcessParameters;                 /* 010/020 */
    PVOID                             SubSystemData;                     /* 014/028 */
    HANDLE                            ProcessHeap;                       /* 018/030 */
    PWINE_RTL_CRITICAL_SECTION        FastPebLock;                       /* 01c/038 */
    PVOID /*PPEBLOCKROUTINE*/         FastPebLockRoutine;                /* 020/040 */
    PVOID /*PPEBLOCKROUTINE*/         FastPebUnlockRoutine;              /* 024/048 */
    ULONG                             EnvironmentUpdateCount;            /* 028/050 */
    PVOID                             KernelCallbackTable;               /* 02c/058 */
    ULONG                             Reserved[2];                       /* 030/060 */
    PVOID /*PPEB_FREE_BLOCK*/         FreeList;                          /* 038/068 */
    ULONG                             TlsExpansionCounter;               /* 03c/070 */
    PWINE_RTL_BITMAP                  TlsBitmap;                         /* 040/078 */
    ULONG                             TlsBitmapBits[2];                  /* 044/080 */
    PVOID                             ReadOnlySharedMemoryBase;          /* 04c/088 */
    PVOID                             ReadOnlySharedMemoryHeap;          /* 050/090 */
    PVOID                            *ReadOnlyStaticServerData;          /* 054/098 */
    PVOID                             AnsiCodePageData;                  /* 058/0a0 */
    PVOID                             OemCodePageData;                   /* 05c/0a8 */
    PVOID                             UnicodeCaseTableData;              /* 060/0b0 */
    ULONG                             NumberOfProcessors;                /* 064/0b8 */
    ULONG                             NtGlobalFlag;                      /* 068/0bc */
    LARGE_INTEGER                     CriticalSectionTimeout;            /* 070/0c0 */
    SIZE_T                            HeapSegmentReserve;                /* 078/0c8 */
    SIZE_T                            HeapSegmentCommit;                 /* 07c/0d0 */
    SIZE_T                            HeapDeCommitTotalFreeThreshold;    /* 080/0d8 */
    SIZE_T                            HeapDeCommitFreeBlockThreshold;    /* 084/0e0 */
    ULONG                             NumberOfHeaps;                     /* 088/0e8 */
    ULONG                             MaximumNumberOfHeaps;              /* 08c/0ec */
    PVOID                            *ProcessHeaps;                      /* 090/0f0 */
    PVOID                             GdiSharedHandleTable;              /* 094/0f8 */
    PVOID                             ProcessStarterHelper;              /* 098/100 */
    PVOID                             GdiDCAttributeList;                /* 09c/108 */
    PVOID                             LoaderLock;                        /* 0a0/110 */
    ULONG                             OSMajorVersion;                    /* 0a4/118 */
    ULONG                             OSMinorVersion;                    /* 0a8/11c */
    ULONG                             OSBuildNumber;                     /* 0ac/120 */
    ULONG                             OSPlatformId;                      /* 0b0/124 */
    ULONG                             ImageSubSystem;                    /* 0b4/128 */
    ULONG                             ImageSubSystemMajorVersion;        /* 0b8/12c */
    ULONG                             ImageSubSystemMinorVersion;        /* 0bc/130 */
    ULONG                             ImageProcessAffinityMask;          /* 0c0/134 */
    HANDLE                            GdiHandleBuffer[28];               /* 0c4/138 */
    ULONG                             unknown[6];                        /* 134/218 */
    PVOID                             PostProcessInitRoutine;            /* 14c/230 */
    PWINE_RTL_BITMAP                  TlsExpansionBitmap;                /* 150/238 */
    ULONG                             TlsExpansionBitmapBits[32];        /* 154/240 */
    ULONG                             SessionId;                         /* 1d4/2c0 */
    ULARGE_INTEGER                    AppCompatFlags;                    /* 1d8/2c8 */
    ULARGE_INTEGER                    AppCompatFlagsUser;                /* 1e0/2d0 */
    PVOID                             ShimData;                          /* 1e8/2d8 */
    PVOID                             AppCompatInfo;                     /* 1ec/2e0 */
    WINE_UNICODE_STRING               CSDVersion;                        /* 1f0/2e8 */
    PVOID                             ActivationContextData;             /* 1f8/2f8 */
    PVOID                             ProcessAssemblyStorageMap;         /* 1fc/300 */
    PVOID                             SystemDefaultActivationData;       /* 200/308 */
    PVOID                             SystemAssemblyStorageMap;          /* 204/310 */
    SIZE_T                            MinimumStackCommit;                /* 208/318 */
    PVOID                            *FlsCallback;                       /* 20c/320 */
    WINE_LIST_ENTRY                   FlsListHead;                       /* 210/328 */
    PWINE_RTL_BITMAP                  FlsBitmap;                         /* 218/338 */
    ULONG                             FlsBitmapBits[4];                  /* 21c/340 */
} WINE_PEB, *PWINE_PEB;

typedef struct _WINE_TEB
{                                                                      /* win32/win64 */
    NT_TIB                            Tib;                               /* 000/0000 */
    PVOID                             EnvironmentPointer;                /* 01c/0038 */
    WINE_CLIENT_ID                    ClientId;                          /* 020/0040 */
    PVOID                             ActiveRpcHandle;                   /* 028/0050 */
    PVOID                             ThreadLocalStoragePointer;         /* 02c/0058 */
    PWINE_PEB                         Peb;                               /* 030/0060 */
    ULONG                             LastErrorValue;                    /* 034/0068 */
    ULONG                             CountOfOwnedCriticalSections;      /* 038/006c */
    PVOID                             CsrClientThread;                   /* 03c/0070 */
    PVOID                             Win32ThreadInfo;                   /* 040/0078 */
    ULONG                             Win32ClientInfo[31];               /* 044/0080 used for user32 private data in Wine */
    PVOID                             WOW32Reserved;                     /* 0c0/0100 */
    ULONG                             CurrentLocale;                     /* 0c4/0108 */
    ULONG                             FpSoftwareStatusRegister;          /* 0c8/010c */
    PVOID                             SystemReserved1[54];               /* 0cc/0110 used for kernel32 private data in Wine */
    LONG                              ExceptionCode;                     /* 1a4/02c0 */
    WINE_ACTIVATION_CONTEXT_STACK     ActivationContextStack;            /* 1a8/02c8 */
    BYTE                              SpareBytes1[24];                   /* 1bc/02e8 used for ntdll private data in Wine */
    PVOID                             SystemReserved2[10];               /* 1d4/0300 used for ntdll private data in Wine */
    WINE_GDI_TEB_BATCH                GdiTebBatch;                       /* 1fc/0350 used for vm86 private data in Wine */
    HANDLE                            gdiRgn;                            /* 6dc/0838 */
    HANDLE                            gdiPen;                            /* 6e0/0840 */
    HANDLE                            gdiBrush;                          /* 6e4/0848 */
    WINE_CLIENT_ID                    RealClientId;                      /* 6e8/0850 */
    HANDLE                            GdiCachedProcessHandle;            /* 6f0/0860 */
    ULONG                             GdiClientPID;                      /* 6f4/0868 */
    ULONG                             GdiClientTID;                      /* 6f8/086c */
    PVOID                             GdiThreadLocaleInfo;               /* 6fc/0870 */
    ULONG                             UserReserved[5];                   /* 700/0878 */
    PVOID                             glDispachTable[280];               /* 714/0890 */
    PVOID                             glReserved1[26];                   /* b74/1150 */
    PVOID                             glReserved2;                       /* bdc/1220 */
    PVOID                             glSectionInfo;                     /* be0/1228 */
    PVOID                             glSection;                         /* be4/1230 */
    PVOID                             glTable;                           /* be8/1238 */
    PVOID                             glCurrentRC;                       /* bec/1240 */
    PVOID                             glContext;                         /* bf0/1248 */
    ULONG                             LastStatusValue;                   /* bf4/1250 */
    WINE_UNICODE_STRING               StaticUnicodeString;               /* bf8/1258 used by advapi32 */
    WCHAR                             StaticUnicodeBuffer[261];          /* c00/1268 used by advapi32 */
    PVOID                             DeallocationStack;                 /* e0c/1478 */
    PVOID                             TlsSlots[64];                      /* e10/1480 */
    WINE_LIST_ENTRY                   TlsLinks;                          /* f10/1680 */
    PVOID                             Vdm;                               /* f18/1690 */
    PVOID                             ReservedForNtRpc;                  /* f1c/1698 */
    PVOID                             DbgSsReserved[2];                  /* f20/16a0 */
    ULONG                             HardErrorDisabled;                 /* f28/16b0 */
    PVOID                             Instrumentation[16];               /* f2c/16b8 */
    PVOID                             WinSockData;                       /* f6c/1738 */
    ULONG                             GdiBatchCount;                     /* f70/1740 */
    ULONG                             Spare2;                            /* f74/1744 */
    PVOID                             Spare3;                            /* f78/1748 */
    PVOID                             Spare4;                            /* f7c/1750 */
    PVOID                             ReservedForOle;                    /* f80/1758 */
    ULONG                             WaitingOnLoaderLock;               /* f84/1760 */
    PVOID                             Reserved5[3];                      /* f88/1768 */
    PVOID                            *TlsExpansionSlots;                 /* f94/1780 */
    ULONG                             ImpersonationLocale;               /* f98/1788 */
    ULONG                             IsImpersonating;                   /* f9c/178c */
    PVOID                             NlsCache;                          /* fa0/1790 */
    PVOID                             ShimData;                          /* fa4/1798 */
    ULONG                             HeapVirtualAffinity;               /* fa8/17a0 */
    PVOID                             CurrentTransactionHandle;          /* fac/17a8 */
    PVOID                             ActiveFrame;                       /* fb0/17b0 */
#ifdef _WIN64
    PVOID                             unknown[2];                        /*     17b8 */
#endif
    PVOID                            *FlsSlots;                          /* fb4/17c8 */
} WINE_TEB, *PWINE_TEB;

#ifdef __cplusplus
}
#endif

#endif /* _WINE_TEB_H_ */
