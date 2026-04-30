-- Fix BIGSERIAL
DROP TABLE detector.online;
CREATE TABLE detector.online(
	detector_id BIGINT NOT NULL UNIQUE,
	total 		BIGINT NOT NULL DEFAULT 0,
	last_utc 	BIGINT NOT NULL DEFAULT 0,

	FOREIGN KEY (detector_id) REFERENCES detector.system(id) ON DELETE CASCADE
);

-- Fix BIGSERIAL
DROP TABLE detector.info;
CREATE TABLE detector.info(
	detector_id 	BIGINT NOT NULL UNIQUE,
	display_name 	TEXT DEFAULT('Unnamed detector') NOT NULL,
	description 	TEXT,
	location 		TEXT DEFAULT('Main location') NOT NULL,

	FOREIGN KEY (detector_id) REFERENCES detector.system(id) ON DELETE CASCADE
);

-- Fix BIGSERIAL
DROP TABLE versions.meta;
CREATE TABLE versions.meta(
	version_id 		BIGINT NOT NULL,
	upload_time_utc BIGINT NOT NULL,
	version_string 	TEXT NOT NULL,
	alias_name_hex 	TEXT,

	FOREIGN KEY (version_id) REFERENCES versions.system(id) ON DELETE CASCADE
);


-- Create software table
CREATE TABLE detector.software(
	detector_id 	BIGINT 	NOT NULL UNIQUE,
	version_id 		BIGINT,
	update_time_utc BIGINT,

	FOREIGN KEY (detector_id) REFERENCES detector.system(id) ON DELETE CASCADE,
	FOREIGN KEY (version_id)  REFERENCES versions.system(id) ON DELETE SET NULL
);


-- Update detector insert trigger
CREATE OR REPLACE FUNCTION detector.add_default_detector_records()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO detector.online 	(detector_id) VALUES (new.id);
	INSERT INTO detector.info   	(detector_id) VALUES (new.id);
	INSERT INTO detector.software   (detector_id) VALUES (new.id);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;
