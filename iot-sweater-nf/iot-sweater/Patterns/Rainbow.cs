using NeoPixel;

using System;
using System.Threading;

namespace iot_sweater.Patterns
{
    class Rainbow : IPattern
    {
        private uint _ledCount;
        private uint _frameAdvance;
        private uint _pixelAdvance;
        private int _pauseIntervalms;
        private UInt32 _firstPixelHue;


        public Rainbow(uint ledCount, uint frameAdvance, uint pixelAdvance, int pauseIntervalms)
        {
            this._ledCount = ledCount;
            this._frameAdvance = frameAdvance;
            this._pixelAdvance = pixelAdvance;
            this._pauseIntervalms = pauseIntervalms;
            this._firstPixelHue = 0;
        }

        public void NextStep(NeopixelChain pixelChain)
        {
            UInt32 currentPixelHue = this._firstPixelHue;

            for (uint i = 0; i < this._ledCount; i++)
            {
                if (currentPixelHue >= (3 * 256))
                { // Normalize back down in case we incremented and overflowed
                    currentPixelHue -= (3 * 256);
                }

                byte phase = (byte)(currentPixelHue >> 8);
                byte step = (byte)(currentPixelHue & 0xffff);

                switch (phase)
                {
                    case 0:
                        pixelChain[i].R = (byte)~step;
                        pixelChain[i].G = step;
                        pixelChain[i].B = 0;
                        break;

                    case 1:
                        pixelChain[i].R = 0;
                        pixelChain[i].G = (byte)~step;
                        pixelChain[i].B = step;
                        break;

                    case 2:
                        pixelChain[i].R = step;
                        pixelChain[i].G = 0;
                        pixelChain[i].B = (byte)~step;
                        break;

                }
                currentPixelHue += currentPixelHue + this._pixelAdvance;
            }
            pixelChain.Update();
            this._firstPixelHue += this._frameAdvance;
            Thread.Sleep(this._pauseIntervalms);
        }
    }
}
