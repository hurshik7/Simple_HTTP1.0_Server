# Simple HTTP1.0 Server

## Purpose
The purpose of this project is to implement a simple HTTP server in C based on RFC 1945: HTTP 1.0. This server will handle basic HTTP 1.0 GET, HEAD, and POST requests. Additionally, it stores the body-data of a POST request into a ndbm database. Also, another purpose of this project is to implement a separate program called “DB Viewer” which shows the contents of the database containing the body of post requests. It uses Ncurses UI, ndbm database.

## Software Requirements
* Clang-tidy
* Cmake
* Dot (This is used by Doxygen)
* Doxygen (Although this is not necessary to run the program, it is intended to be used when building this program, so if you want to build without creating a Doxygen file, edit CMakeLists.txt)
* Standard C17 compiler (e.g. GCC, Clang)
* DC libraries
* Cgreen (cgreen is a framework for unit testing. It's not essential for running both servers)
* **Ncurses**
* **Ndbm** (Gdbm for linux-based OS)

## Platform
tested programs on macOS and Fedora Linux (Fedora 37).

## Obtaining
```
git clone https://github.com/hurshik7/Simple_HTTP1.0_Server.git
```
## Build
First, go into the `source/` directory and build the HTTP server with the following commands.


```
cd source/
cmake -S . -B build
cmake --build build
```


(If the build fails due to missing libraries, please install those libraries first. **For example**, on Fedora OS, if you don’t have gdbm or ncurses libraries, please run following code.) \



```
sudo dnf update
sudo dnf install gdbm-devel
sudo dnf install ncurses-devel
```


Second, go back to the `source/` directory and go into the `source/db_viewer` directory to build the db_viewer.


```
cd ..
cd db_viewer
cmake -S . -B build
cmake --build build 
```


Note: The compiler can be specified by passing one of the following to cmake when configure the project:



* `-DCMAKE_C_COMPILER="gcc"`
* `-DCMAKE_C_COMPILER="clang"`

(For example, `cmake -S . -B build -DCMAKE_C_COMPILER="gcc")`


## Running



* HTTP Server
You are able to configure the port number by passing arguments. Let's assume the current directory is `source/build` when running the HTTP server. Also, If you have a firewall turned on, it is recommended to turn it off. The default port number is 80.
`./http-server`
`./http-server -p <port number>`

* DB Viewer

You can run the DB viewer of the http server with the following command.

`./db_viewer/build/http-server-db-viewer`
Then, you can see the screen like this.
When you select 1. View database, you can see the data of POST requests.

## Feature
### HTTP1.0 Server
* When you run the server you can see a message with the port number. The default port number is `80`.

    ```
    Listening on port 80
    ```


* The server continues to listen for the client unless it is forcibly shut down.

  You can see the message below every 5 seconds.
  ```
  waiting for connections…
  ```
    


* The server reads a request from a client, and it will send a response to the request to the client. This server can handle multiple clients at a time.
* The server handles GET, HEAD, and POST requests of HTTP 1.0 protocol.
* The server stores the body of a POST request into the ndbm database.
* The **root directory** of the server is **the current working directory**.

  For example, let’s assume that the current working directory is `/Users/hurshik123/http_server/build/` then, files which clients want to access such as `index.html` should be under the current working directory (`/Users/hurshik123/http_server/build/index.html`)

* It checks a new client connection with select() and creates a new process by using fork() to handle the HTTP 1.0 request.

### DB Viewer
* The DB viewer shows body data of all POST requests
* It shows Client IP Address, Access time, and data of POST requests which are stored in the ndbm database.
* It uses Ncurses ui so that you can select the options you want. 