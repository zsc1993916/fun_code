package 
{
	import Map;
	public class Snake
	{
		private var head		:int;
		private var tail		:int;
		private var length		:int;
		private var setOfDir	:Vector.<int>;
		private var boxNum		:int;
		
		private var beDir		:int;
		
		/**
		 * @param iMap 	地图
		 * @param iHead 头位置
		 * */
		public function Snake(iMap:Map, iHead:int)
		{
			length = 1;
			head = tail = iHead;
			iMap.drawBox(iHead, 0);
			
			setOfDir = new Vector.<int>(4, false);
			boxNum = iMap.getBoxNum();
			setOfDir[0] = -boxNum;
			setOfDir[1] = boxNum;
			setOfDir[2] = -1;
			setOfDir[3] = 1;
		}
		
		/**
		 * @param iMap 		地图
		 * @param iDir 		方向
		 * @return boolean  移动是否合法
		 * */
		public function move(iMap:Map, iDir:int):int
		{
			var width:int = iMap.getBoxWidth();
			var TDir:int = iMap.getForceDir(head);
			if(iMap.fanxiang(beDir, iDir))
			{
				iDir = beDir;
			}
			var checkResult:int = iMap.checkIndex(head, iDir); 
			
			trace("checkResult:" + checkResult);
			
			if (checkResult == 0) 
			{
				return 4;   // 4代表游戏失败
			}
			
			if(TDir != 4 && !iMap.fanxiang(iDir,TDir)) 
			{
				 iDir = TDir;
			}
			var nextIndex:int = head + setOfDir[iDir];
			
			iMap.updateHead(head, nextIndex);
			head = nextIndex;
			
			beDir = iDir;
			
			if (checkResult == 2) 
			{
				length++;
				iMap.updateFood();
			}
			else tail = iMap.updateTail(tail); 
			return iDir;
		}
		
		public function getLength():int
		{
			return length;
		}
	}
}