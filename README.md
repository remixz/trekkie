## Trekkie — Pebble Watchface

![Image](http://i.imgur.com/jEKDxnH.jpg)

An LCARS-inspired Pebble watchface. Swanky.

*I hold no responsibility if this breaks your Pebble! This is built using the early Proof of Concept SDK, and may be unstable.* I use it on my Pebble without issues though. :smile:

### Building

Because remembering paths is a pain, I've set up a Makefile to do that for you. Make sure to clone this watch into the proper directory:

```
git clone https://github.com/remixz/trekkie.git /path/to/pebble-sdk/watches/trekkie
cd trekkie
make setup
```

Then, to build, just run:
```
make build
```

Installing it on your Pebble is a bit more of an adventure. I install it using [`libpebble`](https://github.com/pebble/libpebble). If you are on Android, you can also serve the .pbw on a web-server and download it.
