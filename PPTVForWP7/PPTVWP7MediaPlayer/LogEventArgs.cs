using System;

namespace PPTVWP7MediaPlayer
{
    /// <summary>
    /// LogEventArgs 调试日志输出的参数类
    /// </summary>
    public class LogEventArgs : EventArgs
    {
        private string _text = "";

        public LogEventArgs(string text)
        {
            _text = text;
        }

        public string Text
        {
            get { return _text; }
        }
    }
}
