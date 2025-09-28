CREATE TABLE devices (
  id SERIAL PRIMARY KEY,
  user_id INT REFERENCES users(id),
  identity_pubkey TEXT NOT NULL,
  push_token TEXT,
  last_seen TIMESTAMP DEFAULT NOW()
);