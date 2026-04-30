CREATE TABLE detector.info(
	detector_id BIGSERIAL NOT NULL UNIQUE,
	display_name TEXT DEFAULT('Unnamed detector') NOT NULL,
	description TEXT,
	location TEXT DEFAULT('Main location') NOT NULL,

	FOREIGN KEY (detector_id) REFERENCES detector.system(id) ON DELETE CASCADE
);