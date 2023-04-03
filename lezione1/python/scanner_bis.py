import asyncio
from bleak import BleakScanner



BEACON_UUID =  "0c40b713-b68f-4136-be27-e4475f6020a0" 
SERVICE_UUID =  "f5953c69-1425-4134-be32-378c4b9e3c87"

last_ts = -1

def callback(device, advertising_data):
    global last_ts
    if advertising_data.service_data:
        if advertising_data.service_data.get(BEACON_UUID) is not None:
            ts = advertising_data.service_data.get(BEACON_UUID).decode()
            value = advertising_data.service_data.get(SERVICE_UUID).decode()
            if int(ts) != last_ts:
                last_ts = int(ts)
                print(f'{last_ts:>03d}: {int(value):>3d}')

async def main():
    stop_event = asyncio.Event()

    # TODO: add something that calls stop_event.set()
    async with BleakScanner(callback) as scanner:
        # Important! Wait for an event to trigger stop, otherwise scanner
        # will stop immediately.
        await stop_event.wait()

    # scanner stops when block exits


async def main_bis():
    scanner = BleakScanner(callback)
    while True:
        await scanner.start()
        await asyncio.sleep(10)
        print('discover stopped')
        await scanner.stop()


asyncio.run(main_bis())
