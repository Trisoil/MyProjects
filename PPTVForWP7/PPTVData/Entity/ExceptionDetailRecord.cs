using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PPTVData.Entity
{
    public class ExceptionDetailRecord
    {
        public string DateTime { get; set; }
        public string ExceptionDetail { get; set; }
        public string InnerExceptionDetail { get; set; }
        public ExceptionDetailRecord() { }
        public ExceptionDetailRecord(string dateTime, string exceptionDetail, string innerException)
        {
            DateTime = dateTime;
            ExceptionDetail = exceptionDetail;
            InnerExceptionDetail = innerException;
        }
    }
}
