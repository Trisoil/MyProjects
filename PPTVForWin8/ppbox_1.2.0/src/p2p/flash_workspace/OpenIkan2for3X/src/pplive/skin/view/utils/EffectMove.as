package pplive.skin.view.utils {
	
	import flash.display.Sprite;
	import flash.display.MovieClip;
	import flash.display.DisplayObject;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.TimerEvent;
	import flash.utils.Timer;

	public class EffectMove {
		
		/*public static function mouseShowUp(root:*, obj:*, Show_Y:Number, Hidden_Y:Number):void {
			var delaytime:Timer = new Timer(3000);
			root.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			function onMoveHandler(e:MouseEvent):void {
				obj.removeEventListener(Event.ENTER_FRAME, onHideHandler);
				obj.addEventListener(Event.ENTER_FRAME, onShowHandler);
			}
			function onShowHandler(e:Event):void {
				delaytime.removeEventListener(TimerEvent.TIMER, onTimerHandler);
				if (obj.y <= Show_Y) {
					obj.y = Show_Y;
					if (! obj.hitTestPoint(root.mouseX, root.mouseY, false)) {
						obj.removeEventListener(Event.ENTER_FRAME, onShowHandler);
						delaytime.addEventListener(TimerEvent.TIMER, onTimerHandler);
						delaytime.start();
					} else {
						delaytime.stop();
					}
				} else {
					obj.y -= 2;
				}
			}
			function onHideHandler(e:Event):void {
				if (obj.y >= Hidden_Y) {
					obj.y = Hidden_Y;
					obj.removeEventListener(Event.ENTER_FRAME, onHideHandler);
					delaytime.addEventListener(TimerEvent.TIMER, onTimerHandler);
				} else {
					obj.y += 2;
				}
			}
			function onTimerHandler(t:TimerEvent):void {
				delaytime.stop();
				obj.addEventListener(Event.ENTER_FRAME, onHideHandler);
			}
		}
		
		public static function mouseShowDown(root:*, obj:*, Show_Y:Number, Hidden_Y:Number):void {
			var delaytime:Timer = new Timer(3000);
			root.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			function onMoveHandler(e:MouseEvent):void {
				obj.removeEventListener(Event.ENTER_FRAME, onHideHandler);
				obj.addEventListener(Event.ENTER_FRAME, onShowHandler);
			}
			function onShowHandler(e:Event):void {
				delaytime.removeEventListener(TimerEvent.TIMER, onTimerHandler);
				if (obj.y >= Show_Y) {
					obj.y = Show_Y;
					if (! obj.hitTestPoint(root.mouseX, root.mouseY, false)) {
						obj.removeEventListener(Event.ENTER_FRAME, onShowHandler);
						delaytime.addEventListener(TimerEvent.TIMER, onTimerHandler);
						delaytime.start();
					} else {
						delaytime.stop();
					}
				} else {
					obj.y += 2;
				}
			}
			function onHideHandler(e:Event):void {
				if (obj.y <= Hidden_Y) {
					obj.y = Hidden_Y;
					obj.removeEventListener(Event.ENTER_FRAME, onHideHandler);
					delaytime.addEventListener(TimerEvent.TIMER, onTimerHandler);
				} else {
					obj.y -= 2;
				}
			}
			function onTimerHandler(t:TimerEvent):void {
				delaytime.stop();
				obj.addEventListener(Event.ENTER_FRAME, onHideHandler);
			}
		}*/
		
		public static function mouseShowAlpha(obj:Array, root:*, showAlpha:Number = 1, hideAlpha:Number = 0):void {
			var delaytime:Timer = new Timer(3000);
			for (var j:int = 0; j < obj.length; j++) {
				if (obj[j] && obj[j].isEffect) {
					obj[j].alpha = 0;
				}				
			}
			root.addEventListener(MouseEvent.MOUSE_MOVE, onMoveHandler);
			function onMoveHandler(e:MouseEvent):void {				
				for (var i:int = 0, len:int = obj.length; i < len; i++) {
					if (obj[i] && obj[i].isEffect) {
						obj[i].visible = true;
						obj[i].removeEventListener(Event.ENTER_FRAME, onHideHandler);
						obj[i].addEventListener(Event.ENTER_FRAME, onShowHandler);
					}
				}
			} 
			function onShowHandler(e:Event):void {
				delaytime.removeEventListener(TimerEvent.TIMER, onTimerHandler);
				if (e.target.alpha >= showAlpha) {
					e.target.alpha = showAlpha;
					if (! e.target.hitTestPoint(root.mouseX, root.mouseY, false)) {
						e.target.removeEventListener(Event.ENTER_FRAME, onShowHandler);
						delaytime.addEventListener(TimerEvent.TIMER, JEventDelegate.create(onTimerHandler, e.target));
						delaytime.start();
					} else {
						delaytime.stop();
					}
				} else {
					e.target.alpha += 0.1;
					if (e.target.alpha > 0.9) {
						e.target.visible = true;
					}
				}
			}
			function onHideHandler(e:Event):void {
				if (e.target.alpha <= hideAlpha) {
					e.target.alpha = hideAlpha;
					e.target.removeEventListener(Event.ENTER_FRAME, onHideHandler);
					delaytime.addEventListener(TimerEvent.TIMER, JEventDelegate.create(onTimerHandler, e.target));
				} else {
					e.target.alpha -= 0.1;
					if (e.target.alpha < 0.1) {
						e.target.visible = false;
					}
				}
			}
			function onTimerHandler(t:TimerEvent, target:*):void {
				delaytime.stop();
				target.addEventListener(Event.ENTER_FRAME, onHideHandler);
			}
		}
		
	}
}