CREATE OR REPLACE FUNCTION detector.add_default_detector_records()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO detector.online (detector_id) VALUES (new.id);
	INSERT INTO detector.info (detector_id) VALUES (new.id);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER trg_after_insert_detector
AFTER INSERT ON detector.system
FOR EACH ROW
EXECUTE FUNCTION detector.add_default_detector_records();
