package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class SoundUI extends MovieClip {
		
		private var _muteMc:MovieClip;
		private var _unmuteMc:MovieClip;
		private var _soundChangeMc:MovieClip;
		private var _status:String = "unmute";
		
		public function SoundUI() {
			init();
		}
		
		private function init():void {
			_muteMc = this.getChildByName("MuteMc") as MovieClip;
			_unmuteMc = this.getChildByName("UnmuteMc") as MovieClip;
			_soundChangeMc = this.getChildByName("SoundChangeMc") as MovieClip;
			_soundChangeMc.addEventListener(SkinEvent.MEDIA_SOUND, onChangHandler);
			goto(_soundChangeMc.volume);
			_muteMc.visible = false;
			_unmuteMc.visible = true;
			_muteMc.buttonMode = true;
			_unmuteMc.buttonMode = true;
			_muteMc.addEventListener(MouseEvent.CLICK, onClickHandler);
			_unmuteMc.addEventListener(MouseEvent.CLICK, onClickHandler);
		}
		
		private function onChangHandler(e:SkinEvent):void {
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SOUND, { "value":e.currObj["value"] } ));
		}
		
		private function onClickHandler(e:MouseEvent):void {
			switch (e.currentTarget) {
				case _muteMc:
					setSoundState();
					_soundChangeMc.setUnmute();
					_status = "unmute";
					break;
				case _unmuteMc:
					setSoundState(false);
					_soundChangeMc.setMute();
					_status = "mute";
					break;
			}	
		}
		
		public function setSoundState(bool:Boolean = true):void {
			if (bool) {
				_muteMc.visible = false;
				_unmuteMc.visible = true;
			} else {
				_muteMc.visible = true;
				_unmuteMc.visible = false;
			}
		}
		
		public function goto(num:Number):void {
			if (num < 25) {
				_unmuteMc["line_mc"].gotoAndStop(1);
			} else if (num >= 25 && num < 50) {
				_unmuteMc["line_mc"].gotoAndStop(2);
			} else if (num >= 50 && num < 75) {
				_unmuteMc["line_mc"].gotoAndStop(3);
			} else {
				_unmuteMc["line_mc"].gotoAndStop(4);
			}
		}
		
		public function setSlider(value:Number):void {
			_soundChangeMc.setSlider(value);
		}
		
		public function get status():String { return _status; }
		
	}

}