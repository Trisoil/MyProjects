/**
* @example:
* import com.wuwl.data.JEventDelegate;
* stage.addEventListener(MouseEvent.MOUSE_DOWN,JEventDelegate.create(mouseDownHandler,"a","b"));
* function mouseDownHandler(e:MouseEvent,...arg) {
*     trace(e);
*     trace(arg);
* }
*/
package pplive.skin.view.utils {
	
	import flash.events.Event;
	
	public class JEventDelegate {
		
		public static function create(f:Function,... arg):Function {
			return function(e:Event):void{f.apply(null,[e].concat(arg));};
		}
		
		public static function toString():String {
			return "Class JEventDelegate";
		}
	}
}