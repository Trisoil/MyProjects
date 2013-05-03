package pplive.skin.view.ui {
	
	import flash.display.Loader;
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.MouseEvent;
	import flash.events.TextEvent;
	import flash.external.ExternalInterface;
	import flash.net.navigateToURL;
	import flash.net.URLRequest;
	import flash.system.System;
	import pplive.skin.view.events.SkinEvent;
	
	/**
	 * ...
	 * @author wuwl
	 */
	public class ErrorUI extends MovieClip {
		
		private var _recommend_mc:MovieClip;
		private var _tip_mc:MovieClip;
		private var _error_mc:MovieClip;
		private var _jsonObj:Object;
		private var _groupArr:Array;
		
		public function ErrorUI() {
			init();
		}
		
		private function init():void {
			this.visible = false;
			_groupArr = new Array();
			_error_mc = this.getChildByName("error_mc") as MovieClip;
			_tip_mc = _error_mc.getChildByName("tip_mc") as MovieClip;
			_recommend_mc = _error_mc.getChildByName("recommend_mc") as MovieClip;
		}
		
		public function hideError():void {
			for (var i:int = 0, len:int = _recommend_mc.numChildren; i < len; i++) {
				if (_recommend_mc.getChildByName("group_" + i)) {
					_recommend_mc.removeChild(_recommend_mc.getChildByName("group_" + i));
				}
			}
			this["bg_mc"].graphics.clear();
			this.visible = false;
		}
		
		public function showError(w:Number, h:Number, obj:Object = null):void {
			trace(obj.title);
			if (obj == null) return;
			if (this.visible) return;
			this.visible = true;
			this["bg_mc"].graphics.clear();
			this["bg_mc"].graphics.beginFill(0x000000, .3);
			this["bg_mc"].graphics.drawRect(0, 0, w, h);
			this["bg_mc"].graphics.endFill();
			_jsonObj = obj;
			if (_jsonObj.hasOwnProperty("title")) {
				if (String(_jsonObj["title"]).indexOf("http://") != -1) {
					_tip_mc["mc_0"].visible = false;
					_tip_mc["mc_1"].visible = true;
					//_tip_mc["mc_1"]["tipTxt"].mouseEnabled = false;
					//_tip_mc["mc_1"]["httpTxt"].mouseEnabled = false;
					_tip_mc["mc_1"]["tipTxt"].htmlText = String(_jsonObj["title"]).split("http://")[0];
					_tip_mc["mc_1"]["httpTxt"].text = "http://" + String(_jsonObj["title"]).split("http://")[1];
					_tip_mc["mc_1"]["copy_btn"].addEventListener(MouseEvent.CLICK, function(e:MouseEvent):void { 
						System.setClipboard(_tip_mc["mc_1"]["httpTxt"].text);		
						_tip_mc["mc_1"]["tipTxt"].text = "链接复制成功，您可以在IE浏览器打开了！";
					} );
					_tip_mc["mc_1"]["close_btn"].addEventListener(MouseEvent.CLICK, function(e:MouseEvent):void { _error_mc["parent"].visible = false; } );
				} else {
					_tip_mc["mc_1"].visible = false;
					_tip_mc["mc_0"].visible = true;
					//_tip_mc["mc_0"]["tipTxt"].mouseEnabled = false;
					_tip_mc["mc_0"]["tipTxt"].htmlText = String(_jsonObj["title"]);
					_tip_mc["mc_0"]["tipTxt"].addEventListener(TextEvent.LINK, onTextLinkHandler);
					_tip_mc["mc_0"]["close_btn"].addEventListener(MouseEvent.CLICK, function(e:MouseEvent):void { _error_mc["parent"].visible = false; } );
				}
			}
			if (_jsonObj.hasOwnProperty("portal") && _jsonObj["portal"] != null && _jsonObj["portal"] != "") {
				_recommend_mc.visible = true;
				_recommend_mc.y = _tip_mc.y + _tip_mc.height;
				var _dis:Number;
				for (var i:int = 0, len:int = _jsonObj["portal"].length; i < len; i++) {
					_groupArr[i] = new Group();
					_groupArr[i].name = "group_" + i;
					_groupArr[i]["logo_mc"].visible = false;
					_groupArr[i].y = 40;
					_groupArr[i]["title_txt"].textColor = 0x999999;
					_groupArr[i]["title_txt"].text = _jsonObj["portal"][i]["title"];
					loadImage(_jsonObj["portal"][i]["pic"], _groupArr[i]["image_mc"]);
					_recommend_mc.addChild(_groupArr[i]);
					_dis = (_recommend_mc.width - (_groupArr[i].width * len)) / (len + 1);
					if (i > 0) {
						_groupArr[i].x = _groupArr[i - 1].x + _groupArr[i - 1].width + _dis;
					} else {
						_groupArr[i].x = _dis;
					}
					_groupArr[i].buttonMode = true;
					_groupArr[i].mouseChildren = false;
					_groupArr[i].link = _jsonObj["portal"][i]["link"];
					_groupArr[i].addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
					_groupArr[i].addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
					_groupArr[i].addEventListener(MouseEvent.CLICK, onClickHandler);
				}
			} else {
				_recommend_mc.visible = false;
				_recommend_mc.y = 0;
			}
			resize(w, h);
		}
		
		private function onTextLinkHandler(e:TextEvent):void {
			this.dispatchEvent(new SkinEvent(SkinEvent.PLUGIN_VERSION));
		}
		
		private function onClickHandler(e:MouseEvent):void {
			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_RECOMMEND, { "value":e.target.link.toString() } ));
		}
		
		private function onOverHandler(e:MouseEvent):void {
			e.target["logo_mc"].visible = true;
			e.target["title_txt"].textColor = 0xffffff;
		}
		
		private function onOutHandler(e:MouseEvent):void {
			e.target["logo_mc"].visible = false;
			e.target["title_txt"].textColor = 0x999999;
		}
	
		private function loadImage(url:String, mc:MovieClip):void {
			var _loader:Loader = new Loader();
			_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onCompleteHandler);
			_loader.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, onErrorHandler);
			_loader.load(new URLRequest(url));
			function onCompleteHandler(e:Event):void {
				mc.addChild(_loader);
				_loader.x = 4;
				_loader.y = 4;
				_loader.width = Math.round(mc.width * .76);
				_loader.height = Math.round(mc.height * .76);
			}
			function onErrorHandler(e:IOErrorEvent):void {
				
			}
		}
		
		public function resize(w:Number, h:Number):void {
			if (this.visible) {
				_error_mc.x = (w - _error_mc.width) / 2;
				_error_mc.y = (h - _error_mc.height) / 2;
				this["bg_mc"].graphics.clear();
				this["bg_mc"].graphics.beginFill(0x000000, .3);
				this["bg_mc"].graphics.drawRect(0, 0, w, h);
				this["bg_mc"].graphics.endFill();
			} else {
				this["bg_mc"].graphics.clear();
			}
		}
		
	}

}