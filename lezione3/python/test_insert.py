from sqlalchemy import insert
from sensor_tables import engine,metadata_obj,ambient_sensor
import datetime as dt

data = {}
data["time"] = dt.datetime.utcnow()
data['sensor_name'] = 'test_sensor'
data["temp"] = 20
data["relhum"] = 50
data["press"] = 950

with engine.connect() as conn:
    conn.execute(insert(ambient_sensor), data)
    conn.commit()
