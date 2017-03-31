/*****************************************************************************
* File    : @file
* Company : Yantronix
* Author  : Satish Nair
* Mailto  : satishgn77@gmail.com
*****************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using System.Windows.Forms.VisualStyles;

namespace Custom_HID
{
    public partial class TraceView : UserControl
    {
        //private Bitmap m_bmpRecordIndicator = Properties.Resources.RecordIndicatorWhite;
        private List<float> m_dataBuffer = new List<float>();
        private int m_bufferLength = 0;
        private int m_position = 0;
        private float m_dataMax = 0xFF;
        private float m_dataMin = 0;
        private float m_upperThreshold = 0;
        private float m_lowerThreshold = 0;
        private float m_yMaxMeasure = 1500f / 16.0f;                  // in mV
        private float m_yMinMeasure = -1500f / 16.0f;                 // in mV
        private Pen m_penTrace;
        private Color m_gridColor = Color.Green;
        private float m_traceWidth = 0.0f;
        private DateTime m_timeMark;
        //private int m_flushCount = 0;
        private float m_sampleRate = 100;                                   // 100 samples/sec
        private float m_fGridOffset = 1.0f;
        private Brush m_recordIndicatorBrush = Brushes.Red;
        private bool m_bRecordIndicator;
        private bool m_bShowGrid = true;
        private readonly Pen m_penGrid = new Pen(Color.LightGreen);
        private readonly Pen m_penThreshold = new Pen(Color.Yellow);

        public TraceView()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
            m_penTrace = new Pen(this.ForeColor, m_traceWidth);
            //m_penGrid = new Pen(this.m_gridColor, 0);
            m_penGrid.DashStyle = DashStyle.Dot;
            m_penThreshold.DashStyle = DashStyle.Dash;
        }

        public int BufferLength
        {
            get { return m_bufferLength; }
            set
            {
                m_bufferLength = value;
                m_dataBuffer = new List<float>(m_bufferLength);

                HorizontalScroll.LargeChange = m_bufferLength;
                HorizontalScroll.Maximum = m_length - m_bufferLength;
            }
        }

        public float SampleRate
        {
            get { return m_sampleRate; }
            set { m_sampleRate = value; }
        }

        public float Duration
        {
            get { return m_bufferLength / m_sampleRate; }
            //set { m_bufferLength = (int)(value * m_sampleRate + 0.5f); }
        }

        public int Position
        {
            get { return m_position; }
            set
            {
                m_position = value;
                HorizontalScroll.Value = m_position;
            }
        }

        public float PositionInSec
        {
            get { return m_position / m_sampleRate; }
            //set { m_position = (int)(value * m_sampleRate + 0.5f); }
        }

        private int m_length;

        public int Length
        {
            get { return m_length; }
            set
            {
                m_length = value;
                HorizontalScroll.Maximum = value - m_bufferLength;
            }
        }

        public float LengthInSec
        {
            get { return m_length / m_sampleRate; }
        }

        public bool ShowScrollBar
        {
            get { return HorizontalScroll.Visible; }
            set { HorizontalScroll.Visible = value; }
        }

        public float DataMax
        {
            get { return m_dataMax; }
            set { m_dataMax = value; }
        }

        public float DataMin
        {
            get { return m_dataMin; }
            set { m_dataMin = value; }
        }

        public float UpperThreshold
        {
            get { return m_upperThreshold; }
            set { m_upperThreshold = value; }
        }

        public float LowerThreshold
        {
            get { return m_lowerThreshold; }
            set { m_lowerThreshold = value; }
        }

        public override Color ForeColor
        {
            get
            {
                return base.ForeColor;
            }
            set
            {
                base.ForeColor = value;
                m_penTrace = new Pen(value, m_traceWidth);
            }
        }

        public Color GridColor
        {
            get { return m_gridColor; }
            set { m_gridColor = value; }
        }

        public Brush RecordIndicatorBrush
        {
            get { return m_recordIndicatorBrush; }
            set { m_recordIndicatorBrush = value; }
        }

        public void AddDataAtEnd(int[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(0, ItermOver(dataSegment.Length));
                foreach (int data in dataSegment)
                {
                    m_dataBuffer.Add((float)data);
                }
            }
        }

        public void AddDataAtEnd(short[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(0, ItermOver(dataSegment.Length));
                foreach (int data in dataSegment)
                {
                    m_dataBuffer.Add((float)data);
                }
            }
        }

        public void AddDataAtEnd(sbyte[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(0, ItermOver(dataSegment.Length));
                foreach (int data in dataSegment)
                {
                    m_dataBuffer.Add((float)data);
                }
            }
        }

        public void AddDataAtEnd(float[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(0, ItermOver(dataSegment.Length));
                m_dataBuffer.AddRange(dataSegment);
            }
        }

        public void AddDataAtBegin(int[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            int nOver = ItermOver(dataSegment.Length);
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(m_dataBuffer.Count - nOver, nOver);
                for (int i = dataSegment.Length - 1; i >= 0; --i)
                {
                    m_dataBuffer.Insert(0, (float)dataSegment[i]);
                }
            }
        }

        public void AddDataAtBegin(short[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            int nOver = ItermOver(dataSegment.Length);
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(m_dataBuffer.Count - nOver, nOver);
                for (int i = dataSegment.Length - 1; i >= 0; --i)
                {
                    m_dataBuffer.Insert(0, (float)dataSegment[i]);
                }
            }
        }

        public void AddDataAtBegin(sbyte[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            int nOver = ItermOver(dataSegment.Length);
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(m_dataBuffer.Count - nOver, nOver);
                for (int i = dataSegment.Length - 1; i >= 0; --i)
                {
                    m_dataBuffer.Insert(0, (float)dataSegment[i]);
                }
            }
        }

        public void AddDataAtBegin(float[] dataSegment)
        {
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            int nOver = ItermOver(dataSegment.Length);
            lock (m_dataBuffer)
            {
                m_dataBuffer.RemoveRange(m_dataBuffer.Count - nOver, nOver);
                m_dataBuffer.InsertRange(0, dataSegment);
            }
        }

        public void ReplaceDataAt(int indx, int[] dataSegment)
        {
            int i = indx;
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                foreach (int data in dataSegment)
                {
                    m_dataBuffer[i++] = (float)data;
                    if (i == m_bufferLength) i = 0;
                }
            }
        }

        public void ReplaceDataAt(int indx, short[] dataSegment)
        {
            int i = indx;
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                foreach (short data in dataSegment)
                {
                    m_dataBuffer[i++] = (float)data;
                    if (i == m_bufferLength) i = 0;
                }
            }
        }

        public void ReplaceDataAt(int indx, sbyte[] dataSegment)
        {
            int i = indx;
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                foreach (short data in dataSegment)
                {
                    m_dataBuffer[i++] = (float)data;
                    if (i == m_bufferLength) i = 0;
                }
            }
        }

        public void ReplaceDataAt(int indx, float[] dataSegment)
        {
            int i = indx;
            if (dataSegment.Length > m_bufferLength)
                throw new IndexOutOfRangeException();
            lock (m_dataBuffer)
            {
                foreach (float data in dataSegment)
                {
                    m_dataBuffer[i++] = data;
                    if (i == m_bufferLength) i = 0;
                }
            }
        }

        public void FillBuffer(float value)
        {
            lock (m_dataBuffer)
            {
                m_dataBuffer.Clear();
                for (int i = 0; i < m_bufferLength; ++i)
                {
                    m_dataBuffer.Add(value);
                }
            }
        }


        // Calculate number of item overflow if adding more data to the buffer
        private int ItermOver(int moreData)
        {
            int over = moreData - ((m_dataBuffer.Count < m_bufferLength) ? (m_bufferLength - m_dataBuffer.Count) : 0);
            if (over < 0) over = 0;
            return over;
        }

        protected override void OnLoad(EventArgs e)
        {
            HorizontalScroll.Maximum = m_length;
            HorizontalScroll.Enabled = true;
            //ShowScrollBar = true;
            base.OnLoad(e);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            int index;
            int height = ClientSize.Height; ;

            // Scale coordinate
            //g.ScaleTransform((float)this.Width / (float)m_bufferLength, -(float)this.Height / (float)(m_dataMax - m_dataMin));

            if (HorizontalScroll.Visible)
            {
                height -= AutoScrollMinSize.Height;
            }

            int drawingHeight = height - Padding.Vertical;
            int drawingBottom = drawingHeight + Padding.Top;
            // Draw grid
            if (m_bShowGrid)
            {
                float stepY = drawingHeight / 6.0f;
                //float y = stepY;
                float stepX = ClientSize.Width * m_sampleRate / m_bufferLength;
                //float x = stepX;
                float stepYMeasure = (m_yMaxMeasure - m_yMinMeasure) / 6;
                float yMeasure = m_yMaxMeasure;

                for (float y = Padding.Top; y < height; y += stepY)
                {
                    g.DrawLine(m_penGrid, 40, y, ClientSize.Width, y);
                    g.DrawString(yMeasure.ToString("F1") + " mV", Font, Brushes.LightGreen, 0, y - 6);
                    yMeasure -= stepYMeasure;
                }

                int sec = 1;
                float x = stepX * m_fGridOffset;
                for (; x < ClientSize.Width; x += stepX)
                {
                    g.DrawLine(m_penGrid, x, Padding.Top, x, drawingBottom);
                    g.DrawString(sec.ToString() + ".0\"", Font, Brushes.White, x - 10, height - 12);
                    ++sec;
                }
                g.DrawString(sec.ToString() + ".0\"", Font, Brushes.White, x - 20, height - 12);
            }

            // Draw trace
            if (m_dataBuffer.Count != 0)
            {
                PointF[] pts = new PointF[m_dataBuffer.Count];
                GraphicsContainer state = g.BeginContainer();
                g.TranslateTransform(0, Padding.Top);
                g.ScaleTransform((float)this.Width / (float)m_bufferLength, -(float)drawingHeight / (float)(m_dataMax - m_dataMin));
                g.TranslateTransform(0, -m_dataMax);

                // Draw Threshold lines
                g.DrawLine(m_penThreshold, 0, m_upperThreshold, ClientSize.Width * m_bufferLength / m_sampleRate, m_upperThreshold);
                g.DrawLine(m_penThreshold, 0, m_lowerThreshold, ClientSize.Width * m_bufferLength / m_sampleRate, m_lowerThreshold);

                index = 0;
                lock (m_dataBuffer)
                {
                    foreach (float data in m_dataBuffer)
                    {
                        pts[index].X = (float)index;
                        pts[index++].Y = data;
                    }
                }
                g.DrawLines(m_penTrace, pts);
                g.EndContainer(state);
            }

            if (m_bRecordIndicator)
            {
                g.ResetTransform();
                g.DrawString(m_timeMark.ToString("d [HH:mm:ss]"), this.Font, m_recordIndicatorBrush, ClientSize.Width - 70, 10);
                /*
                if (m_flushCount < 2)
                {
                    g.ResetTransform();
                    g.DrawImageUnscaled(m_bmpRecordIndicator, 20, 10);
                }
                if (++m_flushCount > 3)
                    m_flushCount = 0;
                 */
            }

            // Draw grid

            base.OnPaint(e);
        }

        protected override void OnPaintBackground(PaintEventArgs e)
        {
            //            if (Application.RenderWithVisualStyles)
            //            {
            //                VisualStyleRenderer render = new VisualStyleRenderer(VisualStyleElement.ExplorerBar.NormalGroupBackground.Normal);
            //                render.DrawBackground(e.Graphics, e.ClipRectangle);
            //            }
            //            else
            {
                base.OnPaintBackground(e);
            }
        }


        protected override void OnSizeChanged(EventArgs e)
        {
            Invalidate();
            base.OnSizeChanged(e);
        }

        public bool RecordIndictor
        {
            get { return m_bRecordIndicator; }
            set
            {
                m_bRecordIndicator = value;
                Invalidate();
            }
        }

        //public Bitmap RecordIndicatorBitmap
        //{
        //    get { return m_bmpRecordIndicator; }
        //    set { m_bmpRecordIndicator = value; }
        //}

        public DateTime TimeMark
        {
            get { return m_timeMark; }
            set { m_timeMark = value; }
        }

        public bool ShowGrid
        {
            get
            {
                return m_bShowGrid;
            }
            set
            {
                m_bShowGrid = value;
            }
        }

        public float GridOffset                     // in seconds
        {
            get { return m_fGridOffset; }
            set
            {
                if (value <= 0.0f)
                {
                    value = 1.0f;
                }
                m_fGridOffset = value;
            }
        }

        public float YMaxMeasure
        {
            get { return m_yMaxMeasure; }
            set { m_yMaxMeasure = value; }
        }

        public float YMinMeasure
        {
            get { return m_yMinMeasure; }
            set { m_yMinMeasure = value; }
        }
    }
}
