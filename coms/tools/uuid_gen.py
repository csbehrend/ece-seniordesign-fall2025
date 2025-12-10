import uuid
import argparse

parser = argparse.ArgumentParser(
    prog="UUIDtools",
    description="Magical UUID things for clinically insane ECE students",
    epilog="What is work/life balance?",
)
group = parser.add_mutually_exclusive_group(required=True)
group.add_argument("-g", "--generate", action="store_true")
group.add_argument(
    "-i",
    "--input",
    type=str,
)

parser.add_argument("-a", "--arrayify", action="store_true")

if __name__ == "__main__":
    args = parser.parse_args()

    ble_service_uuid = None
    if args.generate:
        ble_service_uuid = uuid.uuid4()
        print("Gen UUID4 =", ble_service_uuid)
    elif args.input:
        ble_service_uuid = uuid.UUID(args.input)
        print("Input UUID =", ble_service_uuid)

    assert ble_service_uuid is not None

    if args.arrayify:
        elements = [f"0x{x:02x}" for x in ble_service_uuid.bytes[::-1]]
        center = ", ".join(elements)
        print("Arrayified = ", center)
