package pplive.skin.view.ui{
	
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	import flash.external.ExternalInterface;
	import flash.geom.Rectangle;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class BrightnessChangeUI extends MovieClip {
		
		private var _highSd:MovieClip;
		private var _highDot:MovieClip;
		private var _highSlider:MovieClip;
		private var _light:Number;
		
		private var _confirm_btn:SimpleButton;
		private var _default_btn:SimpleButton;
		private var _drag_mc:MovieClip;
		
		private var _bt:Number = 0;
		private var _ct:Number = 0;
		private var _bt_:Number = 0;
		private var _ct_:Number = 0;
		private var _typeArr:Array;
		//brightness_mc  contrast_mc  bar_mc  slider_mc  drag_mc
		public function BrightnessChangeUI() {
			init();
		}
		
		private function init():void {
			_typeArr = [this["bt_mc"], this["ct_mc"]];
			_confirm_btn = this.getChildByName("confirm_btn") as SimpleButton;
			_default_btn = this.getChildByName("default_btn") as SimpleButton;
			_confirm_btn.addEventListener(MouseEvent.CLICK, onClickHandler);
			_default_btn.addEventListener(MouseEvent.CLICK, onClickHandler);
			for (var i:int = 0, len:int = _typeArr.length; i<len; i++) {
				var _rect_mc:MovieClip = new MovieClip();
				_rect_mc.graphics.clear();
				_rect_mc.graphics.beginFill(0xFF0000, 0);
				_rect_mc.graphics.drawRect( -10, 0, _typeArr[i]["bar_mc"].width + 20, _typeArr[i]["bar_mc"].height);
				_rect_mc.graphics.endFill();
				_typeArr[i].addChild(_rect_mc);
				_typeArr[i].swapChildren(_rect_mc, _typeArr[i]["drag_mc"]);
				_rect_mc.addEventListener(MouseEvent.CLICK, onChangeHandler);
				_typeArr[i]["drag_mc"].buttonMode = true;
				_typeArr[i]["drag_mc"].addEventListener(MouseEvent.MOUSE_DOWN, onDragHandler);
			}
		}
		
		private function onDragHandler(e:MouseEvent):void {
			_drag_mc = e.currentTarget as MovieClip;
			_drag_mc.startDrag(false, new Rectangle(_drag_mc["parent"]["bar_mc"].x - _drag_mc.width / 2, _drag_mc.y, _drag_mc["parent"]["bar_mc"].width, 0));
			_drag_mc.addEventListener(MouseEvent.MOUSE_UP, onDragUpHandler);
			_drag_mc.stage.addEventListener(MouseEvent.MOUSE_UP, onDragUpHandler);
			_drag_mc.addEventListener(MouseEvent.MOUSE_MOVE, onDragMoveHandler);
			_drag_mc.stage.addEventListener(MouseEvent.MOUSE_MOVE, onDragMoveHandler);
		}
		
		private function onDragMoveHandler(e:MouseEvent):void {
			_drag_mc["parent"]["slider_mc"].width = _drag_mc.x + _drag_mc.width / 2;
			dispatch(_drag_mc);
		}
		
		private function onDragUpHandler(e:MouseEvent):void {
			_drag_mc.stopDrag();
			_drag_mc.removeEventListener(MouseEvent.MOUSE_UP, onDragUpHandler);
			_drag_mc.stage.removeEventListener(MouseEvent.MOUSE_UP, onDragUpHandler);
			_drag_mc.removeEventListener(MouseEvent.MOUSE_MOVE, onDragMoveHandler);
			_drag_mc.stage.removeEventListener(MouseEvent.MOUSE_MOVE, onDragMoveHandler);
		}
		
		private function onChangeHandler(e:MouseEvent):void {
			if (e.target["parent"].mouseX <= 0) {
				e.target["parent"]["slider_mc"].width = 0;
			} else if (e.target["parent"].mouseX >= e.target["parent"]["bar_mc"].width) {
				e.target["parent"]["slider_mc"].width = e.target["parent"]["bar_mc"].width;
			} else {
				e.target["parent"]["slider_mc"].width = e.target["parent"].mouseX;
			}
			e.target["parent"]["drag_mc"].x = e.target["parent"]["slider_mc"].width - e.target["parent"]["drag_mc"].width / 2;
			dispatch(e.target);
		}
		
		private function dispatch(target:*):void {
			var dis:Number = Math.round(target["parent"]["slider_mc"].width / target["parent"]["bar_mc"].width * 100);
			dis = dis * 2 - 100;
			if (target["parent"].name == "bt_mc") {
				_bt_ = dis;
			} else if (target["parent"].name == "ct_mc") {
				_ct_ = dis;
			}
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_BRIGHTNESS, { "bt":_bt_, "ct":_ct_ } ));
		}
		
		private function onClickHandler(e:MouseEvent):void {
			if (e.target == _default_btn) {
				_bt_ = 0;
				_ct_ = 0;
			}
			_bt = _bt_;
			_ct = _ct_;
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_BRIGHTNESS, { "bt":_bt, "ct":_ct, "bool":true } ));
		}
		
		public function setBrightness(bt:Number, ct:Number):void {
			this["bt_mc"]["slider_mc"].width = (bt + 100) / 2 / 100 * this["bt_mc"]["bar_mc"].width;
			this["bt_mc"]["drag_mc"].x = this["bt_mc"]["slider_mc"].width - this["bt_mc"]["drag_mc"].width / 2;
			this["ct_mc"]["slider_mc"].width = (ct + 100) / 2 / 100 * this["ct_mc"]["bar_mc"].width;
			this["ct_mc"]["drag_mc"].x = this["ct_mc"]["slider_mc"].width - this["ct_mc"]["drag_mc"].width / 2;
		}
		
		public function get bt():Number { return _bt; }
		public function get ct():Number { return _ct; }
		
	}

}