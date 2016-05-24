prefix ?= /usr/local
bindir ?= $(prefix)/bin
mandir ?= $(prefix)/share/man

CFLAGS += -g --std=c99

.PHONY: all check clean install uninstall test-out

all: chars

check: chars
	./chars             < test-in.txt | diff -u test-out/standard.txt  -
	./chars -p          < test-in.txt | diff -u test-out/printable.txt -
	./chars --oneline   < test-in.txt | diff -u test-out/oneline.txt   -
	./chars --no-escape < test-in.txt | diff -u test-out/no-escape.txt -
	./chars -x          < test-in.txt | diff -u test-out/hex.txt       -
	./chars -X          < test-in.txt | diff -u test-out/hex-upper.txt -
	./chars -u          < test-in.txt | diff -u test-out/decimal.txt   -

clean:
	rm -f chars
	rm -rf chars.dSYM

install: chars chars.1
	mkdir -p $(bindir) $(mandir)/man1
	install chars $(bindir)/chars
	install chars.1 $(mandir)/man1/chars.1

uninstall:
	rm -f $(bindir)/json-yaml
	rm -f $(mandir)/man1/json-yaml.1

# Run this to regenerate the test cases
test-out: chars
	mkdir -p test-out
	./chars -i test-in.txt -o test-out/standard.txt
	./chars -i test-in.txt -o test-out/printable.txt --only printable
	./chars -i test-in.txt -o test-out/oneline.txt   --oneline
	./chars -i test-in.txt -o test-out/no-escape.txt --no-escape
	./chars -i test-in.txt -o test-out/hex.txt       --hex
	./chars -i test-in.txt -o test-out/hex-upper.txt --Hex
	./chars -i test-in.txt -o test-out/decimal.txt   --decimal
