# PeasyGUI

A lightweight and generic GUI for any programming language.

Design your layout with the built-in editor, then launch the GUI from your app. Communication between GUI and app happens via simple string exchange:
- **File-based** via temp directory
OR
- **TCP socket** where PeasyGUI is the server

No installation required — just extract and run.

Unlike large frameworks, PeasyGUI focuses on essential widgets and properties — ideal for quick prototypes or beginners. No need to hand over control flow to a GUI main loop.

---

## Features

- Cross-platform: macOS (≥12.0), Linux, Windows
- CSS-style layout customization (`Qt setStyleSheet`)
- Dynamic control creation at runtime
- Built-In WYSIWYG Designer
- No need to ever compile the PeasyGUI C++

### Controls
**Window** – Clipboard read/write, icon
Events: `ClipboardChanged`, `Closed`, `Resized`, `KeyPressed`

**Button** – Event: `Clicked`

**TextBox** – Single / Multi-line
Events: `ReturnPressed`

**ListBox** – Single / Multi-select
Events: `Clicked`, `DoubleClicked`, `SelectionChanged`

**OpenButton / SaveButton** – File/folder selection, extension filter
Event: `Chosen`

**ImageView** – JPG/PNG etc., zoom, crop, EXIF rotation, opacity, dimensions
Event: `ClickedAtPos`

**CheckBox** – Event: `StateChanged`

**DropArea** – Drag & drop paths
Event: `Dropped`

**DropDown** – Event: `SelectionChanged`

**Slider** – Events: `Moved`, `Pressed`, `Released`

**ProgressBar**, **Label**

... and these are only the most important properties

---

## Installation

Extract the release folder and start the executable. Designer mode opens by default.

- [Linux](packaging/linux/Installation_steps.txt)
- [Windows](packaging/windows/Installation_steps.txt)
- [macOS](packaging/macos/Installation_steps.txt)

### Windows and macOS Security Prompts

When launching PeasyGUI for the first time, you may see warnings such as "macOS cannot verify the developer" or on Windows "The publisher could not be verified" or "Microsoft Defender SmartScreen prevented an unrecognized app from starting".

PeasyGUI is an open-source project developed by an independent programmer and distributed for free. I have not paid for Apple’s or Microsoft’s yearly developer verification programs — which currently cost USD 99 per year for Apple and around USD 200–400 per year for Microsoft.

These fees buy nothing more than a digital certificate that tells the operating system the developer is "known". Unfortunately, this means independent or hobbyist developers who create free software are penalized, while many commercial applications with questionable value are "verified."

PeasyGUI is fully open source, so you are encouraged to inspect the code and build the executable yourself. This guarantees trust and transparency, and ensures no unwanted binaries are run on your system. The project has no external dependencies beyond Python 3 and Qt.

If you prefer to run the provided prebuilt binaries:

macOS: Right-click the app, select Open, and confirm in the dialog.

Windows: Click More info → Run anyway.

---

## Usage

```bash
PeasyGUI.exe --protocol=tcp --port=10000
# Start TCP server on port 10000

PeasyGUI.exe --protocol=filebased --exchange_dir="C:\Temp\"
# Exchange via text files in given folder

PeasyGUI.exe
# Start Designer mode
```

---

### Command-line options

```text
--protocol <tcp|filebased>
--port <port>                  (default: 10000)
--ip <ip>                      (default: 127.0.0.1)
--exchange_dir <dir>           (file exchange folder)
--ini <file>                   (default: AppRoot/PeasyGUI.ini)
```

---

## Build / Test / Deploy

Open the project’s `CMakeLists.txt` in **Qt Creator**, select the desired build type (e.g. *Debug*), and click **Run**.

**Run tests:**
*Menu → Extras → Tests → Run all tests*

**Deployment:**  
Each platform provides a dedicated script to build a fresh distribution.
Make sure the correct Qt version is installed beforehand.

- [Linux](packaging/linux/release.py) — requires Python 3
- [Windows](packaging/windows/release.bat)
- [macOS](packaging/macos/release.command)

---

## Current Limitation

Depending on the OS, TCP packets are split into multiple chunks if their size exceeds ~60 kB.

---

## Roadmap

- Dynamic menubar (`MenuTriggered` events)
- Align & distribute controls in designer
- Stick control edges to window border
- TCP packet size handling (length-prefix option)
- Editable lines in ListBox/DropDown
- `RaiseNow` for control z-order
- Tab order management
- Alternative separators (beyond backtick)
- Frame widget
- SpinBox widget
- Base64 exchange option
- UART server (e.g. COM1 for microcontrollers)

---

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.

All included icons and resources (OpenOffice files, PNGs, JPGs) were created by the author and are also covered by the MIT License.

The application uses Qt 6, which is available under the terms of the LGPLv3, GPLv3, or a commercial license. This project links against the Qt libraries under the terms of the LGPLv3.

The source code for the specific Qt version used can be obtained from: [https://download.qt.io/](https://download.qt.io/).
No modifications were made to the Qt source code.

The application provides an "About Qt" dialog via QMessageBox::aboutQt(), as required by the Qt license.