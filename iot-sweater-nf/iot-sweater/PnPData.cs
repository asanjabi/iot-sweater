using System;
using System.Text;

namespace iot_sweater
{
    public class PnpData
    {
        public PnpData(string modelId)
        {
            this.ModelId = modelId;
        }
        public string ModelId { get; private set; }
    }
}
