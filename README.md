# VALAK
**VALAK** is a powerful ESP32-based tool that performs real Evil Twin attacks by creating identical clones of existing WiFi networks.

## VALAK in Action

https://github.com/user-attachments/assets/7948f107-c219-4b93-a122-9849da337667




## Security Research & Educational Purpose

**VALAK** is developed for **security research, WiFi penetration testing, and educational purposes**. The project demonstrates how Evil Twin attacks and WiFi credential harvesting techniques can be implemented using an **ESP32-based platform** to simulate real-world wireless security threats.

The goal of this project is to help **cybersecurity students, researchers, and network security professionals** understand how wireless credential harvesting attacks work in practice. By studying these techniques, security practitioners can better identify **malicious access points, rogue networks, and Evil Twin attacks**, improving detection mechanisms and strengthening wireless network defenses.

This project promotes **responsible security research and awareness** about the risks associated with insecure WiFi networks and credential interception. The intention is to help individuals and organizations improve their **wireless security posture** by understanding how these attacks operate.

## Legal Disclaimer

This tool is intended **strictly for authorized security testing, research, and educational purposes only**.

By using this software, you agree that:

- You will only use this tool on **networks you own** or **have explicit written permission to test**.
- You will **not use this tool for illegal WiFi attacks, credential harvesting, or any malicious activities**.
- Any misuse of this software is **the sole responsibility of the user**.
- The developer **assumes no liability** and is **not responsible for any misuse, damage, or legal consequences** caused by this tool.
- Users must comply with **all applicable local, state, national, and international laws**.
- Unauthorized use of this tool may violate **cybersecurity, privacy, and computer misuse laws**.
- The author of this project is **not responsible for any illegal use or misuse of this software**.

Use responsibly and ethically.

## Features

- Creates identical clones of any WiFi network by copying the exact SSID name
- Captures passwords entered on fake router update pages after victims connect
- Stores all captured credentials permanently in EEPROM memory that survives power loss
- Changes cloned network name instantly through web interface without re-uploading
- Displays a professional router administration theme that looks like real login pages
- Shows fake device information with realistic MAC addresses and firmware versions
-  ** Admin authentication system** - Protects `/pass`, `/clear`, `/ssid`, `/footer`, `/brand` pages with password
- **Default admin password** `admin123` (configurable in code)
- Supports quick SSID switching using URL parameters like `/ssid?name=STAR_HIGH_SPEED_INTERNET`
- Views all captured passwords in a clean, organized interface at the `/pass` endpoint
- Access network settings and configuration options through the `/pass` page
- Navigate to SSID, footer, and brand settings pages from the `/pass` interface using buttons
- Navigate to clear logs and management functions from the `/pass` interface
- Clears all stored credentials with the `/clear` command
- Blinks built-in LED 6 times whenever a new password is captured
- Spoofs DNS to redirect all web requests to the phishing page
- Implements captive portal so users see login page when connecting
- Handles up to 8 concurrent victim connections
- Runs on ESP32 platform with 512 bytes EEPROM storage
- Stores over 70 passwords simultaneously
- Hosts access point at `192.168.1.1`
- Runs web server on port 80 serving a professional phishing page that captures passwords

## Installation

### Requirements
- ESP32 development board
- USB cable
- Arduino IDE

### Steps
1. Install ESP32 board package in Arduino IDE
  
2. Download the VALAK code

3. Open VALAK.ino in Arduino IDE

4. Select your ESP32 board from Tools → Board
  
5. Select the correct COM port

6. Click Upload button

7. Wait for compilation and upload to complete

## Admin Panel
After connecting to VALAK's WiFi, open browser and go to `http://192.168.1.1/pass` to access the control panel. You will need to enter the admin password. The default password is `admin123`. You can change this in the code by modifying the `adminPassword` variable.

## Access Points

| URL | Page Type |
|-----|-----------|
| `http://192.168.1.1` | Main phishing page |
| `http://192.168.1.1/pass` | Password viewer with admin controls |
| `http://192.168.1.1/clear` | Clear all credentials |
| `http://192.168.1.1/ssid` | Change SSID settings |
| `http://192.168.1.1/footer` | Modify footer text |
| `http://192.168.1.1/brand` | Change router identity |
| `http://192.168.1.1/ssid?name=X` | Instant SSID change |


## Admin Control Center
**All administration is done from the passwords page:**

```text
http://192.168.1.1/pass
         │
         ├── [Clear]    →  Clears all passwords
         ├── [Settings] →  Change SSID name
         ├── [Footer]   →  Edit copyright text
         ├── [Brand]    →  Modify router identity   
         └── [Logout]   →  For Logout

```

## License
This project is licensed under the MIT License.
