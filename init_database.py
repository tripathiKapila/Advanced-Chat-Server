#!/usr/bin/env python3
import sqlite3
import os

def init_database():
    # Remove existing database if it exists
    if os.path.exists('chat.db'):
        os.remove('chat.db')
    
    # Create a new database
    conn = sqlite3.connect('chat.db')
    cursor = conn.cursor()
    
    print("Creating database tables...")
    
    # Create Users table
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS users (
        user_id TEXT PRIMARY KEY,
        nickname TEXT,
        session_id TEXT UNIQUE,
        last_seen TIMESTAMP
    )
    ''')
    
    # Create Chat Rooms table
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS chat_rooms (
        room_id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT UNIQUE NOT NULL,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
    ''')
    
    # Create Messages table
    cursor.execute('''
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
    
    # Create Room Members table
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS room_members (
        room_id INTEGER,
        user_id TEXT,
        joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        PRIMARY KEY (room_id, user_id),
        FOREIGN KEY (room_id) REFERENCES chat_rooms(room_id),
        FOREIGN KEY (user_id) REFERENCES users(user_id)
    )
    ''')
    
    # Create default 'general' chat room
    cursor.execute('''
    INSERT INTO chat_rooms (name) VALUES ('general')
    ''')
    
    # Commit the changes
    conn.commit()
    
    print("Database initialized successfully!")
    print("\nCreated tables:")
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table'")
    tables = cursor.fetchall()
    for table in tables:
        print(f"- {table[0]}")
    
    conn.close()

if __name__ == "__main__":
    init_database() 