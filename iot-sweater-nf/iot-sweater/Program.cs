using System;
using System.Diagnostics;
using System.Threading;

namespace iot_sweater
{
    public class Program
    {
        public static void Main()
        {
            Debug.WriteLine("Hello from nanoFramework!");

            Thread.Sleep(Timeout.Infinite);
        }
    }
}
