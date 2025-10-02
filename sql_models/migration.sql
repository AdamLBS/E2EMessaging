CREATE EXTENSION IF NOT EXISTS "pgcrypto";

CREATE TABLE users (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  username TEXT UNIQUE NOT NULL,
  created_at TIMESTAMP DEFAULT NOW()
);

CREATE TABLE messages (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  from_user UUID REFERENCES users(id),
  to_user UUID REFERENCES users(id),
  ciphertext TEXT NOT NULL,
  created_at TIMESTAMP DEFAULT NOW(),
  delivered BOOLEAN DEFAULT FALSE
);

CREATE TABLE devices (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  user_id UUID REFERENCES users(id) ON DELETE CASCADE,
  identity_pubkey TEXT NOT NULL, 
  device_label TEXT,
  push_token TEXT,
  last_seen TIMESTAMP DEFAULT NOW(),
  created_at TIMESTAMP DEFAULT NOW(),
  UNIQUE(user_id, identity_pubkey)
);

CREATE TABLE signed_prekeys (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  device_id UUID REFERENCES devices(id) ON DELETE CASCADE,
  key TEXT NOT NULL,                     -- SPK publique
  signature TEXT NOT NULL,               -- sig(SPK, IK_priv)
  created_at TIMESTAMP DEFAULT NOW()
);

CREATE TABLE one_time_prekeys (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  device_id UUID REFERENCES devices(id) ON DELETE CASCADE,
  key TEXT NOT NULL,
  used BOOLEAN DEFAULT FALSE,
  created_at TIMESTAMP DEFAULT NOW()
);