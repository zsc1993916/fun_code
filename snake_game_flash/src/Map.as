package 
{
	//import com.hurlant.crypto.tls.BulkCiphers;
	
	import flash.display.Bitmap;
	import flash.display.Sprite;
	
	import Box;

	public class Map extends Sprite
	{	
		private var upLeftX			:int;
		private var upLeftY			:int;
		private var boxWidth		:int;
		private var boxNum			:int;
		
		// empty 0 蛇身， 1 空地， 2 食物 
		private var empty			:Vector.<int>;
		// 邻接表 
		private var nextPosition	:Vector.<int>;
		
		//强制方向   0~3方向， 4 无方向
		private var forceDir		:Vector.<int>;
		
		// 方格
		private var	boxMap			:Vector.<Box>;
		
		// 移动方向
		private var DirX			:Vector.<int>;
		private var DirY			:Vector.<int>;
		
		//资源加载
		private var loader			:BulkLoader;
		private var bm				:Vector.<Bitmap>;
		private var setOfDirUrl		:Vector.<String>;
		private var setOfDirIndex	:Vector.<int>;
		private var Difficulty		:int;
		/**
		 * @param uLX,uLY		  左上角坐标
		 * @param iNum,iWidth    方格数量，单个方格边长
		 * @param iDif			  方向格的个数（游戏难度）
		 * */
		public function Map(uLX:int, uLY:int, iNum:int, iWidth:int, iDif:int,  iLoader:BulkLoader)
		{
			upLeftX	 = uLX;
			upLeftY	 = uLY;
			boxNum	 = iNum;
			boxWidth = iWidth;
			Difficulty = iDif;
			loader = iLoader;
			graphics.lineStyle(2,0x000FF0);
			graphics.beginFill(0xFFFFFF);
			graphics.drawRect(upLeftX, upLeftY, boxNum*boxWidth, boxNum*boxWidth);
			graphics.endFill();
			
			initGeo();      //地图初始化
			
			var haha:int = 8;
			while(haha--)
			{
				updateFood(); //增加食物
			}
		    updateDirBox(); //增加iDif个方向盒子
		}
		
		// 地图初始化
		private function initGeo():void
		{
			empty 		 = new Vector.<int>(boxNum*boxNum);
			nextPosition = new Vector.<int>(boxNum*boxNum);
			boxMap 		 = new Vector.<Box>(boxNum*boxNum);
			bm 		     = new Vector.<Bitmap>(boxNum*boxNum);
			setOfDirIndex= new Vector.<int>(boxNum*boxNum);
			forceDir	 = new Vector.<int>(boxNum*boxNum);
			setOfDirUrl  = new Vector.<String>(4, false);
			DirX = new Vector.<int>(4, false);
			DirY = new Vector.<int>(4, false);
			DirX[0] = 0;  DirX[1] = 0;
			DirX[2] = -1;  DirX[3] = 1;
			DirY[0] = -1;  DirY[1] = 1;
			DirY[2] = 0;  DirY[3] = 0;
			setOfDirUrl[0] = "up.jpg";
			setOfDirUrl[1] = "down.jpg";
			setOfDirUrl[2] = "left.jpg";
			setOfDirUrl[3] = "right.jpg";
			
			for (var i:int = 0; i < boxNum*boxNum; i++) 
			{
				forceDir[i] = 4;
				empty[i] = 1;
				nextPosition[i] = i;
				var iX:int = (int)(i%boxNum);
				var iY:int = (int)(i/boxNum);
				boxMap[i] = new Box(upLeftX+iX*boxWidth, upLeftY+iY*boxWidth,  boxWidth,2);
				addChild(boxMap[i]);
				bm[i] = new Bitmap();
				bm[i].x = upLeftX + iX*boxWidth;
				bm[i].y = upLeftY + iY*boxWidth;
			}
			
		}
		
		/*
		* 随机产生 Difficult 个方向盒子
		*/
		public function updateDirBox():void
		{
			var dirBoxIndex:int = new int;
			for(var i:int = 0; i < Difficulty; i++) 
			{
				if(bm[setOfDirIndex[i]] && bm[setOfDirIndex[i]].parent)
				{
					forceDir[setOfDirIndex[i]] = 4;
					removeChild(bm[setOfDirIndex[i]]);
				}
			}
			initSetOfDirIndex();
			for(i = 0; i < Difficulty; i++)
			{
				var randNum:int = (int)(4 * Math.random());
				if(loader.getBitmap(setOfDirUrl[randNum]))
				{
					forceDir[setOfDirIndex[i]] = randNum;
					bm[setOfDirIndex[i]].bitmapData = loader.getBitmap(setOfDirUrl[randNum]).bitmapData;
				}
				else 
				{
					trace("loader fail!");
				}
				empty[setOfDirIndex[i]] = 3;
				addChild(bm[setOfDirIndex[i]]);
			}
		}
		
		//随机产生 Difficulty 个方向格子
		private function initSetOfDirIndex():void 
		{
			for(var i:int = 0; i < Difficulty; i++)
			{ 
				setOfDirIndex[i] = findEmptyBox();
			}
		}
		
		
		public function getBoxWidth():int
		{
			return boxWidth;
		}
		
		public function getBoxNum():int
		{
			return boxNum;
		}
		
		/**
		 * 判断下一个点是什么
		 * @param index 当前头位置
		 * @param dir   移动方向
		 * */
		
		public function getForceDir(index:int): int
		{
			return forceDir[index];
		}
		public function checkIndex(index:int, dir:int):int
		{
			var x:int = (int)(index % boxNum);
			var y:int = (int)(index / boxNum);
			if(forceDir[index] != 4)
			{
				if(fanxiang(dir, forceDir[index]))
				{
				}
				else dir = forceDir[index];
			}
			var nextX:int = x + DirX[dir];
			var nextY:int = y + DirY[dir];
			var nextIndex:int = nextX + nextY * boxNum;
			trace(x + " " + y + " " + nextX + " " + nextY + " " + nextIndex);
			if (nextX < 0 || nextX >= boxNum
				|| nextY < 0 || nextY >= boxNum)
			{
				return 0;
			}
			if(empty[nextIndex] == 2){
				if(bm[nextIndex] && bm[nextIndex].parent) {
					removeChild(bm[nextIndex]);
				}
			}
			return empty[nextIndex];
		}
		
		//判断反向
		public function fanxiang(dir1:int, dir2:int):Boolean
		{
			if(dir1 == 1 && dir2== 0) return 1;
			if(dir1 == 0 && dir2== 1) return 1;
			if(dir1 == 2 && dir2== 3) return 1;
			if(dir1 == 3 && dir2== 2) return 1;	
			return 0;
		}
		
		public function	updateHead(head:int, nextHead:int):void
		{
			nextPosition[head] = nextHead;
			empty[nextHead] = 0;
			drawBox(nextHead, 0);
		}
		
		public function updateTail(tail:int):int
		{
			empty[tail] = 1;
			drawBox(tail, 2);
			return nextPosition[tail];
		}
		
		public function updateFood():void
		{
			var food:int = findEmptyBox();
			drawBox(food, 1);
			if(loader.getBitmap("food.jpg"))
			{
				bm[food].bitmapData = loader.getBitmap("food.jpg").bitmapData;
			}
			addChild(bm[food]);
			empty[food] = 2;
		}
		
		
		
		
		/**
		 * 更新一个格子的颜色
		 * */
		public function drawBox(index:int, col:int):void
		{
			boxMap[index].changeColor(col);
			/*if(col == 0)
			{
				if(forceDir[index] != 4) 
				{
					if(bm[index] && bm[index].parent){
						removeChild(bm[index]);
					}
				}
			}*/
		}
		
		
		/**
		 * 返回一个空格子的位置
		 * */
		private function findEmptyBox():int
		{
			var res:int = new int;
			while(1) 
			{
				res = (int)(Math.random()*boxNum*boxNum);
				if (empty[res] == 1) 
				{
					break;
				}
			}
			return res;
		}
	}
}