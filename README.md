Telegram Communicator V4

A standalone handheld Telegram communicator built around an ESP32, SSD1306 OLED, and a single rotary encoder.

Hardware
Microcontroller

ESP32 DevKit V1

Display

SSD1306 OLED

128 × 64 pixels

I²C

Pins

SDA → GPIO22
SCL → GPIO21
Rotary Encoder

KY-040

CLK → GPIO18
DT  → GPIO19
SW  → GPIO23
WiFi

Connects automatically on boot.

If WiFi disconnects

Reconnect automatically.

Main Screen
Compose
────────────────

HELLO

A B C [D] E F G

The currently selected character is always centered.

Character Wheel

Contains

ABCDEFGHIJKLMNOPQRSTUVWXYZ

abcdefghijklmnopqrstuvwxyz

0123456789

Space

.

,

!

?

@

#

%

&

(

)

-

_

+

/

=

:

Backspace

Send

The wheel wraps infinitely.

Example

Z

↓

A

↓

B

↓

C
Rotary Encoder

Rotate clockwise

Move one character right.

Rotate anticlockwise

Move one character left.

Exactly one movement for every encoder click.

No skipped characters.

No bouncing.

No jitter.

Short Press

Adds selected character.

Example

Current message

HEL

Selected

L

After press

HELL
Backspace

If current selection is

⌫

Press

Deletes last character.

Space

If selected

SPACE

Press

Adds

" "
Long Press

If held for one second

Send message to Telegram.

OLED

Sending...

then

Message Sent

until button released.

Then return to compose.

Telegram Sending

Uses

HTTPClient

WiFiClientSecure

Telegram HTTPS API

No UniversalTelegramBot library.

Inbox

A second screen.

Inbox
────────────────

Dada

Hello!
Switching Screens

Double click

Compose

↓

Inbox

↓

Compose
Telegram Receive

Every two seconds

ESP32 performs

getUpdates

Stores

lastUpdateID

Only unread messages are processed.

No duplicate messages.

New Message

If a new message arrives

Automatically switch to Inbox.

Display

Inbox

Dada

Hello!
Reply

Double click

Returns to Compose.

Type reply.

Long press.

Message sent.

OLED Behaviour

No flickering.

Only changed regions redraw.

Message Storage

Only latest received message stored.

No history.

WiFi

Boot

Connecting WiFi...

↓

Connected

If disconnected

Automatically reconnect.

Telegram Errors

If Telegram unavailable

Display

Telegram Offline

Keep trying every two seconds.

HTTP Errors

Display

Send Failed

Do not erase compose text.

Character Cursor

Always centered.

Example

J K L [M] N O P

Rotate

K L M [N] O P Q
Text Wrapping

Compose message automatically wraps.

Example

HELLO THIS IS A
LONG MESSAGE
Maximum Message Length

128 characters.

If full

Ignore further input.

Inbox Text

Automatically wraps.

Example

Inbox

Dada

Hello

How are

you today?
Startup Sequence
Telegram
Communicator

↓

Connecting WiFi

↓

Connecting Telegram

↓

Compose
Internal Architecture
Main Loop

│

├── Read Encoder

├── Read Button

├── Update OLED

├── Check Telegram

├── Send Telegram

└── WiFi Reconnect

No blocking delays.

Everything uses millis() timing.

Libraries
WiFi.h

WiFiClientSecure.h

HTTPClient.h

Wire.h

Adafruit_GFX.h

Adafruit_SSD1306.h

ArduinoJson 7.4.3

No unnecessary third-party libraries.

Final Goal

A compact, responsive Telegram communicator that lets you:

Connect to Wi-Fi automatically.
Compose messages with a single rotary encoder.
Send messages through your Telegram bot.
Automatically receive new messages addressed to the bot.
Read the latest received message in a dedicated Inbox screen titled Dada.
Switch between composing and reading using only the encoder button.
Operate smoothly with no encoder glitches or noticeable OLED flicker.
