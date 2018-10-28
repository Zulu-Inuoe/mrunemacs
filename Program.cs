using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace mrunemacs
{
  class Program
  {
    static void RunEmacsClient(string[] args)
    {
      var sb = new StringBuilder("-n -a \"\"");

      if (args.Length > 0)
      {
        foreach (var arg in args)
        {
          sb.Append(" \"" + arg + "\"");
        }
      }
      else
      {
        sb.Append(" -c");
      }

      string emacsClientFileName;

      //If there is an emacsclient in our dir then use that, otherwise rely on PATH
      {
        var dir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        var emacsClientInDir = new FileInfo(Path.Combine(dir.FullName, "emacsclientw.exe"));
        if (emacsClientInDir.Exists)
        {
          emacsClientFileName = emacsClientInDir.FullName;
        }
        else
        {
          emacsClientFileName = "emacsclientw";
        }
      }

      var proc = new Process()
      {
        StartInfo = new ProcessStartInfo
        {
          FileName = emacsClientFileName,
          Arguments = sb.ToString(),
          CreateNoWindow = true,
          WindowStyle = ProcessWindowStyle.Hidden
        }
      };

      proc.Start();
    }

    [STAThread()]
    static void Main(string[] args)
    {
      SingleInstanceApp app;
      if (SingleInstanceApp.Aquire(out app))
      {
        using (app)
        {
          var emacsWindow = FindWindow(EmacsClassName, null);

          if (args.Length == 0 && emacsWindow != IntPtr.Zero)
          {//No args, just want to run emacs  
           //Just pop that window to the foreground
            if (IsIconic(emacsWindow))
            {
              ShowWindow(emacsWindow, SW_RESTORE);
            }
            SetForegroundWindow(emacsWindow);
          }
          else
          {
            RunEmacsClient(args);
          }
        }
      }
    }

    [DllImport("user32.dll", SetLastError = true)]
    static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

    [DllImport("user32.dll", SetLastError = true)]
    static extern bool IsIconic(IntPtr window);

    [DllImport("user32.dll", SetLastError = true)]
    static extern bool ShowWindow(IntPtr window, int cmdShow);

    [DllImport("user32.dll", SetLastError = true)]
    static extern bool SetForegroundWindow(IntPtr window);

    private const int SW_RESTORE = 9;
    private static readonly string EmacsClassName = "Emacs";
  }
}
