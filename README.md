# E2EE Messaging Backend
This is the backend for an end-to-end encrypted messaging application. It is built using the Drogon C++ framework and PostgreSQL for data storage.

## Authentication Flow
```
mermaid
sequenceDiagram
    participant Client as Client (App)
    participant Server as Server (Drogon + PostgreSQL)
    participant DB as Database

    Client->>Server: Register(username)
    Server->>DB: INSERT INTO users
    Server-->>Client: user_id

    Client->>Client: Génère Identity Key + PreKeys
    Client->>Server: Upload device keys
    Server->>DB: INSERT INTO devices + prekeys

    Client->>Server: Login(username + password OR signature)
    Server->>DB: Vérifie user/device
    Server-->>Client: JWT / Access Token

    Note over Client,Server: Auth OK → l’app peut échanger des messages
```