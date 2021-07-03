using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace TideGUI
{
    public partial class Form1 : Form
    {
        public string targetDir = "";
        public Form1()
        {
            InitializeComponent();
            openFileDialog1.InitialDirectory = Path.GetDirectoryName(Application.ExecutablePath)+"\\Harmonic";
        }

 
        private void Plot(string plotfile)
        {
            //chart1.Series["Tide"].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;

            //chart1.Series["Tide"].BorderWidth = 2;
            //chart1.Series["Tide"].XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            //chart1.Series["Tide"].ChartArea = "ChartArea1";

            //chart1.ChartAreas["ChartArea1"].AxisX.Minimum = 0;
            //chart1.ChartAreas["ChartArea1"].AxisX.Maximum = 23;
            //chart1.ChartAreas["ChartArea1"].AxisX.Interval = 1;
            chart1.ChartAreas["ChartArea1"].AxisX.Title = "時刻";

            try
            {
                string last1="", last2="";
                using (StreamReader sr = new StreamReader(plotfile, Encoding.GetEncoding("Shift_JIS")))
                {
                    chart1.Series["Tide"].Points.Clear();
                    string line = "";
                    while ((line = sr.ReadLine()) != null)
                    {
                        // 区切りで分割して配列に格納する
                        string[] stArrayData = line.Split('\t');
                        string[] stArrayData2 = stArrayData[0].Split(' ');
                        string[] stArrayData3 = stArrayData2[2].Split(':');
                        chart1.Series["Tide"].Points.AddXY(stArrayData3[0] + ":" + stArrayData3[1], stArrayData[1]);
                        last1 = stArrayData3[0] + ":" + stArrayData3[1];
                        last2 = stArrayData[1];
                    }
                }
                while (chart1.Series["Tide"].Points.Count % 24 != 0)
                {
                    chart1.Series["Tide"].Points.AddXY(last1, last2);
                }
                chart1.ChartAreas["ChartArea1"].AxisX.Maximum = chart1.Series["Tide"].Points.Count;
                chart1.ChartAreas["ChartArea1"].AxisX.Interval = chart1.ChartAreas["ChartArea1"].AxisX.Maximum / 24;
            }catch( System.IO.FileNotFoundException ex)
            {
                return;
            }
            catch (System.IO.IOException ex)
            {
                return;
            }
         }

        private void button2_Click(object sender, EventArgs e)
        {
            Plot( targetDir + @"\sampling\sampling_point.dat");
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Plot(targetDir + @"\sampling\sampling_point.dat");
        }
    }
}
