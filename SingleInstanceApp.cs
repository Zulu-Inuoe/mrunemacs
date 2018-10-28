using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;

namespace mrunemacs
{
  public sealed class SingleInstanceApp : IDisposable
  {
    public static bool Aquire(out SingleInstanceApp app)
    {
      return Aquire(Assembly.GetExecutingAssembly(), out app);
    }

    public static bool Aquire(Assembly assembly, out SingleInstanceApp app)
    {
      // get application GUID as defined in AssemblyInfo.cs
      string appGuid =
          ((GuidAttribute)Assembly.GetExecutingAssembly().
              GetCustomAttributes(typeof(GuidAttribute), false).
                  GetValue(0)).Value.ToString();

      // unique id for global mutex - Global prefix means it is global to the machine
      string mutexId = string.Format("{{{0}}}", appGuid);

      // Need a place to store a return value in Mutex() constructor call
      bool createdNew;

      // edited by MasonGZhwiti to prevent race condition on security settings via VanNguyen
      var mutex = new Mutex(false, mutexId, out createdNew);
      // edited by acidzombie24
      var hasHandle = false;
      try
      {
        try
        {
          hasHandle = mutex.WaitOne(0, false);
        }
        catch (AbandonedMutexException)
        {
          // Log the fact that the mutex was abandoned in another process,
          // it will still get acquired
          hasHandle = true;
        }

        if (hasHandle)
        {
          app = new SingleInstanceApp(mutex);
          return true;
        }
        else
        {
          app = null;
          return false;
        }
      }
      catch
      {
        if (hasHandle)
        {
          mutex.ReleaseMutex();
        }

        app = null;
        return false;
      }
    }

    public void Dispose()
    {
      if (m_Disposed)
      {
        throw new ObjectDisposedException(ToString());
      }

      m_Disposed = true;
      m_Mutex.ReleaseMutex();
    }

    private SingleInstanceApp(Mutex mutex)
    {
      m_Mutex = mutex;
    }

    private bool m_Disposed;
    private readonly Mutex m_Mutex;
  }
}
