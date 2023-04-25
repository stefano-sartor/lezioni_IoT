from sqlalchemy import create_engine
from sqlalchemy import insert

engine = create_engine("postgresql+pg8000://mqtt:mqttPassword@localhost/sensor_data", echo=True, future=True)

c = engine.connect()
c.close()