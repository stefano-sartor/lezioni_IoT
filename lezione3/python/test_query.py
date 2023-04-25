from sensor_tables import engine,metadata_obj,ambient_sensor
from sqlalchemy import select

stm = select(ambient_sensor).where(ambient_sensor.c.sensor_name == "test_sensor")

with engine.connect() as conn:
    result =conn.execute(stm)
    rm = result.mappings()
    for row in rm:
        print(row)
    conn.commit()


