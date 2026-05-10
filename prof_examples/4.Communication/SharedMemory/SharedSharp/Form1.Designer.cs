namespace SharedSharp
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            textBox = new TextBox();
            SuspendLayout();
            // 
            // textBox
            // 
            textBox.Location = new Point(12, 12);
            textBox.Name = "textBox";
            textBox.ReadOnly = true;
            textBox.Size = new Size(260, 23);
            textBox.TabIndex = 0;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(284, 61);
            Controls.Add(textBox);
            MaximumSize = new Size(300, 100);
            MinimumSize = new Size(300, 100);
            Name = "Form1";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "SharedSharp";
            Activated += Form1_Activated;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private TextBox textBox;
    }
}
