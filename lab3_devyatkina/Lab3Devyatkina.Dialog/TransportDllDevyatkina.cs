using System.Runtime.InteropServices;

namespace Lab3Devyatkina.Dialog
{
	internal static class TransportDllDevyatkina
	{
		[DllImport("Lab3Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
		public static extern int Devyatkina_Connect();

		[DllImport("Lab3Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void Devyatkina_Disconnect();

		[DllImport("Lab3Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int Devyatkina_RequestThreadCount();

		[DllImport("Lab3Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int Devyatkina_StartThreads(int nThreads);

		[DllImport("Lab3Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int Devyatkina_StopThread();

		[DllImport("Lab3Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
		public static extern int Devyatkina_SendText(int to, string text);
	}
}

