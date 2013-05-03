package com.pplive.dac.logclient
{
	public class DataLogItem
	{
		public var Name:String;
		public var Value:String;
		
		public function DataLogItem(name:String, value:String)
		{
			this.Name = name;
			this.Value = value;
		}
	}

}