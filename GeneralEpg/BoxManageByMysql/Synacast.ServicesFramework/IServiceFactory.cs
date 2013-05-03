using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Synacast.ServicesFramework
{
    public interface IServiceFactory
    {
        object ServiceInit();
    }
}
