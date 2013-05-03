package pplive.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.events.MouseEvent;
	
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuweiliang1006@sohu.com
	 */
	public class AdjustUI extends MovieClip {
		
		private var _close_btn:SimpleButton;
		private var _tab_mc:MovieClip;
		private var _box_mc:MovieClip;
		private var _mcArr:Array;
		
		public function AdjustUI() {
			_close_btn = this.getChildByName("close_btn") as SimpleButton;
			_close_btn.addEventListener(MouseEvent.CLICK, onCloseHandler);
			_tab_mc = this.getChildByName("tab_mc") as MovieClip;
			_box_mc = this.getChildByName("box_mc") as MovieClip;
			_mcArr = [];
			for (var i:int = 0, len:int = _tab_mc.numChildren; i < len; i++) {
				(_tab_mc.getChildAt(i) as MovieClip).visible = false;
				(_tab_mc.getChildAt(i) as MovieClip).buttonMode = true;
				(_tab_mc.getChildAt(i) as MovieClip).addEventListener(MouseEvent.CLICK, onChangeHandler);
				_box_mc[_tab_mc.getChildAt(i).name].addEventListener(SkinEvent.MEDIA_BRIGHTNESS, onColorHandler);
				_box_mc[_tab_mc.getChildAt(i).name].addEventListener(SkinEvent.MEDIA_SKIP_PLAY, onColorHandler);
				_mcArr.push( { "btn":_tab_mc.getChildAt(i), "mc":_box_mc[_tab_mc.getChildAt(i).name] } );				
			}
			var j:int = 0;
			while (j < _box_mc.numChildren){
				_box_mc.getChildAt(j).visible = false;
				j++;
			}
		}
		
		private function onColorHandler(e:SkinEvent):void {
			switch(e.type) {
				case SkinEvent.MEDIA_BRIGHTNESS:
					if (e.currObj["bool"]) {
						_box_mc["brightness_mc"].setBrightness(_box_mc["brightness_mc"].bt, _box_mc["brightness_mc"].ct);
						this.visible = false;
					}
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_BRIGHTNESS, e.currObj ));
					break;
				case SkinEvent.MEDIA_SKIP_PLAY:
					if (e.currObj && e.currObj.hasOwnProperty("bool")) {
						this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_SKIP_PLAY, e.currObj ));
					}
					this.visible = false;
					break;
			}
		}
		
		private function onChangeHandler(e:MouseEvent):void {
			var _index:int = _tab_mc.getChildIndex(e.currentTarget as MovieClip);
			reset(_index);
		}
		
		private function onCloseHandler(e:MouseEvent):void {
			_box_mc["broadcast_mc"].isSelect = _box_mc["broadcast_mc"].isSelect;
			_box_mc["brightness_mc"].setBrightness(_box_mc["brightness_mc"].bt, _box_mc["brightness_mc"].ct);
			this.visible = false;
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_BRIGHTNESS, { "bool":false } ));
		}
		public function hideTab(index:int = 0):void{
			for (var i:int = 0, len:int = _mcArr.length; i < len; i++) {
				if (i == index) {
					_mcArr[i]["btn"].visible = false;
					_mcArr[i]["mc"].visible = false;
				} else {
					_mcArr[i]["mc"].visible = true;
				}
			}
		}
		public function reset(index:int):void {
			for (var i:int = 0, len:int = _mcArr.length; i < len; i++) {
				if (i == index) {
					_mcArr[i]["btn"].gotoAndStop(2);
					_mcArr[i]["mc"].visible = true;
				} else {
					_mcArr[i]["btn"].gotoAndStop(1);
					_mcArr[i]["mc"].visible = false;
				}
			}
		}
		
		public function showTab(name:String):void {
			var _name:String = name.toLocaleLowerCase() + "_mc";
			if (_tab_mc.getChildByName(_name)) {
				_tab_mc.getChildByName(_name).visible = true;
			}			
		}
		
		public function setBrightness(bt:Number, ct:Number):void {
			_box_mc["brightness_mc"].setBrightness(bt, ct);
		}
		
		public function setSelect(bool:Boolean):void {
			_box_mc["broadcast_mc"].isSelect = bool;
		}
		
	}

}