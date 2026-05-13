using System.Runtime.InteropServices;

namespace Lab4Devyatkina.Dialog
{
	internal enum MessageTypesDevyatkina
	{
		MT_CONNECT_DEVYATKINA = 1,
		MT_DISCONNECT_DEVYATKINA = 2,
		MT_INFO_DEVYATKINA = 3,
		MT_DATA_DEVYATKINA = 4,
		MT_CONFIRM_DEVYATKINA = 5
	}

	[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
	internal struct MsgStructDevyatkina
	{
		public int to;
		public MessageTypesDevyatkina msgType;
		[MarshalAs(UnmanagedType.LPWStr)]
		public string data;
	}

	internal static class TransportDllDevyatkina
	{
		[DllImport("Lab4Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int Devyatkina_Connect();

		[DllImport("Lab4Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern void Devyatkina_Disconnect();

		[DllImport("Lab4Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int Devyatkina_SendMessage(MsgStructDevyatkina msg);

		[DllImport("Lab4Devyatkina.TransportDll.dll", CallingConvention = CallingConvention.Cdecl)]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool Devyatkina_ReceiveMessage(ref MsgStructDevyatkina msg);
	}
}

