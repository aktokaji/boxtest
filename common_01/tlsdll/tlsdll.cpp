#include "tlsdll.h"


Tlsdll::Tlsdll()
{
}

int test_add(int x, int y)
{
    return x+y;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
    printf("DllMain(): %u\n", fdwReason);
    switch(fdwReason)
    {
        case    DLL_PROCESS_ATTACH:
            /*  ほげほげ    */
            break;

        case    DLL_PROCESS_DETACH:
            /*  ほげほげ    */
            break;

        case    DLL_THREAD_ATTACH:
            /*  ほげほげ    */
            break;

        case    DLL_THREAD_DETACH:
            /*  ほげほげ    */
            break;
    }
    return  TRUE;
    //return  FALSE;
}
