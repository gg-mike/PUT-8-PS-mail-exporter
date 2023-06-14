SRCS=src/Communication.cpp src/Configuration.cpp src/EmailManager.cpp

all:
		g++ -Wall $(SRCS) -o EmailManager -lcurl
