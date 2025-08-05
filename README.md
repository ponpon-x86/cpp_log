# cpp_log

This project consists of a core library, **ultralib**, and two applications: **cpp_log** (the main application) and **socket_app**.

The **ultralib** library contains the core logic for logging user input to either a file or a network socket. It uses the C++ Standard Library (STL) and platform-specific system calls on Windows and Linux to handle socket communication.

The **cpp_log** application uses the library to capture user input and log it based on the selected destination -- either to a file or a socket.

The **socket_app** connects to a listening socket, receives messages sent by **cpp_log**, and tracks statistics related to the received data.

## Table of Contents

- [Build Details](#build-details)
- [Installation](#installation)
- [Usage](#usage)

## Build Details

- written in C++;
- uses CMake *(version 3.10+ recommended)*;
- cross-platform: tested on Windows and Linux;
- no external dependencies.

## Installation

Follow the instructions below to build the project on your platform.

### Prerequisites

- Make sure `cmake` and a C++ compiler are installed. If not:
  -  **Windows**: Install `cmake` and either `MinGW` or `Visual Studio` from official sources
  -  **Linux**: Build from sources or install via your package manager:
    
      ```bash
      sudo apt install build-essential cmake
      ```

### Windows 

#### Option 1: Using Visual Studio
1. Download the repository from website or, if you have git installed, clone it in the Command Prompt:

   ```bash
   git clone https://github.com/ponpon-x86/cpp_log
   ```
3. Open the project in Visual Studio.
4. Build the project (Debug or Release).
5. The binaries will be located in bin/Debug or bin/Release

#### Option 2: Using Command Prompt
Open a terminal and run:
```bash
git clone https://github.com/ponpon-x86/cpp_log
cd cpp_log
mkdir build
cd build
cmake ..  # or: cmake -DBUILD_SHARED_LIBS=OFF ..
cmake --build . --config Release # for release configuration
```

### Linux
Open a terminal and run:
```bash
git clone https://github.com/ponpon-x86/cpp_log
cd cpp_log
mkdir build
cd build
cmake ..  # or: cmake -DBUILD_SHARED_LIBS=OFF ..
make  # or: cmake --build .
```
After building, you should see a bin/ folder in the project directory containing the compiled binaries and library.

## Usage

Both applications include basic input validation: if started with incorrect arguments, they will terminate and display a message explaining the issue.

### Running **cpp_log**

You must provide **at least two command-line arguments**:
1. A log file name.
2. A default priority level.
3. *(OPTIONAL)* An `ip:port` address on which the library will listen *(defaults to `127.0.0.1:60420` if omitted)*.

#### Priority levels present:
- `-r`: regular
- `-i`: important
- `-c`: critical

If the priority of a message is lower than the default, it will *not* be logged. If a message has no explicit priority, the default one will be used.

#### Examples

Start **cpp_log** with default priority **regular**, logging to `log.txt`, and listening on default IP/port:

```bash
./cpp_log log.txt -r
```

Start with priority **critical**, logging to `ultralog.txt`, and explicitly specifying the socket endpoint:

```bash
./cpp_log ultralog.txt -c 127.0.0.1:60420
```

#### During runtime

Once running, the app allows to:
- change the default priority;
- switch logging destination (file or socket);
- exit the program.

> **_NOTE:_** There may be a slight delay in detecting if the client (**socket_app**) has disconnected. This is due to the ping-pong mechanism operating at large fixed intervals (every few seconds, not milliseconds).

> **_NOTE:_** On Linux, the terminal may produce escape sequences *(e.g. when using arrow keys)*. These sequences will be automatically filtered out of messages to ensure consistent and intended behaviour.

### Running **socket_app**

You must provide **three command-line arguments**:
1. The `ip:port` to connect to *(no default is provided)*.
2. The number of messages to receive before updating statistics.
3. The time interval (in seconds) before updating statistics.

#### Functionality

The application:
- connects to **cpp_log** via socket;
- displays the last received message;
- tracks statistics about incoming messages;
- displays the current state of its connection.

Statistics reported *(these statistics update to new ones either every N messages, or T seconds; both are set on startup)*:
- total messages received;
- messages by priority:
  - regular,
  - important,
  - critical;
- messages received in the last hour;
- maximum message length;
- minimum message length;
- average message length.

#### Example

Connect to 127.0.0.1:60420, update statistics every **3 messages** or every **5 seconds** (whichever comes first):
```bash
./socket_app 127.0.0.1:60420 3 5
```

> **_NOTE:_** **socket_app** runs continuously and does not include a built-in exit command.
> To terminate it, use one of the following methods:
> - press `Ctrl+C` in the terminal window where it's running;
> - use **Task Manager** (on Windows);
> - run `kill` (on Linux) with the process ID.
