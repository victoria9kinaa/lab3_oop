using System;
using System.Diagnostics;
using System.Threading;
using System.Windows.Forms;
using System.Collections.Generic;
using System.IO;

namespace GUI
{
    public partial class MainForm : Form
    {
        // ──────────────────────────────────────────────
        // Именованные события (Named Events) для IPC
        // ──────────────────────────────────────────────

        // GUI → Console: запустить N потоков
        private EventWaitHandle? _evStartThreads;
        // GUI → Console: остановить последний поток
        private EventWaitHandle? _evStopThread;
        // GUI → Console: отправить текстовое сообщение
        private EventWaitHandle? _evSendMessage;
        // GUI → Console: завершить процесс консоли (отдельный сигнал!)
        private EventWaitHandle? _evTerminate;
        // Console → GUI: подтверждение создания потоков
        private EventWaitHandle? _evThreadsCreated;
        // Console → GUI: подтверждение остановки потока
        private EventWaitHandle? _evThreadStopped;

        private const string EVENT_START = "Global\\Devyatkina_StartThreads";
        private const string EVENT_STOP = "Global\\Devyatkina_StopThread";
        private const string EVENT_SEND = "Global\\Devyatkina_SendMessage";
        private const string EVENT_TERMINATE = "Global\\Devyatkina_KillConsole";
        private const string EVENT_CREATED = "Global\\Devyatkina_ThreadsCreated";
        private const string EVENT_STOPPED = "Global\\Devyatkina_ThreadStopped";

        // Имя отображаемого файла с общим числом N (простейший shared-mem через имя события)
        // Передаём N через имя события + суффикс, чтобы не тянуть shared memory
        private const string EVENT_COUNT_PREFIX = "Global\\Devyatkina_Count_";

        private Process? _consoleProcess;
        private Thread? _monitorThread;   // поток-монитор завершения консоли
        private Thread? _ackThread;       // поток ожидания подтверждений от консоли
        private Thread? _stopAckThread;   // поток ожидания подтверждения остановки

        private bool _consoleRunning = false;

        // Список номеров живых потоков (отображается в ComboBox)
        private readonly List<int> _threadIds = new();
        private int _nextThreadId = 1;

        public MainForm()
        {
            InitializeComponent();
        }

        // ══════════════════════════════════════════════
        // Кнопка START
        // ══════════════════════════════════════════════
        private void btnStart_Click(object sender, EventArgs e)
        {
            if (!_consoleRunning)
            {
                StartConsoleProcess();
            }
            else
            {
                SignalCreateThreads();
            }
        }

        // ══════════════════════════════════════════════
        // Кнопка STOP
        // ══════════════════════════════════════════════
        private void btnStop_Click(object sender, EventArgs e)
        {
            if (!_consoleRunning) return;

            if (_threadIds.Count == 0)
            {
                // Нет потоков — завершаем консоль
                TerminateConsole();
            }
            else
            {
                SignalStopLastThread();
            }
        }

        // ══════════════════════════════════════════════
        // Кнопка SEND
        // ══════════════════════════════════════════════
        //private void btnSend_Click(object sender, EventArgs e)
        //{
         //   if (!_consoleRunning) return;

         //   string msg = txtMessage.Text.Trim();
         //   if (string.IsNullOrEmpty(msg)) return;

            // В текущей лабе «Send» просто сигнализирует консоли
         //   _evSendMessage?.Set();
         //   AppendLog($"[GUI] Отправлено сообщение: {msg}");
        //}

        // ──────────────────────────────────────────────
        // Запуск консольного процесса
        // ──────────────────────────────────────────────
        private void StartConsoleProcess()
        {
            try
            {
                OpenOrCreateEvents();

                string consolePath = GetConsolePath();
                if (!File.Exists(consolePath))
                {
                    MessageBox.Show($"Консольное приложение не найдено:\n{consolePath}",
                        "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                _consoleProcess = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = consolePath,
                        UseShellExecute = false,
                        CreateNoWindow = false
                    },
                    EnableRaisingEvents = true
                };
                _consoleProcess.Exited += ConsoleProcess_Exited;
                _consoleProcess.Start();

                _consoleRunning = true;
                _nextThreadId = 1;
                _threadIds.Clear();
                UpdateComboBox();

                AppendLog("[GUI] Консоль запущена.");
                btnStart.Text = "Create Threads";

                // Запускаем монитор завершения
                StartProcessMonitor();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Ошибка запуска консоли: {ex.Message}");
            }
        }

        // ──────────────────────────────────────────────
        // Сигнал на создание N потоков
        // ──────────────────────────────────────────────
        private void SignalCreateThreads()
        {
            int n = (int)numericN.Value;
            AppendLog($"[GUI] Сигнал: создать {n} поток(ов)...");

            // Передаём N через временное именованное событие
            // (простое решение без shared memory)
            string countEventName = EVENT_COUNT_PREFIX + n.ToString();
            using var countEvent = new EventWaitHandle(true, EventResetMode.ManualReset, countEventName);

            _evStartThreads?.Set();

            // Ждём подтверждения в фоне, чтобы не блокировать UI
            StartAckWaiter(n);
        }

        // ──────────────────────────────────────────────
        // Ожидание подтверждения от консоли (фоновый поток)
        // ──────────────────────────────────────────────
        private void StartAckWaiter(int expectedCount)
        {
            _ackThread?.Interrupt();
            _ackThread = new Thread(() =>
            {
                try
                {
                    // Ждём сигнала «потоки созданы» (таймаут 10 сек)
                    bool signaled = _evThreadsCreated?.WaitOne(10000) ?? false;
                    if (!signaled) return;

                    _evThreadsCreated?.Reset();

                    // Обновляем UI в главном потоке
                    Invoke(() =>
                    {
                        for (int i = 0; i < expectedCount; i++)
                        {
                            _threadIds.Add(_nextThreadId++);
                        }
                        UpdateComboBox();
                        AppendLog($"[GUI] Подтверждено: создано {expectedCount} поток(ов). " +
                                  $"Всего: {_threadIds.Count}");
                    });
                }
                catch (ThreadInterruptedException) { }
                catch (ObjectDisposedException) { }
            })
            { IsBackground = true };
            _ackThread.Start();
        }

        // ──────────────────────────────────────────────
        // Сигнал на остановку последнего потока
        // ──────────────────────────────────────────────
        private void SignalStopLastThread()
        {
            AppendLog("[GUI] Сигнал: остановить последний поток...");
            _evStopThread?.Set();

            // Ждём подтверждения в фоне
            _stopAckThread?.Interrupt();
            _stopAckThread = new Thread(() =>
            {
                try
                {
                    bool signaled = _evThreadStopped?.WaitOne(10000) ?? false;
                    if (!signaled) return;
                    _evThreadStopped?.Reset();

                    Invoke(() =>
                    {
                        if (_threadIds.Count > 0)
                        {
                            int removed = _threadIds[^1];
                            _threadIds.RemoveAt(_threadIds.Count - 1);
                            UpdateComboBox();
                            AppendLog($"[GUI] Поток #{removed} остановлен. Осталось: {_threadIds.Count}");
                        }
                    });
                }
                catch (ThreadInterruptedException) { }
                catch (ObjectDisposedException) { }
            })
            { IsBackground = true };
            _stopAckThread.Start();
        }

        // ──────────────────────────────────────────────
        // Завершение консольного процесса
        // ──────────────────────────────────────────────
        private void TerminateConsole()
        {
            try
            {
                if (_consoleProcess != null && !_consoleProcess.HasExited)
                {
                    // Отдельный именованный сигнал завершения консоли (WinAPI)
                    _evTerminate?.Set();
                    AppendLog("[GUI] Сигнал TERMINATE отправлен консоли.");
                }
            }
            catch { }
        }

        private void ShutdownConsoleAndWorkers()
        {
            try { _ackThread?.Interrupt(); } catch { }
            try { _stopAckThread?.Interrupt(); } catch { }
            try { _monitorThread?.Interrupt(); } catch { }

            try
            {
                if (_consoleProcess != null && !_consoleProcess.HasExited)
                {
                    _evTerminate?.Set();
                    if (!_consoleProcess.WaitForExit(3000))
                    {
                        _consoleProcess.Kill(true);
                        _consoleProcess.WaitForExit(3000);
                    }
                }
            }
            catch { }
            finally
            {
                _consoleRunning = false;
            }
        }

        // ──────────────────────────────────────────────
        // Мониторинг завершения консоли
        // ──────────────────────────────────────────────
        private void StartProcessMonitor()
        {
            _monitorThread = new Thread(() =>
            {
                try
                {
                    _consoleProcess?.WaitForExit();
                }
                catch { }

                // Консоль завершилась
                try
                {
                    Invoke(OnConsoleExited);
                }
                catch { }
            })
            { IsBackground = true };
            _monitorThread.Start();
        }

        private void ConsoleProcess_Exited(object? sender, EventArgs e)
        {
            // Обрабатывается в _monitorThread → OnConsoleExited
        }

        private void OnConsoleExited()
        {
            _consoleRunning = false;
            _threadIds.Clear();
            UpdateComboBox();
            btnStart.Text = "Start";
            try { _ackThread?.Interrupt(); } catch { }
            try { _stopAckThread?.Interrupt(); } catch { }
            _ackThread = null;
            _stopAckThread = null;
            _monitorThread = null;
            _consoleProcess?.Dispose();
            _consoleProcess = null;
            CloseEvents();
            AppendLog("[GUI] Консоль завершена. Готов к новому запуску.");
        }

        // ──────────────────────────────────────────────
        // Управление именованными событиями
        // ──────────────────────────────────────────────
        private void OpenOrCreateEvents()
        {
            CloseEvents();
            _evStartThreads = new EventWaitHandle(false, EventResetMode.AutoReset, EVENT_START);
            _evStopThread = new EventWaitHandle(false, EventResetMode.AutoReset, EVENT_STOP);
            _evSendMessage = new EventWaitHandle(false, EventResetMode.AutoReset, EVENT_SEND);
            _evTerminate = new EventWaitHandle(false, EventResetMode.AutoReset, EVENT_TERMINATE);
            _evThreadsCreated = new EventWaitHandle(false, EventResetMode.ManualReset, EVENT_CREATED);
            _evThreadStopped = new EventWaitHandle(false, EventResetMode.ManualReset, EVENT_STOPPED);
        }

        private void CloseEvents()
        {
            _evStartThreads?.Close(); _evStartThreads = null;
            _evStopThread?.Close(); _evStopThread = null;
            _evSendMessage?.Close(); _evSendMessage = null;
            _evTerminate?.Close(); _evTerminate = null;
            _evThreadsCreated?.Close(); _evThreadsCreated = null;
            _evThreadStopped?.Close(); _evThreadStopped = null;
        }

        // ──────────────────────────────────────────────
        // Вспомогательные методы UI
        // ──────────────────────────────────────────────
        private void UpdateComboBox()
        {
            cmbThreads.Items.Clear();

            // 🔥 обязательные элементы по заданию
            cmbThreads.Items.Add("Все потоки");
            cmbThreads.Items.Add("Главный поток");

            // добавляем рабочие потоки
            foreach (int id in _threadIds)
            {
                cmbThreads.Items.Add($"Thread #{id}");
            }

            // выбираем первый элемент по умолчанию
            if (cmbThreads.Items.Count > 0)
            {
                cmbThreads.SelectedIndex = 0;
            }
        }

        private void AppendLog(string text)
        {
            txtLog.AppendText($"[{DateTime.Now:HH:mm:ss}] {text}{Environment.NewLine}");
        }

        private static string GetConsolePath()
        {
            // Вариант 1: рядом с GUI.exe (самый простой — скопируй ConsoleApp.exe сюда)
            string baseDir = AppContext.BaseDirectory;
            string next = Path.Combine(baseDir, "ConsoleApp.exe");
            if (File.Exists(next)) return next;

            // Вариант 2: стандартный выход сборки x64\Debug
            // Поднимаемся из GUI\bin\Debug\net8.0-windows\ на 4 уровня вверх к корню solution
            string solutionDir = Path.GetFullPath(Path.Combine(baseDir, @"..\..\..\..\.."));
            string[] candidates = new[]
            {
                Path.Combine(solutionDir, @"x64\Debug\ConsoleApp.exe"),
                Path.Combine(solutionDir, @"x64\Release\ConsoleApp.exe"),
                Path.Combine(solutionDir, @"Console\x64\Debug\ConsoleApp.exe"),
                Path.Combine(solutionDir, @"Console\x64\Release\ConsoleApp.exe"),
            };

            foreach (string path in candidates)
                if (File.Exists(path)) return path;

            // Если ничего не нашли — вернуть путь для сообщения об ошибке
            // с инструкцией куда положить файл
            return Path.Combine(baseDir, "ConsoleApp.exe");
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            ShutdownConsoleAndWorkers();
            CloseEvents();
            base.OnFormClosing(e);
        }

        private void MainForm_Load(object sender, EventArgs e)
        {

        }
    }
}
