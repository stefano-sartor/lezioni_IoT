CREATE USER grafana WITH ENCRYPTED PASSWORD 'password_provvisoria';
CREATE DATABASE db_grafana;
GRANT ALL PRIVILEGES ON DATABASE db_grafana TO grafana;

CREATE DATABASE sensor_data;
CREATE USER mqtt WITH ENCRYPTED PASSWORD 'mqttPassword';
GRANT ALL PRIVILEGES ON DATABASE sensor_data TO mqtt;
