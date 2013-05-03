package pplive.Utils
{
    import flash.events.NetStatusEvent;
    import flash.net.SharedObject;
    

    public class UserData
    {
        private var _sharedObj:SharedObject;
        private var _id:String;
        public function UserData(id:String)
        {
            _id=id;
            _sharedObj = SharedObject.getLocal(id,"/"); 
            flush(8*1024);   
            if(!_sharedObj){
            	trace("###################################### SharedObject 失败！");
            }
            else
            {
            	_sharedObj.addEventListener(NetStatusEvent.NET_STATUS,onFlushStatus);
            }   
        }
        private function onFlushStatus(event:NetStatusEvent):void {
            switch (event.info.code) {
                case "SharedObject.Flush.Success":
                    break;
                case "SharedObject.Flush.Failed":
                    break;
            }
        }

        public function setData(key:String,value:Object):void
        {
            _sharedObj.data[key]=value;          
        }
        public function getData(key:String):Object
        {
            return _sharedObj.data[key];
        }
        public function clear():void
        {
            _sharedObj.clear();
        }
        public function get sharedObj():SharedObject
        {
            return _sharedObj;
        }
        public function get id():String
        {
            return _id;
        } 
        public function flush(size:Number = -1):void
 		{
 			 var flushStatus:String  = size>0? _sharedObj.flush(8*1024): _sharedObj.flush();
 			 /*if (flushStatus != null) {
                switch (flushStatus) {
                    case SharedObjectFlushStatus.PENDING:
                        //mySo.addEventListener(NetStatusEvent.NET_STATUS, onFlushStatus);
                        break;
                    case SharedObjectFlushStatus.FLUSHED:
                        break;
                }
            }*/

 		}
    }
}