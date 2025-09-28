CREATE TABLE messages (
  id SERIAL PRIMARY KEY,
  from_user INT REFERENCES users(id),
  to_user INT REFERENCES users(id),
  ciphertext TEXT NOT NULL,
  created_at TIMESTAMP DEFAULT NOW(),
  delivered BOOLEAN DEFAULT FALSE
);