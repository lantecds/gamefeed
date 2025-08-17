# GameFeed

[![sampctl](https://img.shields.io/badge/sampctl-gamefeed-2f2f2f.svg?style=for-the-badge)](https://github.com/lantecds/gamefeed)

**GameFeed** is a notification and feedback system designed for open.mp servers, providing smooth, GTA-style popups, subtitles, and cash notifications.

With GameFeed, you can easily display various types of notifications such as popups, subtitles, and cash changes, featuring support for fade effects, message queuing, and automatic money display.

## Installation

Simply install to your project:
```bash
sampctl package install lantecds/gamefeed
```

Include in your code and begin using the library:
```pawn
#include <gamefeed>
```

## Usage

### Example

Here’s how you can use the different notification types:

```pawn
// Show a popup message
ShowPlayerPopup(playerid, "Achievement Unlocked", "You've reached level 10!", 3000);

// Show a subtitle
ShowPlayerSubtitle(playerid, "Mission Complete: Street Race", 4000);

// Show a cash change (automatic if GAMEFEED_AUTO_MONEY_DISPLAY is defined)
ShowPlayerCashText(playerid, 5000); // Shows +$5,000
ShowPlayerCashText(playerid, -1000); // Shows -$1,000
```

### Configuration

You can tweak various system settings by modifying these defines:

```pawn
// Popup Configuration
#define POPUP_DEFAULT_DURATION (3000)
#define MAX_POPUP_QUEUE (5)
#define POPUP_TITLE_COLOR (0xFFFFFFFF)
#define POPUP_TEXT_COLOR  (0xFFFFFFFF)
#define POPUP_BG_COLOR    (0x000000FF)
#define POPUP_FADE_INTERVAL (100)

// Subtitle Configuration
#define SUBTITLE_TEXT_COLOR (0xFFFFFFFF)
#define SUBTITLE_DEFAULT_DURATION (4000)
#define SUBTITLE_FADE_INTERVAL (50)

// Cash Text Configuration
#define CASH_TEXT_INCREASE_COLOR (0x2F5A26FF)
#define CASH_TEXT_DECREASE_COLOR (0x9C1619FF)
#define CASH_TEXT_BG_COLOR (0x000000FF)

// Fading Configuration
#define MAX_BG_ALPHA (180)
#define MAX_TEXT_ALPHA (255)
#define FADE_STEP (25)
```

### Functions

#### `ShowPlayerPopup`

Displays a popup message to the player.

```pawn
ShowPlayerPopup(playerid, title[], message[], displaytime = POPUP_DEFAULT_DURATION, bool:jumpqueue = false)
```

* `playerid`: The player to show the popup to.
* `title`: The title of the popup.
* `message`: The main message to display.
* `displaytime`: Duration (in milliseconds) for which the popup will stay on screen.
* `jumpqueue`: If set to `true`, the popup will skip the message queue and display immediately.

Returns `true` if successful, `false` otherwise.

#### `ShowPlayerSubtitle`

Displays a subtitle at the bottom of the screen.

```pawn
ShowPlayerSubtitle(playerid, text[], displaytime = 4000, bool:useFade = true)
```

* `playerid`: The player to show the subtitle to.
* `text`: The subtitle text.
* `displaytime`: Duration (in milliseconds) for which the subtitle will be visible.
* `useFade`: If set to `true`, the subtitle will fade in and out.

Returns `true` if successful.

#### `ShowPlayerCashText`

Displays a notification of a player's cash change.

```pawn
ShowPlayerCashText(playerid, amount, displayTime = 3000)
```

* `playerid`: The player to show the notification to.
* `amount`: The amount of money (can be positive or negative).
* `displayTime`: Duration (in milliseconds) for the notification to display.

Returns `true` if successful.

### Automatic Money Display

To automatically show cash changes whenever `GivePlayerMoney` is called, define `GAMEFEED_AUTO_MONEY_DISPLAY` before including the library:

```pawn
// Enable automatic money display
#define GAMEFEED_AUTO_MONEY_DISPLAY
#include <gamefeed>
```