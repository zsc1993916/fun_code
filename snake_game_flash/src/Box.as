package
{
	import flash.display.Shape;
	
	public class Box extends Shape
	{
		private var x			:int;
		private var y			:int;
		private var width		:int;
		private var colorIndex	:int;
		private var setOfColor	:Vector.<int>;
		/**
		 *  @param ix,iy 	坐标
		 * 	@param iWidth	边长
		 * 	@param iColor 	颜色
		 * */ 
		public function Box(ix:int, iy:int, iWidth:int, iColor:int)
		{
			x = ix;
			y = iy;
			width = iWidth;
			initColor();
			changeColor(iColor);
		}
		
		private function initColor():void
		{
			setOfColor = new Vector.<int>(3,false);
			setOfColor[0] = (0x0FF000);
			setOfColor[1] = (0xFF0000);
			setOfColor[2] = (0x000000);
		}
		
		
		/**
		 *  @param iColor  0-蛇 , 1-空地 ,2-食物。
		 * */ 
		public function changeColor(iColor:int):void
		{
			colorIndex = iColor;
			graphics.beginFill(setOfColor[colorIndex]);
			graphics.drawRect(x, y, width, width);
			graphics.endFill();
			
		}
		
	}
}


