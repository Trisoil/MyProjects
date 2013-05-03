using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PPTVData.Factory.Player
{
    using PPTVData.Entity;

    public class PlaySegmentManage
    {
        PlayInfo _playInfo;

        int _segment;
        int _downSegment;

        public Action<string, double> Play;

        public double TotalSeconds
        {
            get { return _playInfo.TotalDuration; }
        }

        public int Segment
        {
            get { return _segment; }
        }

        public double SegmentStartSeconds
        {
            get {
                return _playInfo.Segments[_segment].TotalDuration - _playInfo.Segments[_segment].Duration;
            }
        }

        public double SegmentTotalSeconds
        {
            get {
                return _playInfo.Segments[_segment].TotalDuration;
            }
        }

        public bool CanDownload
        {
            get {
                return _downSegment < _playInfo.Segments.Count;
            }
        }

        public string DownloadUri
        {
            get {_downSegment = _segment + 1; return CreatePlayUri(_downSegment); }
        }

        public void SetSource(PlayInfo playInfo)
        {
            _playInfo = playInfo;
            _segment = 0;
            if (Play != null)
                Play(CreatePlayUri(_segment), 0);
        }

        public double Seek(TimeSpan position)
        {
            var totalSeconds = position.TotalSeconds;
            var seekDrag = _playInfo.Segments.FirstOrDefault(v => v.TotalDuration > totalSeconds);
            if (seekDrag != null)
            {
                var pos = totalSeconds - (seekDrag.TotalDuration - seekDrag.Duration);
                if (seekDrag.No == _segment)
                {
                    return pos;
                }
                else
                {
                    _segment = seekDrag.No;
                    if (Play != null)
                        Play(CreatePlayUri(_segment), pos);
                }
            }
            return 0;
        }

        public void NextSegment()
        {
            _segment += 1;
        }

        private string CreatePlayUri(int segment)
        {
            var key = KeyGenerator.GetKey(_playInfo.St);
            return string.Format("http://{0}:80/{1}/{2}?key={3}", _playInfo.Sh, segment, _playInfo.Rid, key);
        }
    }
}
