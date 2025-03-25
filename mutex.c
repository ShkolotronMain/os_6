#include <stdio.h>
#include <Windows.h>

// Общие данные

int g_nindex = 0;
#define MAX_TIMES 100
DWORD g_dwtimes[MAX_TIMES];

HANDLE g_hmutex = NULL;

// Программная секция
DWORD WINAPI inflation1()
{
    DWORD mtx;
    while (g_nindex < MAX_TIMES)
    {
        mtx = WaitForSingleObject(g_hmutex, INFINITE);
        if (mtx == WAIT_OBJECT_0)
        {
            g_dwtimes[g_nindex] = GetTickCount();
            Sleep(10);
            g_nindex++;

            ReleaseMutex(g_hmutex);
        }
        else
            break;
    }
    return 0;
}

DWORD WINAPI inflation2()
{
    DWORD mtx;
    while (g_nindex < MAX_TIMES)
    {
        mtx = WaitForSingleObject(g_hmutex, INFINITE);
        if (mtx == WAIT_OBJECT_0)
        {
            g_nindex++;
            g_dwtimes[g_nindex] = GetTickCount();
            Sleep(10);
            
            ReleaseMutex(g_hmutex);
        }
        else
            break;
    }
    return 0;
}

int main()
{
    g_hmutex = CreateMutex(NULL, FALSE, NULL);

    HANDLE threads[2];
    DWORD ids[2];
    threads[0] = CreateThread(NULL, 0, inflation1, NULL, 0, &ids[0]);
    threads[1] = CreateThread(NULL, 0, inflation2, NULL, 0, &ids[1]);
    WaitForMultipleObjects(2, threads, 1, INFINITE);
    CloseHandle(threads[0]);
    CloseHandle(threads[1]);

    for (int i=0; i<MAX_TIMES; i++)
    {
        printf("%d ", g_dwtimes[i]);
        if (!((i+1)%10) && i)
            printf("\n");
    }

    CloseHandle(g_hmutex);
}