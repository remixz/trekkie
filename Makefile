setup:
	../../tools/create_pebble_project.py --symlink-only ../../sdk/ .
	./waf configure
build:
	./waf build
clean-build:
	./waf clean build
.PHONY: setup build clean-build