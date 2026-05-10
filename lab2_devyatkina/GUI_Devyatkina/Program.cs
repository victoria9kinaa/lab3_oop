namespace GUI_Devyatkina
{
    internal static class Program
    {
        [STAThread]
        private static void Main()
        {
            ApplicationConfiguration.Initialize();
            Application.Run(new MainFormDevyatkina());
        }
    }
}
