SERVER = ./server
CLIENT = ./client
CA = ./CA

all:
	+$(MAKE) -C $(SERVER)
	+$(MAKE) -C $(CLIENT)
	+$(MAKE) -C $(CA)

.PHONY: clean
clean:
	@echo "Cleaning object and pem files in all subdir..."
	-rm $(SERVER)/*.o
	-rm $(SERVER)/*.pem
	-rm $(CLIENT)/*.o
	-rm $(CLIENT)/*.pem
	-rm $(CA)/*.pem
