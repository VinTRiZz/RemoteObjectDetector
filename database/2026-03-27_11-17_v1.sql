ALTER TABLE detector.system 
ADD CONSTRAINT cnstr_id_not_zero CHECK (id <> 0);

ALTER TABLE versions.system 
ADD CONSTRAINT cnstr_id_not_zero CHECK (id <> 0);