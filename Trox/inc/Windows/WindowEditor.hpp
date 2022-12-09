#pragma once
#include "Framework.hpp"

namespace windows {
	
	using namespace winapi::window;
	
	struct Editor {
		const uint64 positionX;
		const uint64 positionY;
		const uint64 sizeX;
		const uint64 sizeY;
	};
	
	block PaintBorder(
		const winapi::displayContextHandle& displayContext,
		const Editor& editor,
		const uint64& thickness
	) {
		const uint64& offset = ceil(thickness / 2.0f);
		HPEN pen;
		HBRUSH brush;
		HGDIOBJ oldP, oldB;
		
		pen = CreatePen(PS_SOLID, thickness, RGB(255, 0, 0));
		brush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
		oldP = SelectObject(displayContext, pen);
		oldB = SelectObject(displayContext, brush);
		Rectangle(displayContext, editor.positionX - offset, editor.positionY - offset, editor.sizeX + 11 + offset, editor.sizeY + 11 + offset);
		SelectObject(displayContext, oldP);
		SelectObject(displayContext, oldB);
		DeleteObject(pen);
	}

	block CreateEditor(
		const winapi::handleInstance& process, 
		const winapi::windowHandle& window, 
		const Editor& editor
	) {
		const array<winapi::wchar, 10> windowDefaultText { L"Type here" };
		
		{
			winapi::windowHandle richText (
				CreateWindowExW(
					WS_EX_LEFT, 
					MSFTEDIT_CLASS, 
					windowDefaultText.Pointer(),
					WS_VISIBLE | WS_CHILD | WS_TABSTOP, 
					editor.positionX,
					editor.positionY,
					editor.sizeX,
					editor.sizeY,
					window, 
					NULL, 
					process, 
					NULL
				)
			);
			
			/// Background color change.
			SendMessage(richText, EM_SETBKGNDCOLOR, 0, brushes::colors->second); 
			
			{
				CHARFORMAT2 colorMap1 {{ 0 }};
				colorMap1.cbSize = sizeof ( CHARFORMAT2 ); 			/// Indicate which version of the structure is being used.
				colorMap1.dwMask = CFM_BACKCOLOR | CFM_COLOR; 		/// Text background color and text color
				colorMap1.crBackColor = brushes::colors->second;
				colorMap1.crTextColor = brushes::colors->text;
				
				SendMessage(richText, EM_SETCHARFORMAT, 0, (LPARAM)&colorMap1);	// Changing colors apply to all the text.
			}
			
			/// Funny it doesn't change anything...
			//{
			//	PARAFORMAT2 para;
			//	para.cbSize = sizeof(PARAFORMAT2);
			//	para.dwMask = PFM_SPACEBEFORE; // Of course, you can OR in other bits/options to set!
			//	para.dySpaceBefore = 0;
			//	SendMessage(richText, EM_SETPARAFORMAT, 0, (LPARAM)&para);
			//}
		}
	}
	
}

/* ABOUT
// It appears that there is a better way then using RICHEDIT
//  https://stackoverflow.com/questions/22109514/richedit-syntax-highligh
//  however for now lets stick to it. And if i will be in need i might use
//  things like "ExtTextOut" the other day.
// https://docs.microsoft.com/en-us/windows/win32/controls/rich-edit-controls
// https://www.codeproject.com/articles/5594/a-chat-control-based-on-the-rich-edit-control
// https://www.codeproject.com/articles/1780/painless-streaming-of-rich-text-to-from-cricheditc
// https://docs.microsoft.com/en-us/windows/win32/api/richedit/nc-richedit-editstreamcallback
// https://docs.microsoft.com/en-us/windows/win32/api/richedit/ns-richedit-editstream
// https://stackoverflow.com/questions/8024157/is-it-possible-to-highlight-text-in-a-win32-edit-control
// https://stackoverflow.com/questions/14526097/best-algorithm-for-syntax-highlight-winapi
// https://docs.microsoft.com/en-us/windows/win32/controls/em-setcharformat
*/

/*  EM_SETBKGNDCOLOR - for inner richText edit
proceeded stdcall EditorProcedure(
	windowHandle window,
	uint32 message,
	messageW wArgument,
	messageL lArgument,
	UINT_PTR uid, DWORD_PTR RefData
) {
	// X switch (message) {
	// X	 case EM_SETBKGNDCOLOR: {
	// X	 	//MessageBoxEx(window, L"RICHEDIT", L"PaintCall", MB_OK, 0);
	// X	 	displayContextHandle displayContext = (displayContextHandle)wArgument;
	// X	 	SetTextColor(displayContext, (*themes::colorPalette).textPrimary);
	// X	 	SetBkColor(displayContext, (*themes::colorPalette).backgroundPrimary);
	// X	 	return ((INT_PTR)(themes::backgroundPrimary.Get()));
	// X	 }
	// X }
	return (proceeded)DefSubclassProc(window, (uint32)message, wArgument, lArgument);
}
*/