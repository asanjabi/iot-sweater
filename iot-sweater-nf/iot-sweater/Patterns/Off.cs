using NeoPixel;

using System;
using System.Text;
using System.Threading;

namespace iot_sweater.Patterns
{
    public class Off : IPattern
    {
        private uint _ledCount;
        private int _pauseInterval;
        public Off(uint ledCount, int pauseIntervalms)
        {
            this._ledCount = ledCount;
            this._pauseInterval = pauseIntervalms;
        }
        public void NextStep(NeopixelChain pixelChain)
        {
           for(uint i =0; i < this._ledCount; i ++)
            {
                pixelChain[i].R = 0;
                pixelChain[i].G = 0;
                pixelChain[i].B = 0;
            }
            pixelChain.Update();
            Thread.Sleep(this._pauseInterval);
        }
    }
}
