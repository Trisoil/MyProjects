package pplive.Utils
{
    import flash.external.ExternalInterface;

    /**
     * 简单封装ExternalInterface
     * 减少异常处理
     *
     * @author xushengs@gmail.com
     *
     */
    public class ExternalPacked
    {

        public function ExternalPacked()
        {
        }

        /**
         * 用于添加外部调用接口
         *
         * @param name
         * @param method
         *
         */
        public static function add(name:String, method:Function):void
        {
            // if is available
            if (ExternalInterface.available)
            {
                try
                {
                    ExternalInterface.addCallback(name, method);
                }
                catch (error:SecurityError)
                {
                    trace("A SecurityError occurred: " + error.message + "\n");
                }
                catch (error:Error)
                {
                    trace("An Error occurred: " + error.message + "\n");
                }
            }
            else
            {
                trace("External interface is not available for this container.");
            }
        }

        /**
         * 用于调用外部接口
         *
         * @param method
         * @param args
         *
         */
        public static function call(method:String, ... args):void
        {
			try{
	            if (ExternalInterface.available)
	            {
	                ExternalInterface.call(method, args);
	            }
			}catch(e:Error){
				trace(e);
			}
        }
    }
}