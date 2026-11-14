CREATE DATABASE IF NOT EXISTS animalfight
  DEFAULT CHARACTER SET utf8mb4
  DEFAULT COLLATE utf8mb4_unicode_ci;

USE animalfight;

CREATE TABLE IF NOT EXISTS users (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  username VARCHAR(20) NOT NULL,
  password_hash CHAR(64) NOT NULL,
  password_salt CHAR(32) NOT NULL,
  score INT NOT NULL DEFAULT 0,
  level TINYINT UNSIGNED NOT NULL DEFAULT 1,
  wins INT UNSIGNED NOT NULL DEFAULT 0,
  losses INT UNSIGNED NOT NULL DEFAULT 0,
  draws INT UNSIGNED NOT NULL DEFAULT 0,
  created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (id),
  UNIQUE KEY uq_users_username (username),
  KEY idx_users_score (score DESC, id ASC)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS game_records (
  id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  room_id VARCHAR(12) NOT NULL,
  player_a_id BIGINT UNSIGNED NOT NULL,
  player_b_id BIGINT UNSIGNED NOT NULL,
  winner_id BIGINT UNSIGNED NULL,
  result_reason ENUM('normal', 'timeout', 'disconnect', 'draw') NOT NULL,
  started_at DATETIME NOT NULL,
  finished_at DATETIME NOT NULL,
  PRIMARY KEY (id),
  KEY idx_records_a (player_a_id, finished_at DESC),
  KEY idx_records_b (player_b_id, finished_at DESC),
  CONSTRAINT fk_record_a FOREIGN KEY (player_a_id) REFERENCES users(id),
  CONSTRAINT fk_record_b FOREIGN KEY (player_b_id) REFERENCES users(id),
  CONSTRAINT fk_record_winner FOREIGN KEY (winner_id) REFERENCES users(id)
) ENGINE=InnoDB;

