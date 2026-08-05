#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

#include "WebAdmin.h"
#include "Settings.h"

WebServer webAdminServer(80);

static const char* ADMIN_USERNAME = "admin";
static const char* ADMIN_PASSWORD = "bambuupdate";

static bool rebootPending = false;
static unsigned long rebootRequestTime = 0;

// ----------------------------------------------------
// Escape text before inserting it into HTML
// ----------------------------------------------------

static String htmlEscape(const String& input)
{
    String output;

    output.reserve(
        input.length() + 16
    );

    for (size_t i = 0; i < input.length(); i++)
    {
        char character = input.charAt(i);

        switch (character)
        {
            case '&':
                output += "&amp;";
                break;

            case '<':
                output += "&lt;";
                break;

            case '>':
                output += "&gt;";
                break;

            case '"':
                output += "&quot;";
                break;

            case '\'':
                output += "&#39;";
                break;

            default:
                output += character;
                break;
        }
    }

    return output;
}


// ----------------------------------------------------
// Authentication
// ----------------------------------------------------

static bool authenticateAdmin()
{
    if (webAdminServer.authenticate(
            ADMIN_USERNAME,
            ADMIN_PASSWORD))
    {
        return true;
    }

    webAdminServer.requestAuthentication();
    return false;
}


// ----------------------------------------------------
// Uptime formatting
// ----------------------------------------------------

static String formatUptime()
{
    unsigned long totalSeconds = millis() / 1000;

    unsigned long days = totalSeconds / 86400;
    totalSeconds %= 86400;

    unsigned long hours = totalSeconds / 3600;
    totalSeconds %= 3600;

    unsigned long minutes = totalSeconds / 60;

    String result;

    if (days > 0)
    {
        result += String(days);
        result += "d ";
    }

    if (hours > 0 || days > 0)
    {
        result += String(hours);
        result += "h ";
    }

    result += String(minutes);
    result += "m";

    return result;
}


// ----------------------------------------------------
// Main administration page
// ----------------------------------------------------

static void handleAdminPage()
{
    if (!authenticateAdmin())
    {
        return;
    }

    String page;

    page.reserve(8500);

    page += R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">

    <meta name="viewport"
          content="width=device-width, initial-scale=1.0">

    <title>Bambu Display Admin</title>

    <style>
        body {
            margin: 0;
            padding: 20px;
            background: #151515;
            color: #ffffff;
            font-family: Arial, sans-serif;
        }

        .container {
            max-width: 520px;
            margin: auto;
        }

        .card {
            background: #242424;
            border-radius: 12px;
            padding: 20px;
            margin-bottom: 18px;
        }

        h1 {
            margin-top: 0;
            font-size: 26px;
        }

        h2 {
            margin-top: 0;
            font-size: 20px;
        }

        .row {
            display: flex;
            justify-content: space-between;
            gap: 20px;
            padding: 9px 0;
            border-bottom: 1px solid #3a3a3a;
        }

        .row:last-child {
            border-bottom: none;
        }

        .label {
            color: #bdbdbd;
        }

        .value {
            text-align: right;
            font-weight: bold;
            overflow-wrap: anywhere;
        }

        form {
            margin: 0;
        }

        .field-label {
            display: block;
            margin-top: 14px;
            margin-bottom: 6px;
            color: #d0d0d0;
            font-weight: bold;
        }

        input[type="text"],
        input[type="password"] {
            box-sizing: border-box;
            width: 100%;
            padding: 12px;
            border: 1px solid #4a4a4a;
            border-radius: 7px;
            background: #181818;
            color: #ffffff;
            font-size: 16px;
        }

        input[type="text"]:focus,
        input[type="password"]:focus {
            outline: none;
            border-color: #008f62;
        }

        .help {
            margin-top: 8px;
            color: #aaaaaa;
            font-size: 13px;
            line-height: 1.4;
        }

        button {
            width: 100%;
            padding: 14px;
            margin-top: 10px;
            border: none;
            border-radius: 8px;
            color: white;
            font-size: 17px;
            cursor: pointer;
        }

        .save-button {
            background: #2676bd;
        }

        .update-button {
            background: #008f62;
        }

        .reboot-button {
            background: #b33a3a;
        }

        .footer {
            text-align: center;
            color: #888888;
            font-size: 13px;
            margin-top: 20px;
        }
    </style>
</head>

<body>

<div class="container">

    <div class="card">
        <h1>Bambu Display</h1>

        <div class="row">
            <span class="label">IP Address</span>
            <span class="value">
)rawliteral";

    page += WiFi.localIP().toString();

    page += R"rawliteral(
            </span>
        </div>

        <div class="row">
            <span class="label">Wi-Fi Network</span>
            <span class="value">
)rawliteral";

    page += WiFi.SSID();

    page += R"rawliteral(
            </span>
        </div>

        <div class="row">
            <span class="label">Wi-Fi Signal</span>
            <span class="value">
)rawliteral";

    page += String(WiFi.RSSI());
    page += " dBm";

    page += R"rawliteral(
            </span>
        </div>

        <div class="row">
            <span class="label">Uptime</span>
            <span class="value">
)rawliteral";

    page += formatUptime();

    page += R"rawliteral(
            </span>
        </div>

        <div class="row">
            <span class="label">Free Heap</span>
            <span class="value">
)rawliteral";

    page += String(ESP.getFreeHeap() / 1024);
    page += " KB";

    page += R"rawliteral(
            </span>
        </div>

        <div class="row">
            <span class="label">Flash Size</span>
            <span class="value">
)rawliteral";

    page += String(ESP.getFlashChipSize() / 1024 / 1024);
    page += " MB";

    page += R"rawliteral(
            </span>
        </div>
    </div>

    <div class="card">
        <h2>Printer Settings</h2>

        <form method="POST"
              action="/save-printer">

            <label class="field-label"
                   for="printer_ip">
                Printer IP Address
            </label>

            <input id="printer_ip"
                   type="text"
                   name="printer_ip"
                   value=")rawliteral";

    page += htmlEscape(printerIP);

    page += R"rawliteral("
                   maxlength="45"
                   autocapitalize="off"
                   autocomplete="off"
                   spellcheck="false"
                   required>

            <label class="field-label"
                   for="printer_serial">
                Printer Serial Number
            </label>

            <input id="printer_serial"
                   type="text"
                   name="printer_serial"
                   value=")rawliteral";

    page += htmlEscape(printerSerial);

    page += R"rawliteral("
                   maxlength="40"
                   autocapitalize="characters"
                   autocomplete="off"
                   spellcheck="false"
                   required>

            <label class="field-label"
                   for="access_code">
                LAN Access Code
            </label>

            <input id="access_code"
                   type="password"
                   name="access_code"
                   maxlength="64"
                   autocomplete="new-password"
                   placeholder="Leave blank to keep the current code">

            <p class="help">
                Access-code status:
                <strong>
)rawliteral";

    page +=
        printerAccessCode.length() > 0
            ? "Configured"
            : "Not configured";

    page += R"rawliteral(
                </strong>
                <br>
                The LAN access code is used as the printer's
                MQTT password. Leave this field blank to retain
                the currently saved code.
            </p>

            <button class="save-button"
                    type="submit">
                Save Printer Settings
            </button>

        </form>
    </div>

    <div class="card">
        <h2>Firmware</h2>

        <button class="update-button"
                type="button"
                onclick="window.location.href='/update';">
            Firmware Update
        </button>
    </div>

    <div class="card">
        <h2>System</h2>

        <form method="POST"
              action="/reboot"
              onsubmit="return confirm('Restart the display?');">

            <button class="reboot-button"
                    type="submit">
                Restart Display
            </button>

        </form>
    </div>

    <div class="footer">
        ESP32-S3 Bambu Display
    </div>

</div>

</body>
</html>
)rawliteral";

    webAdminServer.send(
        200,
        "text/html",
        page
    );
}


// ----------------------------------------------------
// Save printer settings
// ----------------------------------------------------

static void handleSavePrinterSettings()
{
    if (!authenticateAdmin())
    {
        return;
    }

    if (!webAdminServer.hasArg("printer_ip") ||
        !webAdminServer.hasArg("printer_serial"))
    {
        webAdminServer.send(
            400,
            "text/plain",
            "Missing printer settings."
        );

        return;
    }

    String newPrinterIP =
        webAdminServer.arg("printer_ip");

    String newPrinterSerial =
        webAdminServer.arg("printer_serial");

    String newAccessCode =
        webAdminServer.hasArg("access_code")
            ? webAdminServer.arg("access_code")
            : "";

    newPrinterIP.trim();
    newPrinterSerial.trim();
    newAccessCode.trim();

    newPrinterSerial.toUpperCase();


    // Validate printer IP address.
    IPAddress parsedAddress;

    if (!parsedAddress.fromString(newPrinterIP))
    {
        webAdminServer.send(
            400,
            "text/plain",
            "The printer IP address is not valid."
        );

        return;
    }


    if (newPrinterSerial.length() == 0)
    {
        webAdminServer.send(
            400,
            "text/plain",
            "The printer serial number is required."
        );

        return;
    }


    // A code is required when no code has previously been saved.
    if (newAccessCode.length() == 0 &&
        printerAccessCode.length() == 0)
    {
        webAdminServer.send(
            400,
            "text/plain",
            "The LAN access code is required."
        );

        return;
    }


    printerIP = newPrinterIP;
    printerSerial = newPrinterSerial;

    // A blank field means retain the existing code.
    if (newAccessCode.length() > 0)
    {
        printerAccessCode = newAccessCode;
    }


    saveSettings();


    Serial.println();
    Serial.println("Printer settings updated from WebAdmin");

    Serial.print("Printer IP: ");
    Serial.println(printerIP);

    Serial.print("Printer serial: ");
    Serial.println(printerSerial);

    Serial.println("LAN access code: (configured)");


    webAdminServer.send(
        200,
        "text/html",
        R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">

    <meta name="viewport"
          content="width=device-width, initial-scale=1.0">

    <title>Printer Settings Saved</title>

    <style>
        body {
            margin: 0;
            padding: 40px 20px;
            background: #151515;
            color: white;
            font-family: Arial, sans-serif;
            text-align: center;
        }

        .card {
            max-width: 440px;
            margin: 40px auto;
            padding: 24px;
            background: #242424;
            border-radius: 12px;
        }

        .success {
            color: #44d69a;
        }
    </style>
</head>

<body>

<div class="card">
    <h2 class="success">
        Printer settings saved
    </h2>

    <p>
        The display is restarting and will connect
        using the new printer settings.
    </p>

    <p>
        Wait approximately 10 seconds, then reopen
        the administration page.
    </p>
</div>

</body>
</html>
)rawliteral"
    );


    rebootPending = true;
    rebootRequestTime = millis();
}

// ----------------------------------------------------
// Firmware update page
// ----------------------------------------------------

static void handleUpdatePage()
{
    if (!authenticateAdmin())
    {
        return;
    }

    static const char updatePage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">

    <meta name="viewport"
          content="width=device-width, initial-scale=1.0">

    <title>Firmware Update</title>

    <style>
        body {
            margin: 0;
            padding: 20px;
            background: #151515;
            color: white;
            font-family: Arial, sans-serif;
        }

        .card {
            max-width: 480px;
            margin: 40px auto;
            padding: 24px;
            background: #242424;
            border-radius: 12px;
        }

        input[type="file"] {
            width: 100%;
            margin: 20px 0;
        }

        button {
            width: 100%;
            padding: 14px;
            border: none;
            border-radius: 8px;
            background: #008f62;
            color: white;
            font-size: 17px;
            cursor: pointer;
        }

        button:disabled {
            background: #555555;
        }

        progress {
            width: 100%;
            height: 24px;
            margin-top: 20px;
        }

        #status {
            margin-top: 12px;
            min-height: 24px;
        }

        a {
            color: #59c9ff;
        }
    </style>
</head>

<body>

<div class="card">

    <h1>Firmware Update</h1>

    <p>
        Select the compiled firmware
        <strong>.bin</strong> file.
    </p>

    <input id="firmware"
           type="file"
           accept=".bin">

    <button id="uploadButton"
            type="button"
            onclick="uploadFirmware()">
        Upload Firmware
    </button>

    <progress id="progressBar"
              value="0"
              max="100">
    </progress>

    <div id="status"></div>

    <p>
        <a href="/">Return to administration page</a>
    </p>

</div>

<script>
function uploadFirmware()
{
    const fileInput =
        document.getElementById("firmware");

    const button =
        document.getElementById("uploadButton");

    const progressBar =
        document.getElementById("progressBar");

    const status =
        document.getElementById("status");

    if (fileInput.files.length === 0)
    {
        status.textContent =
            "Select a firmware file first.";

        return;
    }

    const file = fileInput.files[0];

    if (!file.name.toLowerCase().endsWith(".bin"))
    {
        status.textContent =
            "The file must end in .bin.";

        return;
    }

    const formData = new FormData();

    formData.append(
        "firmware",
        file
    );

    const request = new XMLHttpRequest();

    request.open(
        "POST",
        "/update",
        true
    );

    request.upload.onprogress = function(event)
    {
        if (event.lengthComputable)
        {
            const percent = Math.round(
                event.loaded /
                event.total *
                100
            );

            progressBar.value = percent;

            status.textContent =
                "Uploading: " +
                percent +
                "%";
        }
    };

    request.onload = function()
    {
        if (request.status === 200)
        {
            progressBar.value = 100;

            status.textContent =
                "Update complete. Display is restarting.";
        }
        else
        {
            status.textContent =
                "Update failed: " +
                request.responseText;

            button.disabled = false;
        }
    };

    request.onerror = function()
    {
        status.textContent =
            "Connection lost during upload.";

        button.disabled = false;
    };

    button.disabled = true;

    status.textContent =
        "Starting firmware upload...";

    request.send(formData);
}
</script>

</body>
</html>
)rawliteral";

    webAdminServer.send_P(
        200,
        "text/html",
        updatePage
    );
}


// ----------------------------------------------------
// Firmware upload handler
// ----------------------------------------------------

static void handleFirmwareUpload()
{
    if (!webAdminServer.authenticate(
            ADMIN_USERNAME,
            ADMIN_PASSWORD))
    {
        return;
    }

    HTTPUpload& upload =
        webAdminServer.upload();

    switch (upload.status)
    {
        case UPLOAD_FILE_START:
        {
            Serial.println();
            Serial.println(
                "Firmware update starting..."
            );

            Serial.print("Filename: ");
            Serial.println(upload.filename);

            if (!upload.filename.endsWith(".bin"))
            {
                Serial.println(
                    "Invalid firmware filename."
                );

                Update.abort();
                break;
            }

            if (!Update.begin(UPDATE_SIZE_UNKNOWN))
            {
                Serial.println(
                    "Unable to begin update."
                );

                Update.printError(Serial);
            }

            break;
        }

        case UPLOAD_FILE_WRITE:
        {
            if (Update.hasError())
            {
                break;
            }

            size_t bytesWritten =
                Update.write(
                    upload.buf,
                    upload.currentSize
                );

            if (bytesWritten != upload.currentSize)
            {
                Serial.println(
                    "Firmware write failed."
                );

                Update.printError(Serial);
            }

            break;
        }

        case UPLOAD_FILE_END:
        {
            if (Update.end(true))
            {
                Serial.printf(
                    "Firmware update complete: %u bytes\n",
                    upload.totalSize
                );
            }
            else
            {
                Serial.println(
                    "Unable to finish update."
                );

                Update.printError(Serial);
            }

            break;
        }

        case UPLOAD_FILE_ABORTED:
        {
            Update.abort();

            Serial.println(
                "Firmware upload aborted."
            );

            break;
        }

        default:
            break;
    }
}


// ----------------------------------------------------
// Firmware update completion response
// ----------------------------------------------------

static void handleUpdateFinished()
{
    if (!authenticateAdmin())
    {
        return;
    }

    webAdminServer.sendHeader(
        "Connection",
        "close"
    );

    if (Update.hasError())
    {
        webAdminServer.send(
            500,
            "text/plain",
            "Firmware update failed."
        );

        Serial.println(
            "Firmware update failed."
        );

        Update.printError(Serial);
    }
    else
    {
        webAdminServer.send(
            200,
            "text/plain",
            "Firmware update successful."
        );

        Serial.println(
            "Firmware update successful."
        );

        rebootPending = true;
        rebootRequestTime = millis();
    }
}


// ----------------------------------------------------
// Restart handler
// ----------------------------------------------------

static void handleReboot()
{
    if (!authenticateAdmin())
    {
        return;
    }

    webAdminServer.send(
        200,
        "text/html",
        R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">

    <meta name="viewport"
          content="width=device-width, initial-scale=1.0">

    <title>Restarting</title>

    <style>
        body {
            margin: 0;
            padding: 40px 20px;
            background: #151515;
            color: white;
            font-family: Arial, sans-serif;
            text-align: center;
        }

        .card {
            max-width: 420px;
            margin: 40px auto;
            padding: 24px;
            background: #242424;
            border-radius: 12px;
        }
    </style>
</head>

<body>

<div class="card">
    <h2>Display is restarting...</h2>
    <p>Please wait a few seconds, then return to the administration page.</p>
</div>

</body>
</html>
)rawliteral"
    );

    Serial.println(
        "Restart requested from WebAdmin."
    );

    rebootPending = true;
    rebootRequestTime = millis();
}


// ----------------------------------------------------
// Start web server
// ----------------------------------------------------

void initWebAdmin()
{
    webAdminServer.on(
        "/",
        HTTP_GET,
        handleAdminPage
    );

    webAdminServer.on(
    "/save-printer",
    HTTP_POST,
    handleSavePrinterSettings
    );

    webAdminServer.on(
        "/update",
        HTTP_GET,
        handleUpdatePage
    );

    webAdminServer.on(
        "/update",
        HTTP_POST,
        handleUpdateFinished,
        handleFirmwareUpload
    );

    webAdminServer.on(
        "/reboot",
        HTTP_POST,
        handleReboot
    );

    webAdminServer.onNotFound([]()
    {
        webAdminServer.send(
            404,
            "text/plain",
            "Page not found."
        );
    });

    webAdminServer.begin();

    Serial.println();
    Serial.println(
        "WebAdmin started."
    );

    Serial.print(
        "Administration page: http://"
    );

    Serial.println(
        WiFi.localIP()
    );
}

// ----------------------------------------------------
// Stop web administration server
// ----------------------------------------------------

void stopWebAdmin()
{
    webAdminServer.stop();

    Serial.println();
    Serial.println("WebAdmin stopped.");
}


// ----------------------------------------------------
// Process web requests and pending restart
// ----------------------------------------------------

void updateWebAdmin()
{
    webAdminServer.handleClient();

    if (rebootPending &&
        millis() - rebootRequestTime >= 1500)
    {
        Serial.println(
            "Restarting display..."
        );

        delay(100);

        ESP.restart();
    }
}