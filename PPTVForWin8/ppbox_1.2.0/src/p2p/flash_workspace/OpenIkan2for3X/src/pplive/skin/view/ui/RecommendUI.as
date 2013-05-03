package pplive.skin.view.ui {
	
	import flash.display.GradientType;
	import flash.display.Loader;
	import flash.display.MovieClip;
	import flash.display.SimpleButton;
	import flash.display.SpreadMethod;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.MouseEvent;
	import flash.geom.Matrix;
	import flash.net.URLRequest;
	import flash.text.TextField;
	import flash.utils.clearInterval;
	import flash.utils.setInterval;
	
	import pplive.skin.view.events.SkinEvent;
	import pplive.skin.view.utils.UtilsMethod;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class RecommendUI extends MovieClip {
		public var Name:String = "recommend";
		private var _recommend_mc:MovieClip;
		private var _img_mc:MovieClip;
		private var _title_txt:TextField;
		private var _input_txt:TextField;
		private var _list_mc:MovieClip;
		private var _prev_btn:SimpleButton;
		private var _next_btn:SimpleButton;
		private var _replay_mc:SimpleButton;
		private var _search_mc:MovieClip;
		private var _index:int = 0;
		private var _list_img_mc:MovieClip;
		private var _listArr:Array;
		private var _inter:uint;
		private var _isReverse:Boolean = false;
		private var _rearchBg:MovieClip;
		private var _updateTime:Number = 6000;
		private var ulList:Array = [];
		
		public function RecommendUI() {
			init();
		}
		
		private function init():void {
			this.visible = false;
			_recommend_mc = this.getChildByName("recommend_mc") as MovieClip;
			_img_mc = _recommend_mc.getChildByName("img_mc") as MovieClip;
			_title_txt = _recommend_mc.getChildByName("title_txt") as TextField;
			_search_mc = _recommend_mc.getChildByName("search_mc") as MovieClip;
			_input_txt = _recommend_mc.getChildByName("input_txt") as TextField;
			_list_mc = _recommend_mc.getChildByName("list_mc") as MovieClip;
			_rearchBg = _recommend_mc.getChildByName("search_bg") as MovieClip;
			_prev_btn = _recommend_mc.getChildByName("prev_btn") as SimpleButton;
			_next_btn = _recommend_mc.getChildByName("next_btn") as SimpleButton;
			_replay_mc = _recommend_mc.getChildByName("replay_mc") as SimpleButton;
			_img_mc.buttonMode = true;
			_img_mc.addEventListener(MouseEvent.CLICK, onClickHandler);
			_replay_mc.addEventListener(MouseEvent.CLICK, onClickHandler);
			_search_mc.buttonMode = true;
			_search_mc.addEventListener(MouseEvent.CLICK, onClickHandler);
			_prev_btn.addEventListener(MouseEvent.CLICK, onClickHandler);
			_next_btn.addEventListener(MouseEvent.CLICK, onClickHandler);
			_prev_btn.addEventListener(MouseEvent.MOUSE_OVER, onListOverHandler);
			_prev_btn.addEventListener(MouseEvent.MOUSE_OUT, onListOutHandler);
			_next_btn.addEventListener(MouseEvent.MOUSE_OVER, onListOverHandler);
			_next_btn.addEventListener(MouseEvent.MOUSE_OUT, onListOutHandler);
			ulList.push([_recommend_mc,_img_mc,_title_txt,_search_mc,_input_txt,_list_mc,_rearchBg,_prev_btn,_next_btn,_replay_mc])
		}
		
		private function onListOverHandler(e:MouseEvent):void {
			if (_inter) {
				clearInterval(_inter);
			}
		}
		
		private function onListOutHandler(e:MouseEvent):void {
			update();
		}
		
		private function onClickHandler(e:MouseEvent):void {
			switch(e.currentTarget) {
				case _img_mc:
				case _replay_mc:
					hideRecommend();
					this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
					break;
				case _search_mc:
					UtilsMethod.getURL("http://search.pptv.com/s_video/?kw=" + encodeURI(_input_txt.text));
					break;
				case _prev_btn:
					if (_index > 0) {
						_index--;
					}		
					break;
				case _next_btn:
					if (_index < Math.ceil(_listArr.length / 3) - 1) {
						_index++;
					}
					break;
			}
		}
		private function onFrameHandler(ev:Event):void {			
			_list_img_mc.y -= (_list_img_mc.y + 300 * _index) * 0.25;
		}
		
		public function showRecommend(obj:Object = null, w:Number = 0, h:Number = 0):void {
			if (obj == null) return;
			if (this.visible) return;
			this.visible = true;
			this["bg_mc"].graphics.clear();
			var fillType:String = GradientType.LINEAR;
			var colors:Array = [0x222222, 0x000000];
			var alphas:Array = [100, 100];
			var ratios:Array = [0x00, 0xFF];
			var matr:Matrix = new Matrix();
			matr.createGradientBox(w, 100, Math.PI / 2, 0, 0);
			var spreadMethod:String = SpreadMethod.PAD;
			this["bg_mc"].graphics.beginGradientFill(fillType, colors, alphas, ratios, matr, spreadMethod);
			this["bg_mc"].graphics.drawRect(0, 0, w, h);
			this["bg_mc"].graphics.endFill();
			_title_txt.text = obj["data"]["now"]["titile"];
			_input_txt.text = obj["data"]["now"]["titile"];
			if (obj["data"]["now"]["catpure"].indexOf("http://") != -1) {
				var _loader:Loader = new Loader();
				_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onComImageHandler);
				_loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIOErrorImageHandler);
				_loader.load(new URLRequest(obj["data"]["now"]["catpure"]));
				function onIOErrorImageHandler(e:IOErrorEvent):void {
					trace("");
				}
				function onComImageHandler(e:Event):void {
					_img_mc.addChild(_loader);
					_loader.name = "now_loader";
					_loader.x = 3;
					_loader.y = 3;
					_loader.width = 114;
					_loader.height = 74;
				}
			}	
			_list_img_mc = new MovieClip();
			_list_mc.addChild(_list_img_mc);
			var _list_mask_mc:MovieClip = new MovieClip();
			_list_mc.addChild(_list_mask_mc);
			_list_mask_mc.graphics.clear();
			_list_mask_mc.graphics.beginFill(0x00ff00, .5);
			_list_mask_mc.graphics.drawRect(0, 0, 295, 295);
			_list_mask_mc.graphics.endFill();
			_list_img_mc.mask = _list_mask_mc;
			_listArr = obj["data"]["videos"] as Array;
			var _aloneArr:Array = [];
			for (var i:int = 0; i < _listArr.length; i++) {
				_aloneArr[i] = new AloneMc();
				_list_img_mc.addChild(_aloneArr[i]);
				_aloneArr[i].index = i;
				_aloneArr[i].link = _listArr[i]["link"];
				var _loader_:Loader = new Loader();
				_aloneArr[i]["img_mc"].addChild(_loader_);
				_loader_.contentLoaderInfo.addEventListener(Event.COMPLETE, onImageHandler);
				_loader_.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onIOErrorHandler);
				_loader_.load(new URLRequest(_listArr[i]["catpure"]));
				function onIOErrorHandler(e:IOErrorEvent):void {
					trace("");
				}
				function onImageHandler(e:Event):void {
					e.target.loader.width = 100;
					e.target.loader.height = 75;
				}				
				_aloneArr[i]["title_txt"].text = _listArr[i]["title"];
				_aloneArr[i]["comment_txt"].text = "评论：" + _listArr[i]["comment"];
				_aloneArr[i].y = i * 100;
				_aloneArr[i].mouseChildren = false;
				_aloneArr[i].buttonMode = true;
				_aloneArr[i].addEventListener(MouseEvent.MOUSE_OVER, onMouseHandler);
				_aloneArr[i].addEventListener(MouseEvent.MOUSE_OUT, onMouseHandler);
				_aloneArr[i].addEventListener(MouseEvent.CLICK, onMouseHandler);
			}
			if (_listArr.length > 3) {
				_list_img_mc.addEventListener(Event.ENTER_FRAME, onFrameHandler);
				update();
			}	
			function onMouseHandler(e:MouseEvent):void {
				switch(e.type) {
					case MouseEvent.MOUSE_OVER:
						if (_inter) {
							clearInterval(_inter);
						}
						e.target.gotoAndStop(2);
						break;
					case MouseEvent.MOUSE_OUT:
						update();
						e.target.gotoAndStop(1);
						break;
					case MouseEvent.CLICK:
						UtilsMethod.getURL(e.target.link, "_self");
						break;
				}
			}
			resize(w, h);
		}
		
		private function update():void {
			_inter = setInterval(function():void {
					if (_index < Math.ceil(_listArr.length / 3) - 1 && _isReverse == false) {
						_index++;
					} else {
						_isReverse = true;
						_index--;
						if (_index == 0) {
							_isReverse = false;
						}
					}					
				}, _updateTime);
		}
		
		public function hideRecommend():void {
			if (_inter) {
				clearInterval(_inter);
			}
			if (_list_img_mc && _list_img_mc.hasEventListener(Event.ENTER_FRAME)) {
				_list_img_mc.removeEventListener(Event.ENTER_FRAME, onFrameHandler);
				_list_img_mc.y = 0;
			}
			while (_list_mc.numChildren > 0) {
				_list_mc.removeChildAt(0);
			}
			if (_img_mc.numChildren > 1 && _img_mc.getChildByName("now_loader")) {
				_img_mc.removeChild(_img_mc.getChildByName("now_loader"));
			}			
			_list_img_mc = null;
			_listArr = null;
			_index = 0;
			this["bg_mc"].graphics.clear();
			this.visible = false;
		}
		private function showALL(v:Boolean = true):void{
			for(var i:int=0;i<ulList.length;i++){
				ulList[i].visible = v ? true : false;
			}
		}
		public function resize(w:Number, h:Number):void {
			if (this.visible) {
				if(h < 300){
					showALL(false);
					_img_mc.visible = true;
					_title_txt.visible = true;
					_replay_mc.visible = true;
				} else if(w < 400 && h >= 300){
					showALL(false);
					_list_mc.visible = true;
					_prev_btn.visible = true;
					_next_btn.visible = true;
				} else {
					showALL();
				}
				_recommend_mc.x = (w - _recommend_mc["bg_mc"].width) / 2;
				_recommend_mc.y = (h - _recommend_mc["bg_mc"].height) / 2;
				this["bg_mc"].graphics.clear();
				var fillType:String = GradientType.LINEAR;
				var colors:Array = [0x222222, 0x000000];
				var alphas:Array = [100, 100];
				var ratios:Array = [0x00, 0xFF];
				var matr:Matrix = new Matrix();
				matr.createGradientBox(w, 100, Math.PI / 2, 0, 0);
				var spreadMethod:String = SpreadMethod.PAD;
				this["bg_mc"].graphics.beginGradientFill(fillType, colors, alphas, ratios, matr, spreadMethod);
				this["bg_mc"].graphics.drawRect(0, 0, w, h);
				this["bg_mc"].graphics.endFill();
			} else {
				this["bg_mc"].graphics.clear();
			}
		}
		
	}

}