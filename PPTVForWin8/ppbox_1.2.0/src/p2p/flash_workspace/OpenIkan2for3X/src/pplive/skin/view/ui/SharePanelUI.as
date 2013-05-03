package pplive.skin.view.ui{
	
	import com.adobe.images.JPGEncoder;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.events.MouseEvent;
	import flash.net.FileReference;
	import flash.net.URLRequest;
	import flash.system.System;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFieldType;
	import flash.text.TextFormat;
	import flash.utils.ByteArray;
	import flash.utils.setTimeout;
	
	import pplive.skin.view.events.SkinEvent;
	import pplive.skin.view.ui.CloseBtnUI;
	import pplive.skin.view.ui.CommitBtnUI;
	import pplive.skin.view.ui.RadioBtnUI;
	
	/**
	 * ...
	 * @author pele xiang
	 */
	public class SharePanelUI extends MovieClip {
		
		public static const SHARE_PIC:String = 'sharePic';
		public static const SHARE_VIDEO:String = 'shareVideo';
		
		public var Name:String = "share";
		private var _recommend_mc:MovieClip;
		private var _tip_mc:MovieClip;
		private var _error_mc:MovieClip;
		private var _jsonObj:Object;
		private var _groupArr:Array;
		private var _share_mc:MovieClip;
		
		//新UI
		private var shareVideoMc:Sprite;
		
		private var videoTabCon:Array = [];//视频分享元件列表
		private var picTabCon:Array = [];//图片分享元件列表
		
		private var closeBtnMc:CloseBtnUI;
		private var picMcCon:Sprite;
		private var picBitmap:Bitmap;
		private var picBitmapData:BitmapData;
		private var addressInput:TextField;
		private var tabsp:TabSharePicUI;
		private var tabv:TabVideoUI;
		private var selectedList:Array = [];
		private var tabItems:Array = [];
		
		private var shareText:TextField; //分享到的文字
		private var returnPlayBtn:CommitBtnUI; // 返回播放
		private var returnPlayTxt:TextField;// 返回播放文字
		private var saveBtn:CommitBtnUI;//保存本地文字
		private var saveBtnTxt:TextField;//保存本地文字
		
		public var panelWidth:Number = 420;
		public var panelHeight:Number = 350;
		public var videoAddress:String = 'http://v.pptv.com/show/drxy8FiaibLmx08lo.html';
		public var flashAddress:String = 'http://player.pptv.com/v/drxy8FiaibLmx08lo.swf';
		
		public function SharePanelUI() {
			init();
		}
		
		private function init():void {
			//this.visible = false;
			registerUi();
//			_share_mc = this.getChildByName("share_mc") as MovieClip;
//			_share_mc["videoAddressCopy"].addEventListener(MouseEvent.CLICK, onClickHandler);
//			_share_mc["flashAddressCopy"].addEventListener(MouseEvent.CLICK, onClickHandler);
//			_share_mc["htmlAddressCopy"].addEventListener(MouseEvent.CLICK, onClickHandler);
//			_share_mc["sharefiximg"].buttonMode = true;
//			_share_mc["sharefiximg"].mouseChildren = false;
//			_share_mc["sharefiximg"].addEventListener(MouseEvent.CLICK, onShareFixHandler);
//			_share_mc["shareReplay"].addEventListener(MouseEvent.CLICK, onReplayHandler);
		}
		private function registerUi():void{
			shareVideoMc = new Sprite();
			shareVideoMc.graphics.beginFill(0x000000,.8);
			shareVideoMc.graphics.lineStyle(1,0x666666);
			shareVideoMc.graphics.drawRect(0,0,panelWidth,panelHeight);
			shareVideoMc.graphics.endFill();
			this.addChild(shareVideoMc);
			//图片
			picMcCon = new Sprite();
			picMcCon.y = 38;
			shareVideoMc.addChild(picMcCon);
			picTabCon.push(picMcCon);
			//文字样式
			var tf:TextFormat = new TextFormat();
			tf.color = 0xFFFFFF;
			//关闭按钮 共用部分
			closeBtnMc = new CloseBtnUI();
			closeBtnMc.x = panelWidth - closeBtnMc.width -  8;
			closeBtnMc.y = 5;
			closeBtnMc.addEventListener(MouseEvent.CLICK,onCloseHandler);
			shareVideoMc.addChild(closeBtnMc);
			//提示文字 共用部分
			shareText = new TextField();
			shareText.defaultTextFormat = tf;
			shareText.autoSize = TextFieldAutoSize.LEFT;
			shareText.selectable = false;
			shareText.text = '分享到：';
			shareText.x = 40;
			shareText.y = 120;
			shareVideoMc.addChild(shareText);
			//嵌入代码
			var incodeText:TextField = new TextField();
			incodeText.defaultTextFormat = tf;
			incodeText.autoSize = TextFieldAutoSize.LEFT;
			incodeText.selectable = false;
			incodeText.text = '嵌入代码：';
			incodeText.x = 40;
			incodeText.y = 140;
			shareVideoMc.addChild(incodeText);
			videoTabCon.push(incodeText);
			//flash地址
			var flashAddressTxt:TextField = new TextField();
			flashAddressTxt.defaultTextFormat = tf;
			flashAddressTxt.autoSize =  TextFieldAutoSize.LEFT;
			flashAddressTxt.selectable = false;
			flashAddressTxt.text = 'Flash地址';
			flashAddressTxt.x = 120;
			flashAddressTxt.y = 140;
			shareVideoMc.addChild(flashAddressTxt);
			videoTabCon.push(flashAddressTxt);
			//flashBtn
			var flashBtn:RadioBtnUI = new RadioBtnUI();
			flashBtn.buttonMode = true;
			flashBtn.gotoAndStop(2);
			flashBtn.x = 105;
			flashBtn.y = 144;
			shareVideoMc.addChild(flashBtn);
			selectedList.push(flashBtn);
			videoTabCon.push(flashBtn);
			flashBtn.addEventListener(MouseEvent.CLICK,flashBtnHandler);
			//视频地址
			var videoAddressTxt:TextField = new TextField();
			videoAddressTxt.defaultTextFormat = tf;
			videoAddressTxt.autoSize =  TextFieldAutoSize.LEFT;
			videoAddressTxt.selectable = false;
			videoAddressTxt.text = '视频地址';
			videoAddressTxt.x = 195;
			videoAddressTxt.y = 140;
			shareVideoMc.addChild(videoAddressTxt);
			videoTabCon.push(videoAddressTxt);
			//videoBtn
			var videoBtn:RadioBtnUI = new RadioBtnUI();
			videoBtn.buttonMode = true;
			videoBtn.gotoAndStop(1);
			videoBtn.x = 180;
			videoBtn.y = 144;
			shareVideoMc.addChild(videoBtn);
			selectedList.push(videoBtn);
			videoTabCon.push(videoBtn);
			videoBtn.addEventListener(MouseEvent.CLICK,videoBtnHandler);
			//html地址
			var htmlAddressTxt:TextField = new TextField();
			htmlAddressTxt.defaultTextFormat = tf;
			htmlAddressTxt.autoSize =  TextFieldAutoSize.LEFT;
			htmlAddressTxt.selectable = false;
			htmlAddressTxt.text = 'Html地址';
			htmlAddressTxt.x = 270;
			htmlAddressTxt.y = 140;
			shareVideoMc.addChild(htmlAddressTxt);
			videoTabCon.push(htmlAddressTxt);
			//htmlBtn
			var htmlBtn:RadioBtnUI = new RadioBtnUI();
			htmlBtn.buttonMode = true;
			htmlBtn.gotoAndStop(1);
			htmlBtn.x = 255;
			htmlBtn.y = 144;
			shareVideoMc.addChild(htmlBtn);
			selectedList.push(htmlBtn);
			videoTabCon.push(htmlBtn);
			htmlBtn.addEventListener(MouseEvent.CLICK,htmlBtnHandler);
			//addressInputBg
			var addressInputBg:Sprite = new Sprite();
			addressInputBg.x = 40;
			addressInputBg.y = 170;
			addressInputBg.graphics.beginFill(0xFFFFFF);
			addressInputBg.graphics.lineStyle(1,0x666666);
			addressInputBg.graphics.drawRect(0,0,250,22);
			addressInputBg.graphics.endFill();
			shareVideoMc.addChild(addressInputBg);
			videoTabCon.push(addressInputBg);
			//addressInput
			addressInput = new TextField();
			addressInput.text = 'http://player.pptv.com/v/V6Jp50ib1JWNGxCw.swf';
			addressInput.x = addressInputBg.x + 2;
			addressInput.y = 170;
			addressInput.width = 245;
			addressInput.height = 22;
			addressInput.wordWrap = false;
			addressInput.multiline = false;
			shareVideoMc.addChild(addressInput);
			videoTabCon.push(addressInput);
			//copyBtn
			var copyBtn:CommitBtnUI = new CommitBtnUI();
			copyBtn.x = addressInput.x + addressInput.width + 10;
			copyBtn.y = 169;
			shareVideoMc.addChild(copyBtn);
			videoTabCon.push(copyBtn);
			
			var copyBtnTxt:TextField = new TextField();
			copyBtnTxt.defaultTextFormat = tf;
			copyBtnTxt.autoSize =  TextFieldAutoSize.CENTER;
			copyBtnTxt.selectable = false;
			copyBtnTxt.text = '复制';
			copyBtnTxt.mouseEnabled = false;
			copyBtnTxt.x = copyBtn.x + 20;
			copyBtnTxt.y = copyBtn.y + 4;
			shareVideoMc.addChild(copyBtnTxt);
			copyBtn.addEventListener(MouseEvent.CLICK,copyBtnHandler);
			videoTabCon.push(copyBtnTxt);
			//本地保存
			saveBtn = new CommitBtnUI();
			saveBtn.addEventListener(MouseEvent.CLICK,saveBtnHandler);
			shareVideoMc.addChild(saveBtn);
			saveBtn.x = panelWidth / 2 - (saveBtn.width * 2 + 80) / 2;
			saveBtn.y = panelHeight - saveBtn.height - 10;
			picTabCon.push(saveBtn);
			
			saveBtnTxt = new TextField();
			saveBtnTxt.defaultTextFormat = tf;
			saveBtnTxt.autoSize =  TextFieldAutoSize.CENTER;
			saveBtnTxt.selectable = false;
			saveBtnTxt.mouseEnabled = false;
			saveBtnTxt.x = saveBtn.x + 34;
			saveBtnTxt.y = saveBtn.y + 3;
			saveBtnTxt.text = '本地保存';
			shareVideoMc.addChild(saveBtnTxt);
			picTabCon.push(saveBtnTxt);
			//返回播放 共用
			returnPlayBtn = new CommitBtnUI();
			returnPlayBtn.x = saveBtnTxt.x + 200;
			returnPlayBtn.y = saveBtnTxt.y;
			returnPlayBtn.addEventListener(MouseEvent.CLICK,onCloseHandler);
			shareVideoMc.addChild(returnPlayBtn);
			
			returnPlayTxt = new TextField();
			returnPlayTxt.defaultTextFormat = tf;
			returnPlayTxt.autoSize =  TextFieldAutoSize.CENTER;
			returnPlayTxt.selectable = false;
			returnPlayTxt.mouseEnabled = false;
			returnPlayTxt.x = returnPlayBtn.x + 34;
			returnPlayTxt.y = returnPlayBtn.y + 3;
			returnPlayTxt.text = '返回播放';
			shareVideoMc.addChild(returnPlayTxt);
			
			//Tab口
			tabsp = new TabSharePicUI();
			tabsp.x = 5;
			tabsp.y = 5;
			tabsp.buttonMode = true;
			tabsp.mouseChildren = false;
			tabsp.gotoAndStop(2);
			shareVideoMc.addChild(tabsp);
			tabsp.addEventListener(MouseEvent.CLICK,tabspClickHandler);
			
			tabv = new TabVideoUI();
			tabv.x = tabsp.x + tabsp.width + 5;
			tabv.y = 5;
			tabv.buttonMode = true;
			tabv.mouseChildren = false;
			tabv.gotoAndStop(1);
			shareVideoMc.addChild(tabv);
			tabv.addEventListener(MouseEvent.CLICK,tabvClickHandler);
			
		} 
		private function onReplayHandler(e:MouseEvent):void {
//			this.visible = false;
//			this.dispatchEvent(new SkinEvent(SkinEvent.MEDIA_PLAY));
		}
		private function onCloseHandler(e:MouseEvent):void {
			this.visible = false;
		}
		/**
		 * 视频分享切换句柄
		 * @return 
		 * 
		 */		
		private function tabspClickHandler(e:MouseEvent):void {
			showPic();
		}
		/**
		 * 图片分享切换句柄
		 * @return 
		 * 
		 */	
		private function tabvClickHandler(e:MouseEvent):void {
			showVideo();
		}
		private function flashBtnHandler(e:MouseEvent):void{
			for each(var obj:RadioBtnUI in selectedList){
				obj.gotoAndStop(1);
			}
			e.currentTarget.gotoAndStop(2);
			showAddress("flashAddress",flashAddress);
		}
		private function videoBtnHandler(e:MouseEvent):void{
			for each(var obj:RadioBtnUI in selectedList){
				obj.gotoAndStop(1);
			}
			e.currentTarget.gotoAndStop(2);
			showAddress("videoAddress",videoAddress);
		}
		private function htmlBtnHandler(e:MouseEvent):void{
			for each(var obj:RadioBtnUI in selectedList){
				obj.gotoAndStop(1);
			}
			e.currentTarget.gotoAndStop(2);
			showAddress("htmlAddress",videoAddress);
		}
		private function saveBtnHandler(e:MouseEvent):void{
			if(picBitmap){
				var _fileRef:FileReference=new FileReference();//用于保存文件
				var _encoder:JPGEncoder = new JPGEncoder(100);//用于编码位图
				var ba:ByteArray = _encoder.encode(picBitmapData);
				var td:Date = new Date()
				var saveName:String = ''+td.getFullYear()+(td.getMonth()+1)+td.getDate()+td.getHours()+td.getMinutes()+td.getSeconds();
				_fileRef.save(ba,'pptv'+saveName+'.jpg');
				ba.clear();
			}
		}
		private function copyBtnHandler(e:MouseEvent):void{
			var tT:String = addressInput.text;
			System.setClipboard(tT);
			addressInput.text = '复制成功！请按CTRL+V复制给你的好友！';
			setTimeout(function(){
				addressInput.text = tT;
			},1500);
		}
		private function onShareFixHandler(e:MouseEvent):void {
//			CommonMethod.getURL(CommonMethod.curr_link);
		}
		
		private function onClickHandler(e:MouseEvent):void {
//			_share_mc["copyTip"].visible = true;
//			var str:String = "";
//			switch (e.currentTarget) {
//				case _share_mc["videoAddressCopy"]:
//					System.setClipboard(_share_mc["videoAddress"].text);
//					str = "视频地址已复制成功,您可以通过E-mail/MSN/QQ等通讯工具分享。";
//					break;
//				case _share_mc["flashAddressCopy"]:
//					System.setClipboard(_share_mc["flashAddress"].text);
//					str = "flash地址已复制成功，您可以通过支持插入flash的博客或论坛分享。";
//					break;
//				case _share_mc["htmlAddressCopy"]:
//					System.setClipboard(_share_mc["htmlAddress"].text);
//					str = "html代码已复制成功，您可以通过支持html编辑的网页、论坛或博客分享。";
//					break;
//			}
//			CommonMethod.skin.showError( { "title":str } );
		}
		public function openPanel(tabName:String):void{
			switch(tabName){
				case SHARE_VIDEO:
					showVideo();
					break;
				case SHARE_PIC:
					showPic();
					break;
			}
		}
		private function showVideo():void{
			for each(var obj1:* in videoTabCon){
				obj1.visible = true;
			}
			for each(var obj:* in picTabCon){
				obj.visible = false;
			}
			tabv.gotoAndStop(1);
			tabsp.gotoAndStop(2);
			shareText.x = 40;
			shareText.y = 120;
			returnPlayBtn.x = (shareVideoMc.width - returnPlayBtn.width) / 2;
			returnPlayBtn.y = shareText.y + 100;
			returnPlayTxt.x = returnPlayBtn.x + 12;
			returnPlayTxt.y = returnPlayBtn.y + 3;
		}
		private function showPic():void{
			for each(var obj1:* in picTabCon){
				obj1.visible = true;
			}
			for each(var obj:* in videoTabCon){
				obj.visible = false;
			}
			tabv.gotoAndStop(2);
			tabsp.gotoAndStop(1);
			returnPlayBtn.x = saveBtn.x + saveBtn.width + 80;
			returnPlayBtn.y = panelHeight - saveBtn.height - 10;
			returnPlayTxt.x = returnPlayBtn.x + 12;
			returnPlayTxt.y = returnPlayBtn.y + 3;
			shareText.x = 40;
			shareText.y = returnPlayBtn.y - 30;
		}
		public function showIcon(arr:Array):void {
//			for (var i:int = 0; i < arr.length; i++) {
//				var _mc_:MovieClip = new MovieClip();
//				_share_mc["icon_mc"].addChild(_mc_);
//				_mc_.x = i * 24;
//				_mc_._link_ = arr[i]["link"];
//				_mc_._name_ = arr[i]["name"];
//				var _loader:Loader = new Loader();
//				_mc_.addChild(_loader);
//				_loader.load(new URLRequest(arr[i]["icon"]));
//				_mc_.buttonMode = true;
//				_mc_.addEventListener(MouseEvent.CLICK, onLinkHandler);
//				_mc_.addEventListener(MouseEvent.MOUSE_OVER, onOverHandler);
//				_mc_.addEventListener(MouseEvent.MOUSE_OUT, onOutHandler);
//			}
		}
		
		private function onOverHandler(e:MouseEvent):void {
//			e.target["parent"].addChild(CommonMethod.tip);
//			CommonMethod.showTip(e.currentTarget._name_, this);
		}
		
		private function onOutHandler(e:MouseEvent):void {
//			CommonMethod.hideTip();
		}
		
		private function onLinkHandler(e:MouseEvent):void {
//			CommonMethod.getURL(e.currentTarget._link_.replace("[LINK]", CommonMethod.curr_link).replace("[TITLE]", CommonMethod.curr_title));
		}
		
		public function showAddress(addressType:String = '', flashAddre:String = ''):void {
			switch(addressType){
				case 'videoAddress' :
				case 'flashAddress' :
					addressInput.text = flashAddre;
					break;
				case 'htmlAddress' :
					addressInput.text = '<embed src="' + flashAddre + '" quality="high" width="480" height="390" align="middle" allowScriptAccess="always" allownetworking="all" allowfullscreen="true" type="application/x-shockwave-flash" wmode="window"></embed>';
					break;
			}
//			_share_mc["videoAddress"].text = videoAddre;
//			_share_mc["flashAddress"].text = flashAddre;
//			_share_mc["htmlAddress"].text = '<embed src="' + flashAddre + '" quality="high" width="480" height="390" align="middle" allowScriptAccess="always" allownetworking="all" allowfullscreen="true" type="application/x-shockwave-flash" wmode="window"></embed>';
		}
		
		public function showTitle(txt:String):void {
//			_share_mc["sharetitle"].text = txt;
		}
		
		public function showImage(bmpd:BitmapData):void {
			picBitmap = null;
			picBitmapData = null;
			picBitmapData = bmpd;
			picBitmap = new Bitmap(picBitmapData);
			var dw:Number = picBitmap.width;
			var dh:Number = picBitmap.height;
//			var vw:Number = 240 * dw / dh;
			var vh:Number = 300 * dh / dw;
			picBitmap.width = 300;
			picBitmap.height = vh;
//			picBitmap.width = vw;
//			picBitmap.height = 240;
			picMcCon.addChild(picBitmap);
			picMcCon.x = this.width / 2 - picBitmap.width / 2;
			if(picBitmap.height < 240 ){
				picMcCon.y = 120 - picBitmap.height / 2 + 38;
			} else {
				picMcCon.y = 35;
			}
			
//			var _loader:Loader = new Loader();
//			_loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onCompleteHandler);
//			_loader.load(new URLRequest(url));
		}
		
		private function onCompleteHandler(e:Event):void {
//			var _this_loader:Loader = e.target.loader as Loader;
//			_this_loader.width = 160;
//			_this_loader.height = 120;
//			_share_mc["sharefiximg"]["sharefiximgBg"].addChild(_this_loader);
		}
		
//		public function isVisible(bool:Boolean = true):void {		
////			this.visible = bool;
//			resize();
//		}
		
		public function resize():void {
			if (this.visible) {
				this.graphics.clear();
				this.graphics.beginFill(0x000000);
				this.graphics.drawRect(0, 32, this.stage.stageWidth, this.stage.stageHeight - 62);
				this.graphics.endFill();				
				_share_mc.x = (this.stage.stageWidth - _share_mc.width) / 2;
				_share_mc.y = 32 + (this.stage.stageHeight - 62 - _share_mc.height) / 2;
			} else {
				this.graphics.clear();
			}
		}
		
	}

}