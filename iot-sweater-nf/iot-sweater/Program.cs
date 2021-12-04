using nanoFramework.Azure.Devices.Client;
using nanoFramework.Azure.Devices.Provisioning.Client;
using nanoFramework.Networking;

using NeoPixel;

using System;
using System.Diagnostics;
using System.Threading;

using GC = nanoFramework.Runtime.Native.GC;

namespace iot_sweater
{

    public class Program
    {
        public static void Main()
        {
            Debug.WriteLine("Hello from nanoFramework!");
            Checkmemory("Start");

            //Make sure to add your Wifi settings using VS
            var connected = ConnectToWifi();
            Checkmemory("Network Connected");

            //Make sure your device information is correct and add Azure certificate using VS
            DeviceClient deviceClient = ConnectToIoTCentral();
            Checkmemory("Hub connected");

 
            var pixelChain = new NeopixelChain(Configuration.LEDGpioPin, Configuration.LEDCount);
            Checkmemory("Chain created");

            //Testing
            while (true)
            {
                for (uint i = 0; i < Configuration.LEDCount; i++)
                {
                    pixelChain[i] = RedColor;
                }
                pixelChain.Update();

                Thread.Sleep(2000);
                for (uint i = 0; i < Configuration.LEDCount; i++)
                {
                    pixelChain[i] = BlackColor;
                }
                pixelChain.Update();

                Checkmemory("loop done");
            }
            Thread.Sleep(Timeout.Infinite);
        }

        private static readonly Color RedColor = new Color { R = 255 };
        private static readonly Color BlackColor = new Color();

        private static string Off(int rid, string payload)
        {
            Debug.WriteLine($"Off: {rid} - {payload}");
            return string.Empty;
        }

        private static DeviceClient ConnectToIoTCentral()
        {
            var registrationResult = RegisterDevice();

            if (registrationResult.Status != ProvisioningRegistrationStatusType.Assigned)
            {
                Debug.WriteLine("Failed to register the device");
                Debug.WriteLine($"Status {registrationResult.Status.ToString()}");
                Debug.WriteLine($"SubStatus {registrationResult.Substatus.ToString()}");
                Debug.WriteLine($"Error message: {registrationResult.ErrorMessage}");
                return null;
            }

            var deviceClient = new DeviceClient(registrationResult.AssignedHub, registrationResult.DeviceId, Configuration.Devicekey);
            deviceClient.Open();
            return deviceClient;
        }

        private static DeviceRegistrationResult RegisterDevice()
        {
            DeviceRegistrationResult registrationResult = null;

            using (var provisioning = ProvisioningDeviceClient.Create(Configuration.DpsAddress, Configuration.IdScope, Configuration.DeviceID, Configuration.Devicekey))
            {
                try
                {
                    registrationResult = provisioning.Register(new CancellationTokenSource(Configuration.DpsRegistrationTimout).Token);
                }
                catch (Exception ex)
                {
                    Debug.WriteLine(ex.Message);
                    throw;
                }
            }

            Debug.WriteLine($"Device successfully assigned:");
            Debug.WriteLine($"  Assigned Hub: {registrationResult.AssignedHub}");
            Debug.WriteLine($"  Created time: {registrationResult.CreatedDateTimeUtc}");
            Debug.WriteLine($"  Device ID: {registrationResult.DeviceId}");
            Debug.WriteLine($"  Error code: {registrationResult.ErrorCode}");
            Debug.WriteLine($"  Error message: {registrationResult.ErrorMessage}");
            Debug.WriteLine($"  ETAG: {registrationResult.Etag}");
            Debug.WriteLine($"  Generation ID: {registrationResult.GenerationId}");
            Debug.WriteLine($"  Last update: {registrationResult.LastUpdatedDateTimeUtc}");
            Debug.WriteLine($"  Status: {registrationResult.Status}");
            Debug.WriteLine($"  Sub Status: {registrationResult.Substatus}");
            return registrationResult;
        }

        private static bool ConnectToWifi()
        {
            Debug.WriteLine("Connecting to WiFi.");

            bool success = false;
            int currentTry = 0;

            do
            {
                currentTry++;
                success = NetworkHelper.ReconnectWifi(true, token: new CancellationTokenSource(Configuration.NetworkConnectTimeout).Token);
                if (!success)
                {
                    Debug.WriteLine($"Can't connect to wifi: {NetworkHelper.ConnectionError.Error}");
                    if (NetworkHelper.ConnectionError.Exception != null)
                    {
                        Debug.WriteLine($"NetworkHelper.ConnectionError.Exception");
                    }
                }
            } while (!success && currentTry < Configuration.WifiConnectRetries);

            Debug.WriteLine($"{(success ? "Connected" : "Failed to connect")} to network.");
            Debug.WriteLine($"Date and time is now {DateTime.UtcNow}");

            return success;
        }

        private static uint lastMemory = 0;
        private static void Checkmemory(string note = "")
        {
            uint currentMemory = GC.Run(true);
            int change = (int)(currentMemory - lastMemory);
            lastMemory = currentMemory;
            Debug.WriteLine($">>> {note}: Free memory: {currentMemory}  Change: {change}");
        }
    }
}
