## Typing Automation Tools

### Overview
This repository contains two tools designed to automate typing tasks, particularly useful for scenarios where standard pasting functionality is unavailable or inconvenient.
- `type-clipboard`: Automatically types the current content of your clipboard, ideal for applications that don't support traditional pasting (e.g., some remote desktops, virtual machines, or older applications).
- `type-at-wpm`: Types out given text at a specified Words Per Minute (WPM).

### Platform Compatibility
These tools are Windows-specific as they utilise WIN32 API calls for simulating keyboard input.

### Use Cases
- I made `type-at-wpm` to debug a react web-app that broke when the user typed too fast.
- `type-clipboard` was created to fix my problem with getting text into a Proxmox noVNC terminal.

## Features + Limitations:	
### `type-clipboard`:
- Types out clipboard content, instead of pasting it.
- Bypasses "no-paste" restrictions in various applications.
#### Limitations:
- Many UNICODE characters do not work.

### `type-at-wpm` (Experimental):
Controlled typing speed based on WPM.
#### Limitations: 
- May not be able to perfectly match WPM.