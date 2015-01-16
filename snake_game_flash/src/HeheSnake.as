/**
 *  Stupid Snake
 * 	@author Zo Zhou 
 */
package
{
	import flash.display.Sprite;
	import flash.display.Stage;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.ui.Keyboard;
	
	import Map;
	
	import MyButton;
	
	import Show;
	
	import Snake;
	
	
	
	[SWF(width="800",height="800",frameRate="60")]
	public class HeheSnake extends Sprite
	{
		private const INITSPEED 	:Number = 0.20;
		private const START  		:int = 200;
		
		public static var STAGE		  :Stage;
		public static var PassTime	  :Number;
		public static var bePassTime  :Number;
		public static var SegTime	  :Number;
		
		
		public  var Accele		    :Number;
		private var refreshRate		:int;
		private var cntRefresh		:int;
		private var cntTime			:int;
		private var showWindow		:Show;
		private var loader			:BulkLoader;
		private var state			:int;					
		private var dir				:int;			
		private var stSnake			:Snake;
		private var stMap			:Map;
		private var Difficulty		:int;
		private var button1			:MyButton;
		private var button2			:MyButton;
		private var button3			:MyButton;
		
		public function HeheSnake()
		{
			state = 0;
			if (stage)
			{
				AddToStage(null);
			}
			else
			{
				addEventListener(Event.ADDED_TO_STAGE, AddToStage);
			}
		}	
		
		private function AddToStage(e:Event):void
		{
			STAGE = stage;
			LoadResourse();
		}  
		
		private function LoadResourse ():void 
		{
			loader = new BulkLoader("resourse");
			loader.add("up.jpg");
			loader.add("down.jpg");
			loader.add("left.jpg");
			loader.add("right.jpg");
			loader.add("food.jpg");
			loader.add("head.jpg");
			loader.addEventListener(BulkProgressEvent.COMPLETE, onBulkLoader);
			loader.start();
		}
		
		private function onBulkLoader(e:BulkProgressEvent):void
		{
			trace("Load resourse complete");
			/*
			难度选择 界面     暂空
			*/
			
			showWindow = new Show(loader); 
			
			GameStart();  //参数是难度
			
		}
		
		protected function GameStart():void
		{
			
			button1 = new MyButton(100,30,0xFF0FFF);
			addChild(button1);
			button1.x = 200;
			button1.y = 500;
			button1.SetName("娱乐版");
			button1.addEventListener(MouseEvent.CLICK, onButtonClick1)
			
			button2 = new MyButton(100,30,0xFF0FFF);
			addChild(button2);
			button2.x = 350;
			button2.y = 500;
			button2.SetName("虐心版");
			button2.addEventListener(MouseEvent.CLICK, onButtonClick2)
			
			button3 = new MyButton(100,30,0xFF0FFF);
			addChild(button3);
			button3.x = 500;
			button3.y = 500;
			button3.SetName("呵呵版");
			button3.addEventListener(MouseEvent.CLICK, onButtonClick3)
			
			cntRefresh = refreshRate;
			
			
			addEventListener(Event.ENTER_FRAME, OnEnterFrame);
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, OnKeyDown);
		}
		
		public function  onButtonClick1(e : MouseEvent):void
		{
			Difficulty = 40;
			refreshRate = 5;
			Accele = 0.004;
			CreatGame();
		}
		public function  onButtonClick2(e:MouseEvent):void
		{
			Difficulty = 80;
			refreshRate = 2;
			Accele = 0.006;
			CreatGame();
		}
		public function  onButtonClick3(e : MouseEvent):void
		{
			Difficulty = 300;
			refreshRate = 10000;
			Accele = 0.008;
			CreatGame();
		}
		
		private function CreatGame():void
		{
			PassTime = 0;
			bePassTime = 0;
			SegTime = INITSPEED;
			cntTime = 1;
			cntRefresh = refreshRate;
			stMap = new Map(50,50,35,20,Difficulty,loader);
			var randHead:int = (int)(Math.random()*35*35);
			stSnake = new Snake(stMap, randHead);
			addChild(stMap);
			addChild(showWindow);
			showWindow.visible = false;
		}
		
		protected function OnKeyDown(e:KeyboardEvent):void
		{
			state = 1;
			switch(e.keyCode)
			{
				case Keyboard.UP:
				{
					dir = 0;
					trace("up");
					break;
				}
				case Keyboard.DOWN:
				{
					dir = 1;
					trace("down");
					break;
				}
				case Keyboard.LEFT:
				{
					dir = 2;
					trace("left");
					break;
				}
				case Keyboard.RIGHT:
				{
					dir = 3;
					trace("right");
					break;
				}
					
				default:
					return ;
			}
		}
		
		protected function OnEnterFrame(e:Event):void
		{
			PassTime += 1/STAGE.frameRate;
			if(PassTime > cntTime)
			{
				if(SegTime >= 0.05) SegTime -= Accele;
				cntTime += 1;
				cntRefresh --;
				if(!cntRefresh)
				{
					stMap.updateDirBox();
					cntRefresh = refreshRate;
				}
			}
			if (state == 1 && PassTime - bePassTime >= SegTime)
			{
				bePassTime = PassTime;
				var result:int = stSnake.move(stMap, dir); 
				if(result == 4)
				{
					state = 2;
				}
				else dir = result;
			}
			
			if(state == 2)
			{
				state = 0;
				showWindow.OverWindow(stSnake.getLength());
				showWindow.visible = true;
				trace ("remove");
				GameStart();
			}
		}
	}
}