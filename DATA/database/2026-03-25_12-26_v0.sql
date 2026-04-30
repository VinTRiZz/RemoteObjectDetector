CREATE TABLE detector.online(
	detector_id BIGSERIAL NOT NULL UNIQUE,
	total BIGINT NOT NULL,
	last_utc BIGINT NOT NULL,

	FOREIGN KEY (detector_id) REFERENCES detector.system(id) ON DELETE CASCADE
);