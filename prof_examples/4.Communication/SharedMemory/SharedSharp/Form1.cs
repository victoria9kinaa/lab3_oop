using System.Text;
using System.Runtime.InteropServices;

namespace SharedSharp
{
    public partial class Form1 : Form
    {
        [DllImport("SharedDll.dll", CharSet = CharSet.Unicode)]
        static extern void GetSharedString(StringBuilder s);

        [DllImport("SharedDll.dll")]
        static extern void SetHwnd(nint hWnd);

        public Form1()
        {
            InitializeComponent();
            SetHwnd(this.Handle);
        }

        private void Form1_Activated(object sender, EventArgs e)
        {
            var s = new StringBuilder(1024);
            GetSharedString(s);
            textBox.Text = s.ToString();
        }
    }
}
