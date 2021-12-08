using NeoPixel;

using System;
using System.Text;

namespace iot_sweater.Patterns
{
    public interface IPattern
    {
        void NextStep(NeopixelChain pixelChain);
    }
}
