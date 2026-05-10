import socket, struct, sys

if len(sys.argv) > 1:
    HOST = sys.argv[1]
else:
    HOST = 'localhost'

PORT = 12345

def sendData(s, str):
    n = len(str) * 2
    s.send(struct.pack(f'i{n}s', n, str.encode('utf-16-le')))

def receiveData(s):
    n = struct.unpack('i', s.recv(4))[0]
    return struct.unpack(f'{n}s', s.recv(n))[0].decode('utf-16-le')

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while True:
        sendData(s, input())
        print(receiveData(s))
