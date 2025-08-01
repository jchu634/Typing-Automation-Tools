## Typing Automation Tools

### Overview
This repository contains two tools designed to automate typing tasks, particularly useful for scenarios where standard pasting functionality is unavailable or inconvenient.
- `type-clipboard`: Automatically types the current content of your clipboard, ideal for applications that don't support traditional pasting (e.g., some remote desktops, virtual machines, or older applications).
	- Activate using the hotkey `WIN + F6.
- `type-at-wpm`: Types out given text at a specified Words Per Minute (WPM).

### Platform Compatibility
These tools are Windows-specific as they utilise WIN32 API calls for simulating keyboard input.

### Use Cases
- I made `type-at-wpm` to debug a react web-app that broke when the user typed too fast.
- `type-clipboard` was created to fix my problem with getting text into a Proxmox noVNC terminal.

## Features + Limitations:	
### `type-clipboard`:
Types out clipboard content, instead of pasting it, built as a workaround for lack of clipboard support in many web IPMIs and similar environments.
- May also bypass some "no-paste" restrictions in applications.
#### Limitations:
- Spacing may not be preserved depending on application and clipboard content.

### `type-at-wpm`:
Controlled typing speed based on WPM.\
Built as a tool to debug an application which broke at a high but human typing speed, which was too annoying to reproduce manually.

#### Limitations: 
- The WPM is converted to a keystroke delay, which is not representative of actual typing speed.
- The keystroke delay is most likely not representative. (A calibration site is provided to give a quick check).