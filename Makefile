DOOR=door
DOORO=FDlist.o SerialLink.o ReadKeyBoard.o Arduino.o Swipe.o MySQL.o NetServer.o main.o 

CFLAGS  += -g
CFLAGS  += $(shell mysql_config --cflags)                                                              
LDFLAGS += -g
#LDFLAGS += -Wl,-Bsymbolic-functions -rdynamic -L/lib/mysql -L/usr/lib  -lmysqlclient
#LDFLAGS += -Wl,-Bsymbolic-functions -rdynamic /usr/lib/libmysqlclient.so
#LDFLAGS += -Wl,-Bsymbolic-functions -rdynamic /usr/lib/libmysqlclient.so
#LDFLAGS += $(shell mysql_config --libs)

all: $(DOOR)

$(DOOR): $(DOORO)
	$(CC) $(CFLAGS) -o $(DOOR) $(DOORO) /usr/lib/libmysqlclient.so
#	$(CC)  -o $@ /usr/lib/libmysqlclient.so  $(DOORO)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(DOORO) $(DOOR)


