/**
 * MyButton类
 * @author ZhengShuang Wang
 * Copyright (c) 2014 上海欢乐互娱网络科技有限公司
 */

package
{
	import flash.display.DisplayObject;
	import flash.display.Shape;
	import flash.display.SimpleButton;
	import flash.display.Sprite;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	public class MyButton extends Sprite
	{
		private var m_ttfrName:TextField;
		private var m_iWidth:int;
		private var m_iHeigth:int;
		
		public function MyButton(iWidth:int, iHeight:int, uiColor:uint)
		{
			m_iWidth 	= iWidth;
			m_iHeigth	= iHeight;
			
			var spUpState:Shape = new Shape();
			spUpState.graphics.beginFill(uiColor);
			spUpState.graphics.drawRoundRect(0, 0, iWidth, iHeight, iWidth / 2);
			spUpState.graphics.endFill();
			
			var spOverState:Shape = new Shape();
			spOverState.graphics.beginFill(0x003FFF);
			spOverState.graphics.drawRoundRect(0, 0, iWidth, iHeight, iWidth / 2);
			spOverState.graphics.endFill();
			
			var spDownState:Shape = new Shape();
			spDownState.graphics.beginFill(0x00ABFF);
			spDownState.graphics.drawRoundRect(0, 0, iWidth, iHeight, iWidth / 2);
			spDownState.graphics.endFill();
			
			var stButton:SimpleButton = new SimpleButton(spUpState, spOverState, spDownState, spOverState);

			addChild(stButton);

			var tFormat:TextFormat = new TextFormat();
			
			tFormat.color	= 0xFFFFFF;
			tFormat.font	= "Consolas";
			tFormat.size	= 17;
			
			m_ttfrName = new TextField();
			m_ttfrName.defaultTextFormat = tFormat;
			m_ttfrName.mouseEnabled = false;
			
			addChild(m_ttfrName);
			
			useHandCursor	= true;
			buttonMode		= true;
		}
		
		public function SetName(strName:String):void
		{
			m_ttfrName.text		= strName;
			m_ttfrName.width 	= m_ttfrName.textWidth + 5;
			m_ttfrName.height 	= m_ttfrName.textHeight + 5;
			
			m_ttfrName.x = (m_iWidth - m_ttfrName.width) / 2;
			m_ttfrName.y = (m_iHeigth - m_ttfrName.height) / 2;
		}
	}
}