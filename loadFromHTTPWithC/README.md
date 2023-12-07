# Load From HTTP with C

Loading shell code from HTTP server, Using C Programming Language.

## Technical

Send a HTTP Request, receive binary shellcode, bit by bit copy into buffer.

When receive finish, copy buffer into `VirtualAlloc` and jump into shellcode.

And I don't think I am good in programming.