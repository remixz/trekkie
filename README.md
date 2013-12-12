## Trekkie — Pebble Watchface

![Design](https://raw.github.com/ad1217/trekkie/master/screenshot.png)

An LCARS-inspired Pebble watchface. [Download pre-built app](http://www.mypebblefaces.com/view?fID=458&aName=zachbruggeman&pageTitle=Trekkie&auID=472) (old version, SDK 1)

*I hold no responsibility if this breaks your Pebble! This is built using the Beta of the Pebble SDK 2.0, and may be unstable.* I use it on my Pebble without issues though. :smile:
### Changes in the new version:
* added battery level and bluetooth connection monitor in the pane on the left
* changed date format to DD.MM.YY (day.month.year)

### Future/Planned Changes
* change bluetooth connection icon

### Building from source

Using the `pebble` script from the sdk or [Hexxeh's bluetooth version](https://github.com/Hexxeh/libpebble), run
`pebble build`.

Install with either `pebble install --phone [your phone's ip]`, `pebble install --pebble_id [your pebble's bluetooth adress]` (if using Hexxeh's libpebble), or put the .pbk on your phone and install it through the Pebble app.
