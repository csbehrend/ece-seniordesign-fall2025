import uuid

if __name__ == "__main__":
    ble_service_uuid = uuid.uuid4()
    print(f"New UUID4 = {ble_service_uuid}")
    elements = [f"0x{x:02x}" for x in ble_service_uuid.bytes[::-1]]
    center = ", ".join(elements)
    print(center)
