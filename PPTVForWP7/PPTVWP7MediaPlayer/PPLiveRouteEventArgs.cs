using System;

namespace PPTVWP7MediaPlayer
{
    /// <summary>
    /// 类似于silverlight自带的 RoutedEventArgs 类
    /// </summary>
    public class PPLiveRoutedEventArgs : EventArgs
    {
        private object _original_source = null;

        public PPLiveRoutedEventArgs(object original_source)
        {
            _original_source = original_source;
        }

        public object OriginalSource
        {
            get { return _original_source; }
        }
    }

    /// <summary>
    /// 类似于silverlight自带的 RoutedEventArgs 类
    /// </summary>
    public class PPLiveExceptionRoutedEventArgs : PPLiveRoutedEventArgs
    {
        private Exception _error_exception = null;

        public PPLiveExceptionRoutedEventArgs(object original_source, Exception error_exception)
            : base(original_source)
        {
            _error_exception = error_exception;
        }
        public Exception ErrorException
        {
            get { return _error_exception; }
        }
    }

    /// <summary>
    /// 特殊的自定义的打开错误描述类
    /// </summary>
    public class PPLiveOpenException : Exception
    {
        public PPLiveOpenException(string message)
            : base(message)
        {
        }
    }
}
