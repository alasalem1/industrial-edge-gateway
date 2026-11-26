import socket
import struct

HOST = "0.0.0.0"
PORT = 1502

# SentinelEdge holding registers:
# 0: Temperature = 42.5 C
# 1: Pressure    = 4.9 bar
# 2: Vibration   = 1.5 mm/s
# 3: Motor speed = 1262 RPM
REGISTERS = [425, 49, 15, 1262]

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind((HOST, PORT))
server.listen(5)

print(f"Simulated PLC listening on {HOST}:{PORT}", flush=True)
print(f"Holding registers: {REGISTERS}", flush=True)

while True:
    client, address = server.accept()
    print(f"Client connected: {address}", flush=True)

    with client:
        while True:
            header = client.recv(7)
            if not header:
                break
            if len(header) != 7:
                break

            transaction, protocol, length, unit = struct.unpack(">HHHB", header)
            remaining = length - 1
            pdu = b""

            while len(pdu) < remaining:
                chunk = client.recv(remaining - len(pdu))
                if not chunk:
                    break
                pdu += chunk

            if len(pdu) != remaining or len(pdu) < 5:
                break

            function, start, quantity = struct.unpack(">BHH", pdu[:5])

            if function == 3 and start + quantity <= len(REGISTERS):
                values = REGISTERS[start:start + quantity]
                data = b"".join(struct.pack(">H", value) for value in values)
                response_pdu = bytes([3, len(data)]) + data
            else:
                response_pdu = bytes([function | 0x80, 2])

            response = struct.pack(
                ">HHHB", transaction, protocol, len(response_pdu) + 1, unit
            ) + response_pdu

            client.sendall(response)
            print(
                f"Read request: start={start}, quantity={quantity}; "
                f"registers={REGISTERS[start:start + quantity]}",
                flush=True
            )
