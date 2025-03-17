#!/usr/bin/env python3
import sqlite3
import os

def init_database():
    # Remove existing database if it exists
    if os.path.exists('chat.db'):
        os.remove('chat.db')
    
    # Create a new database
    db_connection = sqlite3.connect('chat.db')
    db_handler = db_connection.cursor()
    
    # Create tables
    db_handler.execute('''
    CREATE TABLE IF NOT EXISTS users (
        user_id TEXT PRIMARY KEY,
        nickname TEXT,
        session_id TEXT UNIQUE,
        last_seen TIMESTAMP
    )
    ''')
    
    db_handler.execute('''
    CREATE TABLE IF NOT EXISTS chat_rooms (
        room_id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT UNIQUE NOT NULL,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
    ''')
    
    db_handler.execute('''
    CREATE TABLE IF NOT EXISTS messages (
        message_id INTEGER PRIMARY KEY AUTOINCREMENT,
        room_id INTEGER,
        user_id TEXT,
        content TEXT,
        timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        FOREIGN KEY (room_id) REFERENCES chat_rooms(room_id),
        FOREIGN KEY (user_id) REFERENCES users(user_id)
    )
    ''')
    
    db_handler.execute('''
    CREATE TABLE IF NOT EXISTS room_members (
        room_id INTEGER,
        user_id TEXT,
        joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        PRIMARY KEY (room_id, user_id),
        FOREIGN KEY (room_id) REFERENCES chat_rooms(room_id),
        FOREIGN KEY (user_id) REFERENCES users(user_id)
    )
    ''')
    
    db_connection.commit()
    db_connection.close()

def insert_test_data():
    db_connection = sqlite3.connect('chat.db')
    db_handler = db_connection.cursor()
    
    # Insert sample users
    users = [
        ('user_1', 'Alice', 'session_1'),
        ('user_2', 'Bob', 'session_2'),
        ('user_3', 'Charlie', 'session_3')
    ]
    db_handler.executemany('INSERT INTO users (user_id, nickname, session_id) VALUES (?, ?, ?)', users)
    
    # Insert sample chat rooms
    rooms = [
        ('general',),
        ('random',)
    ]
    db_handler.executemany('INSERT INTO chat_rooms (name) VALUES (?)', rooms)
    
    # Insert sample messages
    messages = [
        (1, 'user_1', 'Hello everyone!'),
        (1, 'user_2', 'Hi Alice!'),
        (2, 'user_3', 'This is a random message.')
    ]
    db_handler.executemany('INSERT INTO messages (room_id, user_id, content) VALUES (?, ?, ?)', messages)
    
    # Insert sample room members
    members = [
        (1, 'user_1'),
        (1, 'user_2'),
        (2, 'user_3')
    ]
    db_handler.executemany('INSERT INTO room_members (room_id, user_id) VALUES (?, ?)', members)
    
    db_connection.commit()
    db_connection.close()

def validate_data():
    db_connection = sqlite3.connect('chat.db')
    db_handler = db_connection.cursor()
    
    # Validate users
    db_handler.execute("SELECT * FROM users")
    users = db_handler.fetchall()
    print("Users:")
    for user in users:
        print(user)
    
    # Validate chat rooms
    db_handler.execute("SELECT * FROM chat_rooms")
    rooms = db_handler.fetchall()
    print("\nChat Rooms:")
    for room in rooms:
        print(room)
    
    # Validate messages
    db_handler.execute("SELECT * FROM messages")
    messages = db_handler.fetchall()
    print("\nMessages:")
    for message in messages:
        print(message)
    
    # Validate room members
    db_handler.execute("SELECT * FROM room_members")
    members = db_handler.fetchall()
    print("\nRoom Members:")
    for member in members:
        print(member)
    
    db_connection.close()

def cleanup():
    if os.path.exists('chat.db'):
        os.remove('chat.db')
        print("Cleaned up the database.")

if __name__ == "__main__":
    cleanup()  # Clean up any existing database
    init_database()  # Initialize the database
    insert_test_data()  # Insert test data
    validate_data()  # Validate the inserted data