package pplive.ui
{
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.KeyboardEvent;
	import flash.text.TextField;
	import flash.ui.Keyboard;
	
	import pplive.Utils.StringUtil;
	import pplive.Utils.Utils;
	
	public class DebugTxt
	{
		private var _debugCon:Sprite= null;
		public var _debugTxt:TextField = null;
		public  var _shp:Shape = null;
		
		public function DebugTxt(col:*)
		{
			try{
				var control:Sprite = col;
				_debugTxt = new TextField();
				_debugTxt.selectable = false;
				_debugTxt.multiline = true;
				_debugTxt.mouseWheelEnabled = true;
				_debugTxt.width = control.stage.stageWidth;
				_debugTxt.height = control.stage.stageHeight;
				_debugTxt.x = 0;
				_debugTxt.y = 0;
				_shp = new Shape();
				_shp.graphics.beginFill(0xFFFFFF,1);
				_shp.graphics.drawRect(0,0,control.stage.stageWidth-40,control.stage.stageHeight-40);
				_shp.graphics.endFill();
				_debugCon = new Sprite();
				_debugCon.addChild(_shp);
				_debugCon.addChild(_debugTxt);
				_debugCon.alpha = 0;
				_debugCon.visible = false;
				_debugCon.mouseEnabled = false;
				KeyState.setStage(control.stage);
				control.stage.addEventListener(KeyboardEvent.KEY_DOWN,function(evt:KeyboardEvent){
					if(KeyState.keyIsDown(Keyboard.HOME) && KeyState.keyIsDown(Keyboard.SHIFT)){
						_debugCon.alpha = .8;
						_debugCon.visible = true;
						_debugCon.mouseEnabled = true;
						_debugTxt.selectable = true;
						_debugTxt.text = StringUtil.trim(Utils.tf.text);
					} else if(KeyState.keyIsDown(Keyboard.END) && KeyState.keyIsDown(Keyboard.SHIFT)){
						_debugCon.alpha = 0;
						_debugCon.visible = false;
						_debugCon.mouseEnabled = false;
						_debugTxt.selectable = false;
						_debugTxt.text = '';
					}
				});
			} catch(e:Error){
				trace(e);
			}
		}
		public function get text():Sprite{
			return _debugCon;
		}

	}
}