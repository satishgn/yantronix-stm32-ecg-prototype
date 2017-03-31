/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

using System;
using System.Collections.Generic;
using System.Text;

namespace Custom_HID
{
    public class SpecifiedInputReport : InputReport
    {
        private byte[] arrData;

        public SpecifiedInputReport(HIDDevice oDev) : base(oDev)
		{

		}

        public override void ProcessData()
        {
            this.arrData = Buffer;
        }

        public byte[] Data
        {
            get
            {
                return arrData;
            }
        }
    }
}
