using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Threading;
using System.Windows.Forms;

namespace Lab4Devyatkina.Dialog
{
	public class MainFormDevyatkina : Form
	{
		private readonly Button connectBtn = new() { Text = "Connect", Size = new Size(110, 35) };
		private readonly Button disconnectBtn = new() { Text = "Disconnect", Size = new Size(110, 35) };
		private readonly Button sendBtn = new() { Text = "Send", Size = new Size(90, 35) };

		private readonly TextBox outgoingBox = new() { Multiline = true, Height = 90, Width = 320 };
		private readonly TextBox incomingBox = new() { Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical };
		private readonly ComboBox recipientsBox = new() { DropDownStyle = ComboBoxStyle.DropDownList, Width = 250 };
		private readonly CheckBox connectedCheck = new() { Text = "Connected", AutoSize = true, Enabled = false };
		private readonly Label clientLabel = new() { AutoSize = true };

		private Thread? receiveThread;
		private volatile bool isConnected;
		private readonly List<int> recipientIds = new();

		public MainFormDevyatkina()
		{
			Text = "Lab4 Devyatkina";
			FormBorderStyle = FormBorderStyle.Sizable;
			StartPosition = FormStartPosition.CenterScreen;
			ClientSize = new Size(820, 520);
			MinimumSize = new Size(700, 450);

			var lblRecipients = new Label { Text = "To:", AutoSize = true };
			var lblOutgoing = new Label { Text = "Message:", AutoSize = true };
			var lblIncoming = new Label { Text = "Incoming:", AutoSize = true };
			var lblClient = new Label { Text = "Client ID:", AutoSize = true };

			lblIncoming.Location = new Point(16, 14);
			incomingBox.Location = new Point(16, 34);
			incomingBox.Size = new Size(320, 430);

			lblOutgoing.Location = new Point(360, 34);
			outgoingBox.Location = new Point(360, 56);
			outgoingBox.Size = new Size(320, 120);

			lblRecipients.Location = new Point(360, 194);
			recipientsBox.Location = new Point(400, 190);

			sendBtn.Location = new Point(610, 186);

			connectBtn.Location = new Point(360, 250);
			disconnectBtn.Location = new Point(480, 250);

			lblClient.Location = new Point(360, 310);
			clientLabel.Location = new Point(430, 310);
			connectedCheck.Location = new Point(360, 340);

			Controls.Add(lblIncoming);
			Controls.Add(incomingBox);
			Controls.Add(lblOutgoing);
			Controls.Add(outgoingBox);
			Controls.Add(lblRecipients);
			Controls.Add(recipientsBox);
			Controls.Add(sendBtn);
			Controls.Add(connectBtn);
			Controls.Add(disconnectBtn);
			Controls.Add(lblClient);
			Controls.Add(clientLabel);
			Controls.Add(connectedCheck);

			Load += MainFormDevyatkina_Load;
			FormClosing += MainFormDevyatkina_FormClosing;
			recipientsBox.DropDown += RecipientsBox_DropDown;

			connectBtn.Click += ConnectBtn_Click;
			disconnectBtn.Click += DisconnectBtn_Click;
			sendBtn.Click += SendBtn_Click;

			UpdateControls();
		}

		private void MainFormDevyatkina_Load(object? sender, EventArgs e)
		{
			// Optional auto-connect
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

		private void ConnectBtn_Click(object? sender, EventArgs e)
		{
			if (isConnected)
				return;

			var ok = TransportDllDevyatkina.Devyatkina_Connect() != 0;
			if (!ok)
			{
				MessageBox.Show("Server is not running or connection failed.");
				return;
			}

			receiveThread = new Thread(ReceiveLoop) { IsBackground = true };
			receiveThread.Start();
		}

		private void DisconnectBtn_Click(object? sender, EventArgs e)
		{
			if (!isConnected)
				return;

			SendControlMessage(MessageTypesDevyatkina.MT_DISCONNECT_DEVYATKINA);
		}

		private void RecipientsBox_DropDown(object? sender, EventArgs e)
		{
			if (!isConnected)
				return;
			SendControlMessage(MessageTypesDevyatkina.MT_INFO_DEVYATKINA);
		}

		private void SendBtn_Click(object? sender, EventArgs e)
		{
			if (!isConnected)
				return;
			if (recipientsBox.SelectedIndex < 0)
				return;
			if (string.IsNullOrWhiteSpace(outgoingBox.Text))
				return;

			int to = GetSelectedRecipient();
			var msg = new MsgStructDevyatkina
			{
				to = to,
				msgType = MessageTypesDevyatkina.MT_DATA_DEVYATKINA,
				data = outgoingBox.Text
			};

			TransportDllDevyatkina.Devyatkina_SendMessage(msg);
		}

		private void SendControlMessage(MessageTypesDevyatkina type)
		{
			var msg = new MsgStructDevyatkina
			{
				to = -2,
				msgType = type,
				data = string.Empty
			};

			TransportDllDevyatkina.Devyatkina_SendMessage(msg);
		}

		private int GetSelectedRecipient()
		{
			if (recipientsBox.SelectedIndex == 0)
				return -1;
			return recipientIds[recipientsBox.SelectedIndex - 1];
		}

		private void ReceiveLoop()
		{
			var msg = new MsgStructDevyatkina();

			while (TransportDllDevyatkina.Devyatkina_ReceiveMessage(ref msg))
			{
				switch (msg.msgType)
				{
					case MessageTypesDevyatkina.MT_CONFIRM_DEVYATKINA:
						SafeUi(() =>
						{
							isConnected = true;
							clientLabel.Text = msg.data;
							UpdateControls();
						});
						break;
					case MessageTypesDevyatkina.MT_INFO_DEVYATKINA:
						SafeUi(() => UpdateRecipients(msg.data));
						break;
					case MessageTypesDevyatkina.MT_DATA_DEVYATKINA:
						SafeUi(() =>
						{
							incomingBox.AppendText(msg.data);
							incomingBox.AppendText(Environment.NewLine);
						});
						break;
					case MessageTypesDevyatkina.MT_DISCONNECT_DEVYATKINA:
						SafeUi(() => HandleDisconnect());
						return;
				}
			}

			SafeUi(HandleDisconnect);
		}

		private void UpdateRecipients(string data)
		{
			recipientIds.Clear();
			recipientsBox.Items.Clear();

			var ids = data.Split(',', StringSplitOptions.RemoveEmptyEntries)
				.Select(s => int.TryParse(s, out var id) ? id : -1)
				.Where(id => id >= 0)
				.ToList();

			recipientIds.AddRange(ids);
			recipientsBox.Items.Add("All clients");
			foreach (var id in recipientIds)
				recipientsBox.Items.Add($"Client {id}");

			if (recipientsBox.Items.Count > 0)
				recipientsBox.SelectedIndex = 0;
		}

		private void HandleDisconnect()
		{
			isConnected = false;
			clientLabel.Text = string.Empty;
			recipientIds.Clear();
			recipientsBox.Items.Clear();
			UpdateControls();
		}

		private void HeartbeatLoop()
		{
			while (isConnected)
			{
				Thread.Sleep(10000); // Send heartbeat every 10 seconds
				if (isConnected)
				{
					SendControlMessage(MessageTypesDevyatkina.MT_INFO_DEVYATKINA);
				}
			}
		}

		private void UpdateControls()
		{
			connectedCheck.Checked = isConnected;
			connectBtn.Enabled = !isConnected;
			disconnectBtn.Enabled = isConnected;
			sendBtn.Enabled = isConnected;
			recipientsBox.Enabled = isConnected;
			outgoingBox.Enabled = isConnected;
		}

		private void SafeUi(Action action)
		{
			if (InvokeRequired)
				BeginInvoke(action);
			else
				action();
		}
	}
}

