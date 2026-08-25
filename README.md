# ESP32 + RC522 RFID Card Reader 

**Make a magic card reader!** Wave a card near a little blue board, and your
computer prints the card's secret ID number on the screen.

This guide is written so that a curious kid (or a grown-up who has never touched
electronics) can build it from start to finish. Take your time, follow the steps
in order, and don't be afraid — nothing here can hurt you if you follow the
safety notes.

---

## Table of Contents

1. [What Are We Building?](#1-what-are-we-building)
2. [How Does RFID Even Work?](#2-how-does-rfid-even-work)
3. [Shopping List (Parts You Need)](#3-shopping-list-parts-you-need)
4. [Meet Your Parts](#4-meet-your-parts)
5. [The Wiring / Pin Diagram](#5-the-wiring--pin-diagram)
6. [Wiring It Step by Step](#6-wiring-it-step-by-step)
7. [Setting Up the Software](#7-setting-up-the-software)
8. [Uploading the Code (Building It)](#8-uploading-the-code-building-it)
9. [Using It!](#9-using-it-)
10. [The Code, Explained Line by Line](#10-the-code-explained-line-by-line)
11. [When Things Go Wrong (Troubleshooting)](#11-when-things-go-wrong-troubleshooting)
12. [Fun Things to Try Next](#12-fun-things-to-try-next)
13. [Word Dictionary](#13-word-dictionary)

---

## 1. What Are We Building?

You know how at a hotel you tap a card on the door and it unlocks? Or how a bus
pass beeps when you touch it to a reader? That's **RFID**.

We are building the *reader* part. When you tap a card, our project will print
something like this on your computer:

```
Scan a card...
Card UID: 04:A3:2B:7F
Card UID: 1A:2B:3C:4D
```

That `04:A3:2B:7F` is the card's **UID** — its *Unique ID*. Think of it like the
card's name. Every card has a different one, a bit like every person having a
different fingerprint.

Once you can read the card's name, you can build almost anything: a secret box
that only opens for *your* card, an attendance machine for your class, or a game
where different cards are different players.

---

## 2. How Does RFID Even Work?

Here's the cool part: **the card has no battery.** None. Zero. So how does it
talk?

Imagine the reader is humming a song really loudly through invisible radio
waves. The card has a tiny coil of copper wire inside it — like a small antenna
loop. When the card gets close to the humming reader, that hum actually gives
the card a little sip of electricity. It's like how a solar panel gets power
from light, except this gets power from radio waves.

With that sip of power, the little chip inside the card wakes up for a fraction
of a second, shouts its ID number back through the radio waves, and then goes
back to sleep.

```
        ESP32 + RC522                             RFID Card
       ┌───────────────┐                       ┌────────────────┐
       │               │   ))) radio waves ))) │  (no battery!)  │
       │    READER     │  ───────────────────► │   coil + chip   │
       │               │                       │                 │
       │               │  ◄─────────────────── │  "My ID is      │
       └───────────────┘   ((( ID number  (((  │   04A32B7F!"    │
                                               └────────────────┘
              ▲
              │ USB cable
              ▼
        ┌───────────┐
        │  Your PC  │  ← the ID appears here
        └───────────┘
```

The radio waves are very weak, which is why you have to hold the card within
about **2–4 cm (about one inch)** of the reader. That's on purpose — you don't
want a stranger reading your hotel key from across the street!

The frequency used is **13.56 MHz**, and this family of cards is called
**MIFARE**. You'll see those words on the product page when you buy one.

---

## 3. Shopping List (Parts You Need)

| # | Part | How many | Roughly costs | Notes |
|---|------|----------|---------------|-------|
| 1 | **ESP32 development board** | 1 | $4–$8 | The "ESP32 DevKit V1" (30-pin) is the most common. Any ESP32 works. |
| 2 | **RC522 RFID module** | 1 | $2–$4 | A small **blue** board with a big flat coil printed on it. |
| 3 | **RFID card and/or keyfob** | 1+ | Usually free | These almost always come in the box *with* the RC522. |
| 4 | **Female-to-female jumper wires** | 7 | $2 for a pack | "Dupont wires." Female-to-female = holes on both ends. |
| 5 | **Micro-USB or USB-C cable** | 1 | You probably have one | ⚠️ Must be a **data** cable, not a charge-only cable! |
| 6 | **Breadboard** | 0 or 1 | $2 | Optional. You can wire it directly with jumper wires. |
| 7 | **A computer** | 1 | — | Windows, Mac, or Linux all work. |

**Tip for buying:** Search for *"RC522 RFID kit"* — most sellers bundle the
module, a card, a keyfob, and the pin header all together.

⚠️ **Important:** Your RC522 might arrive with the row of metal pins **not
soldered on**. If the pins come as a loose black strip, an adult with a
soldering iron needs to attach them before you can plug wires in. Some sellers
sell a pre-soldered version — worth paying an extra dollar for if you don't have
a soldering iron.

---

## 4. Meet Your Parts

### The ESP32 — the brain 🧠

This is a tiny computer, smaller than a stick of gum. It can run programs, talk
over Wi-Fi and Bluetooth, and control other electronics. It has metal pins along
both edges — those are how it talks to the outside world.

```
              ESP32 DevKit V1 (top view)
        ┌───────────────────────────────────┐
        │  ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣  │  ← pins
        │                                   │
        │   ┌─────────┐        ┌────┐       │
        │   │ silver  │        │ 🔘 │ EN    │
        │   │  metal  │        └────┘       │
        │   │  square │        ┌────┐       │
        │   │ (Wi-Fi) │        │ 🔘 │ BOOT  │
        │   └─────────┘        └────┘       │
        │                                   │
        │  ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣ ▣  │  ← pins
        └──────────────┬────────────────────┘
                       │
                  [USB port]  ← cable to your computer goes here
```

Two buttons you should know:

- **EN** (sometimes labelled RST) — the **restart** button. Press it to restart
  your program, like turning a game console off and on again.
- **BOOT** — the **"get ready to receive code"** button. You usually don't need
  it, but see [Troubleshooting](#11-when-things-go-wrong-troubleshooting).

### The RC522 — the antenna 📡

The blue board. That big rectangle of squiggly lines printed on it is the
**antenna** — the thing that hums the radio waves. Its 8 pins are all in one row
along the edge, and each one is labelled in tiny white letters.

```
              RC522 module (top view)
        ┌──────────────────────────────────────┐
        │  ╔══════════════════════════════╗    │
        │  ║  ┌────────────────────────┐  ║    │
        │  ║  │   the antenna coil     │  ║    │  ← tap your card
        │  ║  │  (squiggly printed     │  ║    │     right here
        │  ║  │   copper lines)        │  ║    │
        │  ║  └────────────────────────┘  ║    │
        │  ╚══════════════════════════════╝    │
        │           ▪▪▪ chip ▪▪▪               │
        └──┬───┬────┬────┬────┬────┬────┬──────┘
           │   │    │    │    │    │    │    │
          SDA SCK MOSI MISO  IRQ  GND  RST  3.3V
```

The 8 pins from one end to the other are:
**SDA, SCK, MOSI, MISO, IRQ, GND, RST, 3.3V**

We will use **7 of them**. The one we skip is **IRQ** — we simply don't need it
for this project.

### The card and keyfob 🪪

The white card looks like a blank credit card. The keyfob is a little plastic
teardrop that goes on a keyring. **Both work exactly the same way.** There's
nothing to charge and nothing to press.

---

## 5. The Wiring / Pin Diagram

This is the most important section. Go slowly. ☕

### ⚠️ THE ONE BIG RULE ⚠️

> **The RC522 runs on 3.3 VOLTS. Never connect it to 5V.**
>
> Plugging it into the 5V / VIN pin can permanently kill the module. The ESP32
> has a pin labelled **3V3** (or **3.3V**) — that is the one you want. Check it
> twice.

### The wiring table

Connect these seven pairs. Nothing else.

| RC522 pin | goes to → | ESP32 pin | What this wire is for | Suggested wire colour |
|-----------|-----------|-----------|-----------------------|-----------------------|
| **3.3V**  | → | **3V3**  | Power in (the "food") | 🔴 Red |
| **GND**   | → | **GND**  | Ground (the "return path") | ⚫ Black |
| **RST**   | → | **GPIO 4** | Reset — lets the ESP32 restart the module | 🟡 Yellow |
| **SDA**   | → | **GPIO 5** | Chip Select — "hey you, I'm talking to you!" | 🟢 Green |
| **MOSI**  | → | **GPIO 23** | Data going **out** of the ESP32 | 🔵 Blue |
| **MISO**  | → | **GPIO 19** | Data coming **in** to the ESP32 | 🟣 Purple |
| **SCK**   | → | **GPIO 18** | Clock — the "tick-tock" that keeps them in sync | ⚪ White |
| *IRQ*     | → | *nothing*  | Not used — leave it empty | — |

> The colours are only a suggestion to help you keep track. Electricity doesn't
> care what colour the plastic is! But red-for-power and black-for-ground is a
> tradition worth keeping — it prevents mistakes.

### The picture version

```
     RC522 MODULE                                 ESP32 DEVKIT
    ┌─────────────┐                            ┌────────────────┐
    │             │                            │                │
    │  3.3V  ○────┼──── 🔴 red ────────────────┼──○ 3V3         │
    │             │                            │                │
    │  RST   ○────┼──── 🟡 yellow ─────────────┼──○ GPIO 4      │
    │             │                            │                │
    │  GND   ○────┼──── ⚫ black ──────────────┼──○ GND         │
    │             │                            │                │
    │  IRQ   ○    │     (nothing — skip it!)   │                │
    │             │                            │                │
    │  MISO  ○────┼──── 🟣 purple ─────────────┼──○ GPIO 19     │
    │             │                            │                │
    │  MOSI  ○────┼──── 🔵 blue ───────────────┼──○ GPIO 23     │
    │             │                            │                │
    │  SCK   ○────┼──── ⚪ white ──────────────┼──○ GPIO 18     │
    │             │                            │                │
    │  SDA   ○────┼──── 🟢 green ──────────────┼──○ GPIO 5      │
    │             │                            │                │
    └─────────────┘                            └───────┬────────┘
                                                       │
                                                    [ USB ]
                                                       │
                                                       ▼
                                                  Your computer
```

### Where are those pins on the ESP32?

Every ESP32 board has slightly different pin *positions*, but the **labels** are
printed right on the board. Look for the tiny white text next to each hole:
`D4`, `D5`, `D18`, `D19`, `D23`, `3V3`, `GND`.

A common **30-pin ESP32 DevKit V1** is laid out like this:

```
                     ┌────────[USB]────────┐
             EN     ─┤                     ├─ GPIO 23   ← 🔵 MOSI
        GPIO 36 (VP) ┤                     ├─ GPIO 22
        GPIO 39 (VN) ┤                     ├─ GPIO  1 (TX0)
        GPIO 34     ─┤                     ├─ GPIO  3 (RX0)
        GPIO 35     ─┤                     ├─ GPIO 21
        GPIO 32     ─┤       ESP32         ├─ GPIO 19   ← 🟣 MISO
        GPIO 33     ─┤     DevKit V1       ├─ GPIO 18   ← ⚪ SCK
        GPIO 25     ─┤      (30 pin)       ├─ GPIO  5   ← 🟢 SDA
        GPIO 26     ─┤                     ├─ GPIO 17 (TX2)
        GPIO 27     ─┤                     ├─ GPIO 16 (RX2)
        GPIO 14     ─┤                     ├─ GPIO  4   ← 🟡 RST
        GPIO 12     ─┤                     ├─ GPIO  2
        GPIO 13     ─┤                     ├─ GPIO 15
        GND         ─┤                     ├─ GND       ← ⚫ GND
        VIN (5V) ⚠️ ─┤                     ├─ 3V3       ← 🔴 3.3V
                     └─────────────────────┘
```

⚠️ **`VIN` / `5V` is the pin you must avoid** for the RC522. On some boards it
sits right beside `3V3`. Put your finger on the label and read it out loud
before you plug the red wire in.

> If your board has 36 or 38 pins instead of 30, the labels are the same — only
> the positions shift. Always trust the printing on **your** board over any
> drawing, including this one.

### Why these particular pin numbers?

Because that's what the code says! Look at the top of the sketch:

```cpp
#define SS_PIN  5     // SDA goes to GPIO 5
#define RST_PIN 4     // RST goes to GPIO 4
```

MOSI (23), MISO (19), and SCK (18) aren't listed in the code because they are
the ESP32's **built-in default SPI pins**. The `SPI` library already knows about
them. It's like how you don't have to tell someone where their own hands are.

If you ever want to use different pins, change the numbers in the `#define`
lines and rewire to match.

---

## 6. Wiring It Step by Step

1. **Unplug the USB cable.** Never wire things up while the board has power.
   Seriously — do this every single time.
2. Lay the ESP32 and the RC522 on the table in front of you, both label-side up.
3. Take the **black** wire. Push one end onto the RC522's **GND** pin and the
   other end onto any ESP32 pin labelled **GND**. (There are several GND pins;
   any of them is fine — they're all the same wire inside.)
4. Take the **red** wire. RC522 **3.3V** → ESP32 **3V3**. *Read the label twice.*
5. Now the five signal wires, one at a time, checking each off the table above:
   - RST → GPIO 4
   - SDA → GPIO 5
   - SCK → GPIO 18
   - MISO → GPIO 19
   - MOSI → GPIO 23
6. **Check every wire again**, using the table. Wiggle each one gently — a wire
   that falls out is the #1 cause of "it doesn't work."
7. Make sure no bare metal is touching any other bare metal.
8. *Now* you may plug in the USB cable. A tiny LED on the ESP32 should light up.

If you smell anything hot or see smoke, **unplug immediately** and re-check your
wiring. (This won't happen if you followed the 3.3V rule.)

---

## 7. Setting Up the Software

### Step 7.1 — Install the Arduino IDE

The Arduino IDE is the program where you write code and send it to the ESP32.

1. Go to **https://www.arduino.cc/en/software**
2. Download **Arduino IDE 2.x** for your operating system.
3. Install it like any normal program. If Windows asks to install "device
   drivers," say **yes** — those let your computer see the ESP32.

### Step 7.2 — Teach the Arduino IDE about the ESP32

Out of the box, the Arduino IDE only knows about Arduino boards. We have to
introduce it to the ESP32.

1. Open the Arduino IDE.
2. Go to **File → Preferences** (on Mac: **Arduino IDE → Settings**).
3. Find the box labelled **"Additional boards manager URLs"**.
4. Paste this line into it:

   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

5. Click **OK**.
6. Click the **Boards Manager** icon in the left sidebar (it looks like a little
   circuit board), or go to **Tools → Board → Boards Manager**.
7. Type **esp32** in the search box.
8. Find **"esp32" by Espressif Systems** and click **Install**.
9. Wait. It's a big download — this can take 5–15 minutes. Go get a snack. 🍪

### Step 7.3 — Install the MFRC522 library

A **library** is code somebody else already wrote so you don't have to. The
MFRC522 library knows how to talk to the RC522 chip, which saves us hundreds of
lines of very tricky code.

1. Click the **Library Manager** icon in the left sidebar (it looks like books),
   or go to **Sketch → Include Library → Manage Libraries**.
2. Search for **MFRC522**.
3. Install **"MFRC522" by GithubCommunity**.
4. Wait for "INSTALLED" to appear. This one is quick.

> There are several similarly-named libraries. The one by **GithubCommunity** is
> the classic one this code is written for. If you install a different one, the
> function names may not match and you'll get errors.

### Step 7.4 — Open the project

1. **File → Open**
2. Navigate to this folder and pick **`esp32-rc522-rfid-reader.ino`**
3. The code appears in the editor. 🎉

> **A rule about Arduino folders:** an `.ino` file must live inside a folder with
> **the exact same name**. Our file is `esp32-rc522-rfid-reader.ino` and it sits
> in the folder `esp32-rc522-rfid-reader`. That matches, so we're fine. If you
> ever rename one, rename the other too.

---

## 8. Uploading the Code (Building It)

"Building" means turning your human-readable code into the 1s and 0s the ESP32
actually understands. "Uploading" means shipping those 1s and 0s over the USB
cable into the chip. The Arduino IDE does both when you click one button.

### Step 8.1 — Pick your board

**Tools → Board → esp32 → "ESP32 Dev Module"**

(If you know you have a different flavour — NodeMCU-32S, WROOM DA, DOIT ESP32
DEVKIT V1 — pick that one instead. "ESP32 Dev Module" works for almost everyone.)

### Step 8.2 — Pick your port

**Tools → Port →** and choose the one that appeared when you plugged in the ESP32.

- **Windows:** something like `COM3`, `COM5`, `COM7`
- **Mac:** something like `/dev/cu.usbserial-0001` or `/dev/cu.SLAB_USBtoUART`
- **Linux:** something like `/dev/ttyUSB0`

**Not sure which one?** Unplug the ESP32, look at the Port menu and remember
what's there. Plug it back in, look again. The *new* one is your ESP32.

**No new port at all?** Jump to
[Troubleshooting](#11-when-things-go-wrong-troubleshooting) — you probably need
a USB driver or a different cable.

### Step 8.3 — Verify (build without uploading)

Click the **✓ checkmark** button in the top-left. This compiles the code and
tells you about mistakes without touching the board.

You want to see: **"Done compiling."**

If you see red errors mentioning `MFRC522.h: No such file or directory`, the
library from Step 7.3 didn't install. Go back and do it again.

### Step 8.4 — Upload!

Click the **→ arrow** button next to the checkmark.

You'll see text scroll by:

```
Connecting........
Chip is ESP32-D0WDQ6 (revision 1)
Writing at 0x00010000... (100 %)
Wrote 214128 bytes...
Hash of data verified.
Leaving...
Hard resetting via RTS pin...
```

**"Hard resetting via RTS pin..."** means it worked! 🎊

If it hangs on `Connecting......_____....._____` — **hold down the BOOT button**
on the ESP32 while it says "Connecting", and let go once writing starts. Some
boards need this little nudge.

---

## 9. Using It! 🎉

1. Open the **Serial Monitor**: click the **magnifying glass** icon in the
   top-right, or press **Ctrl+Shift+M** (Mac: **Cmd+Shift+M**).
2. In the Serial Monitor's dropdown, set the speed to **115200 baud**.
   ⚠️ This *must* be 115200 or you'll see nonsense symbols.
3. Press the **EN** button on the ESP32 to restart it.
4. You should see:

   ```
   Scan a card...
   ```

5. **Tap your card flat on the RC522's antenna area.** Hold it about 1–2 cm
   away, right over the big printed rectangle.

   ```
   Card UID: 04:A3:2B:7F
   ```

   🎉 **You just read an RFID card!**

6. Try the keyfob. Different number! Try a hotel key card, a bus pass, a library
   card — many of them use the same 13.56 MHz standard and will show up too.
   (Some won't — bank cards and passports are often deliberately shielded or use
   a different system. That's normal.)

**Write down your card's UID somewhere.** You'll need it for the fun projects in
[section 12](#12-fun-things-to-try-next).

---

## 10. The Code, Explained Line by Line

Here's the whole program. It's only 35 lines! Let's take it apart piece by piece.

### The full sketch

```cpp
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  5
#define RST_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();            // Init SPI bus
  mfrc522.PCD_Init();     // Init RC522
  Serial.println("Scan a card...");
}

void loop() {
  // Look for new card
  if (!mfrc522.PICC_IsNewCardPresent()) return;

  // Select the card
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
    if (i != mfrc522.uid.size - 1) uidStr += ":";
  }
  uidStr.toUpperCase();
  Serial.println(uidStr);

  mfrc522.PICC_HaltA();      // Stop reading
  mfrc522.PCD_StopCrypto1(); // Stop encryption
}
```

### Part 1 — Borrowing other people's code

```cpp
#include <SPI.h>
#include <MFRC522.h>
```

`#include` means **"go get that toolbox and put it on my workbench."**

- **`SPI.h`** — SPI is a *language* that chips use to talk to each other over
  wires. This toolbox teaches the ESP32 to speak it.
- **`MFRC522.h`** — this is the library you installed in Step 7.3. It knows all
  the secret handshakes the RC522 chip expects.

Without these two lines, we'd have to write thousands of lines of code ourselves.

### Part 2 — Giving the pins nicknames

```cpp
#define SS_PIN  5
#define RST_PIN 4
```

`#define` creates a **nickname**. From now on, anywhere the code says `SS_PIN`,
the computer reads it as `5`.

Why bother? Because `SS_PIN` tells you *what it means*, while `5` is just a
number. And if you rewire to a different pin later, you change it in **one
place** instead of hunting through the whole program.

- `SS_PIN = 5` → the **SDA** wire (SS means "Slave Select" / "Chip Select")
- `RST_PIN = 4` → the **RST** (reset) wire

### Part 3 — Creating your reader object

```cpp
MFRC522 mfrc522(SS_PIN, RST_PIN);
```

This line says: **"Make me an RFID reader, and tell it which two pins it lives
on."**

Think of it like adopting a robot pet named `mfrc522`. From now on you can give
it commands by writing `mfrc522.` followed by what you want it to do.

The parentheses `(SS_PIN, RST_PIN)` are the instructions you hand it at birth —
"your chip-select wire is pin 5, your reset wire is pin 4."

### Part 4 — `setup()`, the "get ready" function

```cpp
void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Scan a card...");
}
```

Every Arduino program has exactly two special functions. `setup()` is the first.
**It runs one single time**, right when the board powers on or you press EN.
It's the "wake up, brush your teeth, get dressed" part of the day.

Line by line:

| Line | What it means |
|------|---------------|
| `Serial.begin(115200)` | Open the talking-to-the-computer channel, at a speed of 115200 bits per second. This number **must match** the Serial Monitor dropdown. |
| `SPI.begin()` | Turn on the SPI wires (MOSI, MISO, SCK). Now the two chips can chat. |
| `mfrc522.PCD_Init()` | Wake up the RC522 and tell it to start humming radio waves. `PCD` = *Proximity Coupling Device*, the fancy name for "the reader." |
| `Serial.println("Scan a card...")` | Print a friendly greeting so you know it's alive. `println` = "print, then start a new line." |

### Part 5 — `loop()`, the "do it forever" function

```cpp
void loop() {
  ...
}
```

`loop()` is the second special function. After `setup()` finishes, the ESP32
runs `loop()` over and over and over — **thousands of times per second**,
forever, until you unplug it.

It's like a security guard who walks the same patrol route all night. Every trip
around, they check: *"Any card here? No? Okay, walk around again."*

### Part 6 — Checking for a card

```cpp
if (!mfrc522.PICC_IsNewCardPresent()) return;
```

This is a mouthful, so let's take it apart:

- `mfrc522.PICC_IsNewCardPresent()` asks the reader: *"Is there a new card in
  front of you right now?"* It answers `true` (yes) or `false` (no).
  `PICC` = *Proximity Integrated Circuit Card* — the fancy name for "the card."
- The `!` in front means **NOT**. So `!answer` flips it: if there's no card,
  `!false` becomes `true`.
- `return;` means **"stop this trip through `loop()` right now and start over."**

Put together in plain English:

> **"If there is NOT a card, quit early and check again."**

Since `loop()` restarts immediately, the ESP32 just spins here — checking,
checking, checking — until a card finally shows up.

This trick is called an **early return** or a **guard clause**. It's a tidy way
to say "don't bother with the rest unless the situation is right."

### Part 7 — Reading the card's ID

```cpp
if (!mfrc522.PICC_ReadCardSerial()) return;
```

Same pattern. Now that we know a card is *there*, we ask it to actually **tell
us its ID**. If the read fails — maybe you pulled the card away too fast, or the
radio signal got garbled — we bail out and try again next loop.

When it succeeds, the library quietly stashes the answer inside the reader
object, in two places:

- `mfrc522.uid.size` — **how many bytes long** the ID is (usually 4, sometimes 7)
- `mfrc522.uid.uidByte[]` — the **actual bytes**, like a little numbered list

### Part 8 — Turning bytes into pretty text

```cpp
Serial.print("Card UID: ");
String uidStr = "";
for (byte i = 0; i < mfrc522.uid.size; i++) {
  if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
  uidStr += String(mfrc522.uid.uidByte[i], HEX);
  if (i != mfrc522.uid.size - 1) uidStr += ":";
}
uidStr.toUpperCase();
Serial.println(uidStr);
```

This is the trickiest part, so here's the whole idea first:

> The card gives us raw numbers like `4, 163, 43, 127`.
> We want to show `04:A3:2B:7F`.
> This chunk of code does that makeover.

**`String uidStr = "";`** — Make an empty box to collect letters in. Right now
it holds nothing at all.

**`for (byte i = 0; i < mfrc522.uid.size; i++)`** — A **for-loop**: "do the
stuff inside these braces once for each byte." If the card's ID is 4 bytes long,
this runs 4 times, with `i` being 0, then 1, then 2, then 3.

Inside the loop, three things happen:

**1. Add a leading zero if needed**

```cpp
if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
```

We're printing in **hexadecimal** (base 16), where numbers go
`0 1 2 3 4 5 6 7 8 9 A B C D E F`. Any byte smaller than 16 (written `0x10` in
hex) prints as a single character — `4` instead of `04`. That looks messy and
makes IDs hard to compare. So we glue a `"0"` on the front. Now everything is
neatly two characters wide.

**2. Add the byte itself**

```cpp
uidStr += String(mfrc522.uid.uidByte[i], HEX);
```

`String(number, HEX)` converts a number into hex text. `163` becomes `"a3"`.
The `+=` means "stick this onto the end of what's already in the box."

**3. Add a colon separator — but not after the last one**

```cpp
if (i != mfrc522.uid.size - 1) uidStr += ":";
```

`!=` means "is not equal to." If we're *not* on the final byte, add a `:`. This
is why you get `04:A3:2B:7F` and not `04:A3:2B:7F:` with an ugly trailing colon.

**`uidStr.toUpperCase();`** — Shout it. `04:a3:2b:7f` becomes `04:A3:2B:7F`.
Capital hex is the normal convention and it's easier to read.

**`Serial.println(uidStr);`** — Send the finished text to your computer screen.

Here's the loop running on a real card, step by step:

| `i` | byte value | hex | leading zero? | result so far |
|-----|-----------|-----|---------------|---------------|
| 0 | 4   | `4`  | yes → `"0"` | `04:` |
| 1 | 163 | `a3` | no          | `04:a3:` |
| 2 | 43  | `2b` | no          | `04:a3:2b:` |
| 3 | 127 | `7f` | no, and it's the last → no colon | `04:a3:2b:7f` |

Then `toUpperCase()` → **`04:A3:2B:7F`** ✨

### Part 9 — Politely hanging up

```cpp
mfrc522.PICC_HaltA();      // Stop reading
mfrc522.PCD_StopCrypto1(); // Stop encryption
```

- **`PICC_HaltA()`** tells the card: *"Thanks, you can go to sleep now."* This is
  what stops the same card from being read a thousand times a second while it
  sits on the reader. Without this line your screen would flood with the same ID
  over and over.
- **`PCD_StopCrypto1()`** closes the encrypted conversation channel. Our simple
  sketch never used encryption, but calling this is a good habit — it leaves the
  reader in a clean state, ready for the next card. If you skip it, later
  projects that *do* use encryption will mysteriously break.

Then `loop()` ends, starts again from the top, and waits for the next card.
Forever. 🔁

---

## 11. When Things Go Wrong (Troubleshooting)

Almost every problem is on this list. Find your symptom:

### 🔴 "MFRC522.h: No such file or directory"

The library isn't installed. Redo **Step 7.3**. Make sure you picked the one by
**GithubCommunity**, then restart the Arduino IDE.

### 🔴 No port shows up in Tools → Port

Try these in order:

1. **Try a different USB cable.** This is the #1 cause. Many cables that came
   with headphones or power banks are *charge only* — they have no data wires
   inside. Use one you know can transfer files.
2. **Install the USB driver.** Look at the small chip near the USB port on your
   ESP32:
   - Says **CP2102** → get the *Silicon Labs CP210x* driver
   - Says **CH340** or **CH9102** → get the *WCH CH34x* driver
   - Search the chip name + "driver" + your operating system.
3. **Try a different USB port** on your computer. Rear ports are usually better
   than front ones or hubs.
4. Restart your computer after installing a driver. Yes, really.

### 🔴 Upload fails: "Failed to connect to ESP32: Timed out"

**Hold down the BOOT button** on the ESP32 while the console says
`Connecting.......`, and release it once you see `Writing at...`.

Some boards need this every time; some never do. It depends on the board's
design.

Also make sure the Serial Monitor is **closed** during upload — it can hog the
port.

### 🔴 Serial Monitor shows gibberish symbols

Your baud rate is wrong. Set the Serial Monitor dropdown to **115200**. That
number has to match `Serial.begin(115200)` in the code.

### 🔴 Nothing prints at all, not even "Scan a card..."

- Press the **EN** button on the ESP32. That message only prints once at startup,
  so if the board booted before you opened the monitor, you missed it.
- Check the baud rate is 115200.
- Check the right Port is selected.

### 🔴 It says "Scan a card..." but never detects a card

This is a **wiring** problem 95% of the time.

1. **Unplug USB.** Re-check all 7 wires against the table in
   [section 5](#5-the-wiring--pin-diagram). One at a time. Out loud.
2. **Push every wire in firmly.** Loose Dupont connectors are sneaky — they look
   connected but aren't.
3. **Is the red wire on 3V3 and not 5V/VIN?** If it was on 5V, the module may
   already be damaged. 😢
4. **Hold the card closer** — 1 to 2 cm, flat against the antenna area, not
   edge-on.
5. **Try the keyfob** instead of the card, in case the card is faulty.
6. **Try a different RC522.** They're cheap and quality varies a lot; some arrive
   dead from the factory. Buying two is smart insurance.
7. **Bad solder joints** — if you soldered the header yourself, check that each
   pin has a shiny cone of solder and none are bridged together.

### 🔴 It reads sometimes but not reliably

- Keep the wires **short**. SPI signals get unhappy over long jumper wires.
- Move the RC522 away from metal objects — metal absorbs the radio field.
- Some ESP32 boards can't push quite enough current on 3V3 with a weak USB port.
  Try a different USB port or a powered hub.

### 🔴 The same card prints over and over

That means `PICC_HaltA()` isn't running. Make sure those last two lines are
inside `loop()` and you didn't accidentally delete them.

---

## 12. Fun Things to Try Next

Now that you can read a card's UID, here are projects to build on top — roughly
easiest first.

### 🟢 Level 1: Say hello by name

Add this above `setup()`, and check the UID inside `loop()`:

```cpp
String myCard = "04:A3:2B:7F";   // ← put YOUR card's UID here!

// then, after Serial.println(uidStr); add:
if (uidStr == myCard) {
  Serial.println("Welcome back, boss!");
} else {
  Serial.println("I don't know you.");
}
```

### 🟢 Level 2: Green light, red light

Wire an LED (with a 220Ω resistor!) to GPIO 2 and light it up on a known card.

### 🟡 Level 3: Add a buzzer

A little piezo buzzer that beeps once for "yes" and twice for "no" makes it feel
like a real machine.

### 🟡 Level 4: Multiple cards

Make an array of allowed UIDs and loop through them — a whole family of keys.

### 🟠 Level 5: A real lock

Add a servo motor that swings a latch open, or a relay module that controls a
12V solenoid lock. **Get an adult to help with anything above 5 volts.**

### 🟠 Level 6: Attendance logger

Add an SD card module and write each UID plus a timestamp to a file. Now you've
built a real attendance system.

### 🔴 Level 7: Use the Wi-Fi!

This is an ESP32 — it has Wi-Fi built in. Send each scan to a website, a Google
Sheet, or a phone notification. Now your card reader is on the internet.

### 🔴 Level 8: Read and write the card's memory

MIFARE Classic cards have about **1 kilobyte** of storage inside, split into
sectors, each protected by a key. The MFRC522 library can read and write it —
look at the `ReadNUID`, `DumpInfo`, and `ReadAndWrite` examples that came with
the library (**File → Examples → MFRC522**).

⚠️ **Only experiment on the blank cards that came in your kit.** Don't mess with
your actual bus pass or building access card — you could break it permanently,
and messing with cards that aren't yours isn't cool.

---

## 13. Word Dictionary

| Word | What it really means |
|------|----------------------|
| **RFID** | *Radio Frequency Identification.* Identifying things using radio waves. |
| **NFC** | *Near Field Communication.* A close cousin of RFID that uses the same 13.56 MHz frequency. Your phone probably has it. |
| **UID** | *Unique IDentifier.* The card's permanent name, burned in at the factory. |
| **MIFARE** | The brand/family of card our RC522 talks to. |
| **PICC** | *Proximity Integrated Circuit Card.* The library's word for **the card**. |
| **PCD** | *Proximity Coupling Device.* The library's word for **the reader**. |
| **GPIO** | *General Purpose Input/Output.* A pin you can control from code. |
| **SPI** | *Serial Peripheral Interface.* The wire language chips use to chat. Needs 4 wires: MOSI, MISO, SCK, and a select pin. |
| **MOSI** | *Master Out, Slave In.* Data flowing **from** the ESP32 **to** the RC522. |
| **MISO** | *Master In, Slave Out.* Data flowing **from** the RC522 **to** the ESP32. |
| **SCK** | *Serial Clock.* A pin that ticks like a metronome so both chips stay in step. |
| **SS / SDA / CS** | *Slave Select / Chip Select.* "I'm talking to YOU now." Three names, same job. |
| **RST** | *Reset.* Lets the ESP32 restart the RC522 chip. |
| **GND** | *Ground.* The return path for electricity. Every circuit needs one. |
| **Baud rate** | How fast text travels down the USB cable. Ours is 115200 bits per second. |
| **Library** | Code somebody else wrote and shared so you don't have to write it. |
| **Sketch** | Arduino's word for "a program." Saved as a `.ino` file. |
| **Compile / Build** | Translating your code into 1s and 0s the chip understands. |
| **Upload / Flash** | Sending those 1s and 0s into the chip over USB. |
| **Hexadecimal (hex)** | Counting in base 16 using `0-9` and `A-F`. `A`=10, `F`=15, `10`=16. |
| **Byte** | 8 bits of data. Holds a number from 0 to 255, or `00` to `FF` in hex. |
| **Serial Monitor** | The window in the Arduino IDE that shows messages from your board. |

---

## Safety Notes for Young Builders 🦺

- **Always unplug before rewiring.** Every time.
- **3.3V only** for the RC522. Never 5V.
- **Never** connect a 3V3 pin directly to a GND pin — that's a short circuit.
- Don't touch the board with wet hands.
- If anything gets hot, smells strange, or smokes — **unplug it right away** and
  ask an adult.
- Get an adult for the soldering iron. It is 350 °C and it does not care that
  you're being careful.
- Boards can get warm during normal use. Warm is fine. *Too hot to touch* is not.

---

## License & Credits

- The **MFRC522 library** is by the GitHub community, based on original work by
  Miguel Balboa — https://github.com/miguelbalboa/rfid
- ESP32 Arduino core by **Espressif Systems**
- This project is free to use, copy, remix, and learn from.

**Now go build something awesome.**
