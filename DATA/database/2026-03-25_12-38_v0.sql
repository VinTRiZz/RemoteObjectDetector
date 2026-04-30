CREATE TABLE versions.meta(
	version_id BIGSERIAL NOT NULL,
	upload_time_utc BIGINT NOT NULL,
	version_string TEXT NOT NULL,
	alias_name_hex TEXT,

	FOREIGN KEY (version_id) REFERENCES versions.system(id) ON DELETE CASCADE
);