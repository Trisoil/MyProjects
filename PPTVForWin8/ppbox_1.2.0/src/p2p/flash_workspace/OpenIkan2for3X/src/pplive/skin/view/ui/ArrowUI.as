package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.display.Shape;
	import flash.external.ExternalInterface;
	import flash.filters.BitmapFilter;
	import flash.filters.BitmapFilterQuality;
	import flash.filters.DropShadowFilter;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class ArrowUI extends MovieClip {
		
		private var _arrow:Shape;
		
		public function ArrowUI(w:Number, h:Number) {
			this.graphics.clear();
			this.graphics.beginFill(0xff0000, 0);
			this.graphics.drawRoundRectComplex(0, 0, w, h, 5, 0, 5, 0);
			this.graphics.endFill();
			_arrow = new Shape();
			_arrow.graphics.clear();
			_arrow.graphics.beginFill(0xffffff, 1);
			_arrow.graphics.moveTo(10, 0);
			_arrow.graphics.lineTo(10, 4);
			_arrow.graphics.lineTo(4, 10);
			_arrow.graphics.lineTo(10, 16);
			_arrow.graphics.lineTo(10, 20);
			_arrow.graphics.lineTo(0, 10);
			_arrow.graphics.lineTo(10, 0);
			_arrow.graphics.endFill();
			this.addChild(_arrow);
			_arrow.x = w - _arrow.width - 5;
			_arrow.y = (h - _arrow.height) / 2;
		}
		
		public function setFilter(bool:Boolean = true):void {
			if (bool) {
				_arrow.filters = null;
			} else {
				_arrow.filters = [getBitmapFilter()];
			}
		}
		
		private function getBitmapFilter():BitmapFilter {
            var _color_:Number = 0x000000;
            var _angle_:Number = 0;
            var _alpha_:Number = 1;
            var _blurX_:Number = 8;
            var _blurY_:Number = 8;
            var _distance_:Number = 0;
            var _strength_:Number = 1.5;
            var _inner_:Boolean = false;
            var _knockout_:Boolean = false;
            var _quality_:Number = BitmapFilterQuality.MEDIUM;
            return new DropShadowFilter(_distance_,
                                        _angle_,
                                        _color_,
                                        _alpha_,
                                        _blurX_,
                                        _blurY_,
                                        _strength_,
                                        _quality_,
                                        _inner_,
                                        _knockout_);
        }
		
	}

}