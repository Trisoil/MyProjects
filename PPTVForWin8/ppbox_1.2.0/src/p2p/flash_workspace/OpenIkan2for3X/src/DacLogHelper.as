package
{
	import com.pplive.dac.logclient.DataLog;
	import com.pplive.dac.logclient.DataLogItem;
	import com.pplive.dac.logclient.DataLogSource;
	
	import mx.utils.ObjectUtil;
	
	import org.as3commons.logging.api.ILogger;
	import org.as3commons.logging.api.getLogger;
	
	public class DacLogHelper
	{
		private static var logger:ILogger = getLogger(DacLogHelper);
		private var dataLog:DataLog;
		private var guid:String;
		
		public function DacLogHelper(guid:String)
		{
			logger.info("DacLogHelper guid:" + guid);
			dataLog = new DataLog(DataLogSource.IKanVodApp);
			this.guid = guid;
		}

		public function sendDacLog(logObject:Object):void
		{
			logger.info("sendDacLog:" + mx.utils.ObjectUtil.toString(logObject));
			
			var metaItems:Vector.<DataLogItem> = new Vector.<DataLogItem>();
			metaItems.push(new DataLogItem("dt", logObject.dt));
			metaItems.push(new DataLogItem("guid", guid));
			
			var items:Vector.<DataLogItem> = new Vector.<DataLogItem>();
			for (var item:Object in logObject)
			{
				if (item.toString() != "dt" && logObject[item] != null)
				{
					items.push(new DataLogItem(item.toString(), logObject[item].toString()));
				}
			}
			
			items.push(new DataLogItem("uid", ""));
			items.push(new DataLogItem("player", "p2p"));
			dataLog.sendLogRequestAsync(metaItems, items);
		}
	}
}