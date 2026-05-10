using System;
using System.Drawing;
using System.Windows.Forms;

namespace Lab3Devyatkina.Dialog
{
	public class MainFormDevyatkina : Form
	{
		private readonly Button startBtn = new() { Text = "Start", Size = new Size(80, 35) };
		private readonly Button stopBtn = new() { Text = "Stop", Size = new Size(80, 35) };
		private readonly Button sendBtn = new() { Text = "Send", Size = new Size(100, 35) };

		private readonly NumericUpDown threadsUpDown = new() { Minimum = 1, Maximum = 64, Value = 1 };
		private readonly TextBox textBox = new() { Width = 320 };
		private readonly ComboBox threadsBox = new() { DropDownStyle = ComboBoxStyle.DropDownList, Width = 200 };

		public MainFormDevyatkina()
		{
			Text = "Lab3 Devyatkina";
			FormBorderStyle = FormBorderStyle.Sizable;
			MaximizeBox = true;
			StartPosition = FormStartPosition.CenterScreen;
			ClientSize = new Size(800, 500);
			MinimumSize = new Size(600, 400);

			var lblThreads = new Label { Text = "Threads:", AutoSize = true };
			var lblText = new Label { Text = "Text:", AutoSize = true };
			var lblTo = new Label { Text = "To:", AutoSize = true };

			lblThreads.Location = new Point(12, 16);
			threadsUpDown.Location = new Point(80, 12);
			startBtn.Location = new Point(200, 10);
			stopBtn.Location = new Point(290, 10);

			lblTo.Location = new Point(12, 56);
			threadsBox.Location = new Point(80, 52);

			lblText.Location = new Point(12, 120);
			textBox.Location = new Point(80, 116);
			textBox.Width = 500;
			sendBtn.Location = new Point(600, 114);

			Controls.Add(lblThreads);
			Controls.Add(threadsUpDown);
			Controls.Add(startBtn);
			Controls.Add(stopBtn);
			Controls.Add(lblTo);
			Controls.Add(threadsBox);
			Controls.Add(lblText);
			Controls.Add(textBox);
			Controls.Add(sendBtn);

			Load += MainFormDevyatkina_Load;
			FormClosing += MainFormDevyatkina_FormClosing;
			threadsBox.DropDown += ThreadsBox_DropDown;

			startBtn.Click += StartBtn_Click;
			stopBtn.Click += StopBtn_Click;
			sendBtn.Click += SendBtn_Click;
		}

		private void MainFormDevyatkina_Load(object? sender, EventArgs e)
		{
			var n = TransportDllDevyatkina.Devyatkina_Connect();
			if (n < 0)
			{
				MessageBox.Show("Server is not running or connection failed.");
				FillThreadsBox(0);
				return;
			}
			FillThreadsBox(n);
			threadsBox.SelectedIndex = 0;
		}

		private void ThreadsBox_DropDown(object? sender, EventArgs e)
		{
			var n = TransportDllDevyatkina.Devyatkina_RequestThreadCount();
			if (n < 0)
			{
				// Try to reconnect and retry
				if (TryReconnect())
				{
					n = TransportDllDevyatkina.Devyatkina_RequestThreadCount();
					if (n >= 0)
						FillThreadsBox(n);
				}
			}
			else
			{
				FillThreadsBox(n);
			}
		}

		private void StartBtn_Click(object? sender, EventArgs e)
		{
			var n = TransportDllDevyatkina.Devyatkina_StartThreads((int)threadsUpDown.Value);
			if (n < 0)
			{
				// Try to reconnect and retry
				if (TryReconnect())
				{
					n = TransportDllDevyatkina.Devyatkina_StartThreads((int)threadsUpDown.Value);
					if (n < 0)
					{
						MessageBox.Show("Server is not running or connection failed.");
						return;
					}
				}
				else
				{
					MessageBox.Show("Server is not running or connection failed.");
					return;
				}
			}
			FillThreadsBox(n);
		}

		private void StopBtn_Click(object? sender, EventArgs e)
		{
			var n = TransportDllDevyatkina.Devyatkina_StopThread();
			if (n < 0)
			{
				// Try to reconnect and retry
				if (TryReconnect())
				{
					n = TransportDllDevyatkina.Devyatkina_StopThread();
					if (n < 0)
					{
						MessageBox.Show("Server is not running or connection failed.");
						return;
					}
				}
				else
				{
					MessageBox.Show("Server is not running or connection failed.");
					return;
				}
			}
			FillThreadsBox(n);
			if (n == 0)
				TransportDllDevyatkina.Devyatkina_Disconnect();
		}

		private void SendBtn_Click(object? sender, EventArgs e)
		{
			if (threadsBox.SelectedIndex < 0)
				return;
			if (string.IsNullOrWhiteSpace(textBox.Text))
				return;

			var to = GetSelectedTo();
			var ok = TransportDllDevyatkina.Devyatkina_SendText(to, textBox.Text) != 0;
			if (!ok)
			{
				MessageBox.Show("Send failed - please check connection");
			}
		}

		private void MainFormDevyatkina_FormClosing(object? sender, FormClosingEventArgs e)
		{
			try
			{
				TransportDllDevyatkina.Devyatkina_Disconnect();
			}
			catch
			{
			}
		}

		private int GetSelectedTo()
		{
			// 0: All threads, 1: Main thread, 2..: worker threads
			if (threadsBox.SelectedIndex == 0)
				return -1;
			if (threadsBox.SelectedIndex == 1)
				return 0;
			return threadsBox.SelectedIndex - 1;
		}

		private void FillThreadsBox(int nWorkers)
		{
			var selected = threadsBox.SelectedIndex;
			threadsBox.Items.Clear();

			threadsBox.Items.Add("All threads");
			threadsBox.Items.Add("Main thread");
			for (int i = 1; i <= nWorkers; ++i)
				threadsBox.Items.Add($"Thread {i}");

			if (threadsBox.Items.Count > 0)
				threadsBox.SelectedIndex = Math.Clamp(selected, 0, threadsBox.Items.Count - 1);
		}

		private bool TryReconnect()
		{
			try
			{
				// Disconnect first
				TransportDllDevyatkina.Devyatkina_Disconnect();
				
				// Try to reconnect
				var result = TransportDllDevyatkina.Devyatkina_Connect();
				return result >= 0;
			}
			catch
			{
				return false;
			}
		}
	}
}

