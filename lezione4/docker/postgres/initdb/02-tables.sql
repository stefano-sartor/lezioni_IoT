\c sensor_data mqtt;

CREATE TABLE ambient_sensor (
	id SERIAL NOT NULL, 
	time TIMESTAMP WITH TIME ZONE NOT NULL, 
	sensor_name VARCHAR(32) NOT NULL, 
	temp FLOAT, 
	relhum FLOAT, 
	press FLOAT, 
	PRIMARY KEY (id)
)