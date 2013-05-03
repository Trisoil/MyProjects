package  {
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.MouseEvent;
	
	/**
	 *  setPosition(per:Number)   用于设定当前的播放进度  0 <= per <= 1  可不断侦听更新播放点
	 *  外部侦听 "_position_" 事件   可获取重置点 percent 值   0 <= percent <= 1
	 */
	
	public class Progress extends Sprite {
		
		private var heightBoth:uint = 10;
		private	var widthLine:uint;
		private var widthBar:uint = 3;
		private var bgColor:uint      = 0xFFCC00;
		private var borderColor:uint  = 0xFF00FF;
		private var borderSize:uint = 0;
		private var line:Sprite;
		private var bar:Sprite;
		private var _percent:Number;
		
		public function Progress(width:uint) {
			this.widthLine = width;
			// constructor code
			doDrawLine();
			doDrawBar();
		}
		
		private function doDrawLine():void {
			line = new Sprite();
			line.graphics.beginFill(bgColor);
			line.graphics.drawRect(0, 0, widthLine, heightBoth);
			line.graphics.endFill();
			line.addEventListener(MouseEvent.MOUSE_UP, onMouseUp, false, 0, true);
			addChild(line);
		}
		
		private function onMouseUp(event:MouseEvent):void
		{
			bar.x = event.localX;
			_percent = bar.x / widthLine;
			dispatchEvent(new Event("_position_"));
		}
		
		private function doDrawBar():void {
			bar = new Sprite();
			bar.graphics.beginFill(bgColor);
			bar.graphics.lineStyle(borderSize, borderColor);
			bar.graphics.drawRoundRect(0, 0, widthBar, heightBoth, 1);
			bar.graphics.endFill();
			addChild(bar);
		}
		
		public function setPosition(per:Number):void {
			bar.x = per * widthLine;
		}
		
		public function get percent():Number {
			return _percent;
		}

	}
	
}
