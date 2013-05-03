package pptv.skin.view.ui {
	
	import flash.display.MovieClip;
	import flash.display.Shape;
	
	/**
	 * ...
	 * @author minliang1112@foxmail.com
	 */
	public class ControlBgMc extends MovieClip {
		
		private var _grid:Shape;
		
		public function ControlBgMc() {
			_grid = new Shape();
			this.addChild(_grid);
		}
		
		public function resize(w:Number, h:Number = 30):void {
			this.graphics.clear();
			this.graphics.beginFill(0x000000, 1);
			this.graphics.drawRect(0, 0, w, h);
			this.graphics.endFill();
			_grid.graphics.beginFill(0xFF0000, 1);
			_grid.graphics.drawRect(0, 0, w, h);
			
		}
		
	}

}

/*import flash.display.MovieClip;
var _home:MovieClip = new MovieClip();
this.addChild(_home);
var _mc:MovieClip = new MovieClip();
_home.addChild(_mc);
_mc.graphics.clear();
for (var i:int=0;i<30/2;i++){
	if (i%2==0){//trace(i);
		_mc.graphics.beginFill(0x000000,1);
	} else {
		_mc.graphics.beginFill(0xff0000,1);
	}
	_mc.graphics.drawRect(0,i*2,2,2);
}
_mc.graphics.endFill();
var _mc_:MovieClip = new MovieClip();
_home.addChild(_mc_);
_mc_.x = _mc.x+_mc.width;
_mc_.graphics.clear();
for (i=0;i<30/2;i++){
	if (i%2==0){//trace(i);
		_mc_.graphics.beginFill(0xff0000,1);
	} else {
		_mc_.graphics.beginFill(0x000000,1);
	}
	_mc_.graphics.drawRect(0,i*2,2,2);
}
_mc_.graphics.endFill();


var sp3:Sprite = new Sprite();
sp3.name = "容器";
sp3.graphics.beginFill(0x000000);

sp3.graphics.drawRect(0,0,400,30);

var bmpData:BitmapData = new BitmapData(_home.width,_home.height,true,0xffffffff);
bmpData.draw(_home);
sp3.graphics.beginBitmapFill(bmpData);

sp3.graphics.drawRect(sp3.x,sp3.y,sp3.width,sp3.height);

sp3.graphics.endFill();

addChild(sp3);
sp3.x = 50;
sp3.y = 50;
sp3.alpha = .2;*/