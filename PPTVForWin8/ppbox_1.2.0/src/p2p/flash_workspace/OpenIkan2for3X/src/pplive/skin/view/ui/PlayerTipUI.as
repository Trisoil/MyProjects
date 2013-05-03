package pplive.skin.view.ui
{
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import flash.events.TextEvent;
	import flash.events.TimerEvent;
	import flash.external.ExternalInterface;
	import flash.text.StyleSheet;
	import flash.text.TextField;
	import flash.text.TextFormatAlign;
	import flash.utils.*;
	
	import pplive.skin.view.events.SkinEvent;
	import pplive.skin.view.utils.UtilsMethod;
	
	public class PlayerTipUI extends MovieClip
	{
		private var _tipTxt:String;
		private var _tipTxtField:TextField;
		private var _interval:Number = 0;
		private var _times:Number = -1;
		private var _link:String = '';
		private var cssStyle:StyleSheet;
		private var ct:uint = 0;
		private var _count:int = 0;
		private var _inter:uint = 0;
		private var tct:Timer;
		private var _display:Number = 7000;
		private var closeBtnMc:CloseBtnUI;
		
		public function PlayerTipUI()
		{
			this.visible = false;
			_tipTxtField = new TextField();
			_tipTxtField.autoSize = TextFormatAlign.LEFT;
			_tipTxtField.wordWrap = false;
			_tipTxtField.multiline = false;
			_tipTxtField.selectable = false;
			_tipTxtField.x = 3;
			_tipTxtField.addEventListener(TextEvent.LINK,tipTxtLinkHandler);
			cssStyle = new StyleSheet();
			cssStyle.parseCSS("div{color:#FFFFFF}");
			cssStyle.parseCSS("a {color:#0099FF;text-decoration:underline;}");
			cssStyle.parseCSS("a:link,a:visited,a:active {color:#0099FF; text-decoration:underline;}");
			this.addChild(_tipTxtField);
			//关闭按钮 共用部分
			closeBtnMc = new CloseBtnUI();
			closeBtnMc.scaleX = 0.8;
			closeBtnMc.scaleY = 0.8;
			closeBtnMc.y = 3;
			closeBtnMc.addEventListener(MouseEvent.CLICK,onCloseHandler);
			this.addChild(closeBtnMc);
			super();
		}
		public function setObj(obj:Object):void{
			_count = 0;
			clearTimeout(ct);
			if(obj.hasOwnProperty('txt')){
				_tipTxt = obj['txt'].toString();
			}
			if(obj.hasOwnProperty('times')){
				_times = Number(obj['times']);
			}
			if(obj.hasOwnProperty('link')){
				_link = obj['link'].toString();
			}
			if(obj.hasOwnProperty('display')){
				_display = Number(obj['display']);
			}
			if(obj.hasOwnProperty('interval')){
				_interval = Number(obj['interval']);
				if(_interval <= _display)_interval = _interval + _display;
			}
			_tipTxtField.styleSheet = cssStyle;
			_tipTxtField.htmlText = _tipTxt;
			if(_times >= 0){
				showTip();
				if (_times != 1) _inter = setInterval(showTip, _interval);
			} else {
				hideTip(0);
			}
			//{hasPlugin:是否有插件,txt:'提示文字',interval:提示频率(毫秒),times:提示次数(0为无限)};
		}
		private function showTip():void{
			this.visible = true;
			hideTip(_display);
			if (_times > 0) {
				_count++;
				if (_count == _times) clearInterval(_inter);
			}
		}
		public function hideTip(num:Number = 7000):void{
			var self = this;
			ct = setTimeout(function():void{
				self.visible = false;
			},num);	
		}
		public function tipTxtLinkHandler(e:TextEvent):void{
			if(e.text == 'download'){
				if(_link.length > 0){
					UtilsMethod.getURL(_link,'_self');
				}
				ExternalInterface.call('IkanPlayer.triggerPlugin');
			} else if(e.text == 'options'){
				this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SHOW_BROADCAST));
			}
		}
		public function onCloseHandler(e:MouseEvent):void {
			hideTip(1);
		}
		public function resize(w:Number, h:Number):void {
//			this["bg_mc"].width = w;
//			this["info_mc"].x = Math.round((this["bg_mc"].width - this["info_mc"].width) / 2);
//			this["close_btn"].x = this["info_mc"].x + this["info_mc"].width;
//			this.y = h - this.height;
//			closeBtnMc.x = this.width - closeBtnMc.width - 4;
//			closeBtnMc.y = 3;
			this["bg_mc"].width = w;
			closeBtnMc.x = this["bg_mc"].width - closeBtnMc.width - 4;
			this.y = h - this["bg_mc"].height;
		}
	}
}