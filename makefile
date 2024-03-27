all: configure build

debug: configure_debug build

configure:
	cmake -S . -B out/build

configure_debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -S . -B out/build

build:
	$(MAKE) --no-print-directory -C out/build

clean:
	rm -r -f out