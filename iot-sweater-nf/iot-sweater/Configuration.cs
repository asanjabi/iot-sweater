﻿namespace iot_sweater
{
    public partial class Configuration
    {
        public const int LEDGpioPin = 33;
        public const uint LEDCount = 30;


        public const int NetworkConnectTimeout = 60000;
        public const int WifiConnectRetries = 10;

        public const int DpsRegistrationTimout = 30000;
        public const string DpsAddress = "global.azure-devices-provisioning.net"; // You can use as well your own address like yourdps.azure-devices-provisioning.net
    }
}

    //Create a new file in you project called Secrets.cs and paste this in there with your own values
    //Make sure not to check it in
    //
    //namespace iot_sweater
    //{
    //    public partial class Configuration
    //    {
    //        public const string DeviceID = "";
    //        public const string Devicekey = "";
    //        public const string IdScope = "";
    //    }
    //}
