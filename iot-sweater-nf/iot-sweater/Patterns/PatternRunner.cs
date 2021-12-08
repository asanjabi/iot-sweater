
using NeoPixel;

using System;
using System.Diagnostics;
using System.Threading;

using GC = nanoFramework.Runtime.Native.GC;

namespace iot_sweater.Patterns
{
    public class PatternRunner
    {
        private int _gpioPin;
        private uint _ledCount;
        private IPattern _pattern;
        private NeopixelChain _neopixelChain;
        private bool _stop = false;

        public PatternRunner(int gpIoPin, uint ledCount, IPattern pattern)
        {
            this._gpioPin = gpIoPin;
            this._ledCount = ledCount;
            if (pattern == null)
            {
                throw new NullReferenceException("Pattern can not be null");
            }
            this._pattern = pattern;
            this.Setup();
        }

        public void Run()
        {
            this._pattern.NextStep(this._neopixelChain);
        }

        public void NewPattern(IPattern newPattern)
        {
            IPattern oldPattern = this._pattern;
            this._pattern = newPattern;

            IDisposable oldDisposable = oldPattern as IDisposable;
            if(oldDisposable != null)
            {
                oldDisposable.Dispose();
            }
            GC.Run(true);
        }
        
        private void Setup()
        {
            try
            {
                this._neopixelChain = new NeopixelChain(this._gpioPin, this._ledCount);
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Problem creating the chain {ex.Message}");
            }
        }
    }
}
