from sqlalchemy import Table, Column, Integer, String,DateTime,Float
from sqlalchemy import MetaData
from sqlalchemy import create_engine

engine = create_engine("postgresql+pg8000://mqtt:mqttPassword@postgres/sensor_data", echo=True, future=True)

metadata_obj = MetaData()

ambient_sensor = Table(
    "ambient_sensor",
    metadata_obj,
    Column("id", Integer, primary_key=True,autoincrement='auto'),
    Column("time", DateTime(True), nullable=False),
    Column('sensor_name',String(32),nullable=False),
    Column("temp", Float),
    Column("relhum", Float),
    Column("press", Float),
)
