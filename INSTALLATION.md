Use the official ESPHome Web installer here:

https://web.esphome.io/

In your instructions, that section can read:

1. Open Google Chrome or Microsoft Edge.
2. Go to https://web.esphome.io/
3. Connect the display with a USB data cable.
4. Click Connect.
5. Select the ESP32-S3 COM port.
6. Click Install and choose the merged dashboard .bin file.
7. Wait for the installation to finish before unplugging the display.


## First-Time Wi-Fi Configuration

Use this procedure after installing the firmware on a new display.

Before You Begin

Have the following information available:

1. Your home Wi-Fi network name
2. Your Wi-Fi password
3. Your Bambu Lab P1S printer’s IP address
   
   Finding the Printer IP Address
   The printer IP address can be found on the Bambu Lab P1S touchscreen:
   Open the printer’s settings. Open the network or LAN information page. Locate the printer’s IP address.
   The display and printer must normally be connected to the same local network.
   
5. A phone, tablet, or computer with Wi-Fi

The ESP32-S3 display connects to 2.4 GHz Wi-Fi. Make sure your network provides a 2.4 GHz connection.

1. Start Wi-Fi Setup Mode
2. Power on the display.
3. Open the SYS page using the bottom navigation bar.
4. Tap WIFI SETUP.
   
The display will show:
WiFi Setup Mode

Connect your phone to:
Bambu_P1S_Setup

Then open:
192.168.4.1

The display temporarily disconnects from your home network and creates its own Wi-Fi setup network.

2. Connect to the Display

On your phone, tablet, or computer:

Open the device’s Wi-Fi settings.
Select:
Bambu_P1S_Setup
Connect to the network.

The setup network does not require a password.

Your device may warn that the network has no internet connection. Choose the option to remain connected.

On some phones, temporarily turning off mobile data prevents the phone from leaving the setup network.

3. Open the Setup Page

Open Chrome, Edge, Safari, or another web browser and enter:

http://192.168.4.1

Be sure to include:

http://

The Bambu P1S Setup page should appear.

4. Enter the Wi-Fi Information

On the setup page:

Select your home Wi-Fi network from the WiFi Network list.
Enter the Wi-Fi password.
Enter the IP address of your Bambu Lab P1S printer.

Example:

192.168.1.159
Tap SAVE.

Wi-Fi passwords are case-sensitive. Enter uppercase letters, lowercase letters, numbers, and symbols exactly as they appear in the router settings.

5. Wait for the Display to Restart

After pressing SAVE, the page will show:

Settings Saved
Restarting...

The display will save the following information:

Wi-Fi network name
Wi-Fi password
Printer IP address

The display will then restart automatically.

6. Reconnect Your Phone or Computer

Because the temporary setup network shuts down during the restart:

Open your phone or computer’s Wi-Fi settings.
Reconnect to your normal home Wi-Fi network if it does not reconnect automatically.
7. Confirm the Connection

After the display restarts, open the SYS page.

Confirm that it shows:

Your Wi-Fi network name
A valid display IP address
Wi-Fi signal strength
MQTT status as connected

The dashboard may take several seconds to connect to the printer and receive its initial status.

Finding the Printer IP Address

The printer IP address can be found on the Bambu Lab P1S touchscreen:

Open the printer’s settings.
Open the network or LAN information page.
Locate the printer’s IP address.

The display and printer must normally be connected to the same local network.
