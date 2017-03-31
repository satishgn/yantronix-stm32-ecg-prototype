/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Custom_HID;
using System.Windows.Forms.DataVisualization.Charting;

namespace YantronixECGMonitor
{
    public partial class YantronixForm : Form
    {
        //private memebers
        private int product_id;
        private int vendor_id;
        private Guid device_class;
        private IntPtr usb_event_handle;
        private SpecifiedDevice specified_device;
        private IntPtr handle;

        private byte[] sendData;

        private bool Is12LeadECG = false;    //All 12 leads

        private ushort ecg_lead = 0;
        private ushort adc_vref_mv = 3300;
        private ushort adc_resolution_bits = 15;
        private ushort adc_sampling_frequency = 500;
        private ushort adc_max_data = 32767;            //(2^adc_resolution_bits - 1)

        private ushort selected_ecg_lead = 0;
        private double ecg_lead_samples_max_time = 5;       //Max 5 seconds data
        private double ecg_lead_samples_pos_time = 1.25;    //reject 1.25 second data
        private double ecg_lead_samples_size_time = 2.5;    //display 2.5 seconds data
        private double ecg_lead_samples_settling_time = 0;
        private int ecg_lead_samples_settling = 0;          //ecg_lead_samples_settling_time * adc_sampling_frequency
        private int ecg_lead_samples_acquired = 0;          //acquired samples count

        private ushort hp_filter_type = 2;  //Default Bessel 0.5Hz HP filter
        private double[] xv_hp = new double[2];
        private double[] yv_hp = new double[2];

        private ushort lp_filter_type = 2;  //Default Butterworth 40Hz LP filter
        private ushort lp_filter_gain = 1;
        private double[] xv_lp = new double[9];
        private double[] yv_lp = new double[9];

        //Constant strings
        private const string device_connected = "YantronixECGMonitor [Connected]";
        private const string device_disconnected = "YantronixECGMonitor [Disconnected]";

        //events
        /// <summary>
        /// This event will be triggered when the device you specified is pluged into your usb port on
        /// the computer. And it is completly enumerated by windows and ready for use.
        /// </summary>
        public event EventHandler OnSpecifiedDeviceArrived;

        /// <summary>
        /// This event will be triggered when the device you specified is removed from your computer.
        /// </summary>
        public event EventHandler OnSpecifiedDeviceRemoved;

        /// <summary>
        /// This event will be triggered when a device is pluged into your usb port on
        /// the computer. And it is completly enumerated by windows and ready for use.
        /// </summary>
        public event EventHandler OnDeviceArrived;

        /// <summary>
        /// This event will be triggered when a device is removed from your computer.
        /// </summary>
        public event EventHandler OnDeviceRemoved;

        public YantronixForm()
        {
            //initializing in initial state
            this.product_id = 0x5750;   //Int32.Parse(this.tb_product.Text, System.Globalization.NumberStyles.HexNumber);
            this.vendor_id = 0x0483;    //Int32.Parse(this.tb_vendor.Text, System.Globalization.NumberStyles.HexNumber);
            this.specified_device = null;
            this.device_class = Win32Usb.HIDGuid;

            this.OnDeviceArrived += new EventHandler(this.usb_OnDeviceArrived);
            this.OnDeviceRemoved += new EventHandler(this.usb_OnDeviceRemoved);
            this.OnSpecifiedDeviceArrived += new EventHandler(this.usb_OnSpecifiedDeviceArrived);
            this.OnSpecifiedDeviceRemoved += new EventHandler(this.usb_OnSpecifiedDeviceRemoved);

            InitializeComponent();
            updateChartAreas();

            this.Text = device_disconnected;

            this.button6.Text = "TEST - ON";
            this.button7.Text = "ECG - ON";

            this.comboBox1.Items.Add("12 Leads");
            this.comboBox1.Items.Add("I");
            this.comboBox1.Items.Add("II");
            this.comboBox1.Items.Add("III");
            this.comboBox1.Items.Add("aVR");
            this.comboBox1.Items.Add("aVL");
            this.comboBox1.Items.Add("aVF");
            this.comboBox1.Items.Add("V1");
            this.comboBox1.Items.Add("V2");
            this.comboBox1.Items.Add("V3");
            this.comboBox1.Items.Add("V4");
            this.comboBox1.Items.Add("V5");
            this.comboBox1.Items.Add("V6");

            this.comboBox2.Items.Add("No Filter");
            this.comboBox2.Items.Add("0.05 Hz");
            this.comboBox2.Items.Add("0.5 Hz");
            this.comboBox2.Items.Add("1 Hz");
            this.comboBox2.Items.Add("1.5 Hz");
            this.comboBox2.Items.Add("2 Hz");
            this.comboBox2.Items.Add("2.5 Hz");
            this.comboBox2.Items.Add("5 Hz");
            this.comboBox2.Items.Add("10 Hz");
            this.comboBox2.Text = " 0.5 Hz ";

            this.comboBox3.Items.Add("No Filter");
            this.comboBox3.Items.Add("20 Hz");
            this.comboBox3.Items.Add("40 Hz");
            this.comboBox3.Items.Add("80 Hz");
            this.comboBox3.Items.Add("160 Hz");
            this.comboBox3.Text = " 40 Hz ";

            this.comboBox4.Items.Add("0");
            this.comboBox4.Items.Add("0.25");
            this.comboBox4.Items.Add("0.5");
            this.comboBox4.Items.Add("0.75");
            this.comboBox4.Items.Add("1");
            this.comboBox4.Items.Add("5");
            this.comboBox4.Items.Add("10");
            this.comboBox4.Items.Add("15");
            this.comboBox4.Items.Add("20");
            this.comboBox4.Items.Add("25");
            this.comboBox4.Items.Add("30");

            this.button6.Enabled = false;
            this.button7.Enabled = false;
            this.label3.Enabled = false;
            this.comboBox1.Enabled = false;
            this.comboBox4.Enabled = false;
        }

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
            RegisterHandle(Handle);
        }

        protected override void WndProc(ref Message m)
        {
            ParseMessages(ref m);
            base.WndProc(ref m);	// pass message on to base form
        }

        /// <summary>
        /// Registers this application, so it will be notified for usb events.  
        /// </summary>
        /// <param name="Handle">a IntPtr, that is a handle to the application.</param>
        public void RegisterHandle(IntPtr Handle)
        {
            usb_event_handle = Win32Usb.RegisterForUsbEvents(Handle, device_class);
            this.handle = Handle;
            //Check if the device is already present.
            CheckDevicePresent();
        }

        /// <summary>
        /// Unregisters this application, so it won't be notified for usb events.  
        /// </summary>
        /// <returns>Returns if it wass succesfull to unregister.</returns>
        public bool UnregisterHandle()
        {
            if (this.handle != null)
            {
                return Win32Usb.UnregisterForUsbEvents(this.handle);
            }

            return false;
        }

        /// <summary>
        /// This method will filter the messages that are passed for usb device change messages only. 
        /// And parse them and take the appropriate action 
        /// </summary>
        /// <param name="m">a ref to Messages, The messages that are thrown by windows to the application.</param>
        public void ParseMessages(ref Message m)
        {
            if (m.Msg == Win32Usb.WM_DEVICECHANGE)	// we got a device change message! A USB device was inserted or removed
            {
                switch (m.WParam.ToInt32())	// Check the W parameter to see if a device was inserted or removed
                {
                    case Win32Usb.DEVICE_ARRIVAL:	// inserted
                        if (OnDeviceArrived != null)
                        {
                            OnDeviceArrived(this, new EventArgs());
                            CheckDevicePresent();
                        }
                        break;
                    case Win32Usb.DEVICE_REMOVECOMPLETE:	// removed
                        if (OnDeviceRemoved != null)
                        {
                            OnDeviceRemoved(this, new EventArgs());
                            CheckDevicePresent();
                        }
                        break;
                }
            }
        }

        /// <summary>
        /// Checks the devices that are present at the moment and checks if one of those
        /// is the device you defined by filling in the product id and vendor id.
        /// </summary>
        public void CheckDevicePresent()
        {
            try
            {
                //Mind if the specified device existed before.
                bool history = false;
                if (specified_device != null)
                {
                    history = true;
                }

                specified_device = SpecifiedDevice.FindSpecifiedDevice(this.vendor_id, this.product_id);	// look for the device on the USB bus
                if (specified_device != null)	// did we find it?
                {
                    if (OnSpecifiedDeviceArrived != null)
                    {
                        this.OnSpecifiedDeviceArrived(this, new EventArgs());
                        specified_device.DataRecieved += new DataRecievedEventHandler(this.usb_OnDataRecieved);
                        specified_device.DataSend += new DataSendEventHandler(this.usb_OnDataSend);
                    }
                }
                else
                {
                    if (OnSpecifiedDeviceRemoved != null && history)
                    {
                        this.OnSpecifiedDeviceRemoved(this, new EventArgs());
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private void usb_OnDeviceArrived(object sender, EventArgs e)
        {
            //"Found a Device")
        }

        private void usb_OnDeviceRemoved(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                Invoke(new EventHandler(usb_OnDeviceRemoved), new object[] { sender, e });
            }
            else
            {
                //"Device was removed"
            }
        }

        private void usb_OnSpecifiedDeviceArrived(object sender, EventArgs e)
        {
            this.Text = device_connected;

            sendData = new byte[specified_device.OutputReportLength];

            this.button6.Enabled = true;
            this.button7.Enabled = true;
            this.label3.Enabled = true;

            this.comboBox1.Text = " 12 Leads ";
            this.comboBox1.Enabled = true;

            this.comboBox4.SelectedItem = ecg_lead_samples_settling_time.ToString();
            this.comboBox4.Enabled = true;
        }

        private void usb_OnSpecifiedDeviceRemoved(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                Invoke(new EventHandler(usb_OnSpecifiedDeviceRemoved), new object[] { sender, e });
            }
            else
            {
                this.Text = device_disconnected;

                this.button6.Text = "TEST - ON";
                this.button7.Text = "ECG - ON";

                this.button6.Enabled = false;
                this.button7.Enabled = false;
                this.label3.Enabled = false;

                this.comboBox1.Text = "";
                this.comboBox1.Enabled = false;

                this.comboBox4.Text = "";
                this.comboBox4.Enabled = false;
            }
        }

        private void usb_OnDataRecieved(object sender, DataRecievedEventArgs args)
        {
            if (InvokeRequired)
            {
                try
                {
                    Invoke(new DataRecievedEventHandler(usb_OnDataRecieved), new object[] { sender, args });
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                }
            }
            else
            {
                byte[] arrData = args.data;
                byte m_reportID = arrData[0];

                switch (m_reportID)
                {
                    case 2:
                        adc_vref_mv = (ushort)(arrData[2] + (arrData[3] << 8));
                        adc_resolution_bits = (ushort)(arrData[4] + (arrData[5] << 8));
                        adc_sampling_frequency = (ushort)(arrData[6] + (arrData[7] << 8));
                        ecg_lead = (ushort)(arrData[8] + (arrData[9] << 8));

                        adc_max_data = (ushort)(Math.Pow(2, adc_resolution_bits) - 1);

                        ecg_lead_samples_acquired = 0;

                        if (ecg_lead == 0)
                        {
                            Is12LeadECG = true;
                            for (int count = 1; count < chart1.Series.Count; count++)
                            {
                                chart1.Series["Series" + count].Points.Clear();
                                chart1.Series["Series" + count].Color = Color.Black;
                                chart1.Series["Series" + count].Points.Add(new DataPoint(0D, "0,0"));
                            }
                        }
                        else
                        {
                            chart1.Series["Series" + ecg_lead].Points.Clear();
                            chart1.Series["Series" + ecg_lead].Color = Color.Black;
                            chart1.Series["Series" + ecg_lead].Points.Add(new DataPoint(0D, "0,0"));

                            chart1.Series["LoadingSeries"].ChartArea = "ChartArea" + ecg_lead;
                            chart1.Series["Series" + ecg_lead].ChartArea = "InvisibleArea";
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Enable3D = true;
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Inclination = 0;
                        }

                        updateChartAreas();

                        chart1.Legends["Legend2"].CustomItems[2].Cells[1].Text = textBox3.Text;//Name
                        chart1.Legends["Legend2"].CustomItems[3].Cells[1].Text = "";//ID
                        chart1.Legends["Legend2"].CustomItems[4].Cells[1].Text = textBox4.Text;//Gender
                        chart1.Legends["Legend2"].CustomItems[5].Cells[1].Text = textBox5.Text;//Age
                        chart1.Legends["Legend2"].CustomItems[8].Cells[1].Text = "SGN-10X";//Device
                        chart1.Legends["Legend2"].CustomItems[9].Cells[1].Text = DateTime.Now.ToShortDateString();//Date
                        chart1.Legends["Legend2"].CustomItems[10].Cells[1].Text = DateTime.Now.ToShortTimeString();//Time
                        chart1.Legends["Legend2"].CustomItems[13].Cells[1].Text = "";//Heart Rate
                        chart1.Legends["Legend2"].CustomItems[14].Cells[1].Text = "";//P Duration
                        chart1.Legends["Legend2"].CustomItems[15].Cells[1].Text = "";//PR Interval
                        chart1.Legends["Legend2"].CustomItems[16].Cells[1].Text = "";//QRS Duration
                        chart1.Legends["Legend2"].CustomItems[17].Cells[1].Text = "";//QT Interval
                        break;

                    case 3:
                        if (ecg_lead != arrData[1])
                        {
                            ecg_lead = arrData[1];
                            ecg_lead_samples_acquired = 0;

                            chart1.Series["LoadingSeries"].ChartArea = "ChartArea" + ecg_lead;
                            chart1.Series["Series" + ecg_lead].ChartArea = "InvisibleArea";
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Enable3D = true;
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Inclination = 0;
                        }

                        if (ecg_lead_samples_acquired < ecg_lead_samples_settling)
                        {
                            ecg_lead_samples_acquired += 31;
                            break;
                        }

                        //int size = (arrData.Length) / 2;
                        ushort[] raw_data = new ushort[31];   //new int[size - 1];
                        int i = 2;  //first 2 bytes are report ID so exclude it
                        int j = 0;

                        chart1.Series.SuspendUpdates();

                        //while (i < arrData.Length)
                        while (i < 64)
                        {
                            raw_data[j] = (ushort)(arrData[i++] + (arrData[i++] << 8));

                            /*
                            if (chart1.Series["Series" + ecg_lead].Points.Count() == (chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.ScaleView.Position + chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.ScaleView.Size))
                            {
                                chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.ScaleView.Position = chart1.Series["Series" + ecg_lead].Points.Count();
                            }
                            else
                            {
                                chart1.Series["Series" + ecg_lead].Points.AddY(0, raw_data[j]);
                            }
                            */ 
                            
                            if (chart1.Series["Series" + ecg_lead].Points.Count() >= chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.Maximum)
                            {
                                chart_FilterSeries(ecg_lead, (ushort)chart1.Series["Series" + ecg_lead].Points.Count());
                                chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Enable3D = false;
                                chart1.Series["Series" + ecg_lead].ChartArea = "ChartArea" + ecg_lead;
                                chart1.Series["LoadingSeries"].ChartArea = "InvisibleArea";

                                if (!Is12LeadECG || ecg_lead == 12)
                                {
                                    sendData[0] = 26;
                                    sendData[1] = 0;
                                    button7.Text = "ECG - ON";
                                }
                                else
                                {
                                    sendData[0] = 25;
                                    sendData[2] = (byte)(ecg_lead + 1);
                                }

                                if (specified_device != null)
                                {
                                    specified_device.SendData(sendData);
                                }
                            }
                            else
                            {
                                chart1.Series["Series" + ecg_lead].Points.AddY(0, raw_data[j]);
                            }

                            j++;
                        }

                        if (chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Rotation < 180)
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Rotation += 10;
                        else
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Rotation = -180;

                        chart1.Series.ResumeUpdates();
                        break;

                    case 4:
                        if (ecg_lead != arrData[1])
                        {
                            ecg_lead = arrData[1];
                            ecg_lead_samples_acquired = 0;

                            chart1.Series["LoadingSeries"].ChartArea = "ChartArea" + ecg_lead;
                            chart1.Series["Series" + ecg_lead].ChartArea = "InvisibleArea";
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Enable3D = true;
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Inclination = 0;
                        }

                        if (ecg_lead_samples_acquired < ecg_lead_samples_settling)
                        {
                            ecg_lead_samples_acquired += 31;
                            break;
                        }

                        ushort[] iir_mk_filter_data = new ushort[31];
                        i = 2;  //first 2 bytes are report ID so exclude it
                        j = 0;

                        chart1.Series.SuspendUpdates();

                        while (i < 64)
                        {
                            iir_mk_filter_data[j] = (ushort)(arrData[i++] + (arrData[i++] << 8));
                            
                            /*
                            if (chart1.Series["Series" + ecg_lead].Points.Count() == (chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.ScaleView.Position + chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.ScaleView.Size))
                            {
                                chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.ScaleView.Position = chart1.Series["Series" + ecg_lead].Points.Count();
                            }
                            else
                            {
                                chart1.Series["Series" + ecg_lead].Points.AddY(0, iir_mk_filter_data[j]);
                            }
                            */

                            if (chart1.Series["Series" + ecg_lead].Points.Count() >= chart1.ChartAreas["ChartArea" + ecg_lead].AxisX.Maximum)
                            {
                                chart_FilterSeries(ecg_lead, (ushort)chart1.Series["Series" + ecg_lead].Points.Count());
                                chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Enable3D = false;
                                chart1.Series["Series" + ecg_lead].ChartArea = "ChartArea" + ecg_lead;
                                chart1.Series["LoadingSeries"].ChartArea = "InvisibleArea";

                                if (!Is12LeadECG || ecg_lead == 12)
                                {
                                    sendData[0] = 26;
                                    sendData[1] = 0;
                                    button7.Text = "ECG - ON";
                                }
                                else
                                {
                                    sendData[0] = 25;
                                    sendData[2] = (byte)(ecg_lead + 1);
                                }

                                if (specified_device != null)
                                {
                                    specified_device.SendData(sendData);
                                }
                            }
                            else
                            {
                                chart1.Series["Series" + ecg_lead].Points.AddY(0, iir_mk_filter_data[j]);
                            }

                            j++;
                        }

                        if (chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Rotation < 180)
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Rotation += 10;
                        else
                            chart1.ChartAreas["ChartArea" + ecg_lead].Area3DStyle.Rotation = -180;

                        chart1.Series.ResumeUpdates();
                        break;
                }
            }
        }

        private void usb_OnDataSend(object sender, DataSendEventArgs e)
        {
            //"Output report sent successfully."
        }

        private void updateChartAreas()
        {
            for (int count = 1; count < chart1.ChartAreas.Count; count++)
            {
                chart1.ChartAreas["ChartArea" + count].AxisX.Interval = adc_sampling_frequency;
                chart1.ChartAreas["ChartArea" + count].AxisX.LabelStyle.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisX.LineColor = System.Drawing.Color.Lime;
                chart1.ChartAreas["ChartArea" + count].AxisX.LineWidth = 2;
                /* One large 5mm x 5mm box represents 0.2 seconds(200 ms) time */
                chart1.ChartAreas["ChartArea" + count].AxisX.MajorGrid.Interval = 0.2 * adc_sampling_frequency;
                chart1.ChartAreas["ChartArea" + count].AxisX.MajorGrid.LineColor = System.Drawing.Color.Lime;
                chart1.ChartAreas["ChartArea" + count].AxisX.MajorGrid.LineWidth = 2;
                chart1.ChartAreas["ChartArea" + count].AxisX.MajorTickMark.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisX.Maximum = ecg_lead_samples_max_time * adc_sampling_frequency;
                chart1.ChartAreas["ChartArea" + count].AxisX.Minimum = 0;
                chart1.ChartAreas["ChartArea" + count].AxisX.MinorGrid.Enabled = true;
                /* One small 1mm x 1mm block represents 0.04 seconds(40 ms) time */
                chart1.ChartAreas["ChartArea" + count].AxisX.MinorGrid.Interval = 0.04 * adc_sampling_frequency;
                chart1.ChartAreas["ChartArea" + count].AxisX.MinorGrid.LineColor = System.Drawing.Color.Lime;
                chart1.ChartAreas["ChartArea" + count].AxisX.ScaleView.Position = ecg_lead_samples_pos_time * adc_sampling_frequency;
                chart1.ChartAreas["ChartArea" + count].AxisX.ScaleView.Size = ecg_lead_samples_size_time * adc_sampling_frequency;
                chart1.ChartAreas["ChartArea" + count].AxisX.ScaleView.Zoomable = false;
                chart1.ChartAreas["ChartArea" + count].AxisX.ScrollBar.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisX2.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
                chart1.ChartAreas["ChartArea" + count].AxisX2.LabelStyle.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisX2.MajorGrid.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisX2.MajorTickMark.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisX2.ScrollBar.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisY.Interval = 1000 * adc_max_data / adc_vref_mv;
                chart1.ChartAreas["ChartArea" + count].AxisY.LabelStyle.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisY.LineColor = System.Drawing.Color.Lime;
                chart1.ChartAreas["ChartArea" + count].AxisY.LineWidth = 2;
                /* One large 5mm x 5mm box represents 0.5 mV amplitude */
                chart1.ChartAreas["ChartArea" + count].AxisY.MajorGrid.Interval = 500 * adc_max_data / adc_vref_mv;
                chart1.ChartAreas["ChartArea" + count].AxisY.MajorGrid.LineColor = System.Drawing.Color.Lime;
                chart1.ChartAreas["ChartArea" + count].AxisY.MajorGrid.LineWidth = 2;
                chart1.ChartAreas["ChartArea" + count].AxisY.MajorTickMark.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisY.Maximum = adc_max_data;
                chart1.ChartAreas["ChartArea" + count].AxisY.Minimum = 0;
                chart1.ChartAreas["ChartArea" + count].AxisY.MinorGrid.Enabled = true;
                /* One small 1mm x 1mm block represents 0.1 mV amplitude */
                chart1.ChartAreas["ChartArea" + count].AxisY.MinorGrid.Interval = 100 * adc_max_data / adc_vref_mv;
                chart1.ChartAreas["ChartArea" + count].AxisY.MinorGrid.LineColor = System.Drawing.Color.Lime;
                chart1.ChartAreas["ChartArea" + count].AxisY.ScaleView.Position = 0;
                chart1.ChartAreas["ChartArea" + count].AxisY.ScaleView.Size = adc_max_data;
                chart1.ChartAreas["ChartArea" + count].AxisY.ScaleView.Zoomable = false;
                chart1.ChartAreas["ChartArea" + count].AxisY.ScrollBar.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisY2.Enabled = System.Windows.Forms.DataVisualization.Charting.AxisEnabled.False;
                chart1.ChartAreas["ChartArea" + count].AxisY2.LabelStyle.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisY2.MajorGrid.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisY2.MajorTickMark.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].AxisY2.ScrollBar.Enabled = false;
                chart1.ChartAreas["ChartArea" + count].CursorX.Position = double.NaN;
                chart1.ChartAreas["ChartArea" + count].CursorX.IsUserEnabled = true;
                chart1.ChartAreas["ChartArea" + count].CursorX.IsUserSelectionEnabled = true;
                chart1.ChartAreas["ChartArea" + count].CursorX.SelectionColor = System.Drawing.Color.DarkGray;
                chart1.ChartAreas["ChartArea" + count].CursorX.SelectionEnd = 0;
                chart1.ChartAreas["ChartArea" + count].CursorX.SelectionStart = 0;
                chart1.ChartAreas["ChartArea" + count].CursorY.Position = double.NaN;
                chart1.ChartAreas["ChartArea" + count].CursorY.IsUserEnabled = true;
                chart1.ChartAreas["ChartArea" + count].CursorY.IsUserSelectionEnabled = true;
                chart1.ChartAreas["ChartArea" + count].CursorY.SelectionColor = System.Drawing.Color.DarkGray;
                chart1.ChartAreas["ChartArea" + count].CursorY.SelectionEnd = 0;
                chart1.ChartAreas["ChartArea" + count].CursorY.SelectionStart = 0;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            openFileDialog1.Filter = "DATA (*.dat)|*.dat";
            openFileDialog1.FilterIndex = 1;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string fileName = openFileDialog1.FileName;
                FileStream fs = new FileStream(fileName, FileMode.Open, FileAccess.Read);
                BinaryReader br = new BinaryReader(fs);

                try
                {
                    ushort EcgLeadCount = 0;
                    ushort EcgLead = 0;
                    ushort EcgLeadDataCount = 0;
                    ushort EcgLeadData = 0;

                    //Console.WriteLine(br.BaseStream.Length);
                    //Console.WriteLine(br.BaseStream.Position);
                    /*
                    while (br.BaseStream.Position < br.BaseStream.Length)
                    {
                        ushort tmp = (ushort)br.ReadUInt16(); //Process tmp                      
                    }
                    */

                    adc_vref_mv = (ushort)br.ReadUInt16();
                    adc_resolution_bits = (ushort)br.ReadUInt16();
                    adc_sampling_frequency = (ushort)br.ReadUInt16();

                    adc_max_data = (ushort)(Math.Pow(2, adc_resolution_bits) - 1);

                    updateChartAreas();

                    textBox3.Text = "";
                    textBox4.Text = "";
                    textBox5.Text = "";

                    for (int count = 1; count < chart1.Series.Count; count++)
                    {
                        chart1.Series["Series" + count].Points.Clear();
                        chart1.Series["Series" + count].Color = Color.Black;
                    }

                    EcgLeadCount = (ushort)br.ReadUInt16();

                    for (int count = 0; count < EcgLeadCount; count++)
                    {
                        EcgLead = (ushort)br.ReadUInt16();
                        EcgLeadDataCount = (ushort)br.ReadUInt16();
                        for (int i = 0; i < EcgLeadDataCount; i++)
                        {
                            EcgLeadData = (ushort)br.ReadUInt16();
                            chart1.Series["Series" + EcgLead].Points.AddY(0, EcgLeadData);
                        }
                        chart_FilterSeries(EcgLead, EcgLeadDataCount);
                    }

                    chart1.Legends["Legend2"].CustomItems[2].Cells[1].Text = br.ReadString();//Name
                    chart1.Legends["Legend2"].CustomItems[3].Cells[1].Text = br.ReadString();//ID
                    chart1.Legends["Legend2"].CustomItems[4].Cells[1].Text = br.ReadString();//Gender
                    chart1.Legends["Legend2"].CustomItems[5].Cells[1].Text = br.ReadString();//Age
                    chart1.Legends["Legend2"].CustomItems[8].Cells[1].Text = br.ReadString();//Device
                    chart1.Legends["Legend2"].CustomItems[9].Cells[1].Text = br.ReadString();//Date
                    chart1.Legends["Legend2"].CustomItems[10].Cells[1].Text = br.ReadString();//Time
                    chart1.Legends["Legend2"].CustomItems[13].Cells[1].Text = br.ReadString();//Heart Rate
                    chart1.Legends["Legend2"].CustomItems[14].Cells[1].Text = br.ReadString();//P Duration
                    chart1.Legends["Legend2"].CustomItems[15].Cells[1].Text = br.ReadString();//PR Interval
                    chart1.Legends["Legend2"].CustomItems[16].Cells[1].Text = br.ReadString();//QRS Duration
                    chart1.Legends["Legend2"].CustomItems[17].Cells[1].Text = br.ReadString();//QT Interval
                }
                // If the end of the stream is reached before reading
                // the data values, ignore the error and use the
                // default settings for the remaining values.
                catch (EndOfStreamException ex)
                {
                    //MessageBox.Show("Error : " + ex.GetType().Name);
                }
                catch (Exception ex)
                {
                    //MessageBox.Show("Error : " + ex.GetType().Name);
                }
                finally
                {
                    br.Close();
                }

                fs.Close();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            // Create a new save file dialog
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();

            // Sets the current file name filter string, which determines 
            // the choices that appear in the "Save as file type" or 
            // "Files of type" box in the dialog box.
            saveFileDialog1.Filter = "Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg|EMF (*.emf)|*.emf|PNG (*.png)|*.png|GIF (*.gif)|*.gif|TIFF (*.tif)|*.tif";
            saveFileDialog1.FilterIndex = 2;
            saveFileDialog1.RestoreDirectory = true;

            // Set image file format
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                ChartImageFormat format = ChartImageFormat.Bmp;

                if (saveFileDialog1.FileName.EndsWith("bmp"))
                {
                    format = ChartImageFormat.Bmp;
                }
                else if (saveFileDialog1.FileName.EndsWith("jpg"))
                {
                    format = ChartImageFormat.Jpeg;
                }
                else if (saveFileDialog1.FileName.EndsWith("emf"))
                {
                    format = ChartImageFormat.Emf;
                }
                else if (saveFileDialog1.FileName.EndsWith("gif"))
                {
                    format = ChartImageFormat.Gif;
                }
                else if (saveFileDialog1.FileName.EndsWith("png"))
                {
                    format = ChartImageFormat.Png;
                }
                else if (saveFileDialog1.FileName.EndsWith("tif"))
                {
                    format = ChartImageFormat.Tiff;
                }

                // Save image
                chart1.SaveImage(saveFileDialog1.FileName, format);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                chart1.Printing.PageSetup();
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Chart Control for .NET Framework", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            try
            {
                chart1.Printing.PrintPreview();
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Chart Control for .NET Framework", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            try
            {
                chart1.Printing.Print(true);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, "Chart Control for .NET Framework", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            try
            {
                if (sender == button6)
                {
                    sendData[0] = 27;
                    if (button6.Text.Equals("TEST - OFF"))
                    {
                        sendData[1] = 0;
                        button6.Text = "TEST - ON";
                        this.button7.Enabled = true;
                        this.label3.Enabled = true;
                        this.comboBox1.Enabled = true;
                    }
                    else
                    {
                        sendData[1] = 1;
                        button6.Text = "TEST - OFF";
                        this.button7.Enabled = false;
                        this.label3.Enabled = false;
                        this.comboBox1.Enabled = false;
                    }
                }

                if (specified_device != null)
                {
                    specified_device.SendData(sendData);
                }
                else
                {
                    MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            try
            {
                if (sender == button7)
                {
                    sendData[0] = 26;
                    if (button7.Text.Equals("ECG - OFF"))
                    {
                        sendData[1] = 0;
                        button7.Text = "ECG - ON";
                        this.button6.Enabled = true;
                    }
                    else
                    {
                        Is12LeadECG = false;
                        sendData[1] = 1;
                        sendData[2] = (byte)selected_ecg_lead;
                        button7.Text = "ECG - OFF";
                        this.button6.Enabled = false;
                    }
                }

                if (specified_device != null)
                {
                    specified_device.SendData(sendData);
                }
                else
                {
                    MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void button8_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog2 = new SaveFileDialog();

            saveFileDialog2.Filter = "DATA (*.dat)|*.dat";
            saveFileDialog2.FilterIndex = 1;
            saveFileDialog2.RestoreDirectory = true;

            if (saveFileDialog2.ShowDialog() == DialogResult.OK)
            {
                string fileName = saveFileDialog2.FileName;
                FileStream fs = new FileStream(fileName, FileMode.Create, FileAccess.Write);
                BinaryWriter bw = new BinaryWriter(fs);

                try
                {
                    ushort EcgLeadCount = 0;
                    ushort EcgLead = 0;
                    ushort EcgLeadDataCount = 0;
                    ushort EcgLeadData = 0;

                    bw.Write(adc_vref_mv);
                    bw.Write(adc_resolution_bits);
                    bw.Write(adc_sampling_frequency);

                    EcgLeadCount = (ushort)(chart1.Series.Count - 1);
                    bw.Write(EcgLeadCount);

                    for (EcgLead = 1; EcgLead < EcgLeadCount + 1; EcgLead++)
                    {
                        bw.Write(EcgLead);
                        EcgLeadDataCount = (ushort)chart1.Series["Series" + EcgLead].Points.Count();
                        bw.Write(EcgLeadDataCount);
                        for (int i = 0; i < EcgLeadDataCount; i++)
                        {
                            EcgLeadData = (ushort)chart1.Series["Series" + EcgLead].Points[i].YValues[1];
                            bw.Write(EcgLeadData);
                        }
                    }

                    bw.Write(chart1.Legends["Legend2"].CustomItems[2].Cells[1].Text);//Name
                    bw.Write(chart1.Legends["Legend2"].CustomItems[3].Cells[1].Text);//ID
                    bw.Write(chart1.Legends["Legend2"].CustomItems[4].Cells[1].Text);//Gender
                    bw.Write(chart1.Legends["Legend2"].CustomItems[5].Cells[1].Text);//Age
                    bw.Write(chart1.Legends["Legend2"].CustomItems[8].Cells[1].Text);//Device
                    bw.Write(chart1.Legends["Legend2"].CustomItems[9].Cells[1].Text);//Date
                    bw.Write(chart1.Legends["Legend2"].CustomItems[10].Cells[1].Text);//Time
                    bw.Write(chart1.Legends["Legend2"].CustomItems[13].Cells[1].Text);//Heart Rate
                    bw.Write(chart1.Legends["Legend2"].CustomItems[14].Cells[1].Text);//P Duration
                    bw.Write(chart1.Legends["Legend2"].CustomItems[15].Cells[1].Text);//PR Interval
                    bw.Write(chart1.Legends["Legend2"].CustomItems[16].Cells[1].Text);//QRS Duration
                    bw.Write(chart1.Legends["Legend2"].CustomItems[17].Cells[1].Text);//QT Interval
                }
                catch (Exception ex)
                {
                    //MessageBox.Show("Error : " + ex.GetType().Name);
                }
                finally
                {
                    bw.Close();
                }

                fs.Close();
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (sender == comboBox1)
                {
                    selected_ecg_lead = (ushort)Convert.ToUInt16(this.comboBox1.SelectedIndex);
                    Is12LeadECG = false;
                    sendData[0] = 25;
                    sendData[2] = (byte)selected_ecg_lead;
                }

                if (specified_device != null)
                {
                    specified_device.SendData(sendData);
                }
                else
                {
                    MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                }
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString());
            }
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (sender == comboBox2)
                {
                    hp_filter_type = (ushort)Convert.ToUInt16(this.comboBox2.SelectedIndex);

                    ushort EcgLead = 0;
                    ushort EcgLeadDataCount = 0;

                    for (EcgLead = 1; EcgLead < chart1.Series.Count; EcgLead++)
                    {
                        EcgLeadDataCount = (ushort)chart1.Series["Series" + EcgLead].Points.Count();
                        chart_FilterSeries(EcgLead, EcgLeadDataCount);
                    }

                    chart1.Series.Invalidate();
                }
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString());
            }
        }

        private void comboBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (sender == comboBox3)
                {
                    lp_filter_type = (ushort)Convert.ToUInt16(this.comboBox3.SelectedIndex);

                    ushort EcgLead = 0;
                    ushort EcgLeadDataCount = 0;

                    for (EcgLead = 1; EcgLead < chart1.Series.Count; EcgLead++)
                    {
                        EcgLeadDataCount = (ushort)chart1.Series["Series" + EcgLead].Points.Count();
                        chart_FilterSeries(EcgLead, EcgLeadDataCount);
                    }

                    chart1.Series.Invalidate();
                }
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString());
            }
        }

        private void comboBox4_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (sender == comboBox4)
                {
                    ecg_lead_samples_settling_time = Convert.ToDouble(this.comboBox4.SelectedItem);
                    ecg_lead_samples_settling = (int)ecg_lead_samples_settling_time * adc_sampling_frequency;
                }
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString());
            }
        }

        private void chart_FilterSeries(ushort EcgLead, ushort EcgLeadDataCount)
        {
            int i = 0;
            double EcgLeadData = 0;

            xv_hp[0] = xv_hp[1] = adc_max_data / 2;
            yv_hp[0] = yv_hp[1] = 0;   
            for (i = 0; i < EcgLeadDataCount; i++)
            {
                EcgLeadData = chart1.Series["Series" + EcgLead].Points[i].YValues[1];
                chart1.Series["Series" + EcgLead].Points[i].YValues[0] = hp_Filter(EcgLeadData);
            }

            xv_hp[0] = xv_hp[1] = 0;
            yv_hp[0] = yv_hp[1] = 0;
            for (i = EcgLeadDataCount - 1; i >= 0; i--)
            {
                EcgLeadData = chart1.Series["Series" + EcgLead].Points[i].YValues[0];
                chart1.Series["Series" + EcgLead].Points[i].YValues[0] = hp_Filter(EcgLeadData);
            }

            if (EcgLead >= 7 && EcgLead <= 12)
                lp_filter_gain = 100;
            else
                lp_filter_gain = 200;

            xv_lp[0] = xv_lp[1] = xv_lp[2] = xv_lp[3] = xv_lp[4] = xv_lp[5] = xv_lp[6] = xv_lp[7] = xv_lp[8] = 0;
            yv_lp[0] = yv_lp[1] = yv_lp[2] = yv_lp[3] = yv_lp[4] = yv_lp[5] = yv_lp[6] = yv_lp[7] = yv_lp[8] = 0;
            for (i = 0; i < EcgLeadDataCount; i++)
            {
                EcgLeadData = chart1.Series["Series" + EcgLead].Points[i].YValues[0];
                chart1.Series["Series" + EcgLead].Points[i].YValues[0] = adc_max_data / 2 + lp_filter_gain * lp_Filter(EcgLeadData);
                //chart1.Series["Series" + EcgLead].Points[i].YValues[0] = adc_max_data / 2 - lp_filter_gain * lp_Filter(EcgLeadData);
            }
        }

        /* 1st order Bessel HP filter, Fs=500Hz */
        private double hp_Filter(double data)
        {
            switch (hp_filter_type)
            {
                case 0:
                    //Don't filter
                    yv_hp[1] = data;
                    break;

                case 1:
                    //0.05Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.000314159e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.9993718788 * yv_hp[0]);
                    break;

                case 2:
                    //0.5Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.003141582e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.9937365126 * yv_hp[0]);
                    break;

                case 3:
                    //1Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.006283103e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.9875122565 * yv_hp[0]);
                    break;

                case 4:
                    //1.5Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.009424499e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.9813269860 * yv_hp[0]);
                    break;

                case 5:
                    //2Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.012565709e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.9751804568 * yv_hp[0]);
                    break;

                case 6:
                    //2.5Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.015706672e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.9690724263 * yv_hp[0]);
                    break;

                case 7:
                    //5Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.031405595e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.9391013674 * yv_hp[0]);
                    break;

                case 8:
                    //10Hz
                    xv_hp[0] = xv_hp[1];
                    xv_hp[1] = data / 1.062749300e+00;
                    yv_hp[0] = yv_hp[1];
                    yv_hp[1] = (xv_hp[1] - xv_hp[0]) + (0.8819113783 * yv_hp[0]);
                    break;
            }

            return yv_hp[1];
        }

        /* 8th order Butterworth LP filter, Fs=500Hz */
        private double lp_Filter(double data)
        {
            switch (lp_filter_type)
            {
                case 0:
                    //Don't filter
                    yv_lp[8] = data;
                    break;

                case 1:
                    //20Hz
                    xv_lp[0] = xv_lp[1]; xv_lp[1] = xv_lp[2]; xv_lp[2] = xv_lp[3]; xv_lp[3] = xv_lp[4]; xv_lp[4] = xv_lp[5]; xv_lp[5] = xv_lp[6]; xv_lp[6] = xv_lp[7]; xv_lp[7] = xv_lp[8];
                    xv_lp[8] = data / 2.942168107e+07;
                    yv_lp[0] = yv_lp[1]; yv_lp[1] = yv_lp[2]; yv_lp[2] = yv_lp[3]; yv_lp[3] = yv_lp[4]; yv_lp[4] = yv_lp[5]; yv_lp[5] = yv_lp[6]; yv_lp[6] = yv_lp[7]; yv_lp[7] = yv_lp[8];
                    yv_lp[8] = (xv_lp[0] + xv_lp[8]) + 8 * (xv_lp[1] + xv_lp[7]) + 28 * (xv_lp[2] + xv_lp[6])
                                 + 56 * (xv_lp[3] + xv_lp[5]) + 70 * xv_lp[4]
                                 + (-0.2745334599 * yv_lp[0]) + (2.5498405963 * yv_lp[1])
                                 + (-10.3935331060 * yv_lp[2]) + (24.2886977590 * yv_lp[3])
                                 + (-35.5989372960 * yv_lp[4]) + (33.5161659990 * yv_lp[5])
                                 + (-19.7998045540 * yv_lp[6]) + (6.7120953600 * yv_lp[7]);
                    break;

                case 2:
                    //40Hz
                    xv_lp[0] = xv_lp[1]; xv_lp[1] = xv_lp[2]; xv_lp[2] = xv_lp[3]; xv_lp[3] = xv_lp[4]; xv_lp[4] = xv_lp[5]; xv_lp[5] = xv_lp[6]; xv_lp[6] = xv_lp[7]; xv_lp[7] = xv_lp[8];
                    xv_lp[8] = data / 1.954800496e+05;
                    yv_lp[0] = yv_lp[1]; yv_lp[1] = yv_lp[2]; yv_lp[2] = yv_lp[3]; yv_lp[3] = yv_lp[4]; yv_lp[4] = yv_lp[5]; yv_lp[5] = yv_lp[6]; yv_lp[6] = yv_lp[7]; yv_lp[7] = yv_lp[8];
                    yv_lp[8] = (xv_lp[0] + xv_lp[8]) + 8 * (xv_lp[1] + xv_lp[7]) + 28 * (xv_lp[2] + xv_lp[6])
                                 + 56 * (xv_lp[3] + xv_lp[5]) + 70 * xv_lp[4]
                                 + (-0.0733593960 * yv_lp[0]) + (0.7756867958 * yv_lp[1])
                                 + (-3.6269383559 * yv_lp[2]) + (9.8053520671 * yv_lp[3])
                                 + (-16.7853675510 * yv_lp[4]) + (18.6608184850 * yv_lp[5])
                                 + (-13.1837123410 * yv_lp[6]) + (5.4262106993 * yv_lp[7]);
                    break;

                case 3:
                    //80Hz
                    xv_lp[0] = xv_lp[1]; xv_lp[1] = xv_lp[2]; xv_lp[2] = xv_lp[3]; xv_lp[3] = xv_lp[4]; xv_lp[4] = xv_lp[5]; xv_lp[5] = xv_lp[6]; xv_lp[6] = xv_lp[7]; xv_lp[7] = xv_lp[8];
                    xv_lp[8] = data / 1.835002963e+03;
                    yv_lp[0] = yv_lp[1]; yv_lp[1] = yv_lp[2]; yv_lp[2] = yv_lp[3]; yv_lp[3] = yv_lp[4]; yv_lp[4] = yv_lp[5]; yv_lp[5] = yv_lp[6]; yv_lp[6] = yv_lp[7]; yv_lp[7] = yv_lp[8];
                    yv_lp[8] = (xv_lp[0] + xv_lp[8]) + 8 * (xv_lp[1] + xv_lp[7]) + 28 * (xv_lp[2] + xv_lp[6])
                                 + 56 * (xv_lp[3] + xv_lp[5]) + 70 * xv_lp[4]
                                 + (-0.0042663264 * yv_lp[0]) + (0.0560269539 * yv_lp[1])
                                 + (-0.3326748641 * yv_lp[2]) + (1.1714965450 * yv_lp[3])
                                 + (-2.6995850628 * yv_lp[4]) + (4.1976145283 * yv_lp[5])
                                 + (-4.3935290292 * yv_lp[6]) + (2.8654079437 * yv_lp[7]);
                    break;

                case 4:
                    //160Hz
                    xv_lp[0] = xv_lp[1]; xv_lp[1] = xv_lp[2]; xv_lp[2] = xv_lp[3]; xv_lp[3] = xv_lp[4]; xv_lp[4] = xv_lp[5]; xv_lp[5] = xv_lp[6]; xv_lp[6] = xv_lp[7]; xv_lp[7] = xv_lp[8];
                    xv_lp[8] = data / 2.263981421e+01;
                    yv_lp[0] = yv_lp[1]; yv_lp[1] = yv_lp[2]; yv_lp[2] = yv_lp[3]; yv_lp[3] = yv_lp[4]; yv_lp[4] = yv_lp[5]; yv_lp[5] = yv_lp[6]; yv_lp[6] = yv_lp[7]; yv_lp[7] = yv_lp[8]; 
                    yv_lp[8] =   (xv_lp[0] + xv_lp[8]) + 8 * (xv_lp[1] + xv_lp[7]) + 28 * (xv_lp[2] + xv_lp[6])
                                 + 56 * (xv_lp[3] + xv_lp[5]) + 70 * xv_lp[4]
                                 + ( -0.0019523370 * yv_lp[0]) + ( -0.0268451387 * yv_lp[1])
                                 + ( -0.1684851815 * yv_lp[2]) + ( -0.6289286398 * yv_lp[3])
                                 + ( -1.5614713525 * yv_lp[4]) + ( -2.6216302930 * yv_lp[5])
                                 + ( -3.0705930906 * yv_lp[6]) + ( -2.2276072549 * yv_lp[7]);
                    break;
            }

            return yv_lp[8];
        }

        private void chart1_CursorPositionChanging(object sender, CursorEventArgs e)
        {
            if (e.Axis.AxisName == AxisName.X)
            {
                textBox1.Text = Math.Round((Math.Abs(e.ChartArea.CursorX.SelectionEnd - e.ChartArea.CursorX.SelectionStart) / adc_sampling_frequency), 3).ToString();//sec
                //textBox1.Text = (adc_sampling_frequency * 60 / Math.Abs(e.ChartArea.CursorX.SelectionEnd - e.ChartArea.CursorX.SelectionStart)).ToString();//BPM
            }
            else
            {
                textBox2.Text = Math.Round((Math.Abs(e.ChartArea.CursorY.SelectionEnd - e.ChartArea.CursorY.SelectionStart) * adc_vref_mv / adc_max_data / 1000), 1).ToString();//mV
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (button7.Text.Equals("ECG - OFF"))
                {
                    sendData[0] = 26;
                    sendData[1] = 0;
                }
                else if (button6.Text.Equals("TEST - OFF"))
                {
                    sendData[0] = 27;
                    sendData[1] = 0;
                }
                else
                {
                    return;
                }

                if (specified_device != null)
                {
                    specified_device.SendData(sendData);
                }
                else
                {
                    MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {
            chart1.Legends["Legend2"].CustomItems[2].Cells[1].Text = textBox3.Text;
        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {
            chart1.Legends["Legend2"].CustomItems[4].Cells[1].Text = textBox4.Text;
        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {
            chart1.Legends["Legend2"].CustomItems[5].Cells[1].Text = textBox5.Text;
        }
    }
}
