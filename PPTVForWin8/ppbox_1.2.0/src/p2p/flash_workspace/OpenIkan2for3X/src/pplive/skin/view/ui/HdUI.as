package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	import flash.external.ExternalInterface;
	import flash.text.TextField;
	import flash.utils.clearTimeout;
	import flash.utils.setTimeout;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class HdUI extends MovieClip {
		
		public var _hd_inter:uint;
		private var _delayTime:Number = 500;
		private var _hdBtn:SimpleButton;
		private var _smoothMc:MovieClip;
		
		public function HdUI() {
			_hdBtn = this.getChildByName("HdBtn") as SimpleButton;
			_smoothMc = this.getChildByName("SmoothMc") as MovieClip;
			init();
		}
		
		private function init():void {
			this.visible = true;
			this["changeSmoothTxt"].mouseEnabled = false;
			_hdBtn.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
			_hdBtn.addEventListener(MouseEvent.CLICK, onClickHandler);
			_smoothMc.addEventListener(SkinEvent.MEDIA_SHOW_CHANGE, onChangeHandler);
			_smoothMc.addEventListener(SkinEvent.MEDIA_HIDE_CHANGE, onChangeHandler);
			_smoothMc.addEventListener(SkinEvent.MEDIA_CHANGE, onChangeHandler);
		}
		
		private function onChangeHandler(e:SkinEvent):void {
			switch (e.type) {
				case SkinEvent.MEDIA_SHOW_CHANGE:
					clearInter();
					_smoothMc.visible = true;
					break;
				case SkinEvent.MEDIA_HIDE_CHANGE:
					_hd_inter = setTimeout(hideLight, _delayTime);
					break;
				case SkinEvent.MEDIA_CHANGE:					
					setHd(_smoothMc.status);
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_CHANGE, { "value":_smoothMc.status } ));
					break;
			}
		}
		
		public function setHd(num:Number):void {
			_smoothMc.status = num;
			_smoothMc.setState(num);
			if (num == 0) {
				setTxt("流畅");
			} else if (num == 1) {
				setTxt("清晰");
			} else if (num == 2) {
				setTxt("高清");
			}
		}
		
		private function setTxt(str:String):void {
			if (str != null && str != "") {
				this["changeSmoothTxt"].text = str;
			}
		}
		private function onOutHandler(e:MouseEvent):void {
			_hd_inter = setTimeout(hideLight, _delayTime);
		}
		
		private function hideLight():void {
			_smoothMc.visible = false;
		}
		
		public function clearInter():void {
			if (_hd_inter) {
				clearTimeout(_hd_inter);
			}
		}
		
		private function onClickHandler(e:MouseEvent):void {
			_smoothMc.visible = true;
		}
		
	}

}