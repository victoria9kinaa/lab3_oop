using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace SocketSharp
{
    class Program
    {
        static void SendString(Socket s, string str)
        {
            int n = str.Length * 2;
            s.Send(BitConverter.GetBytes(n), sizeof(int), SocketFlags.None);
            s.Send(Encoding.Unicode.GetBytes(str), n, SocketFlags.None);
        }

        static string ReceiveString(Socket s)
        {
            byte[] b = new byte[sizeof(int)];

            s.Receive(b, sizeof(int), SocketFlags.None);
            int n = BitConverter.ToInt32(b, 0);
            b = new byte[n];
            s.Receive(b, n, SocketFlags.None);

            return Encoding.Unicode.GetString(b, 0, n);
        }
        static void Main(string[] args)
        {
            int nPort = 12345;
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            s.Connect(endPoint);
            if(s.Connected)
            {
                while(true)
                {
                    SendString(s, Console.ReadLine());
                    Console.WriteLine(ReceiveString(s));
                }
            }
        }
    }
}
