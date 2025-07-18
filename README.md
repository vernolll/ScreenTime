# Screen Time Tracker

**Description**
The project consists of two components:

* **API** is a background service that tracks screen time spent in various applications and writes the data to a JSON file.
* **Qt6 application** is a graphical interface that reads data from JSON and displays it in the form of convenient histograms, allowing you to analyze activity by day and week.

---

## Features

* Real-time tracking of application usage time
* Saving statistics in JSON with a convenient structure (dates and applications with their time)
* Qt6 GUI with visualization: charts by day and week
* Buttons for switching between daily and weekly viewing modes
* Legend with color-coded applications

---

## Technical Details

* The API is written in C++ (or another language if necessary)
* Qt6 is used for charting and interface
* The JSON format is something like this:

```json
{
  "21-06-2025": {
    "Explorer.EXE": 45,
    "WindowsTerminal.exe": 22,
    "devenv.exe": 16
  },
  "22-06-2025": {
    "Explorer.EXE": 3,
    "Telegram.exe": 17,
    "devenv.exe": 59
  }
}
```

## Dependencies

* Qt6 (Core, Widgets, Charts)
* C++17 (or later)

---

## Project structure

* `/api/` — sources and components for tracking screen time and writing to JSON
* `/app/` — Qt6-data visualization application

