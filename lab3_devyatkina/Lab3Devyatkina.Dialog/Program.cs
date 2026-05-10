using System;
using System.Windows.Forms;

namespace Lab3Devyatkina.Dialog
{
	internal static class Program
	{
		[STAThread]
		static void Main()
		{
			ApplicationConfiguration.Initialize();
			Application.Run(new MainFormDevyatkina());
		}
	}
}

