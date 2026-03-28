// #include "wifi_setup.h"
// #include "secrets.h"
// #include "state_json_codec.h"
// #include "state_service.h"
// #include <WiFi.h>

// static const uint8_t BOOT_MAX_ATTEMPTS = 3;
// static const unsigned long ATTEMPT_TIMEOUT_MS = 15000;
// static const unsigned long RESCUE_START_MS = 120000;
// static const unsigned long STABLE_DISABLE_AP_MS = 60000;
// static const unsigned long CONNECTION_LOST_COOLDOWN_MS = 2000;
// static const unsigned long PROVISION_REQUEST_DEDUP_MS = 1500;
// static const unsigned long STA_BEGIN_MIN_INTERVAL_MS = 3000;
// static const unsigned long POST_CONNECT_RETRY_HOLDOFF_MS = 3000;
// static const unsigned long IDLE_STUCK_RESET_MS = 15000;
// static const size_t WIFI_MAX_SSID_LEN = 32;
// static const size_t WIFI_MAX_PASS_LEN = 63;
// static const uint8_t RETRY_BACKOFF_S[] = {5, 10, 20, 30};

// static const char *PROVISION_AP_SSID = "webled-setup";
// static const char *PROVISION_AP_PASSWORD =
//     "webled-setup"; // has to be at least 8 chars

// static String gStoredSsid = WIFI_SSID;
// static String gStoredPassword = WIFI_PASSWORD;
// // static String gStoredSsid = ""; // DO NOT REMOVE
// // static String gStoredPassword = ""; // DO NOT REMOVE

// static String gPendingSsid;
// static String gPendingPassword;
// static bool gHasPendingCredentials = false;

// static bool gBootSequenceActive = false;
// static bool gHasEverConnected = false;
// static bool gAttemptInProgress = false;
// static bool gApProvisioningEnabled = false;
// static bool gApRescueEnabled = false;

// static uint8_t gAttemptCount = 0;
// static uint8_t gBackoffIndex = 0;

// static unsigned long gAttemptStartedAt = 0;
// static unsigned long gNextRetryAt = 0;
// static unsigned long gDisconnectedSince = 0;
// static unsigned long gStableConnectedSince = 0;
// static unsigned long gLastRetryWaitLogAt = 0;
// static unsigned long gLastProvisionRequestAt = 0;
// static unsigned long gLastStaBeginAt = 0;
// static unsigned long gLastIdleDeferralLogAt = 0;
// static unsigned long gLastConnectedAt = 0;
// static unsigned long gIdleSince = 0;

// static String gLastProvisionSsid;
// static String gLastProvisionPassword;

// static const char *wifiModeToString(wifi_mode_t mode) {
//     switch (mode) {
//     case WIFI_MODE_STA:
//         return "sta";
//     case WIFI_MODE_AP:
//         return "ap";
//     case WIFI_MODE_APSTA:
//         return "ap_sta";
//     default:
//         return "boot";
//     }
// }

// static const char *wlStatusToString(wl_status_t status) {
//     switch (status) {
//     case WL_IDLE_STATUS:
//         return "idle";
//     case WL_NO_SSID_AVAIL:
//         return "no_ssid";
//     case WL_SCAN_COMPLETED:
//         return "scan_completed";
//     case WL_CONNECTED:
//         return "connected";
//     case WL_CONNECT_FAILED:
//         return "connect_failed";
//     case WL_CONNECTION_LOST:
//         return "connection_lost";
//     case WL_DISCONNECTED:
//         return "disconnected";
//     default:
//         return "unknown";
//     }
// }

// static void logWiFiEvent(const String &message) {
//     Serial.printf("[wifi-sm][%10lu] %s\n", millis(), message.c_str());
// }

// static WiFiModeState toWiFiModeState(wifi_mode_t mode) {
//     switch (mode) {
//     case WIFI_MODE_STA:
//         return WiFiModeState::Sta;
//     case WIFI_MODE_AP:
//         return WiFiModeState::Ap;
//     case WIFI_MODE_APSTA:
//         return WiFiModeState::ApSta;
//     default:
//         return WiFiModeState::Boot;
//     }
// }

// static bool hasRetryCredentials() {
//     return gHasPendingCredentials ? gPendingSsid.length() > 0
//                                   : gStoredSsid.length() > 0;
// }

// static const String &activeSsid() {
//     return gHasPendingCredentials ? gPendingSsid : gStoredSsid;
// }

// static const String &activePassword() {
//     return gHasPendingCredentials ? gPendingPassword : gStoredPassword;
// }

// static bool validateActiveCredentials(String &error) {
//     String ssid = activeSsid();
//     ssid.trim();

//     if (ssid.length() == 0) {
//         error = "active ssid is missing";
//         return false;
//     }

//     if (ssid.length() > WIFI_MAX_SSID_LEN) {
//         error = "active ssid is too long";
//         return false;
//     }

//     if (activePassword().length() > WIFI_MAX_PASS_LEN) {
//         error = "active password is too long";
//         return false;
//     }

//     return true;
// }

// static void resetStaDriver(const String &reason) {
//     logWiFiEvent("Resetting STA driver: " + reason);
//     WiFi.disconnect(true, true);
//     delay(20);

//     if (gApProvisioningEnabled || gApRescueEnabled) {
//         WiFi.mode(WIFI_AP_STA);
//         WiFi.softAP(PROVISION_AP_SSID, PROVISION_AP_PASSWORD);
//     } else {
//         WiFi.mode(WIFI_STA);
//     }

//     gAttemptInProgress = false;
//     gAttemptStartedAt = 0;
//     gIdleSince = 0;
// }

// static void setDisconnectedState(WiFiLinkState link) {
//     String ip = gApProvisioningEnabled || gApRescueEnabled
//                     ? WiFi.softAPIP().toString()
//                     : String("0.0.0.0");
//     stateServiceUpdateWiFiStatus(toWiFiModeState(WiFi.getMode()), link,
//                                  activeSsid(), ip, 0);
// }

// static void startProvisioningAp(bool includeSta) {
//     wifi_mode_t currentMode = WiFi.getMode();
//     bool modeMatches = includeSta ? currentMode == WIFI_MODE_APSTA
//                                   : currentMode == WIFI_MODE_AP;
//     if (gApProvisioningEnabled && modeMatches) {
//         if (!includeSta) {
//             gApRescueEnabled = false;
//         }
//         return;
//     }

//     if (includeSta) {
//         WiFi.mode(WIFI_AP_STA);
//     } else {
//         WiFi.mode(WIFI_AP);
//     }

//     bool apOk = WiFi.softAP(PROVISION_AP_SSID, PROVISION_AP_PASSWORD);

//     gApProvisioningEnabled = true;
//     if (!includeSta) {
//         gApRescueEnabled = false;
//     }

//     String modeLabel = includeSta ? "ap_sta" : "ap";
//     String eventLabel =
//         includeSta ? "AP+STA mode enabled" : "AP provisioning enabled";
//     logWiFiEvent(eventLabel + " mode=" + modeLabel +
//                  " ssid=" + String(PROVISION_AP_SSID) +
//                  " ap_ip=" + WiFi.softAPIP().toString() +
//                  " ap_ok=" + String(apOk ? "true" : "false"));
// }

// static void disableApIfEnabled() {
//     if (!gApProvisioningEnabled && !gApRescueEnabled) {
//         return;
//     }

//     WiFi.softAPdisconnect(true);
//     WiFi.mode(WIFI_STA);
//     gApProvisioningEnabled = false;
//     gApRescueEnabled = false;
//     logWiFiEvent("AP disabled after stable STA");
// }

// static void scheduleRetry(unsigned long now);

// static void startStaAttempt(bool keepApEnabled) {
//     if (!hasRetryCredentials()) {
//         return;
//     }

//     unsigned long now = millis();
//     String credentialError;
//     if (!validateActiveCredentials(credentialError)) {
//         logWiFiEvent("Skipping STA attempt: " + credentialError);

//         if (gHasPendingCredentials) {
//             gHasPendingCredentials = false;
//             gPendingSsid = "";
//             gPendingPassword = "";
//             logWiFiEvent("Dropped invalid pending credentials");
//         }

//         if (!gBootSequenceActive) {
//             scheduleRetry(now);
//             setDisconnectedState(WiFiLinkState::Failed);
//         }

//         return;
//     }

//     if (gLastConnectedAt != 0 &&
//         now - gLastConnectedAt < POST_CONNECT_RETRY_HOLDOFF_MS) {
//         gNextRetryAt = gLastConnectedAt + static_cast<unsigned long>(
//                                               POST_CONNECT_RETRY_HOLDOFF_MS);
//         if (now - gLastRetryWaitLogAt >= 1000) {
//             long holdoffMs = static_cast<long>(gNextRetryAt - now);
//             if (holdoffMs < 0) {
//                 holdoffMs = 0;
//             }
//             logWiFiEvent("Holding off reconnect for " + String(holdoffMs) +
//                          "ms after recent connect");
//             gLastRetryWaitLogAt = now;
//         }
//         return;
//     }

//     if (gLastStaBeginAt != 0 &&
//         now - gLastStaBeginAt < STA_BEGIN_MIN_INTERVAL_MS) {
//         gNextRetryAt = gLastStaBeginAt + STA_BEGIN_MIN_INTERVAL_MS;
//         return;
//     }

//     if (WiFi.status() == WL_CONNECTED) {
//         if (WiFi.SSID() == activeSsid()) {
//             logWiFiEvent(
//                 "Skipping STA attempt, already connected to target SSID");

//             gAttemptInProgress = false;
//             gAttemptCount = 0;
//             gBootSequenceActive = false;
//             gNextRetryAt = 0;
//             return;
//         }

//         logWiFiEvent("Disconnecting from current SSID before switching to " +
//                      activeSsid());

//         WiFi.disconnect();
//         gAttemptInProgress = false;
//         gAttemptStartedAt = 0;
//         gNextRetryAt = now + CONNECTION_LOST_COOLDOWN_MS;
//         stateServiceUpdateWiFiStatus(toWiFiModeState(WiFi.getMode()),
//                                      WiFiLinkState::Connecting, activeSsid(),
//                                      "0.0.0.0", 0);
//         return;
//     }

//     if (WiFi.status() == WL_IDLE_STATUS) {
//         if (gIdleSince == 0) {
//             gIdleSince = now;
//         }

//         if (now - gIdleSince >= IDLE_STUCK_RESET_MS) {
//             resetStaDriver("wl=idle for 15s");
//             scheduleRetry(now);
//             setDisconnectedState(WiFiLinkState::Failed);
//             return;
//         }

//         gNextRetryAt = now + CONNECTION_LOST_COOLDOWN_MS;
//         if (now - gLastIdleDeferralLogAt >= 5000) {
//             logWiFiEvent(
//                 "Deferring STA attempt while driver is busy (wl=idle)");

//             gLastIdleDeferralLogAt = now;
//         }
//         return;
//     }

//     gIdleSince = 0;

//     if (keepApEnabled || gApProvisioningEnabled || gApRescueEnabled) {
//         WiFi.mode(WIFI_AP_STA);
//     } else {
//         WiFi.mode(WIFI_STA);
//     }

//     uint8_t attemptNo = gAttemptCount + 1;
//     String modeBeforeBegin = wifiModeToString(
//         keepApEnabled || gApProvisioningEnabled || gApRescueEnabled
//             ? WIFI_MODE_APSTA
//             : WIFI_MODE_STA);

//     logWiFiEvent("Starting STA attempt #" + String(attemptNo) +
//                  " mode=" + modeBeforeBegin + " ssid=" + activeSsid() +
//                  " ssid_bytes=" + String(activeSsid().length()));

//     gLastStaBeginAt = now;
//     gLastRetryWaitLogAt = 0;
//     WiFi.begin(activeSsid().c_str(), activePassword().c_str());

//     gAttemptInProgress = true;
//     gAttemptStartedAt = millis();
//     gAttemptCount++;

//     stateServiceUpdateWiFiStatus(toWiFiModeState(WiFi.getMode()),
//                                  WiFiLinkState::Connecting, activeSsid(),
//                                  "0.0.0.0", 0);
// }

// static void scheduleRetry(unsigned long now) {
//     uint8_t idx = gBackoffIndex;
//     uint8_t maxIdx = static_cast<uint8_t>(
//         sizeof(RETRY_BACKOFF_S) / sizeof(RETRY_BACKOFF_S[0]) - 1);

//     if (idx > maxIdx) {
//         idx = maxIdx;
//     }

//     gNextRetryAt = now + static_cast<unsigned long>(RETRY_BACKOFF_S[idx]) *
//                              static_cast<unsigned long>(1000);

//     logWiFiEvent("Scheduling retry in " + String(RETRY_BACKOFF_S[idx]) +
//                  "s (wl=" + String(wlStatusToString(WiFi.status())) + ")");

//     if (gBackoffIndex < maxIdx) {
//         gBackoffIndex++;
//     }
// }

// static void commitCredentialsOnSuccess() {
//     if (!gHasPendingCredentials) {
//         stateServiceSetWiFiCredentials(gStoredSsid, gStoredSsid.length() >
//         0); return;
//     }

//     if (WiFi.status() != WL_CONNECTED || WiFi.SSID() != gPendingSsid) {
//         return;
//     }

//     gStoredSsid = gPendingSsid;
//     gStoredPassword = gPendingPassword;
//     gHasPendingCredentials = false;
//     gPendingSsid = "";
//     gPendingPassword = "";

//     stateServiceSetWiFiCredentials(gStoredSsid, gStoredSsid.length() > 0);
//     logWiFiEvent("Committed new STA credentials after successful
//     association");
// }

// static void onConnected(unsigned long now) {
//     bool justConnected = gStableConnectedSince == 0;

//     gHasEverConnected = true;
//     gBootSequenceActive = false;
//     gAttemptInProgress = false;
//     gAttemptCount = 0;
//     gBackoffIndex = 0;
//     gNextRetryAt = 0;
//     gDisconnectedSince = 0;
//     gIdleSince = 0;
//     gLastConnectedAt = now;

//     if (justConnected) {
//         gStableConnectedSince = now;
//         logWiFiEvent("STA connected ip=" + WiFi.localIP().toString() +
//                      " rssi=" + String(WiFi.RSSI()) +
//                      " mode=" + String(wifiModeToString(WiFi.getMode())));
//     }

//     commitCredentialsOnSuccess();

//     stateServiceUpdateWiFiStatus(toWiFiModeState(WiFi.getMode()),
//                                  WiFiLinkState::Connected, WiFi.SSID(),
//                                  WiFi.localIP().toString(), WiFi.RSSI());

//     if ((gApProvisioningEnabled || gApRescueEnabled) &&
//         now - gStableConnectedSince >= STABLE_DISABLE_AP_MS) {
//         disableApIfEnabled();
//         stateServiceUpdateWiFiStatus(WiFiModeState::Sta,
//                                      WiFiLinkState::Connected, WiFi.SSID(),
//                                      WiFi.localIP().toString(), WiFi.RSSI());
//     }
// }

// static void onAttemptTimedOut(unsigned long now) {
//     (void)now;
//     gAttemptInProgress = false;
//     WiFi.disconnect();

//     logWiFiEvent("STA attempt timed out after " +
//                  String(ATTEMPT_TIMEOUT_MS / 1000) + "s");

//     if (gBootSequenceActive) {
//         if (gAttemptCount >= BOOT_MAX_ATTEMPTS) {
//             gBootSequenceActive = false;
//             if (gHasPendingCredentials) {
//                 gHasPendingCredentials = false;
//                 gPendingSsid = "";
//                 gPendingPassword = "";

//                 logWiFiEvent("Provisioning connect failed, pending
//                 credentials "
//                              "discarded");
//             }

//             startProvisioningAp(false);
//             setDisconnectedState(WiFiLinkState::Failed);

//             logWiFiEvent(
//                 "Boot attempts exhausted, switching to AP provisioning
//                 mode");

//             return;
//         }

//         startStaAttempt(gApProvisioningEnabled || gApRescueEnabled);
//         return;
//     }

//     setDisconnectedState(WiFiLinkState::Failed);
//     scheduleRetry(now);
// }

// void setupWiFi() {
//     logWiFiEvent("Wi-Fi state machine init");
//     WiFi.setAutoReconnect(false);
//     stateServiceSetWiFiCredentials(gStoredSsid, gStoredSsid.length() > 0);

//     gHasEverConnected = false;
//     gAttemptInProgress = false;
//     gApProvisioningEnabled = false;
//     gApRescueEnabled = false;
//     gAttemptCount = 0;
//     gBackoffIndex = 0;
//     gAttemptStartedAt = 0;
//     gNextRetryAt = 0;
//     gDisconnectedSince = 0;
//     gStableConnectedSince = 0;
//     gLastRetryWaitLogAt = 0;
//     gLastProvisionRequestAt = 0;
//     gLastStaBeginAt = 0;
//     gLastIdleDeferralLogAt = 0;
//     gLastConnectedAt = 0;
//     gIdleSince = 0;
//     gLastProvisionSsid = "";
//     gLastProvisionPassword = "";

//     if (gStoredSsid.length() == 0) {
//         gBootSequenceActive = false;
//         startProvisioningAp(false);
//         setDisconnectedState(WiFiLinkState::Idle);

//         logWiFiEvent("No saved credentials, waiting in AP provisioning
//         mode");

//         return;
//     }

//     gBootSequenceActive = true;

//     logWiFiEvent("Boot connect sequence started (max_attempts=" +
//                  String(BOOT_MAX_ATTEMPTS) +
//                  ", timeout_s=" + String(ATTEMPT_TIMEOUT_MS / 1000) + ")");

//     startStaAttempt(false);
// }

// void wifiSetupTick() {
//     unsigned long now = millis();
//     wl_status_t currentStatus = WiFi.status();

//     if (currentStatus == WL_CONNECTED) {
//         if (gHasPendingCredentials && WiFi.SSID() != activeSsid()) {
//             startStaAttempt(gApProvisioningEnabled || gApRescueEnabled);
//             return;
//         }

//         onConnected(now);
//         return;
//     }

//     gStableConnectedSince = 0;
//     if (gDisconnectedSince == 0) {
//         gDisconnectedSince = now;

//         logWiFiEvent(
//             "STA disconnected, wl=" + String(wlStatusToString(currentStatus))
//             +
//             ", recovery timer started");

//         if (!gBootSequenceActive && currentStatus == WL_CONNECTION_LOST) {
//             unsigned long cooldownUntil = now + CONNECTION_LOST_COOLDOWN_MS;

//             if (gNextRetryAt == 0 || gNextRetryAt < cooldownUntil) {
//                 gNextRetryAt = cooldownUntil;
//             }

//             logWiFiEvent(
//                 "Applying post-loss cooldown before reconnect attempt");
//         }
//     }

//     if (gAttemptInProgress) {
//         if (now - gAttemptStartedAt >= ATTEMPT_TIMEOUT_MS) {
//             onAttemptTimedOut(now);
//         }
//         return;
//     }

//     if (!hasRetryCredentials()) {
//         startProvisioningAp(false);
//         setDisconnectedState(WiFiLinkState::Idle);
//         return;
//     }

//     if (gBootSequenceActive) {
//         if (gAttemptCount < BOOT_MAX_ATTEMPTS) {
//             startStaAttempt(gApProvisioningEnabled || gApRescueEnabled);
//         } else {
//             gBootSequenceActive = false;
//             startProvisioningAp(false);
//             setDisconnectedState(WiFiLinkState::Failed);
//         }
//         return;
//     }

//     if (gHasEverConnected && (now - gDisconnectedSince >= RESCUE_START_MS) &&
//         !gApRescueEnabled) {
//         startProvisioningAp(true);
//         gApRescueEnabled = true;
//         setDisconnectedState(WiFiLinkState::Disconnected);
//         logWiFiEvent("Rescue threshold reached (120s), enabling AP+STA
//         mode");
//     }

//     if (gNextRetryAt == 0 || now >= gNextRetryAt) {
//         startStaAttempt(gApProvisioningEnabled || gApRescueEnabled);
//     } else {
//         setDisconnectedState(WiFiLinkState::Disconnected);
//         if (now - gLastRetryWaitLogAt >= 5000) {
//             long msRemaining = static_cast<long>(gNextRetryAt - now);
//             if (msRemaining < 0) {
//                 msRemaining = 0;
//             }
//             logWiFiEvent("Waiting for retry window, next attempt in " +
//                          String(msRemaining) + "ms");
//             gLastRetryWaitLogAt = now;
//         }
//     }
// }

// bool wifiStartProvisioningConnect(const String &ssid, const String &password,
//                                   String &error) {
//     unsigned long now = millis();
//     String trimmedSsid = ssid;
//     trimmedSsid.trim();

//     if (trimmedSsid.length() == 0) {
//         error = "ssid is required";
//         return false;
//     }

//     if (trimmedSsid.length() > WIFI_MAX_SSID_LEN) {
//         error = "ssid must be <= 32 bytes";
//         logWiFiEvent("Rejecting provisioning request: ssid too long (bytes="
//         +
//                      String(trimmedSsid.length()) + ")");
//         return false;
//     }

//     if (password.length() > WIFI_MAX_PASS_LEN) {
//         error = "password must be <= 63 bytes";
//         logWiFiEvent(
//             "Rejecting provisioning request: password too long (bytes=" +
//             String(password.length()) + ")");
//         return false;
//     }

//     if (now - gLastProvisionRequestAt <= PROVISION_REQUEST_DEDUP_MS &&
//         trimmedSsid == gLastProvisionSsid &&
//         password == gLastProvisionPassword) {
//         logWiFiEvent("Ignoring duplicate provisioning request in dedup
//         window"); return true;
//     }

//     if (WiFi.status() == WL_CONNECTED && WiFi.SSID() == trimmedSsid &&
//         !gHasPendingCredentials) {
//         stateServiceSetWiFiCredentials(trimmedSsid, true);

//         logWiFiEvent(
//             "Provisioning request matches active STA, skipping reconnect");

//         gLastProvisionRequestAt = now;
//         gLastProvisionSsid = trimmedSsid;
//         gLastProvisionPassword = password;

//         return true;
//     }

//     logWiFiEvent("Provisioning connect requested ssid=" + trimmedSsid +
//                  " ssid_bytes=" + String(trimmedSsid.length()) +
//                  " password_len=" + String(password.length()));

//     gLastProvisionRequestAt = now;
//     gLastProvisionSsid = trimmedSsid;
//     gLastProvisionPassword = password;

//     gPendingSsid = trimmedSsid;
//     gPendingPassword = password;
//     gHasPendingCredentials = true;

//     gBootSequenceActive = true;
//     gAttemptInProgress = false;
//     gAttemptCount = 0;
//     gBackoffIndex = 0;
//     gNextRetryAt = 0;
//     gDisconnectedSince = now;
//     gStableConnectedSince = 0;

//     startProvisioningAp(true);
//     startStaAttempt(true);

//     return true;
// }

// static const char *authModeToString(wifi_auth_mode_t authMode) {
//     switch (authMode) {
//     case WIFI_AUTH_OPEN:
//         return "open";
//     case WIFI_AUTH_WEP:
//         return "wep";
//     case WIFI_AUTH_WPA_PSK:
//         return "wpa_psk";
//     case WIFI_AUTH_WPA2_PSK:
//         return "wpa2_psk";
//     case WIFI_AUTH_WPA_WPA2_PSK:
//         return "wpa_wpa2_psk";
//     case WIFI_AUTH_WPA2_ENTERPRISE:
//         return "wpa2_enterprise";
//     case WIFI_AUTH_WPA3_PSK:
//         return "wpa3_psk";
//     case WIFI_AUTH_WPA2_WPA3_PSK:
//         return "wpa2_wpa3_psk";
//     case WIFI_AUTH_WAPI_PSK:
//         return "wapi_psk";
//     default:
//         return "unknown";
//     }
// }

// String wifiScanNetworksJson() {
//     WiFiScanResult scanResult;

//     int scanState = WiFi.scanComplete();
//     if (scanState == WIFI_SCAN_RUNNING) {
//         scanResult.status = WiFiScanStatus::Running;
//     } else if (scanState < 0) {
//         WiFi.scanDelete();
//         WiFi.scanNetworks(true, true);
//         scanResult.status = WiFiScanStatus::Started;
//     } else {
//         scanResult.status = WiFiScanStatus::Complete;

//         int networkCount = scanState;
//         if (networkCount > MAX_WIFI_SCAN_NETWORKS) {
//             networkCount = MAX_WIFI_SCAN_NETWORKS;
//         }

//         scanResult.count = static_cast<uint8_t>(networkCount);

//         for (uint8_t i = 0; i < scanResult.count; i++) {
//             scanResult.networks[i].ssid = WiFi.SSID(i);
//             scanResult.networks[i].rssi = WiFi.RSSI(i);
//             scanResult.networks[i].channel = WiFi.channel(i);
//             scanResult.networks[i].auth =
//                 authModeToString(WiFi.encryptionType(i));
//         }

//         WiFi.scanDelete();
//     }

//     return serializeWiFiScanResultJson(scanResult);
// }

#include "wifi_setup.h"
#include "WiFi.h"
#include "secrets.h"
#include <Arduino.h>

static const String ssid = WIFI_SSID;
static const String password = WIFI_PASSWORD;
// static const String ssid = WIFI_SSID_2;
// static const String password = WIFI_PASSWORD_2;

static const String AP_SSID = "webled-setup";
static const String AP_PASSWORD = "webled-setup";

static const unsigned short CONNECT_TIMEOUT_MS = 15000;

static void onWiFiEvent(WiFiEvent_t event) {
    switch (event) {
    // STA events
    case ARDUINO_EVENT_WIFI_STA_START:
        Serial.println("[WiFi] STA started");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.println("[WiFi] STA connected to AP");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.println("[WiFi] STA got IP: " + WiFi.localIP().toString());
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("[WiFi] STA disconnected (AutoReconnect will retry)");
        break;
    // AP events
    case ARDUINO_EVENT_WIFI_AP_START:
        Serial.println("[WiFi] AP started");
        break;
    default:
        Serial.println("[WiFi] Unhandled event: " + String(event));
        break;
    }
}

void setupWiFi() {
    Serial.println("[WiFi] Initializing WiFi...");
    WiFi.onEvent(onWiFiEvent);
    WiFi.mode(WIFI_AP_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);

    // wait for connection, but don't block forever if credentials are wrong
    unsigned long connectStartMs = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - connectStartMs < CONNECT_TIMEOUT_MS) {
        delay(1000);
        Serial.print("[WiFi] Connecting to WiFi... " +
                     String((millis() - connectStartMs) / 1000) + "s\r");
    }
    Serial.println("[WiFi] STA Connection attempt finished");

    WiFi.softAP(AP_SSID, AP_PASSWORD);

    // set custom IP for AP
    IPAddress apIP(10, 0, 0, 1);
    IPAddress netMsk(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    Serial.println("[WiFi] AP Configured");
    Serial.println("[WiFi] AP IP address: " + WiFi.softAPIP().toString());
    Serial.println("[WiFi] AP Mask: " + WiFi.softAPSubnetMask().toString());
}