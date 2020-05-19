SRC = ./WebServer
CA = ./CA

all:
	+$(MAKE) -C $(CA)
	+$(MAKE) -C $(SRC)

.PHONY: clean
clean:
	@echo "Cleaning object and pem files in all subdir..."
	+$(MAKE) -C $(SRC) clean
	+$(MAKE) -C $(CA) clean
