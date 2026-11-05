SERVER_SRC=Acceptor.cpp TcpServer.cpp Channel.cpp EventLoop.cpp Svr.cpp Connection.cpp
TARGET1=Server

CLIENT_SRC=Client.cpp Clt.cpp
TARGET2=Client

HEAD=GetTime.h 

all:$(TARGET1) $(TARGET2)

$(TARGET1): $(SERVER_SRC) $(HEAD) 
	g++ -Wall -pthread $(SERVER_SRC) -o $(TARGET1)

$(TARGET2): $(CLIENT_SRC) $(HEAD)
	g++ -Wall $(CLIENT_SRC) -o $(TARGET2)
	
clean:
	rm -f $(TARGET1) $(TARGET2)
	
.PHONY: all clean