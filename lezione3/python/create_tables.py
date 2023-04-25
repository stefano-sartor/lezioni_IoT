from sensor_tables import engine,metadata_obj

metadata_obj.create_all(bind=engine)