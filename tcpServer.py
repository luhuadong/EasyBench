from socket import *

server = socket(AF_INET, SOCK_STREAM)

server.bind(("", 8080))
server.listen(5)

client, info = server.accept()

while True:

    data = client.recv(1024)
    print("%s:%s"%(str(info), data))

client.close()
server.close()

