/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

namespace Custom_HID
{
    partial class YantronixForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ecg_lead_comboBox = new System.Windows.Forms.ComboBox();
            this.ecg_lead_label = new System.Windows.Forms.Label();
            this.play_log_button = new System.Windows.Forms.Button();
            this.record_log_button = new System.Windows.Forms.Button();
            this.test_button = new System.Windows.Forms.Button();
            this.lower_threshold_label = new System.Windows.Forms.Label();
            this.lower_threshold_comboBox = new System.Windows.Forms.ComboBox();
            this.dfu_button = new System.Windows.Forms.Button();
            this.hr_button = new System.Windows.Forms.Button();
            this.upper_threshold_comboBox = new System.Windows.Forms.ComboBox();
            this.hr_label = new System.Windows.Forms.Label();
            this.upper_threshold_label = new System.Windows.Forms.Label();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.filter_button = new System.Windows.Forms.Button();
            this.iir_mk_filter_button = new System.Windows.Forms.Button();
            this.filter_type_comboBox = new System.Windows.Forms.ComboBox();
            this.fft_points_label = new System.Windows.Forms.Label();
            this.filter_type_label = new System.Windows.Forms.Label();
            this.filter_scale_factor_comboBox = new System.Windows.Forms.ComboBox();
            this.fft_points_comboBox = new System.Windows.Forms.ComboBox();
            this.filter_scale_factor_label = new System.Windows.Forms.Label();
            this.fft_button = new System.Windows.Forms.Button();
            this.iir_biquad_filter_button = new System.Windows.Forms.Button();
            this.iir_arma_filter_button = new System.Windows.Forms.Button();
            this.fir_filter_button = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.sampling_rate_multiplier_label = new System.Windows.Forms.Label();
            this.buffer_length_comboBox = new System.Windows.Forms.ComboBox();
            this.buffer_length_label = new System.Windows.Forms.Label();
            this.oversampling_enable_checkBox = new System.Windows.Forms.CheckBox();
            this.adc_channel_comboBox = new System.Windows.Forms.ComboBox();
            this.adc_channel_label = new System.Windows.Forms.Label();
            this.adc_samp_freq_comboBox = new System.Windows.Forms.ComboBox();
            this.adc_sampling_freq_label = new System.Windows.Forms.Label();
            this.adc_button = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.dac_out_ia_ref_enable_checkBox = new System.Windows.Forms.CheckBox();
            this.dc_output_radioButton = new System.Windows.Forms.RadioButton();
            this.ac_output_radioButton = new System.Windows.Forms.RadioButton();
            this.dac_dc_amp_label = new System.Windows.Forms.Label();
            this.dac_dc_amp_comboBox = new System.Windows.Forms.ComboBox();
            this.dac_freq2_label = new System.Windows.Forms.Label();
            this.dac_amp2_label = new System.Windows.Forms.Label();
            this.dac_freq1_comboBox = new System.Windows.Forms.ComboBox();
            this.dac_amp2_comboBox = new System.Windows.Forms.ComboBox();
            this.dac_freq1_label = new System.Windows.Forms.Label();
            this.dac_amp1_comboBox = new System.Windows.Forms.ComboBox();
            this.dac_freq2_comboBox = new System.Windows.Forms.ComboBox();
            this.dac_amp1_label = new System.Windows.Forms.Label();
            this.dac_button = new System.Windows.Forms.Button();
            this.ecg_button = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.right_key_pictureBox = new System.Windows.Forms.PictureBox();
            this.left_key_pictureBox = new System.Windows.Forms.PictureBox();
            this.down_key_pictureBox = new System.Windows.Forms.PictureBox();
            this.up_key_pictureBox = new System.Windows.Forms.PictureBox();
            this.led_button = new System.Windows.Forms.Button();
            this.shutdown_button = new System.Windows.Forms.Button();
            this.received_message_label = new System.Windows.Forms.Label();
            this.received_message_listBox = new System.Windows.Forms.ListBox();
            this.traceView1 = new Custom_HID.TraceView();
            this.groupBox1.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.right_key_pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.left_key_pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.down_key_pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.up_key_pictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.ecg_lead_comboBox);
            this.groupBox1.Controls.Add(this.ecg_lead_label);
            this.groupBox1.Controls.Add(this.play_log_button);
            this.groupBox1.Controls.Add(this.record_log_button);
            this.groupBox1.Controls.Add(this.test_button);
            this.groupBox1.Controls.Add(this.lower_threshold_label);
            this.groupBox1.Controls.Add(this.lower_threshold_comboBox);
            this.groupBox1.Controls.Add(this.dfu_button);
            this.groupBox1.Controls.Add(this.hr_button);
            this.groupBox1.Controls.Add(this.upper_threshold_comboBox);
            this.groupBox1.Controls.Add(this.hr_label);
            this.groupBox1.Controls.Add(this.upper_threshold_label);
            this.groupBox1.Controls.Add(this.groupBox5);
            this.groupBox1.Controls.Add(this.groupBox4);
            this.groupBox1.Controls.Add(this.groupBox3);
            this.groupBox1.Controls.Add(this.ecg_button);
            this.groupBox1.Controls.Add(this.groupBox2);
            this.groupBox1.Controls.Add(this.led_button);
            this.groupBox1.Controls.Add(this.shutdown_button);
            this.groupBox1.Controls.Add(this.traceView1);
            this.groupBox1.Controls.Add(this.received_message_label);
            this.groupBox1.Controls.Add(this.received_message_listBox);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(911, 562);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Device Details";
            // 
            // ecg_lead_comboBox
            // 
            this.ecg_lead_comboBox.Enabled = false;
            this.ecg_lead_comboBox.FormattingEnabled = true;
            this.ecg_lead_comboBox.Location = new System.Drawing.Point(653, 321);
            this.ecg_lead_comboBox.Name = "ecg_lead_comboBox";
            this.ecg_lead_comboBox.Size = new System.Drawing.Size(73, 21);
            this.ecg_lead_comboBox.TabIndex = 45;
            this.ecg_lead_comboBox.SelectedIndexChanged += new System.EventHandler(this.ecg_lead_comboBox_SelectedIndexChanged);
            // 
            // ecg_lead_label
            // 
            this.ecg_lead_label.AutoSize = true;
            this.ecg_lead_label.Location = new System.Drawing.Point(598, 324);
            this.ecg_lead_label.Name = "ecg_lead_label";
            this.ecg_lead_label.Size = new System.Drawing.Size(59, 13);
            this.ecg_lead_label.TabIndex = 44;
            this.ecg_lead_label.Text = "ECG Lead:";
            // 
            // play_log_button
            // 
            this.play_log_button.Enabled = false;
            this.play_log_button.Location = new System.Drawing.Point(600, 219);
            this.play_log_button.Name = "play_log_button";
            this.play_log_button.Size = new System.Drawing.Size(129, 23);
            this.play_log_button.TabIndex = 43;
            this.play_log_button.Text = "PLAY";
            this.play_log_button.UseVisualStyleBackColor = true;
            this.play_log_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // record_log_button
            // 
            this.record_log_button.Enabled = false;
            this.record_log_button.Location = new System.Drawing.Point(600, 194);
            this.record_log_button.Name = "record_log_button";
            this.record_log_button.Size = new System.Drawing.Size(129, 23);
            this.record_log_button.TabIndex = 42;
            this.record_log_button.Text = "RECORD";
            this.record_log_button.UseVisualStyleBackColor = true;
            this.record_log_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // test_button
            // 
            this.test_button.Enabled = false;
            this.test_button.Location = new System.Drawing.Point(600, 269);
            this.test_button.Name = "test_button";
            this.test_button.Size = new System.Drawing.Size(129, 23);
            this.test_button.TabIndex = 41;
            this.test_button.Text = "TEST";
            this.test_button.UseVisualStyleBackColor = true;
            this.test_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // lower_threshold_label
            // 
            this.lower_threshold_label.AutoSize = true;
            this.lower_threshold_label.Location = new System.Drawing.Point(665, 376);
            this.lower_threshold_label.Name = "lower_threshold_label";
            this.lower_threshold_label.Size = new System.Drawing.Size(23, 13);
            this.lower_threshold_label.TabIndex = 23;
            this.lower_threshold_label.Text = "LT:";
            // 
            // lower_threshold_comboBox
            // 
            this.lower_threshold_comboBox.Enabled = false;
            this.lower_threshold_comboBox.FormattingEnabled = true;
            this.lower_threshold_comboBox.Location = new System.Drawing.Point(688, 373);
            this.lower_threshold_comboBox.Name = "lower_threshold_comboBox";
            this.lower_threshold_comboBox.Size = new System.Drawing.Size(38, 21);
            this.lower_threshold_comboBox.TabIndex = 24;
            this.lower_threshold_comboBox.SelectedIndexChanged += new System.EventHandler(this.threshold_comboBox_SelectedIndexChanged);
            // 
            // dfu_button
            // 
            this.dfu_button.Enabled = false;
            this.dfu_button.Location = new System.Drawing.Point(600, 244);
            this.dfu_button.Name = "dfu_button";
            this.dfu_button.Size = new System.Drawing.Size(129, 23);
            this.dfu_button.TabIndex = 40;
            this.dfu_button.Text = "DFU";
            this.dfu_button.UseVisualStyleBackColor = true;
            this.dfu_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // hr_button
            // 
            this.hr_button.Enabled = false;
            this.hr_button.Location = new System.Drawing.Point(600, 346);
            this.hr_button.Name = "hr_button";
            this.hr_button.Size = new System.Drawing.Size(129, 23);
            this.hr_button.TabIndex = 39;
            this.hr_button.Text = "HR";
            this.hr_button.UseVisualStyleBackColor = true;
            this.hr_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // upper_threshold_comboBox
            // 
            this.upper_threshold_comboBox.Enabled = false;
            this.upper_threshold_comboBox.FormattingEnabled = true;
            this.upper_threshold_comboBox.Location = new System.Drawing.Point(626, 373);
            this.upper_threshold_comboBox.Name = "upper_threshold_comboBox";
            this.upper_threshold_comboBox.Size = new System.Drawing.Size(38, 21);
            this.upper_threshold_comboBox.TabIndex = 22;
            this.upper_threshold_comboBox.SelectedIndexChanged += new System.EventHandler(this.threshold_comboBox_SelectedIndexChanged);
            // 
            // hr_label
            // 
            this.hr_label.BackColor = System.Drawing.Color.Black;
            this.hr_label.Font = new System.Drawing.Font("Microsoft Sans Serif", 15F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.hr_label.ForeColor = System.Drawing.Color.Yellow;
            this.hr_label.Location = new System.Drawing.Point(457, 502);
            this.hr_label.Name = "hr_label";
            this.hr_label.Size = new System.Drawing.Size(122, 24);
            this.hr_label.TabIndex = 37;
            this.hr_label.Text = "0  bpm";
            this.hr_label.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.hr_label.Visible = false;
            // 
            // upper_threshold_label
            // 
            this.upper_threshold_label.AutoSize = true;
            this.upper_threshold_label.Location = new System.Drawing.Point(601, 376);
            this.upper_threshold_label.Name = "upper_threshold_label";
            this.upper_threshold_label.Size = new System.Drawing.Size(25, 13);
            this.upper_threshold_label.TabIndex = 21;
            this.upper_threshold_label.Text = "UT:";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.filter_button);
            this.groupBox5.Controls.Add(this.iir_mk_filter_button);
            this.groupBox5.Controls.Add(this.filter_type_comboBox);
            this.groupBox5.Controls.Add(this.fft_points_label);
            this.groupBox5.Controls.Add(this.filter_type_label);
            this.groupBox5.Controls.Add(this.filter_scale_factor_comboBox);
            this.groupBox5.Controls.Add(this.fft_points_comboBox);
            this.groupBox5.Controls.Add(this.filter_scale_factor_label);
            this.groupBox5.Controls.Add(this.fft_button);
            this.groupBox5.Controls.Add(this.iir_biquad_filter_button);
            this.groupBox5.Controls.Add(this.iir_arma_filter_button);
            this.groupBox5.Controls.Add(this.fir_filter_button);
            this.groupBox5.Location = new System.Drawing.Point(600, 399);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(298, 147);
            this.groupBox5.TabIndex = 35;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = " DSP";
            // 
            // filter_button
            // 
            this.filter_button.Enabled = false;
            this.filter_button.Location = new System.Drawing.Point(158, 115);
            this.filter_button.Name = "filter_button";
            this.filter_button.Size = new System.Drawing.Size(132, 23);
            this.filter_button.TabIndex = 26;
            this.filter_button.Text = "APPLY HOST FILTER";
            this.filter_button.UseVisualStyleBackColor = true;
            this.filter_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // iir_mk_filter_button
            // 
            this.iir_mk_filter_button.Enabled = false;
            this.iir_mk_filter_button.Location = new System.Drawing.Point(158, 90);
            this.iir_mk_filter_button.Name = "iir_mk_filter_button";
            this.iir_mk_filter_button.Size = new System.Drawing.Size(132, 23);
            this.iir_mk_filter_button.TabIndex = 25;
            this.iir_mk_filter_button.Text = "IIR MK";
            this.iir_mk_filter_button.UseVisualStyleBackColor = true;
            this.iir_mk_filter_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // filter_type_comboBox
            // 
            this.filter_type_comboBox.Enabled = false;
            this.filter_type_comboBox.FormattingEnabled = true;
            this.filter_type_comboBox.Location = new System.Drawing.Point(70, 41);
            this.filter_type_comboBox.Name = "filter_type_comboBox";
            this.filter_type_comboBox.Size = new System.Drawing.Size(220, 21);
            this.filter_type_comboBox.TabIndex = 29;
            this.filter_type_comboBox.SelectedIndexChanged += new System.EventHandler(this.filter_config_comboBox_SelectedIndexChanged);
            // 
            // fft_points_label
            // 
            this.fft_points_label.AutoSize = true;
            this.fft_points_label.Location = new System.Drawing.Point(156, 19);
            this.fft_points_label.Name = "fft_points_label";
            this.fft_points_label.Size = new System.Drawing.Size(60, 13);
            this.fft_points_label.TabIndex = 19;
            this.fft_points_label.Text = "FFT points:";
            // 
            // filter_type_label
            // 
            this.filter_type_label.AutoSize = true;
            this.filter_type_label.Location = new System.Drawing.Point(7, 44);
            this.filter_type_label.Name = "filter_type_label";
            this.filter_type_label.Size = new System.Drawing.Size(59, 13);
            this.filter_type_label.TabIndex = 28;
            this.filter_type_label.Text = "Filter Type:";
            // 
            // filter_scale_factor_comboBox
            // 
            this.filter_scale_factor_comboBox.Enabled = false;
            this.filter_scale_factor_comboBox.FormattingEnabled = true;
            this.filter_scale_factor_comboBox.Location = new System.Drawing.Point(242, 67);
            this.filter_scale_factor_comboBox.Name = "filter_scale_factor_comboBox";
            this.filter_scale_factor_comboBox.Size = new System.Drawing.Size(47, 21);
            this.filter_scale_factor_comboBox.TabIndex = 27;
            this.filter_scale_factor_comboBox.SelectedIndexChanged += new System.EventHandler(this.filter_config_comboBox_SelectedIndexChanged);
            // 
            // fft_points_comboBox
            // 
            this.fft_points_comboBox.Enabled = false;
            this.fft_points_comboBox.FormattingEnabled = true;
            this.fft_points_comboBox.Location = new System.Drawing.Point(219, 16);
            this.fft_points_comboBox.Name = "fft_points_comboBox";
            this.fft_points_comboBox.Size = new System.Drawing.Size(71, 21);
            this.fft_points_comboBox.TabIndex = 20;
            this.fft_points_comboBox.SelectedIndexChanged += new System.EventHandler(this.fft_points_comboBox_SelectedIndexChanged);
            // 
            // filter_scale_factor_label
            // 
            this.filter_scale_factor_label.AutoSize = true;
            this.filter_scale_factor_label.Location = new System.Drawing.Point(169, 70);
            this.filter_scale_factor_label.Name = "filter_scale_factor_label";
            this.filter_scale_factor_label.Size = new System.Drawing.Size(70, 13);
            this.filter_scale_factor_label.TabIndex = 26;
            this.filter_scale_factor_label.Text = "Scale Factor:";
            // 
            // fft_button
            // 
            this.fft_button.Enabled = false;
            this.fft_button.Location = new System.Drawing.Point(6, 14);
            this.fft_button.Name = "fft_button";
            this.fft_button.Size = new System.Drawing.Size(131, 23);
            this.fft_button.TabIndex = 18;
            this.fft_button.Text = "FFT";
            this.fft_button.UseVisualStyleBackColor = true;
            this.fft_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // iir_biquad_filter_button
            // 
            this.iir_biquad_filter_button.Enabled = false;
            this.iir_biquad_filter_button.Location = new System.Drawing.Point(6, 115);
            this.iir_biquad_filter_button.Name = "iir_biquad_filter_button";
            this.iir_biquad_filter_button.Size = new System.Drawing.Size(131, 23);
            this.iir_biquad_filter_button.TabIndex = 24;
            this.iir_biquad_filter_button.Text = "IIR BIQUAD";
            this.iir_biquad_filter_button.UseVisualStyleBackColor = true;
            this.iir_biquad_filter_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // iir_arma_filter_button
            // 
            this.iir_arma_filter_button.Enabled = false;
            this.iir_arma_filter_button.Location = new System.Drawing.Point(6, 90);
            this.iir_arma_filter_button.Name = "iir_arma_filter_button";
            this.iir_arma_filter_button.Size = new System.Drawing.Size(131, 23);
            this.iir_arma_filter_button.TabIndex = 23;
            this.iir_arma_filter_button.Text = "IIR ARMA";
            this.iir_arma_filter_button.UseVisualStyleBackColor = true;
            this.iir_arma_filter_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // fir_filter_button
            // 
            this.fir_filter_button.Enabled = false;
            this.fir_filter_button.Location = new System.Drawing.Point(6, 65);
            this.fir_filter_button.Name = "fir_filter_button";
            this.fir_filter_button.Size = new System.Drawing.Size(131, 23);
            this.fir_filter_button.TabIndex = 21;
            this.fir_filter_button.Text = "FIR";
            this.fir_filter_button.UseVisualStyleBackColor = true;
            this.fir_filter_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.sampling_rate_multiplier_label);
            this.groupBox4.Controls.Add(this.buffer_length_comboBox);
            this.groupBox4.Controls.Add(this.buffer_length_label);
            this.groupBox4.Controls.Add(this.oversampling_enable_checkBox);
            this.groupBox4.Controls.Add(this.adc_channel_comboBox);
            this.groupBox4.Controls.Add(this.adc_channel_label);
            this.groupBox4.Controls.Add(this.adc_samp_freq_comboBox);
            this.groupBox4.Controls.Add(this.adc_sampling_freq_label);
            this.groupBox4.Controls.Add(this.adc_button);
            this.groupBox4.Location = new System.Drawing.Point(735, 254);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(163, 146);
            this.groupBox4.TabIndex = 34;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "ADC Control";
            // 
            // sampling_rate_multiplier_label
            // 
            this.sampling_rate_multiplier_label.AutoSize = true;
            this.sampling_rate_multiplier_label.Location = new System.Drawing.Point(88, 71);
            this.sampling_rate_multiplier_label.Name = "sampling_rate_multiplier_label";
            this.sampling_rate_multiplier_label.Size = new System.Drawing.Size(30, 13);
            this.sampling_rate_multiplier_label.TabIndex = 22;
            this.sampling_rate_multiplier_label.Text = "SR x";
            // 
            // buffer_length_comboBox
            // 
            this.buffer_length_comboBox.Enabled = false;
            this.buffer_length_comboBox.FormattingEnabled = true;
            this.buffer_length_comboBox.Location = new System.Drawing.Point(118, 68);
            this.buffer_length_comboBox.Name = "buffer_length_comboBox";
            this.buffer_length_comboBox.Size = new System.Drawing.Size(38, 21);
            this.buffer_length_comboBox.TabIndex = 21;
            this.buffer_length_comboBox.SelectedIndexChanged += new System.EventHandler(this.adc_config_comboBox_SelectedIndexChanged);
            // 
            // buffer_length_label
            // 
            this.buffer_length_label.AutoSize = true;
            this.buffer_length_label.Location = new System.Drawing.Point(6, 71);
            this.buffer_length_label.Name = "buffer_length_label";
            this.buffer_length_label.Size = new System.Drawing.Size(74, 13);
            this.buffer_length_label.TabIndex = 20;
            this.buffer_length_label.Text = "Buffer Length:";
            // 
            // oversampling_enable_checkBox
            // 
            this.oversampling_enable_checkBox.AutoSize = true;
            this.oversampling_enable_checkBox.Enabled = false;
            this.oversampling_enable_checkBox.Location = new System.Drawing.Point(9, 95);
            this.oversampling_enable_checkBox.Name = "oversampling_enable_checkBox";
            this.oversampling_enable_checkBox.Size = new System.Drawing.Size(126, 17);
            this.oversampling_enable_checkBox.TabIndex = 19;
            this.oversampling_enable_checkBox.Text = "Oversampling Enable";
            this.oversampling_enable_checkBox.UseVisualStyleBackColor = true;
            this.oversampling_enable_checkBox.CheckedChanged += new System.EventHandler(this.oversampling_enable_checkBox_CheckedChanged);
            // 
            // adc_channel_comboBox
            // 
            this.adc_channel_comboBox.Enabled = false;
            this.adc_channel_comboBox.FormattingEnabled = true;
            this.adc_channel_comboBox.Location = new System.Drawing.Point(94, 18);
            this.adc_channel_comboBox.Name = "adc_channel_comboBox";
            this.adc_channel_comboBox.Size = new System.Drawing.Size(62, 21);
            this.adc_channel_comboBox.TabIndex = 18;
            this.adc_channel_comboBox.SelectedIndexChanged += new System.EventHandler(this.adc_config_comboBox_SelectedIndexChanged);
            // 
            // adc_channel_label
            // 
            this.adc_channel_label.AutoSize = true;
            this.adc_channel_label.Location = new System.Drawing.Point(6, 21);
            this.adc_channel_label.Name = "adc_channel_label";
            this.adc_channel_label.Size = new System.Drawing.Size(74, 13);
            this.adc_channel_label.TabIndex = 17;
            this.adc_channel_label.Text = "ADC Channel:";
            // 
            // adc_samp_freq_comboBox
            // 
            this.adc_samp_freq_comboBox.Enabled = false;
            this.adc_samp_freq_comboBox.FormattingEnabled = true;
            this.adc_samp_freq_comboBox.Location = new System.Drawing.Point(94, 43);
            this.adc_samp_freq_comboBox.Name = "adc_samp_freq_comboBox";
            this.adc_samp_freq_comboBox.Size = new System.Drawing.Size(62, 21);
            this.adc_samp_freq_comboBox.TabIndex = 16;
            this.adc_samp_freq_comboBox.SelectedIndexChanged += new System.EventHandler(this.adc_config_comboBox_SelectedIndexChanged);
            // 
            // adc_sampling_freq_label
            // 
            this.adc_sampling_freq_label.AutoSize = true;
            this.adc_sampling_freq_label.Location = new System.Drawing.Point(6, 46);
            this.adc_sampling_freq_label.Name = "adc_sampling_freq_label";
            this.adc_sampling_freq_label.Size = new System.Drawing.Size(79, 13);
            this.adc_sampling_freq_label.TabIndex = 15;
            this.adc_sampling_freq_label.Text = "Sampling Rate:";
            // 
            // adc_button
            // 
            this.adc_button.Enabled = false;
            this.adc_button.Location = new System.Drawing.Point(9, 117);
            this.adc_button.Name = "adc_button";
            this.adc_button.Size = new System.Drawing.Size(146, 23);
            this.adc_button.TabIndex = 11;
            this.adc_button.Text = "ADC";
            this.adc_button.UseVisualStyleBackColor = true;
            this.adc_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.dac_out_ia_ref_enable_checkBox);
            this.groupBox3.Controls.Add(this.dc_output_radioButton);
            this.groupBox3.Controls.Add(this.ac_output_radioButton);
            this.groupBox3.Controls.Add(this.dac_dc_amp_label);
            this.groupBox3.Controls.Add(this.dac_dc_amp_comboBox);
            this.groupBox3.Controls.Add(this.dac_freq2_label);
            this.groupBox3.Controls.Add(this.dac_amp2_label);
            this.groupBox3.Controls.Add(this.dac_freq1_comboBox);
            this.groupBox3.Controls.Add(this.dac_amp2_comboBox);
            this.groupBox3.Controls.Add(this.dac_freq1_label);
            this.groupBox3.Controls.Add(this.dac_amp1_comboBox);
            this.groupBox3.Controls.Add(this.dac_freq2_comboBox);
            this.groupBox3.Controls.Add(this.dac_amp1_label);
            this.groupBox3.Controls.Add(this.dac_button);
            this.groupBox3.Location = new System.Drawing.Point(735, 26);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(163, 226);
            this.groupBox3.TabIndex = 33;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "DAC Control";
            // 
            // dac_out_ia_ref_enable_checkBox
            // 
            this.dac_out_ia_ref_enable_checkBox.AutoSize = true;
            this.dac_out_ia_ref_enable_checkBox.Enabled = false;
            this.dac_out_ia_ref_enable_checkBox.Location = new System.Drawing.Point(9, 172);
            this.dac_out_ia_ref_enable_checkBox.Name = "dac_out_ia_ref_enable_checkBox";
            this.dac_out_ia_ref_enable_checkBox.Size = new System.Drawing.Size(146, 17);
            this.dac_out_ia_ref_enable_checkBox.TabIndex = 38;
            this.dac_out_ia_ref_enable_checkBox.Text = "DAC Out -- IA Ref Enable";
            this.dac_out_ia_ref_enable_checkBox.UseVisualStyleBackColor = true;
            this.dac_out_ia_ref_enable_checkBox.CheckedChanged += new System.EventHandler(this.dac_out_ia_ref_enable_checkBox_CheckedChanged);
            // 
            // dc_output_radioButton
            // 
            this.dc_output_radioButton.AutoSize = true;
            this.dc_output_radioButton.Enabled = false;
            this.dc_output_radioButton.Location = new System.Drawing.Point(85, 20);
            this.dc_output_radioButton.Name = "dc_output_radioButton";
            this.dc_output_radioButton.Size = new System.Drawing.Size(75, 17);
            this.dc_output_radioButton.TabIndex = 37;
            this.dc_output_radioButton.TabStop = true;
            this.dc_output_radioButton.Text = "DC Output";
            this.dc_output_radioButton.UseVisualStyleBackColor = true;
            this.dc_output_radioButton.CheckedChanged += new System.EventHandler(this.dac_output_radioButton_CheckedChanged);
            // 
            // ac_output_radioButton
            // 
            this.ac_output_radioButton.AutoSize = true;
            this.ac_output_radioButton.Enabled = false;
            this.ac_output_radioButton.Location = new System.Drawing.Point(11, 20);
            this.ac_output_radioButton.Name = "ac_output_radioButton";
            this.ac_output_radioButton.Size = new System.Drawing.Size(74, 17);
            this.ac_output_radioButton.TabIndex = 36;
            this.ac_output_radioButton.TabStop = true;
            this.ac_output_radioButton.Text = "AC Output";
            this.ac_output_radioButton.UseVisualStyleBackColor = true;
            this.ac_output_radioButton.CheckedChanged += new System.EventHandler(this.dac_output_radioButton_CheckedChanged);
            // 
            // dac_dc_amp_label
            // 
            this.dac_dc_amp_label.AutoSize = true;
            this.dac_dc_amp_label.Location = new System.Drawing.Point(9, 147);
            this.dac_dc_amp_label.Name = "dac_dc_amp_label";
            this.dac_dc_amp_label.Size = new System.Drawing.Size(85, 13);
            this.dac_dc_amp_label.TabIndex = 35;
            this.dac_dc_amp_label.Text = "   DC Amp. (mV):";
            // 
            // dac_dc_amp_comboBox
            // 
            this.dac_dc_amp_comboBox.Enabled = false;
            this.dac_dc_amp_comboBox.FormattingEnabled = true;
            this.dac_dc_amp_comboBox.Location = new System.Drawing.Point(95, 144);
            this.dac_dc_amp_comboBox.Name = "dac_dc_amp_comboBox";
            this.dac_dc_amp_comboBox.Size = new System.Drawing.Size(61, 21);
            this.dac_dc_amp_comboBox.TabIndex = 34;
            this.dac_dc_amp_comboBox.SelectedIndexChanged += new System.EventHandler(this.dac_config_comboBox_SelectedIndexChanged);
            // 
            // dac_freq2_label
            // 
            this.dac_freq2_label.AutoSize = true;
            this.dac_freq2_label.Location = new System.Drawing.Point(6, 98);
            this.dac_freq2_label.Name = "dac_freq2_label";
            this.dac_freq2_label.Size = new System.Drawing.Size(88, 13);
            this.dac_freq2_label.TabIndex = 30;
            this.dac_freq2_label.Text = "Frequency 2(Hz):";
            // 
            // dac_amp2_label
            // 
            this.dac_amp2_label.AutoSize = true;
            this.dac_amp2_label.Location = new System.Drawing.Point(9, 122);
            this.dac_amp2_label.Name = "dac_amp2_label";
            this.dac_amp2_label.Size = new System.Drawing.Size(86, 13);
            this.dac_amp2_label.TabIndex = 32;
            this.dac_amp2_label.Text = "Amplitude 2(mV):";
            // 
            // dac_freq1_comboBox
            // 
            this.dac_freq1_comboBox.Enabled = false;
            this.dac_freq1_comboBox.FormattingEnabled = true;
            this.dac_freq1_comboBox.Location = new System.Drawing.Point(94, 44);
            this.dac_freq1_comboBox.Name = "dac_freq1_comboBox";
            this.dac_freq1_comboBox.Size = new System.Drawing.Size(61, 21);
            this.dac_freq1_comboBox.TabIndex = 13;
            this.dac_freq1_comboBox.SelectedIndexChanged += new System.EventHandler(this.dac_config_comboBox_SelectedIndexChanged);
            // 
            // dac_amp2_comboBox
            // 
            this.dac_amp2_comboBox.Enabled = false;
            this.dac_amp2_comboBox.FormattingEnabled = true;
            this.dac_amp2_comboBox.Location = new System.Drawing.Point(95, 119);
            this.dac_amp2_comboBox.Name = "dac_amp2_comboBox";
            this.dac_amp2_comboBox.Size = new System.Drawing.Size(61, 21);
            this.dac_amp2_comboBox.TabIndex = 31;
            this.dac_amp2_comboBox.SelectedIndexChanged += new System.EventHandler(this.dac_config_comboBox_SelectedIndexChanged);
            // 
            // dac_freq1_label
            // 
            this.dac_freq1_label.AutoSize = true;
            this.dac_freq1_label.Location = new System.Drawing.Point(5, 48);
            this.dac_freq1_label.Name = "dac_freq1_label";
            this.dac_freq1_label.Size = new System.Drawing.Size(88, 13);
            this.dac_freq1_label.TabIndex = 14;
            this.dac_freq1_label.Text = "Frequency 1(Hz):";
            // 
            // dac_amp1_comboBox
            // 
            this.dac_amp1_comboBox.Enabled = false;
            this.dac_amp1_comboBox.FormattingEnabled = true;
            this.dac_amp1_comboBox.Location = new System.Drawing.Point(94, 69);
            this.dac_amp1_comboBox.Name = "dac_amp1_comboBox";
            this.dac_amp1_comboBox.Size = new System.Drawing.Size(61, 21);
            this.dac_amp1_comboBox.TabIndex = 27;
            this.dac_amp1_comboBox.SelectedIndexChanged += new System.EventHandler(this.dac_config_comboBox_SelectedIndexChanged);
            // 
            // dac_freq2_comboBox
            // 
            this.dac_freq2_comboBox.Enabled = false;
            this.dac_freq2_comboBox.FormattingEnabled = true;
            this.dac_freq2_comboBox.Location = new System.Drawing.Point(95, 94);
            this.dac_freq2_comboBox.Name = "dac_freq2_comboBox";
            this.dac_freq2_comboBox.Size = new System.Drawing.Size(61, 21);
            this.dac_freq2_comboBox.TabIndex = 29;
            this.dac_freq2_comboBox.SelectedIndexChanged += new System.EventHandler(this.dac_config_comboBox_SelectedIndexChanged);
            // 
            // dac_amp1_label
            // 
            this.dac_amp1_label.AutoSize = true;
            this.dac_amp1_label.Location = new System.Drawing.Point(8, 72);
            this.dac_amp1_label.Name = "dac_amp1_label";
            this.dac_amp1_label.Size = new System.Drawing.Size(86, 13);
            this.dac_amp1_label.TabIndex = 28;
            this.dac_amp1_label.Text = "Amplitude 1(mV):";
            // 
            // dac_button
            // 
            this.dac_button.Enabled = false;
            this.dac_button.Location = new System.Drawing.Point(8, 195);
            this.dac_button.Name = "dac_button";
            this.dac_button.Size = new System.Drawing.Size(147, 23);
            this.dac_button.TabIndex = 12;
            this.dac_button.Text = "DAC";
            this.dac_button.UseVisualStyleBackColor = true;
            this.dac_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // ecg_button
            // 
            this.ecg_button.Enabled = false;
            this.ecg_button.Location = new System.Drawing.Point(600, 294);
            this.ecg_button.Name = "ecg_button";
            this.ecg_button.Size = new System.Drawing.Size(129, 23);
            this.ecg_button.TabIndex = 22;
            this.ecg_button.Text = "ECG";
            this.ecg_button.UseVisualStyleBackColor = true;
            this.ecg_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.right_key_pictureBox);
            this.groupBox2.Controls.Add(this.left_key_pictureBox);
            this.groupBox2.Controls.Add(this.down_key_pictureBox);
            this.groupBox2.Controls.Add(this.up_key_pictureBox);
            this.groupBox2.Location = new System.Drawing.Point(600, 26);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(129, 115);
            this.groupBox2.TabIndex = 17;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Key Status";
            // 
            // right_key_pictureBox
            // 
            this.right_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;
            this.right_key_pictureBox.Location = new System.Drawing.Point(81, 49);
            this.right_key_pictureBox.Name = "right_key_pictureBox";
            this.right_key_pictureBox.Size = new System.Drawing.Size(25, 25);
            this.right_key_pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.right_key_pictureBox.TabIndex = 3;
            this.right_key_pictureBox.TabStop = false;
            // 
            // left_key_pictureBox
            // 
            this.left_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;
            this.left_key_pictureBox.Location = new System.Drawing.Point(24, 49);
            this.left_key_pictureBox.Name = "left_key_pictureBox";
            this.left_key_pictureBox.Size = new System.Drawing.Size(25, 25);
            this.left_key_pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.left_key_pictureBox.TabIndex = 2;
            this.left_key_pictureBox.TabStop = false;
            // 
            // down_key_pictureBox
            // 
            this.down_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;
            this.down_key_pictureBox.Location = new System.Drawing.Point(53, 78);
            this.down_key_pictureBox.Name = "down_key_pictureBox";
            this.down_key_pictureBox.Size = new System.Drawing.Size(25, 25);
            this.down_key_pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.down_key_pictureBox.TabIndex = 1;
            this.down_key_pictureBox.TabStop = false;
            // 
            // up_key_pictureBox
            // 
            this.up_key_pictureBox.Image = global::Custom_HID.Properties.Resources.KeyOff;
            this.up_key_pictureBox.Location = new System.Drawing.Point(53, 20);
            this.up_key_pictureBox.Name = "up_key_pictureBox";
            this.up_key_pictureBox.Size = new System.Drawing.Size(25, 25);
            this.up_key_pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.up_key_pictureBox.TabIndex = 0;
            this.up_key_pictureBox.TabStop = false;
            // 
            // led_button
            // 
            this.led_button.Enabled = false;
            this.led_button.Location = new System.Drawing.Point(600, 169);
            this.led_button.Name = "led_button";
            this.led_button.Size = new System.Drawing.Size(129, 23);
            this.led_button.TabIndex = 10;
            this.led_button.Text = "LED";
            this.led_button.UseVisualStyleBackColor = true;
            this.led_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // shutdown_button
            // 
            this.shutdown_button.Enabled = false;
            this.shutdown_button.Location = new System.Drawing.Point(600, 144);
            this.shutdown_button.Name = "shutdown_button";
            this.shutdown_button.Size = new System.Drawing.Size(129, 23);
            this.shutdown_button.TabIndex = 9;
            this.shutdown_button.Text = "SHUTDOWN";
            this.shutdown_button.UseVisualStyleBackColor = true;
            this.shutdown_button.Click += new System.EventHandler(this.button_Click_Action);
            // 
            // received_message_label
            // 
            this.received_message_label.AutoSize = true;
            this.received_message_label.Location = new System.Drawing.Point(12, 20);
            this.received_message_label.Name = "received_message_label";
            this.received_message_label.Size = new System.Drawing.Size(107, 13);
            this.received_message_label.TabIndex = 5;
            this.received_message_label.Text = "Received Messages:";
            // 
            // received_message_listBox
            // 
            this.received_message_listBox.FormattingEnabled = true;
            this.received_message_listBox.HorizontalScrollbar = true;
            this.received_message_listBox.Location = new System.Drawing.Point(15, 36);
            this.received_message_listBox.Name = "received_message_listBox";
            this.received_message_listBox.Size = new System.Drawing.Size(575, 199);
            this.received_message_listBox.TabIndex = 0;
            // 
            // traceView1
            // 
            this.traceView1.BackColor = System.Drawing.Color.Black;
            this.traceView1.BufferLength = 0;
            this.traceView1.DataMax = 255F;
            this.traceView1.DataMin = 0F;
            this.traceView1.ForeColor = System.Drawing.Color.Lime;
            this.traceView1.GridColor = System.Drawing.Color.Green;
            this.traceView1.GridOffset = 1F;
            this.traceView1.Length = 0;
            this.traceView1.Location = new System.Drawing.Point(15, 250);
            this.traceView1.LowerThreshold = 0F;
            this.traceView1.Name = "traceView1";
            this.traceView1.Padding = new System.Windows.Forms.Padding(0, 6, 0, 6);
            this.traceView1.Position = 0;
            this.traceView1.RecordIndictor = false;
            this.traceView1.SampleRate = 100F;
            this.traceView1.ShowGrid = true;
            this.traceView1.ShowScrollBar = false;
            this.traceView1.Size = new System.Drawing.Size(575, 296);
            this.traceView1.TabIndex = 8;
            this.traceView1.TimeMark = new System.DateTime(((long)(0)));
            this.traceView1.UpperThreshold = 0F;
            this.traceView1.YMaxMeasure = 93.75F;
            this.traceView1.YMinMeasure = -93.75F;
            // 
            // STM32Primer2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(935, 586);
            this.Controls.Add(this.groupBox1);
            this.MaximizeBox = false;
            this.Name = "STM32Primer2";
            this.Text = "YANTRONIX-HID";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.STM32Primer2_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.right_key_pictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.left_key_pictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.down_key_pictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.up_key_pictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ListBox received_message_listBox;
        private System.Windows.Forms.Label received_message_label;
        private TraceView traceView1;
        private System.Windows.Forms.Button shutdown_button;
        private System.Windows.Forms.Button led_button;
        private System.Windows.Forms.Button adc_button;
        private System.Windows.Forms.Button dac_button;
        private System.Windows.Forms.Label dac_freq1_label;
        private System.Windows.Forms.ComboBox dac_freq1_comboBox;
        private System.Windows.Forms.Label adc_sampling_freq_label;
        private System.Windows.Forms.ComboBox adc_samp_freq_comboBox;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.PictureBox up_key_pictureBox;
        private System.Windows.Forms.PictureBox left_key_pictureBox;
        private System.Windows.Forms.PictureBox down_key_pictureBox;
        private System.Windows.Forms.PictureBox right_key_pictureBox;
        private System.Windows.Forms.Button fft_button;
        private System.Windows.Forms.ComboBox fft_points_comboBox;
        private System.Windows.Forms.Label fft_points_label;
        private System.Windows.Forms.Button fir_filter_button;
        private System.Windows.Forms.Button ecg_button;
        private System.Windows.Forms.Button iir_biquad_filter_button;
        private System.Windows.Forms.Button iir_arma_filter_button;
        private System.Windows.Forms.Button iir_mk_filter_button;
        private System.Windows.Forms.Button filter_button;
        private System.Windows.Forms.Label dac_amp1_label;
        private System.Windows.Forms.ComboBox dac_amp1_comboBox;
        private System.Windows.Forms.Label dac_amp2_label;
        private System.Windows.Forms.ComboBox dac_amp2_comboBox;
        private System.Windows.Forms.Label dac_freq2_label;
        private System.Windows.Forms.ComboBox dac_freq2_comboBox;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.ComboBox adc_channel_comboBox;
        private System.Windows.Forms.Label adc_channel_label;
        private System.Windows.Forms.CheckBox oversampling_enable_checkBox;
        private System.Windows.Forms.ComboBox upper_threshold_comboBox;
        private System.Windows.Forms.Label upper_threshold_label;
        private System.Windows.Forms.Label hr_label;
        private System.Windows.Forms.Label dac_dc_amp_label;
        private System.Windows.Forms.ComboBox dac_dc_amp_comboBox;
        private System.Windows.Forms.RadioButton ac_output_radioButton;
        private System.Windows.Forms.RadioButton dc_output_radioButton;
        private System.Windows.Forms.Label filter_scale_factor_label;
        private System.Windows.Forms.ComboBox filter_scale_factor_comboBox;
        private System.Windows.Forms.ComboBox filter_type_comboBox;
        private System.Windows.Forms.Label filter_type_label;
        private System.Windows.Forms.CheckBox dac_out_ia_ref_enable_checkBox;
        private System.Windows.Forms.ComboBox buffer_length_comboBox;
        private System.Windows.Forms.Label buffer_length_label;
        private System.Windows.Forms.Label sampling_rate_multiplier_label;
        private System.Windows.Forms.Button hr_button;
        private System.Windows.Forms.Button dfu_button;
        private System.Windows.Forms.Button test_button;
        private System.Windows.Forms.Button play_log_button;
        private System.Windows.Forms.Button record_log_button;
        private System.Windows.Forms.ComboBox lower_threshold_comboBox;
        private System.Windows.Forms.Label lower_threshold_label;
        private System.Windows.Forms.ComboBox ecg_lead_comboBox;
        private System.Windows.Forms.Label ecg_lead_label;
    }
}

