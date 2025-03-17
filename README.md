# Advanced Chat Server

A C++ chat server implementation with advanced features, designed to showcase modern C++ programming techniques and design patterns.

## Features

- **Command Pattern**: Implements a command system for handling user commands
- **Chat Rooms**: Support for multiple chat rooms with join/leave functionality
- **Private Messaging**: Whisper command for private communication
- **Multi-threaded**: Uses thread pool for handling multiple connections
- **Asynchronous I/O**: Uses Boost.Asio for non-blocking I/O operations
- **Session Management**: Tracks user sessions and handles timeouts
- **Logging System**: Custom logging implementation

## Available Commands

- `/join <room_name>` - Join a chat room
- `/leave <room_name>` - Leave a chat room
- `/listrooms` - List all available chat rooms
- `/createroom <room_name>` - Create a new chat room
- `/whisper <session_id> <message>` - Send a private message to another user
- `/listusers <room_name>` - List all users in a chat room
- `/nickname <new_nickname>` - Change your display name
- `/help` - Display available commands

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler
- Boost libraries (system, thread)

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running the Server

```bash
cd build
./AdvancedChatServer
```

The server will start and listen on port 8080 by default.

## Connecting to the Server

You can use the provided Python client to connect to the server:

```bash
python client.py
```

Or use telnet:

```bash
telnet localhost 8080
```

## Project Structure

- `src/` - Source code directory
  - `main.cpp` - Entry point
  - `Command.hpp/cpp` - Command pattern implementation
  - `Commands.hpp/cpp` - Specific command implementations
  - `ChatRoom.hpp/cpp` - Chat room implementation
  - `Session.hpp/cpp` - Client session handling
  - `SessionManager.hpp/cpp` - Session management
  - `UserManager.hpp/cpp` - User management
  - `Logging.hpp/cpp` - Logging system

## Design Patterns Used

- **Command Pattern**: For handling user commands
- **Singleton Pattern**: For session and logging managers
- **Factory Pattern**: For creating chat rooms and sessions
- **Observer Pattern**: For message broadcasting

## Advanced C++ Features

- Smart pointers for memory management
- Lambda expressions for callbacks
- Thread safety with mutexes
- Asynchronous programming with Boost.Asio
- Modern C++17 features 