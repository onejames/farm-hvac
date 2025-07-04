# AWS IoT Core Setup Guide

This document provides a step-by-step guide to configure AWS IoT Core to receive data from the ESP32 HVAC Monitoring System.

## Prerequisites

*   An active AWS account.
*   The HVAC Monitor project code, specifically the `secrets.h` file.

---

### Step 1: Sign In and Navigate to AWS IoT Core

1.  Sign in to the AWS Management Console.
2.  In the top search bar, type `IoT Core` and select the service to open its dashboard.
3.  In the top-right corner, ensure you are in your preferred AWS Region (e.g., `us-east-1`, `us-west-2`).

---

### Step 2: Create a "Thing"

A "Thing" is the virtual representation of your physical ESP32 device in the AWS cloud.

1.  In the left-hand navigation pane, go to **Manage** > **All devices** > **Things**.
2.  Click the **Create things** button.
3.  Select **Create single thing** and click **Next**.
4.  For the **Thing name**, enter `ESP32_HVAC_MONITOR`. This must match the `THINGNAME` defined in your `src/secrets.h` file. Leave all other settings at their defaults.
5.  Click **Next**.
6.  On the "Configure device certificate" page, select **Auto-generate a new certificate (recommended)**.
7.  Click **Next**.

---

### Step 3: Create a Security Policy

A Policy defines what your Thing is allowed to do (e.g., connect to AWS, publish data to a specific topic).

1.  On the "Attach policies to certificate" page, you won't have a policy yet. Click the **Create policy** button. This will open a new browser tab.
2.  In the new tab, for the **Policy name**, enter `HVACMonitorPolicy`.
3.  In the "Policy document" section, switch to the **JSON** editor.
4.  Delete the existing content and paste the following JSON. This policy grants your device the minimum required permissions to connect and publish data to the `hvac/data` topic.

    ```json
    {
      "Version": "2012-10-17",
      "Statement": [
        {
          "Effect": "Allow",
          "Action": "iot:Connect",
          "Resource": "arn:aws:iot:*:*:client/ESP32_HVAC_MONITOR"
        },
        {
          "Effect": "Allow",
          "Action": "iot:Publish",
          "Resource": "arn:aws:iot:*:*:topic/hvac/data"
        }
      ]
    }
    ```
5.  Click the **Create** button at the bottom.

---

### Step 4: Attach Policy and Download Credentials

1.  Go back to the first browser tab where you were creating your Thing.
2.  You should still be on the "Attach policies to certificate" page. Click the refresh button next to the policy list.
3.  Select the new `HVACMonitorPolicy` you just created.
4.  Click the **Create thing** button.
5.  **This is the most important step.** You will now see a "Download certificates and keys" page. You must download these files now, as you will not be able to access the private key again.
    *   Download the **Device certificate**.
    *   Download the **Private key file**.
    *   Download the **Amazon Root CA 1**.
    *   Keep these files in a safe, private location.

---

### Step 5: Update `secrets.h`

1.  **Find Your AWS IoT Endpoint**: In the AWS IoT Core console, go to **Settings** in the left navigation pane. Copy the **Endpoint** URL. It will look something like `a1b2c3d4e5f6g7-ats.iot.us-east-1.amazonaws.com`.
2.  **Open `secrets.h`**: Open `/Users/jameslaster/Code/James/farm-hvac/src/secrets.h` in your code editor.
3.  **Paste the Endpoint**: Replace the placeholder value for `AWS_IOT_ENDPOINT` with the URL you just copied.
4.  **Copy Certificates**: Open the three certificate/key files you downloaded in a plain text editor.
    *   Copy the *entire content* of the **Device certificate** file (the one ending in `-certificate.pem.crt`) and paste it inside the `R"EOF(...)EOF"` block for `AWS_CERT_CRT`.
    *   Copy the *entire content* of the **Private key** file (the one ending in `-private.pem.key`) and paste it into the `AWS_CERT_PRIVATE` variable.
    *   Copy the *entire content* of the **Amazon Root CA 1** file (the one named `AmazonRootCA1.pem`) and paste it into the `AWS_CERT_CA` variable.

---

### Step 6: Test the Connection

1.  Fill in your Wi-Fi credentials in `secrets.h`.
2.  Build and upload the project to your ESP32 using PlatformIO.
3.  In the AWS IoT Core console, go to **Test** > **MQTT test client**.
4.  In the "Subscribe to a topic" tab, enter the topic `hvac/data` and click **Subscribe**.
5.  If everything is configured correctly, you will see the JSON data from your HVAC monitor appearing in the test client every 5 seconds.