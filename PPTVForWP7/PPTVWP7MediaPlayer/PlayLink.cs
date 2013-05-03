using System;
using System.Windows;
using System.Net;

namespace PPTVWP7MediaPlayer
{
    class PlayLinkInfo
    {
        private string _playlink = "";

        private string _playlink_decode = "";
        private string _domain = "";
        private string _filename = "";
        private bool _is_ok = false;

        public PlayLinkInfo(string playlink)
        {
            try
            {
                if (playlink.IndexOf("%") != -1)
                {
                    _playlink = HttpUtility.UrlDecode(playlink);
                }
                else
                    _playlink = playlink;

                _playlink_decode = Common.Decode(_playlink, "kioe257ds");
                int pos = _playlink_decode.IndexOf("@@@");
                if (pos >= 0)
                    _playlink_decode = _playlink_decode.Substring(0, pos);

                int splite_position = _playlink_decode.IndexOf("||");
                if (splite_position < 0)
                {
                    _is_ok = false;
                    return;
                }
                _domain = _playlink_decode.Substring(0, splite_position);
                _filename = _playlink_decode.Substring(splite_position + "||".Length);
                _is_ok = true;
            }
            catch
            {
                _is_ok = false;
            }
        }

        public string PlayLink
        {
            get { return _playlink; }
            set { _playlink = value; }
        }

        public string Domain
        {
            get { return _domain; }
        }

        public string FileName
        {
            get { return _filename; }
            set { _filename = value; }
        }

        public bool IsOK
        {
            get { return _is_ok; }
        }
    }
}
