SSL = openssl

CA_ROOT = ./Real-CA/
FAKE_CA_ROOT = ./Fake-CA/

pKey = $(CA_ROOT)CA-pKey.pem
CERT = $(CA_ROOT)CA-cert.pem

F_pKey = $(FAKE_CA_ROOT)Fake-CA-pKey.pem
F_CERT = $(FAKE_CA_ROOT)Fake-CA-cert.pem


all : generateCA generateFakeCA

generateCA: 
	# Real-CA
	# Create Real-CA Folder
	-rm -rf $(CA_ROOT)
	mkdir $(CA_ROOT)
	# Generate RSA private key for CA using 2048 bits.
	$(SSL) genrsa -des3 -out $(pKey) 2048
	# Generate certificate of CA
	$(SSL) req -x509 -new -nodes -key $(pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=CA" -days 365 -out $(CERT)

generateFakeCA:
	# Fake-CA
	# Create Fake-CA Folder
	-rm -rf $(FAKE_CA_ROOT)
	mkdir $(FAKE_CA_ROOT)
	# Generate RSA private key for CA using 2048 bits.
	$(SSL) genrsa -des3 -out $(F_pKey) 2048
	# Generate certificate of CA
	$(SSL) req -x509 -new -nodes -key $(F_pKey) -subj "/C=TW/ST=Taiwan/L=Taipei City/O=NTUST/OU=B10615045/CN=FakeCA" -days 365 -out $(F_CERT)

.PHONY: clean

clean:
	@echo "Cleaning pem srl files..."
	-rm *.pem
	-rm *.srl
	-rm -rf $(CA_ROOT)
	-rm -rf $(FAKE_CA_ROOT)

