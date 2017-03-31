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

namespace Custom_HID
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

        /*
        private double[] xv = new double[9];
        private double[] yv = new double[9];
        private const double gain = 231.1047679;
        */

        /*************** Floating point Process ****************/
        /*
        //4th Order IIR Filter
        private double[] xv = new double[5];
        private double[] yv = new double[5];

        //Cascade of 2nd Order IIR filters
        //4 Low pass filters
        private double[] xv_lp1 = new double[3];
        private double[] yv_lp1 = new double[3];
        private double[] xv_lp2 = new double[3];
        private double[] yv_lp2 = new double[3];
        private double[] xv_lp3 = new double[3];
        private double[] yv_lp3 = new double[3];
        private double[] xv_lp4 = new double[3];
        private double[] yv_lp4 = new double[3];
        //4 High pass filters
        private double[] xv_hp1 = new double[3];
        private double[] yv_hp1 = new double[3];
        private double[] xv_hp2 = new double[3];
        private double[] yv_hp2 = new double[3];
        private double[] xv_hp3 = new double[3];
        private double[] yv_hp3 = new double[3];
        private double[] xv_hp4 = new double[3];
        private double[] yv_hp4 = new double[3];
        //6 Band pass filters
        private double[] xv_bp1 = new double[3];
        private double[] yv_bp1 = new double[3];
        private double[] xv_bp2 = new double[3];
        private double[] yv_bp2 = new double[3];
        private double[] xv_bp3 = new double[3];
        private double[] yv_bp3 = new double[3];
        private double[] xv_bp4 = new double[3];
        private double[] yv_bp4 = new double[3];
        private double[] xv_bp5 = new double[3];
        private double[] yv_bp5 = new double[3];
        private double[] xv_bp6 = new double[3];
        private double[] yv_bp6 = new double[3];
        //4 Band stop (Notch) filters
        private double[] xv_bs1 = new double[3];
        private double[] yv_bs1 = new double[3];
        private double[] xv_bs2 = new double[3];
        private double[] yv_bs2 = new double[3];
        private double[] xv_bs3 = new double[3];
        private double[] yv_bs3 = new double[3];
        private double[] xv_bs4 = new double[3];
        private double[] yv_bs4 = new double[3];
        */
        /*******************************************************/

        /*************** Fixed point Process ****************/
        /**/
        //4th Order IIR Filter
        private int[] xv = new int[5];
        private int[] yv = new int[5];

        //Cascade of 2nd Order IIR filters
        //4 Low pass filters
        private int[] xv_lp1 = new int[3];
        private int[] yv_lp1 = new int[3];
        private int[] xv_lp2 = new int[3];
        private int[] yv_lp2 = new int[3];
        private int[] xv_lp3 = new int[3];
        private int[] yv_lp3 = new int[3];
        private int[] xv_lp4 = new int[3];
        private int[] yv_lp4 = new int[3];
        //4 High pass filters
        private int[] xv_hp1 = new int[3];
        private int[] yv_hp1 = new int[3];
        private int[] xv_hp2 = new int[3];
        private int[] yv_hp2 = new int[3];
        private int[] xv_hp3 = new int[3];
        private int[] yv_hp3 = new int[3];
        private int[] xv_hp4 = new int[3];
        private int[] yv_hp4 = new int[3];
        //6 Band pass filters
        private int[] xv_bp1 = new int[3];
        private int[] yv_bp1 = new int[3];
        private int[] xv_bp2 = new int[3];
        private int[] yv_bp2 = new int[3];
        private int[] xv_bp3 = new int[3];
        private int[] yv_bp3 = new int[3];
        private int[] xv_bp4 = new int[3];
        private int[] yv_bp4 = new int[3];
        private int[] xv_bp5 = new int[3];
        private int[] yv_bp5 = new int[3];
        private int[] xv_bp6 = new int[3];
        private int[] yv_bp6 = new int[3];
        //4 Band stop (Notch) filters
        private int[] xv_bs1 = new int[3];
        private int[] yv_bs1 = new int[3];
        private int[] xv_bs2 = new int[3];
        private int[] yv_bs2 = new int[3];
        private int[] xv_bs3 = new int[3];
        private int[] yv_bs3 = new int[3];
        private int[] xv_bs4 = new int[3];
        private int[] yv_bs4 = new int[3];
        /**/
        /*******************************************************/

        private short filter_type = 1;  //Default LP filter
        private short scale_factor = 1;
        private int filterOffset = 0;

        private byte[] sendData;

        //ADC
        private const int tim3_clock_freq = 72000000;
        private short tim3_prescaler = 44;//7199;
        private short tim3_autoreload = 199;//79;
        private short adc_vref_mv = 2880;
        private short adc_resolution_bits = 12;
        private short adc_channel = 2;
        private short adc_sampling_frequency = 125;//500;
        private short sampling_rate_multiplier = 1;
        private short oversampling_enabled = 1;
        private const int ADC_MAX_LEVEL = 4096;
        private const int ADC_OVERSAMPLED_MAX_LEVEL = 32768;//65536
        private int adc_max_data = ADC_OVERSAMPLED_MAX_LEVEL;//ADC_MAX_LEVEL;
        private byte ecg_lead = 0;
        private byte selected_ecg_lead = 0;
        
        //DAC
        private const int tim6_clock_freq = 72000000;
        private short tim6_prescaler = 3749;
        private short tim6_autoreload = 149;
        private const float dac_vref_mV = 2880f;
        private short dac_freq1_amp = 1024; //Max allowed is 2048
        private short dac_freq1_npt = 128;
        private short dac_freq2_amp = 0;
        private short dac_freq2_npt = 0;
        private short dac_freq1_Hz = 1; //To prevent divide by 0 condition within ECG Button action
        private short dac_amp1_mV;
        private short dac_freq2_Hz;
        private short dac_amp2_mV;
        private short dac_dma_tim6_enabled = 1;
        private short dac_dc_amp = 0;   //Max allowed is 4096
        private const int DAC_MAX_LEVEL = 4095;
        private short dac_out_ia_ref_enabled = 0;

        //ECG HeartRate
        private int upper_threshold_mV = 0;
        private int lower_threshold_mV = 0;
        private int HRInterval = 0;
        private int HR = 0;
        private bool bHROn = false;

        //Constant strings
        private const string device_connected = "YANTRONIX-HID [Connected]";
        private const string device_disconnected = "YANTRONIX-HID [Disconnected]";
        private const string adc_enable = "ADC - ENABLE";
        private const string adc_disable = "ADC - DISABLE";
        private const string dac_enable = "DAC - ENABLE";
        private const string dac_disable = "DAC - DISABLE";
        private const string led_enable = "LED - ON";
        private const string led_disable = "LED - OFF";
        private const string dfu_enable = "DFU - ON";
        private const string test_disable = "TEST - OFF";
        private const string test_enable = "TEST - ON";
        private const string record_log_disable = "RECORD LOG FILE";
        private const string record_log_enable = "STOP RECORDING";
        private const string play_log_disable = "PLAY LOG FILE";
        private const string play_log_enable = "STOP PLAYING";
        private const string fft_enable = "FFT - ENABLE";
        private const string fft_disable = "FFT - DISABLE";
        private const string filter_enable = "APPLY HOST FILTER";
        private const string filter_disable = "REMOVE HOST FILTER";
        private const string fir_filter_enable = "FIR - ON";
        private const string fir_filter_disable = "FIR - OFF";
        private const string iir_arma_filter_enable = "IIR ARMA - ON";
        private const string iir_arma_filter_disable = "IIR ARMA - OFF";
        private const string iir_biquad_filter_enable = "IIR BIQUAD - ON";
        private const string iir_biquad_filter_disable = "IIR BIQUAD - OFF";
        private const string iir_mk_filter_enable = "IIR MK - ON";
        private const string iir_mk_filter_disable = "IIR MK - OFF";
        private const string ecg_enable = "ECG - ON";
        private const string ecg_disable = "ECG - OFF";
        private const string hr_enable = "DISPLAY HR";
        private const string hr_disable = "HIDE HR";

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

            this.Text = device_disconnected;
            this.adc_button.Text = adc_enable;
            this.dac_button.Text = dac_enable;
            this.led_button.Text = led_enable;
            this.dfu_button.Text = dfu_enable;
            this.test_button.Text = test_enable;
            this.record_log_button.Text = record_log_disable;
            this.play_log_button.Text = play_log_disable;
            this.ecg_button.Text = ecg_enable;
            this.hr_button.Text = hr_enable;
            this.filter_button.Text = filter_enable;
            this.fir_filter_button.Text = fir_filter_enable;
            this.iir_arma_filter_button.Text = iir_arma_filter_enable;
            this.iir_biquad_filter_button.Text = iir_biquad_filter_enable;
            this.iir_mk_filter_button.Text = iir_mk_filter_enable;

            this.ecg_lead_comboBox.Items.Add("12 Leads");
            this.ecg_lead_comboBox.Items.Add("I");
            this.ecg_lead_comboBox.Items.Add("II");
            this.ecg_lead_comboBox.Items.Add("III");
            this.ecg_lead_comboBox.Items.Add("aVR");
            this.ecg_lead_comboBox.Items.Add("aVL");
            this.ecg_lead_comboBox.Items.Add("aVF");
            this.ecg_lead_comboBox.Items.Add("V1");
            this.ecg_lead_comboBox.Items.Add("V2");
            this.ecg_lead_comboBox.Items.Add("V3");
            this.ecg_lead_comboBox.Items.Add("V4");
            this.ecg_lead_comboBox.Items.Add("V5");
            this.ecg_lead_comboBox.Items.Add("V6");
            this.ecg_lead_comboBox.Items.Add("Test Lead");

            this.adc_channel_comboBox.Items.Add("");
            this.adc_channel_comboBox.Items.Add("1");
            this.adc_channel_comboBox.Items.Add("2");

            this.adc_samp_freq_comboBox.Items.Add("");
            this.adc_samp_freq_comboBox.Items.Add("125");
            this.adc_samp_freq_comboBox.Items.Add("250");
            this.adc_samp_freq_comboBox.Items.Add("500");
            //this.adc_samp_freq_comboBox.Items.Add("1000");

            this.buffer_length_comboBox.Items.Add("");
            for (int i = 1; i < 11; i++)
                this.buffer_length_comboBox.Items.Add(i.ToString());

            this.dac_freq1_comboBox.Items.Add("");
            for (int i = 1; i < 76; i++)
                this.dac_freq1_comboBox.Items.Add(i.ToString());

            this.dac_amp1_comboBox.Items.Add("");
            for (int i = 1; i < 2881; i++)
                this.dac_amp1_comboBox.Items.Add(i.ToString());

            this.dac_dc_amp_comboBox.Items.Add("");
            for (int i = 1; i < 2881; i++)
                this.dac_dc_amp_comboBox.Items.Add(i.ToString());

            this.upper_threshold_comboBox.Items.Add("");
            for (int i = 0; i < 2881; i++)
                this.upper_threshold_comboBox.Items.Add(i.ToString());

            this.fft_points_comboBox.Items.Add("");
            this.fft_points_comboBox.Items.Add("256");

            this.filter_type_comboBox.Items.Add("");
            this.filter_type_comboBox.Items.Add("BUT_ORD-4_LP-30_FS-125");
            this.filter_type_comboBox.Items.Add("BUT_ORD-4_BP-1-30_FS-125");
            this.filter_type_comboBox.Items.Add("BUT_BIQUAD_ORD-12_BP-1-30_FS-125");

            this.filter_scale_factor_comboBox.Items.Add("");
            for (int i = 1; i < 101; i++)
                this.filter_scale_factor_comboBox.Items.Add(i.ToString());

            traceView1.SampleRate = adc_sampling_frequency;
            traceView1.BufferLength = adc_sampling_frequency * sampling_rate_multiplier;
            traceView1.DataMax = ADC_OVERSAMPLED_MAX_LEVEL;// ADC_MAX_LEVEL;  //4096;
            traceView1.DataMin = 0;
            traceView1.YMaxMeasure = adc_vref_mv; //1500f / 1200f;
            traceView1.YMinMeasure = 0f;    //-1500f / 1200f;
            traceView1.UpperThreshold = 0;
            traceView1.LowerThreshold = 0;
            traceView1.FillBuffer(0);
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
            //this.lb_message.Items.Add("Found a Device");
        }

        private void usb_OnDeviceRemoved(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                Invoke(new EventHandler(usb_OnDeviceRemoved), new object[] { sender, e });
            }
            else
            {
                //this.lb_message.Items.Add("Device was removed");
            }
        }

        private void usb_OnSpecifiedDeviceArrived(object sender, EventArgs e)
        {
            //this.lb_message.Items.Add("My device was found");
            this.Text = device_connected;

            sendData = new byte[specified_device.OutputReportLength];

            this.adc_button.Text = adc_disable;
            this.dac_button.Text = dac_disable;
            this.led_button.Text = led_enable;
            this.dfu_button.Text = dfu_enable;
            this.test_button.Text = test_enable;
            this.record_log_button.Text = record_log_disable;
            this.play_log_button.Text = play_log_disable;
            this.ecg_button.Text = ecg_enable;
            this.hr_button.Text = hr_enable;                

            this.fft_button.Text = fft_enable;
            this.filter_button.Text = filter_enable;
            this.fir_filter_button.Text = fir_filter_enable;
            this.iir_arma_filter_button.Text = iir_arma_filter_enable;
            this.iir_biquad_filter_button.Text = iir_biquad_filter_enable;
            this.iir_mk_filter_button.Text = iir_mk_filter_enable;

            this.shutdown_button.Enabled = true;
            this.adc_button.Enabled = false;
            this.dac_button.Enabled = false;
            this.led_button.Enabled = true;
            this.dfu_button.Enabled = true;
            this.test_button.Enabled = true;
            this.record_log_button.Enabled = false;
            this.play_log_button.Enabled = false;
            this.ecg_button.Enabled = true;
            this.hr_button.Enabled = false;
            this.fft_button.Enabled = false;
            this.filter_button.Enabled = true;
            this.fir_filter_button.Enabled = false;
            this.iir_arma_filter_button.Enabled = false;
            this.iir_biquad_filter_button.Enabled = false;
            this.iir_mk_filter_button.Enabled = true;

            this.ecg_lead_comboBox.Text = " 12 Leads ";
            this.ecg_lead_comboBox.Enabled = true;

            this.filter_type_comboBox.Enabled = true;
            this.filter_scale_factor_comboBox.Enabled = true;

            this.adc_channel_comboBox.Enabled = false;
            this.adc_samp_freq_comboBox.Enabled = false;
            this.buffer_length_comboBox.Enabled = false;
            this.oversampling_enable_checkBox.Enabled = false;

            this.ac_output_radioButton.Enabled = false;
            this.ac_output_radioButton.Checked = false;
            this.dc_output_radioButton.Checked = false;
            this.dc_output_radioButton.Enabled = false;
            this.dac_freq1_comboBox.Enabled = false;
            this.dac_freq2_comboBox.Enabled = false;
            this.dac_amp1_comboBox.Enabled = false;
            this.dac_amp2_comboBox.Enabled = false;
            this.dac_dc_amp_comboBox.Enabled = false;
            this.dac_out_ia_ref_enable_checkBox.Enabled = false;

            this.fft_points_comboBox.Enabled = false;

            this.upper_threshold_comboBox.Enabled = false;
            this.lower_threshold_comboBox.Enabled = false;

            //Initialise all variables
            filter_type = 1;
            scale_factor = 1;
            filterOffset = 0;

            tim3_prescaler = 44;// 7199;
            tim3_autoreload = 199;// 79;
            adc_resolution_bits = 12;
            adc_channel = 2;
            adc_sampling_frequency = 125;   //500;
            sampling_rate_multiplier = 1;
            oversampling_enabled = 1;
            adc_max_data = ADC_OVERSAMPLED_MAX_LEVEL;// ADC_MAX_LEVEL;
            ecg_lead = 0;

            tim6_prescaler = 3749;
            tim6_autoreload = 149;
            dac_freq1_amp = 1024; //Max allowed is 2048
            dac_freq1_npt = 128;
            dac_freq2_amp = 0;
            dac_freq2_npt = 0;
            dac_freq1_Hz = 1; //To prevent divide by 0 condition within ECG Button action
            dac_amp1_mV = 0;
            dac_freq2_Hz = 0;
            dac_amp2_mV = 0;
            dac_dma_tim6_enabled = 1;
            dac_dc_amp = 0;
            dac_out_ia_ref_enabled = 0;

            upper_threshold_mV = 0;
            lower_threshold_mV = 0;
            hr_label.Text = "";
            hr_label.Visible = false;

            traceView1.SampleRate = adc_sampling_frequency;
            traceView1.BufferLength = adc_sampling_frequency * sampling_rate_multiplier;
            traceView1.DataMax = ADC_OVERSAMPLED_MAX_LEVEL;// ADC_MAX_LEVEL;  //4096;
            traceView1.DataMin = 0;
            traceView1.YMaxMeasure = adc_vref_mv; //1500f / 1200f;
            traceView1.YMinMeasure = 0f;    //-1500f / 1200f;
            traceView1.UpperThreshold = 0;
            traceView1.LowerThreshold = 0;
            traceView1.FillBuffer(0);
        }

        private void usb_OnSpecifiedDeviceRemoved(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                Invoke(new EventHandler(usb_OnSpecifiedDeviceRemoved), new object[] { sender, e });
            }
            else
            {
                //this.lb_message.Items.Add("My device was removed");
                this.Text = device_disconnected;

                this.adc_button.Text = adc_enable;
                this.dac_button.Text = dac_enable;
                this.led_button.Text = led_enable;
                this.dfu_button.Text = dfu_enable;
                this.test_button.Text = test_enable;
                this.record_log_button.Text = record_log_disable;
                this.play_log_button.Text = play_log_disable;
                this.ecg_button.Text = ecg_enable;
                this.hr_button.Text = hr_enable;
                this.fft_button.Text = fft_enable;
                this.filter_button.Text = filter_enable;
                this.fir_filter_button.Text = fir_filter_enable;
                this.iir_arma_filter_button.Text = iir_arma_filter_enable;
                this.iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                this.iir_mk_filter_button.Text = iir_mk_filter_enable;

                this.shutdown_button.Enabled = false;
                this.adc_button.Enabled = false;
                this.dac_button.Enabled = false;
                this.led_button.Enabled = false;
                this.dfu_button.Enabled = false;
                this.test_button.Enabled = false;
                this.record_log_button.Enabled = false;
                this.play_log_button.Enabled = false;
                this.ecg_button.Enabled = false;
                this.hr_button.Enabled = false;
                this.fft_button.Enabled = false;
                this.filter_button.Enabled = false;
                this.fir_filter_button.Enabled = false;
                this.iir_arma_filter_button.Enabled = false;
                this.iir_biquad_filter_button.Enabled = false;
                this.iir_mk_filter_button.Enabled = false;

                this.ecg_lead_comboBox.Text = "";
                this.ecg_lead_comboBox.Enabled = false;

                this.adc_channel_comboBox.Enabled = false;
                this.adc_samp_freq_comboBox.Enabled = false;
                this.buffer_length_comboBox.Enabled = false;

                this.filter_type_comboBox.Enabled = false;
                this.filter_scale_factor_comboBox.Enabled = false;

                //this.adc_channel_comboBox.SelectedIndex = 0;
                //this.adc_samp_freq_comboBox.SelectedIndex = 0;
                this.oversampling_enable_checkBox.Enabled = false;

                //this.dac_freq1_comboBox.SelectedIndex = 0;
                //this.dac_freq2_comboBox.SelectedIndex = 0;
                //this.dac_amp1_comboBox.SelectedIndex = 0;
                //this.dac_amp2_comboBox.SelectedIndex = 0;
                //this.dac_dc_amp_comboBox.SelectedIndex = 0;
                this.ac_output_radioButton.Enabled = false;
                this.dc_output_radioButton.Enabled = false;
                this.dac_freq1_comboBox.Enabled = false;
                this.dac_freq2_comboBox.Enabled = false;
                this.dac_amp1_comboBox.Enabled = false;
                this.dac_amp2_comboBox.Enabled = false;
                this.dac_dc_amp_comboBox.Enabled = false;
                this.dac_out_ia_ref_enable_checkBox.Enabled = false;
                
                this.fft_points_comboBox.Enabled = false;

                //this.upper_threshold_comboBox.SelectedIndex = 0;
                //this.lower_threshold_comboBox.SelectedIndex = 0;
                this.upper_threshold_comboBox.Enabled = false;
                this.lower_threshold_comboBox.Enabled = false;
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
                ecg_lead = arrData[1];

                switch (m_reportID)
                {
                    case 1:
                        string printStr = new string(Encoding.ASCII.GetString(arrData, 2, arrData.Length - 2).ToCharArray());
                        this.received_message_listBox.Items.Insert(0, printStr);
                        //this.lb_message.Items.Add(printStr);
                        break;

                    case 2:
                        adc_vref_mv = (short)(arrData[2] + (arrData[3] << 8));
                        adc_resolution_bits = (short)(arrData[4] + (arrData[5] << 8));
                        adc_sampling_frequency = (short)(arrData[6] + (arrData[7] << 8));

                        adc_max_data = (int)(Math.Pow(2, adc_resolution_bits));

                        traceView1.SampleRate = adc_sampling_frequency;
                        traceView1.BufferLength = adc_sampling_frequency * sampling_rate_multiplier;
                        traceView1.DataMax = adc_max_data;
                        traceView1.DataMin = 0;
                        traceView1.YMaxMeasure = adc_vref_mv;
                        traceView1.YMinMeasure = 0f;
                        break;

                    case 3:
                        //int size = (arrData.Length) / 2;
                        int[] raw_data = new int[31];   //new int[size - 1];

                        int i = 2;  //first 2 bytes are report ID so exclude it
                        int j = 0;
                        //while (i < arrData.Length)
                        while (i < 64)
                        {
                            raw_data[j] = (int)(arrData[i] + (arrData[i + 1] << 8));
                            i += 2;
                            j++;
                        }

                        if (filter_button.Text.Equals(filter_enable))
                        {
                            traceView1.AddDataAtEnd(raw_data);
                        }
                        else
                        {
                            traceView1.AddDataAtEnd(filterProcess(raw_data));
                        }

                        //if (adc_channel == 1)
                            //detectHR(raw_data);

                        traceView1.Invalidate();
                        break;

                    case 4:
                        int[] iir_mk_filter_data = new int[31];

                        i = 2;  //first 2 bytes are report ID so exclude it
                        j = 0;
                        while (i < 64)
                        {
                            iir_mk_filter_data[j] = (int)(arrData[i] + (arrData[i + 1] << 8));
                            i += 2;
                            j++;
                        }

                        if (filter_button.Text.Equals(filter_enable))
                        {
                            traceView1.AddDataAtEnd(iir_mk_filter_data);
                        }
                        else
                        {
                            traceView1.AddDataAtEnd(filterProcess(iir_mk_filter_data));
                        }

                        //if (adc_channel == 1)
                        //detectHR(iir_mk_filter_data);

                        traceView1.Invalidate();
                        break;

                    case 5:
                        if(arrData[2]==1)
                            this.right_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOn;
                        else
                            this.right_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;

                        this.received_message_listBox.Items.Insert(0, "Message From STM32: " + (short)(arrData[4] + (arrData[5] << 8)));
                        break;

                    case 6:
                        if (arrData[2] == 1)
                            this.left_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOn;
                        else
                            this.left_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;

                        this.received_message_listBox.Items.Insert(0, "Message From STM32: " + (short)(arrData[4] + (arrData[5] << 8)));
                        break;

                    case 7:
                        if (arrData[2] == 1)
                            this.up_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOn;
                        else
                            this.up_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;

                        this.received_message_listBox.Items.Insert(0, "Message From STM32: " + (short)(arrData[4] + (arrData[5] << 8)));
                        break;

                    case 8:
                        if (arrData[2] == 1)
                            this.down_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOn;
                        else
                            this.down_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;

                        this.received_message_listBox.Items.Insert(0, "Message From STM32: " + (short)(arrData[4] + (arrData[5] << 8)));
                        break;

                    default:
                        string rec_data = "Data: ";
                        //int rec_data_count = 0;
                        foreach (byte myData in arrData)
                        {
                            if (myData.ToString().Length == 1)
                            {
                                rec_data += "00";
                            }

                            if (myData.ToString().Length == 2)
                            {
                                rec_data += "0";
                            }

                            rec_data += myData.ToString() + " ";

                            //++rec_data_count;
                            //if (rec_data_count == 6)
                            //    break;
                        }
                        this.received_message_listBox.Items.Insert(0, rec_data); 
                        break;
                }
            }
        }

        private void usb_OnDataSend(object sender, DataSendEventArgs e)
        {
            //this.lb_message.Items.Add("Some data was send");
            this.received_message_listBox.Items.Insert(0, "Output report sent successfully.");
        }

        /*
        private int[] filterProcess(int[] data)
        {
            int i = 0;
            int[] output = new int[data.Length];

            while (i < data.Length)
            {
                xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6];
                xv[6] = (double)data[i] / gain;
                yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6];
                yv[6] = (xv[6] - xv[0]) + 3 * (xv[2] - xv[4])
                 + (-0.6175512499 * yv[0]) + (3.9197050969 * yv[1])
                 + (-10.4691513780 * yv[2]) + (15.0642577980 * yv[3])
                 + (-12.3121230920 * yv[4]) + (5.4148628120 * yv[5]);
                output[i++] = (short)yv[6] + 2048;
            }

            return output;
        }
        */

        /*************** Floating point Process ****************/
        /*
        private int[] filterProcess(int[] data)
        {
            int i = 0;
            int[] output = new int[data.Length];

            while (i < data.Length)
            {
                switch (filter_type)
                {
                    case 1:
                        //30Hz 4th order Butterworth LP filter, Fs=125Hz
                        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                        xv[4] = (double)data[i] / (1.209590303e+01 / scale_factor);
                        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                        yv[4] = (xv[0] + xv[4]) + 4 * (xv[1] + xv[3]) + 6 * xv[2]
                                     + (-0.0181370770 * yv[0]) + (0.0329072181 * yv[1])
                                     + (-0.4937426526 * yv[2]) + (0.1562105841 * yv[3]);
                        output[i++] = (int)(yv[4] - (adc_max_data / 2) * (scale_factor - 1)); //For LP & BS filters                       
                        break;

                    case 2:
                        //1Hz-30Hz 4th order Butterworth BP filter, Fs=125Hz
                        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                        xv[4] = (double)data[i] / (3.796330621e+00 / scale_factor);
                        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                        yv[4] = (xv[0] + xv[4]) - 2 * xv[2]
                                     + (-0.1746814133 * yv[0]) + (0.4597046484 * yv[1])
                                     + (-1.3220128045 * yv[2]) + (2.0342085435 * yv[3]);
                        output[i++] = (int)(yv[4] + (adc_max_data / 2));  //For HP & BP filters                     
                        break;

                    case 3:
                        //6 * 2nd order Butterworth Band pass cascaded filters, 1Hz - 30Hz, Fs=125, optional-first 2 with gain of 'scale_factor' each.
                        xv_bp1[0] = xv_bp1[1]; xv_bp1[1] = xv_bp1[2];
                        xv_bp1[2] = (double)data[i] / (1.977594035e+00 / scale_factor);
                        yv_bp1[0] = yv_bp1[1]; yv_bp1[1] = yv_bp1[2];
                        yv_bp1[2] = (xv_bp1[2] - xv_bp1[0])
                                     + (-0.0566090212 * yv_bp1[0]) + (1.0078744416 * yv_bp1[1]);

                        xv_bp2[0] = xv_bp2[1]; xv_bp2[1] = xv_bp2[2];
                        xv_bp2[2] = yv_bp1[2] / (1.977594035e+00 / scale_factor);
                        yv_bp2[0] = yv_bp2[1]; yv_bp2[1] = yv_bp2[2];
                        yv_bp2[2] = (xv_bp2[2] - xv_bp2[0])
                                     + (-0.0566090212 * yv_bp2[0]) + (1.0078744416 * yv_bp2[1]);

                        xv_bp3[0] = xv_bp3[1]; xv_bp3[1] = xv_bp3[2];
                        xv_bp3[2] = yv_bp2[2] / 1.977594035e+00;
                        yv_bp3[0] = yv_bp3[1]; yv_bp3[1] = yv_bp3[2];
                        yv_bp3[2] = (xv_bp3[2] - xv_bp3[0])
                                     + (-0.0566090212 * yv_bp3[0]) + (1.0078744416 * yv_bp3[1]);

                        xv_bp4[0] = xv_bp4[1]; xv_bp4[1] = xv_bp4[2];
                        xv_bp4[2] = yv_bp3[2] / 1.977594035e+00;
                        yv_bp4[0] = yv_bp4[1]; yv_bp4[1] = yv_bp4[2];
                        yv_bp4[2] = (xv_bp4[2] - xv_bp4[0])
                                     + (-0.0566090212 * yv_bp4[0]) + (1.0078744416 * yv_bp4[1]);

                        xv_bp5[0] = xv_bp5[1]; xv_bp5[1] = xv_bp5[2];
                        xv_bp5[2] = yv_bp4[2] / 1.977594035e+00;
                        yv_bp5[0] = yv_bp5[1]; yv_bp5[1] = yv_bp5[2];
                        yv_bp5[2] = (xv_bp5[2] - xv_bp5[0])
                                     + (-0.0566090212 * yv_bp5[0]) + (1.0078744416 * yv_bp5[1]);

                        xv_bp6[0] = xv_bp6[1]; xv_bp6[1] = xv_bp6[2];
                        xv_bp6[2] = yv_bp5[2] / 1.977594035e+00;
                        yv_bp6[0] = yv_bp6[1]; yv_bp6[1] = yv_bp6[2];
                        yv_bp6[2] = (xv_bp6[2] - xv_bp6[0])
                                     + (-0.0566090212 * yv_bp6[0]) + (1.0078744416 * yv_bp6[1]);

                        output[i++] = (int)(yv_bp6[2] + (adc_max_data / 2));
                        break;
                }
            }
            return output;
        }
        */
        /*******************************************************/

        /*************** Fixed point Process ****************/
        /**/
        private int[] filterProcess(int[] data)
        {
            int i = 0;
            int[] output = new int[data.Length];

            while (i < data.Length)
            {
                switch (filter_type)
                {
                    case 1:
                        //30Hz 4th order Butterworth LP filter, Fs=125Hz
                        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                        xv[4] = data[i];
                        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                        yv[4] = (scale_factor * 2709 * ((xv[0] + xv[4]) + 4 * (xv[1] + xv[3]) + 6 * xv[2])
                                     + (-594 * yv[0]) + (1078 * yv[1])
                                     + (-16178 * yv[2]) + (5118 * yv[3])) >> 15;
                        output[i++] = (ushort)(yv[4] - (adc_max_data / 2) * (scale_factor - 1)); //For LP & BS filters                       
                        break;

                    case 2:
                        //1Hz-30Hz 4th order Butterworth BP filter, Fs=125Hz
                        xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
                        xv[4] = data[i];
                        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
                        yv[4] = (scale_factor * 8631 * ((xv[0] + xv[4]) - 2 * xv[2])
                                     + (-5723 * yv[0]) + (15063 * yv[1])
                                     + (-43319 * yv[2]) + (66656 * yv[3])) >> 15;
                        output[i++] = (ushort)(yv[4] + (adc_max_data / 2));  //For HP & BP filters                     
                        break;

                    case 3:
                        //6 * 2nd order Butterworth Band pass cascaded filters, 1Hz - 30Hz, Fs=125, optional-first 2 with gain of 'scale_factor' each.
                        xv_bp1[0] = xv_bp1[1]; xv_bp1[1] = xv_bp1[2];
                        xv_bp1[2] = data[i];
                        yv_bp1[0] = yv_bp1[1]; yv_bp1[1] = yv_bp1[2];
                        yv_bp1[2] = (scale_factor * 16569 * (xv_bp1[2] - xv_bp1[0])
                                     + (-1854 * yv_bp1[0]) + (33026 * yv_bp1[1])) >> 15;

                        xv_bp2[0] = xv_bp2[1]; xv_bp2[1] = xv_bp2[2];
                        xv_bp2[2] = yv_bp1[2];
                        yv_bp2[0] = yv_bp2[1]; yv_bp2[1] = yv_bp2[2];
                        yv_bp2[2] = (scale_factor * 16569 * (xv_bp2[2] - xv_bp2[0])
                                     + (-1854 * yv_bp2[0]) + (33026 * yv_bp2[1])) >> 15;

                        xv_bp3[0] = xv_bp3[1]; xv_bp3[1] = xv_bp3[2];
                        xv_bp3[2] = yv_bp2[2];
                        yv_bp3[0] = yv_bp3[1]; yv_bp3[1] = yv_bp3[2];
                        yv_bp3[2] = (16569 * (xv_bp3[2] - xv_bp3[0])
                                     + (-1854 * yv_bp3[0]) + (33026 * yv_bp3[1])) >> 15;

                        xv_bp4[0] = xv_bp4[1]; xv_bp4[1] = xv_bp4[2];
                        xv_bp4[2] = yv_bp3[2];
                        yv_bp4[0] = yv_bp4[1]; yv_bp4[1] = yv_bp4[2];
                        yv_bp4[2] = (16569 * (xv_bp4[2] - xv_bp4[0])
                                     + (-1854 * yv_bp4[0]) + (33026 * yv_bp4[1])) >> 15;

                        xv_bp5[0] = xv_bp5[1]; xv_bp5[1] = xv_bp5[2];
                        xv_bp5[2] = yv_bp4[2];
                        yv_bp5[0] = yv_bp5[1]; yv_bp5[1] = yv_bp5[2];
                        yv_bp5[2] = (16569 * (xv_bp5[2] - xv_bp5[0])
                                     + (-1854 * yv_bp5[0]) + (33026 * yv_bp5[1])) >> 15;

                        xv_bp6[0] = xv_bp6[1]; xv_bp6[1] = xv_bp6[2];
                        xv_bp6[2] = yv_bp5[2];
                        yv_bp6[0] = yv_bp6[1]; yv_bp6[1] = yv_bp6[2];
                        yv_bp6[2] = (16569 * (xv_bp6[2] - xv_bp6[0])
                                     + (-1854 * yv_bp6[0]) + (33026 * yv_bp6[1])) >> 15;
                                     
                        output[i++] = (ushort)(yv_bp6[2] + (adc_max_data / 2));
                        break;
                }
            }
            return output;
        }
        /**/
        /*******************************************************/

        /*
        //Fixed point
        private int[] ecgFilterProcess(short[] data)
        {
            int i = 0;
            int[] output = new int[data.Length];

            for (i = 0; i < data.Length; i++)
            {
                //4 Band pass filters, 0.05 - 100Hz, Fs=500, first 2 with gain of 4 each.
                xv_bp1[0] = xv_bp1[1]; xv_bp1[1] = xv_bp1[2];
                xv_bp1[2] = data[i];
                yv_bp1[0] = yv_bp1[1]; yv_bp1[1] = yv_bp1[2];
                yv_bp1[2] = (6891 * (xv_bp1[2] - xv_bp1[0])
                             + (-650 * yv_bp1[0]) + (4745 * yv_bp1[1])) >> 12;

                xv_bp2[0] = xv_bp2[1]; xv_bp2[1] = xv_bp2[2];
                xv_bp2[2] = yv_bp1[2];
                yv_bp2[0] = yv_bp2[1]; yv_bp2[1] = yv_bp2[2];
                yv_bp2[2] = (6891 * (xv_bp2[2] - xv_bp2[0])
                             + (-650 * yv_bp2[0]) + (4745 * yv_bp2[1])) >> 12;

                xv_bp3[0] = xv_bp3[1]; xv_bp3[1] = xv_bp3[2];
                xv_bp3[2] = yv_bp2[2];
                yv_bp3[0] = yv_bp3[1]; yv_bp3[1] = yv_bp3[2];
                yv_bp3[2] = (1723 * (xv_bp3[2] - xv_bp3[0])
                             + (-650 * yv_bp3[0]) + (4745 * yv_bp3[1])) >> 12;

                xv_bp4[0] = xv_bp4[1]; xv_bp4[1] = xv_bp4[2];
                xv_bp4[2] = yv_bp3[2];
                yv_bp4[0] = yv_bp4[1]; yv_bp4[1] = yv_bp4[2];
                yv_bp4[2] = (1723 * (xv_bp4[2] - xv_bp4[0])
                             + (-650 * yv_bp4[0]) + (4745 * yv_bp4[1])) >> 12;
                
                //4 Band stop filters, 45 - 55Hz(50Hz Notch), Fs=500
                xv_bs1[0] = xv_bs1[1]; xv_bs1[1] = xv_bs1[2];
                xv_bs1[2] = yv_bp4[2];
                yv_bs1[0] = yv_bs1[1]; yv_bs1[1] = yv_bs1[2];
                yv_bs1[2] = (4096 * (xv_bs1[0] + xv_bs1[2]) - 6627 * xv_bs1[1]
                             + (-3598 * yv_bs1[0]) + (6211 * yv_bs1[1])) >> 12;
               
                xv_bs2[0] = xv_bs2[1]; xv_bs2[1] = xv_bs2[2];
                xv_bs2[2] = yv_bs1[2];
                yv_bs2[0] = yv_bs2[1]; yv_bs2[1] = yv_bs2[2];
                yv_bs2[2] = (4096 * (xv_bs2[0] + xv_bs2[2]) - 6627 * xv_bs2[1]
                             + (-3598 * yv_bs2[0]) + (6211 * yv_bs2[1])) >> 12;

                xv_bs3[0] = xv_bs3[1]; xv_bs3[1] = xv_bs3[2];
                xv_bs3[2] = yv_bs2[2];
                yv_bs3[0] = yv_bs3[1]; yv_bs3[1] = yv_bs3[2];
                yv_bs3[2] = (4096 * (xv_bs3[0] + xv_bs3[2]) - 6627 * xv_bs3[1]
                             + (-3598 * yv_bs3[0]) + (6211 * yv_bs3[1])) >> 12;

                xv_bs4[0] = xv_bs4[1]; xv_bs4[1] = xv_bs4[2];
                xv_bs4[2] = yv_bs3[2];
                yv_bs4[0] = yv_bs4[1]; yv_bs4[1] = yv_bs4[2];
                yv_bs4[2] = (4096 * (xv_bs4[0] + xv_bs4[2]) - 6627 * xv_bs4[1]
                             + (-3598 * yv_bs4[0]) + (6211 * yv_bs4[1])) >> 12;
                
                //Ensure that the output voltage is centered within the output range
                if (oversampling_enabled == 1)
                {
                    if ((yv_bs4[2] + filterOffset) > 24000) filterOffset -= 1;
                    if ((yv_bs4[2] + filterOffset) < 8000) filterOffset += 1;
                }
                else
                {
                    if ((yv_bs4[2] + filterOffset) > 3000) filterOffset -= 1;
                    if ((yv_bs4[2] + filterOffset) < 1000) filterOffset += 1;
                }

                //output[i] = (yv_bs4[2] + filterOffset) & 0xffff;  //0xfff
                output[i] = (short)(yv_bs4[2] + filterOffset);
            }

            return output;
        }
        */

        private void detectHR(short[] data)
        {
            int i = 0;
            if (!bHROn) // detect QRS wave
            {
                while (i < data.Length)
                {
                    ++HRInterval;
                    if (data[i++] >= traceView1.UpperThreshold && HRInterval > 48)  // HR must < 150/min
                    {
                        HR = (adc_sampling_frequency * 60) / HRInterval;   // 1 minute has 500 * 60 = 30000 samples for adc_sampling_frequency=500
                        bHROn = true;
                        HRInterval = 0;
                        hr_label.Text = HR + " bpm";
                        break;
                    }
                }
            }
            if (bHROn) // make sure the QRS wave return below threshold
            {
                HRInterval += data.Length - i;  // HR interval must > data.Length
                while (i < data.Length)
                {
                    if (data[i++] < traceView1.LowerThreshold)
                    {
                        bHROn = false;
                        break;
                    }
                }
            }
        }

        private void button_Click_Action(object sender, EventArgs e)
        {
            try
            {
                if (sender == shutdown_button)
                {
                    sendData[0] = 20;
                    sendData[1] = 1;
                }
                else if (sender == led_button)
                {
                    sendData[0] = 21;
                    if (led_button.Text.Equals(led_disable))
                    {
                        sendData[1] = 1;
                        //sendData[1] = 0;
                        led_button.Text = led_enable;
                    }
                    else
                    {
                        sendData[1] = 0;
                        //sendData[1] = 1;
                        led_button.Text = led_disable;
                    }
                }
                else if (sender == adc_button)
                {
                    sendData[0] = 22;
                    if (adc_button.Text.Equals(adc_disable))
                    {
                        sendData[1] = 0;
                        adc_button.Text = adc_enable;
                    }
                    else
                    {
                        sendData[1] = 1;
                        adc_button.Text = adc_disable;
                    }
                }
                else if (sender == dac_button)
                {
                    sendData[0] = 23;
                    if (dac_button.Text.Equals(dac_disable))
                    {
                        sendData[1] = 0;
                        dac_button.Text = dac_enable;
                    }
                    else
                    {
                        sendData[1] = 1;
                        dac_button.Text = dac_disable;
                    }
                }
                else if (sender == fft_button)
                {
                    sendData[0] = 26;
                    if (fft_button.Text.Equals(fft_disable))
                    {
                        sendData[1] = 0;
                        fft_button.Text = fft_enable;
                        fft_points_comboBox.SelectedIndex = 0;
                    }
                    else
                    {
                        sendData[1] = 1;
                        fft_button.Text = fft_disable;
                        fft_points_comboBox.SelectedIndex = 1;
                    }
                }
                else if (sender == filter_button)
                {
                    if (filter_button.Text.Equals(filter_disable))
                    {
                        filter_button.Text = filter_enable;
                    }
                    else
                    {
                        filter_button.Text = filter_disable;
                    }
                }
                else if (sender == fir_filter_button)
                {
                    sendData[0] = 27;
                    if (fir_filter_button.Text.Equals(fir_filter_disable))
                    {
                        sendData[1] = 0;
                        this.filter_button.Enabled = true;
                        fir_filter_button.Text = fir_filter_enable;
                        iir_arma_filter_button.Text = iir_arma_filter_enable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                        iir_mk_filter_button.Text = iir_mk_filter_enable;
                    }
                    else
                    {
                        sendData[1] = 1;
                        sendData[2] = (byte)(filter_type & 0xFF);
                        sendData[3] = (byte)((filter_type & 0xFF00) >> 8);
                        sendData[4] = (byte)(scale_factor & 0xFF);
                        sendData[5] = (byte)((scale_factor & 0xFF00) >> 8);
                        filter_button.Text = filter_enable;
                        this.filter_button.Enabled = false;
                        fir_filter_button.Text = fir_filter_disable;
                        iir_arma_filter_button.Text = iir_arma_filter_enable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                        iir_mk_filter_button.Text = iir_mk_filter_enable;
                    }
                }
                else if (sender == iir_arma_filter_button)
                {
                    sendData[0] = 28;
                    if (iir_arma_filter_button.Text.Equals(iir_arma_filter_disable))
                    {
                        sendData[1] = 0;
                        this.filter_button.Enabled = true;
                        fir_filter_button.Text = fir_filter_enable;
                        iir_arma_filter_button.Text = iir_arma_filter_enable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                        iir_mk_filter_button.Text = iir_mk_filter_enable;
                    }
                    else
                    {
                        sendData[1] = 1;
                        sendData[2] = (byte)(filter_type & 0xFF);
                        sendData[3] = (byte)((filter_type & 0xFF00) >> 8);
                        sendData[4] = (byte)(scale_factor & 0xFF);
                        sendData[5] = (byte)((scale_factor & 0xFF00) >> 8);
                        filter_button.Text = filter_enable;
                        this.filter_button.Enabled = false;
                        fir_filter_button.Text = fir_filter_enable;
                        iir_arma_filter_button.Text = iir_arma_filter_disable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                        iir_mk_filter_button.Text = iir_mk_filter_enable;
                    }
                }
                else if (sender == iir_biquad_filter_button)
                {
                    sendData[0] = 29;
                    if (iir_biquad_filter_button.Text.Equals(iir_biquad_filter_disable))
                    {
                        sendData[1] = 0;
                        this.filter_button.Enabled = true;
                        fir_filter_button.Text = fir_filter_enable;
                        iir_arma_filter_button.Text = iir_arma_filter_enable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                        iir_mk_filter_button.Text = iir_mk_filter_enable;
                    }
                    else
                    {
                        sendData[1] = 1;
                        sendData[2] = (byte)(filter_type & 0xFF);
                        sendData[3] = (byte)((filter_type & 0xFF00) >> 8);
                        sendData[4] = (byte)(scale_factor & 0xFF);
                        sendData[5] = (byte)((scale_factor & 0xFF00) >> 8);
                        filter_button.Text = filter_enable;
                        this.filter_button.Enabled = false;
                        fir_filter_button.Text = fir_filter_enable;
                        iir_arma_filter_button.Text = iir_arma_filter_enable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_disable;
                        iir_mk_filter_button.Text = iir_mk_filter_enable;
                    }
                }
                else if (sender == iir_mk_filter_button)
                {
                    sendData[0] = 28;
                    if (iir_mk_filter_button.Text.Equals(iir_mk_filter_disable))
                    {
                        sendData[1] = 0;
                        fir_filter_button.Text = fir_filter_enable;
                        iir_arma_filter_button.Text = iir_arma_filter_enable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                        iir_mk_filter_button.Text = iir_mk_filter_enable;
                    }
                    else
                    {
                        sendData[1] = 1;
                        sendData[2] = (byte)(filter_type & 0xFF);
                        sendData[3] = (byte)((filter_type & 0xFF00) >> 8);
                        sendData[4] = (byte)(scale_factor & 0xFF);
                        sendData[5] = (byte)((scale_factor & 0xFF00) >> 8);
                        fir_filter_button.Text = fir_filter_enable;
                        iir_arma_filter_button.Text = iir_arma_filter_enable;
                        iir_biquad_filter_button.Text = iir_biquad_filter_enable;
                        iir_mk_filter_button.Text = iir_mk_filter_disable;
                    }
                }
                else if (sender == ecg_button)
                {
                    sendData[0] = 26;
                    if (ecg_button.Text.Equals(ecg_disable))
                    {
                        sendData[1] = 0;
                        ecg_button.Text = ecg_enable;
                        this.test_button.Enabled = true;
                    }
                    else
                    {
                        sendData[1] = 1;
                        sendData[2] = selected_ecg_lead;
                        ecg_button.Text = ecg_disable;
                        this.test_button.Enabled = false;
                    }
                }
                else if (sender == hr_button)
                {
                    if (hr_button.Text.Equals(hr_disable))
                    {
                        hr_button.Text = hr_enable;
                        this.hr_label.Text = "";
                        this.hr_label.Visible = false;
                    }
                    else
                    {
                        hr_button.Text = hr_disable;
                        this.hr_label.Text = "0 bpm";
                        this.hr_label.Visible = true;
                    }
                }
                else if (sender == dfu_button)
                {
                    sendData[0] = 22;
                    sendData[1] = 1;
                }
                else if (sender == test_button)
                {
                    sendData[0] = 27;
                    if (test_button.Text.Equals(test_disable))
                    {
                        sendData[1] = 0;
                        test_button.Text = test_enable;
                        this.ecg_button.Enabled = true;
                    }
                    else
                    {
                        sendData[1] = 1;
                        test_button.Text = test_disable;
                        this.ecg_button.Enabled = false;
                    }
                }
                else if (sender == record_log_button)
                {
                    record_log_button.Enabled = false;

                    //DRAW_LCD_TRACE_DISABLED
                    sendData[0] = 32;
                    sendData[1] = 18;
                    sendData[2] = 0;//No Trace

                    if (specified_device != null)
                    {
                        specified_device.SendData(sendData);
                        System.Threading.Thread.Sleep(100);
                    }
                    else
                    {
                        MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                        record_log_button.Enabled = true; 
                        return;
                    }

                    sendData[0] = 33;
                    sendData[1] = 17;
                    if (record_log_button.Text.Equals(record_log_disable))
                    {
                        sendData[2] = 1;
                        record_log_button.Text = record_log_enable;
                        if (specified_device != null)
                        {
                            specified_device.SendData(sendData);
                            System.Threading.Thread.Sleep(100);
                        }
                        else
                        {
                            MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                        }
                    }
                    else
                    {
                        sendData[2] = 0;
                        record_log_button.Text = record_log_disable;
                        if (specified_device != null)
                        {
                            specified_device.SendData(sendData);
                            System.Threading.Thread.Sleep(100);
                        }
                        else
                        {
                            MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                        }
                    }

                    record_log_button.Enabled = true;

                    return;
                }
                else if (sender == play_log_button)
                {
                    play_log_button.Enabled = false;

                    //DRAW_LCD_TRACE_DISABLED
                    sendData[0] = 32;
                    sendData[1] = 18;
                    sendData[2] = 0;//No Trace

                    if (specified_device != null)
                    {
                        specified_device.SendData(sendData);
                        System.Threading.Thread.Sleep(100);
                    }
                    else
                    {
                        MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                        play_log_button.Enabled = true;
                        return;
                    }

                    sendData[0] = 33;
                    sendData[1] = 18;
                    if (play_log_button.Text.Equals(play_log_disable))
                    {
                        sendData[2] = 1;
                        play_log_button.Text = play_log_enable;
                        if (specified_device != null)
                        {
                            specified_device.SendData(sendData);
                            System.Threading.Thread.Sleep(100);
                        }
                        else
                        {
                            MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                        }
                    }
                    else
                    {
                        sendData[2] = 0;
                        play_log_button.Text = play_log_disable;
                        if (specified_device != null)
                        {
                            specified_device.SendData(sendData);
                            System.Threading.Thread.Sleep(100);
                        }
                        else
                        {
                            MessageBox.Show("Sorry but your device is not present. Plug it in!! ");
                        }
                    }

                    play_log_button.Enabled = true;

                    return;
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

        private void adc_config_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if ((sender == adc_channel_comboBox) && (adc_channel_comboBox.SelectedIndex != 0))
                {
                    adc_channel = (short)Convert.ToUInt16(this.adc_channel_comboBox.SelectedItem.ToString());
                    /*
                    if (adc_channel == 1)
                    {
                        this.dac_freq1_comboBox.Enabled = false;
                        this.dac_freq2_comboBox.Enabled = false;
                        this.dac_amp1_comboBox.Enabled = false;
                        this.dac_amp2_comboBox.Enabled = false;
                        //this.dac_button.Enabled = false;
                        this.ecg_button.Enabled = false;
                        this.hr_label.Text = "0 bpm";
                        this.hr_label.Visible = true;
                    }
                    else if (adc_channel == 2)
                    {
                        this.dac_freq1_comboBox.Enabled = true;
                        this.dac_freq2_comboBox.Enabled = true;
                        this.dac_amp1_comboBox.Enabled = true;
                        this.dac_amp2_comboBox.Enabled = true;
                        //this.dac_button.Enabled = true;
                        this.ecg_button.Enabled = true;
                        this.hr_label.Text = "";
                        this.hr_label.Visible = false;
                    }
                    */
                }
                else if ((sender == adc_samp_freq_comboBox) && (adc_samp_freq_comboBox.SelectedIndex != 0))
                {
                    adc_sampling_frequency = (short)Convert.ToUInt16(this.adc_samp_freq_comboBox.SelectedItem.ToString());
                    if (oversampling_enabled == 1)
                        tim3_autoreload = (short)((tim3_clock_freq / (tim3_prescaler + 1) / (64*adc_sampling_frequency)) - 1);
                    else
                        tim3_autoreload = (short)((tim3_clock_freq / (tim3_prescaler + 1) / adc_sampling_frequency) - 1);
                }
                else if ((sender == buffer_length_comboBox) && (buffer_length_comboBox.SelectedIndex != 0))
                {
                    sampling_rate_multiplier = (short)Convert.ToUInt16(this.buffer_length_comboBox.SelectedItem.ToString());
                }
                else
                {
                    return;
                }

                sendData[0] = 24;
                sendData[1] = (byte)(adc_channel & 0xFF);
                sendData[2] = (byte)((adc_channel & 0xFF00) >> 8);
                sendData[3] = (byte)(tim3_prescaler & 0xFF);
                sendData[4] = (byte)((tim3_prescaler & 0xFF00) >> 8);
                sendData[5] = (byte)(tim3_autoreload & 0xFF);
                sendData[6] = (byte)((tim3_autoreload & 0xFF00) >> 8);
                sendData[7] = (byte)(oversampling_enabled & 0xFF);
                sendData[8] = (byte)((oversampling_enabled & 0xFF00) >> 8);

                //MessageBox.Show("OverSampling: " + oversampling_enabled + ", TIM3_Prescaler: " + tim3_prescaler + ", TIM3_Autoreload: " + tim3_autoreload);

                if (specified_device != null)
                {
                    specified_device.SendData(sendData);

                    if (oversampling_enabled == 1)
                    {
                        adc_max_data = ADC_OVERSAMPLED_MAX_LEVEL;
                        traceView1.DataMax = ADC_OVERSAMPLED_MAX_LEVEL;
                        traceView1.DataMin = 0;
                        traceView1.UpperThreshold = (short)((traceView1.DataMax / dac_vref_mV) * upper_threshold_mV);
                        traceView1.LowerThreshold = (short)((traceView1.DataMax / dac_vref_mV) * lower_threshold_mV);
                    }
                    else
                    {
                        adc_max_data = ADC_MAX_LEVEL;
                        traceView1.DataMax = ADC_MAX_LEVEL;
                        traceView1.DataMin = 0;
                        traceView1.UpperThreshold = (short)((traceView1.DataMax / dac_vref_mV) * upper_threshold_mV);
                        traceView1.LowerThreshold = (short)((traceView1.DataMax / dac_vref_mV) * lower_threshold_mV);
                    }

                    traceView1.SampleRate = adc_sampling_frequency;
                    traceView1.BufferLength = adc_sampling_frequency * sampling_rate_multiplier;
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

        private void fft_points_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            //Do nothing
        }

        private void dac_config_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if ((sender == dac_freq1_comboBox) && (dac_freq1_comboBox.SelectedIndex != 0))
                {
                    dac_freq1_Hz = (short)Convert.ToUInt16(this.dac_freq1_comboBox.SelectedItem.ToString());
                    tim6_autoreload = (short)((tim6_clock_freq / (tim6_prescaler + 1) / dac_freq1_npt / dac_freq1_Hz) - 1);
                    
                    this.dac_freq2_comboBox.Items.Clear();
                    this.dac_freq2_comboBox.Items.Add("");
                    this.dac_freq2_comboBox.Items.Add((dac_freq1_Hz * 1).ToString());
                    this.dac_freq2_comboBox.Items.Add((dac_freq1_Hz * 2).ToString());
                    this.dac_freq2_comboBox.Items.Add((dac_freq1_Hz * 4).ToString());
                    this.dac_freq2_comboBox.Items.Add((dac_freq1_Hz * 8).ToString());
                    this.dac_freq2_comboBox.Items.Add((dac_freq1_Hz * 16).ToString());
                    this.dac_freq2_comboBox.Items.Add((dac_freq1_Hz * 32).ToString());

                    dac_freq2_npt = 0;
                    this.dac_freq2_comboBox.SelectedIndex = 0;
                }
                else if ((sender == dac_amp1_comboBox) && (dac_amp1_comboBox.SelectedIndex != 0))
                {
                    dac_amp1_mV = (short)Convert.ToUInt16(this.dac_amp1_comboBox.SelectedItem.ToString());
                    dac_freq1_amp = (short)(((DAC_MAX_LEVEL / 2) / dac_vref_mV) * dac_amp1_mV);

                    this.dac_amp2_comboBox.Items.Clear();
                    this.dac_amp2_comboBox.Items.Add("");
                    for (int i = 0; i < (2881-dac_amp1_mV); i++)
                        this.dac_amp2_comboBox.Items.Add(i.ToString());

                    dac_freq2_amp = 0;
                    this.dac_amp2_comboBox.SelectedIndex = 0;
                }
                else if ((sender == dac_freq2_comboBox) && (dac_freq2_comboBox.SelectedIndex != 0))
                {
                    dac_freq2_Hz = (short)Convert.ToUInt16(this.dac_freq2_comboBox.SelectedItem.ToString());
                    dac_freq2_npt = (short)(tim6_clock_freq / (tim6_prescaler + 1) / (tim6_autoreload + 1) / dac_freq2_Hz);
                    if (dac_freq2_npt > 4 && dac_freq2_npt < 8)
                    {
                        dac_freq2_npt = 4;
                    }
                    else if (dac_freq2_npt > 8 && dac_freq2_npt < 16)
                    {
                        dac_freq2_npt = 8;
                    }
                    else if (dac_freq2_npt > 16 && dac_freq2_npt < 32)
                    {
                        dac_freq2_npt = 16;
                    }
                    else if (dac_freq2_npt > 32 && dac_freq2_npt < 64)
                    {
                        dac_freq2_npt = 32;
                    }
                    else if (dac_freq2_npt > 64 && dac_freq2_npt < 128)
                    {
                        dac_freq2_npt = 64;
                    }
                    else if (dac_freq2_npt > 128)
                    {
                        dac_freq2_npt = 128;
                    }
                    //MessageBox.Show("Frequency2 set: " + (short)(tim6_clock_freq / (tim6_prescaler + 1) / (tim6_autoreload + 1) / dac_freq2_npt));
                }
                else if ((sender == dac_amp2_comboBox) && (dac_amp2_comboBox.SelectedIndex != 0))
                {
                    dac_amp2_mV = (short)Convert.ToUInt16(this.dac_amp2_comboBox.SelectedItem.ToString());
                    dac_freq2_amp = (short)(((DAC_MAX_LEVEL / 2) / dac_vref_mV) * dac_amp2_mV);
                }
                else if ((sender == dac_dc_amp_comboBox) && (dac_dc_amp_comboBox.SelectedIndex != 0))
                {
                    dac_dc_amp = (short)((DAC_MAX_LEVEL / dac_vref_mV) * ((short)Convert.ToUInt16(this.dac_dc_amp_comboBox.SelectedItem.ToString())));
                }
                else
                {
                    return;
                }

                if (dac_freq1_amp + dac_freq2_amp > (DAC_MAX_LEVEL / 2))
                {
                    MessageBox.Show("The Amplitudes selected exceeds the limit.\nPlease try again");
                    return;
                }

                sendData[0] = 25;
                sendData[1] = (byte)(dac_out_ia_ref_enabled & 0xFF);
                sendData[2] = (byte)((dac_out_ia_ref_enabled & 0xFF00) >> 8);
                sendData[3] = (byte)(dac_dma_tim6_enabled & 0xFF);
                sendData[4] = (byte)((dac_dma_tim6_enabled & 0xFF00) >> 8);
                sendData[5] = (byte)(dac_dc_amp & 0xFF);
                sendData[6] = (byte)((dac_dc_amp & 0xFF00) >> 8);
                sendData[7] = (byte)(tim6_prescaler & 0xFF);
                sendData[8] = (byte)((tim6_prescaler & 0xFF00) >> 8);
                sendData[9] = (byte)(tim6_autoreload & 0xFF);
                sendData[10] = (byte)((tim6_autoreload & 0xFF00) >> 8);
                sendData[11] = (byte)(dac_freq1_amp & 0xFF);
                sendData[12] = (byte)((dac_freq1_amp & 0xFF00) >> 8);
                sendData[13] = (byte)(dac_freq1_npt & 0xFF);
                sendData[14] = (byte)((dac_freq1_npt & 0xFF00) >> 8);
                sendData[15] = (byte)(dac_freq2_amp & 0xFF);
                sendData[16] = (byte)((dac_freq2_amp & 0xFF00) >> 8);
                sendData[17] = (byte)(dac_freq2_npt & 0xFF);
                sendData[18] = (byte)((dac_freq2_npt & 0xFF00) >> 8);

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

        private void oversampling_enable_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (oversampling_enable_checkBox.Checked)
                {
                    oversampling_enabled = 1;
                    tim3_prescaler = 44;
                    tim3_autoreload = (short)((tim3_clock_freq / (tim3_prescaler + 1) / (64 * adc_sampling_frequency)) - 1);
                }
                else
                {
                    oversampling_enabled = 0;
                    tim3_prescaler = 7199;
                    tim3_autoreload = (short)((tim3_clock_freq / (tim3_prescaler + 1) / adc_sampling_frequency) - 1);
                }

                sendData[0] = 24;
                sendData[1] = (byte)(adc_channel & 0xFF);
                sendData[2] = (byte)((adc_channel & 0xFF00) >> 8);
                sendData[3] = (byte)(tim3_prescaler & 0xFF);
                sendData[4] = (byte)((tim3_prescaler & 0xFF00) >> 8);
                sendData[5] = (byte)(tim3_autoreload & 0xFF);
                sendData[6] = (byte)((tim3_autoreload & 0xFF00) >> 8);
                sendData[7] = (byte)(oversampling_enabled & 0xFF);
                sendData[8] = (byte)((oversampling_enabled & 0xFF00) >> 8);

                //MessageBox.Show("OverSampling: " + oversampling_enabled + ", TIM3_Prescaler: " + tim3_prescaler + ", TIM3_Autoreload: " + tim3_autoreload);

                if (specified_device != null)
                {
                    specified_device.SendData(sendData);

                    if (oversampling_enabled == 1)
                    {
                        adc_max_data = ADC_OVERSAMPLED_MAX_LEVEL;
                        traceView1.DataMax = ADC_OVERSAMPLED_MAX_LEVEL;
                        traceView1.DataMin = 0;
                        traceView1.UpperThreshold = (short)((traceView1.DataMax / dac_vref_mV) * upper_threshold_mV);
                        traceView1.LowerThreshold = (short)((traceView1.DataMax / dac_vref_mV) * lower_threshold_mV);
                    }
                    else
                    {
                        adc_max_data = ADC_MAX_LEVEL;
                        traceView1.DataMax = ADC_MAX_LEVEL;
                        traceView1.DataMin = 0;
                        traceView1.UpperThreshold = (short)((traceView1.DataMax / dac_vref_mV) * upper_threshold_mV);
                        traceView1.LowerThreshold = (short)((traceView1.DataMax / dac_vref_mV) * lower_threshold_mV);
                    }
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

        private void threshold_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if ((sender == upper_threshold_comboBox) && (upper_threshold_comboBox.SelectedIndex != 0))
            {
                upper_threshold_mV = (short)Convert.ToUInt16(this.upper_threshold_comboBox.SelectedItem.ToString());
                traceView1.UpperThreshold = (short)((traceView1.DataMax / dac_vref_mV) * upper_threshold_mV);

                this.lower_threshold_comboBox.Items.Clear();
                this.lower_threshold_comboBox.Items.Add("");
                for (int i = 0; i < upper_threshold_mV+1; i++)
                    this.lower_threshold_comboBox.Items.Add(i.ToString());

                lower_threshold_mV = upper_threshold_mV;
                traceView1.LowerThreshold = traceView1.UpperThreshold;
                this.lower_threshold_comboBox.SelectedIndex = 0;
            }
            else if ((sender == lower_threshold_comboBox) && (lower_threshold_comboBox.SelectedIndex != 0))
            {
                lower_threshold_mV = (short)Convert.ToUInt16(this.lower_threshold_comboBox.SelectedItem.ToString());
                traceView1.LowerThreshold = (short)((traceView1.DataMax / dac_vref_mV) * lower_threshold_mV);
            }
        }

        private void dac_output_radioButton_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (ac_output_radioButton.Checked)
                {
                    dac_dma_tim6_enabled = 1;
                    this.dac_freq1_comboBox.Enabled = true;
                    this.dac_freq2_comboBox.Enabled = true;
                    this.dac_amp1_comboBox.Enabled = true;
                    this.dac_amp2_comboBox.Enabled = true;
                    this.ecg_button.Enabled = true;
                    this.dac_dc_amp_comboBox.Enabled = false;
                }
                else if (dc_output_radioButton.Checked)
                {
                    dac_dma_tim6_enabled = 0;
                    this.dac_freq1_comboBox.Enabled = false;
                    this.dac_freq2_comboBox.Enabled = false;
                    this.dac_amp1_comboBox.Enabled = false;
                    this.dac_amp2_comboBox.Enabled = false;
                    this.ecg_button.Enabled = false;
                    this.dac_dc_amp_comboBox.Enabled = true;
                }

                sendData[0] = 25;
                sendData[1] = (byte)(dac_out_ia_ref_enabled & 0xFF);
                sendData[2] = (byte)((dac_out_ia_ref_enabled & 0xFF00) >> 8);
                sendData[3] = (byte)(dac_dma_tim6_enabled & 0xFF);
                sendData[4] = (byte)((dac_dma_tim6_enabled & 0xFF00) >> 8);
                sendData[5] = (byte)(dac_dc_amp & 0xFF);
                sendData[6] = (byte)((dac_dc_amp & 0xFF00) >> 8);
                sendData[7] = (byte)(tim6_prescaler & 0xFF);
                sendData[8] = (byte)((tim6_prescaler & 0xFF00) >> 8);
                sendData[9] = (byte)(tim6_autoreload & 0xFF);
                sendData[10] = (byte)((tim6_autoreload & 0xFF00) >> 8);
                sendData[11] = (byte)(dac_freq1_amp & 0xFF);
                sendData[12] = (byte)((dac_freq1_amp & 0xFF00) >> 8);
                sendData[13] = (byte)(dac_freq1_npt & 0xFF);
                sendData[14] = (byte)((dac_freq1_npt & 0xFF00) >> 8);
                sendData[15] = (byte)(dac_freq2_amp & 0xFF);
                sendData[16] = (byte)((dac_freq2_amp & 0xFF00) >> 8);
                sendData[17] = (byte)(dac_freq2_npt & 0xFF);
                sendData[18] = (byte)((dac_freq2_npt & 0xFF00) >> 8);

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

        private void dac_out_ia_ref_enable_checkBox_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (dac_out_ia_ref_enable_checkBox.Checked)
                {
                    dac_out_ia_ref_enabled = 1;
                    dac_dc_amp = (short)(DAC_MAX_LEVEL/2);
                    this.ac_output_radioButton.Enabled = false;
                    this.dc_output_radioButton.Enabled = false;
                    this.dac_freq1_comboBox.Enabled = false;
                    this.dac_freq2_comboBox.Enabled = false;
                    this.dac_amp1_comboBox.Enabled = false;
                    this.dac_amp2_comboBox.Enabled = false;
                    this.ecg_button.Enabled = false;
                    this.dac_dc_amp_comboBox.Enabled = true;
                }
                else
                {
                    dac_out_ia_ref_enabled = 0;
                    dac_dc_amp = 0;
                    this.ac_output_radioButton.Enabled = true;
                    this.dc_output_radioButton.Enabled = true;
                    this.dac_freq1_comboBox.Enabled = true;
                    this.dac_freq2_comboBox.Enabled = true;
                    this.dac_amp1_comboBox.Enabled = true;
                    this.dac_amp2_comboBox.Enabled = true;
                    this.ecg_button.Enabled = true;
                    this.dac_dc_amp_comboBox.Enabled = true;
                }

                sendData[0] = 25;
                sendData[1] = (byte)(dac_out_ia_ref_enabled & 0xFF);
                sendData[2] = (byte)((dac_out_ia_ref_enabled & 0xFF00) >> 8);
                sendData[3] = (byte)(dac_dma_tim6_enabled & 0xFF);
                sendData[4] = (byte)((dac_dma_tim6_enabled & 0xFF00) >> 8);
                sendData[5] = (byte)(dac_dc_amp & 0xFF);
                sendData[6] = (byte)((dac_dc_amp & 0xFF00) >> 8);
                sendData[7] = (byte)(tim6_prescaler & 0xFF);
                sendData[8] = (byte)((tim6_prescaler & 0xFF00) >> 8);
                sendData[9] = (byte)(tim6_autoreload & 0xFF);
                sendData[10] = (byte)((tim6_autoreload & 0xFF00) >> 8);
                sendData[11] = (byte)(dac_freq1_amp & 0xFF);
                sendData[12] = (byte)((dac_freq1_amp & 0xFF00) >> 8);
                sendData[13] = (byte)(dac_freq1_npt & 0xFF);
                sendData[14] = (byte)((dac_freq1_npt & 0xFF00) >> 8);
                sendData[15] = (byte)(dac_freq2_amp & 0xFF);
                sendData[16] = (byte)((dac_freq2_amp & 0xFF00) >> 8);
                sendData[17] = (byte)(dac_freq2_npt & 0xFF);
                sendData[18] = (byte)((dac_freq2_npt & 0xFF00) >> 8);

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

        private void filter_config_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if ((sender == filter_scale_factor_comboBox) && (filter_scale_factor_comboBox.SelectedIndex != 0))
            {
                scale_factor = (short)Convert.ToUInt16(this.filter_scale_factor_comboBox.SelectedItem.ToString());
            }
            else if ((sender == filter_type_comboBox) && (filter_type_comboBox.SelectedIndex != 0))
            {
                filter_type = (short)this.filter_type_comboBox.SelectedIndex;
            }
        }

        private void SendDataString(string dataString, byte sendDataStringStartByte)
        {
            byte[] byteData = System.Text.Encoding.ASCII.GetBytes(dataString);//System.Text.Encoding.UTF8.GetBytes(dataString)
            int i = 0, j = sendDataStringStartByte;
            while (i < byteData.Length)
            {
                sendData[j] = byteData[i];
                i++;
                j++;
            }
            sendData[j] = 0;
        }

        private void ecg_lead_comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                if (sender == ecg_lead_comboBox)
                {
                    selected_ecg_lead = (byte)Convert.ToByte(this.ecg_lead_comboBox.SelectedIndex.ToString());
                    
                    sendData[0] = 25;
                    sendData[2] = selected_ecg_lead;

                    //MessageBox.Show("ECG Lead: " + selected_ecg_lead);
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

        private void STM32Primer2_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (ecg_button.Text.Equals(ecg_disable))
                {
                    sendData[0] = 26;
                    sendData[1] = 0;
                }
                else if (test_button.Text.Equals(test_disable))
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
    }
}

/*
 * LP, HP, BP, BS IIR Filter Design using MKFilter:
 * http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html
 */
/*
filtertype  =  Chebyshev  
passtype  =  Lowpass  
ripple  =  -1.0  
order  =  6  
samplerate  =  500  
corner1  =  40  

Ansi 'C' Code 
Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
Command line: /www/usr/fisher/helpers/mkfilter -Ch -1.0000000000e+00 -Lp -o 6 -a 8.0000000000e-02 0.0000000000e+00 -l

#define NZEROS 6
#define NPOLES 6
#define GAIN   7.042944789e+04

static float xv[NZEROS+1], yv[NPOLES+1];

static void filterloop()
  { for (;;)
      { xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = next input value / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[0] + xv[6]) + 6 * (xv[1] + xv[5]) + 15 * (xv[2] + xv[4])
                     + 20 * xv[3]
                     + ( -0.6274134206 * yv[0]) + (  3.8077318277 * yv[1])
                     + ( -9.8794904669 * yv[2]) + ( 14.0294765250 * yv[3])
                     + (-11.5082731440 * yv[4]) + (  5.1770599674 * yv[5]);
        next output value = yv[6];
      }
  }
*/
/*
filtertype  =  Butterworth  
passtype  =  Lowpass  
ripple  =   
order  =  6  
samplerate  =  500  
corner1  =  40  

Ansi 'C' Code 
Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 6 -a 8.0000000000e-02 0.0000000000e+00 -l
         
#define NZEROS 6
#define NPOLES 6
#define GAIN   9.339780497e+03

static float xv[NZEROS+1], yv[NPOLES+1];

static void filterloop()
  { for (;;)
      { xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = next input value / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[0] + xv[6]) + 6 * (xv[1] + xv[5]) + 15 * (xv[2] + xv[4])
                     + 20 * xv[3]
                     + ( -0.1396600417 * yv[0]) + (  1.1086708553 * yv[1])
                     + ( -3.7230194289 * yv[2]) + (  6.7850160254 * yv[3])
                     + ( -7.0995038188 * yv[4]) + (  4.0616439992 * yv[5]);
        next output value = yv[6];
      }
  }
*/
/*
filtertype  =  Butterworth  
passtype  =  Highpass  
ripple  =   
order  =  6  
samplerate  =  500  
corner1  =  10  

Ansi ``C'' Code 
Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
Command line: /www/usr/fisher/helpers/mkfilter -Bu -Hp -o 6 -a 2.0000000000e-02 0.0000000000e+00 -l

#define NZEROS 6
#define NPOLES 6
#define GAIN   1.275025803e+00

static float xv[NZEROS+1], yv[NPOLES+1];

static void filterloop()
  { for (;;)
      { xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = next input value / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[0] + xv[6]) - 6 * (xv[1] + xv[5]) + 15 * (xv[2] + xv[4])
                     - 20 * xv[3]
                     + ( -0.6151231220 * yv[0]) + (  3.9893594042 * yv[1])
                     + (-10.7932966700 * yv[2]) + ( 15.5936352110 * yv[3])
                     + (-12.6891130570 * yv[4]) + (  5.5145351212 * yv[5]);
        next output value = yv[6];
      }
  }
*/
/*
filtertype  =  Butterworth  
passtype  =  Bandpass  
ripple  =   
order  =  3  
samplerate  =  500  
corner1  =  10  
corner2  =  20  

Ansi ``C'' Code 
Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
Command line: /www/usr/fisher/helpers/mkfilter -Bu -Bp -o 3 -a 2.0000000000e-02 4.0000000000e-02 -l

#define NZEROS 6
#define NPOLES 6
#define GAIN   4.553559752e+03

static float xv[NZEROS+1], yv[NPOLES+1];

static void filterloop()
  { for (;;)
      { xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = next input value / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[6] - xv[0]) + 3 * (xv[2] - xv[4])
                     + ( -0.7776385602 * yv[0]) + (  4.7844837093 * yv[1])
                     + (-12.3491952600 * yv[2]) + ( 17.1138599760 * yv[3])
                     + (-13.4297107700 * yv[4]) + (  5.6581732343 * yv[5]);
        next output value = yv[6];
      }
  }
*/
/*
filtertype  =  Butterworth  
passtype  =  Bandstop  
ripple  =   
order  =  3  
samplerate  =  500  
corner1  =  45  
corner2  =  55  

Ansi ``C'' Code 
Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
Command line: /www/usr/fisher/helpers/mkfilter -Bu -Bs -o 3 -a 9.0000000000e-02 1.1000000000e-01 -l

#define NZEROS 6
#define NPOLES 6
#define GAIN   1.133994877e+00

static float xv[NZEROS+1], yv[NPOLES+1];

static void filterloop()
  { for (;;)
      { xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; 
        xv[6] = next input value / GAIN;
        yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; 
        yv[6] =   (xv[0] + xv[6]) -   4.8636993659 * (xv[1] + xv[5]) +  10.8851905070 * (xv[2] + xv[4])
                     -  13.9886427400 * xv[3]
                     + ( -0.7776385602 * yv[0]) + (  3.9405261824 * yv[1])
                     + ( -9.1900435716 * yv[2]) + ( 12.3130835930 * yv[3])
                     + ( -9.9939478418 * yv[4]) + (  4.6601015134 * yv[5]);
        next output value = yv[6];
      }
  }
*/