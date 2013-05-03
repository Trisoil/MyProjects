package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class SoundChangeUI extends MovieClip {
		
		private var _soundSlider:MovieClip;
		private var _soundSd:MovieClip;
		private var _soundDot:MovieClip;
		private var _volumeWidth:Number;
		private var _volume:Number;
		
		public function SoundChangeUI() {
			init();
		}
		
		private function init():void {
			_soundSlider = this["soundSliderMc"];
			_soundSd = this["soundsdMc"];
			_soundDot = this["soundDotMc"];
			_soundDot.buttonMode = true;
			_soundDot.addEventListener(MouseEvent.CLICK, onDotClickHandler);
			_soundSlider.width = _soundSd.width / 2;
			_volumeWidth = _soundSlider.width;
			setSound(_volumeWidth);
		}
		
		private function onDotClickHandler(e:MouseEvent):void {
			if (this.mouseX < _soundSd.x) {
				_soundSlider.width = 0;
				setSound(0);
			} else if (this.mouseX > _soundSd.x + _soundSd.width) {
				_soundSlider.width = _soundSd.width;
				_volumeWidth = _soundSlider.width;
				setSound(_volumeWidth);
			} else {
				_soundSlider.width = this.mouseX - _soundSd.x;
				_volumeWidth = _soundSlider.width;
				setSound(_volumeWidth);
			}
		}
		
		public function setMute():void {
			_soundSlider.width = 0;
			setSound(0);
		}
		
		public function setUnmute():void {
			_soundSlider.width = _volumeWidth;
			setSound(_volumeWidth);
		}
		
		public function setSlider(value:Number):void {
			_soundSlider.width = value / 100 * _soundSd.width;
			_volumeWidth = _soundSlider.width;
			setSound(_volumeWidth);
		}
		
		private function setSound(W:Number):void {
			_volume = Math.round(W / _soundSd.width * 100);
			_volume = _volume < 100 ? _volume : 100;
			_volume = _volume > 0 ? _volume : 0;
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SOUND, { "value":_volume } ));
		}
		
		public function get volume():Number { return _volume; }
		
	}

}