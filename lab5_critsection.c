#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

// Общие данные
#define BUF_SIZE 10
int used = 0;
int buffer[BUF_SIZE] = {0};

CRITICAL_SECTION mas;


// Выводит содержимое буфера и процент заполненности
DWORD WINAPI buffer_control()
{
    int running = 1;
    while (running)
    {
        // Вывод буфера
        printf("\n[КОНТРОЛЬ] Содержимое: [");
        for (int i=0; i<BUF_SIZE; i++)
        {
            printf("%d", buffer[i]);
            if (i != BUF_SIZE-1)
                printf(", ");
        }
        printf("]\n");

        // Процент заполненности
        printf("[КОНТРОЛЬ] Буфер заполнен на %.0f%%\n", (used/(float)BUF_SIZE)*100);

        Sleep(2000);
    }

    return 0;
}

DWORD WINAPI get_number()
{
    int running = 1;
    while (running)
    {
        // Если в буфере хоть что-то есть
        if (used)
        {
            EnterCriticalSection(&mas);
            // Ищем ближайшую к началу занятую ячейку
            for (int i=0; i<BUF_SIZE; i++)
            {
                if (buffer[i])
                {
                    printf("\n[Потребитель]: считано число %d из ячейки %d\n",buffer[i], i);
                    buffer[i] = 0;
                    used--;
                    break;
                }
            }
            LeaveCriticalSection(&mas);
        }

        Sleep(825);
    }

    return 0;
}

DWORD WINAPI set_number()
{
    int running = 1;
    // Поток работает
    while (running)
    {
        // Если буфер не заполнен
        if (used != BUF_SIZE)
        {
            EnterCriticalSection(&mas);
            int number = rand()%30 + 1;
            // Ищем свободную ячейку
            for (int i=BUF_SIZE-1; i>=0; i--)
            {
                if (!buffer[i])
                {
                    buffer[i] = number;
                    used++;
                    printf("\n[Производитель]: записано число %d в ячейку %d\n",number, i);
                    break;
                }
            }
            LeaveCriticalSection(&mas);
        }

        Sleep(800);
    }
    
    return 0;
}


int main()
{
    InitializeCriticalSection(&mas);
    // 0 - prod
    // 1 - cons
    // 2 - control
    HANDLE threads[3];
    DWORD ids[3];

    threads[0] = CreateThread(NULL, 0, set_number, NULL, 0, &ids[0]);
    threads[1] = CreateThread(NULL, 0, get_number, NULL, 0, &ids[1]);
    threads[2] = CreateThread(NULL, 0, buffer_control, NULL, 0, &ids[2]);

    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    for(int i=0; i<3; i++)
        CloseHandle(threads[i]);

    DeleteCriticalSection(&mas);
    
    return 0;
}