package
{
	import flash.display.Bitmap;
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;

	public class Show extends Sprite
	{
		private var loader		:BulkLoader;
		private var statu		:int;
		private var drawOver	:Shape;
		private var writeOver	:TextField;
		private var writeScore	:TextField;
		private var button1		:MyButton;
		
		
		public function Show(iLoader : BulkLoader)
		{
			statu = 1;
			loader = iLoader;
		}
		public function OverWindow(len:int):void
		{
			/*
			drawOver = new Shape;
			drawOver.graphics.beginFill(0x99FFFF);
			drawOver.graphics.drawRect(stage.width/2, stage.height/2, 300, 200);
			drawOver.graphics.endFill();
			addChild(drawOver);*/
			
			if (writeOver && writeOver.parent){
				removeChild(writeOver);
			}
			
			if (writeScore && writeScore.parent){
				removeChild(writeScore);
			}
			var tFormat:TextFormat = new TextFormat();
			tFormat.color	= 0xFF0FFF;
			tFormat.font	= "Consolas";
			tFormat.size	= 55;		
			writeOver = new TextField();
			writeOver.defaultTextFormat = tFormat;
			writeOver.mouseEnabled = false;
			writeOver.text = "Game Over!";
			
			writeOver.width = writeOver.textWidth+5;
			writeOver.height = writeOver.textHeight+5;
			writeOver.x = (HeheSnake.STAGE.width + 50 - writeOver.width)/2;
			writeOver.y = (HeheSnake.STAGE.height + writeOver.height)/2 - 100;
			addChild(writeOver);
			
			var tFormat1:TextFormat = new TextFormat();
			tFormat1.color	= 0xFF0FFF;
			tFormat1.font	= "Consolas";
			tFormat1.size	= 40;	
			writeScore = new TextField;
			writeScore.defaultTextFormat = tFormat1;
			writeScore.text = "Score: " + len.toString();
			writeScore.width = writeScore.textWidth+5;
			writeScore.height = writeScore.textHeight+5;
			writeScore.x = (HeheSnake.STAGE.width + 50 - writeScore.width)/2;
			writeScore.y = (HeheSnake.STAGE.height + writeScore.height)/2;
			addChild(writeScore);
			
			/*var button1:MyButton = new MyButton(100,20,0xFF0FFF);
			button1.x = 500;
			button1.y = 500;
			button1.SetName("Restart!");
			button1.addEventListener(MouseEvent.CLICK, onButtonClick);
			addChild(button1);*/
		}
		/*public function onButtonClick(e:Event):void
		{		
			if(writeOver && writeOver.parent)
			{
				removeChild(writeOver);
			}
			if(writeScore && writeScore.parent)
			{
				removeChild(writeScore);
			}
			/*if(button1 && button1.parent)
			{
				removeChild(button1);
			}
		}*/
	}
}