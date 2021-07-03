using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        public string targetDir = "";
        public int convPng = 0;
        public int autoclose = 0;

        string fontName = "";
        int retry;
        int count2;
        int count;
        int step;
        System.Drawing.Text.PrivateFontCollection pfc;

        public Form1()
        {
            count = 0;
            count2 = 1;
            retry = 0;
            step = 0;
            InitializeComponent();

            //PrivateFontCollectionオブジェクトを作成する
            pfc = new System.Drawing.Text.PrivateFontCollection();
            //PrivateFontCollectionにフォントを追加する
            pfc.AddFontFile(@"Font\7barSP.TTF");
            //同様にして、複数のフォントを追加できる
            //pfc.AddFontFile(@"C:\test\wlmaru20044u.ttf");

            //PrivateFontCollectionに追加されているフォントの名前を列挙する
            foreach (System.Drawing.FontFamily ff in pfc.Families)
            {
               fontName = ff.Name;
               break;
            }

            /*
            //PrivateFontCollectionの先頭のフォントのFontオブジェクトを作成する
            System.Drawing.Font f =
                new System.Drawing.Font(pfc.Families[0], 24);
            */
            //後始末
            //pfc.Dispose();
        }

        private bool draw()
        {
            if (step == 0) return false;
            string file = targetDir + "\\" + string.Format("image\\output_W{0,0:D6}.bmp", count);
            string file2 = targetDir + "\\" + string.Format("image\\output_W{0,0:D6}.bmp", count + step);
            string timefile = targetDir + "\\" + string.Format("output\\time{0,0:D6}.txt", count);

            if ( checkBox1.Checked)
            {
                file = targetDir + "\\" + string.Format("image3D\\output_{0,0:D6}.bmp", count);
                file2 = targetDir + "\\" + string.Format("image3D\\output_{0,0:D6}.bmp", count + step);
            }

            string f = targetDir + "\\" + string.Format("output\\Closed_{0,0:D6}", count);
            int endFlg = 0;
            if (System.IO.File.Exists(f) && checkBox3.Checked )
            {
                endFlg = 1;
                //Close();
            }



            if (System.IO.File.Exists(file) && System.IO.File.Exists(timefile) || endFlg == 1)
            {
                if (endFlg == 0 && !System.IO.File.Exists(file2))
                {
                    retry++;
                    if (retry * timer1.Interval / 1000 < 60 * 3)
                    {
                        return false;
                    }
                }

                retry = 0;
                Bitmap bmp = new Bitmap(file);
                if ( true )
                {
                    this.pictureBox1.Image = bmp;

                    pictureBox1.Refresh();

                    string text = "";
                    string copylighttext = "";

                    using (System.IO.StreamReader sr = new System.IO.StreamReader(
                        timefile, Encoding.GetEncoding("Shift_JIS")))
                    {
                        //text = sr.ReadToEnd();
                        text = sr.ReadLine();
                       // copylighttext = sr.ReadLine();
                    }

                    Bitmap saveImg = null;
                    Graphics gg = null;
                    if (!checkBox1.Checked && checkBox2.Checked)
                    {
                        saveImg = new Bitmap(this.pictureBox1.Width, this.pictureBox1.Height);
                        gg = Graphics.FromImage(saveImg);
                        gg.DrawImage(this.pictureBox1.Image, 0, 0, this.pictureBox1.Width, this.pictureBox1.Height);
                    }

                    using (Graphics g = pictureBox1.CreateGraphics())
                    {
                        if (fontName == "")
                        {
                            using (Font font = new Font("MS ゴシック", 24))
                            {
                                // PictureBoxのクライアント領域いっぱいを矩形とします
                                RectangleF rect = new RectangleF(pictureBox1.Width / 2 - pictureBox1.Width / 5, pictureBox1.Height - 120, pictureBox1.Width, pictureBox1.Height);
                                RectangleF rect2 = new RectangleF(0, pictureBox1.Height - 35, pictureBox1.Width, pictureBox1.Height);
                                //RectangleF rect = new RectangleF(0, 0, pictureBox1.Width, pictureBox1.Height);

                                g.DrawString(text, font, Brushes.DarkGray, rect);
                                if (gg != null) gg.DrawString(text, font, Brushes.DarkGray, rect2);
                            }
                        }
                        else
                        {
                            using (Font font = new Font(pfc.Families[0], 24))
                            {
                                // PictureBoxのクライアント領域いっぱいを矩形とします
                                RectangleF rect = new RectangleF(pictureBox1.Width / 2 - pictureBox1.Width / 5, pictureBox1.Height - 120, pictureBox1.Width, pictureBox1.Height);
                                RectangleF rect2 = new RectangleF(0, pictureBox1.Height-35, pictureBox1.Width, pictureBox1.Height);
                                //RectangleF rect = new RectangleF(0, 0, pictureBox1.Width, pictureBox1.Height);

                                g.DrawString(text, font, Brushes.DarkGray, rect);
                                if (gg != null) gg.DrawString(text, font, Brushes.DarkGray, rect2);
                            }
                        }
                    }

                    if (gg != null)
                    {
                        string savefile = targetDir + "\\" + string.Format("image\\output_W{0,0:D6}.png", count);

                        saveImg.Save(savefile, System.Drawing.Imaging.ImageFormat.Png);
                        saveImg.Dispose();
                        gg.Dispose();
                    }

                    if (count / step >= this.hScrollBar1.Maximum)
                    {
                        this.hScrollBar1.Maximum += 1000;
                    }
                    this.hScrollBar1.Value = count / step;

                    if (endFlg == 1)
                    {
                        Close();
                    }
                    return true;
                }
            }
            return false;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (step == 0) return;

            if (draw())
            {
                count += step;
                if (count / step >= this.hScrollBar1.Maximum)
                {
                    this.hScrollBar1.Maximum += 1000;
                }
                this.hScrollBar1.Value = count / step;
            }
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            if (step != 0) return;

            string file = targetDir + "\\" + string.Format("image\\output_W{0,0:D6}.bmp", 0);
            if (checkBox1.Checked)
            {
                file = targetDir + "\\" + string.Format("image3D\\output_{0,0:D6}.bmp", 0);
            }


            if (System.IO.File.Exists(file))
            {
                string file2 = targetDir + "\\" + string.Format("image\\output_W{0,0:D6}.bmp", count2);
                if (checkBox1.Checked)
                {
                    file2 = targetDir + "\\" + string.Format("image3D\\output_{0,0:D6}.bmp", count + count2);
                }
                if (System.IO.File.Exists(file2))
                {
                    step = count2;
                }
                else
                {
                    count2++;
                    if (count2 > 1000) count2 = 1;
                }
            }
        }

        private void hScrollBar1_ValueChanged(object sender, EventArgs e)
        {
            count = this.hScrollBar1.Value * step;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            timer1.Enabled = true;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            count = 0;
            this.hScrollBar1.Value = 0;
        }

        private void hScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            draw();
        }

        private void Form1_ResizeBegin(object sender, EventArgs e)
        {
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
        }

        private void Form1_ResizeEnd(object sender, EventArgs e)
        {
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
