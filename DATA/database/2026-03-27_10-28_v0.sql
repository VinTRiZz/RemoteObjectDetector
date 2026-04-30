DROP TABLE detector.info;
CREATE TABLE detector.info(
	detector_id 	BIGINT NOT NULL UNIQUE,
	display_name 	TEXT DEFAULT('556e6e616d6564206465746563746f720a') NOT NULL,
	description 	TEXT,
	location 		TEXT DEFAULT('4d61696e206c6f636174696f6e0a') NOT NULL,

	FOREIGN KEY (detector_id) REFERENCES detector.system(id) ON DELETE CASCADE
);