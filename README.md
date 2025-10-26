# 🛑 E2EE Messaging Backend — *Archived Project*

> ⚠️ **This repository is now archived.**  
> The project now continues under a new name: **[HushNet](https://github.com/HushNet/)** — a modern, federated, end-to-end encrypted messaging ecosystem built from this foundation.

---
## Authentication Flow
```mermaid
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


## Communication Flow
```mermaid
sequenceDiagram
    participant AliceDevice as Alice (Device A)
    participant Server as Server
    participant BobPhone as Bob (Phone)
    participant BobLaptop as Bob (Laptop)

    Note over AliceDevice,BobLaptop: Phase 1 - Key discovery for multi-device

    AliceDevice->>Server: Request IK, SPK, OTPKs of all Bob's devices
    Server-->>AliceDevice: Returns {IK, SPK, OTPK} for BobPhone and BobLaptop
    Server->>Server: Marks OTPKs of BobPhone and BobLaptop as "used"

    Note over AliceDevice,BobLaptop: Phase 2 - Session setup

    AliceDevice->>AliceDevice: Compute shared secret with BobPhone (IK, SPK, OTPK)
    AliceDevice->>AliceDevice: Compute shared secret with BobLaptop (IK, SPK, OTPK)

    AliceDevice->>Server: Send First Message for BobPhone (encrypted with Phone session key)
    AliceDevice->>Server: Send First Message for BobLaptop (encrypted with Laptop session key)

    Server-->>BobPhone: Deliver ciphertext + ratchet header
    Server-->>BobLaptop: Deliver ciphertext + ratchet header

    BobPhone->>BobPhone: Use SPK_priv + OTPK_priv to derive shared secret
    BobLaptop->>BobLaptop: Use SPK_priv + OTPK_priv to derive shared secret
    BobPhone->>BobPhone: Destroy consumed OTPK
    BobLaptop->>BobLaptop: Destroy consumed OTPK

    Note over AliceDevice,BobLaptop: Phase 3 - Independent Double Ratchets

    AliceDevice->>AliceDevice: Initialize Double Ratchet for Phone
    AliceDevice->>AliceDevice: Initialize Double Ratchet for Laptop
    BobPhone->>BobPhone: Initialize Double Ratchet (Phone session)
    BobLaptop->>BobLaptop: Initialize Double Ratchet (Laptop session)

    AliceDevice->>Server: Send Message 2 (for BobPhone)
    AliceDevice->>Server: Send Message 2 (for BobLaptop)
    Server-->>BobPhone: Deliver ciphertext
    Server-->>BobLaptop: Deliver ciphertext
    BobPhone->>BobPhone: Decrypt with ratchet key (Phone session)
    BobLaptop->>BobLaptop: Decrypt with ratchet key (Laptop session)

    Note over AliceDevice,BobLaptop: Each device has its own Double Ratchet,<br/>so message history and ratchets are isolated per device.
```
