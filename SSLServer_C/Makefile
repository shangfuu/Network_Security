SERVER = ./server
CLIENT = ./client

all:
	+$(MAKE) -C $(SERVER)
	+$(MAKE) -C $(CLIENT)

.PHONY: clean
clean:
	@echo "Cleaning Object files under ./server"
	-rm $(SERVER)/*.o
	@echo "Cleaning Object files under ./client"
	-rm $(CLIENT)/*.o
