using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Diagnostics;

namespace PPTVData.Log
{
    public class LogManager
    {
        #region 单例
        static private LogManager _log_manager = null;
        static public LogManager Ins
        {
            get
            {
                if (null == _log_manager)
                    _log_manager = new LogManager();
                return _log_manager;
            }
        }
        #endregion

        private StringBuilder _all_log_info = new StringBuilder();
        private string _current_log_info = "";
        private int _max_current_log_length = 10000;
        private bool _enable_container_output = true;

        private TextBox _log_container = null;


        private LogManager() { }

        public int MaxCurrentLogLength
        {
            get { return _max_current_log_length; }
            set { _max_current_log_length = value; }
        }

        public string AllLoginfo
        {
            get { return _all_log_info.ToString(); }
        }

        public bool EnableContainerOutput
        {
            get { return _enable_container_output; }
            set
            {
                _enable_container_output = value;
                if (null != _log_container && true == _enable_container_output)
                {
                    _log_container.Text = _current_log_info;
                }
            }
        }

        public void SetLogContainer(TextBox log_container)
        {
            _log_container = log_container;
            if (null != _log_container && true == _enable_container_output)
            {
                _log_container.Text = _current_log_info;
            }
        }

        public void DebugLog(string str)
        {
#if DEBUG
            Debug.WriteLine("sl_client:d: " + DateTime.Now.ToString("HH:mm:ss:fff") + "\t" + str);
#endif
        }

        public void DebugLog(string str, params object[] objs)
        {
#if DEBUG
            DebugLog(string.Format(str, objs));
#endif
        }

        public void Log(string str)
        {
            string short_time_string = DateTime.Now.ToString("HH:mm:ss:fff");
#if DEBUG
            Debug.WriteLine("sl_client:r: " + short_time_string + "\t" + str);
#endif
            _all_log_info.Append(short_time_string);
            _all_log_info.Append("\t");
            _all_log_info.Append(str);
            _all_log_info.Append("\r\n");

            _current_log_info = str + "\r\n" + _current_log_info;
            if (_current_log_info.Length > _max_current_log_length)
                _current_log_info = _current_log_info.Substring(0, _max_current_log_length);

            if (null != _log_container && true == _enable_container_output)
            {
                _log_container.Text = _current_log_info;
            }
        }

        public void Log(string str, params object[] objs)
        {
            Log(string.Format(str, objs));
        }
    }
}
