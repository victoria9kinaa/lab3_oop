using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace GUI_Devyatkina
{
    public partial class MainFormDevyatkina : Form
    {
        [DllImport("MMFLib_Devyatkina.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        private static extern int SendCommandDevyatkina(int commandType, int targetId, int param, string text,
            out int ackStatus, StringBuilder ackText, int ackTextChars, int timeoutMs);

        private Process? _consoleProcess;
        private readonly List<int> _threadIds = new();
        private int _nextThreadId = 1;
        private bool _consoleRunning;

        public MainFormDevyatkina()
        {
            InitializeComponent();
            UpdateCombo();
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            if (!_consoleRunning)
            {
                StartConsole();
                return;
            }

            int n = (int)numericN.Value;
            if (SendCommand(1, -1, n, string.Empty, out var ack))
            {
                for (int i = 0; i < n; ++i) _threadIds.Add(_nextThreadId++);
                UpdateCombo();
                AppendLog($"Создано потоков: {n}. {ack}");
            }
            else
            {
                AppendLog("Не удалось создать потоки.");
            }
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            if (!_consoleRunning) return;
            if (_threadIds.Count == 0)
            {
                if (SendCommand(4, 0, 0, string.Empty, out var shutdownAck))
                {
                    AppendLog(shutdownAck);
                }
                return;
            }

            if (SendCommand(2, 0, 0, string.Empty, out var ack))
            {
                _threadIds.RemoveAt(_threadIds.Count - 1);
                UpdateCombo();
                AppendLog($"Остановлен последний поток. {ack}");
            }
            else
            {
                AppendLog("Не удалось остановить поток.");
            }
        }

        private void btnSend_Click(object sender, EventArgs e)
        {
            if (!_consoleRunning) return;
            var text = txtMessage.Text.Trim();
            if (string.IsNullOrWhiteSpace(text)) return;

            int targetId = ResolveTargetId();
            if (!SendCommand(3, targetId, 0, text, out var ack))
            {
                AppendLog("Ошибка отправки в MMF.");
                return;
            }

            AppendLog($"Сообщение доставлено, target={targetId}. {ack}");
        }

        private int ResolveTargetId()
        {
            if (cmbThreads.SelectedItem == null) return -1;
            string selected = cmbThreads.SelectedItem.ToString() ?? "Все потоки";
            if (selected == "Все потоки") return -1;
            if (selected == "Главный поток") return 0;
            if (selected.StartsWith("Thread #"))
            {
                var part = selected.Replace("Thread #", string.Empty);
                if (int.TryParse(part, out int id)) return id;
            }
            return -1;
        }

        private void StartConsole()
        {
            string path = GetConsolePath();
            if (!File.Exists(path))
            {
                MessageBox.Show($"Не найден ConsoleAppDevyatkina.exe:\n{path}");
                return;
            }

            _consoleProcess = Process.Start(new ProcessStartInfo
            {
                FileName = path,
                UseShellExecute = false,
                CreateNoWindow = false
            });

            _consoleRunning = true;
            _threadIds.Clear();
            _nextThreadId = 1;
            btnStart.Text = "Create Threads";
            UpdateCombo();
            AppendLog("Консоль запущена.");
        }

        private static bool SendCommand(int commandType, int targetId, int param, string text, out string ackText)
        {
            var ack = new StringBuilder(2048);
            int status;
            int ok = SendCommandDevyatkina(commandType, targetId, param, text, out status, ack, ack.Capacity, 10000);
            ackText = ack.ToString();
            return ok == 1 && status == 1;
        }

        private static string GetConsolePath()
        {
            var baseDir = AppContext.BaseDirectory;
            var candidates = new[]
            {
                Path.Combine(baseDir, "ConsoleAppDevyatkina.exe"),
                Path.Combine(baseDir, @"..\..\..\..\x64\Debug\ConsoleAppDevyatkina.exe"),
                Path.Combine(baseDir, @"..\..\..\..\x64\Release\ConsoleAppDevyatkina.exe")
            };

            foreach (var candidate in candidates)
            {
                string absolute = Path.GetFullPath(candidate);
                if (File.Exists(absolute)) return absolute;
            }

            return Path.GetFullPath(candidates[0]);
        }

        private void UpdateCombo()
        {
            cmbThreads.Items.Clear();
            cmbThreads.Items.Add("Все потоки");
            cmbThreads.Items.Add("Главный поток");
            foreach (int id in _threadIds) cmbThreads.Items.Add($"Thread #{id}");
            cmbThreads.SelectedIndex = 0;
        }

        private void AppendLog(string text)
        {
            txtLog.AppendText($"[{DateTime.Now:HH:mm:ss}] {text}{Environment.NewLine}");
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            try
            {
                if (_consoleRunning)
                {
                    SendCommand(4, 0, 0, string.Empty, out _);
                }

                if (_consoleProcess is { HasExited: false })
                {
                    _consoleProcess.WaitForExit(2000);
                    if (!_consoleProcess.HasExited)
                    {
                        _consoleProcess.Kill(true);
                    }
                }
            }
            catch
            {
            }

            base.OnFormClosing(e);
        }
    }
}
