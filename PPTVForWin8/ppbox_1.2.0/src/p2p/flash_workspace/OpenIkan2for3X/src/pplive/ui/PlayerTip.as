package pplive.ui
{
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;

	public class PlayerTip extends Sprite
	{
		private var _bg:Shape = null;
		private var _txt:TextField = null;
		
		
		public function PlayerTip()
		{
			super();
			init();
		}
		private function init():void{
			this.mouseChildren = false;
			this.mouseEnabled = false;
			//背景
			_bg = new Shape();
			_bg.graphics.beginFill(0xCCCCCC,1);
			_bg.graphics.lineStyle(0.5,0x333333);
			_bg.graphics.drawRect(0,0,100,20);
			_bg.graphics.endFill();
			this.addChild(_bg);
			
			//文字
			_txt = new TextField();
			var ft:TextFormat = new TextFormat();
			ft.color = 0x333333;
			ft.font = 'Times New Roman';
			ft.size = 12;
			
			_txt.autoSize = TextFieldAutoSize.LEFT;
			_txt.selectable = false;
			_txt.mouseEnabled = false;
			_txt.defaultTextFormat = ft;
			_txt.text = '测试';
			_txt.x = 2;
			_txt.y = -2;
			
			this.addChild(_txt);
			
			hide();
		}
		public function hide():void{
			this.alpha = 0;
			this.visible = false;
			this.x = 0;
			this.y = 0;
		}
		public function show():void{
			if(_txt.text.length > 0){
				this.alpha = 1;
				this.visible = true;
			}
		}
		public function resize(x:Number,y:Number,w:Number = 0,h:Number = 0):void{
			this.x = x;
			this.y = y;
			this._bg.width = this._txt.width + 4 || w;
			this._bg.height = this._txt.height - 4 || h;	
		}
		public function set text(value:String):void{
			_txt.text = value;
		}
		
	}
}