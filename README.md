# VALAK
**VALAK** is a powerful ESP32-based WiFi penetration testing tool that performs real Evil Twin attacks by creating identical clones of existing WiFi networks to capture credentials and test network security.

## Features

- Creates identical clones of any WiFi network by copying the exact SSID name
- Captures passwords entered on fake router update pages after victims connect
- Stores all captured credentials permanently in EEPROM memory that survives power loss
- Changes cloned network name instantly through web interface without re-uploading
- Supports quick SSID switching using URL parameters like `/ssid?name=STAR_HIGH_SPEED_INTERNET`
- Displays a professional router administration theme that looks like real login pages
- Shows fake device information with realistic MAC addresses and firmware versions
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
- Hosts access point at 192.168.1.1 - Runs web server on port 80 serving a professional phishing page that captures passwords
