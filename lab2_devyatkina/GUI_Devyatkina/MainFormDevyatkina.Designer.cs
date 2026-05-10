namespace GUI_Devyatkina
{
    partial class MainFormDevyatkina
    {
        private System.ComponentModel.IContainer components = null;
        private Button btnStart = null!;
        private Button btnStop = null!;
        private Button btnSend = null!;
        private NumericUpDown numericN = null!;
        private ComboBox cmbThreads = null!;
        private TextBox txtMessage = null!;
        private TextBox txtLog = null!;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            btnStart = new Button();
            btnStop = new Button();
            btnSend = new Button();
            numericN = new NumericUpDown();
            cmbThreads = new ComboBox();
            txtMessage = new TextBox();
            txtLog = new TextBox();
            ((System.ComponentModel.ISupportInitialize)numericN).BeginInit();
            SuspendLayout();

            btnStart.Location = new System.Drawing.Point(20, 20);
            btnStart.Size = new System.Drawing.Size(120, 35);
            btnStart.Text = "Start";
            btnStart.Click += btnStart_Click;

            btnStop.Location = new System.Drawing.Point(160, 20);
            btnStop.Size = new System.Drawing.Size(120, 35);
            btnStop.Text = "Stop";
            btnStop.Click += btnStop_Click;

            numericN.Location = new System.Drawing.Point(300, 24);
            numericN.Minimum = 1;
            numericN.Maximum = 20;
            numericN.Value = 1;
            numericN.Size = new System.Drawing.Size(80, 23);

            cmbThreads.DropDownStyle = ComboBoxStyle.DropDownList;
            cmbThreads.Location = new System.Drawing.Point(20, 70);
            cmbThreads.Size = new System.Drawing.Size(360, 23);

            txtMessage.Location = new System.Drawing.Point(20, 105);
            txtMessage.Size = new System.Drawing.Size(520, 23);

            btnSend.Location = new System.Drawing.Point(560, 100);
            btnSend.Size = new System.Drawing.Size(120, 32);
            btnSend.Text = "Send";
            btnSend.Click += btnSend_Click;

            txtLog.Location = new System.Drawing.Point(20, 150);
            txtLog.Multiline = true;
            txtLog.ScrollBars = ScrollBars.Vertical;
            txtLog.ReadOnly = true;
            txtLog.Size = new System.Drawing.Size(660, 320);

            ClientSize = new System.Drawing.Size(700, 500);
            Controls.Add(btnStart);
            Controls.Add(btnStop);
            Controls.Add(numericN);
            Controls.Add(cmbThreads);
            Controls.Add(txtMessage);
            Controls.Add(btnSend);
            Controls.Add(txtLog);
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Lab2 GUI Devyatkina";

            ((System.ComponentModel.ISupportInitialize)numericN).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }
    }
}
