using System.Threading;

namespace iot_sweater.Patterns
{
    public class Theather : IPattern
    {
        private uint _ledCount;
        private byte _red;
        private byte _green;
        private byte _blue;
        private uint _spacing;
        private int _pauseInterval;
        private int _currentStep;

        public Theather(uint ledCount, byte red, byte green, byte blue, uint spacing, int pauseIntervalms)
        {
            this._ledCount = ledCount;
            this._red = red;
            this._blue = blue;
            this._green = green;
            this._spacing = spacing;
            this._pauseInterval = pauseIntervalms;
            this._currentStep = 0;
        }

        public void NextStep(NeoPixel.NeopixelChain pixelChain)
        {
            for (int q = 0; q < this._spacing; q++)
            {
                for (uint i = 0; i < this._ledCount; i++)
                {
                    if (this._currentStep == q)
                    {
                        pixelChain[i].R = this._red;
                        pixelChain[i].G = this._green;
                        pixelChain[i].B = this._blue;
                    }
                    else
                    {
                        pixelChain[i].R = 0;
                        pixelChain[i].G = 0;
                        pixelChain[i].B = 0;
                    }

                    this._currentStep++;
                    if (this._currentStep == this._spacing)
                    {
                        this._currentStep = 0;
                    }
                }
                pixelChain.Update();
                Thread.Sleep(this._pauseInterval);
            }
        }
    }
}
