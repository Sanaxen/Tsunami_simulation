using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    static class Program
    {
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Form1 form = new Form1();
            if ( args.Length > 0 ) form.targetDir = args[0];
            if (args.Length > 1)
            {
                if (int.Parse(args[1]) != 0 ) form.checkBox2.Checked = true;
            }
            if (args.Length > 2) 
            {
                if (int.Parse(args[2]) != 0 ) form.checkBox3.Checked = true;
            }
            form.checkBox2.Refresh();
            form.checkBox3.Refresh();
            Application.Run(form);
        }
    }
}
