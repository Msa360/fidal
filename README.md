# Fidal

## The simplest file transfer command line program

- ### &check; Super simple
- ### &check; TCP
- ### &check; Quick start

## Usage
```shell
fidal listen [port]            # start server in current directory
fidal get [ip:port] [filename] # get a file from a server instance
fidal ls [ip:port] [dirname]   # see what files are available
```

using tcp/ip to transfer data over network

see [docs/headers.txt](docs/headers.txt) to understand the protocol

### todo:

- transfer progress bar
- encryption
- passwords to connect to servers
- add .fidalignore file functionality to protect certain files
