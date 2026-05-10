using System.Diagnostics;

namespace Events.Sharp
{
    public partial class Form1 : Form
    {
        Process? childProcess = null;
        System.Threading.EventWaitHandle stopEvent = new EventWaitHandle(false, EventResetMode.ManualReset, "StopEvent");
        System.Threading.EventWaitHandle startEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StartEvent");
        System.Threading.EventWaitHandle confirmEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "ConfirmEvent");

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(childProcess == null || childProcess.HasExited)
            {
                childProcess = Process.Start("Events.exe");
            }
            else
            {
                startEvent.Set();
                confirmEvent.WaitOne();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (!(childProcess == null || childProcess.HasExited))
            {
                stopEvent.Set();
                confirmEvent.WaitOne();
            }
        }
    }
}
