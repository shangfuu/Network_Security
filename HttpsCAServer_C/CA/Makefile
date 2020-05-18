SSL = openssl

CA_ROOT = ./Real-CA/

pKey = $(CA_ROOT)CA-pKey.pem
CERT = $(CA_ROOT)CA-cert.pem


all : generateCA

generateCA: 
	# Real-CA
	# Create Real-CA Folder
	-rm -rf $(CA_ROOT)
	mkdir $(CA_ROOT)
	# Generate RSA private key for CA using 2048 bits.
	$(SSL) genrsa -des3 -out $(pKey) 2048
	# Generate certificate of CA
	$(SSL) req -x509 -new -nodes -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=CA/CN=CA_B10615045" -days 365 -out $(CERT)

.PHONY: clean

clean:
	@echo "Cleaning pem srl files..."
	-rm *.pem
	-rm *.srl
	-rm -rf $(CA_ROOT)

