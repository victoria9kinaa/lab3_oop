namespace GUI
{
    partial class MainForm
    {
        private System.ComponentModel.IContainer components = null;

        private System.Windows.Forms.Button btnStart = null!;
        private System.Windows.Forms.Button btnStop = null!;
        //private System.Windows.Forms.Button btnSend = null!;
        private System.Windows.Forms.NumericUpDown numericN = null!;
        //private System.Windows.Forms.TextBox txtMessage = null!;
        private System.Windows.Forms.ComboBox cmbThreads = null!;
        private System.Windows.Forms.TextBox txtLog = null!;
        private System.Windows.Forms.Label lblN = null!;
        private System.Windows.Forms.Label lblThreads = null!;
        //private System.Windows.Forms.Label lblMessage = null!;
        private System.Windows.Forms.Label lblLog = null!;
        private System.Windows.Forms.GroupBox grpControls = null!;
        private System.Windows.Forms.GroupBox grpThreads = null!;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
                components.Dispose();
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            grpControls = new System.Windows.Forms.GroupBox();
            lblN = new System.Windows.Forms.Label();
            numericN = new System.Windows.Forms.NumericUpDown();
            btnStart = new System.Windows.Forms.Button();
            btnStop = new System.Windows.Forms.Button();
            //lblMessage = new System.Windows.Forms.Label();
            //txtMessage = new System.Windows.Forms.TextBox();
            //btnSend = new System.Windows.Forms.Button();
            grpThreads = new System.Windows.Forms.GroupBox();
            lblThreads = new System.Windows.Forms.Label();
            cmbThreads = new System.Windows.Forms.ComboBox();
            lblLog = new System.Windows.Forms.Label();
            txtLog = new System.Windows.Forms.TextBox();

            grpControls.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)numericN).BeginInit();
            grpThreads.SuspendLayout();
            SuspendLayout();

            // ── grpControls ──────────────────────────
            grpControls.Text = "Управление";
            grpControls.Location = new System.Drawing.Point(12, 12);
            grpControls.Size = new System.Drawing.Size(400, 220);
            grpControls.Font = new System.Drawing.Font("Segoe UI", 10F);
            grpControls.TabIndex = 0;
            grpControls.TabStop = false;
            grpControls.Controls.Add(lblN);
            grpControls.Controls.Add(numericN);
            grpControls.Controls.Add(btnStart);
            grpControls.Controls.Add(btnStop);
            //grpControls.Controls.Add(lblMessage);
           // grpControls.Controls.Add(txtMessage);
            //grpControls.Controls.Add(btnSend);

            lblN.Text = "Количество потоков (N):";
            lblN.Location = new System.Drawing.Point(12, 32);
            lblN.Size = new System.Drawing.Size(200, 24);
            lblN.TabIndex = 0;

            numericN.Location = new System.Drawing.Point(220, 30);
            numericN.Size = new System.Drawing.Size(80, 27);
            numericN.Minimum = 1;
            numericN.Maximum = 20;
            numericN.Value = 1;
            numericN.TabIndex = 1;

            btnStart.Text = "Start";
            btnStart.Location = new System.Drawing.Point(12, 72);
            btnStart.Size = new System.Drawing.Size(110, 36);
            btnStart.TabIndex = 2;
            btnStart.UseVisualStyleBackColor = true;
            btnStart.Click += btnStart_Click;

            btnStop.Text = "Stop";
            btnStop.Location = new System.Drawing.Point(140, 72);
            btnStop.Size = new System.Drawing.Size(110, 36);
            btnStop.TabIndex = 3;
            btnStop.UseVisualStyleBackColor = true;
            btnStop.Click += btnStop_Click;

            //lblMessage.Text = "Сообщение:";
            //lblMessage.Location = new System.Drawing.Point(12, 130);
            //lblMessage.Size = new System.Drawing.Size(120, 24);
            //lblMessage.TabIndex = 4;

            ////txtMessage.Location = new System.Drawing.Point(12, 158);
            //txtMessage.Size = new System.Drawing.Size(260, 27);
            //txtMessage.TabIndex = 5;

            //btnSend.Text = "Send";
            //btnSend.Location = new System.Drawing.Point(285, 155);
            //btnSend.Size = new System.Drawing.Size(100, 36);
           // btnSend.TabIndex = 6;
           // btnSend.UseVisualStyleBackColor = true;
            //btnSend.Click += btnSend_Click;

            // ── grpThreads ───────────────────────────
            grpThreads.Text = "Активные потоки";
            grpThreads.Location = new System.Drawing.Point(430, 12);
            grpThreads.Size = new System.Drawing.Size(400, 220);
            grpThreads.Font = new System.Drawing.Font("Segoe UI", 10F);
            grpThreads.TabIndex = 1;
            grpThreads.TabStop = false;
            grpThreads.Controls.Add(lblThreads);
            grpThreads.Controls.Add(cmbThreads);

            lblThreads.Text = "Список потоков:";
            lblThreads.Location = new System.Drawing.Point(12, 32);
            lblThreads.Size = new System.Drawing.Size(200, 24);
            lblThreads.TabIndex = 0;

            cmbThreads.Location = new System.Drawing.Point(12, 62);
            cmbThreads.Size = new System.Drawing.Size(360, 30);
            cmbThreads.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            cmbThreads.TabIndex = 1;

            // ── Log ──────────────────────────────────
            lblLog.Text = "Лог событий:";
            lblLog.Location = new System.Drawing.Point(12, 248);
            lblLog.Size = new System.Drawing.Size(150, 24);
            lblLog.Font = new System.Drawing.Font("Segoe UI", 10F);
            lblLog.TabIndex = 2;

            txtLog.Location = new System.Drawing.Point(12, 275);
            txtLog.Size = new System.Drawing.Size(818, 290);
            txtLog.Multiline = true;
            txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            txtLog.ReadOnly = true;
            txtLog.BackColor = System.Drawing.Color.White;
            txtLog.Font = new System.Drawing.Font("Consolas", 9.5F);
            txtLog.TabIndex = 3;

            // ── MainForm ─────────────────────────────
            AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            ClientSize = new System.Drawing.Size(844, 580);
            MinimumSize = new System.Drawing.Size(860, 620);
            Font = new System.Drawing.Font("Segoe UI", 10F);
            StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            Text = "Lab IPC — Devyatkina";
            Name = "MainForm";
            Controls.Add(grpControls);
            Controls.Add(grpThreads);
            Controls.Add(lblLog);
            Controls.Add(txtLog);
            Load += MainForm_Load;

            grpControls.ResumeLayout(false);
            grpControls.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)numericN).EndInit();
            grpThreads.ResumeLayout(false);
            ResumeLayout(false);
            PerformLayout();
        }
    }
}
