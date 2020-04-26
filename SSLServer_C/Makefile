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
	+$(MAKE) -C $(SERVER) clean
	+$(MAKE) -C $(CLIENT) clean
	+$(MAKE) -C $(CA) clean
