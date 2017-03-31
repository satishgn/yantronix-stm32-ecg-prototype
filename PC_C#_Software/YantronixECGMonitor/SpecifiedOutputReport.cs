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
    public class SpecifiedOutputReport : OutputReport
    {
        public SpecifiedOutputReport(HIDDevice oDev) : base(oDev) {

        }

        public bool SendData(byte[] data)
        {
            byte[] arrBuff = Buffer; //new byte[Buffer.Length];
            for (int i = 0; i < arrBuff.Length; i++)
            {
                arrBuff[i] = data[i];
            }

            //Buffer = arrBuff;

            //returns false if the data does not fit in the buffer. else true
            if (arrBuff.Length < data.Length)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
}
