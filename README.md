## Trekkie — Pebble Watchface

![Design](https://raw.github.com/ad1217/trekkie/master/screenshot.png)

An LCARS-inspired Pebble watchface. Download the pre-built watchface from the Pebble App Store!

*This watchface may be unstable. I hold no responsibility if this breaks your Pebble!* I use it on my Pebble without issues though. :smile:
### Changes in the since SDK 1.x
* added battery level and bluetooth connection icon (recently changed) in the pane on the left
* changed date format to DD.MM.YY (day.month.year)

### Building from source

Using the `pebble` script from the sdk or [Hexxeh's bluetooth version](https://github.com/Hexxeh/libpebble), run
`pebble build`.

Install with either `pebble install --phone [your phone's ip]`, `pebble install --pebble_id [your pebble's bluetooth adress]` (if using Hexxeh's libpebble), or put the .pbk on your phone and install it through the Pebble app.
