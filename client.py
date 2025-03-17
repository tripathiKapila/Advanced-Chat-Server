#!/usr/bin/env python3
import socket
import threading
import sys
import time
import os
import colorama
from colorama import Fore, Back, Style

# Initialize colorama
colorama.init()

# ASCII Art for the welcome screen
WELCOME_ART = """
  _  __          _ _ _       ___ _           _   
 | |/ /__ _ _ __(_) ( )___  / __| |__   __ _| |_ 
 | ' // _` | '_ \ | |// __| \__ \ '_ \ / _` | __|
 | . \ (_| | |_) | |  \__ \ ___) | | | (_| | |_ 
 |_|\_\__,_| .__/|_|  |___/|____/|_|  \__,_|\__|
           |_|                                   
             ENHANCED PYTHON CLIENT
"""

class ChatClient:
    def __init__(self, host='localhost', port=8080, test_mode=False):
        self.host = host
        self.port = port
        self.socket = None
        self.running = False
        self.message_lock = threading.Lock()
        self.username = None
        self.test_mode = test_mode
        
        # Clear screen and show welcome message
        os.system('cls' if os.name == 'nt' else 'clear')
        print(Fore.CYAN + WELCOME_ART + Style.RESET_ALL)
        if not test_mode:
            print(Fore.YELLOW + "Type messages to send to the server. Type 'exit' to quit." + Style.RESET_ALL)
            print(Fore.YELLOW + "=" * 80 + Style.RESET_ALL)
        
    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.running = True
            
            # Print connection message
            self.print_system_message(f"Connected to {self.host}:{self.port}")
            
            # Start a thread to receive messages
            receive_thread = threading.Thread(target=self.receive_messages)
            receive_thread.daemon = True
            receive_thread.start()
            
            # Start sending messages only if not in test mode
            if not self.test_mode:
                self.send_messages()
            
        except Exception as e:
            self.print_error(f"Error connecting to server: {e}")
            self.disconnect()
    
    def disconnect(self):
        self.running = False
        if self.socket:
            self.socket.close()
            self.socket = None
        self.print_system_message("Disconnected from server")
    
    def print_system_message(self, message):
        with self.message_lock:
            timestamp = time.strftime("%H:%M:%S")
            print(f"{Fore.WHITE}[{timestamp}] {Fore.YELLOW}[SYSTEM] {message}{Style.RESET_ALL}")
    
    def print_error(self, message):
        with self.message_lock:
            timestamp = time.strftime("%H:%M:%S")
            print(f"{Fore.WHITE}[{timestamp}] {Fore.RED}[ERROR] {message}{Style.RESET_ALL}")
    
    def print_sent_message(self, message):
        with self.message_lock:
            timestamp = time.strftime("%H:%M:%S")
            print(f"{Fore.WHITE}[{timestamp}] {Fore.MAGENTA}[SENT] {message}{Style.RESET_ALL}")
    
    def print_received_message(self, message):
        with self.message_lock:
            timestamp = time.strftime("%H:%M:%S")
            print(f"{Fore.WHITE}[{timestamp}] {Fore.GREEN}[RECV] {message}{Style.RESET_ALL}")
    
    def receive_messages(self):
        while self.running:
            try:
                data = self.socket.recv(1024)
                if not data:
                    self.print_system_message("Server closed the connection")
                    self.disconnect()
                    break
                
                try:
                    message = data.decode('utf-8', errors='replace').strip()
                    if message:
                        self.print_received_message(message)
                except UnicodeDecodeError as e:
                    self.print_error(f"Received invalid data from server, ignoring")
                    continue
            except Exception as e:
                self.print_error(f"Error receiving message: {e}")
                self.disconnect()
                break
    
    def send_messages(self):
        try:
            while self.running:
                message = input()
                if not self.running:
                    break
                
                if message.startswith('/'):
                    self.processCommand(message)
                elif message:
                    self.socket.sendall(message.encode('utf-8'))
                    self.print_sent_message(message)
        except Exception as e:
            self.print_error(f"Error sending message: {e}")
            self.disconnect()

    def sendMessage(self, message):
        if self.socket and self.running:
            try:
                self.socket.sendall(message.encode('utf-8'))
                self.print_sent_message(message)
                # Add a small delay to ensure message is sent
                time.sleep(0.1)
            except Exception as e:
                self.print_error(f"Error sending message: {e}")
                self.disconnect()
        else:
            self.print_error("Not connected to server")

    def showHelp(self):
        print(Fore.CYAN + """
Available commands:
/listrooms           - List available chat rooms
/createroom <name>   - Create a new chat room
/whisper <id> <msg>  - Send private message to user
/nickname <name>     - Change your nickname
/server [host:port]  - Show or change server address
/help                - Show this help message
/exit or /quit       - Exit the client
""" + Style.RESET_ALL)

    def showConnectionStatus(self, connected, host, port):
        status = "Connected" if connected else "Disconnected"
        color = Fore.GREEN if connected else Fore.RED
        print(f"{color}[STATUS] {status} - Server: {host}:{port}{Style.RESET_ALL}")

    def processCommand(self, command):
        cmd = command
        args = ""
        
        spacePos = command.find(' ')
        if spacePos != -1:
            cmd = command[:spacePos]
            args = command[spacePos + 1:]
        
        if cmd == "/listrooms":
            self.sendMessage(command)
        elif cmd == "/createroom":
            if not args:
                self.print_error("Missing room name. Usage: /createroom <room_name>")
                return
            self.sendMessage(command)
        elif cmd == "/whisper":
            if not args or ' ' not in args:
                self.print_error("Missing parameters. Usage: /whisper <user_id> <message>")
                return
            self.sendMessage(command)
        elif cmd == "/nickname":
            if not args:
                self.print_error("Missing nickname. Usage: /nickname <new_nickname>")
                return
            self.sendMessage(command)
        elif cmd == "/server":
            if not args:
                self.print_system_message("Current server: " + self.host + ":" + str(self.port))
            else:
                # Parse host:port
                colonPos = args.find(':')
                if colonPos != -1:
                    newHost = args[:colonPos]
                    portStr = args[colonPos + 1:]
                    
                    try:
                        newPort = int(portStr)
                        if newPort > 0 and newPort < 65536:
                            if self.running:
                                self.disconnect()
                            
                            self.host = newHost
                            self.port = newPort
                            self.print_system_message("Server set to " + self.host + ":" + str(self.port))
                            self.showConnectionStatus(False, self.host, self.port)
                        else:
                            self.print_error("Invalid port number. Must be between 1 and 65535.")
                    except ValueError:
                        self.print_error("Invalid port number format.")
                else:
                    self.print_error("Invalid format. Use /server host:port")
        elif cmd == "/help":
            self.showHelp()
        elif cmd == "/exit" or cmd == "/quit":
            self.print_system_message("Exiting client...")
            self.disconnect()
            self.running = False
        else:
            # Forward other commands to server if connected
            if self.running:
                self.sendMessage(command)
            else:
                self.print_error("Unknown command: " + cmd)
                self.print_system_message("Type /help for available commands.")

def run_test_cases():
    client = ChatClient(test_mode=True)
    print("\nRunning test cases...\n")
    
    # Test Case 1: Connect to server
    print(Fore.CYAN + "Test Case 1: Connecting to server..." + Style.RESET_ALL)
    client.connect()
    
    # Wait for welcome message and extract session ID
    time.sleep(1)
    session_id = None
    
    # Test Case 2: List rooms
    print(Fore.CYAN + "\nTest Case 2: Listing rooms..." + Style.RESET_ALL)
    client.sendMessage("/listrooms")
    time.sleep(1)
    
    # Test Case 3: Create a room (using a unique name)
    print(Fore.CYAN + "\nTest Case 3: Creating a room..." + Style.RESET_ALL)
    room_name = f"TestRoom_{int(time.time())}"  # Create unique room name
    client.sendMessage(f"/createroom {room_name}")
    time.sleep(1)
    
    # Test Case 4: Change nickname
    print(Fore.CYAN + "\nTest Case 4: Changing nickname..." + Style.RESET_ALL)
    client.sendMessage("/nickname TestUser")
    time.sleep(1)
    
    # Test Case 5: Send a whisper (using the correct session ID format)
    print(Fore.CYAN + "\nTest Case 5: Sending a whisper..." + Style.RESET_ALL)
    client.sendMessage("/whisper user_1 Hello there!")
    time.sleep(1)
    
    # Test Case 6: Show help
    print(Fore.CYAN + "\nTest Case 6: Showing help..." + Style.RESET_ALL)
    client.showHelp()
    
    print(Fore.CYAN + "\nTest cases completed. You can now interact with the server normally." + Style.RESET_ALL)
    
    # Wait a moment before disconnecting
    time.sleep(1)
    
    # Clean disconnect
    if client.socket:
        try:
            client.socket.sendall("/quit".encode('utf-8'))
            time.sleep(0.5)  # Wait for quit command to be processed
        except:
            pass
    client.disconnect()

if __name__ == "__main__":
    host = 'localhost'
    port = 8080
    
    if len(sys.argv) > 1:
        host = sys.argv[1]
    if len(sys.argv) > 2:
        port = int(sys.argv[2])
    
    if "--test" in sys.argv:
        run_test_cases()
    else:
        client = ChatClient(host, port)
        client.connect() 